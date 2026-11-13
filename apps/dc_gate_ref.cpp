// Stage H5 reference: the incompressible (Stage H) diodicity on the same mesh and
// at the same Reynolds number, damping and wall condition as a gd_dc_gate row, so
// that the swap-contacts asymmetry of the gated solver can be compared with it.
// usage: gd_dc_gate_ref --geo ... --set 100K|300K [--slip inf|0] [--hw --hm] --Re r1,r2,... [--out file.csv]
#include "generators.hpp"
#include "thz_geo.hpp"
#include "hydro/diode.hpp"
#include "hydro/mesh.hpp"
#include "hydro/thz_units.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace gd;
using namespace gd::hydro;

int main(int argc, char** argv) {
  std::string geo = "funnel", set = "300K", out;
  std::vector<double> Res;
  double slip = INFINITY, hw = 20, hm = 120;
  for (int i = 1; i + 1 < argc; i += 2) {
    std::string k = argv[i], v = argv[i + 1];
    if (k == "--geo") geo = v; else if (k == "--set") set = v; else if (k == "--out") out = v;
    else if (k == "--slip") slip = v == "inf" ? INFINITY : std::stod(v);
    else if (k == "--hw") hw = std::stod(v); else if (k == "--hm") hm = std::stod(v);
    else if (k == "--Re") { std::stringstream ss(v); std::string t; while (std::getline(ss, t, ',')) Res.push_back(std::stod(t)); }
    else { std::fprintf(stderr, "unknown option %s\n", k.c_str()); return 1; }
  }
  double nu = set == "300K" ? 0.03 : 0.1, tau = set == "300K" ? 1e-12 : 5e-12, tauee = set == "300K" ? 0.1e-12 : 1e-12;
  ThzGeo G;
  if (geo == "symcon") G = {make_symmetric_constriction(2000, 6000, 30, 175, 500), 2000, "h_symcon"};
  else G = make_thz_geo(geo);
  Mesh m = Mesh::load(ensure_mesh(G.b, G.mesh_name, hw, hm), G.W_ref);
  ThzUnits u = ThzUnits::make(1e12, 30e-9, 3.8, G.W_ref * 1e-9, nu, tau, tauee);
  double Da = u.Gamma / u.inv_Res;   // W_ref^2 / (nu tau_mr)
  std::sort(Res.begin(), Res.end());
  std::vector<DiodePoint> dp = diodicity_sweep(m, Da, slip, Res);
  bool fresh = true;
  if (!out.empty()) { std::ifstream in(out); fresh = !in.good() || in.peek() == std::ifstream::traits_type::eof(); }
  std::ofstream csv;
  if (!out.empty()) { csv.open(out, std::ios::app); csv.precision(12); if (fresh) csv << "geo,set,slip,hw,hm,Re,Da,D_inc,ok\n"; }
  for (const DiodePoint& d : dp) {
    std::printf("%s %s slip %g Re %.4f Da %.2f: D_inc - 1 = %.4e%s\n", geo.c_str(), set.c_str(), slip, d.Re, Da, d.D - 1, d.ok ? "" : "  NOT CONVERGED");
    if (csv.is_open()) csv << geo << "," << set << "," << slip << "," << hw << "," << hm << "," << d.Re << "," << Da << "," << d.D << "," << d.ok << "\n";
  }
  return 0;
}
