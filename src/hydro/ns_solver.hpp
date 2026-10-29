// Steady incompressible Navier-Stokes with Brinkman (momentum-relaxation)
// damping and Navier-slip walls on a Taylor-Hood P2/P1 mesh, nondimensional:
//     Re (u.grad)u = -grad p + lap u - Da u,   div u = 0
// Inlet: prescribed developed Brinkman-Poiseuille profile carrying flux Q.
// Outlet: do-nothing (grad u . n - p n = 0), which sets the pressure level.
// Walls: u.n = 0 imposed strongly in a rotated nodal frame (vertex normals
// averaged over the two adjacent edges; sharp corners pinned to u = 0) and the
// tangential Navier condition t.(grad u n) = -(1/S) u.t as a boundary integral.
// Newton with the analytic Jacobian, continuation in Re, Eigen SparseLU.
#pragma once
#include "hydro/mesh.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <functional>
#include <limits>
#include <memory>
#include <vector>

namespace gd::hydro {

struct NSParams {
  double Re = 0;
  double Da = 0;
  double slip = std::numeric_limits<double>::infinity();  // l_s / W_ref; 0 = no-slip
  int inlet = 0, outlet = 1;
  double flux = 1.0;                 // nondimensional flow rate (U W_ref = 1)
  double corner_deg = 25;            // wall vertices sharper than this are pinned
  double tol = 1e-10;
  int max_newton = 25;
  bool verbose = false;
  // Test hook: when set, every boundary node is Dirichlet with this velocity
  // and the pressure is pinned at vertex 0 to pin_value.
  std::function<Vec2(Vec2)> dirichlet_all;
  double pin_value = 0;
};

enum class NodeType { Interior, Inlet, Outlet, Wall, Pinned, DirichletFn };

struct Solution {
  std::vector<Vec2> u;      // Cartesian velocity at all P2 nodes
  std::vector<double> p;    // pressure at vertices
  double Re = 0;
  bool converged = false;
  int newton_iters = 0;
  double residual = 0;
};

// Developed profile across a channel of half-width h (nondimensional) with
// Brinkman number Da and slip S, carrying flux Q: returns speed at y in [-h,h].
double channel_profile(double y, double h, double Da, double S, double Q);
// Corresponding pressure gradient magnitude G = -dp/dx.
double channel_pressure_gradient(double h, double Da, double S, double Q);

class NSSolver {
 public:
  NSSolver(const Mesh& m, const NSParams& p);
  // Continuation from Stokes (or from init) to params.Re. Returns the solution
  // at the last converged Re (check .Re and .converged).
  Solution solve(const Solution* init = nullptr);
  // Newton at fixed Re from the current internal state.
  bool newton(double Re);
  Solution current() const;
  void set_state(const Solution& s);

  const std::vector<NodeType>& node_types() const { return type_; }
  double inlet_width() const { return 2 * in_h_; }
  int num_dof() const { return ndof_; }

 private:
  void classify();
  void impose_dirichlet();
  void assemble(double Re, Eigen::VectorXd& F, std::vector<Eigen::Triplet<double>>& trip);
  Vec2 node_u(int a) const;  // Cartesian velocity of node a from the state vector

  const Mesh& m_;
  NSParams p_;
  int nn_ = 0, ndof_ = 0;
  std::vector<NodeType> type_;
  std::vector<Vec2> t_, n_;           // rotated frame for Wall nodes
  std::vector<char> dir_;             // Dirichlet flag per dof
  Eigen::VectorXd x_;                 // state in nodal frames
  // inlet geometry
  Vec2 in_c_, in_t_, in_n_;
  double in_h_ = 0.5;
  std::unique_ptr<Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>> lu_;
  bool analyzed_ = false;
};

}  // namespace gd::hydro
