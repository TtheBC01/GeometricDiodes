#include "hydro/post.hpp"
#include "hydro/fem.hpp"
#include <cmath>
#include <fstream>

namespace gd::hydro {

double mean_pressure(const Mesh& m, const Solution& s, int tag) {
  double num = 0, den = 0;
  for (const auto& e : m.bedges) {
    if (e.tag != tag) continue;
    num += e.len * 0.5 * (s.p[e.a] + s.p[e.b]);
    den += e.len;
  }
  return den > 0 ? num / den : 0.0;
}

double inward_flux(const Mesh& m, const Solution& s, int tag) {
  double q = 0;
  for (const auto& e : m.bedges) {
    if (e.tag != tag) continue;
    Vec2 un[3] = {s.u[e.a], s.u[e.b], s.u[e.m]};
    for (const auto& g : edge_quad3()) {
      auto phi = p2_edge_basis(g.s);
      Vec2 u = un[0] * phi[0] + un[1] * phi[1] + un[2] * phi[2];
      q += g.w * e.len * dot(u, e.n);
    }
  }
  return q;
}

static bool tri_in_range(const Mesh& m, const std::array<int, 6>& T, double x0, double x1) {
  double cx = (m.X[T[0]].x + m.X[T[1]].x + m.X[T[2]].x) / 3;
  return cx >= x0 && cx <= x1;
}

double viscous_dissipation(const Mesh& m, const Solution& s, double x0, double x1) {
  double D = 0;
  std::array<double, 6> N; std::array<Vec2, 6> dN;
  for (const auto& T : m.tri) {
    if (!tri_in_range(m, T, x0, x1)) continue;
    TriGeom g = tri_geom(m.X[T[0]], m.X[T[1]], m.X[T[2]]);
    for (const auto& q : tri_quad7()) {
      p2_basis(g, q.L0, q.L1, q.L2, N, dN);
      double gu[2][2] = {{0, 0}, {0, 0}};
      for (int a = 0; a < 6; ++a) {
        gu[0][0] += s.u[T[a]].x * dN[a].x; gu[0][1] += s.u[T[a]].x * dN[a].y;
        gu[1][0] += s.u[T[a]].y * dN[a].x; gu[1][1] += s.u[T[a]].y * dN[a].y;
      }
      D += q.w * g.area * (gu[0][0] * gu[0][0] + gu[0][1] * gu[0][1] + gu[1][0] * gu[1][0] + gu[1][1] * gu[1][1]);
    }
  }
  return D;
}

double kinetic_energy_region(const Mesh& m, const Solution& s, double x0, double x1) {
  double K = 0;
  std::array<double, 6> N; std::array<Vec2, 6> dN;
  for (const auto& T : m.tri) {
    if (!tri_in_range(m, T, x0, x1)) continue;
    TriGeom g = tri_geom(m.X[T[0]], m.X[T[1]], m.X[T[2]]);
    for (const auto& q : tri_quad7()) {
      p2_basis(g, q.L0, q.L1, q.L2, N, dN);
      Vec2 u{0, 0};
      for (int a = 0; a < 6; ++a) u += s.u[T[a]] * N[a];
      K += q.w * g.area * u.norm2();
    }
  }
  return K;
}

double mean_pressure_at_x(const Mesh& m, const Solution& s, double xs) {
  // Intersect every triangle edge with the plane and average p over the chord
  // pieces (length-weighted).
  double num = 0, den = 0;
  for (const auto& T : m.tri) {
    Vec2 pts[2]; double pv[2]; int k = 0;
    for (int i = 0; i < 3 && k < 2; ++i) {
      int a = T[i], b = T[(i + 1) % 3];
      double xa = m.X[a].x, xb = m.X[b].x;
      if ((xa - xs) * (xb - xs) > 0 || xa == xb) continue;
      double t = (xs - xa) / (xb - xa);
      pts[k] = m.X[a] + (m.X[b] - m.X[a]) * t;
      pv[k] = s.p[a] + (s.p[b] - s.p[a]) * t;
      ++k;
    }
    if (k == 2) {
      double len = (pts[1] - pts[0]).norm();
      num += len * 0.5 * (pv[0] + pv[1]); den += len;
    }
  }
  return den > 0 ? num / den : 0.0;
}

bool sample_velocity(const Mesh& m, const Solution& s, Vec2 x, Vec2& u) {
  std::array<double, 6> N; std::array<Vec2, 6> dN;
  for (const auto& T : m.tri) {
    Vec2 p0 = m.X[T[0]], p1 = m.X[T[1]], p2 = m.X[T[2]];
    double A2 = (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
    double L1 = ((x.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (x.y - p0.y)) / A2;
    double L2 = ((p1.x - p0.x) * (x.y - p0.y) - (x.x - p0.x) * (p1.y - p0.y)) / A2;
    double L0 = 1 - L1 - L2;
    const double eps = -1e-10;
    if (L0 < eps || L1 < eps || L2 < eps) continue;
    TriGeom g = tri_geom(p0, p1, p2);
    p2_basis(g, L0, L1, L2, N, dN);
    u = {0, 0};
    for (int a = 0; a < 6; ++a) u += s.u[T[a]] * N[a];
    return true;
  }
  return false;
}

double max_speed(const Solution& s) {
  double mx = 0;
  for (const auto& u : s.u) mx = std::max(mx, u.norm());
  return mx;
}

void write_vtk(const Mesh& m, const Solution& s, const std::string& path) {
  std::ofstream f(path);
  f << "# vtk DataFile Version 3.0\nGeometricDiodes hydro\nASCII\nDATASET UNSTRUCTURED_GRID\n";
  f << "POINTS " << m.nv << " double\n";
  for (int i = 0; i < m.nv; ++i) f << m.X[i].x * m.scale << " " << m.X[i].y * m.scale << " 0\n";
  f << "CELLS " << m.tri.size() << " " << 4 * m.tri.size() << "\n";
  for (const auto& T : m.tri) f << "3 " << T[0] << " " << T[1] << " " << T[2] << "\n";
  f << "CELL_TYPES " << m.tri.size() << "\n";
  for (size_t i = 0; i < m.tri.size(); ++i) f << "5\n";
  f << "POINT_DATA " << m.nv << "\nVECTORS velocity double\n";
  for (int i = 0; i < m.nv; ++i) f << s.u[i].x << " " << s.u[i].y << " 0\n";
  f << "SCALARS pressure double 1\nLOOKUP_TABLE default\n";
  for (int i = 0; i < m.nv; ++i) f << s.p[i] << "\n";
}

}  // namespace gd::hydro
