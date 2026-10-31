// Stage H2, full nonlinear gated-compressible solver: steady (dc drive) and
// time domain (BDF2) with the same ports as the perturbation solver. Used by
// the ladder to anchor the perturbation solver (T1, T5, T6); not a production
// tool. Density order 1 (Taylor-Hood) throughout.
#pragma once
#include "hydro/ns_solver.hpp"
#include "hydro/thz_common.hpp"
#include "hydro/thz_solver.hpp"
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
  double P_in = 0;                 // <sum_c V_c I_c> over the last period (power into the graphene)
  double n_min = 1, n_max = 1;     // density extrema over the last period
  double dc_drift = 0;             // max relative change of I_dc, V_dc between the last two periods
  double bias_change = 0;          // bias tee: relative change of the dc bias current at the last update
  std::vector<double> I_bias;      // bias tee: dc load current per port used in the last period
  // Aitken extrapolation of the period means over the last three periods
  // (single slow exponential): extrapolated values, the relative size of the
  // remaining tail at port 0 and the fitted time constant in periods (< 0: no fit)
  std::vector<double> V_dc_ext, I_dc_ext;
  double tail = 0, tau_periods = -1;
  int periods_run = 0;
  bool ok = true;
};

struct BiasTee {
  bool on = false;
  double Rdc_dev = -1;
  int first = 1, every = 1, max_updates = 1000;
  std::vector<double> seed;
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
  // Ideal bias tee: Voltage ports obey V = drive - Rac (I - I_b) - Rdc I_b with
  // I_b the dc load current (the ac current sees Rac, the dc current the load
  // Rdc). I_b is found by a fixed point on the period-mean current, updated only
  // after the slow dc mode has settled: at the end of period `first`, then every
  // `every` periods while at least `every` periods remain. The first update uses
  // the analytic slope (Rac - Rdc) / (Rdc_dev + Rac) of the map I_b -> <I>, later
  // ones the secant. `seed` (per port) starts I_b at the small-signal prediction.
  TimeTrace run(double Omega, int periods, int steps, double tol = 1e-10, const BiasTee& bt = BiasTee());
  // Start from the perturbative periodic state instead of rest: x(t) = rest +
  // Va Re(x_1 e^{i Omega t}) + Va^2 x_20 at t = 0 and t = -dt (dt = period /
  // steps), so the run begins with BDF2 and the only transient is the
  // nonlinear correction. f and s are the first- and second-order solutions
  // per unit amplitude with the same ports (dc load included); n_20 = P -
  // (kappa/4) |n_1|^2.
  void init_from_perturbation(const FirstOrder& f, const SecondOrder& s, double Va, double Omega, int steps);
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
  bool bias_tee_ = false;
  bool have_prev_ = false;
  Eigen::VectorXd xprev_;
  Eigen::VectorXd perturbative_state(const FirstOrder& f, const SecondOrder& s, double Va, double t) const;
  std::vector<double> ibias_, ibias_prev_, imean_prev_;
  int bias_updates_ = 0;
};

}  // namespace gd::hydro
