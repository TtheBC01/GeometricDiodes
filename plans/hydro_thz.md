# Plan: unsteady gated-compressible electron hydrodynamics (Stage H2)

Written 2026-10-30, before any code. Companion to `plans/hydro_tesla.md`
(Stage H, steady incompressible) and `results/CLASSIC.md`. Stage H answered
the dc question: a geometric diode in the electron fluid rectifies through
inertial losses that scale as Re^2, and within the currents graphene
survives that is a few per cent for the classical Tesla valve and about a
per cent for the funnel. Stage H2 asks the question the rectenna goal
actually depends on: **when the drive is a terahertz field from an antenna
rather than a dc current, how much dc current or voltage does a graphene
geometric diode deliver per watt, how does that fall off with frequency,
and what sets the roll-off?**

## 0. The numbers that frame everything

Gated graphene is a compressible electron fluid: the local density follows
the local potential through the gate capacitance, so density disturbances
travel as plasma waves. At n = 1e12 cm^-2 (m* = 0.0205 m_e, E_F = 117 meV)
over d = 30 nm of hBN (eps_r = 3.8, C_g = 1.1e-3 F/m^2):

    s^2 = e^2 n / (C_g m*) + v_F^2 / 2       (gate term + quantum capacitance)
    s   = 3.6e6 m/s = 3.6 v_F

| scale | 175 nm neck device (L 6 um) | 1 um channel (L 10 um) |
|---|---|---|
| plasmon quarter-wave resonance s / 4L | 0.15 THz | 0.09 THz |
| viscous time W^2 / nu (nu 0.1 m^2/s) | 0.3 ps -> 500 GHz | 10 ps -> 16 GHz |
| momentum relaxation tau_mr (hBN, 100 K / 300 K) | 5 ps / 1 ps | same |
| omega tau_mr at 1 THz | 30 / 6 | 30 / 6 |
| electron-electron time tau_ee (100 K / 300 K) | 1 ps / 0.1 ps | same |
| omega tau_ee at 1 THz | 6 / 0.7 | 6 / 0.7 |
| ballistic transit of the asymmetric region at v_F | 0.5 ps (neck) | 3 ps |

Three things follow and shape the plan.

1. **At 1 THz the drive is faster than momentum relaxation** (omega tau_mr
   of 6 to 30), so the device is in the resonant plasmonic regime of
   Dyakonov and Shur, not the overdamped drift regime that the dc picture
   assumes. Electron inertia (kinetic inductance) and gate capacitance form
   a transmission line and the whole device is a plasmonic cavity.
2. **At 1 THz the electron fluid is only marginally a fluid.** omega tau_ee
   is below one only at room temperature; at 100 K the carriers are
   collisionless over a period. The long-wavelength Euler equations
   (continuity, momentum with pressure and friction) remain valid in both
   limits and are what the graphene THz-detector literature uses (Tomadin
   and Polini 2013; Bandurin et al. 2018); only the viscous term changes
   character, becoming reactive when omega tau_ee > 1. The model therefore
   carries a Maxwell viscoelastic viscosity nu(omega) = nu / (1 - i omega
   tau_ee), which is free in a frequency-domain solver.
3. **Small signal is the regime that matters.** A rectenna element sees
   nanowatts to microwatts, i.e. ac amplitudes far below the gate overdrive
   U_0 = e n / C_g = 1.4 V. The quantity to compute is the second-order
   (quadratic) response, which is a perturbation problem, not a
   large-amplitude time integration.

Two benchmarks exist in closed form and fix the solver before any geometry
is touched: the Dyakonov-Shur photoresponse of a uniform gated channel
(ac voltage at source, open drain, friction tau, arbitrary omega), and the
quasi-static limit, where the dc response must equal (1/4) I''(V) V_a^2 with
I(V) the steady characteristic that Stage H already produced.

## 1. Model

Unknowns: carrier density n(x, t) and drift velocity u(x, t) on the 2D
device domain. Local-capacitance (gradual-channel) electrostatics: the
channel potential is phi = e n / C_g relative to the gate, valid when the
gate distance d is small against every in-plane length (30 nm against
175 nm is the worst case here; the correction is a logarithmic factor in
the long-wavelength limit and is noted, not modelled).

    d_t n + div(n u) = 0
    d_t u + (u . grad) u + u / tau_mr = -(1 / m*) grad[ e^2 n / C_g + mu(n) ] + (1/n) div(sigma_visc)

with mu(n) = hbar v_F sqrt(pi n) the chemical potential (this is the
quantum-capacitance term; its gradient gives the v_F^2 / 2 in s^2),
m* = hbar sqrt(pi n) / v_F the density-dependent Fermi-liquid mass, and the
2D viscous stress sigma = n m* nu [grad u + grad u^T - (div u) I] with zero
bulk viscosity (Dirac fluid, scale invariance). Setting d_t = 0 and
n = const recovers Stage H exactly, which is the first regression test.

Nondimensionalisation: lengths by W_ref (contact width, as in Stage H),
velocity by s_0 (plasma speed at n_0), time by W_ref / s_0, density by
n_0. Dimensionless groups: Mach number M = u_0 / s_0 of the dc bias (zero
for a pure rectenna), reduced frequency Omega = omega W_ref / s_0, friction
Gamma = W_ref / (s_0 tau_mr), viscosity 1 / Re_s = nu / (s_0 W_ref),
collision parameter omega tau_ee, and the quantum-capacitance ratio
C_g / C_q = (e^2 n / C_g) / (m* v_F^2 / 2), which fixes how much of s^2 is
gate and how much is pressure.

Boundary conditions.

* Walls: u . n = 0 and Navier slip with slip length l_s, as in Stage H
  (perfect slip, l_s / W_ref = 0.5 and 0.1 as the three cases).
* Contacts, benchmark form (Dyakonov-Shur): source n = n_0 (1 + a cos
  omega t) (ac gate-to-channel voltage), drain n u . n = 0 (open circuit).
* Contacts, rectenna form: the antenna is a Thevenin source V_a cos omega t
  with resistance R_ant (75 ohm dipole, or swept) connected across source
  and drain; the dc output is read either as open-circuit dc voltage or as
  dc current into a load R_L. Implemented as lumped-element boundary
  conditions: the contact potential is one extra scalar unknown per
  contact, coupled to the integrated flux through the contact (Kirchhoff),
  exactly as a circuit port in an electromagnetic FEM. The contact itself
  is taken as an equipotential with no contact resistance (contact
  resistance can be added to R_ant).

What is deliberately left out, with the reason: nonlocal electrostatics
(needs a 3D Poisson solve; the local approximation is the standard one and
its error is known), electron heating and the bolometric/thermoelectric
response (a separate mechanism that the experiments on graphene THz
detectors have to subtract; worth its own stage if the plasmonic numbers
look promising), the ballistic transit-time nonlinearity of the funnel at
THz (a Boltzmann problem, outside any fluid model, see section 6), and the
3D antenna electromagnetics (enters only as R_ant and the available power).

## 2. Solver

Primary method: **frequency-domain perturbation to second order.** Expand
about the dc operating point (n_0, u_0 = 0 for the unbiased rectenna):

    n = n_0 + eps n_1 e^{i omega t} + eps^2 (n_20 + n_22 e^{2 i omega t}) + c.c. + ...

and likewise for u and the port potentials. Order eps is the linear
plasmonic response: one complex, indefinite, Helmholtz-like linear system
in (n_1, u_1) per frequency. Order eps^2, dc part, is a real steady
problem forced by products of first-order fields (the Reynolds-stress-like
terms u_1^* . grad u_1, div(n_1 u_1^*), and the friction and viscous
cross terms). Its solution n_20, u_20 gives the rectified dc current or
voltage directly. This is the way the Dyakonov-Shur result itself is
derived, it is the small-signal regime a rectenna lives in, it costs two
linear solves per frequency instead of thousands of time steps, it makes
the viscoelastic nu(omega) trivial, and it is the natural tool for someone
who thinks in harmonic balance. The dc problem is the Stage H operator
linearised about rest, so the Stokes-Brinkman machinery is reused as is.

Secondary method: **time domain**, BDF2 with Newton per step on the full
nonlinear system, used only to confirm the perturbation solver at two or
three (geometry, frequency) points and to show where the response leaves
the quadratic regime as the amplitude grows. Same spatial discretisation.

Discretisation: triangular P2 velocity / P1 density, as in Stage H. The
compressible system has no inf-sup constraint (density has its own time
derivative), but the first-order problem is wave-like and the second-order
convective terms need the same care as before; residual-based (GLS)
stabilisation is added on the density equation and switched off in the
ladder to show it changes nothing at the resolutions used. Complex systems
are solved as doubled real systems with Eigen SparseLU, reusing the
analysed pattern across frequencies; at ~17 000 nodes that is ~100 000
real unknowns, seconds per frequency.

Meshes: the existing gmsh pipeline and the three approved geometries
(funnel, classical Tesla at 175 nm and 1 um). One new geometry, a uniform
gated channel for the Dyakonov-Shur benchmark, is trivial but will still be
drawn and shown before it is run, per the standing rule.

Outputs per frequency: complex port admittance Y(omega) (which also gives
the plasmon resonances and the device impedance for antenna matching),
dc short-circuit current I_dc and open-circuit voltage V_dc per unit V_a^2,
current responsivity R_I = I_dc / P_avail and voltage responsivity
R_V = V_dc / P_avail with P_avail = V_a^2 / (8 R_ant), and field maps of
|n_1|, |u_1| and the dc streaming flow u_20.

## 3. Acceptance ladder (all must pass before any device number is quoted)

| test | setup | pass criterion |
|---|---|---|
| T1 | static limit: omega = 0, n uniform, dc bias | reproduces Stage H Pi(Re) for tesla1 and funnel to 1e-8 (same operator) |
| T2 | linear plasmon dispersion, uniform channel, no friction, no viscosity | port admittance poles at omega_k = (2k + 1) pi s / 2L for the DS boundary conditions, s from the gate + quantum-capacitance formula, to 0.5 % after mesh refinement |
| T3 | damping | resonance half-width 1 / (2 tau_mr) with friction only; additional nu k^2 width with viscosity; Maxwell nu(omega) reduces to the collisionless reactive limit as omega tau_ee -> inf |
| T4 | Dyakonov-Shur photoresponse, uniform channel, friction only | dc voltage matches the closed-form DS expression for arbitrary omega tau and omega L / s (nonresonant and resonant branches) to 1 % |
| T5 | quasi-static limit, funnel and classical Tesla | omega -> 0 dc response equals (1/4) I''(V) V_a^2 with I'' from the Stage H curvature (the even part of Pi(Re)) to 3 %, the mesh-convergence level of Stage H |
| T6 | time domain against perturbation | at two frequencies and small amplitude the BDF2 dc response equals the second-order result to 1 % and scales as V_a^2 over a decade of amplitude |
| T7 | symmetry floor | symmetric constriction gives zero dc response; the residual sets the numerical floor (expected ~1e-6 of the funnel value) |
| T8 | convergence | mesh (three levels) and, for T6, time step; stabilisation on/off differs by less than the mesh error |
| T9 | units | R_I in A/W and R_V in V/W for a stated antenna reproduce the DS textbook estimate 2 R_ant / U_0 for the matched nonresonant channel |

## 4. Science runs, in order

* **S1, Dyakonov-Shur reproduction.** Uniform 1 um channel, 50 GHz to
  3 THz, tau_mr 0.5 to 5 ps, with and without viscosity. Establishes the
  solver against the one case the field agrees on and shows the resonant
  and overdamped regimes in our own units.
* **S2, funnel as a rectenna element.** Nguyen device D geometry (175 nm
  neck) and its 1 um scaling, antenna drive across the contacts with
  R_ant = 75 ohm, 10 GHz to 3 THz, at the two parameter sets (300 K: tau_mr
  1 ps, tau_ee 0.1 ps, nu 0.03; 100 K: tau_mr 5 ps, tau_ee 1 ps, nu 0.1),
  three slip conditions. Deliverable: R_I(omega) and R_V(omega) with the
  roll-offs identified as viscous (W^2 / nu), convective, or plasmonic,
  and the device impedance Z(omega) for matching.
* **S3, classical Tesla valve.** Same drive, 175 nm and 1 um versions, at
  the frequencies where S2 shows the funnel's response and where it rolls
  off. Settles whether the Tesla's stronger dc inertial term (CLASSIC.md)
  survives at frequency or is killed first by the longer flow path.
* **S4, the rectenna number.** For the best case of S2 and S3: responsivity
  and noise-equivalent power at 300 K (Johnson noise of the device
  resistance) against frequency, compared with (a) the ballistic
  geometric-diode literature values and (b) the quasi-static estimate from
  Stage H plus an RC cut-off. This is the go/no-go for a graphene
  geometric-diode rectenna in the hydrodynamic picture.

## 5. Effort and risks

Code: the first-order complex solver and the second-order forcing terms
(~900 lines, mostly new assembly on the existing P2/P1 machinery), lumped
port boundary conditions (~150), the BDF2 time stepper (~300), the DS
closed-form reference and the ladder (~500), sweeps and plots (~300).
Comparable to Stage H.

Risks, in order of likelihood:

1. The quasi-static test T5 disagrees with Stage H by more than the mesh
   error. That would mean the second-order forcing terms are incomplete
   (there are eleven of them) and would be found by T6. The test is designed
   to catch exactly this.
2. The local-capacitance approximation at d / W = 30 / 175 is poor near the
   neck. Checked by varying d; if the plasmon speed in the neck depends on
   d more than the formula predicts, a nonlocal correction is needed and
   the stage grows.
3. The responsivity comes out small for an uninteresting reason: the
   device impedance is kilohms against a 75 ohm antenna, so most of the
   available power is reflected. This is real and is reported as such, with
   the matched-impedance responsivity given alongside so the device physics
   and the matching problem are separated.
4. At 1 THz and 100 K the fluid picture is marginal (omega tau_ee = 6). The
   Maxwell viscosity covers the viscous term; the convective nonlinearity
   itself is a long-wavelength statement and survives, but the result at
   1 THz is quoted as indicative, with the quantitative claim stopping at
   ~300 GHz at 100 K and ~1 THz at 300 K.

## 6. What this stage does not answer

The ballistic funnel's dc asymmetry (A = 1.62, KILLTEST.md) is a
bias-window effect set over the electron transit through the asymmetric
region, 0.5 ps across the neck. Whether that mechanism survives at 1 THz
is a time-dependent Boltzmann question, outside any fluid model, and would
be a separate stage (a time-dependent version of the Stage 1 Monte Carlo
with a harmonic field). Stage H2 settles the plasmonic-hydrodynamic
mechanism; if it delivers a useful responsivity the ballistic question
becomes moot, and if it does not, the ballistic stage is the remaining
route to a graphene geometric-diode rectenna.
