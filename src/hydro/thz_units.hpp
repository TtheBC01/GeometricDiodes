// Physical layer for Stage H2 (plans/hydro_thz_notes.md, section 2): from
// (n_0, gate, W_ref, nu, tau_mr, tau_ee) to the nondimensional groups and the
// unit conversions for current, voltage, power, frequency and responsivity.
#pragma once
#include "hydro/units.hpp"
#include <cmath>

namespace gd::hydro {

struct ThzUnits {
  // inputs
  double n_cm2 = 1e12;
  double d_gate_m = 30e-9, eps_r = 3.8;
  double W_m = 2e-6;                 // W_ref (contact width)
  double nu = 0.1;                   // m^2/s
  double tau_mr = 5e-12, tau_ee = 1e-12;   // s
  // derived
  double Cg = 0, m0 = 0, n0 = 0, s0 = 0, q = 0, U0 = 0;
  double Gamma = 0, inv_Res = 0, t_ee = 0;
  double I_unit = 0, V_unit = 0, R_unit = 0, P_unit = 0, f_unit = 0;

  static ThzUnits make(double n_cm2, double d_gate_m, double eps_r, double W_m, double nu, double tau_mr,
                       double tau_ee) {
    using namespace units;
    ThzUnits u;
    u.n_cm2 = n_cm2; u.d_gate_m = d_gate_m; u.eps_r = eps_r; u.W_m = W_m; u.nu = nu; u.tau_mr = tau_mr; u.tau_ee = tau_ee;
    const double eps0 = 8.8541878128e-12;
    u.Cg = eps0 * eps_r / d_gate_m;
    u.n0 = n_m2(n_cm2);
    u.m0 = m_star(n_cm2);
    double s2_gate = e * e * u.n0 / (u.Cg * u.m0), s2_q = vF * vF / 2;
    u.s0 = std::sqrt(s2_gate + s2_q);
    u.q = s2_q / (s2_gate + s2_q);
    u.U0 = e * u.n0 / u.Cg;                       // gate overdrive, V
    u.Gamma = W_m / (u.s0 * tau_mr);
    u.inv_Res = nu / (u.s0 * W_m);
    u.t_ee = u.s0 * tau_ee / W_m;
    u.I_unit = e * u.n0 * u.s0 * W_m;             // A per unit nondimensional flux
    u.V_unit = u.m0 * u.s0 * u.s0 / e;            // V per unit Phi
    u.R_unit = u.V_unit / u.I_unit;
    u.P_unit = u.V_unit * u.I_unit;
    u.f_unit = u.s0 / (2 * M_PI * W_m);           // Hz per unit Omega
    return u;
  }
  double Omega(double f_Hz) const { return f_Hz / f_unit; }
  double R_nd(double R_ohm) const { return R_ohm / R_unit; }
  // Responsivities from nondimensional second-order results per Va^2 (Va nondimensional):
  // available power P_av = Va^2 V_unit^2 / (8 R_ohm).
  double amp_per_watt(double I20_per_Va2, double R_ant_ohm) const {
    return I20_per_Va2 * I_unit * 8 * R_ant_ohm / (V_unit * V_unit);
  }
  double volt_per_watt(double V20_per_Va2, double R_ant_ohm) const {
    return V20_per_Va2 * V_unit * 8 * R_ant_ohm / (V_unit * V_unit);
  }
};

}  // namespace gd::hydro
