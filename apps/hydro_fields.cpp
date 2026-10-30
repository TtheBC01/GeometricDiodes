// Solve one case forward and reverse, split the dissipation by region and
// write VTK fields plus a compact field dump for tools/hydro_png.py.
// usage: gd_hydro_fields <geo> <Re> <Da> <S> [hw hm]
#include "generators.hpp"
#include "hydro/diode.hpp"
#include "hydro/mesh.hpp"
#include "hydro/post.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>

using namespace gd;
using namespace gd::hydro;
Boundary make_geo(const std::string& name, double& x_stage0, double& x_stage1);

Boundary make_geo(const std::string& name, double& x0, double& x1) {
  if (name == "tesla1" || name == "tesla3" || name == "tesla3v2") {
    TeslaParams t; t.Lc = t.Ld = 6000;
    t.n_stages = name == "tesla1" ? 1 : 3;
    if (name == "tesla3v2") { t.beta_entry_deg = 20; t.beta_exit_deg = 60; t.r = 300; t.a = 2400; }
    TeslaInfo info; Boundary b = make_tesla(t, &info);
    x0 = t.Lc; x1 = t.Lc + info.main_length;
    return b;
  }
  if (name == "tesla_classic") {  // tools/tesla_classic.py with 6 um leads; stage region = everything between the leads
    Boundary b = load_boundary("meshes/tesla_classic_h.poly");
    x0 = 6000; x1 = b.bbox_max().x - 6000; return b;
  }
  if (name == "funnel") { FunnelParams f; f.Lc = 6000; f.Ld = 6000; x0 = f.Lc; x1 = 6000 + 0.5 * (f.Wc - f.Wn) / std::tan(deg2rad(f.alpha_deg)) + f.Ln; return make_funnel(f); }
  throw std::runtime_error("unknown geometry");
}

static void dump(const Mesh& m, const Solution& s, const std::string& path) {
  std::ofstream f(path);
  f.precision(8);
  f << m.nv << " " << m.tri.size() << " " << m.scale << "\n";
  for (int i = 0; i < m.nv; ++i) f << m.X[i].x << " " << m.X[i].y << " " << s.u[i].x << " " << s.u[i].y << " " << s.p[i] << "\n";
  for (const auto& T : m.tri) f << T[0] << " " << T[1] << " " << T[2] << "\n";
}

int main(int argc, char** argv) {
  if (argc < 5) { std::printf("usage: gd_hydro_fields <geo> <Re> <Da> <S> [hw hm]\n"); return 1; }
  std::string geo = argv[1];
  double Re = std::atof(argv[2]), Da = std::atof(argv[3]);
  double S = std::string(argv[4]) == "inf" ? INFINITY : std::atof(argv[4]);
  double hw = argc > 6 ? std::atof(argv[5]) : 20, hm = argc > 6 ? std::atof(argv[6]) : 120;
  double xs0, xs1;
  Boundary b = make_geo(geo, xs0, xs1);
  Mesh m = Mesh::load(ensure_mesh(b, "h_" + geo, hw, hm), b.contact_width(0));
  double W = b.contact_width(0), L = b.bbox_max().x / W;
  xs0 /= W; xs1 /= W;
  std::printf("%s: Re %g Da %g S %g, stage region x in [%.3f, %.3f] W, L = %.3f W\n", geo.c_str(), Re, Da, S, xs0, xs1, L);
  std::printf("%-8s %10s %10s | %10s %10s %10s | %10s\n", "dir", "Pi", "Pi/diss", "diss_lead0", "diss_stage", "diss_lead1", "KE_stage");
  for (int dir = 0; dir < 2; ++dir) {
    NSParams p; p.Re = Re; p.Da = Da; p.slip = S; p.inlet = dir == 0 ? 0 : 1; p.outlet = dir == 0 ? 1 : 0;
    NSSolver solver(m, p);
    Solution s = solver.solve();
    double qin = inward_flux(m, s, p.inlet);
    double Pi = (mean_pressure(m, s, p.inlet) - mean_pressure(m, s, p.outlet)) / qin;
    double d0 = viscous_dissipation(m, s, -1e300, xs0), d1 = viscous_dissipation(m, s, xs0, xs1), d2 = viscous_dissipation(m, s, xs1, 1e300);
    double dslip = 0;  // slip dissipation (1/S) int u_t^2 ds on walls
    if (std::isfinite(S) && S > 0)
      for (const auto& e : m.bedges) if (e.tag < 0) {
        double ut0 = dot(s.u[e.a], e.t), ut1 = dot(s.u[e.b], e.t), utm = dot(s.u[e.m], e.t);
        dslip += e.len / S * (ut0 * ut0 + ut1 * ut1 + 4 * utm * utm + 2 * ut0 * utm + 2 * ut1 * utm - ut0 * ut1) / 15.0;  // exact int of P2^2
      }
    double dbrink = Da * kinetic_energy_region(m, s);
    std::printf("%-8s %10.3f %10.4f | %10.3f %10.3f %10.3f | %10.4f   (slip %.3f, Brinkman %.3f) %s\n", dir == 0 ? "forward" : "reverse", Pi,
                Pi / (d0 + d1 + d2 + dslip + dbrink), d0, d1, d2, kinetic_energy_region(m, s, xs0, xs1), dslip, dbrink, s.converged ? "" : "NOT CONVERGED");
    std::string tag = geo + "_Re" + argv[2] + "_S" + argv[4] + (dir == 0 ? "_f" : "_r");
    write_vtk(m, s, "results/hydro_" + tag + ".vtk");
    dump(m, s, "results/hydro_" + tag + ".fld");
  }
}
