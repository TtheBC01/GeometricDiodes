// One large-signal point for Stage H3 (plans/hydro_largesignal.md): time-domain
// run started from the perturbative state, Aitken tail extrapolation and the dc
// Thevenin correction to the resistor fixed point. Shared by gd_thz_power and
// the ladder (T10-T13).
#pragma once
#include "hydro/mesh.hpp"
#include "hydro/thz_solver.hpp"
#include "hydro/thz_time.hpp"
#include <chrono>
#include <cmath>

namespace gd::hydro {

struct LsOptions {
  int steps = 60;
  int periods = 10;
  double tol = 1e-10;
  bool bias = true;          // ideal bias tee (dc load RL) vs the antenna as dc load
  double Rdc = 1, RL = 1;    // small-signal dc resistance of the device, dc load
  double seed_scale = 1.0;   // start the bias at seed_scale x the small-signal load current
  int bias_updates = 0, first = 1, every = 1;
};

struct LsPoint {
  double Va = 0;
  TimeTrace tr;
  double I_raw = 0, V_raw = 0, I_ext = 0, V_ext = 0, tail = 0, tau_periods = -1;
  double Vrect = 0, Istar = 0, corr = 0;   // dc Thevenin source, resistor fixed point, relative correction
  double Pin = 0, Pdc = 0, wall = 0;
  bool ok = false;
};

// p: parameters with the ports at amplitude Va (ac) and the dc load RL; fo, sc: the
// perturbation solution per unit amplitude with the same ports.
inline LsPoint run_ls_point(const Mesh& m, const ThzParams& p, const FirstOrder& fo, const SecondOrder& sc, double Va, double Omega,
                            const LsOptions& o) {
  LsPoint r; r.Va = Va;
  ThzTimeSolver ts(m, p, 1);
  ts.init_from_perturbation(fo, sc, Va, Omega, o.steps);
  BiasTee bt; bt.on = o.bias; bt.Rdc_dev = o.Rdc; bt.first = o.first; bt.every = o.every; bt.max_updates = o.bias_updates;
  // every port carries its perturbative dc current (the drain's is minus the source's):
  // a port whose dc load differs from its ac resistance needs its own bias current
  bt.seed.assign(sc.I20.size(), 0.0);
  for (size_t c = 0; c < sc.I20.size(); ++c) bt.seed[c] = o.seed_scale * sc.I20[c] * Va * Va;
  auto t0 = std::chrono::steady_clock::now();
  r.tr = ts.run(Omega, o.periods, o.steps, o.tol, bt);
  r.wall = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  r.ok = r.tr.ok && r.tr.n_min > 0;
  // dc voltage across the device: source minus drain (the drain is at 0 only when its dc load is a short)
  auto across = [&](const std::vector<double>& V) { return V.size() > 1 ? V[0] - V[1] : V[0]; };
  r.I_raw = r.tr.I_dc[0]; r.V_raw = across(r.tr.V_dc);
  r.I_ext = r.tr.I_dc_ext.empty() ? r.I_raw : r.tr.I_dc_ext[0];
  r.V_ext = r.tr.V_dc_ext.empty() ? r.V_raw : across(r.tr.V_dc_ext);
  r.tail = r.tr.tail; r.tau_periods = r.tr.tau_periods;
  // use the extrapolated means unless the tail estimate is unreasonable
  double I = r.tail < 0.2 ? r.I_ext : r.I_raw, V = r.tail < 0.2 ? r.V_ext : r.V_raw;
  // device dc Thevenin at this pumping: V = Vrect + Rdc I; load fixed point I* = -Vrect / (Rdc + RL)
  r.Vrect = V - o.Rdc * I;
  r.Istar = -r.Vrect / (o.Rdc + o.RL);
  r.corr = (r.Istar - I) / (std::fabs(r.Istar) > 1e-300 ? r.Istar : 1e-300);
  r.Pin = r.tr.P_in;
  r.Pdc = r.Istar * r.Istar * o.RL;
  return r;
}

}  // namespace gd::hydro
