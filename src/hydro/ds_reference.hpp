// Dyakonov-Shur references for the H2 ladder (plans/hydro_thz_notes.md, sec. 7).
// Nondimensional shallow-water channel of length L (units W_ref), plasma speed
// 1, friction Gamma, no viscosity; source potential 1 + Re(Va e^{i Omega t}),
// drain open. Both functions return the dc drain voltage per Va^2.
#pragma once
#include <cmath>
#include <complex>

namespace gd::hydro::ds {

using cd = std::complex<double>;

// Closed form, Dyakonov and Shur IEEE TED 43, 380 (1996): Delta U / Ua^2 = f / 4.
inline double closed_form(double Omega, double Gamma, double L) {
  double wt = Omega / Gamma;  // omega tau
  double r = std::sqrt(1 + 1 / (wt * wt));
  double kp = Omega * std::sqrt((r + 1) / 2), kpp = Omega * std::sqrt((r - 1) / 2);
  double beta = 2 * wt / std::sqrt(1 + wt * wt);
  double sh = std::sinh(kpp * L), c = std::cos(kp * L);
  double f = 1 + beta - (1 + beta * std::cos(2 * kp * L)) / (sh * sh + c * c);
  return f / 4;
}

// Independent evaluation: analytic first-order fields, second-order dc
// pressure integrated by quadrature (same perturbation equations as the FEM,
// different discretisation and no finite elements).
//   n1 = Va cos(k (L - x)) / cos(k L),  u1 = -n1' / (i Omega + Gamma),  k^2 = Omega^2 - i Omega Gamma
//   J = u20 + Re(n1 u1*)/2 = 0 (open drain) ,  P' = -Re(u1* u1')/2 - Gamma u20,  Delta U = P(L) - P(0).
inline double quadrature(double Omega, double Gamma, double L, int nseg = 20000) {
  cd k = std::sqrt(cd(Omega * Omega, -Omega * Gamma));
  cd a(0.0, Omega); a += Gamma;
  auto n1 = [&](double x) { return std::cos(k * (L - x)) / std::cos(k * L); };
  auto n1p = [&](double x) { return k * std::sin(k * (L - x)) / std::cos(k * L); };
  auto n1pp = [&](double x) { return -k * k * std::cos(k * (L - x)) / std::cos(k * L); };
  auto Pp = [&](double x) {
    cd u1 = -n1p(x) / a, u1p = -n1pp(x) / a;
    double u20 = -0.5 * std::real(n1(x) * std::conj(u1));
    return -0.5 * std::real(std::conj(u1) * u1p) - Gamma * u20;
  };
  // Simpson
  double h = L / nseg, s = Pp(0) + Pp(L);
  for (int i = 1; i < nseg; ++i) s += (i % 2 ? 4 : 2) * Pp(i * h);
  return s * h / 3;
}

}  // namespace gd::hydro::ds
