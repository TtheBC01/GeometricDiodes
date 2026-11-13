// Stage B2 driver: the ballistic funnel (or the straight control) under a
// static or an ac imposed potential, uniform or localised at the neck, with
// the two-sided equilibrium estimator. One CSV row per invocation (appended),
// optional exit-energy histogram.
//
// usage: gd_bthz --geo funnel_nguyenD|straight_control|funnel|symmetric --field uniform|local
//                [--Wc --Lc --alpha --Wn --Wd --Ld nm/deg] [--scale s] [--label name]
//                (Stage B7: `funnel` takes its dimensions from the flags, default Device D;
//                 `symmetric` is the mirror-symmetric constriction with the same Wc, Lc, alpha, Wn;
//                 --scale multiplies every length of the shape, not ellF or lambda)
//                [--ellF nm] [--lambda nm|inf] [--p spec] --f Hz --V volts
//                [--N per side] [--cpp chords per period] [--seed s]
//                [--out csv] [--hist path]
// f = 0 runs the static case (V is the dc bias) with both estimators; f > 0
// runs the ac case (V is the amplitude Va).
#include "generators.hpp"
#include "transport.hpp"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>

using namespace gd;

int main(int argc, char** argv) {
  std::string geo = "funnel_nguyenD", field = "uniform", out = "results/bthz/points.csv", hist, thist;
  double ellF = 500, lambda = std::numeric_limits<double>::infinity(), spec = 1.0, f = 0, V = 0.1;
  long N = 1000000; int cpp = 40; uint64_t seed = 101; bool reflect = false;
  FunnelParams fq; double scale = 1.0; std::string label;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    auto next = [&]() -> std::string { if (i + 1 >= argc) { std::fprintf(stderr, "missing value for %s\n", a.c_str()); std::exit(2); } return argv[++i]; };
    if (a == "--geo") geo = next();
    else if (a == "--field") field = next();
    else if (a == "--ellF") ellF = std::atof(next().c_str());
    else if (a == "--lambda") { std::string s = next(); lambda = s == "inf" ? std::numeric_limits<double>::infinity() : std::atof(s.c_str()); }
    else if (a == "--p") spec = std::atof(next().c_str());
    else if (a == "--f") f = std::atof(next().c_str());
    else if (a == "--V") V = std::atof(next().c_str());
    else if (a == "--N") N = std::atol(next().c_str());
    else if (a == "--cpp") cpp = std::atoi(next().c_str());
    else if (a == "--seed") seed = std::strtoull(next().c_str(), nullptr, 10);
    else if (a == "--out") out = next();
    else if (a == "--hist") hist = next();
    else if (a == "--thist") thist = next();
    else if (a == "--reflect") reflect = true;
    else if (a == "--Wc") fq.Wc = std::atof(next().c_str());
    else if (a == "--Lc") fq.Lc = std::atof(next().c_str());
    else if (a == "--alpha") fq.alpha_deg = std::atof(next().c_str());
    else if (a == "--Wn") fq.Wn = std::atof(next().c_str());
    else if (a == "--Wd") fq.Wd = std::atof(next().c_str());
    else if (a == "--Ld") fq.Ld = std::atof(next().c_str());
    else if (a == "--scale") scale = std::atof(next().c_str());
    else if (a == "--label") label = next();
    else { std::fprintf(stderr, "unknown option %s\n", a.c_str()); return 2; }
  }
  const double n_cm2 = 1.4e12;
  Boundary b; double xF;
  if (geo == "funnel_nguyenD") {
    FunnelParams fp; b = make_funnel(fp);
    xF = fp.Lc + 0.5 * (fp.Wc - fp.Wn) / std::tan(deg2rad(fp.alpha_deg));  // neck plane, 3580.5 nm
  } else if (geo == "funnel" || geo == "symmetric") {
    fq.Wc *= scale; fq.Lc *= scale; fq.Wn *= scale; fq.Wd *= scale; fq.Ld *= scale;
    double Lt = 0.5 * (fq.Wc - fq.Wn) / std::tan(deg2rad(fq.alpha_deg));
    if (geo == "funnel") b = make_funnel(fq);
    else b = make_symmetric_constriction(fq.Wc, fq.Lc, fq.alpha_deg, fq.Wn, 0);
    xF = fq.Lc + Lt;   // neck plane (the mirror plane of the symmetric control)
  } else if (geo == "straight_control") {
    b = make_stepped_channel(2000, 1000, 175, 5490, 2000, 1000);
    xF = 0.5 * b.bbox_max().x;  // device midpoint: keeps the control mirror symmetric
  } else { std::fprintf(stderr, "unknown geometry %s\n", geo.c_str()); return 2; }

  TransportParams p;
  p.EF = EF_from_density(n_cm2); p.specularity = spec; p.lambda_mfp = lambda;
  p.field = field == "local" ? FieldModel::Localised : FieldModel::Uniform;
  p.ell_F = ellF; p.x_F = xF; p.chords_per_period = cpp; p.dirac_reflect = reflect;
  if (f > 0) { p.f_Hz = f; p.Va = V; } else { p.V = V; }
  Transport tr(b, p);

  auto t0 = std::chrono::steady_clock::now();
  Transport::AC ac = tr.ac_current(N, seed);
  double Ibw = NAN, Ibw_err = NAN;
  if (f <= 0 && V != 0) { auto iv = tr.current(N, seed + 31); Ibw = iv.I; Ibw_err = iv.I_err; }
  double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();

  // responsivity against the available power of a 75 ohm antenna, the
  // device's own first-harmonic admittance setting the mismatch
  const double Rant = 75.0;
  double RI = NAN, Pav = NAN;
  if (f > 0) {
    double Vant = V * std::abs(1.0 + Rant * ac.Y1);
    Pav = Vant * Vant / (8 * Rant);
    RI = ac.Idc / Pav;
  }
  auto frac = [](const RunStats& s, Fate fa) { return s.fraction(fa); };
  double f_dirac = 0.5 * (frac(ac.S, Fate::Dirac) + frac(ac.D, Fate::Dirac));
  double f_trap = 0.5 * (frac(ac.S, Fate::Trapped) + frac(ac.S, Fate::Leaked) + frac(ac.D, Fate::Trapped) + frac(ac.D, Fate::Leaked));
  double f_heat = (ac.S.sum_heated + ac.D.sum_heated) / std::max(1L, ac.S.n + ac.D.n);
  double Ekmax = std::max(ac.S.Ek_exit_max, ac.D.Ek_exit_max);
  double mt = (ac.S.sum_time + ac.D.sum_time) / std::max(1L, ac.S.n + ac.D.n);
  double ntx = ac.S.fate_count[(int)Fate::Drain] + ac.D.fate_count[(int)Fate::Source];
  double mtx = ntx > 0 ? (ac.S.sum_time_tx + ac.D.sum_time_tx) / ntx : 0;

  bool fresh = true;
  { std::ifstream in(out); fresh = !in.good() || in.peek() == std::ifstream::traits_type::eof(); }
  std::ofstream csv(out, std::ios::app);
  if (fresh)
    csv << "geo,field,ellF_nm,xF_nm,lambda_nm,p,f_Hz,V_V,N_side,Idc_A,Idc_err_A,Ibw_A,Ibw_err_A,ReY1_S,ImY1_S,I1_err_A,"
           "T_SD,T_DS,Phi_S_eV,Phi_D_eV,f_dirac,f_trapped,f_heated,Ek_exit_max_eV,mean_time_fs,mean_time_tx_fs,"
           "Pav_W,RI_A_per_W,cpp,seconds,reflect,L_nm,Wc_nm,Wn_nm,shape\n";
  csv.precision(8);
  csv << (label.empty() ? geo : label) << "," << field << "," << ellF << "," << xF << "," << lambda << "," << spec << "," << f << "," << V << "," << N << ","
      << ac.Idc << "," << ac.Idc_err << "," << Ibw << "," << Ibw_err << "," << ac.Y1.real() << "," << ac.Y1.imag() << "," << ac.I1_err << ","
      << ac.T_SD << "," << ac.T_DS << "," << ac.Phi_S << "," << ac.Phi_D << "," << f_dirac << "," << f_trap << "," << f_heat << ","
      << Ekmax << "," << mt << "," << mtx << "," << Pav << "," << RI << "," << cpp << "," << secs << "," << (reflect ? 1 : 0) << ","
      << b.bbox_max().x << "," << fq.Wc << "," << fq.Wn << "," << geo << "\n";

  if (!hist.empty()) {
    std::ofstream h(hist);
    h << "Ek_lo_eV,Ek_hi_eV,count_S,count_D\n";
    const auto& HS = ac.S.Ek_exit_hist; const auto& HD = ac.D.Ek_exit_hist;
    int nb = (int)HS.counts.size() - 2;
    for (int i = 0; i < nb + 2; ++i) {
      double lo = i == 0 ? 0 : HS.lo * std::pow(HS.hi / HS.lo, (double)(i - 1) / nb);
      double hi = i == nb + 1 ? INFINITY : HS.lo * std::pow(HS.hi / HS.lo, (double)i / nb);
      h << lo << "," << hi << "," << HS.counts[i] << "," << HD.counts[i] << "\n";
    }
  }

  if (!thist.empty()) {
    std::ofstream h(thist);
    h << "t_lo_fs,t_hi_fs,all_S,all_D,tx_S,tx_D\n";
    const auto& A = ac.S.time_hist;
    int nb = (int)A.counts.size() - 2;
    for (int i = 0; i < nb + 2; ++i) {
      double lo = i == 0 ? 0 : A.lo * std::pow(A.hi / A.lo, (double)(i - 1) / nb);
      double hi = i == nb + 1 ? INFINITY : A.lo * std::pow(A.hi / A.lo, (double)i / nb);
      h << lo << "," << hi << "," << ac.S.time_hist.counts[i] << "," << ac.D.time_hist.counts[i] << ","
        << ac.S.time_tx_hist.counts[i] << "," << ac.D.time_tx_hist.counts[i] << "\n";
    }
  }

  std::printf("%-16s %-7s%s ellF %5.0f lambda %5s p %.1f f %9.3g V %5.3f N %ld | Idc %9.4f +- %.4f uA", (label.empty() ? geo : label).c_str(), field.c_str(), reflect ? "*" : "", ellF,
              std::isinf(lambda) ? "inf" : std::to_string((int)lambda).c_str(), spec, f, V, N, ac.Idc * 1e6, ac.Idc_err * 1e6);
  if (f <= 0 && V != 0) std::printf(" | bias-window %9.4f +- %.4f uA", Ibw * 1e6, Ibw_err * 1e6);
  if (f > 0) std::printf(" | Y1 %.4f%+.4fi mS | R_I %.3f A/W (P_av %.1f uW)", ac.Y1.real() * 1e3, ac.Y1.imag() * 1e3, RI, Pav * 1e6);
  std::printf(" | T_SD %.4f T_DS %.4f dirac %.1e trap %.1e heated %.1e Ekmax %.3f | %.1f s\n", ac.T_SD, ac.T_DS, f_dirac, f_trap, f_heat, Ekmax, secs);
  return 0;
}
