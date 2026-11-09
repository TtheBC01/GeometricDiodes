// Stage H2 science runs (plans/hydro_thz.md, sec. 4): second-order rectified
// response of a device under an antenna drive, swept in frequency. Stage H4
// (plans/hydro_density.md) adds the operating-point sweep: contact resistance,
// matched antenna, scaled scattering times, validity flags and the derived
// detector and rectenna figures (NEP, eta/P_av, P_knee, eta_peak).
//
// usage: gd_thz_sweep --geo funnel [--set 100K|300K] [--term sym|dss|dsd]
//                     [--f 1e10,...] [--nf 25 --fmin 1e10 --fmax 3e12]
//                     [--Rant 75 | --match 1] [--Rc 0 (ohm um)] [--slip inf] [--hw 20 --hm 120] [--out file.csv]
//                     [--n 1e12 --d 30e-9 --eps 3.8] [--nu x --tau x --tauee x] [--tauscale x|sqrtn] [--T K]
// geometries: chan (uniform 1 um x 10 um channel), funnel (175 nm neck, 2 um
// leads 6 um long), funnel_short (same with 1 um leads), funnel_w1000 (1 um neck, 4 um leads), tesla_classic, tesla_classic_w1000.
// terminations: ds = ideal ac source at the source, drain open (Dyakonov-Shur
// detector, readout = drain dc voltage); sym = antenna split +-Va/2 with R/2 at each contact (gate at
// the midpoint potential: geometric rectification only); dss = Thevenin at the
// source, drain grounded to the gate (Dyakonov-Shur configuration); dsd = the
// mirror image (source grounded, Thevenin at the drain).
// contact resistance (sec. 2 of the H4 plan): R_c in ohm um over the contact width
// W_ref, in series with every non-open port, ac and dc. The dc Thevenin resistance
// is then R_dc + 2 R_c. At ac the drain current is not -I_s (the gate carries
// displacement current), so the impedance the antenna sees is reported separately
// as Z_ant = (Va - R_ant I_s) / I_s (sym: the differential loop impedance).
// --match 1: a second pass per frequency with R_ant = Re Z_ant of the first.
// --tauscale: multiplies tau_mr and tau_ee (sqrtn: by sqrt(n / 1e12), constant mobility).
#include "generators.hpp"
#include "thz_geo.hpp"
#include "hydro/mesh.hpp"
#include "hydro/thz_solver.hpp"
#include "hydro/thz_units.hpp"
#include "hydro/units.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace gd;
using namespace gd::hydro;

static std::vector<double> parse_list(const std::string& s) {
  std::vector<double> v; std::stringstream ss(s); std::string t;
  while (std::getline(ss, t, ',')) v.push_back(std::stod(t));
  return v;
}

// H3 saturation factor S(f, T): the ratio of the time-domain efficiency at
// V_a = 0.7 V_unit to the quadratic law (results/THZ.md, S5), interpolated in
// log f between 0.1, 0.3 and 1 THz and clamped outside; 1 for other sets.
static double saturation_factor(const std::string& set, double f) {
  static const double fs[3] = {1e11, 3e11, 1e12};
  const double* S;
  static const double S100[3] = {0.86, 0.89, 0.60}, S300[3] = {1.14, 1.10, 0.81};
  if (set == "100K") S = S100; else if (set == "300K") S = S300; else return 1.0;
  if (f <= fs[0]) return S[0];
  if (f >= fs[2]) return S[2];
  int i = f < fs[1] ? 0 : 1;
  double t = std::log(f / fs[i]) / std::log(fs[i + 1] / fs[i]);
  return S[i] + t * (S[i + 1] - S[i]);
}

int main(int argc, char** argv) {
  std::string geo = "funnel", set = "100K", term = "sym", out;
  std::vector<double> freqs;
  int nf = 25; double fmin = 1e10, fmax = 3e12, Rant = 75, slip = INFINITY, hw = 20, hm = 120;
  double n_cm2 = 1e12, d = 30e-9, eps = 3.8, nu = -1, tau = -1, tauee = -1, Rc_ohm_um = 0, T = -1;
  std::string closure = "graphene", tauscale = "1";
  bool match = false;
  for (int i = 1; i + 1 < argc; i += 2) {
    std::string k = argv[i], v = argv[i + 1];
    if (k == "--geo") geo = v; else if (k == "--set") set = v; else if (k == "--term") term = v;
    else if (k == "--f") freqs = parse_list(v); else if (k == "--nf") nf = std::stoi(v);
    else if (k == "--fmin") fmin = std::stod(v); else if (k == "--fmax") fmax = std::stod(v);
    else if (k == "--Rant") Rant = std::stod(v); else if (k == "--slip") slip = v == "inf" ? INFINITY : std::stod(v);
    else if (k == "--hw") hw = std::stod(v); else if (k == "--hm") hm = std::stod(v); else if (k == "--out") out = v;
    else if (k == "--n") n_cm2 = std::stod(v); else if (k == "--d") d = std::stod(v); else if (k == "--eps") eps = std::stod(v);
    else if (k == "--closure") closure = v; else if (k == "--nu") nu = std::stod(v); else if (k == "--tau") tau = std::stod(v); else if (k == "--tauee") tauee = std::stod(v);
    else if (k == "--Rc") Rc_ohm_um = std::stod(v); else if (k == "--match") match = std::stoi(v) != 0;
    else if (k == "--tauscale") tauscale = v; else if (k == "--T") T = std::stod(v);
    else { std::fprintf(stderr, "unknown option %s\n", k.c_str()); return 1; }
  }
  // parameter sets (plan, sec. 4): 300 K: tau_mr 1 ps, tau_ee 0.1 ps, nu 0.03; 100 K: 5 ps, 1 ps, 0.1
  if (set == "300K") { if (nu < 0) nu = 0.03; if (tau < 0) tau = 1e-12; if (tauee < 0) tauee = 0.1e-12; if (T < 0) T = 300; }
  else if (set == "100K") { if (nu < 0) nu = 0.1; if (tau < 0) tau = 5e-12; if (tauee < 0) tauee = 1e-12; if (T < 0) T = 100; }
  else if (set == "ideal") { if (nu < 0) nu = 0.0; if (tau < 0) tau = 1e30; if (tauee < 0) tauee = 0; if (T < 0) T = 0; }
  else { std::fprintf(stderr, "unknown set %s\n", set.c_str()); return 1; }
  double tscale = tauscale == "sqrtn" ? std::sqrt(n_cm2 / 1e12) : std::stod(tauscale);
  tau *= tscale; tauee *= tscale;
  if (freqs.empty()) for (int i = 0; i < nf; ++i) freqs.push_back(fmin * std::pow(fmax / fmin, nf > 1 ? double(i) / (nf - 1) : 0.0));

  ThzGeo G = make_thz_geo(geo);
  Mesh m = Mesh::load(ensure_mesh(G.b, G.mesh_name, hw, hm), G.W_ref);
  ThzUnits u = ThzUnits::make(n_cm2, d, eps, G.W_ref * 1e-9, nu, tau, tauee);
  ThzParams p;
  p.Gamma = u.Gamma; p.nu = u.inv_Res; p.tau_ee = u.t_ee; p.closure.q = u.q;
  p.closure.kind = closure == "sw" ? Closure::ShallowWater : Closure::Graphene;   // sw: the classical DS model
  p.slip = slip;
  const double Rc_ohm = Rc_ohm_um / (G.W_ref * 1e-3);   // ohm um over the contact width in um
  const double Rc = u.R_nd(Rc_ohm);
  // first-order (ac) ports for the terminations with R_c in series at every contact;
  // dc: short circuit at the source, drain grounded (both through R_c)
  auto ports_for = [&](double R) {
    std::vector<Port> ps;
    if (term == "sym") ps = {Port::thevenin(0, 0.5, R / 2 + Rc, Rc), Port::thevenin(1, -0.5, R / 2 + Rc, Rc)};
    else if (term == "dss") ps = {Port::thevenin(0, 1.0, R + Rc, Rc), Port::thevenin(1, 0.0, Rc, Rc)};
    else if (term == "dsd") ps = {Port::thevenin(0, 0.0, Rc, Rc), Port::thevenin(1, 1.0, R + Rc, Rc)};
    else if (term == "ds") ps = {Port::thevenin(0, 1.0, Rc, Rc), Port::open(1)};   // Dyakonov-Shur proper: ideal source, open drain
    return ps;
  };
  if (term != "sym" && term != "dss" && term != "dsd" && term != "ds") { std::fprintf(stderr, "unknown termination %s\n", term.c_str()); return 1; }
  p.ports = ports_for(u.R_nd(Rant));

  // validity of the operating point (H4 plan, sec. 1)
  const double kT_eV = units::kB * T / units::e, EF_eV = units::EF_J(n_cm2) / units::e;
  const double EF_kT = T > 0 ? EF_eV / kT_eV : INFINITY, U0_kT = T > 0 ? u.U0 / kT_eV : INFINITY;
  const int valid_FL = EF_kT >= 3, valid_gate = U0_kT >= 4, flag_q = u.q > 0.3;

  std::printf("%s set %s term %s closure %s: %d nodes, W_ref %g nm, n %.3g cm^-2, d %.3g nm, s0 %.3e m/s, q %.3f, U0 %.4f V, "
              "Gamma %.4f, 1/Re_s %.3e, Omega tau_ee per THz %.2f, R_unit %.1f ohm (R_ant %g -> %.3f), R_c %.1f ohm (%.3f), "
              "f_unit %.3f THz, I_unit %.3e A, V_unit %.4f V, tau %.3g ps (scale %.3f)\n",
              geo.c_str(), set.c_str(), term.c_str(), closure.c_str(), m.nn(), G.W_ref, n_cm2, d * 1e9, u.s0, u.q, u.U0, u.Gamma,
              u.inv_Res, u.Omega(1e12) * u.t_ee, u.R_unit, Rant, u.R_nd(Rant), Rc_ohm, Rc, u.f_unit / 1e12, u.I_unit, u.V_unit,
              tau * 1e12, tscale);
  std::printf("T %g K: E_F %.1f meV = %.1f kT (Fermi liquid %s), U0 = %.1f kT (gating %s), q %.2f%s\n", T, EF_eV * 1e3, EF_kT,
              valid_FL ? "ok" : "VIOLATED", U0_kT, valid_gate ? "ok" : "VIOLATED", u.q, flag_q ? " (quantum capacitance dominant)" : "");
  ThzSolver s(m, p);
  const double Rdc = s.dc_resistance(0, 1);
  const double Rdc_tot_ohm = Rdc * u.R_unit + 2 * Rc_ohm;   // dc Thevenin resistance seen by the load
  std::printf("dc resistance %.4f R_unit = %.1f ohm (+ 2 R_c = %.1f ohm)%s\n", Rdc, Rdc * u.R_unit, Rdc_tot_ohm,
              match ? "; antenna matched to Re Z_ant at each frequency" : "");
  std::printf("%10s %8s %7s | %10s %10s %9s | %11s %11s | %10s %10s | %9s | %10s %10s %9s\n", "f (Hz)", "Omega", "R_ant",
              "Re Z (ohm)", "Im Z (ohm)", "Re Z_ant", "I_sc/Va^2", "V_oc/Va^2", "R_I (A/W)", "R_V (V/W)", "P_in/P_av", "NEP W/rtHz",
              "eta/P (1/W)", "P_knee W");
  std::ofstream csv;
  if (!out.empty()) {
    csv.open(out); csv.precision(10);
    csv << "geo,set,term,f_Hz,Omega,ReZ_ohm,ImZ_ohm,Isc_per_Va2,Voc_per_Va2,RI_A_per_W,RV_V_per_W,Pin_over_Pav,Rdc_ohm,"
           "ReZant_ohm,ImZant_ohm,Rant_ohm,Rc_ohm,n_cm2,d_m,T_K,tau_s,tauee_s,nu,EF_over_kT,U0_over_kT,q,valid_FL,valid_gate,flag_q,"
           "Vunit_V,Runit_ohm,NEP_W_per_rtHz,eta_per_W,Pknee_W,S_sat,eta_peak,eta_1uW,eta_10uW,eta_100uW\n";
  }
  struct Row { cd Z, Zant; double Pin_Pav, Isc, Voc, Rant; };
  // one evaluation at a given antenna resistance: linear solve, short-circuit and open-circuit dc solves
  auto evaluate = [&](double R_ohm, double f) {
    double R = u.R_nd(R_ohm), Om = u.Omega(f);
    ThzParams pp = p; pp.ports = ports_for(R);
    ThzSolver ss(m, pp);
    FirstOrder fo = ss.solve_linear(Om);
    Row r; r.Rant = R_ohm;
    r.Z = (fo.V1[0] - fo.V1[1]) / fo.I1[0];                   // device impedance (V_s - V_d) / I_s, as in S2
    if (term == "sym") { cd Id = 0.5 * (fo.I1[0] - fo.I1[1]); r.Zant = (1.0 - R * Id) / Id; }
    else { int c = term == "dsd" ? 1 : 0; r.Zant = (1.0 - R * fo.I1[c]) / fo.I1[c]; }
    r.Pin_Pav = (fo.power_in(0) + fo.power_in(1)) * 8 * R;    // |Va|^2 = 1 for every termination
    SecondOrder sc = ss.solve_dc(fo);
    r.Isc = sc.I20[0];
    if (term == "ds") { r.Isc = 0; r.Voc = sc.V20[1] - sc.V20[0]; }   // DS readout: drain dc voltage, source dc grounded
    else {
      ThzParams po = pp; po.ports[0].dc = Port::Kind::Open;   // open-circuit readout: source dc open, drain through R_c (no dc current)
      ThzSolver so(m, po);
      SecondOrder oc = so.solve_dc(fo);
      r.Voc = oc.V20[0] - oc.V20[1];
    }
    return r;
  };
  for (double f : freqs) {
    double Om = u.Omega(f);
    Row r = evaluate(Rant, f);
    if (match && std::real(r.Zant) > 0) r = evaluate(std::real(r.Zant) * u.R_unit, f);
    double RI = u.amp_per_watt(r.Isc, r.Rant), RV = u.volt_per_watt(r.Voc, r.Rant);
    // detector and rectenna figures (H4 plan, sec. 3): Johnson NEP (current noise of the dc Thevenin
    // resistance over the current responsivity); matched-load efficiency per watt of available power; the knee at
    // V_a = 0.5 V_unit; the peak at 0.7 V_unit times the H3 saturation factor
    double NEP = RI > 0 ? std::sqrt(4 * units::kB * T / Rdc_tot_ohm) / RI : INFINITY;   // current noise of R_dc + 2 R_c over R_I
    double eta_per_W = 16 * r.Voc * r.Voc * r.Rant * r.Rant / (Rdc_tot_ohm * u.V_unit * u.V_unit);
    double Pknee = std::pow(0.5 * u.V_unit, 2) / (8 * r.Rant);
    double S = saturation_factor(set, f);
    double eta_peak = 2 * r.Voc * r.Voc * (r.Rant / Rdc_tot_ohm) * 0.49 * S;
    auto eta_at = [&](double P) { return std::min(eta_per_W * P, eta_peak); };
    std::printf("%10.3e %8.4f %7.1f | %10.2f %10.2f %9.2f | %11.4e %11.4e | %10.3e %10.3e | %9.4f | %10.3e %10.3e %9.3e\n", f, Om, r.Rant,
                std::real(r.Z) * u.R_unit, std::imag(r.Z) * u.R_unit, std::real(r.Zant) * u.R_unit, r.Isc, r.Voc, RI, RV, r.Pin_Pav, NEP, eta_per_W, Pknee);
    std::fflush(stdout);
    if (csv) csv << geo << "," << set << "," << term << "," << f << "," << Om << "," << std::real(r.Z) * u.R_unit << ","
                 << std::imag(r.Z) * u.R_unit << "," << r.Isc << "," << r.Voc << "," << RI << "," << RV << "," << r.Pin_Pav << ","
                 << Rdc * u.R_unit << "," << std::real(r.Zant) * u.R_unit << "," << std::imag(r.Zant) * u.R_unit << "," << r.Rant << "," << Rc_ohm << "," << n_cm2 << "," << d << "," << T << "," << tau << ","
                 << tauee << "," << nu << "," << EF_kT << "," << U0_kT << "," << u.q << "," << valid_FL << "," << valid_gate << ","
                 << flag_q << "," << u.V_unit << "," << u.R_unit << "," << NEP << "," << eta_per_W << "," << Pknee << "," << S << ","
                 << eta_peak << "," << eta_at(1e-6) << "," << eta_at(1e-5) << "," << eta_at(1e-4) << "\n";
  }
  return 0;
}
