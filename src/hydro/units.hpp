// Physical layer for the hydrodynamic stage (SI inside, inputs in the units
// experimentalists quote). Fermi-liquid graphene: m* = hbar k_F / v_F.
//   Re = I / (n e nu)              (width independent in 2D)
//   Da = W^2 / (nu tau_mr)         (Gurzhi number)
//   S  = l_s / W
//   V  = Pi * m* nu U / (e W),  U = I/(n e W)  =>  R = Pi m* nu / (e^2 n W^2)
#pragma once
#include <cmath>

namespace gd::hydro::units {

constexpr double e = 1.602176634e-19;      // C
constexpr double hbar = 1.054571817e-34;   // J s
constexpr double kB = 1.380649e-23;        // J/K
constexpr double vF = 1.0e6;               // m/s
constexpr double m_e = 9.1093837e-31;      // kg

inline double n_m2(double n_cm2) { return n_cm2 * 1e4; }
inline double kF(double n_cm2) { return std::sqrt(M_PI * n_m2(n_cm2)); }
inline double EF_J(double n_cm2) { return hbar * vF * kF(n_cm2); }
inline double m_star(double n_cm2) { return hbar * kF(n_cm2) / vF; }
// Bare e-e length hbar vF EF / (kB T)^2 (no numerical prefactor).
inline double l_ee(double n_cm2, double T) { double kT = kB * T; return hbar * vF * EF_J(n_cm2) / (kT * kT); }
inline double nu_from_lee(double lee_m) { return vF * lee_m / 4; }
inline double tau_from_lmr(double lmr_m) { return lmr_m / vF; }
inline double reynolds(double I_A, double n_cm2, double nu) { return I_A / (n_m2(n_cm2) * e * nu); }
inline double gurzhi_Da(double W_m, double nu, double tau) { return W_m * W_m / (nu * tau); }
inline double drift_speed(double I_A, double n_cm2, double W_m) { return I_A / (n_m2(n_cm2) * e * W_m); }
inline double resistance_ohm(double Pi, double W_m, double n_cm2, double nu) {
  return Pi * m_star(n_cm2) * nu / (e * e * n_m2(n_cm2) * W_m * W_m);
}
// Drude conductivity n e^2 tau / m* (siemens, 2D sheet).
inline double drude_sigma(double n_cm2, double tau) { return n_m2(n_cm2) * e * e * tau / m_star(n_cm2); }

}  // namespace gd::hydro::units
