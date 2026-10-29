// Graphene low-energy band quantities (electrons only, Dirac cone).
#pragma once
#include "core.hpp"

namespace gd {

// Carrier density n in cm^-2 -> Fermi wave vector in 1/nm: k_F = sqrt(pi n).
inline double kF_from_density(double n_cm2) { return std::sqrt(kPi * n_cm2 * 1e-14); }
inline double EF_from_density(double n_cm2) { return kHbarVF * kF_from_density(n_cm2); }  // eV
inline double fermi_wavelength(double n_cm2) { return 2 * kPi / kF_from_density(n_cm2); }   // nm

inline double fermi(double E, double mu, double kT) {
  double x = (E - mu) / kT;
  if (x > 60) return 0.0;
  if (x < -60) return 1.0;
  return 1.0 / (1.0 + std::exp(x));
}

// Number of propagating modes in a contact of width W at kinetic energy Ek:
// N = k W / pi (per spin/valley), k = Ek / (hbar v_F).
inline double modes(double Ek, double W) { return Ek * W / (kPi * kHbarVF); }

// Sharvin (ballistic) conductance of a width-W constriction at E_F in siemens.
inline double sharvin_conductance(double EF, double W) {
  return kValleySpin * kE2OverH * modes(EF, W);
}

}  // namespace gd
