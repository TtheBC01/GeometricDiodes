#include "hydro/diode.hpp"
#include "hydro/post.hpp"
#include <cmath>
#include <cstdio>

namespace gd::hydro {

std::vector<DiodePoint> diodicity_sweep(const Mesh& m, double Da, double S, const std::vector<double>& Re_list,
                                        bool verbose) {
  std::vector<DiodePoint> out(Re_list.size());
  for (int dir = 0; dir < 2; ++dir) {
    NSParams p; p.Da = Da; p.slip = S; p.verbose = verbose;
    p.inlet = dir == 0 ? 0 : 1; p.outlet = dir == 0 ? 1 : 0;
    Solution prev; bool have_prev = false;
    for (size_t i = 0; i < Re_list.size(); ++i) {
      p.Re = Re_list[i];
      NSSolver solver(m, p);
      Solution s = solver.solve(have_prev ? &prev : nullptr);
      DiodePoint& d = out[i];
      d.Re = Re_list[i]; d.Da = Da; d.S = S;
      // Normalise by the discrete inlet flux: the interpolated inlet profile
      // carries 1 + O(h^3) and the two contacts are meshed differently, which
      // would otherwise bias D at the 1e-6 level for steep Brinkman profiles.
      double qin = inward_flux(m, s, p.inlet);
      double Pi = (mean_pressure(m, s, p.inlet) - mean_pressure(m, s, p.outlet)) / qin;
      double fe = std::fabs(-inward_flux(m, s, p.outlet) / qin - 1.0);
      d.flux_err = std::max(d.flux_err, fe);
      if (!s.converged) Pi = std::nan("");  // never report the stale last-converged state as this Re
      if (dir == 0) { d.Pi_f = Pi; d.umax_f = max_speed(s); d.diss_f = viscous_dissipation(m, s); d.ok = s.converged; }
      else { d.Pi_r = Pi; d.umax_r = max_speed(s); d.diss_r = viscous_dissipation(m, s); d.ok = d.ok && s.converged; }
      if (s.converged) { prev = s; have_prev = true; }
      else if (verbose) std::printf("  diodicity_sweep: Re=%g dir=%d did not converge (reached %g)\n", p.Re, dir, s.Re);
    }
  }
  for (auto& d : out) d.D = d.Pi_r / d.Pi_f;
  return out;
}

}  // namespace gd::hydro
