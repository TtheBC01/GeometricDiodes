// Stage H2 science runs (plans/hydro_thz.md, sec. 4): second-order rectified
// response of a device under an antenna drive, swept in frequency.
//
// usage: gd_thz_sweep --geo funnel [--set 100K|300K] [--term sym|dss|dsd]
//                     [--f 1e10,...] [--nf 25 --fmin 1e10 --fmax 3e12]
//                     [--Rant 75] [--slip inf] [--hw 20 --hm 120] [--out file.csv]
//                     [--n 1e12 --d 30e-9 --eps 3.8] [--nu x --tau x --tauee x]
// geometries: chan (uniform 1 um x 10 um channel), funnel (175 nm neck, 2 um
// leads 6 um long), funnel_short (same with 1 um leads), funnel_w1000 (1 um neck, 4 um leads), tesla_classic, tesla_classic_w1000.
// terminations: ds = ideal ac source at the source, drain open (Dyakonov-Shur
// detector, readout = drain dc voltage); sym = antenna split +-Va/2 with R/2 at each contact (gate at
// the midpoint potential: geometric rectification only); dss = Thevenin at the
// source, drain grounded to the gate (Dyakonov-Shur configuration); dsd = the
// mirror image (source grounded, Thevenin at the drain).
#include "generators.hpp"
#include "thz_geo.hpp"
#include "hydro/mesh.hpp"
#include "hydro/thz_solver.hpp"
#include "hydro/thz_units.hpp"
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

int main(int argc, char** argv) {
  std::string geo = "funnel", set = "100K", term = "sym", out;
  std::vector<double> freqs;
  int nf = 25; double fmin = 1e10, fmax = 3e12, Rant = 75, slip = INFINITY, hw = 20, hm = 120;
  double n_cm2 = 1e12, d = 30e-9, eps = 3.8, nu = -1, tau = -1, tauee = -1;
  std::string closure = "graphene";
  for (int i = 1; i + 1 < argc; i += 2) {
    std::string k = argv[i], v = argv[i + 1];
    if (k == "--geo") geo = v; else if (k == "--set") set = v; else if (k == "--term") term = v;
    else if (k == "--f") freqs = parse_list(v); else if (k == "--nf") nf = std::stoi(v);
    else if (k == "--fmin") fmin = std::stod(v); else if (k == "--fmax") fmax = std::stod(v);
    else if (k == "--Rant") Rant = std::stod(v); else if (k == "--slip") slip = v == "inf" ? INFINITY : std::stod(v);
    else if (k == "--hw") hw = std::stod(v); else if (k == "--hm") hm = std::stod(v); else if (k == "--out") out = v;
    else if (k == "--n") n_cm2 = std::stod(v); else if (k == "--d") d = std::stod(v); else if (k == "--eps") eps = std::stod(v);
    else if (k == "--closure") closure = v; else if (k == "--nu") nu = std::stod(v); else if (k == "--tau") tau = std::stod(v); else if (k == "--tauee") tauee = std::stod(v);
    else { std::fprintf(stderr, "unknown option %s\n", k.c_str()); return 1; }
  }
  // parameter sets (plan, sec. 4): 300 K: tau_mr 1 ps, tau_ee 0.1 ps, nu 0.03; 100 K: 5 ps, 1 ps, 0.1
  if (set == "300K") { if (nu < 0) nu = 0.03; if (tau < 0) tau = 1e-12; if (tauee < 0) tauee = 0.1e-12; }
  else if (set == "100K") { if (nu < 0) nu = 0.1; if (tau < 0) tau = 5e-12; if (tauee < 0) tauee = 1e-12; }
  else if (set == "ideal") { if (nu < 0) nu = 0.0; if (tau < 0) tau = 1e30; if (tauee < 0) tauee = 0; }
  else { std::fprintf(stderr, "unknown set %s\n", set.c_str()); return 1; }
  if (freqs.empty()) for (int i = 0; i < nf; ++i) freqs.push_back(fmin * std::pow(fmax / fmin, nf > 1 ? double(i) / (nf - 1) : 0.0));

  ThzGeo G = make_thz_geo(geo);
  Mesh m = Mesh::load(ensure_mesh(G.b, G.mesh_name, hw, hm), G.W_ref);
  ThzUnits u = ThzUnits::make(n_cm2, d, eps, G.W_ref * 1e-9, nu, tau, tauee);
  ThzParams p;
  p.Gamma = u.Gamma; p.nu = u.inv_Res; p.tau_ee = u.t_ee; p.closure.q = u.q;
  p.closure.kind = closure == "sw" ? Closure::ShallowWater : Closure::Graphene;   // sw: the classical DS model
  p.slip = slip;
  double R = u.R_nd(Rant);
  // first-order (ac) ports for the three terminations; dc: short circuit at the source, drain grounded
  if (term == "sym") p.ports = {Port::thevenin(0, 0.5, R / 2, 0.0), Port::thevenin(1, -0.5, R / 2, 0.0)};
  else if (term == "dss") p.ports = {Port::thevenin(0, 1.0, R, 0.0), Port::ground(1)};
  else if (term == "dsd") p.ports = {Port::ground(0), Port::thevenin(1, 1.0, R, 0.0)};
  else if (term == "ds") p.ports = {Port::thevenin(0, 1.0, 0.0, 0.0), Port::open(1)};   // Dyakonov-Shur proper: ideal source, open drain
  else { std::fprintf(stderr, "unknown termination %s\n", term.c_str()); return 1; }

  std::printf("%s set %s term %s closure %s: %d nodes, W_ref %g nm, s0 %.3e m/s, q %.3f, U0 %.3f V, Gamma %.4f, 1/Re_s %.3e, "
              "Omega tau_ee per THz %.2f, R_unit %.1f ohm (R_ant %g -> %.3f), f_unit %.3f THz, I_unit %.3e A, V_unit %.3f V\n",
              geo.c_str(), set.c_str(), term.c_str(), closure.c_str(), m.nn(), G.W_ref, u.s0, u.q, u.U0, u.Gamma, u.inv_Res,
              u.Omega(1e12) * u.t_ee, u.R_unit, Rant, R, u.f_unit / 1e12, u.I_unit, u.V_unit);
  ThzSolver s(m, p);
  double Rdc = s.dc_resistance(0, 1);
  std::printf("dc resistance %.4f R_unit = %.1f ohm\n", Rdc, Rdc * u.R_unit);
  std::printf("%10s %8s | %10s %10s | %11s %11s | %10s %10s | %9s\n", "f (Hz)", "Omega", "Re Z (ohm)", "Im Z (ohm)",
              "I_sc/Va^2", "V_oc/Va^2", "R_I (A/W)", "R_V (V/W)", "P_in/P_av");
  std::ofstream csv;
  if (!out.empty()) {
    csv.open(out); csv.precision(10);
    csv << "geo,set,term,f_Hz,Omega,ReZ_ohm,ImZ_ohm,Isc_per_Va2,Voc_per_Va2,RI_A_per_W,RV_V_per_W,Pin_over_Pav,Rdc_ohm\n";
  }
  ThzParams po = p; po.ports[0].dc = Port::Kind::Open;   // open-circuit readout: source dc open, drain grounded
  ThzSolver so(m, po);
  for (double f : freqs) {
    double Om = u.Omega(f);
    FirstOrder fo = s.solve_linear(Om);
    // device impedance seen by the antenna: (V_s - V_d) / I_s
    cd Z = (fo.V1[0] - fo.V1[1]) / fo.I1[0];
    double Pin = fo.power_in(0) + fo.power_in(1);
    double Pav = 1.0 / (8 * R);   // |Va|^2 = 1 for both terminations
    SecondOrder sc = s.solve_dc(fo);
    double Isc = sc.I20[0], Voc;
    if (term == "ds") { Isc = 0; Voc = sc.V20[1] - sc.V20[0]; }   // DS readout: drain dc voltage, source dc grounded
    else { SecondOrder oc = so.solve_dc(fo); Voc = oc.V20[0] - oc.V20[1]; }
    double RI = u.amp_per_watt(Isc, Rant), RV = u.volt_per_watt(Voc, Rant);
    std::printf("%10.3e %8.4f | %10.2f %10.2f | %11.4e %11.4e | %10.3e %10.3e | %9.4f\n", f, Om, std::real(Z) * u.R_unit,
                std::imag(Z) * u.R_unit, Isc, Voc, RI, RV, Pin / Pav);
    std::fflush(stdout);
    if (csv) csv << geo << "," << set << "," << term << "," << f << "," << Om << "," << std::real(Z) * u.R_unit << ","
                 << std::imag(Z) * u.R_unit << "," << Isc << "," << Voc << "," << RI << "," << RV << "," << Pin / Pav << ","
                 << Rdc * u.R_unit << "\n";
  }
  return 0;
}
