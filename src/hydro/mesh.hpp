// Triangle mesh for the hydrodynamic stage: P1 vertices from gmsh plus P2
// midside nodes created here. Coordinates are nondimensional (divided by the
// reference width passed to load). Boundary edges carry the contact tag of
// the ballistic geometry (-1 wall, c >= 0 contact c) and an inward normal.
#pragma once
#include "geometry.hpp"
#include <array>
#include <string>
#include <vector>

namespace gd::hydro {

struct BEdge {
  int a = 0, b = 0, m = 0;  // end vertices and midside node
  int tag = -1;             // -1 wall, >= 0 contact id
  Vec2 n;                   // unit normal pointing into the domain
  Vec2 t;                   // unit tangent a -> b
  double len = 0;
};

struct Mesh {
  std::vector<Vec2> X;                     // nodes: [0, nv) vertices, [nv, nn) midsides
  int nv = 0;                              // vertex count (P1 pressure nodes)
  std::vector<std::array<int, 6>> tri;     // v0 v1 v2 m01 m12 m20, CCW
  std::vector<BEdge> bedges;
  double scale = 1;                        // physical length per unit (nm)
  double h_min = 0, h_max = 0;             // edge length statistics
  int nn() const { return (int)X.size(); }

  // Load a .mesh written by tools/mesh.py, dividing coordinates by scale.
  static Mesh load(const std::string& path, double scale);
  // Mirror x -> -x (reverses triangle orientation, fixed here).
  Mesh mirrored_x() const;
};

// Write the polygon loops of a Boundary for tools/mesh.py.
void write_poly(const Boundary& b, const std::string& path);

// Ensure meshes/<name>_hw<h_wall>_hm<h_max>.mesh exists (run tools/mesh.py
// through .venv/bin/python when it does not) and return its path. Sizes in nm.
std::string ensure_mesh(const Boundary& b, const std::string& name, double h_wall, double h_max,
                        bool png = false);

}  // namespace gd::hydro
