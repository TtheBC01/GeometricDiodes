// Stage H8 helper: create the mesh of a catalogue device (apps/thz_geo.hpp) without solving on it, and
// print its size. Run scripts call it one mesh at a time before they start jobs in parallel, so that two
// jobs never write the same mesh file, and so that the node count is known before memory is committed.
//
// usage: gd_thz_mesh --geo funnel_w1000_L3000 [--hw 20 --hm 120] [--png 1]
#include "generators.hpp"
#include "thz_geo.hpp"
#include "hydro/mesh.hpp"
#include <cstdio>
#include <string>

using namespace gd;
using namespace gd::hydro;

int main(int argc, char** argv) {
  std::string geo; double hw = 20, hm = 120; bool png = false;
  for (int i = 1; i + 1 < argc; i += 2) {
    std::string k = argv[i], v = argv[i + 1];
    if (k == "--geo") geo = v; else if (k == "--hw") hw = std::stod(v); else if (k == "--hm") hm = std::stod(v);
    else if (k == "--png") png = v != "0";
    else { std::fprintf(stderr, "unknown option %s\n", k.c_str()); return 1; }
  }
  if (geo.empty()) { std::fprintf(stderr, "usage: gd_thz_mesh --geo name [--hw 20 --hm 120] [--png 1]\n"); return 1; }
  ThzGeo g = make_thz_geo(geo);
  std::string path = ensure_mesh(g.b, g.mesh_name, hw, hm, png);
  Mesh m = Mesh::load(path, g.W_ref);
  std::printf("%s hw %g hm %g: %d vertices, %d nodes, %d triangles (%s)\n", geo.c_str(), hw, hm, m.nv, m.nn(),
              (int)m.tri.size(), path.c_str());
  return 0;
}
