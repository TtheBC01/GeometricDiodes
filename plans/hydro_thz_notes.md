# Stage H2 derivation notes (model, nondimensional form, perturbation, discretisation)

Companion to `plans/hydro_thz.md`. Everything the code in `src/hydro/thz_*`
implements is derived here so the acceptance ladder can be read against it.

## 1. Barotropic form of the gated graphene fluid

Unknowns: density n(x,t), drift velocity u(x,t). Gradual-channel
electrostatics: channel potential energy e^2 n / C_g. Degenerate Dirac
carriers: chemical potential mu(n) = hbar v_F sqrt(pi n), Fermi-liquid mass
m*(n) = hbar sqrt(pi n) / v_F. The electrochemical potential is

    Phi(n) = e^2 n / C_g + mu(n)              (contact voltage V = Phi / e)

and the force per unit mass is -(1/m*) grad Phi. Because both Phi and m*
depend on n only, this is the gradient of a single function h(n):

    h'(n) = Phi'(n) / m*(n) = e^2 v_F / (C_g hbar sqrt(pi n)) + v_F^2 / (2 n)
    s^2(n) := n h'(n) = e^2 n / (C_g m*(n)) + v_F^2 / 2

so the momentum equation is barotropic,

    d_t u + (u.grad) u + u / tau = -grad h(n) + nu lap u,

with the kinematic viscosity nu taken constant (dynamic viscosity n m* nu;
the alternative closure differs at second order by a term of relative size
1/Re_s and is not pursued). Zero bulk viscosity in 2D makes the viscous
force exactly nu lap u. Continuity: d_t n + div(n u) = 0.

Shallow-water (classical Dyakonov-Shur) closure for the benchmark: constant
m, Phi = e^2 n / C, so h = s_0^2 n / n_0 and s^2 = s_0^2 n / n_0.

## 2. Nondimensional system

Reference: density n_0, plasma speed s_0 = s(n_0), length W_ref (contact
width, as in Stage H), time W_ref / s_0, h in units of s_0^2. With tildes
dropped:

    d_t n + div(n u) = 0
    d_t u + (u.grad) u + Gamma u = -grad h(n) + (1/Re_s) lap u

    Gamma = W_ref / (s_0 tau_mr)        friction
    Re_s  = s_0 W_ref / nu              plasma Reynolds number
    q     = (v_F^2 / 2) / s_0^2         quantum-capacitance fraction of s_0^2
    t_ee  = s_0 tau_ee / W_ref          collision time (viscoelastic viscosity)
    Omega = omega W_ref / s_0           reduced frequency

Closures (h(1) = 0, h'(1) = 1 in both):

    shallow water:  h(n) = n - 1,                        h''(1) = 0
    graphene:       h(n) = 2(1-q)(sqrt n - 1) + q ln n,  h''(1) = -(1+q)/2 =: kappa

Contact voltage in units of m_0* s_0^2 / e: Phi(n)/ (m_0* s_0^2), with
Phi'(1) = 1 and Phi''(1) = 0 (shallow water) or -q/2 (graphene).

Unit conversions (n_0, W_ref, s_0, m_0*): current I = e n_0 s_0 W_ref x
(integral of n u.n over the contact), voltage V = (m_0* s_0^2 / e) x Phi,
resistance R_0 = m_0* s_0 / (e^2 n_0 W_ref) (131 ohm at n_0 = 1e12 cm^-2,
W_ref = 2 um, s_0 = 3.6e6 m/s), power P_0 = m_0* n_0 s_0^3 W_ref,
frequency f = Omega s_0 / (2 pi W_ref) (0.29 THz per unit Omega for
W_ref = 2 um).

Viscoelastic viscosity, e^{i omega t} convention: nu(omega) = nu / (1 + i
Omega t_ee). Real part is the dissipative viscosity, imaginary part a
reactive (pressure-like) term that dominates once Omega t_ee > 1.

## 3. Perturbation about rest

n = 1 + Re(n_1 e^{i Omega t}) + n_20 + ..., u = Re(u_1 e^{i Omega t}) + u_20 + ...
(amplitude absorbed in the complex fields; second harmonic not needed).

First order (complex, linear):

    i Omega n_1 + div u_1 = 0
    i Omega u_1 + Gamma u_1 + grad n_1 - nu(Omega) lap u_1 = 0

Second order, time average (real, steady), using <AB> = Re(A_1 B_1*)/2:

    div u_20 = g,          g := -Re[ div(n_1 u_1*) ] / 2
    Gamma u_20 + grad P - (1/Re_s) lap u_20 = f,   f := -Re[ (u_1* . grad) u_1 ] / 2
    P := n_20 + (kappa / 4) |n_1|^2          (dc part of h to second order)

Static viscosity at dc. The dc problem is the Stokes-Brinkman operator of
Stage H with a prescribed divergence and a body force. The dc contact
voltage (dc part of Phi to second order) is

    Phi_20 = n_20 + (Phi''(1) / 4) |n_1|^2 = P + c_V |n_1|^2,
    c_V = -(kappa / 4) + Phi''(1) / 4 = 0 (shallow water), 1/8 (graphene).

Particle flux through a contact (inward normal n_in): first order
I_1 = int u_1 . n_in ds; dc I_20 = int (u_20 + Re(n_1 u_1*)/2) . n_in ds.

## 4. Ports

Each contact c is an equipotential: n_1 = V_1c and Phi_20 = V_20c along it
(V_1c complex, V_20c real, one extra unknown each). Kirchhoff at the
terminal, with I_c the flux into the device:

    voltage source with series R:  R I_c + V_c = V_a,c     (R = 0: ideal source; V_a = 0: ground)
    current source:                I_c = I_0,c
    open:                          I_c = 0

The same forms hold at first order (ac Thevenin: V_a, R_ant) and at dc
(load R_L to ground, V_a = 0: R_L I_20 + V_20 = 0; R_L = 0 gives the
short-circuit current, open gives the open-circuit voltage). For the
Dyakonov-Shur benchmark the source is an ideal ac voltage source with dc
value fixed (V_a at first order, ground at dc) and the drain is open at
both orders; the response is V_20 at the drain.

Available power of the Thevenin source: P_av = |V_a|^2 / (8 R_ant).
Responsivities: R_I = I_20,sc / P_av, R_V = V_20,oc / P_av, both
independent of |V_a| at second order.

## 5. Discretisation

P2 velocity (Cartesian at interior nodes, rotated tangential/normal frame at
wall nodes with the Engelman-Sani-Gresho length-weighted normal, exactly as
Stage H), P1 density. The density gradient is kept in strong form in the
momentum equation, so the only natural boundary term is the viscous
traction, which is dropped at contacts (do-nothing). Continuity is tested
with P1 functions in strong divergence form. At a contact, the continuity
rows of its vertices are replaced by n_v - V_c = 0 and one port row
enforces Kirchhoff with the flux written as the edge integral of the P2
velocity. Wall nodes: u . n = 0 strongly, Navier slip as the edge integral
(1/S) int (u.t)(v.t) ds, corners sharper than 25 deg pinned.

First-order system: complex sparse LU (Eigen) on 2 nn + nv + n_ports
unknowns, pattern analysed once per mesh, refactorised per frequency.
Second-order system: real, same pattern, one factorisation per geometry
and parameter set. Time domain (check only): BDF2 on the full nonlinear
system with Newton per step and the port constraint evaluated at t^{k+1}.

No stabilisation is used; the ladder shows the plasmon wavelength is
resolved by a factor of thirty or more on the standard meshes.

## 6. Static limit used in T1

With a dc current source I_0 and a grounded drain, the steady nonlinear
problem at small Mach number M = I_0 (nondimensional) reduces to the Stage H
problem at Re = M Re_s with pressure p = h. Stage H's Pi (pressure drop per
unit flux in units m* nu U / W) relates to the dc voltage drop here by

    Delta h = Pi (1/Re_s) M,    i.e.   Pi_H2 := Delta h Re_s / M = Pi_H(Re = M Re_s)

to O(M^2), which is the pass criterion (M = 0.01 gives 1e-4).

## 7. Dyakonov-Shur reference (shallow water, 1D, friction only)

Channel of length L, source potential U_0 + U_a cos(omega t), drain open.
With k = (omega / s) sqrt(1 - i / (omega tau)) = k' - i k'':

    k'  = (omega / s) sqrt[ (sqrt(1 + 1/(omega tau)^2) + 1) / 2 ]
    k'' = (omega / s) sqrt[ (sqrt(1 + 1/(omega tau)^2) - 1) / 2 ]
    Delta U = (U_a^2 / 4 U_0) f(omega)
    f = 1 + beta - (1 + beta cos 2k'L) / (sinh^2 k''L + cos^2 k'L),  beta = 2 omega tau / sqrt(1 + (omega tau)^2)

(Dyakonov and Shur, IEEE Trans. Electron Devices 43, 380 (1996)). Limits:
f -> 1 for omega tau << 1 and k''L >> 1 (broadband detector), resonant
peaks at k'L = (2j+1) pi/2 for omega tau >> 1. The ladder checks the 2D
solver against this formula and, independently, against a 1D finite
difference solution of the same second-order equations, so a transcription
error in the formula cannot pass as a solver error or vice versa.
