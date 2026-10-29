// P2/P1 Taylor-Hood triangle: shape functions, gradients and quadrature.
// Barycentric ordering follows Mesh::tri: vertices 0,1,2 then midsides
// m01, m12, m20.
#pragma once
#include "core.hpp"
#include <array>

namespace gd::hydro {

struct TriGeom {
  double area = 0;
  std::array<Vec2, 3> gL;  // gradients of the barycentric coordinates
};

inline TriGeom tri_geom(Vec2 p0, Vec2 p1, Vec2 p2) {
  TriGeom g;
  double A2 = (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
  g.area = 0.5 * A2;
  g.gL[0] = {(p1.y - p2.y) / A2, (p2.x - p1.x) / A2};
  g.gL[1] = {(p2.y - p0.y) / A2, (p0.x - p2.x) / A2};
  g.gL[2] = {(p0.y - p1.y) / A2, (p1.x - p0.x) / A2};
  return g;
}

// P2 values and gradients at barycentric (L0, L1, L2).
inline void p2_basis(const TriGeom& g, double L0, double L1, double L2, std::array<double, 6>& N,
                     std::array<Vec2, 6>& dN) {
  N[0] = L0 * (2 * L0 - 1); N[1] = L1 * (2 * L1 - 1); N[2] = L2 * (2 * L2 - 1);
  N[3] = 4 * L0 * L1; N[4] = 4 * L1 * L2; N[5] = 4 * L2 * L0;
  dN[0] = g.gL[0] * (4 * L0 - 1);
  dN[1] = g.gL[1] * (4 * L1 - 1);
  dN[2] = g.gL[2] * (4 * L2 - 1);
  dN[3] = g.gL[0] * (4 * L1) + g.gL[1] * (4 * L0);
  dN[4] = g.gL[1] * (4 * L2) + g.gL[2] * (4 * L1);
  dN[5] = g.gL[2] * (4 * L0) + g.gL[0] * (4 * L2);
}

// Dunavant 7-point rule, degree 5. Weights sum to 1 (multiply by area).
struct QuadPt { double L0, L1, L2, w; };
inline const std::array<QuadPt, 7>& tri_quad7() {
  static const double a = 0.059715871789770, b = 0.470142064105115, wa = 0.132394152788506;
  static const double c = 0.797426985353087, d = 0.101286507323456, wc = 0.125939180544827;
  static const std::array<QuadPt, 7> q = {{{1.0 / 3, 1.0 / 3, 1.0 / 3, 0.225},
                                           {a, b, b, wa}, {b, a, b, wa}, {b, b, a, wa},
                                           {c, d, d, wc}, {d, c, d, wc}, {d, d, c, wc}}};
  return q;
}

// 3-point Gauss on an edge parametrised by s in [0,1]; weights sum to 1.
struct EdgeQuadPt { double s, w; };
inline const std::array<EdgeQuadPt, 3>& edge_quad3() {
  static const double r = 0.3872983346207417;  // sqrt(3/5)/2
  static const std::array<EdgeQuadPt, 3> q = {{{0.5 - r, 5.0 / 18}, {0.5, 8.0 / 18}, {0.5 + r, 5.0 / 18}}};
  return q;
}
// P2 edge basis: node a (s=0), node b (s=1), midside.
inline std::array<double, 3> p2_edge_basis(double s) {
  return {(1 - s) * (1 - 2 * s), s * (2 * s - 1), 4 * s * (1 - s)};
}

}  // namespace gd::hydro
