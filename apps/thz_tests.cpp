// Acceptance ladder for the Stage H2 solvers (plans/hydro_thz.md, sec. 3), T1-T9,
// the Stage H3 large-signal ladder (plans/hydro_largesignal.md, sec. 4), T10-T14,
// and the Stage H4 operating-point ladder (plans/hydro_density.md, sec. 4), T15-T17.
#include "dc_gate_common.hpp"
#include "hydro/diode.hpp"
#include "generators.hpp"
#include "thz_geo.hpp"
#include "thz_power_common.hpp"
#include "hydro/ds_reference.hpp"
#include "hydro/mesh.hpp"
#include "hydro/post.hpp"
#include "hydro/thz_solver.hpp"
#include "hydro/thz_time.hpp"
#include "hydro/thz_units.hpp"
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
  std::fflush(stdout);
  (ok ? n_pass : n_fail)++;
}
static std::string fmt(const char* f, ...) {
  char b[1024]; va_list ap; va_start(ap, f); std::vsnprintf(b, sizeof b, f, ap); va_end(ap); return b;
}
static std::set<std::string> g_select;
static bool want(const std::string& name) { return g_select.empty() || g_select.count(name); }
static double g_hw = 50;   // channel benchmark mesh size, nm on a 1000 nm channel (--hw)
static int g_order = 1;    // density order for the wave problem in the channel tests (--order)

// Uniform channel of length L (units of its width), perfect slip, shallow water.
static Mesh channel_mesh(double L, double hw) {
  Boundary b = make_straight_channel(L * 1000, 1000);
  return Mesh::load(ensure_mesh(b, fmt("t_chan%g", L), hw, hw), 1000);
}
static ThzParams ds_params(double Gamma, double nu, double Va = 1.0) {
  ThzParams p;
  p.Gamma = Gamma; p.nu = nu; p.closure.kind = Closure::ShallowWater;
  p.ports = {Port::thevenin(0, Va, 0.0, 0.0), Port::open(1)};  // ideal ac source, dc grounded; open drain
  return p;
}

int main(int argc, char** argv) {
  // usage: gd_thz_tests [quick | T2,T4,...]
  bool quick = argc > 1 && std::string(argv[1]) == "quick";
  if (argc > 1 && !quick && argv[1][0] != '-') { std::stringstream ss(argv[1]); std::string t; while (std::getline(ss, t, ',')) g_select.insert(t); }
  for (int i = 1; i + 1 < argc; ++i) { if (std::string(argv[i]) == "--hw") g_hw = std::atof(argv[i + 1]); if (std::string(argv[i]) == "--order") g_order = std::atoi(argv[i + 1]); }
  auto t0 = std::chrono::steady_clock::now();
  const double L = 10.0;

  // T2: linear response of the uniform channel against n1(x) = Va cos(k(L-x))/cos(kL),
  //     k^2 = Omega^2 - i Omega Gamma (inviscid), at frequencies on and off resonance.
  if (want("T2")) {
    Mesh m = channel_mesh(L, g_hw);
    ThzParams p = ds_params(0.2, 0.0);
    ThzSolver s(m, p, g_order);
    double worst = 0; std::string det;
    for (double Om : {0.05, 0.157, 0.3, 0.7, 1.1}) {   // 0.157 = pi/(2L), first resonance
      FirstOrder f = s.solve_linear(Om);
      cd k = std::sqrt(cd(Om * Om, -Om * p.Gamma));
      cd ex = 1.0 / std::cos(k * L);
      double err = std::abs(f.V1[1] - ex) / std::abs(ex);
      worst = std::max(worst, err);
      det += fmt(" [Om %.3f: |V_d| %.4f exact %.4f err %.1e]", Om, std::abs(f.V1[1]), std::abs(ex), err);
    }
    report("T2", worst < 2e-3, "plasmon response vs 1D closed form, Gamma 0.2:" + det);
  }

  // T3: damping. Resonance half-width of |V_d(Omega)| near the first mode with
  //     friction only (width Gamma/2), then with viscosity (dispersion
  //     k^2 = (Om^2 - i Om Gamma)/(1 + i Om nu)) and the viscoelastic limit.
  if (want("T3")) {
    Mesh m = channel_mesh(L, g_hw);
    auto peak = [&](ThzParams p, double& Om_pk, double& width) {
      ThzSolver s(m, p);
      // scan, find max of |V_d|, then half-power points by bisection on a fine grid
      // coarse scan, then the half-power points by linear interpolation
      std::vector<double> Oms, vals;
      for (double Om = 0.10; Om <= 0.22; Om += 0.002) { Oms.push_back(Om); vals.push_back(std::abs(s.solve_linear(Om).V1[1])); }
      size_t im = std::max_element(vals.begin(), vals.end()) - vals.begin();
      // refine the peak with a parabola through three points
      if (im > 0 && im + 1 < vals.size()) {
        double y0 = vals[im - 1], y1 = vals[im], y2 = vals[im + 1];
        Om_pk = Oms[im] + 0.002 * 0.5 * (y0 - y2) / (y0 - 2 * y1 + y2);
      } else Om_pk = Oms[im];
      double half = vals[im] / std::sqrt(2.0);
      size_t lo = im, hi = im;
      while (lo > 0 && vals[lo] > half) --lo;
      while (hi + 1 < vals.size() && vals[hi] > half) ++hi;
      auto interp = [&](size_t a, size_t b) {   // Om where |V| crosses half between samples a and b
        return Oms[a] + (half - vals[a]) * (Oms[b] - Oms[a]) / (vals[b] - vals[a]);
      };
      width = interp(hi - 1, hi) - interp(lo, lo + 1);
    };
    double Om1, w1, Om2, w2, Om3, w3;
    peak(ds_params(0.02, 0.0), Om1, w1);
    peak(ds_params(0.02, 0.5), Om2, w2);          // viscous: extra damping nu k^2 at k = pi/2L
    ThzParams pv = ds_params(0.02, 0.5); pv.tau_ee = 50.0;   // Omega tau_ee ~ 8: viscosity turns reactive
    peak(pv, Om3, w3);
    double k1 = M_PI / (2 * L);
    double w1_ex = 0.02, w2_ex = 0.02 + 0.5 * k1 * k1;   // half-power full width = Gamma_eff
    // with Omega tau_ee = 8 the Maxwell viscosity is reactive: the width must return to the friction-only value
    bool ok = std::fabs(Om1 - k1) < 0.003 && std::fabs(w1 - w1_ex) / w1_ex < 0.05 &&
              std::fabs(w2 - w2_ex) / w2_ex < 0.05 && std::fabs(w3 - w1) / w1 < 0.1;
    report("T3", ok, fmt("first mode at Om %.4f (exact %.4f); width friction %.4f (exact %.4f), +viscosity %.4f "
                         "(exact %.4f), viscoelastic Om tau_ee=8: %.4f (back to the friction width)",
                         Om1, k1, w1, w1_ex, w2, w2_ex, w3));
  }

  // T4: Dyakonov-Shur dc photoresponse, friction only, against the closed form
  //     and the independent quadrature of the analytic first-order fields.
  if (want("T4")) {
    Mesh m = channel_mesh(L, g_hw);
    double worst_cf = 0, worst_q = 0; std::string det;
    for (double Gamma : {1.0, 0.1}) {
      ThzParams p = ds_params(Gamma, 0.0);
      ThzSolver s(m, p);
      for (double Om : {0.1, 0.3, 1.0, 3.0}) {
        FirstOrder f = s.solve_linear(Om);
        SecondOrder d = s.solve_dc(f);
        double fem = d.V20[1], cf = ds::closed_form(Om, Gamma, L), qd = ds::quadrature(Om, Gamma, L);
        double ecf = std::fabs(fem - cf) / std::fabs(cf), eq = std::fabs(fem - qd) / std::fabs(qd);
        worst_cf = std::max(worst_cf, ecf); worst_q = std::max(worst_q, eq);
        det += fmt("\n         Gamma %.1f Om %.1f: FEM %.5f DS %.5f quad %.5f (err %.1e / %.1e)", Gamma, Om, fem, cf, qd, ecf, eq);
      }
    }
    report("T4", worst_cf < 1e-2 && worst_q < 1e-2, "DS photoresponse dU/Ua^2 (closed form / quadrature):" + det);
  }

  // T1: static limit. Steady nonlinear H2 solver with a dc current source at
  //     Mach M = 0.01 and Re_s = 100 (so Re = M Re_s = 1) against Stage H's
  //     Pi(Re = 1) on the same mesh: Delta h Re_s / M = Pi to O(M^2).
  if (!quick && want("T1")) {
    TeslaParams t1; t1.n_stages = 1; t1.Lc = t1.Ld = 6000;
    Boundary b = make_tesla(t1);
    Mesh m = Mesh::load(ensure_mesh(b, "h_tesla1", 20, 120), b.contact_width(0));
    // Pi ~ 1.6e4 here, so the density drop Pi nu M must stay small: M = 2.5e-4 gives 1e-3.
    const double Res = 4000, M = 2.5e-4;
    ThzParams p; p.Gamma = 0; p.nu = 1 / Res; p.closure.kind = Closure::ShallowWater; p.slip = 0;
    Port src; src.tag = 0; src.dc = Port::Kind::Current; src.Idc = M;
    p.ports = {src, Port::ground(1)};
    ThzTimeSolver ts(m, p, 1);
    ThzState st = ts.solve_steady(1e-11);
    double Pi_h2 = st.V[0] * Res / M;
    NSParams np_; np_.Re = M * Res; np_.Da = 0; np_.slip = 0;
    NSSolver ns(m, np_);
    Solution sh = ns.solve();
    double Pi_h = (mean_pressure(m, sh, 0) - mean_pressure(m, sh, 1)) / inward_flux(m, sh, 0);
    double err = std::fabs(Pi_h2 - Pi_h) / Pi_h;
    report("T1", st.t >= 0 && sh.converged && err < 1e-3,
           fmt("tesla1 no-slip, Re 1: Pi from H2 steady %.6f vs Stage H %.6f (rel diff %.1e, M^2 = %.0e)", Pi_h2, Pi_h, err, M * M));
  }

  // T5: quasi-static limit of the perturbation solver on the funnel against the
  //     curvature of the steady nonlinear characteristic: with V(I) = R I + b I^2
  //     the short-circuit dc current under an ideal ac voltage source is -b Va^2 / (2 R^3).
  if (!quick && want("T5")) {
    FunnelParams fp; fp.Lc = 6000; fp.Ld = 6000;
    Boundary b = make_funnel(fp);
    Mesh m = Mesh::load(ensure_mesh(b, "h_funnel", 20, 120), 2000);
    const double Res = 100;
    ThzParams p; p.Gamma = 0.2; p.nu = 1 / Res; p.closure.kind = Closure::ShallowWater;
    // curvature b of V(I) = R I + b I^2 at two small amplitudes, Richardson-extrapolated to I -> 0
    auto curvature = [&](double I0, double& R, double& bq) {
      double Vp, Vm;
      for (int sgn : {+1, -1}) {
        ThzParams ps = p;
        Port src; src.tag = 0; src.dc = Port::Kind::Current; src.Idc = sgn * I0;
        ps.ports = {src, Port::ground(1)};
        ThzTimeSolver ts(m, ps, 1);
        ThzState st = ts.solve_steady(1e-12);
        (sgn > 0 ? Vp : Vm) = st.V[0];
      }
      R = (Vp - Vm) / (2 * I0); bq = (Vp + Vm) / (2 * I0 * I0);
    };
    double R1, b1, R2, b2;
    curvature(0.004, R1, b1); curvature(0.008, R2, b2);
    double R = (4 * R1 - R2) / 3, bq = (4 * b1 - b2) / 3;
    double pred = -bq / (2 * R * R * R);          // I_sc / Va^2
    ThzParams pa = p; pa.ports = {Port::thevenin(0, 1.0, 0.0, 0.0), Port::ground(1)};
    std::string det;
    double best = 1e9;
    for (int order : {1, 2}) {
      ThzSolver s(m, pa, order);
      for (double Om : {0.01, 0.02}) {
        FirstOrder f = s.solve_linear(Om);
        SecondOrder d = s.solve_dc(f);
        double err = std::fabs(d.I20[0] - pred) / std::fabs(pred);
        if (order == 1) best = std::min(best, err);
        det += fmt(" [P%d Om %.2f: %.4e]", order, Om, d.I20[0]);
      }
    }
    report("T5", best < 0.03, fmt("funnel quasi-static I_sc/Va^2: steady curvature %.4e (R %.3f, b %.2f, b at 0.004/0.008: %.2f/%.2f); perturbation", pred, R, bq, b1, b2) + det);
  }

  // T6: time domain against the perturbation solver, DS channel, Gamma 1, Om 1,
  //     and the Va^2 scaling of the dc response.
  if (!quick && want("T6")) {
    Mesh m = channel_mesh(L, g_hw);
    ThzParams p = ds_params(1.0, 0.02);
    ThzSolver s(m, p);
    FirstOrder f = s.solve_linear(1.0);
    SecondOrder d = s.solve_dc(f);
    double pert = d.V20[1];
    std::string det; bool ok = true;
    for (double Va : {0.01, 0.03}) {
      ThzParams pt = ds_params(1.0, 0.02, Va);
      ThzTimeSolver ts(m, pt, 1);
      TimeTrace tr = ts.run(1.0, 48, 60, 1e-11);   // dc relaxes diffusively, tau ~ Gamma (2L/pi)^2 = 40
      double ratio = tr.V_dc[1] / (Va * Va * pert);
      ok = ok && tr.ok && std::fabs(ratio - 1) < 0.02 + 2 * Va;   // allow the O(Va) cubic correction
      det += fmt(" [Va %.2f: time-domain V_dc %.4e, perturbation %.4e, ratio %.4f%s]", Va, tr.V_dc[1], Va * Va * pert, ratio, tr.ok ? "" : " (nc)");
    }
    report("T6", ok, "BDF2 vs second-order perturbation, Om 1, Gamma 1, nu 0.02:" + det);
  }

  // T7: symmetry floor. With a mirror-symmetric termination (antenna split
  //     +-Va/2 with R/2 at each contact, the gate at the midpoint potential) a
  //     symmetric constriction and a uniform channel must give zero dc current;
  //     the funnel sets the scale. (An ac source at one end and ground at the
  //     other is itself the Dyakonov-Shur asymmetry and rectifies any channel.)
  if (want("T7")) {
    Boundary b = make_symmetric_constriction(2000, 6000, 30, 175, 500);
    Mesh m = Mesh::load(ensure_mesh(b, "h_symcon", 20, 120), 2000);
    FunnelParams fp; fp.Lc = 6000; fp.Ld = 6000;
    Boundary bf = make_funnel(fp);
    Mesh mf = Mesh::load(ensure_mesh(bf, "h_funnel", 20, 120), 2000);
    Mesh mc = channel_mesh(L, g_hw);
    ThzParams p; p.Gamma = 0.5; p.nu = 0.01; p.closure.q = 0.1;
    p.ports = {Port::thevenin(0, 0.5, 0.25, 0.0), Port::thevenin(1, -0.5, 0.25, 0.0)};
    double Isym, Ifun, Ichan, Ids;
    { ThzSolver s(m, p); FirstOrder f = s.solve_linear(1.0); Isym = s.solve_dc(f).I20[0]; }
    { ThzSolver s(mf, p); FirstOrder f = s.solve_linear(1.0); Ifun = s.solve_dc(f).I20[0]; }
    { ThzSolver s(mc, p); FirstOrder f = s.solve_linear(1.0); Ichan = s.solve_dc(f).I20[0]; }
    ThzParams pds = p; pds.ports = {Port::thevenin(0, 1.0, 0.5, 0.0), Port::ground(1)};
    { ThzSolver s(m, pds); FirstOrder f = s.solve_linear(1.0); Ids = s.solve_dc(f).I20[0]; }
    bool ok = std::fabs(Isym) < 1e-3 * std::fabs(Ifun) && std::fabs(Ichan) < 1e-3 * std::fabs(Ifun);
    report("T7", ok, fmt("dc short-circuit current per Va^2, symmetric termination: constriction %.2e, uniform channel %.2e, "
                         "funnel %.3e; the same constriction with a DS termination (source R, drain ground): %.3e",
                         Isym, Ichan, Ifun, Ids));
  }

  // T8: mesh convergence of the funnel response at Omega 1 and 3, refining wall
  //     and bulk sizes together: (hw, hm) = (28, 168), (20, 120), (14, 84) nm.
  if (!quick && want("T8")) {
    FunnelParams fp; fp.Lc = 6000; fp.Ld = 6000;
    Boundary bf = make_funnel(fp);
    ThzParams p; p.Gamma = 0.5; p.nu = 0.01; p.closure.q = 0.1;
    p.ports = {Port::thevenin(0, 0.5, 0.25, 0.0), Port::thevenin(1, -0.5, 0.25, 0.0)};
    std::string det; double worst = 0;
    for (double Om : {1.0, 3.0}) {
      double vals[3]; int i = 0;
      for (double hw : {28.0, 20.0, 14.0}) {
        Mesh mf = Mesh::load(ensure_mesh(bf, "h_funnel", hw, 6 * hw), 2000);
        ThzSolver s(mf, p); FirstOrder f = s.solve_linear(Om); vals[i++] = s.solve_dc(f).I20[0];
      }
      double ch = std::fabs(vals[2] - vals[1]) / std::fabs(vals[2]);
      worst = std::max(worst, ch);
      det += fmt(" [Om %.0f: %.5e %.5e %.5e, last change %.1e]", Om, vals[0], vals[1], vals[2], ch);
    }
    report("T8", worst < 0.02, "funnel I_sc/Va^2 vs mesh (hw, hm) = (28,168), (20,120), (14,84) nm:" + det);
  }

  // T9: units. With the shallow-water closure the voltage unit equals the gate
  //     overdrive U_0 = e n / C_g, so the DS broadband response Va^2/4 maps to
  //     Ua^2/(4 U_0); and the matched nonresonant voltage responsivity is
  //     2 R_ant / U_0 (available power Ua^2/8R, response Ua^2/4U_0).
  if (want("T9")) {
    ThzUnits u = ThzUnits::make(1e12, 30e-9, 3.8, 2e-6, 0.1, 5e-12, 1e-12);
    double Ua = 1e-3, R = 75;                       // 1 mV amplitude, 75 ohm antenna
    double Va = Ua / u.V_unit;
    double dU_nd = Va * Va / 4;                     // DS f = 1 limit, nondimensional
    double dU_V = dU_nd * u.V_unit, dU_ref = Ua * Ua / (4 * u.U0);
    // shallow-water V_unit: m0 s0^2 / e with s0^2 = e U0 / m0  ->  U0 exactly
    double Vunit_sw = u.m0 * (units::e * u.U0 / u.m0) / units::e;
    double RV = u.volt_per_watt(0.25, R), RV_ref = 2 * R / u.U0;
    bool ok = std::fabs(Vunit_sw - u.U0) / u.U0 < 1e-12 && std::fabs(RV - RV_ref * (Vunit_sw / u.V_unit)) / RV < 1e-9;
    report("T9", ok, fmt("n 1e12, d 30 nm: s0 %.2e m/s (q %.3f), U0 %.3f V, R_unit %.1f ohm, f_unit %.3f THz, I_unit %.3e A; "
                         "DS dU %.3e V vs Ua^2/4U0 %.3e V (graphene V_unit %.3f V); R_V(f=1, 75 ohm) %.1f V/W",
                         u.s0, u.q, u.U0, u.R_unit, u.f_unit / 1e12, u.I_unit, dU_V, dU_ref, u.V_unit, RV));
  }

  // Stage H4 ladder (plans/hydro_density.md, sec. 4), quick: a contact
  // resistance in series with every port is bookkeeping only (T15); the
  // physical layer enters the solver only through the dimensionless groups
  // (T16); the units at the second operating point (T17).
  if (want("T15") || want("T16")) {
    ThzGeo G = make_thz_geo("funnel_w1000");
    Mesh m = Mesh::load(ensure_mesh(G.b, G.mesh_name, 160, 960), G.W_ref);
    // dss termination with R_c at both contacts, as gd_thz_sweep builds it
    auto ports = [](double R, double Rc) { return std::vector<Port>{Port::thevenin(0, 1.0, R + Rc, Rc), Port::thevenin(1, 0.0, Rc, Rc)}; };
    struct Resp { double Rdc, Isc, Voc, Pin_Pav; cd Z, Zant, Vs, Vd, Is, Id; double R, Rc; };
    auto respond = [&](const ThzUnits& u, double Rant_ohm, double Rc_ohm, double f) {
      ThzParams p; p.Gamma = u.Gamma; p.nu = u.inv_Res; p.tau_ee = u.t_ee; p.closure.q = u.q;
      double R = u.R_nd(Rant_ohm), Rc = u.R_nd(Rc_ohm), Om = u.Omega(f);
      p.ports = ports(R, Rc);
      ThzSolver s(m, p);
      Resp r; r.Rdc = s.dc_resistance(0, 1);
      FirstOrder fo = s.solve_linear(Om);
      r.Z = (fo.V1[0] - fo.V1[1]) / fo.I1[0];
      r.Zant = (1.0 - R * fo.I1[0]) / fo.I1[0];   // what the antenna sees: (Va - R I_s) / I_s
      r.Vs = fo.V1[0]; r.Vd = fo.V1[1]; r.Is = fo.I1[0]; r.Id = fo.I1[1]; r.R = R; r.Rc = Rc;
      r.Pin_Pav = (fo.power_in(0) + fo.power_in(1)) * 8 * R;
      SecondOrder sc = s.solve_dc(fo); r.Isc = sc.I20[0];
      ThzParams po = p; po.ports[0].dc = Port::Kind::Open; ThzSolver so(m, po);
      SecondOrder oc = so.solve_dc(fo); r.Voc = oc.V20[0] - oc.V20[1];
      return r;
    };
    ThzUnits uA = ThzUnits::make(1e12, 30e-9, 3.8, 4e-6, 0.1, 5e-12, 1e-12);
    if (want("T15")) {
      const double Rc_ohm = 300.0 / 4.0;   // 300 ohm um over the 4 um contact
      Resp a = respond(uA, 75, Rc_ohm, 3e11), b = respond(uA, 75, 0.0, 3e11);
      // the R_c = 0 construction must equal the S2 ports exactly
      ThzParams p0; p0.Gamma = uA.Gamma; p0.nu = uA.inv_Res; p0.tau_ee = uA.t_ee; p0.closure.q = uA.q;
      p0.ports = {Port::thevenin(0, 1.0, uA.R_nd(75), 0.0), Port::ground(1)};
      ThzSolver s0(m, p0); FirstOrder fo0 = s0.solve_linear(uA.Omega(3e11)); SecondOrder sc0 = s0.solve_dc(fo0);
      double Rc = uA.R_nd(Rc_ohm);
      double thev = std::fabs(a.Isc * (a.Rdc + 2 * Rc) + a.Voc) / std::fabs(a.Voc);   // I_sc (R_dc + 2 R_c) = -V_oc
      double thev0 = std::fabs(b.Isc * b.Rdc + b.Voc) / std::fabs(b.Voc);
      double rep = std::fabs(b.Isc - sc0.I20[0]) / std::fabs(sc0.I20[0]);
      // port relations with R_c in series: V_s = Va - (R + R_c) I_s and V_d = -R_c I_d. The ac drain
      // current is not -I_s (the gate carries displacement current), so the antenna sees
      // Z_ant = (Va - R I_s) / I_s rather than Z_dev + 2 R_c.
      double prel = std::max(std::abs(a.Vs - (1.0 - (a.R + a.Rc) * a.Is)), std::abs(a.Vd + a.Rc * a.Id));
      double zant = std::abs(a.Zant - (a.Z + a.Rc - a.Rc * a.Id / a.Is)) / std::abs(a.Zant);
      double imb = std::abs(a.Id + a.Is) / std::abs(a.Is);
      bool ok = thev < 1e-3 && thev0 < 1e-3 && rep < 1e-6 && prel < 1e-9 && zant < 1e-9 && a.Pin_Pav <= 1.0 + 1e-9 && b.Pin_Pav <= 1.0 + 1e-9 &&
                a.Pin_Pav < b.Pin_Pav && std::fabs(a.Rdc - b.Rdc) < 1e-12;
      report("T15", ok, fmt("contact resistance 300 ohm um (%.1f ohm = %.4f R_unit) at (1e12, 30 nm) 100 K 0.3 THz dss: I_sc (R_dc + 2 R_c) = -V_oc to %.1e "
                            "(R_c = 0: %.1e); R_c = 0 reproduces the S2 ports to %.1e; port relations V_s = Va - (R + R_c) I_s, V_d = -R_c I_d to %.1e; "
                            "Z_ant = Z_dev + R_c (1 - I_d / I_s) to %.1e (ac |I_d + I_s| / |I_s| = %.3f: gate displacement current); "
                            "P_in/P_av %.4f (R_c = 0: %.4f); I_sc %.4e -> %.4e, V_oc %.4e -> %.4e", Rc_ohm, Rc, thev, thev0, rep, prel, zant, imb,
                            a.Pin_Pav, b.Pin_Pav, b.Isc, a.Isc, b.Voc, a.Voc));
    }
    if (want("T16")) {
      // B: 4 n, d / 2 (same sqrt(n) d, so the same s0 and q; R_unit halves); C: W, tau, nu doubled
      // (same Gamma, 1/Re_s, q; f_unit and R_unit halve). Antenna and frequency co-scaled.
      ThzUnits uB = ThzUnits::make(4e12, 15e-9, 3.8, 4e-6, 0.1, 5e-12, 1e-12);
      ThzUnits uC = ThzUnits::make(1e12, 30e-9, 3.8, 8e-6, 0.2, 10e-12, 2e-12);
      auto groups = [](const ThzUnits& u, double Rant, double f) {
        return std::vector<double>{u.Gamma, u.q, u.inv_Res, u.t_ee, u.Omega(f), u.R_nd(Rant)};
      };
      double fA = 3e11, RA = 75;
      double fB = fA * uB.f_unit / uA.f_unit, RB = RA * uB.R_unit / uA.R_unit;
      double fC = fA * uC.f_unit / uA.f_unit, RC = RA * uC.R_unit / uA.R_unit;
      auto gA = groups(uA, RA, fA), gB = groups(uB, RB, fB), gC = groups(uC, RC, fC);
      double gerr = 0;
      for (size_t i = 0; i < gA.size(); ++i) gerr = std::max({gerr, std::fabs(gB[i] - gA[i]) / std::fabs(gA[i]), std::fabs(gC[i] - gA[i]) / std::fabs(gA[i])});
      Resp a = respond(uA, RA, 0, fA), b = respond(uB, RB, 0, fB), c = respond(uC, RC, 0, fC);
      auto rel = [](double x, double y) { return std::fabs(x - y) / std::fabs(y); };
      double nd = std::max({rel(b.Isc, a.Isc), rel(c.Isc, a.Isc), rel(b.Voc, a.Voc), rel(c.Voc, a.Voc), std::abs(b.Z - a.Z) / std::abs(a.Z),
                            std::abs(c.Z - a.Z) / std::abs(a.Z), rel(b.Rdc, a.Rdc), rel(c.Rdc, a.Rdc)});
      // physical responsivities scale as I_unit R_ant / V_unit^2 and V_unit R_ant / V_unit^2
      double RIA = uA.amp_per_watt(a.Isc, RA), RIB = uB.amp_per_watt(b.Isc, RB), RIC = uC.amp_per_watt(c.Isc, RC);
      double sB = (uB.I_unit / uA.I_unit) * (RB / RA) * std::pow(uA.V_unit / uB.V_unit, 2);
      double sC = (uC.I_unit / uA.I_unit) * (RC / RA) * std::pow(uA.V_unit / uC.V_unit, 2);
      double ph = std::max(rel(RIB / RIA, sB), rel(RIC / RIA, sC));
      bool ok = gerr < 1e-9 && nd < 1e-6 && ph < 1e-6;
      report("T16", ok, fmt("group invariance: (4e12, 15 nm) and (1e12, 30 nm, W tau nu x2) co-scaled to (1e12, 30 nm, 75 ohm, 0.3 THz): "
                            "groups equal to %.1e, nondimensional I_sc, V_oc, Z, R_dc equal to %.1e, R_I scales with I_unit R_ant / V_unit^2 to %.1e "
                            "(R_I %.3f, %.3f, %.3f A/W at R_ant %.1f, %.1f, %.1f ohm)", gerr, nd, ph, RIA, RIB, RIC, RA, RB, RC));
    }
  }

  // T17: units at the second operating point (5e11, 10 nm): closed forms and
  //      hand values; then the DS response of the uniform channel with the
  //      graphene closure at this q against the classical Ua^2 f / (4 U_0).
  if (want("T17")) {
    ThzUnits u = ThzUnits::make(5e11, 10e-9, 3.8, 4e-6, 0.1, 5e-12, 1e-12);
    using namespace units;
    double EF_meV = EF_J(5e11) / e * 1e3;
    auto rel = [](double x, double y) { return std::fabs(x - y) / std::fabs(y); };
    // closed forms: V_unit = U0 / (1 - q), R_unit = m0 s0 / (e^2 n0 W), f_unit = s0 / (2 pi W)
    double cf = std::max({rel(u.V_unit, u.U0 / (1 - u.q)), rel(u.R_unit, u.m0 * u.s0 / (e * e * u.n0 * 4e-6)), rel(u.f_unit, u.s0 / (2 * M_PI * 4e-6)),
                          rel(u.U0, e * u.n0 * 10e-9 / (8.8541878128e-12 * 3.8))});
    // hand values (python, 2026-11-08): s0 1.8402e6 m/s, q 0.1477, U0 0.2381 V, V_unit 0.2793 V, R_unit 47.37 ohm, f_unit 73.22 GHz, E_F 82.49 meV
    double hv = std::max({rel(u.s0, 1.8402e6), rel(u.q, 0.1477), rel(u.U0, 0.2381), rel(u.V_unit, 0.2793), rel(u.R_unit, 47.37),
                          rel(u.f_unit, 73.22e9), rel(EF_meV, 82.49)});
    // DS response of the channel, graphene closure at this q vs shallow water, friction only, Gamma 1, Omega 3 (as T4)
    Mesh m = channel_mesh(L, g_hw);
    ThzParams psw = ds_params(1.0, 0.0), pgr = psw; pgr.closure.kind = Closure::Graphene; pgr.closure.q = u.q;
    ThzSolver ssw(m, psw), sgr(m, pgr);
    FirstOrder fsw = ssw.solve_linear(3.0), fgr = sgr.solve_linear(3.0);
    double dU_sw = ssw.solve_dc(fsw).V20[1], dU_gr = sgr.solve_dc(fgr).V20[1];
    // in volts: graphene dU_gr V_unit vs classical dU_sw U0 (shallow water V_unit = U0)
    double ratio = dU_gr * u.V_unit / (dU_sw * u.U0);
    bool ok = cf < 1e-9 && hv < 1e-3 && std::fabs(ratio - 1) < 0.1;
    report("T17", ok, fmt("units at (5e11, 10 nm): E_F %.1f meV, s0 %.4e m/s, q %.4f, U0 %.4f V, V_unit %.4f V, R_unit %.2f ohm, f_unit %.2f GHz; "
                          "closed forms to %.1e, hand values to %.1e; channel DS dU at Gamma 1 Omega 3: graphene %.5f V_unit vs shallow water %.5f U0, "
                          "ratio in volts %.4f", EF_meV, u.s0, u.q, u.U0, u.V_unit, u.R_unit, u.f_unit / 1e9, cf, hv, dU_gr, dU_sw, ratio));
  }

  // Stage H3 ladder (plans/hydro_largesignal.md, sec. 4): 1 um funnel, 100 K,
  // dss termination with an ideal bias tee, 0.3 THz, on a coarse mesh
  // (hw 160, hm 960 nm). Runs start from the perturbative state. The time-domain
  // efficiency must meet the perturbation asymptote at small amplitude (T10), the
  // dc Thevenin correction must reproduce the short- and open-circuit limits
  // from a deliberately wrong bias seed (T11), the largest amplitude must be
  // converged in the time step (T12), and the power bookkeeping must be
  // passive and match the linear absorbed power (T13). T14 repeats T10 for the
  // sym termination, whose drain also has a bias tee (R/2 ac, short dc).
  if (!quick && (want("T10") || want("T11") || want("T12") || want("T13") || want("T14"))) {
    ThzGeo G = make_thz_geo("funnel_w1000");
    Mesh m = Mesh::load(ensure_mesh(G.b, G.mesh_name, 160, 960), G.W_ref);
    ThzUnits u = ThzUnits::make(1e12, 30e-9, 3.8, 4e-6, 0.1, 5e-12, 1e-12);
    ThzParams p; p.Gamma = u.Gamma; p.nu = u.inv_Res; p.tau_ee = 0; p.closure.q = u.q;   // Newtonian, as the time solver
    const double R = u.R_nd(75), Om = u.Omega(3e11), Tp = 2 * M_PI / Om, Pav1 = 1.0 / (8 * R);
    auto ports = [&](double Va, double RL) { return std::vector<Port>{Port::thevenin(0, Va, R, RL), Port::ground(1)}; };
    p.ports = ports(1.0, 0.0);
    ThzSolver s0(m, p);
    const double Rdc = s0.dc_resistance(0, 1);
    LsOptions o; o.Rdc = Rdc; o.first = (int)std::ceil(std::max(3 * 2 / u.Gamma, 10.0) / Tp); o.periods = o.first + 3;
    std::printf("  H3 ladder mesh: %d nodes, Rdc %.4f, Omega %.4f, %d periods\n", m.nn(), Rdc, Om, o.periods);
    // continuous (fo, sc) and BDF2-discrete (fod, scd) perturbation solutions
    struct Small { FirstOrder fo, fod; SecondOrder sc, scd; double Pin_lin; };
    auto small = [&](double RL, int steps) {
      ThzParams pl = p; pl.ports = ports(1.0, RL); ThzSolver sl(m, pl);
      Small r; r.fo = sl.solve_linear(Om); r.sc = sl.solve_dc(r.fo); r.Pin_lin = (r.fo.power_in(0) + r.fo.power_in(1)) / Pav1;
      r.fod = sl.solve_linear(Om, ThzSolver::bdf2_symbol(Om, Tp / steps)); r.scd = sl.solve_dc(r.fod); return r;
    };
    auto point = [&](double Va, double RL, const Small& sm, int steps, double seed_scale) {
      ThzParams pt = p; pt.ports = ports(Va, RL);
      LsOptions oo = o; oo.RL = RL; oo.steps = steps; oo.seed_scale = seed_scale;
      return run_ls_point(m, pt, sm.fod, sm.scd, Va, Om, oo);
    };
    auto eta_of = [&](const LsPoint& r) { return r.Pdc / (r.Va * r.Va * Pav1); };
    if (want("T14")) {
      auto ports_sym = [&](double Va, double RL) {
        return std::vector<Port>{Port::thevenin(0, 0.5 * Va, R / 2, RL), Port::thevenin(1, -0.5 * Va, R / 2, 0.0)};
      };
      ThzParams pl = p; pl.ports = ports_sym(1.0, Rdc); ThzSolver sl(m, pl);
      FirstOrder fod = sl.solve_linear(Om, ThzSolver::bdf2_symbol(Om, Tp / 60)); SecondOrder scd = sl.solve_dc(fod);
      double es = scd.I20[0] * scd.I20[0] * Rdc / Pav1;
      ThzParams pt = p; pt.ports = ports_sym(0.02, Rdc);
      LsOptions oo = o; oo.RL = Rdc;
      LsPoint t = run_ls_point(m, pt, fod, scd, 0.02, Om, oo);
      double ratio = t.Pdc / (4e-4 * Pav1) / (es * 4e-4);
      bool ok = t.ok && std::fabs(ratio - 1) < 0.01 + 2 * 0.02 && t.tail < 0.02 && std::fabs(t.corr) < 0.02 && std::fabs(t.tr.I_bias[0] + t.tr.I_bias[1]) < 1e-12;
      report("T14", ok, fmt("sym termination (bias tees at both contacts) at Va 0.02, R_L = R_dc: eta %.4e vs BDF2-discrete asymptote %.4e, ratio %.4f; "
                            "tail %.1e, Thevenin corr %.1e, drift %.1e; V_d dc %.2e vs V_s dc %.2e", t.Pdc / (4e-4 * Pav1), es * 4e-4, ratio, t.tail,
                            t.corr, t.tr.dc_drift, t.tr.V_dc[1], t.tr.V_dc[0]));
    }
    Small sm = small(Rdc, 60);
    const double eta_small1 = sm.scd.I20[0] * sm.scd.I20[0] * Rdc / Pav1;   // eta / Va^2, BDF2-discrete
    const double eta_small_c = sm.sc.I20[0] * sm.sc.I20[0] * Rdc / Pav1;   // continuous
    LsPoint t1 = point(0.01, Rdc, sm, 60, 1.0), t2 = point(0.02, Rdc, sm, 60, 1.0);
    if (want("T10")) {
      double r1 = eta_of(t1) / (eta_small1 * 1e-4), r2 = eta_of(t2) / (eta_small1 * 4e-4), scal = t2.Istar / t1.Istar / 4;
      bool ok = t1.ok && t2.ok && std::fabs(r1 - 1) < 0.01 + 2 * 0.01 && std::fabs(r2 - 1) < 0.01 + 2 * 0.02 && std::fabs(scal - 1) < 0.05
                && t1.tail < 0.02 && t2.tail < 0.02 && std::fabs(t1.corr) < 0.02;
      report("T10", ok, fmt("time-domain eta vs BDF2-discrete perturbation asymptote, 1 um funnel 100 K dss bias tee R_L = R_dc = %.3f, 0.3 THz: "
                            "[Va 0.01: eta %.4e vs %.4e, ratio %.4f] [Va 0.02: eta %.4e vs %.4e, ratio %.4f]; I*(0.02)/I*(0.01)/4 = %.4f; "
                            "tail %.1e/%.1e, Thevenin corr %.1e/%.1e, drift %.1e/%.1e; continuous asymptote / discrete %.4f", Rdc, eta_of(t1),
                            eta_small1 * 1e-4, r1, eta_of(t2), eta_small1 * 4e-4, r2, scal, t1.tail, t2.tail, t1.corr, t2.corr, t1.tr.dc_drift,
                            t2.tr.dc_drift, eta_small_c / eta_small1));
    }
    if (want("T13")) {
      double Pav = 1e-4 * Pav1, ratio = t1.Pin / Pav / sm.Pin_lin;
      bool ok = t1.ok && t1.Pin >= t1.Pdc && t1.Pin <= Pav && std::fabs(ratio - 1) < 0.02;
      report("T13", ok, fmt("power bookkeeping at Va 0.01: P_in %.4e, P_dc %.4e, P_av %.4e (P_in/P_av %.4f, linear %.4f, ratio %.4f)",
                            t1.Pin, t1.Pdc, Pav, t1.Pin / Pav, sm.Pin_lin, ratio));
    }
    if (want("T11")) {
      // short circuit (R_L = 0) and open circuit (R_L = 1e4) with the bias seeded at half the prediction
      Small ss = small(0.0, 60), so = small(1e4, 60);
      ThzParams po = p; po.ports = ports(1.0, 0.0); po.ports[0].dc = Port::Kind::Open; ThzSolver sopen(m, po);
      double V20o; { FirstOrder fo = sopen.solve_linear(Om, ThzSolver::bdf2_symbol(Om, Tp / 60)); SecondOrder sc = sopen.solve_dc(fo); V20o = sc.V20[0] - sc.V20[1]; }
      LsPoint ps = point(0.01, 0.0, ss, 60, 0.5), pop = point(0.01, 1e4, so, 60, 0.5);
      double rs = ps.Istar / (ss.scd.I20[0] * 1e-4), ro = pop.Vrect / (V20o * 1e-4);
      bool ok = ps.ok && pop.ok && std::fabs(rs - 1) < 0.02 && std::fabs(ro - 1) < 0.02;
      report("T11", ok, fmt("Thevenin correction from a half-size bias seed at Va 0.01: R_L = 0: I* %.4e vs I_sc Va^2 %.4e (ratio %.4f, "
                            "raw I_dc %.4e, corr %.1e, tail %.1e); R_L = 1e4: V_rect %.4e vs V_oc Va^2 %.4e (ratio %.4f, corr %.1e, tail %.1e)",
                            ps.Istar, ss.scd.I20[0] * 1e-4, rs, ps.I_raw, ps.corr, ps.tail, pop.Vrect, V20o * 1e-4, ro, pop.corr, pop.tail));
    }
    if (want("T12")) {
      Small sm120 = small(Rdc, 120);
      LsPoint a = point(0.3, Rdc, sm, 60, 1.0), b = point(0.3, Rdc, sm120, 120, 1.0);
      double ea = eta_of(a), eb = eta_of(b);
      bool ok = a.ok && b.ok && std::fabs(ea / eb - 1) < 0.03 && std::fabs(a.tr.n_min - b.tr.n_min) < 0.02;
      report("T12", ok, fmt("time-step convergence at Va 0.3: eta %.4e (60 steps) vs %.4e (120 steps), ratio %.4f; n_min %.4f vs %.4f, "
                            "n_max %.4f vs %.4f; tail %.1e/%.1e, corr %.1e/%.1e; small-signal eta %.4e", ea, eb, ea / eb, a.tr.n_min, b.tr.n_min,
                            a.tr.n_max, b.tr.n_max, a.tail, b.tail, a.corr, b.corr, eta_small1 * 0.09));
    }
  }

  // ===================== Stage H5: steady gated flow ========================
  // (plans/hydro_dcgate.md)
  // T18: uniform channel with the gate tied to the grounded contact against the
  //      one-dimensional closed form F(n_0) - F(1) = Gamma j L (inertia included),
  //      both closures, both signs of the current, up to V = 0.3 U_0.
  if (want("T18")) {
    Mesh m = channel_mesh(L, g_hw);
    double worst = 0; std::string det;
    for (int kind = 0; kind < 2; ++kind) {
      ThzParams p; p.Gamma = 0.28; p.nu = 0.0084;
      if (kind == 0) p.closure.kind = Closure::ShallowWater; else { p.closure.kind = Closure::Graphene; p.closure.q = 0.04; }
      double U0 = 1 - p.closure.q;
      for (double v : {0.03, 0.1, 0.3}) {
        double I = v * U0 / (p.Gamma * L);
        for (int sgn : {+1, -1}) {
          SteadyPoint a = steady_point(m, p, 0, sgn * I);
          double ref = p.closure.Phi(gated_channel_n0(p.closure, p.Gamma * L, sgn * I));
          double err = std::fabs(a.V / ref - 1); worst = std::max(worst, a.ok ? err : 1.0);
          det += fmt("\n         %s v %+.2f: V %.6f closed form %.6f (err %.1e)", kind == 0 ? "shallow water" : "graphene     ", sgn * v, a.V, ref, err);
        }
      }
    }
    report("T18", worst < 1e-2, "uniform gated channel, steady V(I) against the 1-D closed form:" + det);
  }

  // T19: mirror-symmetric constriction: swapping the driven contact changes
  //      nothing beyond the asymmetry of the unstructured mesh; the polarity
  //      flip on the same mesh sets the scale.
  if (!quick && want("T19")) {
    Boundary b = make_symmetric_constriction(2000, 6000, 30, 175, 500);
    Mesh m = Mesh::load(ensure_mesh(b, "h_symcon", 20, 120), 2000);
    ThzParams p; p.Gamma = 0.5; p.nu = 0.01; p.closure.q = 0.04;
    double Ie = 1e-7, R = steady_point(m, p, 0, Ie).V / Ie, I = 0.1 * (1 - p.closure.q) / R;
    SteadyPoint a = steady_point(m, p, 0, I), bb = steady_point(m, p, 0, -I), c = steady_point(m, p, 1, I);
    double swap = c.V / a.V - 1, flip = std::fabs(bb.V) / a.V - 1;
    report("T19", a.ok && bb.ok && c.ok && std::fabs(swap) < 1e-3 * std::fabs(flip),
           fmt("symmetric constriction at V = 0.1 U_0: swap asymmetry %.2e, flip asymmetry %.4e (ratio %.1e)", swap, flip, swap / flip));
  }

  // T20: incompressible limit. Funnel, perfect slip, no damping, Re = 10: the
  //      swap-contacts diodicity of the gated solver at Mach 1e-3 against
  //      Stage H on the same mesh.
  if (!quick && want("T20")) {
    FunnelParams fp; fp.Lc = 6000; fp.Ld = 6000;
    Boundary b = make_funnel(fp);
    Mesh m = Mesh::load(ensure_mesh(b, "h_funnel", 20, 120), 2000);
    const double Res = 1e4, M = 1e-3;
    ThzParams p; p.Gamma = 0; p.nu = 1 / Res; p.closure.kind = Closure::ShallowWater;
    SteadyPoint a = steady_point(m, p, 0, M), c = steady_point(m, p, 1, M);
    double D2 = c.V / a.V;
    std::vector<DiodePoint> dp = diodicity_sweep(m, 0.0, std::numeric_limits<double>::infinity(), {1.0, 3.0, 10.0});
    double D = dp.back().D;
    report("T20", a.ok && c.ok && dp.back().ok && std::fabs(D2 / D - 1) < 0.02,
           fmt("funnel, slip, Re 10: gated solver D %.5f (density drop %.1e) vs Stage H D %.5f (rel diff %.1e)", D2, a.nmax - a.nmin, D, D2 / D - 1));
  }

  double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  std::printf("\n%d passed, %d failed (%.1f s)\n", n_pass, n_fail, secs);
  return n_fail ? 1 : 0;
}
