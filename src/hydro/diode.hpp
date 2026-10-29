// Forward/reverse diodicity sweeps over Re for one mesh and (Da, S).
#pragma once
#include "hydro/ns_solver.hpp"
#include <vector>

namespace gd::hydro {

struct DiodePoint {
  double Re = 0, Da = 0, S = 0;
  double Pi_f = 0, Pi_r = 0;   // nondimensional pressure drops, inlet mean minus outlet mean
  double D = 0;                // Pi_r / Pi_f  (reverse = flow from contact 1 to contact 0)
  bool ok = false;
  double flux_err = 0;         // max |outflow/inflow - 1| over the two directions (mass balance)
  double umax_f = 0, umax_r = 0;
  double diss_f = 0, diss_r = 0;  // int |grad u|^2
  int iters_f = 0, iters_r = 0;
};

// Re_list ascending; each direction is continued from the previous Re.
std::vector<DiodePoint> diodicity_sweep(const Mesh& m, double Da, double S, const std::vector<double>& Re_list,
                                        bool verbose = false);

}  // namespace gd::hydro
