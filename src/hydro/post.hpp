// Postprocessing: contact averages, fluxes, dissipation, VTK output.
#pragma once
#include "hydro/ns_solver.hpp"
#include <string>

namespace gd::hydro {

// Length-weighted mean of the P1 pressure along the edges with this tag.
double mean_pressure(const Mesh& m, const Solution& s, int tag);
// Inward flux  int u.n ds over the edges with this tag (positive = entering).
double inward_flux(const Mesh& m, const Solution& s, int tag);
// Viscous dissipation int |grad u|^2 dA (optionally restricted to x in [x0,x1]).
double viscous_dissipation(const Mesh& m, const Solution& s, double x0 = -1e300, double x1 = 1e300);
double kinetic_energy_region(const Mesh& m, const Solution& s, double x0 = -1e300, double x1 = 1e300);
// Mean pressure on the cross-section x = xs within |y| <= ymax (uses triangle
// edges crossing the plane; adequate for probes inside straight leads).
double mean_pressure_at_x(const Mesh& m, const Solution& s, double xs);
// Point sample of the P2 velocity (brute-force triangle search; tests only).
// Returns false when the point is outside the mesh.
bool sample_velocity(const Mesh& m, const Solution& s, Vec2 x, Vec2& u);
// Max |u|.
double max_speed(const Solution& s);
// Legacy VTK with vertex velocity, pressure and node type; coordinates in nm.
void write_vtk(const Mesh& m, const Solution& s, const std::string& path);

}  // namespace gd::hydro
