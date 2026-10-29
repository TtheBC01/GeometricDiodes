// Diodicity sweep: geometries x Da x S x Re, forward and reverse, CSV out.
// usage: gd_hydro_sweep [--geo a,b,c] [--Re 0.1,0.3,1] [--Da 0,1] [--S 0.5,inf]
//                       [--hw 20] [--hm 120] [--threads 6] [--out results/hydro_sweep.csv]
#include "generators.hpp"
#include "hydro/diode.hpp"
#include "hydro/mesh.hpp"
#include "hydro/units.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace gd;
using namespace gd::hydro;

static std::vector<double> parse_list(const std::string& s) {
  std::vector<double> v; std::stringstream ss(s); std::string t;
  while (std::getline(ss, t, ',')) v.push_back(t == "inf" ? INFINITY : std::stod(t));
  return v;
}
static std::vector<std::string> parse_names(const std::string& s) {
  std::vector<std::string> v; std::stringstream ss(s); std::string t;
  while (std::getline(ss, t, ',')) v.push_back(t);
  return v;
}

Boundary make_geo(const std::string& name) {
  if (name == "tesla1") { TeslaParams t; t.n_stages = 1; t.Lc = t.Ld = 6000; return make_tesla(t); }
  if (name == "tesla3") { TeslaParams t; t.n_stages = 3; t.Lc = t.Ld = 6000; return make_tesla(t); }
  if (name == "tesla3v2") { TeslaParams t; t.n_stages = 3; t.beta_entry_deg = 20; t.beta_exit_deg = 60; t.r = 300; t.a = 2400; t.Lc = t.Ld = 6000; return make_tesla(t); }
  if (name == "funnel") { FunnelParams f; f.Lc = 6000; f.Ld = 6000; return make_funnel(f); }
  if (name == "straight") return make_stepped_channel(2000, 6000, 175, 5490, 2000, 6000);
  if (name == "symcon") return make_symmetric_constriction(2000, 6000, 30, 175, 500);
  // tesla1 scaled to a 1 um channel (Geurs-like single loop): all lengths x 5.714
  if (name == "tesla1w1000") { TeslaParams t; t.n_stages = 1; double k = 1000.0 / 175; t.w = 1000; t.a *= k; t.r *= k; t.gap *= k; t.Wc = t.Wd = 4000; t.Lc = t.Ld = 12000; t.lead_in *= k; t.lead_out *= k; return make_tesla(t); }
  throw std::runtime_error("unknown geometry " + name);
}

int main(int argc, char** argv) {
  std::vector<std::string> geos = {"tesla1"};
  std::vector<double> Res = {0.1, 0.3, 1, 3, 10}, Das = {1.0}, Ss = {0.5};
  double hw = 20, hm = 120; int threads = 6; std::string out = "results/hydro_sweep.csv";
  for (int i = 1; i + 1 < argc; i += 2) {
    std::string k = argv[i], v = argv[i + 1];
    if (k == "--geo") geos = parse_names(v);
    else if (k == "--Re") Res = parse_list(v);
    else if (k == "--Da") Das = parse_list(v);
    else if (k == "--S") Ss = parse_list(v);
    else if (k == "--hw") hw = std::stod(v);
    else if (k == "--hm") hm = std::stod(v);
    else if (k == "--threads") threads = std::stoi(v);
    else if (k == "--out") out = v;
  }
  struct Task { std::string geo; double Da, S; };
  std::vector<Task> tasks;
  for (auto& g : geos) for (double Da : Das) for (double S : Ss) tasks.push_back({g, Da, S});
  // Meshes are generated up front (python, serial) and cached.
  std::map<std::string, Mesh> meshes;
  for (auto& g : geos) {
    Boundary b = make_geo(g);
    std::string err = b.validate(0);
    if (!err.empty()) std::printf("WARNING %s: %s\n", g.c_str(), err.c_str());
    std::string path = ensure_mesh(b, "h_" + g + (g == "tesla1w1000" ? "" : ""), hw, hm);
    meshes.emplace(g, Mesh::load(path, b.contact_width(0)));
    std::printf("%-12s %7d vertices %7d tris  W_ref %g nm\n", g.c_str(), meshes.at(g).nv, (int)meshes.at(g).tri.size(), b.contact_width(0));
  }
  std::vector<std::vector<DiodePoint>> results(tasks.size());
  std::mutex mu; size_t next = 0;
  auto worker = [&]() {
    for (;;) {
      size_t i;
      { std::lock_guard<std::mutex> lk(mu); if (next >= tasks.size()) return; i = next++; }
      const Task& t = tasks[i];
      auto r = diodicity_sweep(meshes.at(t.geo), t.Da, t.S, Res);
      std::lock_guard<std::mutex> lk(mu);
      results[i] = r;
      for (auto& d : r)
        std::printf("%-12s Da %5.2f S %5.2f Re %7.3f | Pi_f %12.4f Pi_r %12.4f | D-1 %+10.3e | flux err %.1e umax %.2f %.2f %s\n",
                    t.geo.c_str(), t.Da, t.S, d.Re, d.Pi_f, d.Pi_r, d.D - 1, d.flux_err, d.umax_f, d.umax_r, d.ok ? "" : "NOT CONVERGED");
      std::fflush(stdout);
    }
  };
  std::vector<std::thread> pool;
  for (int k = 0; k < std::min<int>(threads, (int)tasks.size()); ++k) pool.emplace_back(worker);
  for (auto& th : pool) th.join();
  std::ofstream csv(out);
  csv.precision(15);
  csv << "geometry,W_ref_nm,Da,S,Re,Pi_f,Pi_r,D,converged,flux_err,umax_f,umax_r,diss_f,diss_r\n";
  for (size_t i = 0; i < tasks.size(); ++i)
    for (auto& d : results[i])
      csv << tasks[i].geo << "," << meshes.at(tasks[i].geo).scale << "," << d.Da << "," << d.S << "," << d.Re << "," << d.Pi_f << ","
          << d.Pi_r << "," << d.D << "," << d.ok << "," << d.flux_err << "," << d.umax_f << "," << d.umax_r << "," << d.diss_f << "," << d.diss_r << "\n";
  std::printf("CSV written to %s\n", out.c_str());
}
