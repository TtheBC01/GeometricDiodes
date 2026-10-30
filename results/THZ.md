# Stage H2 result: plasmonic rectification of graphene geometric diodes (2026-10-30)

Plan: `plans/hydro_thz.md`. Derivation: `plans/hydro_thz_notes.md`. Code:
`src/hydro/thz_*.{hpp,cpp}`, `apps/thz_tests.cpp`, `apps/thz_sweep.cpp`.
Logs and CSVs: `results/thz/`.

## Method in one paragraph

The Stage H electron fluid is made compressible by the gate: the channel
density follows the local electrochemical potential through the gate
capacitance and the quantum capacitance, and the two enter the momentum
equation as one barotropic potential h(n) with n h'(n) = s^2(n), the plasma
speed squared (3.6 v_F at n = 1e12 cm^-2 over 30 nm of hBN). The response to
an antenna drive is expanded to second order about rest: the first-order
problem is a complex time-harmonic plasmon problem, the second-order dc
problem is the Stage H Stokes-Brinkman operator forced by products of
first-order fields, and the rectified current or voltage is read at lumped
ports that model the antenna as a Thevenin source. Both problems are solved
with Taylor-Hood P2/P1 elements on the Stage H meshes; the port potential
enters as a boundary traction, which keeps the discrete mass balance exact.
A BDF2 solver of the full nonlinear equations confirms the perturbation
solver at small amplitude.

## Acceptance ladder (`thz/thz_tests_stdout.txt`)

Full run of `gd_thz_tests` (all nine tests, default mesh): 9 passed, 0 failed,
4767 s. The viscoelastic criterion in T3 was corrected before this run (see
deviation 5 below).

| test | what it checks | result | criterion |
|---|---|---|---|
| T1 | H2 steady solver vs Stage H: dissipation Pi, tesla1 no-slip, Re 1, M = 2.5e-4 | rel diff 5.0e-4 (M^2 = 6e-8 sets the floor of the comparison) | < 1e-3 |
| T2 | linear plasmon response vs 1/cos(kL) closed form, Gamma 0.2 | errors 6.6e-9 to 1.2e-5 over Omega 0.05 to 1.1 | < 1e-3 |
| T3 | resonance position and widths of the first mode | Omega 0.1568 (exact 0.1571); width friction 0.0202 (0.0200), with viscosity 0.0326 (0.0323); viscoelastic (Omega tau_ee = 8) 0.0203, back to the friction width | widths within 5 %, viscoelastic within 10 % of friction width |
| T4 | Dyakonov-Shur photoresponse vs the 1996 closed form and quadrature, Gamma 1 and 0.1, Omega 0.1 to 3 | errors 1.8e-7 to 2.1e-4 | < 1 % |
| T5 | funnel quasi-static I_sc/Va^2: steady Richardson curvature vs perturbation | steady 2.206e-2; P1 perturbation 2.210e-2 (Omega 0.01), 2.224e-2 (0.02) | < 3 % |
| T6 | BDF2 time domain (48 periods x 60 steps) vs second-order perturbation, Omega 1, Gamma 1 | ratios 0.9997 (Va 0.01) and 0.9994 (Va 0.03) | within 2 % |
| T7 | symmetric termination kills the circuit asymmetry: dc current of constriction and channel relative to the funnel | 5.9e-7 and 1.1e-8 vs 1.6e-3 (< 4e-4 of funnel); the same constriction with a DS termination gives 1.3e-2 | < 1e-3 of funnel |
| T8 | funnel I_sc/Va^2 vs mesh (hw, hm) = (28,168), (20,120), (14,84) nm | Omega 1: 1.584e-3, 1.601e-3, 1.570e-3 (last change 2.0 %); Omega 3: 5.92e-3, 5.84e-3, 5.78e-3 (1.1 %) | last change < 2 % |
| T9 | units: s0, U0, R_unit, f_unit, DS dU vs Ua^2/4U0 | s0 3.57e6 m/s, U0 1.429 V, R_unit 130 ohm, f_unit 0.284 THz; DS dU 1.68e-7 V vs 1.75e-7 V (graphene V_unit, 4 % apart, as the closure predicts) | dU within 10 % |

T5 also records the P2/P2 density variant at 1.82e-2, 17 % low, which is why
Taylor-Hood (P2 velocity, P1 density) is the default. T8's Omega 1 change sits
on the 2 % line: the funnel's second-order current is converged to about 2 % on
the production mesh (hw 20, hm 120 nm), and the responsivities below carry that
uncertainty.

## Deviations from the plan, and what they taught

1. **Density order.** The plan proposed P2/P1 with an optional GLS term. With
   the density gradient in strong form the dispersion error of that pair was
   only first order in h (errors of 4 to 8 % at Omega = 1 on the standard
   mesh). Integrating the gradient by parts fixed it: errors of 3e-5 on the
   coarsest mesh, second-order convergence, and no stabilisation needed.
   Equal-order P2/P2 was tried and discarded: it is accurate for the wave
   problem but loses 15 to 20 % of the quasi-static response (T5) because
   the pair is not inf-sup stable as Omega -> 0.
2. **Contact condition.** Replacing the continuity rows of contact vertices by
   a potential constraint removed global mass conservation from the test
   space; with no viscosity the dc problem then leaked 20 % of the drift flux
   through the voltage-fixed contact and T4 failed at low friction. The
   potential is now imposed as the pressure traction of the integrated-by-
   parts gradient, every continuity row is kept, and the dc flux balance
   closes to 1e-16.
3. **Time-domain relaxation.** The dc part of the density relaxes
   diffusively with time constant Gamma (2L/pi)^2, about 40 time units or six
   drive periods for the benchmark channel, so the time-domain check needs
   some fifty periods, not ten.
4. **Terminations.** An ac source at one contact with the other contact
   grounded to the gate is itself the Dyakonov-Shur asymmetry: it rectifies
   a perfectly symmetric channel (T7: eight times the funnel's own geometric
   response in the test case). The plan's "antenna across the device" is
   therefore two different experiments depending on where the gate is
   referenced. The runs report both: the symmetric split (gate at the
   midpoint potential, geometric rectification only) and the DS configuration.
5. **Viscoelastic width (T3).** The plan expected the Maxwell term to
   broaden the mode. With nu(Omega) = nu / (1 + i Omega tau_ee) the viscous
   damping is suppressed at Omega tau_ee >> 1, so the width returns to the
   friction width (0.0203 vs 0.0202) and the real part of nu(Omega) adds a
   small frequency shift instead. The criterion was rewritten to that
   expectation; the earlier scan that "failed" was the physics, not the solver.

## S1: Dyakonov-Shur detector in physical units (`thz/S1_chan_*`)

Uniform channel 1 um wide, 10 um long, ideal ac source at the source contact,
open drain, dc drain voltage per Ua^2 in units of V_unit (1.487 V for the
graphene closure, U_0 = 1.429 V for shallow water); n = 1e12 cm^-2, d = 30 nm.
Omega = f / 0.568 THz here (W_ref = 1 um). The shallow-water run is the
classical DS model; the last two columns are the DS closed form at the same
(Omega, Gamma).

| f | Omega | graphene 100 K | 100 K inviscid | shallow water 100 K | graphene 300 K | DS formula, Gamma 0.056 | DS formula, Gamma 0.28 |
|---|---|---|---|---|---|---|---|
| 10 GHz | 0.018 | 0.012 | 0.012 | 0.008 | 0.016 | 0.008 | 0.018 |
| 33 GHz | 0.058 | 0.158 | 0.159 | 0.110 | 0.151 | 0.110 | 0.163 |
| 108 GHz | 0.190 | 1.875 | 1.875 | 1.352 | 0.382 | 1.353 | 0.459 |
| 279 GHz | 0.490 | 3.450 | 3.412 | 2.489 | 0.612 | 2.490 | 0.704 |
| 914 GHz | 1.609 | 0.091 | 0.110 | 0.102 | 0.499 | 0.102 | 0.615 |
| 3 THz | 5.280 | 0.443 | 0.230 | 0.189 | 0.565 | 0.187 | 0.647 |

Reading:

1. The shallow-water run reproduces the Dyakonov-Shur formula to three digits
   at every frequency in physical units, including the resonant peaks
   (k'L = pi/2, 3pi/2, ... at 89, 270, ... GHz for tau_mr = 5 ps) and the
   broadband plateau at 300 K (omega tau < 1 below 160 GHz). This is the
   ladder's T4 again, now through the units layer.
2. The graphene closure (m* ~ sqrt(n), quantum capacitance) raises the
   response by 40 % near the resonances and up to a factor two off
   resonance. The difference is the nonlinearity of the Dirac fluid itself:
   the second-order coefficients h''(1) = -(1+q)/2 and Phi''(1) = -q/2 are
   absent in the shallow-water model.
3. Viscosity (nu = 0.1 m^2/s) is invisible below 1 THz in a 1 um channel and
   changes the 3 THz value by a factor two, where omega tau_ee = 19 and the
   Maxwell form makes the viscous term reactive; that point is indicative only.
4. In voltage responsivity (drain dc voltage per available power of a 75 ohm
   source) the 100 K resonant peaks reach 2.0e3 V/W (448 GHz) and the 300 K
   plateau is 150 to 270 V/W, i.e. the textbook 2 R_ant / U_0 = 105 V/W
   times f(omega). These are upper bounds: ideal voltage drive, no contact
   resistance, no parasitic capacitance.

## S2: funnel as a rectenna element (`thz/S2_funnel*`, figures `S2_funnel_RI.png`, `S2_funnel_short_RI.png`, `S2_funnel_w1000_RI.png`)

Three funnels, all 30 deg taper to the neck with an abrupt shoulder, 75 ohm
antenna, n = 1e12 cm^-2, d = 30 nm; 100 K: tau_mr 5 ps, tau_ee 1 ps,
nu 0.1 m^2/s; 300 K: 1 ps, 0.1 ps, 0.03. "sym" is the symmetric split of the
antenna (geometric rectification only); "DS" is the source through R_ant
with the drain grounded to the gate. R_I is the short-circuit dc current per
available power, in A/W. The full tables with R_V, NEP and impedance are in
`thz/tables.md`.

| device | set | term | 10 GHz | 100 GHz | 300 GHz | 1 THz | peak |R_I| (at) | R_dc | power absorbed |
|---|---|---|---|---|---|---|---|---|---|
| 175 nm neck, 6 um leads | 100 K | sym | 3e-4 | 6e-3 | -5e-3 | 0.21 | 0.22 (1.87 THz) | 1.9 kOhm | 15-80 % |
| | 100 K | DS | 2.6e-2 | 0.16 | 8e-2 | 0.32 | 0.45 (1.87 THz) | | |
| | 300 K | sym | -4e-6 | 5e-5 | 6e-3 | 2.7e-2 | 2.7e-2 (0.9 THz) | 1.2 kOhm | |
| | 300 K | DS | 3.8e-2 | 8.9e-2 | 7.1e-2 | 0.10 | 0.10 (1.87 THz) | | |
| 175 nm neck, 1 um leads (control) | 100 K | sym | 2.7e-4 | 1.7e-3 | 2.9e-2 | 9.6e-2 | 0.32 (1.87 THz) | 1.8 kOhm | 7-60 % |
| | 100 K | DS | 2.6e-2 | 4.7e-2 | 0.22 | -3.6e-2 | 0.51 (1.87 THz) | | |
| | 300 K | sym | -4e-4 | 5.8e-3 | 4.4e-2 | -7.2e-2 | 0.14 (1.87 THz) | 0.84 kOhm | |
| | 300 K | DS | 5.0e-2 | 6.7e-2 | 0.16 | 0.14 | 0.32 (1.87 THz) | | |
| 1 um neck, 4 um leads | 100 K | sym | -3e-5 | 2.7e-2 | 8.0e-2 | 0.13 | 0.15 (0.45 THz) | 168 ohm | 79-95 % |
| | 100 K | DS | 0.15 | 0.24 | 0.21 | 0.34 | 0.34 (0.9 THz) | | |
| | 300 K | sym | -8e-6 | 3.5e-4 | 2.5e-3 | 2.4e-3 | 5.0e-3 (0.45 THz) | 579 ohm | 41-92 % |
| | 300 K | DS | 7.0e-2 | 8.5e-2 | 9.0e-2 | 8.9e-2 | 9.2e-2 (0.45 THz) | | |

Reading:

1. **Below 100 GHz the geometric rectification is the Stage H curvature and
   it is small.** 1e-4 to 1e-3 A/W at 100 K, 1e-5 to 1e-4 at 300 K, for the
   175 nm neck; the quasi-static ladder test (T5) ties these to the steady
   I-V. This is the regime where the steady Stage H verdict applies and it
   says the same thing: the electron-fluid funnel is a weak dc rectifier.
2. **Above 300 GHz the funnel becomes a plasmonic device and its geometric
   response rises by two to three orders of magnitude.** At 100 K the
   175 nm funnel reaches 0.2 to 0.3 A/W at 1.87 THz with either lead length,
   and the fine scan (`S2_funnel_short_100K_sym_fine`) shows why: at that
   frequency the taper's impedance passes through a series resonance, Re Z
   = 57 ohm and Im Z = -8 ohm against the 75 ohm antenna, so the horn is
   matched and the asymmetric standing wave in the taper drives the
   second-order flow. The peak is intrinsic to the taper, not to the 6 um
   leads (the control with 1 um leads has it at the same frequency and
   amplitude), and it survives at 300 K at 0.03 to 0.14 A/W.
3. **The circuit asymmetry dominates the geometric one at room
   temperature.** With the DS termination every funnel gives 0.04 to 0.1 A/W
   flat from 10 GHz to 3 THz at 300 K, ten to forty times its geometric
   response at the same frequency, and the sign follows the termination
   (dss and dsd are near mirror images). A graphene funnel rectenna at
   300 K is a Dyakonov-Shur detector whose shape adds a factor of order one
   near the taper resonance; the shape alone does not carry it.
4. **The 1 um device is the one an antenna can drive.** Its impedance is 80
   to 200 ohm across the band, so 80 to 95 % of the available power is
   absorbed, against 15 to 60 % for the 175 nm neck at 1 to 2 kOhm. Its DS
   response is 0.15 to 0.34 A/W at 100 K and 0.07 to 0.09 A/W at 300 K with
   no resonant structure to tune; its geometric response at 100 K reaches
   0.13 A/W at 1 THz, comparable to the DS term there, but collapses to a
   few mA/W at 300 K because the inertial term scales with Re and the
   viscous and friction damping at 300 K kill the plasmon.
5. **Temperature enters through tau_mr.** omega tau_mr crosses one at 32 GHz
   (100 K) and 160 GHz (300 K); above it the device is a resonator and the
   responsivity is set by matching and Q, below it by the drift
   nonlinearity. The 100 K curves are structured because the 6 um leads and
   the taper are cavities with Q ~ 5; the 300 K curves are smooth.

Mesh: hw 14 / hm 84 nm (T8: 1 to 2 % between the two finest meshes at
Omega 1 and 3, i.e. 0.3 and 0.9 THz); above 2 THz the plasmon wavelength
(1.2 um at 3 THz) is resolved by 14 elements per wavelength in the leads and
the values are good to a few per cent, not better.

## S3: classical Tesla valve (`thz/S3_tesla_classic_*`, figure `S3_tesla_classic_RI.png`)

The approved four-loop valve of `CLASSIC.md` (175 nm channel, 6 um leads,
W_ref = 2 um, mesh hw 14 / hm 84 nm), 75 ohm antenna, R_I in A/W:

| set | termination | 10 GHz | 100 GHz | 300 GHz | 1 THz | peak |R_I| | at | R_dc |
|---|---|---|---|---|---|---|---|---|
| 100 K | symmetric (geometry only) | 6e-6 | 2e-5 | -8e-4 | 1.0e-2 | 1.7e-2 | 721 GHz | 10.9 kOhm |
| 100 K | DS (source R, drain to gate) | 4.7e-3 | 2.5e-2 | 1.5e-2 | 1.8e-2 | 0.11 | 721 GHz | 10.9 kOhm |
| 300 K | symmetric | -4e-6 | -2e-5 | 1.3e-4 | 1.5e-4 | 1.3e-3 | 1.47 THz | 6.0 kOhm |
| 300 K | DS | 8.5e-3 | 2.2e-2 | 1.4e-2 | 1.3e-2 | 2.3e-2 | 448 GHz | 6.0 kOhm |

Reading:

1. **The Tesla geometry contributes almost nothing at frequency.** Its own
   rectification is 1e-5 to 1e-3 A/W below 300 GHz and reaches 1e-2 only
   at a 100 K plasmon resonance near 720 GHz, ten to twenty times below
   the funnel at every frequency and temperature (S2). The low-frequency
   values are the quasi-static Stage H curvature: at 10 GHz and 300 K the
   valve is 4e-6 A/W, the funnel 3e-4.
2. **Its impedance is the wrong order of magnitude.** 6 to 11 kOhm against
   a 75 ohm antenna; the absorbed fraction of the available power is 3 to
   50 % and the series resistance of five micrometres of 175 nm channel
   dominates the dc readout. Whatever the DS termination adds (2e-2 to 0.1
   A/W, all of it the circuit asymmetry, none of it the loops) the funnel
   does better at a tenth of the resistance.
3. This closes the Tesla valve for the rectenna in the third and last
   regime: ballistic (null), steady hydrodynamic (a few per cent within the
   Fermi-liquid window) and plasmonic (an order of magnitude below the
   funnel). The reason is the same in each: the valve's asymmetry is an
   inertial loss that needs Re >~ 10, and at frequency the long flow path
   that produces it is also five times the plasmon wavelength, so the
   stages do not act coherently.

## S4: the rectenna number

Noise-equivalent power from the Johnson noise of the measured dc resistance
alone, NEP = sqrt(4 k_B T / R_dc) / R_I for current readout (identical for
voltage readout when R_V = R_I R_dc, which holds here to within the
open/short difference), 75 ohm antenna, no contact resistance, no 1/f noise,
no amplifier noise, perfect coupling. The ballistic-literature comparison
uses the quasi-static estimate from `KILLTEST.md`.

| device, set | termination | best R_I (at) | NEP at best (W/Hz^1/2) | NEP at 1 THz | NEP at 300 GHz |
|---|---|---|---|---|---|
| 1 um funnel, 100 K | DS | 0.34 A/W (0.9 THz) | 1.7e-11 | 1.7e-11 | 2.7e-11 |
| 1 um funnel, 100 K | geometric | 0.15 A/W (0.45 THz) | 3.9e-11 | 4.3e-11 | 7.2e-11 |
| 1 um funnel, 300 K | DS | 0.09 A/W (0.45 THz) | 5.8e-11 | 6.0e-11 | 5.9e-11 |
| 175 nm funnel, 100 K | DS | 0.45 A/W (1.87 THz) | 3.8e-12 | 5.3e-12 | 2.0e-11 |
| 175 nm funnel, 100 K | geometric | 0.22 A/W (1.87 THz) | 7.9e-12 | 8.2e-12 | 3.5e-10 |
| 175 nm funnel, 300 K | DS | 0.10 A/W (1.87 THz) | 3.6e-11 | 3.6e-11 | 5.2e-11 |
| 175 nm funnel, 300 K | geometric | 0.027 A/W (0.9 THz) | 1.4e-10 | 1.4e-10 | 6.2e-10 |
| classical Tesla, 300 K | geometric | 1.3e-3 A/W (1.5 THz) | 1.2e-9 | 1.1e-8 | 1.3e-8 |

Reading:

1. **Idealised, a graphene funnel element with the DS termination reaches
   4 to 60 pW/Hz^1/2 across 0.3 to 2 THz**, the lower figure at 100 K with
   the 175 nm neck at its taper resonance, the upper at 300 K. Reported
   room-temperature graphene THz detectors sit at hundreds of pW/Hz^1/2 to
   nW/Hz^1/2 with responsivities of 0.1 to 30 V/W; our 50 to 300 V/W at
   300 K and the corresponding NEP are therefore upper bounds by one to two
   orders, the gap being contact resistance, antenna coupling and the
   electrostatics the local-capacitance model idealises, not the
   mechanism.
2. **The geometric diode contributes at 100 K near the taper resonance and
   nowhere else.** At 300 K the shape-only NEP is 140 pW/Hz^1/2 at best,
   four times worse than the circuit term of the same device; at 100 K the
   shape-only 175 nm funnel at 1.87 THz (8 pW/Hz^1/2) is within a factor two
   of the DS value, and that is the one place where the funnel geometry
   earns its place in a plasmonic design: as a matched horn, not as a
   Borda-Carnot diode.
3. **Against the ballistic estimate.** The room-temperature ballistic
   asymmetry A = 1.62 at 0.1 V (`KILLTEST.md`) corresponds to a quasi-static
   curvature responsivity of order 1 to 3 A/W if it held at THz, five to
   thirty times the hydrodynamic numbers here. Whether it holds at THz is
   the transit-time question outside this model (0.5 ps across the neck,
   i.e. a roll-off near 300 GHz if the field must be quasi-static over the
   transit): it is the remaining route to a shape-based rectenna and the
   natural Stage B2.
4. **What this stage decides.** In the hydrodynamic-plasmonic picture a
   graphene geometric-diode rectenna at 1 THz is possible on paper at the
   10 to 100 pW/Hz^1/2 level, but the rectification is the Dyakonov-Shur
   circuit asymmetry of a gated channel with a well-matched (1 um class)
   impedance, and the Tesla valve adds nothing. The funnel shape matters as
   an impedance transformer at its taper resonance, which is a design
   handle, and as a dc diode only at 100 K.

## Limitations

Local-capacitance electrostatics (d = 30 nm against a 175 nm neck is the
worst case); no electron heating or thermoelectric response; the ballistic
transit-time nonlinearity of the funnel is outside the fluid model; the
antenna enters only as R_ant and the available power; the hydrodynamic
description is quantitative to about 300 GHz at 100 K and 1 THz at 300 K
(omega tau_ee = 1), beyond which the viscous term is carried by the Maxwell
form and the results are indicative.
