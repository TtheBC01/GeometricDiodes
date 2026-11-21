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

// The same channel for any convective closure, exact Dirac gas included: the steady
// one-dimensional momentum balance
//   (A + c_div + 2 c_grad) u u' + Gamma u = -(h'(n) - E) n',   u = j / n,
//   A = 1 + gamma^2 b2 / 2,  b2 = u^2 / (2 q),  E = (1 - gamma^(-1/2)) (1 - q) / sqrt n + b2 q / n
// (A = 1, E = 0 unless the closure is exact) integrated from the grounded contact (n = 1 at
// x = L) to the driven one with RK4. Returns the density at the driven contact, or -1 if
// the drift reaches v_F or the flow chokes on the way.
inline double gated_channel_ode(const ClosureFn& c, double Gamma, double L, double j, int nstep = 20000) {
  const double cc = c.cdiv() + 2 * c.cgrad();
  const bool ex = c.exact();
  bool bad = false;
  auto rhs = [&](double n) {
    double u = j / n, A = 1, E = 0;
    if (ex) {
      double b2 = u * u / (2 * c.q);
      if (b2 >= 1) { bad = true; return 0.0; }
      A = 1 + 0.5 * b2 / (1 - b2);
      E = (1 - std::pow(1 - b2, 0.25)) * (1 - c.q) / std::sqrt(n) + b2 * c.q / n;
    }
    double den = c.hp(n) - E - (A + cc) * u * u / n;
    if (den <= 0) { bad = true; return 0.0; }
    return -Gamma * u / den;
  };
  double n = 1.0, h = -L / nstep;   // from x = L down to x = 0
  for (int i = 0; i < nstep && !bad; ++i) {
    double k1 = rhs(n), k2 = rhs(n + 0.5 * h * k1), k3 = rhs(n + 0.5 * h * k2), k4 = rhs(n + h * k3);
    n += h * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    if (n <= 0) bad = true;
  }
  return bad ? -1.0 : n;
}

// Steady radial flow in a sector of a ring, r1 < r < r2, flux J per radian outward from the
// inner contact (negative: inward), outer contact grounded (n = 1). Written in the conservative
// form, from the momentum per particle M u and the pressure P of the gas, not from the velocity
// form the solvers use:
//   u d(M u)/dr + (1/n) dP/dr + (1 - q) dn/dr + Gamma M u = 0,   n u r = J.
// model 0: no inertia (friction and the static force only);
// model 1: M = sqrt n, P = P0(n) = (2 q / 3) n^(3/2): the Tomadin-Polini form (c_div -1/2, c_grad 0);
// model 2: M = sqrt n, P = P0 - n^(3/2) u^2 / 4: the degenerate Dirac gas to second order;
// model 3: M = sqrt(n) gamma^(1/2), P = P0 gamma^(-3/2): the same gas exactly.
// Returns the density at the inner contact (-1 if u reaches v_F).
inline double radial_reference(int model, double q, double Gamma, double r1, double r2, double J, int nstep = 20000) {
  bool bad = false;
  auto gam = [&](double u) { double b2 = u * u / (2 * q); if (b2 >= 1) { bad = true; return 1.0; } return 1 / std::sqrt(1 - b2); };
  auto Mu = [&](double n, double u) { return model == 3 ? std::sqrt(n) * std::sqrt(gam(u)) * u : std::sqrt(n) * u; };
  auto P = [&](double n, double u) {
    double P0 = (2 * q / 3) * std::pow(n, 1.5);
    if (model == 2) return P0 - 0.25 * std::pow(n, 1.5) * u * u;
    if (model == 3) return P0 * std::pow(gam(u), -1.5);
    return P0;
  };
  auto rhs = [&](double r, double n) {
    double u = J / (n * r);
    if (model == 0) return -Gamma * std::sqrt(n) * u / (q / std::sqrt(n) + (1 - q));
    const double dn = 1e-6 * n, du = 1e-6 * std::max(std::fabs(u), 1e-6);
    double Mn = (Mu(n + dn, u) - Mu(n - dn, u)) / (2 * dn), Mv = (Mu(n, u + du) - Mu(n, u - du)) / (2 * du);
    double Pn = (P(n + dn, u) - P(n - dn, u)) / (2 * dn), Pv = (P(n, u + du) - P(n, u - du)) / (2 * du);
    double den = u * Mn - Mv * u * u / n + Pn / n - Pv * u / (n * n) + (1 - q);
    return (Mv * u * u / r + Pv * u / (n * r) - Gamma * Mu(n, u)) / den;
  };
  double n = 1.0, r = r2, h = (r1 - r2) / nstep;
  for (int i = 0; i < nstep && !bad; ++i) {
    double k1 = rhs(r, n), k2 = rhs(r + 0.5 * h, n + 0.5 * h * k1), k3 = rhs(r + 0.5 * h, n + 0.5 * h * k2), k4 = rhs(r + h, n + h * k3);
    n += h * (k1 + 2 * k2 + 2 * k3 + k4) / 6; r += h;
    if (n <= 0) bad = true;
  }
  return bad ? -1.0 : n;
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
