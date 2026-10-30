// Stage H2, perturbation solver (plans/hydro_thz_notes.md, sections 3 to 5):
// first-order time-harmonic plasmonic response and second-order dc response
// of the gated-compressible electron fluid about rest, with lumped ports.
#pragma once
#include "hydro/ns_solver.hpp"
#include "hydro/thz_common.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <memory>

namespace gd::hydro {

struct FirstOrder {
  double Omega = 0;
  std::vector<cd> n1;        // all P2 nodes (midsides interpolated when the density is P1)
  std::vector<cd> u1x, u1y;  // P2 nodes, Cartesian
  std::vector<cd> V1, I1;    // per port: potential, flux into the device
  // time-averaged power into the device through port c: Re(V1 I1*)/2
  double power_in(int c) const { return 0.5 * std::real(V1[c] * std::conj(I1[c])); }
};

struct SecondOrder {
  Solution sol;              // u = u_20 (Cartesian), p = P = dc part of h
  std::vector<double> V20, I20;   // per port: dc potential, dc flux into the device (incl. Stokes drift)
  std::vector<double> I20_drift;  // the Re(n1 u1*)/2 part of I20
};

class ThzSolver {
 public:
  // density_order: 1 (default, Taylor-Hood) or 2 (equal order; inaccurate as Omega -> 0).
  ThzSolver(const Mesh& m, const ThzParams& p, int density_order = 1);
  FirstOrder solve_linear(double Omega);
  SecondOrder solve_dc(const FirstOrder& f);
  // dc (zero-frequency, linear) resistance between ports src and drn in units
  // of R_unit: potential at src per unit flux with drn grounded, no forcing.
  double dc_resistance(int src, int drn);
  const ThzGrid& grid() const { return g1_; }
  const ThzGrid& dc_grid() const { return g0_; }
  const ThzParams& params() const { return p_; }

 private:
  // Linear operator (i Omega + Gamma) u + grad n - nu lap u, i Omega n + div u,
  // plus slip, on grid g; rows/cols in g's dof layout, Dirichlet rows left empty.
  void assemble_operator(const ThzGrid& g, double Omega, cd nu, std::vector<Eigen::Triplet<cd>>& trip) const;
  // Port rows and contact-node rows for the given order (ac or dc) with a
  // prescribed known flux per port (dc Stokes drift) and known node offsets.
  void assemble_ports(const ThzGrid& g, bool dc, const std::vector<double>& known_flux,
                      const std::vector<double>& node_rhs, std::vector<Eigen::Triplet<cd>>& trip,
                      Eigen::VectorXcd& rhs) const;
  // Flux coefficient entries: I_c = sum_k coef_k x_k over velocity dofs (same in both grids).
  void flux_row(const ThzGrid& g, int c, std::vector<std::pair<int, double>>& coef) const;

  const Mesh& m_;
  ThzParams p_;
  ThzGrid g1_;   // first order (P2 or P1 density)
  ThzGrid g0_;   // dc (P1 density)
  std::unique_ptr<Eigen::SparseLU<Eigen::SparseMatrix<cd>, Eigen::COLAMDOrdering<int>>> lu_;
  bool analyzed_ = false;
};

}  // namespace gd::hydro
