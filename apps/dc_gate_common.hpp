// Stage H5 (plans/hydro_dcgate.md): steady gated flow under the two reversal
// protocols, and the one-dimensional closed form it is tested against.
#pragma once
#include "hydro/thz_time.hpp"
#include <cmath>

namespace gd::hydro {

// One-dimensional gated channel of length L (units of its width), flux j per
// unit width from the driven contact (x = 0) to the grounded one (x = L, n = 1):
//   d/dx [u^2 / 2 + h(n)] = -Gamma u,  n u = j
//   =>  F(n_0) - F(1) = Gamma j L,  F(n) = G(n) + j^2 / n,  G' = n h'(n).
// The shape of a slowly varying channel enters only through L -> int dx / W.
// Returns the density at the driven contact; its potential is closure.Phi(n_0).
inline double gated_channel_n0(const ClosureFn& c, double GammaL, double j) {
  auto G = [&](double n) { return c.kind == Closure::ShallowWater ? 0.5 * n * n : (2.0 / 3.0) * (1 - c.q) * std::pow(n, 1.5) + c.q * n; };
  auto F = [&](double n) { return G(n) + j * j / n; };
  double rhs = F(1.0) + GammaL * j, n = 1.0 + GammaL * j;
  if (n < 0.05) n = 0.05;
  for (int it = 0; it < 100; ++it) {
    double d = (F(n) - rhs) / (n * c.hp(n) - j * j / (n * n));
    n -= d; if (n < 1e-3) n = 1e-3;
    if (std::fabs(d) < 1e-14) break;
  }
  return n;
}

struct SteadyPoint { double V = 0, umax = 0, nmin = 1, nmax = 1; bool ok = false; };

// Steady state with a dc current I injected at contact `driven` (0 or 1) and the
// other contact grounded to the gate. Returns the potential of the driven contact.
inline SteadyPoint steady_point(const Mesh& m, ThzParams p, int driven, double I, double tol = 1e-11) {
  Port src; src.tag = driven; src.dc = Port::Kind::Current; src.Idc = I;
  p.ports = driven == 0 ? std::vector<Port>{src, Port::ground(1)} : std::vector<Port>{Port::ground(0), src};
  ThzTimeSolver ts(m, p, 1);
  ThzState st = ts.solve_steady(tol);
  SteadyPoint r; r.ok = st.t >= 0; r.V = st.V[driven];
  for (const Vec2& u : st.u) r.umax = std::max(r.umax, std::hypot(u.x, u.y));
  r.nmin = r.nmax = st.n.empty() ? 1.0 : st.n[0];
  for (double n : st.n) { r.nmin = std::min(r.nmin, n); r.nmax = std::max(r.nmax, n); }
  return r;
}

}  // namespace gd::hydro
