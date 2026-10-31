// Stage H3 (plans/hydro_largesignal.md): large-signal conversion efficiency of
// a device under an antenna drive, from the full nonlinear BDF2 time-domain
// solver, with the small-signal (perturbation) asymptote on the same mesh.
//
// usage: gd_thz_power --geo funnel_w1000 [--set 100K|300K] [--term dss|sym|ant]
//                     --f 3e11 [--Va 0.01,0.02,...] [--RL match|<ohm>]
//                     [--steps 60] [--settle auto] [--relax auto] [--bias_updates 0] [--min_periods 6]
//                     [--Rant 75] [--hw 80 --hm 480] [--out file.csv] [--verbose 1]
//                     [--n 1e12 --d 30e-9 --eps 3.8] [--nu x --tau x --tauee x]
// terminations: dss = Thevenin antenna at the source, drain grounded to the gate,
// ideal bias tee with dc load RL at the source; sym = antenna split +-Va/2 with
// R/2 at each contact, bias tee with load RL at the source, drain dc-grounded;
// ant = dss without a bias tee (the dc current returns through R_ant, RL = R_ant).
// The amplitude ladder stops at the first point where Newton fails or n <= 0.
#include "generators.hpp"
#include "thz_geo.hpp"
#include "thz_power_common.hpp"
#include "hydro/mesh.hpp"
#include "hydro/thz_solver.hpp"
#include "hydro/thz_time.hpp"
#include "hydro/thz_units.hpp"
#include <chrono>
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
  std::string geo = "funnel_w1000", set = "100K", term = "dss", out, RLs = "match", closure = "graphene";
  std::vector<double> freqs, Vas = {0.01, 0.02, 0.05, 0.1, 0.2, 0.3, 0.5, 0.7};
  int steps = 60, bias_updates = 0, min_periods = 6, verbose = 0;
  double settle = -1, relax = -1, Rant = 75, hw = 80, hm = 480, tol = 1e-10;
  double n_cm2 = 1e12, d = 30e-9, eps = 3.8, nu = -1, tau = -1, tauee = -1;
  for (int i = 1; i + 1 < argc; i += 2) {
    std::string k = argv[i], v = argv[i + 1];
    if (k == "--geo") geo = v; else if (k == "--set") set = v; else if (k == "--term") term = v;
    else if (k == "--f") freqs = parse_list(v); else if (k == "--Va") Vas = parse_list(v); else if (k == "--RL") RLs = v;
    else if (k == "--steps") steps = std::stoi(v); else if (k == "--settle") settle = std::stod(v);
    else if (k == "--bias_updates") bias_updates = std::stoi(v); else if (k == "--min_periods") min_periods = std::stoi(v);
    else if (k == "--relax") relax = std::stod(v);
    else if (k == "--Rant") Rant = std::stod(v); else if (k == "--hw") hw = std::stod(v); else if (k == "--hm") hm = std::stod(v);
    else if (k == "--out") out = v; else if (k == "--verbose") verbose = std::stoi(v); else if (k == "--tol") tol = std::stod(v);
    else if (k == "--n") n_cm2 = std::stod(v); else if (k == "--d") d = std::stod(v); else if (k == "--eps") eps = std::stod(v);
    else if (k == "--closure") closure = v; else if (k == "--nu") nu = std::stod(v); else if (k == "--tau") tau = std::stod(v); else if (k == "--tauee") tauee = std::stod(v);
    else { std::fprintf(stderr, "unknown option %s\n", k.c_str()); return 1; }
  }
  if (set == "300K") { if (nu < 0) nu = 0.03; if (tau < 0) tau = 1e-12; if (tauee < 0) tauee = 0.1e-12; }
  else if (set == "100K") { if (nu < 0) nu = 0.1; if (tau < 0) tau = 5e-12; if (tauee < 0) tauee = 1e-12; }
  else if (set == "ideal") { if (nu < 0) nu = 0.0; if (tau < 0) tau = 1e30; if (tauee < 0) tauee = 0; }
  else { std::fprintf(stderr, "unknown set %s\n", set.c_str()); return 1; }
  if (freqs.empty()) { std::fprintf(stderr, "--f required\n"); return 1; }
  if (term != "dss" && term != "sym" && term != "ant") { std::fprintf(stderr, "unknown termination %s\n", term.c_str()); return 1; }

  ThzGeo G = make_thz_geo(geo);
  Mesh m = Mesh::load(ensure_mesh(G.b, G.mesh_name, hw, hm), G.W_ref);
  ThzUnits u = ThzUnits::make(n_cm2, d, eps, G.W_ref * 1e-9, nu, tau, tauee);
  ThzParams p;
  p.Gamma = u.Gamma; p.nu = u.inv_Res; p.tau_ee = u.t_ee; p.closure.q = u.q;
  p.closure.kind = closure == "sw" ? Closure::ShallowWater : Closure::Graphene;
  p.tau_ee = 0;   // the nonlinear time-domain solver is Newtonian; keep the small-signal reference consistent
  p.verbose = verbose > 1;
  double R = u.R_nd(Rant);
  // ports at unit amplitude; the dc load is filled in below
  auto ports_for = [&](double Va, double RL) {
    std::vector<Port> ps;
    if (term == "sym") ps = {Port::thevenin(0, 0.5 * Va, R / 2, RL), Port::thevenin(1, -0.5 * Va, R / 2, 0.0)};
    else ps = {Port::thevenin(0, Va, R, RL), Port::ground(1)};
    return ps;
  };
  p.ports = ports_for(1.0, 0.0);
  ThzSolver s(m, p);
  double Rdc = s.dc_resistance(0, 1);
  double RL = term == "ant" ? R : (RLs == "match" ? Rdc : u.R_nd(std::stod(RLs)));
  bool bias = term != "ant";
  // slow modes: cavity ringing 2/Gamma (settle: three e-folds, at least 10 time units);
  // the dc charge of the leads relaxes through R_ant + R_dc with the RC time of the
  // device, which the perturbative start leaves excited only by the nonlinear
  // correction and the Aitken extrapolation removes.
  double xmin = 1e300, xmax = -1e300;
  for (const auto& v : m.X) { xmin = std::min(xmin, v.x); xmax = std::max(xmax, v.x); }
  double Ldev = xmax - xmin;
  if (relax < 0) relax = 1.0 / u.Gamma + u.Gamma * std::pow(2 * Ldev / M_PI, 2);
  if (settle < 0) settle = std::max(3 * 2 / u.Gamma, 10.0);
  std::printf("dc resistance %.4f R_unit = %.1f ohm; dc load R_L %.4f R_unit = %.1f ohm (%s); device length %.2f W_ref, settle %.1f, relax %.1f\n",
              Rdc, Rdc * u.R_unit, RL, RL * u.R_unit, bias ? "bias tee" : "no bias tee: dc through R_ant", Ldev, settle, relax);
  std::ofstream csv;
  if (!out.empty()) {
    csv.open(out); csv.precision(10);
    csv << "geo,set,term,f_Hz,Omega,Va,Pav_W,Pin_W,Idc_raw_A,Idc_ext_A,Istar_A,Vdc_V,Vrect_V,Pdc_W,eta_av,eta_abs,eta_small,Pin_over_Pav_lin,"
           "eta_small_bdf,n_min,n_max,tail,tau_periods,corr,dc_drift,periods,steps,ok,wall_s,Rdc_ohm,RL_ohm\n";
  }
  for (double f : freqs) {
    double Om = u.Omega(f), Tp = 2 * M_PI / Om;
    // small-signal asymptote on this mesh: loaded dc current per Va^2 with the same dc load
    ThzParams pl = p; pl.ports = ports_for(1.0, RL);
    ThzSolver sl(m, pl);
    FirstOrder fo = sl.solve_linear(Om);
    SecondOrder sc = sl.solve_dc(fo);
    double I20 = sc.I20[0];                       // loaded dc current per Va^2 (nondimensional)
    double Pav1 = 1.0 / (8 * R);                  // available power per Va^2
    double eta_small = I20 * I20 * RL / Pav1;     // eta / Va^2
    double Pin_lin = (fo.power_in(0) + fo.power_in(1)) / Pav1;
    // the same for the BDF2-discrete operator: the periodic state the time stepping converges to at O(Va^2)
    FirstOrder fod = sl.solve_linear(Om, ThzSolver::bdf2_symbol(Om, Tp / steps));
    SecondOrder scd = sl.solve_dc(fod);
    double eta_small_bdf = scd.I20[0] * scd.I20[0] * RL / Pav1;
    LsOptions o;
    o.steps = steps; o.tol = tol; o.bias = bias; o.Rdc = Rdc; o.RL = RL;
    o.first = std::max(1, (int)std::ceil(settle / Tp)); o.every = std::max(1, (int)std::ceil(relax / Tp)); o.bias_updates = bias_updates;
    o.periods = std::max(min_periods, o.first + 3 + bias_updates * o.every);
    std::printf("f %.3e Hz, Omega %.4f, period %.3f: small-signal I_dc/Va^2 %.4e, eta/Va^2 %.4e (BDF2-discrete %.4e), P_in/P_av %.3f; "
                "%d periods x %d steps (%d bias updates from period %d every %d)\n", f, Om, Tp, I20, eta_small, eta_small_bdf, Pin_lin,
                o.periods, steps, bias_updates, o.first, o.every);
    std::printf("%7s %10s %10s %11s %11s %11s %10s %10s %10s %7s %7s %8s %8s %8s %6s\n", "Va", "P_av (W)", "P_in (W)", "I_dc raw", "I* (A)",
                "P_dc (W)", "eta_av", "eta_abs", "eta_small", "n_min", "n_max", "tail", "corr", "drift", "wall");
    for (double Va : Vas) {
      ThzParams pt = p; pt.ports = ports_for(Va, RL); pt.verbose = verbose > 1;
      LsPoint r = run_ls_point(m, pt, fod, scd, Va, Om, o);
      double Pav = Va * Va * Pav1;
      double eta_av = r.Pdc / Pav, eta_abs = r.Pin > 0 ? r.Pdc / r.Pin : 0.0, es = eta_small * Va * Va;
      std::printf("%7.3f %10.3e %10.3e %11.4e %11.4e %11.4e %10.3e %10.3e %10.3e %7.4f %7.4f %8.1e %8.1e %8.1e %6.0f%s\n", Va, Pav * u.P_unit,
                  r.Pin * u.P_unit, r.I_raw * u.I_unit, r.Istar * u.I_unit, r.Pdc * u.P_unit, eta_av, eta_abs, es, r.tr.n_min, r.tr.n_max,
                  r.tail, r.corr, r.tr.dc_drift, r.wall, r.ok ? "" : (r.tr.ok ? "  (n <= 0: breakdown)" : "  (Newton failed: breakdown)"));
      std::fflush(stdout);
      if (csv) csv << geo << "," << set << "," << term << "," << f << "," << Om << "," << Va << "," << Pav * u.P_unit << ","
                   << r.Pin * u.P_unit << "," << r.I_raw * u.I_unit << "," << r.I_ext * u.I_unit << "," << r.Istar * u.I_unit << ","
                   << r.V_raw * u.V_unit << "," << r.Vrect * u.V_unit << "," << r.Pdc * u.P_unit << "," << eta_av << "," << eta_abs << ","
                   << es << "," << Pin_lin << "," << eta_small_bdf * Va * Va << "," << r.tr.n_min << "," << r.tr.n_max << "," << r.tail << "," << r.tau_periods << ","
                   << r.corr << "," << r.tr.dc_drift << "," << r.tr.periods_run << "," << steps << "," << (r.ok ? 1 : 0) << "," << r.wall
                   << "," << Rdc * u.R_unit << "," << RL * u.R_unit << "\n";
      if (csv) csv.flush();
      if (!r.ok) break;
    }
  }
  return 0;
}
