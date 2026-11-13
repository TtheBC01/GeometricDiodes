// Stage H5 driver (plans/hydro_dcgate.md): steady gated-compressible flow through
// a device under the two reversal protocols. For each target bias v = V / U_0
// four steady solves: current +I or -I injected at contact 0 (contact 1 grounded
// to the gate), and the same at contact 1 (contact 0 grounded).
//   a: drive 0, +I  flow 0 -> 1, gate tied to the downstream contact
//   b: drive 0, -I  flow 1 -> 0, gate tied to the upstream contact
//   c: drive 1, +I  flow 1 -> 0, gate tied to the downstream contact
//   d: drive 1, -I  flow 0 -> 1, gate tied to the upstream contact
// Swap the contacts (shape term):  D_swap_dn = V_c / V_a,  D_swap_up = |V_b| / |V_d|
// Flip the polarity (circuit + shape):  D_flip0 = |V_b| / V_a,  D_flip1 = |V_d| / V_c
// Reverse = flow from contact 1 to contact 0, as in Stage H.
//
// usage: gd_dc_gate --geo chan|symcon|funnel|funnel_w1000|tesla_classic|tesla_classic_w1000
//                   [--set 100K|300K] [--slip inf|0] [--hw 20 --hm 120] [--v 0.003,0.01,...]
//                   [--closure graphene|sw] [--n 1e12 --d 30e-9 --eps 3.8] [--out file.csv]
#include "dc_gate_common.hpp"
#include "generators.hpp"
#include "thz_geo.hpp"
#include "hydro/mesh.hpp"
#include "hydro/thz_units.hpp"
#include "hydro/units.hpp"
#include <chrono>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace gd;
using namespace gd::hydro;

int main(int argc, char** argv) {
  std::string geo = "funnel", set = "300K", out, closure = "graphene";
  std::vector<double> vs = {0.003, 0.01, 0.03, 0.1, 0.2, 0.3};
  double slip = INFINITY, hw = 20, hm = 120, n_cm2 = 1e12, d = 30e-9, eps = 3.8;
  for (int i = 1; i + 1 < argc; i += 2) {
    std::string k = argv[i], v = argv[i + 1];
    if (k == "--geo") geo = v; else if (k == "--set") set = v; else if (k == "--out") out = v;
    else if (k == "--slip") slip = v == "inf" ? INFINITY : std::stod(v);
    else if (k == "--hw") hw = std::stod(v); else if (k == "--hm") hm = std::stod(v);
    else if (k == "--closure") closure = v;
    else if (k == "--n") n_cm2 = std::stod(v); else if (k == "--d") d = std::stod(v); else if (k == "--eps") eps = std::stod(v);
    else if (k == "--v") { vs.clear(); std::stringstream ss(v); std::string t; while (std::getline(ss, t, ',')) vs.push_back(std::stod(t)); }
    else { std::fprintf(stderr, "unknown option %s\n", k.c_str()); return 1; }
  }
  double nu, tau, tauee;
  if (set == "300K") { nu = 0.03; tau = 1e-12; tauee = 0.1e-12; }
  else if (set == "100K") { nu = 0.1; tau = 5e-12; tauee = 1e-12; }
  else { std::fprintf(stderr, "unknown set %s\n", set.c_str()); return 1; }

  ThzGeo G;
  if (geo == "symcon") G = {make_symmetric_constriction(2000, 6000, 30, 175, 500), 2000, "h_symcon"};
  else G = make_thz_geo(geo);
  Mesh m = Mesh::load(ensure_mesh(G.b, G.mesh_name, hw, hm), G.W_ref);
  ThzUnits u = ThzUnits::make(n_cm2, d, eps, G.W_ref * 1e-9, nu, tau, tauee);
  ThzParams p;
  p.Gamma = u.Gamma; p.nu = u.inv_Res; p.closure.q = u.q; p.slip = slip;
  p.closure.kind = closure == "sw" ? Closure::ShallowWater : Closure::Graphene;
  const double U0 = 1 - u.q;   // gate overdrive in units of V_unit

  auto t0 = std::chrono::steady_clock::now();
  auto secs = [&]() { return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count(); };
  // small-signal resistance from a tiny current, both drive contacts
  const double Ie = 1e-7;
  SteadyPoint e0 = steady_point(m, p, 0, Ie), e1 = steady_point(m, p, 1, Ie);
  double R0 = e0.V / Ie, R1 = e1.V / Ie;
  std::printf("%s set %s slip %g mesh hw %g hm %g (%d nodes): R = %.6f / %.6f R_unit = %.1f ohm, U_0 = %.4f V, s_0 = %.3f v_F, "
              "Gamma %.4f, 1/Re_s %.3e, q %.4f (%.1f s)\n", geo.c_str(), set.c_str(), slip, hw, hm, m.nn(), R0, R1, R0 * u.R_unit,
              u.U0, u.s0 / units::vF, p.Gamma, p.nu, u.q, secs());
  bool fresh = true;
  if (!out.empty()) { std::ifstream in(out); fresh = !in.good() || in.peek() == std::ifstream::traits_type::eof(); }
  std::ofstream csv;
  if (!out.empty()) {
    csv.open(out, std::ios::app); csv.precision(12);
    if (fresh) csv << "geo,set,slip,closure,hw,hm,nodes,v_target,I_nd,I_A,Re,V_a,V_b,V_c,V_d,V_a_V,ok,D_swap_dn,D_swap_up,D_flip0,D_flip1,"
                      "D_1d_flip,umax_vF,nmin,nmax,R_ohm,U0_V,q,seconds\n";
  }
  std::printf("%8s %10s %8s %12s %12s %12s %12s %8s %7s\n", "V/U_0", "I (uA)", "Re", "swap_dn - 1", "swap_up - 1", "flip0 - 1", "1-D flip - 1", "u/v_F", "s");
  for (double v : vs) {
    double I = v * U0 / R0, ts = secs();
    SteadyPoint a = steady_point(m, p, 0, I), b = steady_point(m, p, 0, -I), c = steady_point(m, p, 1, I), dd = steady_point(m, p, 1, -I);
    bool ok = a.ok && b.ok && c.ok && dd.ok;
    double Dsd = c.V / a.V, Dsu = std::fabs(b.V) / std::fabs(dd.V), Df0 = std::fabs(b.V) / a.V, Df1 = std::fabs(dd.V) / c.V;
    // one-dimensional prediction for the flip with the same small-signal resistance (Gamma L -> R0), no inertia
    ClosureFn cl = p.closure;
    double Vp = 0, Vm = 0;
    {
      // inertia off: solve G(n) - G(1) = R0 * I directly (j -> 0 in F with Gamma L j held)
      auto solve = [&](double RI) {
        double n = std::max(0.05, 1.0 + RI);
        for (int it = 0; it < 100; ++it) {
          double Gn = cl.kind == Closure::ShallowWater ? 0.5 * n * n : (2.0 / 3.0) * (1 - cl.q) * std::pow(n, 1.5) + cl.q * n;
          double G1 = cl.kind == Closure::ShallowWater ? 0.5 : (2.0 / 3.0) * (1 - cl.q) + cl.q;
          double dn = (Gn - G1 - RI) / (n * cl.hp(n)); n -= dn; if (n < 1e-3) n = 1e-3;
          if (std::fabs(dn) < 1e-14) break;
        }
        return cl.Phi(n);
      };
      Vp = solve(R0 * I); Vm = solve(-R0 * I);
    }
    double D1d = std::fabs(Vm) / Vp;
    double umax = std::max({a.umax, b.umax, c.umax, dd.umax}) * u.s0 / units::vF;
    double nmin = std::min({a.nmin, b.nmin, c.nmin, dd.nmin}), nmax = std::max({a.nmax, b.nmax, c.nmax, dd.nmax});
    double dt = secs() - ts;
    std::printf("%8.4f %10.3f %8.3f %12.4e %12.4e %12.4e %12.4e %8.4f %7.1f%s\n", v, I * u.I_unit * 1e6, I / p.nu, Dsd - 1, Dsu - 1, Df0 - 1, D1d - 1,
                umax, dt, ok ? "" : "  NOT CONVERGED");
    std::fflush(stdout);
    if (csv.is_open())
      csv << geo << "," << set << "," << slip << "," << closure << "," << hw << "," << hm << "," << m.nn() << "," << v << "," << I << "," << I * u.I_unit << ","
          << I / p.nu << "," << a.V << "," << b.V << "," << c.V << "," << dd.V << "," << a.V * u.V_unit << "," << ok << "," << Dsd << "," << Dsu << ","
          << Df0 << "," << Df1 << "," << D1d << "," << umax << "," << nmin << "," << nmax << "," << R0 * u.R_unit << "," << u.U0 << "," << u.q << "," << dt << "\n";
  }
  return 0;
}
