// Mesh all standard geometries once and render them, to inspect the pipeline.
#include "generators.hpp"
#include "hydro/mesh.hpp"
#include <cstdio>
using namespace gd;
using namespace gd::hydro;
int main() {
  struct G { std::string name; Boundary b; double hw, hm; };
  TeslaParams t1; t1.n_stages = 1; t1.Lc = t1.Ld = 6000;
  TeslaParams t3; t3.n_stages = 3; t3.Lc = t3.Ld = 6000;
  FunnelParams fp; fp.Lc = 6000; fp.Ld = 6000;
  std::vector<G> gs = {{"h_straight", make_straight_channel(6000, 1000), 50, 50},
                       {"h_tesla1", make_tesla(t1), 20, 120},
                       {"h_tesla3", make_tesla(t3), 20, 120},
                       {"h_funnel", make_funnel(fp), 20, 120}};
  for (auto& g : gs) {
    std::string path = ensure_mesh(g.b, g.name, g.hw, g.hm, true);
    Mesh m = Mesh::load(path, g.b.contact_width(0));
    std::printf("%-12s %7d vertices %7d nodes %7d tris %5zu bedges  h [%.4f, %.4f] (W units)\n", g.name.c_str(),
                m.nv, m.nn(), (int)m.tri.size(), m.bedges.size(), m.h_min, m.h_max);
  }
}
