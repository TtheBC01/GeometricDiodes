#include "hydro/mesh.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

namespace gd::hydro {

void write_poly(const Boundary& b, const std::string& path) {
  std::ofstream f(path);
  if (!f) throw std::runtime_error("write_poly: cannot open " + path);
  // Segments are stored loop by loop in order; a loop break is where a != prev.b.
  const auto& segs = b.segments();
  std::vector<std::vector<int>> loops;
  for (size_t i = 0; i < segs.size(); ++i) {
    if (i == 0 || (segs[i].a - segs[i - 1].b).norm() > 1e-9) loops.emplace_back();
    loops.back().push_back((int)i);
  }
  f.precision(12);
  f << "loops " << loops.size() << "\n";
  for (const auto& L : loops) {
    f << "loop " << L.size() << "\n";
    for (int si : L) {
      const auto& s = segs[si];
      f << s.a.x << " " << s.a.y << " " << (s.kind == SegKind::Contact ? s.contact : -1) << "\n";
    }
  }
}

static bool file_exists(const std::string& p) {
  struct stat st;
  return stat(p.c_str(), &st) == 0;
}

std::string ensure_mesh(const Boundary& b, const std::string& name, double h_wall, double h_max, bool png) {
  char buf[256];
  std::snprintf(buf, sizeof buf, "meshes/%s_hw%g_hm%g", name.c_str(), h_wall, h_max);
  std::string base = buf, mesh = base + ".mesh", poly = "meshes/" + name + ".poly";
  if (file_exists(mesh)) return mesh;
  write_poly(b, poly);
  std::snprintf(buf, sizeof buf, "%g %g", h_wall, h_max);
  std::string cmd = ".venv/bin/python tools/mesh.py \"" + poly + "\" \"" + mesh + "\" " + buf;
  if (png) cmd += " --png \"" + base + ".png\"";
  int rc = std::system(cmd.c_str());
  if (rc != 0 || !file_exists(mesh)) throw std::runtime_error("ensure_mesh: mesher failed: " + cmd);
  return mesh;
}

Mesh Mesh::load(const std::string& path, double scale) {
  std::ifstream f(path);
  if (!f) throw std::runtime_error("Mesh::load: cannot open " + path);
  Mesh m;
  m.scale = scale;
  std::string kw;
  int n;
  f >> kw >> n;
  if (kw != "nodes") throw std::runtime_error("Mesh::load: bad header");
  m.X.resize(n);
  for (int i = 0; i < n; ++i) { f >> m.X[i].x >> m.X[i].y; m.X[i] = m.X[i] / scale; }
  m.nv = n;
  f >> kw >> n;
  if (kw != "triangles") throw std::runtime_error("Mesh::load: expected triangles");
  std::vector<std::array<int, 3>> t3(n);
  for (auto& t : t3) f >> t[0] >> t[1] >> t[2];
  f >> kw >> n;
  if (kw != "edges") throw std::runtime_error("Mesh::load: expected edges");
  std::vector<std::array<int, 3>> e3(n);
  for (auto& e : e3) f >> e[0] >> e[1] >> e[2];

  // Midside nodes via an edge map.
  std::map<std::pair<int, int>, int> mid;
  auto midside = [&](int a, int b) {
    auto key = std::minmax(a, b);
    auto it = mid.find(key);
    if (it != mid.end()) return it->second;
    int id = (int)m.X.size();
    m.X.push_back((m.X[a] + m.X[b]) * 0.5);
    mid[key] = id;
    return id;
  };
  m.tri.reserve(t3.size());
  m.h_min = 1e300; m.h_max = 0;
  for (const auto& t : t3) {
    std::array<int, 6> T{t[0], t[1], t[2], 0, 0, 0};
    T[3] = midside(t[0], t[1]); T[4] = midside(t[1], t[2]); T[5] = midside(t[2], t[0]);
    for (int k = 0; k < 3; ++k) {
      double h = (m.X[t[k]] - m.X[t[(k + 1) % 3]]).norm();
      m.h_min = std::min(m.h_min, h); m.h_max = std::max(m.h_max, h);
    }
    m.tri.push_back(T);
  }
  // Boundary edges: inward normal from the owning triangle's third vertex.
  std::map<std::pair<int, int>, int> edge_tri;
  for (size_t i = 0; i < m.tri.size(); ++i)
    for (int k = 0; k < 3; ++k) edge_tri[std::minmax(m.tri[i][k], m.tri[i][(k + 1) % 3])] = (int)i;
  m.bedges.reserve(e3.size());
  for (const auto& e : e3) {
    BEdge be;
    be.a = e[0]; be.b = e[1]; be.tag = e[2];
    auto it = mid.find(std::minmax(be.a, be.b));
    if (it == mid.end()) throw std::runtime_error("Mesh::load: boundary edge not in any triangle");
    be.m = it->second;
    Vec2 d = m.X[be.b] - m.X[be.a];
    be.len = d.norm();
    be.t = d / be.len;
    int ti = edge_tri.at(std::minmax(be.a, be.b));
    const auto& T = m.tri[ti];
    int third = T[0];
    for (int k = 0; k < 3; ++k) if (T[k] != be.a && T[k] != be.b) third = T[k];
    Vec2 nl = be.t.left();
    be.n = dot(m.X[third] - m.X[be.a], nl) > 0 ? nl : -nl;
    m.bedges.push_back(be);
  }
  return m;
}

Mesh Mesh::mirrored_x() const {
  Mesh m = *this;
  for (auto& p : m.X) p.x = -p.x;
  for (auto& T : m.tri) { std::swap(T[1], T[2]); std::swap(T[3], T[5]); }
  for (auto& e : m.bedges) { e.n.x = -e.n.x; e.t.x = -e.t.x; }
  return m;
}

}  // namespace gd::hydro
