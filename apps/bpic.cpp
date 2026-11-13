// Stage B8 driver: self-consistent particle simulation of one device at one
// operating point (src/pic.*; plan plans/ballistic_selfconsistent.md).
//
//   gd_bpic --geo funnel|symmetric|strip [shape flags] [--label s]
//           [--d nm] [--eps e] [--mode sym|gated|gates] [--V volts] [--f Hz]
//           [--lambda nm|inf] [--p specularity]
//           [--h nm] [--N particles] [--dt fs] [--filter cells] [--clen nm] [--cblock n]
//           [--settle fs] [--period fs] [--nper n] [--ramp fs]
//           [--noint] [--imposed 0|1|2] [--ellF nm]
//           [--seed n] [--threads n] [--out file.csv] [--map prefix]
//
// Shape flags: funnel and symmetric take --Wc --Lc --alpha --Wn --Wd --Ld
// (defaults: the 0.55 um funnel G3 of Stage B7); strip takes --L --W.
// --mode: sym    U_S = +V/2, U_D = -V/2 (shape-only drive)
//         gated  U_S = +V,   U_D = 0    (drain tied to the gate)
//         gates  U_S = 0,    U_D = -V   (source tied to the gate)
// With --f > 0 the drive is V cos(2 pi f t) and --period defaults to 1/f;
// otherwise the drive is static and the averaging window is cut into
// "periods" of --period fs (default 1000) for the batch error.
#include "generators.hpp"
#include "pic.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace gd;

int main(int argc, char** argv) {
  std::string geo = "funnel", label, out, map, mode = "sym";
  FunnelParams fq; fq.Wc = 500; fq.Lc = 100; fq.alpha_deg = 30; fq.Wn = 100; fq.Wd = 500; fq.Ld = 100;
  double Ls = 546.41, Ws = 500;
  PicParams p;
  double V = 0, f = 0, settle = -1, period = -1, ramp = -1;
  int nper = 10;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    auto next = [&]() -> std::string { if (i + 1 >= argc) { std::fprintf(stderr, "missing value for %s\n", a.c_str()); std::exit(2); } return argv[++i]; };
    if (a == "--geo") geo = next();
    else if (a == "--label") label = next();
    else if (a == "--Wc") fq.Wc = std::stod(next());
    else if (a == "--Lc") fq.Lc = std::stod(next());
    else if (a == "--alpha") fq.alpha_deg = std::stod(next());
    else if (a == "--Wn") fq.Wn = std::stod(next());
    else if (a == "--Wd") fq.Wd = std::stod(next());
    else if (a == "--Ld") fq.Ld = std::stod(next());
    else if (a == "--L") Ls = std::stod(next());
    else if (a == "--W") Ws = std::stod(next());
    else if (a == "--d") p.d_gate = std::stod(next());
    else if (a == "--eps") p.eps_r = std::stod(next());
    else if (a == "--mode") mode = next();
    else if (a == "--V") V = std::stod(next());
    else if (a == "--f") f = std::stod(next());
    else if (a == "--lambda") { std::string v = next(); p.lambda_mfp = v == "inf" ? std::numeric_limits<double>::infinity() : std::stod(v); }
    else if (a == "--p") p.specularity = std::stod(next());
    else if (a == "--h") p.h = std::stod(next());
    else if (a == "--N") p.N = std::stol(next());
    else if (a == "--dt") p.dt = std::stod(next());
    else if (a == "--filter") p.filter = std::stod(next());
    else if (a == "--clen") p.contact_len = std::stod(next());
    else if (a == "--cblock") p.contact_block = std::stoi(next());
    else if (a == "--settle") settle = std::stod(next());
    else if (a == "--period") period = std::stod(next());
    else if (a == "--nper") nper = std::stoi(next());
    else if (a == "--ramp") ramp = std::stod(next());
    else if (a == "--noint") p.interaction = false;
    else if (a == "--imposed") p.imposed = std::stoi(next());
    else if (a == "--ellF") p.ell_F = std::stod(next());
    else if (a == "--seed") p.seed = std::stoull(next());
    else if (a == "--threads") p.threads = std::stoi(next());
    else if (a == "--out") out = next();
    else if (a == "--map") map = next();
    else { std::fprintf(stderr, "unknown argument %s\n", a.c_str()); return 2; }
  }
  Boundary b;
  double Lt = 0.5 * (fq.Wc - fq.Wn) / std::tan(deg2rad(fq.alpha_deg));
  if (geo == "funnel") { b = make_funnel(fq); p.x_F = fq.Lc + Lt; }
  else if (geo == "symmetric") { b = make_symmetric_constriction(fq.Wc, fq.Lc, fq.alpha_deg, fq.Wn, 0); p.x_F = fq.Lc + Lt; }
  else if (geo == "strip") b = make_straight_channel(Ls, Ws);
  else { std::fprintf(stderr, "unknown --geo %s\n", geo.c_str()); return 2; }
  if (label.empty()) label = geo;
  p.mode = mode == "sym" ? DriveMode::Sym : mode == "gated" ? DriveMode::GateDrain : DriveMode::GateSource;
  if (mode != "sym" && mode != "gated" && mode != "gates") { std::fprintf(stderr, "unknown --mode %s\n", mode.c_str()); return 2; }
  if (f > 0) { p.f_Hz = f; p.Va = V; if (period < 0) period = 1e15 / f; } else { p.V = V; if (period < 0) period = 1000; }
  if (ramp < 0) ramp = f > 0 ? std::min(1000.0, period) : 300;
  p.ramp_fs = ramp;
  if (settle < 0) settle = std::max(3000.0, f > 0 ? 2 * period : 0.0);
  if (f > 0) settle = std::ceil(settle / period - 1e-9) * period;   // a whole number of periods, so that the window starts at phase zero

  if (f > 0 && p.dt <= 0) {   // a whole number of steps per period keeps the harmonic analysis clean
    double dt0 = 0.5 * p.h / kVF;
    p.dt = period / std::ceil(period / dt0 - 1e-9);
  }
  auto c0 = std::chrono::steady_clock::now();
  Pic pic(b, p);
  double Ek0 = 0;
  { pic.step(); Ek0 = pic.history().back().Ek; }
  while (pic.time() < settle - 1e-9) pic.step();
  double t0 = pic.time();
  if (!map.empty()) pic.start_average();
  while (pic.time() < t0 + nper * period - 1e-9) pic.step();
  PicResult r = pic.analyse(t0, period, nper);
  double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - c0).count();

  const double Rant = 75.0;
  double Rabs = r.Pabs != 0 ? r.Idc / r.Pabs : 0;
  double Pav = f > 0 ? std::norm(V * (1.0 + Rant * r.Y1)) / (8 * Rant) : 0;
  double R1 = Pav > 0 ? r.Idc / Pav : 0;
  double qS = 0, qD = 0; long nrec = 0;
  for (const PicStep& s : pic.history()) if (s.t > t0) { qS += s.qS; qD += s.qD; ++nrec; }
  if (nrec) { qS /= nrec; qD /= nrec; }

  std::printf("%s %s mode %s d %g nm lambda %g nm f %.3g Hz V %g: I_dc %.4f +- %.4f uA (S %.4f, D %.4f, vol %.4f)\n", label.c_str(), geo.c_str(), mode.c_str(),
              p.d_gate, p.lambda_mfp, f, V, r.Idc * 1e6, r.Idc_err * 1e6, r.IdcS * 1e6, r.IdcD * 1e6, r.Idc_vol * 1e6);
  std::printf("  Y1 %.5f %+.5fi mS, P_abs %.5f uW, R_abs %.4f A/W, R_1 %.4f A/W; E_kin %.5f -> %.5f eV; particles %.0f (weight %.5f e), leaked %ld\n",
              r.Y1.real() * 1e3, r.Y1.imag() * 1e3, r.Pabs * 1e6, Rabs, R1, Ek0, r.Ek_last, r.np_mean, pic.weight(), pic.leaked());
  std::printf("  grid %d x %d at %g nm, dt %g fs, %zu steps, %.1f s (push %.1f, field %.1f)\n", pic.nx(), pic.ny(), p.h, pic.dt(), pic.history().size() - 1, secs,
              pic.seconds_push(), pic.seconds_field());

  if (!out.empty()) {
    bool fresh = !std::ifstream(out).good();
    FILE* fp = std::fopen(out.c_str(), "a");
    if (!fp) { std::fprintf(stderr, "cannot open %s\n", out.c_str()); return 1; }
    if (fresh)
      std::fprintf(fp, "label,geo,mode,interaction,imposed,d_nm,eps,lambda_nm,p,f_Hz,V_V,h_nm,N,dt_fs,filter,clen_nm,cblock,settle_fs,period_fs,nper,"
                       "Idc_A,Idc_err_A,IdcS_A,IdcD_A,Idcvol_A,ReY1_S,ImY1_S,Pabs_W,Rabs_A_per_W,R1_A_per_W,Ek0_eV,Ek_first_eV,Ek_last_eV,"
                       "np_mean,weight,qS,qD,Qg,charge_err,leaked,L_nm,area_nm2,neq_nm2,seed,seconds\n");
    std::fprintf(fp, "%s,%s,%s,%d,%d,%g,%g,%g,%g,%.6g,%g,%g,%ld,%g,%g,%g,%d,%g,%g,%d,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.3g,%ld,%.8g,%.8g,%.8g,%llu,%.1f\n",
                 label.c_str(), geo.c_str(), mode.c_str(), (int)p.interaction, p.imposed, p.d_gate, p.eps_r, p.lambda_mfp, p.specularity, f, V, p.h, p.N, pic.dt(), p.filter,
                 p.contact_len, p.contact_block, settle, period, nper, r.Idc, r.Idc_err, r.IdcS, r.IdcD, r.Idc_vol, r.Y1.real(), r.Y1.imag(), r.Pabs, Rabs, R1, Ek0,
                 r.Ek_first, r.Ek_last, r.np_mean, pic.weight(), qS, qD, r.Qg_mean, r.charge_error, pic.leaked(), pic.length(), pic.area(), pic.n_eq(),
                 (unsigned long long)p.seed, secs);
    std::fclose(fp);
  }
  if (!map.empty() && pic.averaged_steps() > 0) {
    // one line per node: x y U_mean U_cos U_sin rho_mean (eV, electrons per node); harmonics are 2 <U cos>, 2 <U sin>
    FILE* fp = std::fopen((map + ".txt").c_str(), "w");
    if (fp) {
      std::fprintf(fp, "# nx %d ny %d h %g steps %d\n# x_nm y_nm U_mean_eV U_cos_eV U_sin_eV rho_mean_e\n", pic.nx(), pic.ny(), p.h, pic.averaged_steps());
      double n = pic.averaged_steps();
      for (int j = 0; j < pic.ny(); ++j)
        for (int i = 0; i < pic.nx(); ++i) {
          long k = (long)j * pic.nx() + i; Vec2 q = pic.node(i, j);
          std::fprintf(fp, "%.2f %.2f %.6g %.6g %.6g %.6g\n", q.x, q.y, pic.U_mean()[k] / n, 2 * pic.U_cos()[k] / n, 2 * pic.U_sin()[k] / n, pic.rho_mean()[k] / n);
        }
      std::fclose(fp);
    }
  }
  return 0;
}
