// Acceptance ladder for the hydrodynamic solver (plans/hydro_tesla.md, sec. 3).
#include "generators.hpp"
#include "hydro/diode.hpp"
#include "hydro/mesh.hpp"
#include "hydro/ns_solver.hpp"
#include "hydro/post.hpp"
#include "hydro/units.hpp"
#include <chrono>
#include <cstdarg>
#include <cmath>
#include <cstdio>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace gd;
using namespace gd::hydro;

static int n_pass = 0, n_fail = 0;
static void report(const std::string& name, bool ok, const std::string& detail) {
  std::printf("[%s] %-6s %s\n", ok ? "PASS" : "FAIL", name.c_str(), detail.c_str());
  (ok ? n_pass : n_fail)++;
}
static std::string fmt(const char* f, ...) {
  char b[512]; va_list ap; va_start(ap, f); std::vsnprintf(b, sizeof b, f, ap); va_end(ap); return b;
}

struct ChannelResult { double err_u, err_p, dpdx, flux_out, Pi; };
// Straight channel W=1000, L=6000: compare with the developed profile.
static ChannelResult straight_channel(double hw, double Da, double S, double Re = 0, bool verbose = false) {
  Boundary b = make_straight_channel(6000, 1000);
  Mesh m = Mesh::load(ensure_mesh(b, "h_straight", hw, hw), 1000);
  NSParams p; p.Da = Da; p.slip = S; p.Re = Re; p.verbose = verbose;
  NSSolver solver(m, p);
  Solution s = solver.solve();
  double G = channel_pressure_gradient(0.5, Da, S, 1.0);
  double eu = 0, nu = 0;
  for (int a = 0; a < m.nn(); ++a) {
    double ex = channel_profile(m.X[a].y, 0.5, Da, S, 1.0);
    eu = std::max(eu, std::fabs(s.u[a].x - ex) + std::fabs(s.u[a].y));
    nu = std::max(nu, ex);
  }
  // pressure: p = G (L - x) with outlet at 0
  double ep = 0;
  double L = 6.0;
  for (int v = 0; v < m.nv; ++v) ep = std::max(ep, std::fabs(s.p[v] - G * (L - m.X[v].x)));
  double Pi = mean_pressure(m, s, 0) - mean_pressure(m, s, 1);
  return {eu / nu, G > 0 ? ep / (G * L) : ep, (mean_pressure_at_x(m, s, 1.0) - mean_pressure_at_x(m, s, 5.0)) / 4.0,
          -inward_flux(m, s, 1), Pi};
}

static std::set<std::string> g_select;
static bool want(const std::string& name) { return g_select.empty() || g_select.count(name); }

int main(int argc, char** argv) {
  // usage: gd_hydro_tests [quick | H5,H7,...]
  bool quick = argc > 1 && std::string(argv[1]) == "quick";
  if (argc > 1 && !quick) { std::stringstream ss(argv[1]); std::string t; while (std::getline(ss, t, ',')) g_select.insert(t); }
  auto t0 = std::chrono::steady_clock::now();

  // H1: Poiseuille, no-slip, exact for P2.
  if (want("H1")) {
    auto r = straight_channel(50, 0, 0);
    report("H1", r.err_u < 1e-9 && r.err_p < 1e-9, fmt("Poiseuille no-slip: rel err u %.1e p %.1e, flux out %.10f", r.err_u, r.err_p, r.flux_out));
  }
  // H1b: Navier slip S = 0.5, 2, inf (plug) -- shifted parabola, still exact.
  if (want("H1")) for (double S : {0.5, 2.0, std::numeric_limits<double>::infinity()}) {
    auto r = straight_channel(50, 0, S);
    report("H1b", r.err_u < 1e-9 && r.err_p < 1e-9, fmt("slip S=%g: rel err u %.1e p %.1e, -dp/dx %.6f (exact %.6f)", S, r.err_u, r.err_p, r.dpdx, channel_pressure_gradient(0.5, 0, S, 1)));
  }
  // H2: Brinkman-Poiseuille convergence (cosh profile not in P2 space).
  if (want("H2")) {
    double Da = 10, S = 0.5;
    std::vector<double> hs = {100, 50, 25}, eu, ep;
    for (double h : hs) { auto r = straight_channel(h, Da, S); eu.push_back(r.err_u); ep.push_back(r.err_p); }
    double rate_u = std::log(eu[1] / eu[2]) / std::log(2), rate_p = std::log(ep[1] / ep[2]) / std::log(2);
    report("H2", rate_u > 2.5 && eu[2] < 1e-5, fmt("Brinkman Da=%g S=%g: u err %.2e %.2e %.2e (rate %.2f), p err %.2e %.2e %.2e (rate %.2f)", Da, S, eu[0], eu[1], eu[2], rate_u, ep[0], ep[1], ep[2], rate_p));
    auto r = straight_channel(50, 1.0, std::numeric_limits<double>::infinity());
    report("H2b", r.err_u < 1e-9, fmt("Brinkman Da=1 plug flow (exact): rel err u %.1e, -dp/dx %.6f (exact %.6f)", r.err_u, r.dpdx, channel_pressure_gradient(0.5, 1.0, INFINITY, 1)));
  }
  // H4: Kovasznay flow, Re = 40, Dirichlet everywhere, convergence.
  if (want("H4")) {
    double Re = 40, lam = Re / 2 - std::sqrt(Re * Re / 4 + 4 * kPi * kPi);
    auto uex = [&](Vec2 x) { return Vec2{1 - std::exp(lam * x.x) * std::cos(2 * kPi * x.y), lam / (2 * kPi) * std::exp(lam * x.x) * std::sin(2 * kPi * x.y)}; };
    // our pressure scaling: p_ours = Re * p_K
    auto pex = [&](Vec2 x) { return Re * 0.5 * (1 - std::exp(2 * lam * x.x)); };
    Polygon P; P.pts = {{-500, -500}, {1000, -500}, {1000, 500}, {-500, 500}};
    Boundary b({P}, end_plane_tagger(1e9));  // no contacts: all walls
    std::vector<double> hs = {100, 50, 25}, eu, ep;
    for (double h : hs) {
      Mesh m = Mesh::load(ensure_mesh(b, "h_kov", h, h), 1000);
      NSParams p; p.Re = Re; p.dirichlet_all = uex; p.pin_value = pex(m.X[0]);
      NSSolver solver(m, p);
      Solution s = solver.solve();
      double e2 = 0, A = 0, pe = 0;
      for (int a = 0; a < m.nn(); ++a) e2 = std::max(e2, (s.u[a] - uex(m.X[a])).norm());
      for (int v = 0; v < m.nv; ++v) pe = std::max(pe, std::fabs(s.p[v] - pex(m.X[v])));
      (void)A;
      eu.push_back(e2); ep.push_back(pe / Re);
      if (!s.converged) std::printf("  Kovasznay h=%g did not converge\n", h);
    }
    double rate_u = std::log(eu[1] / eu[2]) / std::log(2), rate_p = std::log(ep[1] / ep[2]) / std::log(2);
    report("H4", rate_u > 2.5 && eu[2] < 2e-4, fmt("Kovasznay Re=40: max u err %.2e %.2e %.2e (rate %.2f), p err %.2e %.2e %.2e (rate %.2f)", eu[0], eu[1], eu[2], rate_u, ep[0], ep[1], ep[2], rate_p));
  }
  // H3: Stokes reciprocity in Tesla geometries: D = 1.
  if (want("H3")) {
    TeslaParams t1; t1.n_stages = 1; t1.Lc = t1.Ld = 6000;
    Boundary b = make_tesla(t1);
    Mesh m = Mesh::load(ensure_mesh(b, "h_tesla1", 20, 120), b.contact_width(0));
    for (auto [Da, S] : std::vector<std::pair<double, double>>{{0.0, 0.0}, {2.0, 0.5}, {0.0, INFINITY}}) {
      NSParams p; p.Da = Da; p.slip = S; p.Re = 0;
      p.inlet = 0; p.outlet = 1; NSSolver sf(m, p); Solution f = sf.solve();
      p.inlet = 1; p.outlet = 0; NSSolver sb(m, p); Solution r = sb.solve();
      double Pf = mean_pressure(m, f, 0) - mean_pressure(m, f, 1);
      double Pr = mean_pressure(m, r, 1) - mean_pressure(m, r, 0);
      double D = Pr / Pf;
      report("H3", std::fabs(D - 1) < 1e-5 && f.converged && r.converged, fmt("Stokes reciprocity tesla1 Da=%g S=%g: Pi_f %.6f Pi_r %.6f D-1 = %.2e (flux out f %.6f r %.6f)", Da, S, Pf, Pr, D - 1, -inward_flux(m, f, 1), -inward_flux(m, r, 0)));
    }
  }
  // H8: symmetric constriction and straight channel at Re = 10: D = 1.
  if (want("H8")) {
    Boundary b = make_symmetric_constriction(2000, 6000, 30, 175, 500);
    Mesh m = Mesh::load(ensure_mesh(b, "h_symcon", 20, 120), b.contact_width(0));
    NSParams p; p.Da = 1.0; p.slip = 0.5; p.Re = 10;
    p.inlet = 0; p.outlet = 1; NSSolver sf(m, p); Solution f = sf.solve();
    p.inlet = 1; p.outlet = 0; NSSolver sb(m, p); Solution r = sb.solve();
    double Pf = mean_pressure(m, f, 0) - mean_pressure(m, f, 1);
    double Pr = mean_pressure(m, r, 1) - mean_pressure(m, r, 0);
    report("H8", std::fabs(Pr / Pf - 1) < 1e-4 && f.converged && r.converged, fmt("symmetric constriction Re=10: Pi_f %.6f Pi_r %.6f D-1 = %.2e", Pf, Pr, Pr / Pf - 1));
  }
  // Units cross-checks: Poiseuille R vs Gurzhi formula, Brinkman R vs Drude.
  if (want("U1")) {
    double n = 1e12, nu = 0.1, W = 1e-6, L = 1e-6;
    double R_pois = units::resistance_ohm(12 * L / W, W, n, nu);
    double gurzhi = 12 * units::m_star(n) * nu * L / (units::n_m2(n) * units::e * units::e * W * W * W);
    double tau = 5e-12, Da = units::gurzhi_Da(W, nu, tau);
    double R_ohm = units::resistance_ohm(Da * L / W, W, n, nu);  // plug Brinkman: Pi = Da * L/W
    double drude = L / (W * units::drude_sigma(n, tau));
    double sigma_kt = 2 * kE2OverH * units::kF(n) * (tau * units::vF);  // (2e^2/h) k_F lambda
    report("U1", std::fabs(R_pois / gurzhi - 1) < 1e-12 && std::fabs(R_ohm / drude - 1) < 1e-12 && std::fabs(sigma_kt / units::drude_sigma(n, tau) - 1) < 1e-6,
           fmt("units: R_Poiseuille %.2f ohm (Gurzhi %.2f), R_ohmic %.2f ohm (Drude %.2f, (2e^2/h)k_F l %.4e vs %.4e S), m* %.4f m_e, Da %.2f",
               R_pois, gurzhi, R_ohm, drude, sigma_kt, units::drude_sigma(n, tau), units::m_star(n) / units::m_e, Da));
  }
  // H7: mesh convergence of D - 1 on tesla1 (Da=1, S=0.5). At Re = 10 the
  // Tesla term is ~1e-2 and must converge to 10 %; at Re = 1 the four
  // refinements only bound the linear-in-Re coefficient (the sequence
  // -9e-5, -2.5e-5, -2.2e-5, +6e-6 changes sign), so H7b asks that every
  // level stay below 1e-4, i.e. no resolved O(Re) asymmetry.
  if (!quick && want("H7")) {
    TeslaParams t1; t1.n_stages = 1; t1.Lc = t1.Ld = 6000;
    Boundary b = make_tesla(t1);
    std::vector<std::pair<double, double>> hs = {{20, 120}, {14, 85}, {10, 60}};
    std::vector<double> D10, D1;
    std::string d10 = "D-1 at Re=10:", d1 = "D-1 at Re=1:";
    for (auto [hw, hm] : hs) {
      Mesh m = Mesh::load(ensure_mesh(b, "h_tesla1", hw, hm), b.contact_width(0));
      auto r = diodicity_sweep(m, 1.0, 0.5, {1.0, 3.0, 10.0});
      D1.push_back(r[0].D - 1); D10.push_back(r[2].D - 1);
      d10 += fmt(" [hw %g: %+.4e (Re 3: %+.3e), %d tris, Pi_f %.1f]", hw, r[2].D - 1, r[1].D - 1, (int)m.tri.size(), r[2].Pi_f);
      d1 += fmt(" [hw %g: %+.3e]", hw, r[0].D - 1);
    }
    double rel = std::fabs(D10[2] - D10[1]) / std::fabs(D10[2]);
    report("H7", rel < 0.10, d10 + fmt(" change last two %.1f%%", 100 * rel));
    bool small = true; for (double v : D1) small = small && std::fabs(v) < 1e-4;
    report("H7b", small, d1 + " (bound only: O(Re) term unresolved, below 1e-4 on every mesh)");
  }
  // H9: small-Re behaviour and mirror check. H9a only verifies that D - 1 is a
  // smooth, Re-proportional quantity on a given mesh (its continuum value is
  // below the mesh floor, see H7b); H9b bounds the solver's directional bias.
  if (!quick && want("H9")) {
    TeslaParams t1; t1.n_stages = 1; t1.Lc = t1.Ld = 6000;
    Boundary b = make_tesla(t1);
    Mesh m = Mesh::load(ensure_mesh(b, "h_tesla1", 20, 120), b.contact_width(0));
    auto r = diodicity_sweep(m, 1.0, 0.5, {0.05, 0.1, 0.2});
    double s1 = (r[0].D - 1) / 0.05, s2 = (r[1].D - 1) / 0.1, s3 = (r[2].D - 1) / 0.2;
    report("H9a", std::fabs(s2 / s1 - 1) < 0.05 && std::fabs(s3 / s1 - 1) < 0.1,
           fmt("tesla1 Da=1 S=0.5: (D-1)/Re = %.4e %.4e %.4e at Re 0.05 0.1 0.2", s1, s2, s3));
    // Mirror x -> L - x and re-mesh: forward there is reverse here.
    std::vector<Polygon> loops = b.loops();
    double L = b.bbox_max().x;
    for (auto& P : loops) for (auto& q : P.pts) q.x = L - q.x;
    Boundary bm(loops, end_plane_tagger(L));
    Mesh mm = Mesh::load(ensure_mesh(bm, "h_tesla1_mirror", 20, 120), bm.contact_width(0));
    auto rm = diodicity_sweep(mm, 1.0, 0.5, {0.2});
    double Dm = rm[0].D, D = r[2].D;
    // A directional solver bias b (inlet-profile vs do-nothing asymmetry) would
    // give D*D_mirror - 1 = 2b; require it below a quarter of the signal.
    double bias = 0.5 * (Dm * D - 1);
    report("H9b", std::fabs(bias) < 0.25 * std::fabs(D - 1), fmt("mirrored+remeshed tesla1 Re=0.2: D-1 %.3e vs 1/D_mirror-1 %.3e, directional bias %.1e", D - 1, 1 / Dm - 1, bias));
  }
  // H5: backward-facing step, expansion ratio 2. Literature Re = u_mean*2h/nu
  // (Armaly 1983, Erturk 2008); here the reference length is the inlet height
  // h, so Re_solver = Re_lit / 2. Reattachment from the sign change of the
  // near-wall streamwise velocity (zero wall shear).
  if (!quick && want("H5")) {
    Polygon P; P.pts = {{0, 0}, {2000, 0}, {2000, -1000}, {32000, -1000}, {32000, 1000}, {0, 1000}};
    Boundary b({P}, end_plane_tagger(32000));
    Mesh m = Mesh::load(ensure_mesh(b, "h_bfs", 20, 80), b.contact_width(0));
    struct Ref { double Re_lit, xr_lo, xr_hi; };
    // Consensus bands: Re=100: Armaly (digitised) 2.96, CFD 3.00; Re=200: Erturk 4.98, Armaly 4.90, CFD 4.85-4.95.
    std::vector<Ref> refs = {{100, 2.85, 3.05}, {200, 4.75, 5.10}};
    NSParams p; p.Da = 0; p.slip = 0; p.inlet = 0; p.outlet = 1;
    Solution prev; bool have = false;
    for (const auto& r : refs) {
      p.Re = r.Re_lit / 2;
      NSSolver solver(m, p);
      Solution s = solver.solve(have ? &prev : nullptr);
      if (s.converged) { prev = s; have = true; }
      double xr = -1, xprev = 2.05, uprev = -1;
      for (double x = 2.05; x < 20; x += 0.01) {
        Vec2 u;
        if (!sample_velocity(m, s, {x, -1 + 0.01}, u)) continue;
        if (x > 2.2 && uprev < 0 && u.x >= 0) { xr = xprev + (x - xprev) * (-uprev) / (u.x - uprev) - 2.0; break; }
        uprev = u.x; xprev = x;
      }
      report("H5", s.converged && xr > r.xr_lo && xr < r.xr_hi, fmt("backward-facing step Re_lit=%g: x_r/h = %.3f (literature %.2f-%.2f), Newton %s", r.Re_lit, xr, r.xr_lo, r.xr_hi, s.converged ? "converged" : "FAILED"));
    }
    write_vtk(m, prev, "results/bfs_Re200.vtk");
  }
  // H6: classical fluid Tesla valve (Da=0, no-slip): D rises above 1 with Re in
  // the laminar range. No fully specified 2D geometry with tabulated D(Re)
  // exists in the literature, so this is a trend and magnitude check against
  // single/few-stage values (D ~ 1.01-1.05 at Re 50-100, 1.1-1.3 at Re 200).
  // Steady Newton with Re continuation fails between Re = 100 and 150 in this
  // geometry (see results/HYDRO.md); graphene cannot reach it, so the check
  // stops at 100.
  if (!quick && want("H6")) {
    TeslaParams t1; t1.n_stages = 1; t1.Lc = t1.Ld = 6000;
    Boundary b = make_tesla(t1);
    Mesh m = Mesh::load(ensure_mesh(b, "h_tesla1", 20, 120), b.contact_width(0));
    auto r = diodicity_sweep(m, 0.0, 0.0, {25, 50, 100});
    std::string detail = "classical tesla1 (Da=0, no-slip) D:";
    bool mono = true, ok = true;
    for (size_t i = 0; i < r.size(); ++i) {
      detail += fmt(" Re %g: %.4f%s", r[i].Re, r[i].D, r[i].ok ? "" : "(nc)");
      if (i > 0 && r[i].D <= r[i - 1].D) mono = false;
      ok = ok && r[i].ok;
    }
    bool mag = r[2].D > 1.01 && r[2].D < 1.06;
    report("H6", ok && mono && mag, detail + (mono ? " (monotonic)" : " (NOT monotonic)"));
  }
  double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  std::printf("\n%d passed, %d failed (%.1f s)\n", n_pass, n_fail, secs);
  return n_fail ? 1 : 0;
}
