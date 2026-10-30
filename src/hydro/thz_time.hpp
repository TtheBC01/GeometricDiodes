// Stage H2, full nonlinear gated-compressible solver: steady (dc drive) and
// time domain (BDF2) with the same ports as the perturbation solver. Used by
// the ladder to anchor the perturbation solver (T1, T5, T6); not a production
// tool. Density order 1 (Taylor-Hood) throughout.
#pragma once
#include "hydro/ns_solver.hpp"
#include "hydro/thz_common.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <memory>

namespace gd::hydro {

struct ThzState {
  std::vector<double> n;      // density nodes (nd)
  std::vector<Vec2> u;        // P2 nodes, Cartesian
  std::vector<double> V, I;   // ports
  double t = 0;
};

struct TimeTrace {
  std::vector<double> t;
  std::vector<std::vector<double>> V, I;   // [port][step]
  // averages over the last period
  std::vector<double> V_dc, I_dc;
  bool ok = true;
};

class ThzTimeSolver {
 public:
  ThzTimeSolver(const Mesh& m, const ThzParams& p, int density_order);
  // Steady solve with the dc port settings (Current / Voltage / Open) from rest
  // or from init; continuation in the drive amplitude when Newton fails.
  ThzState solve_steady(double tol = 1e-10, int max_newton = 30);
  // Time integration from rest over `periods` periods of the ac drive at Omega,
  // `steps` BDF2 steps per period; the ac port settings drive, dc settings are
  // ignored except that Voltage ports see Re(Va e^{i Omega t}).
  TimeTrace run(double Omega, int periods, int steps, double tol = 1e-10);
  ThzState state() const;
  const ThzGrid& grid() const { return g_; }

 private:
  // Residual and Jacobian of the semi-discrete system at the current state with
  // time-derivative coefficient c0 (0 = steady) and history term hist.
  void assemble(double c0, const Eigen::VectorXd& hist, double t, double Omega, bool steady,
                Eigen::VectorXd& F, std::vector<Eigen::Triplet<double>>& trip) const;
  bool newton(double c0, const Eigen::VectorXd& hist, double t, double Omega, bool steady, double tol, int maxit);
  double port_drive(int c, double t, double Omega, bool steady) const;
  // Contact boundary value of h for a port potential V (inverse of Phi).
  void contact_h(double V, double& hc, double& dh_dV) const;

  const Mesh& m_;
  ThzParams p_;
  ThzGrid g_;
  Eigen::VectorXd x_;
  std::unique_ptr<Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>> lu_;
  bool analyzed_ = false;
};

}  // namespace gd::hydro
