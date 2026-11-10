# Stage H2, H3 and H4 results: plasmonic rectification of graphene geometric diodes (2026-10-30 to 2026-11-08)

Plan: `plans/hydro_thz.md` (H2, small signal), `plans/hydro_largesignal.md`
(H3, finite amplitude, section S5) and `plans/hydro_density.md` (H4, operating
point and contacts, section S6). Derivation: `plans/hydro_thz_notes.md`.
Code: `src/hydro/thz_*.{hpp,cpp}`, `apps/thz_tests.cpp`, `apps/thz_sweep.cpp`,
`apps/thz_power.cpp`. Logs and CSVs: `results/thz/`.

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
6. **Bias tee at both contacts (H3).** The first large-signal runs of the sym
   termination sat at a constant 0.81 of the quadratic law down to the
   smallest amplitude, which is not physics: the dc load current was held
   only at the source port, so the drain's dc current returned through its
   R/2 antenna half instead of to ground, and the Thevenin correction then
   read the rectified voltage a factor (R_dc + R_L)/(R_dc + R_L + R/2) = 0.90
   too small. Every port now carries its perturbative dc current and the
   Thevenin voltage is taken across the device; T14 repeats the small-
   amplitude check for the sym termination. The dss and ant runs ground the
   drain and were never affected.

7. **Contact resistance is not a series element at ac (H4).** The plan said
   the antenna sees Z_dev + 2 R_c. The ac drain current is not -I_s: the
   gate carries displacement current (|I_d + I_s| / |I_s| = 1.16 at the H3
   point, 0.3 THz), so the antenna sees Z_dev + R_c (1 - I_d / I_s), and
   R_c at the drain lifts the drain off the gate at ac by R_c I_d, which is
   the Dyakonov-Shur asymmetry itself. T15 checks the two port relations
   V_s = Va - (R + R_c) I_s and V_d = -R_c I_d instead, and the sweep
   reports Z_ant separately from Z_dev. At dc the identity I_sc (R_dc +
   2 R_c) = -V_oc holds to 1e-14.
8. **NEP units (H4).** The first H4 sweep divided the Johnson *voltage*
   noise by the *current* responsivity, 4.4 nW/Hz^1/2 at the H3 point
   against the 26 pW/Hz^1/2 of S4; caught by comparing with S4, fixed
   (sqrt(4 k_B T / (R_dc + 2 R_c)) / R_I) and the grid rerun. The mesh
   statement of S5 was also wrong: the 18601-node production mesh of the
   1 um funnel is hw 80 / hm 480 nm (gd_thz_power's default), not hw 20 /
   hm 120 (that is the 175 nm funnel's); S5 and the H4 plan now say so.

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
   room-temperature graphene THz detectors (verified against the sources on
   2026-11-10, `paper/literature_detectors.md`) have NEPs of 100 to 130
   pW/Hz^1/2 optical at 0.4 THz at best (Generalov 2017, Ludwig 2024) and
   80 to 160 pW/Hz^1/2 referred to a diffraction-limited spot at 2.5 to
   2.8 THz (Castilla 2019, Viti 2020), 0.5 to 2 nW/Hz^1/2 for most others
   and tens of nW/Hz^1/2 for the first devices, with responsivities of 0.1
   to about 100 V/W; our 50 V/W and 60 pW/Hz^1/2 at 300 K are therefore at
   the level of the best measured devices, not one to two orders above
   them as an earlier version of this note said from memory. Commercial
   zero-bias Schottky detectors are at 5 to 27 pW/Hz^1/2 below 1.1 THz.
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
   natural Stage B2. (Done, 2026-11-09, `BALLISTIC_THZ.md`: the ballistic
   asymmetry survives to 1 THz only with the drop confined to the neck, and
   then at 0.37 to 0.45 A/W in the clean limit, equal to the DS figures
   here, not above them.)
4. **What this stage decides.** In the hydrodynamic-plasmonic picture a
   graphene geometric-diode rectenna at 1 THz is possible on paper at the
   10 to 100 pW/Hz^1/2 level, but the rectification is the Dyakonov-Shur
   circuit asymmetry of a gated channel with a well-matched (1 um class)
   impedance, and the Tesla valve adds nothing. The funnel shape matters as
   an impedance transformer at its taper resonance, which is a design
   handle, and as a dc diode only at 100 K.

## S5: conversion efficiency at finite amplitude (`thz/P1_*` to `thz/P4_*`, figures `P1_eta.png` to `P4_eta.png`, tables `thz/tables_H3.md`)

Plan `plans/hydro_largesignal.md`. The 1 um funnel of S2 (1 um neck, 4 um
leads, production mesh hw 80 / hm 480 nm, 18601 nodes) driven by the full nonlinear BDF2
solver at finite antenna amplitude Va (units of V_unit = 1.487 V; Va = 1 is
the gate overdrive U0 = 1.43 V, so Va <= 0.7 covers every physical
amplitude). The antenna is the 75 ohm Thevenin source of S2, the dc path an
ideal bias tee into a load R_L, and the figure of merit the conversion
efficiency eta_av = P_dc / P_av with P_av = Va^2 / (8 R_ant) the available
power (Va = 0.1 is 37 uW, Va = 0.7 is 1.8 mW). Four experiments:

| run | set | termination | dc load | f | amplitudes |
|---|---|---|---|---|---|
| P1 | 100 K | dss (Thevenin at the source, drain grounded to the gate) | bias tee, R_L = R_dc = 168 ohm | 0.1, 0.3, 1 THz | Va 0.01 to 0.7 |
| P2 | 300 K | dss | bias tee, R_L = R_dc = 579 ohm | 0.1, 0.3, 1 THz | 0.01 to 0.7 |
| P3 | 100 K | dss | no bias tee: the 75 ohm antenna is the dc load | 0.3 THz | 0.01 to 0.7 |
| P4 | 100 K | sym (antenna split +-Va/2, gate at the midpoint: geometric rectification only) | bias tee, R_L = R_dc | 0.3, 1 THz | 0.05 to 0.5 |

Numerics (plan sec. 3, and deviations 6 and the H3 lessons in the memory
notes): 60 BDF2 steps per period; each run starts from the second-order
perturbative state of the *time-stepped* operator (the linear problem solved
with the BDF2 symbol in place of i Omega), which removes the start-up charge
transient that otherwise swamps the quadratic dc signal at small amplitude;
the dc load current is held at the small-signal prediction I20 Va^2 and the
operating point is corrected afterwards through the device's dc Thevenin
source, V_rect = V_dc - R_dc I_dc, I* = -V_rect / (R_dc + R_L); the period
means are Aitken-extrapolated over the last three periods after a settling
time of 3 x 2/Gamma (27 time units at 100 K, 10 at 300 K). The time solver
is Newtonian (no Maxwell relaxation), so the small-signal reference here is
the tau_ee = 0 perturbation solution, not the viscoelastic numbers of S2;
at 100 K and 1 THz (Omega tau_ee = 6) this lowers the small-signal
efficiency four-fold against S2 and the 1 THz, 100 K rows are indicative only.
Cost: 0.6 to 0.8 s per step, 300 to 1600 s per amplitude, 10 CPU-hours for
P1 to P3 and 2 more for P4.

### Ladder T10 to T14 (`thz/ladder_H3.txt`, `thz/ladder_H3_T14.txt`; coarse mesh hw 160 / hm 960 nm, 8383 nodes, 0.3 THz, 100 K)

| test | what it checks | result | criterion |
|---|---|---|---|
| T10 | time-domain eta vs the BDF2-discrete perturbation asymptote at Va 0.01 and 0.02, dss, R_L = R_dc | ratios 1.0033 and 1.0038; I*(0.02)/I*(0.01)/4 = 1.0003; continuous / discrete asymptote 0.953 | within 3 % and 5 %, tail < 2 %, Thevenin correction < 2 % |
| T11 | dc Thevenin correction from a bias seed deliberately half the prediction: short circuit and open circuit (R_L = 1e4) | I* / I_sc Va^2 = 1.0016 (raw mean 16 % off); V_rect / V_oc Va^2 = 1.0017 | within 2 % |
| T12 | time-step convergence at Va 0.3: 60 vs 120 steps per period | eta ratio 0.991; n_min 0.8258 vs 0.8264 | within 3 %, n_min within 0.02 |
| T13 | power bookkeeping at Va 0.01: P_dc <= P_in <= P_av, P_in vs the linear absorbed power | P_in / P_av 0.9186 vs 0.9205 linear (ratio 0.998) | within 2 % |
| T14 | T10 for the sym termination (bias tees at both contacts) at Va 0.02 | eta ratio 0.9977 (the unfixed drain port gave 0.81); Thevenin correction 4e-4, drain dc potential 4e-4 of the source's | within 5 % |

Uncertainty of the production numbers: the absolute level of the quadratic
law carries the 60-step time-step bias, +0.6 % at 0.1 THz, +4.5 % at
0.3 THz and -0.5 % at 1 THz (BDF2-discrete vs continuous perturbation on the
production mesh), and the 2 % mesh uncertainty of T8; the *shape* of
eta(P_av), i.e. the saturation, is converged to 1 % (T12). The ratios below
are taken against the BDF2-discrete law so that the time-step bias cancels.

### Results

Peak efficiency, saturation and breakdown per run (`tables_H3.md`):

| run | set | term | f | peak eta_av | at P_av | eta / quadratic law at peak | n_min, n_max at peak | onset of saturation (eta 10 % below quadratic) | breakdown |
|---|---|---|---|---|---|---|---|---|---|
| P1 | 100 K | dss | 0.1 THz | 3.8e-3 | 1.8 mW (Va 0.7) | 0.86 | 0.52, 1.37 | 0.9 mW (Va 0.5) | none to Va 0.7 |
| P1 | 100 K | dss | 0.3 THz | 2.9e-3 | 1.8 mW | 0.89 | 0.59, 1.61 | 1.8 mW (Va 0.7) | none |
| P1 | 100 K | dss | 1 THz | 1.4e-3 | 1.8 mW | 0.60 | 0.61, 1.39 | 0.15 mW (Va 0.2) | none |
| P2 | 300 K | dss | 0.1 THz | 2.1e-3 | 1.8 mW | 1.14 | 0.55, 1.36 | none (super-quadratic) | none |
| P2 | 300 K | dss | 0.3 THz | 2.4e-3 | 1.8 mW | 1.10 | 0.59, 1.30 | none (super-quadratic) | none |
| P2 | 300 K | dss | 1 THz | 1.6e-3 | 1.8 mW | 0.81 | 0.59, 1.33 | 0.9 mW (Va 0.5) | none |
| P3 | 100 K | ant | 0.3 THz | 2.3e-3 | 1.8 mW | 0.82 | 0.60, 1.61 | 1.8 mW (Va 0.7) | none |
| P4 | 100 K | sym | 0.3 THz | 2.3e-4 | 0.9 mW (Va 0.5) | 0.43 | 0.75, 1.26 | 0.15 mW (Va 0.2) | none to Va 0.5 |
| P4 | 100 K | sym | 1 THz | 2.2e-7 | 0.9 mW | (law at a zero crossing) | 0.82, 1.18 | - | none |

Efficiency at fixed available power, interpolated in log-log between the
computed amplitudes (1 mW is Va 0.52, a 0.78 V amplitude):

| run | set | term | f | 1 uW | 10 uW | 100 uW | 1 mW | quadratic law eta / P_av (1/W) |
|---|---|---|---|---|---|---|---|---|
| P1 | 100 K | dss | 0.1 THz | 2.4e-6 | 2.4e-5 | 2.4e-4 | 2.1e-3 | 2.4 |
| P1 | 100 K | dss | 0.3 THz | 1.8e-6 | 1.8e-5 | 1.9e-4 | 1.8e-3 | 1.7 |
| P1 | 100 K | dss | 1 THz | 1.3e-6 | 1.3e-5 | 1.1e-4 | 7.8e-4 | 1.3 |
| P2 | 300 K | dss | 0.1 THz | 1.0e-6 | 1.0e-5 | 1.0e-4 | 1.1e-3 | 1.0 |
| P2 | 300 K | dss | 0.3 THz | 1.2e-6 | 1.2e-5 | 1.2e-4 | 1.3e-3 | 1.2 |
| P2 | 300 K | dss | 1 THz | 1.1e-6 | 1.1e-5 | 1.1e-4 | 9.3e-4 | 1.1 |
| P3 | 100 K | ant | 0.3 THz | 1.6e-6 | 1.6e-5 | 1.6e-4 | 1.5e-3 | 1.5 |
| P4 | 100 K | sym | 0.3 THz | 5.8e-7 | 5.8e-6 | 5.2e-5 | - | 0.56 |
| P4 | 100 K | sym | 1 THz | 8.5e-10 | 8.4e-9 | 4.1e-8 | - | 2e-5 |

Reading:

1. **The quadratic law holds to 100 uW of available power everywhere, and
   to about 1 mW below 300 GHz.** The time-domain efficiency meets the
   perturbation asymptote at the 0.3 % level at Va 0.01 (0.37 uW) and stays
   within 10 % of it up to Va 0.5 (0.9 mW) at 0.1 and 0.3 THz, at both
   temperatures. The 1 THz runs saturate first (10 % below quadratic at
   150 uW at 100 K, 0.9 mW at 300 K) and reach 0.6 to 0.8 of the law at
   1.8 mW. Over the amplitudes a THz antenna can deliver to one element the
   device is a square-law rectifier, and the S2/S4 responsivities and NEP
   apply unchanged.
2. **No breakdown up to the gate overdrive.** At Va 0.7 (a 1.04 V swing on
   a 1.43 V overdrive) the carrier density inside the device swings between
   0.52 and 1.6 of its gated value and Newton converges every step; neither
   the n -> 0 limit of the local-capacitance closure nor a shock is reached,
   so the saturation onset is a measured value, not a lower bound set by the
   method. The density range says the next physics to add at these
   amplitudes is the non-local electrostatics of the depleted neck, not
   shock capturing.
3. **The sign of the first correction is set by the damping.** At 100 K
   (Gamma 0.22) the rectified current falls below the quadratic law, most at
   1 THz where the plasmon standing wave in the leads is strongest; at
   300 K (Gamma 1.1, overdamped) the 0.1 and 0.3 THz responses grow
   *faster* than Va^2 (1.10 to 1.14 at 1.8 mW), i.e. the diffusive device
   has a weakly expansive I-V. The 300 K shape was checked at 120 steps per
   period on 2026-11-10 (`results/checks/P2_steps120_*`): eta / law is
   1.004, 1.058, 1.109 at Va 0.1, 0.5, 0.7 at 0.3 THz and 1.145 at Va 0.7 at
   0.1 THz, against 0.992, 1.054, 1.101 and 1.144 at 60 steps, a change of
   0.1 to 1.2 %, with n_min unchanged to 3e-4. The super-quadratic rise is
   not a time-step effect.
4. **The matched dc load is worth 15 %, no more.** With the 75 ohm antenna
   as the dc load (P3) the efficiency is 0.85 of the matched-load value at
   small amplitude, exactly 4 R_L R_dc / (R_L + R_dc)^2, and the ratio is
   unchanged to 1.8 mW: there is no large-signal reason for a bias tee, and
   a rectenna element can return its dc current through the antenna.
5. **The geometric rectification saturates first and hardest.** With the
   gate at the antenna midpoint (sym, P4) the funnel shape alone gives
   0.56 per watt at 0.3 THz, a quarter of the DS circuit term of the same
   device, and it leaves the quadratic law at 150 uW, reaching 0.43 of it
   at 0.9 mW (eta 2.3e-4) while the dss response of the same device at the
   same amplitude is still at 0.9 of its law. The dc Thevenin correction
   grows to 20 % there, so the last P4 point carries a few per cent more
   uncertainty than the rest. At 1 THz the shape-only response of the 1 um
   funnel is below 3e-7 at every amplitude, four thousand times below the
   circuit term, as the small-signal zero crossing of S2 predicts (the
   small-signal law itself is 40 times time-step sensitive there and the
   ratio is meaningless). The Borda-Carnot picture of a geometric diode
   that improves at large signal is not what the fluid does: the
   constriction's rectification is a resonance effect that detunes as the
   density swings, and the circuit asymmetry, not the shape, carries the
   large-signal response too.
6. **Verdict: detector, not harvester.** The efficiency is proportional to
   the available power, eta = (1 to 2.4 per watt) x P_av, up to about 1 mW
   per element, where it levels off at 0.1 to 0.4 %. A THz beam at
   1 W/m^2 delivers of order 10 nW to a half-wave antenna at 1 THz, i.e.
   eta ~ 1e-8; even a 1 mW source focused on a single 1 um element is
   converted at 0.2 %. The hydrodynamic graphene rectenna has no harvesting
   regime: its use is as a square-law detector, where the quadratic range
   found here (seven decades of power above the 100 K NEP of S4) is the
   figure that matters. Reaching per-cent efficiency would need the
   small-signal curvature r to rise by an order of magnitude, which is the
   lower-density, thinner-gate regime excluded from this stage.

## S6: carrier density, gate distance and contact resistance (`thz/D1_*`, `D1m_*`, `D1t_*`, `D2_*`, `D2m_*`, `D3_*`; figures `D1_maps.png`, `D1_lines.png`, `D2_contacts.png`, `D3_ladder.png`; tables `thz/tables_H4.md`)

Plan `plans/hydro_density.md`. S5 found the efficiency of the 1 um funnel
proportional to the available power up to a knee near 1 mW and a peak of a
few tenths of a per cent, both at one operating point (n = 1e12 cm^-2,
d = 30 nm). In units of V_unit and R_unit the matched-load efficiency is
eta = 2 r^2 (R_ant / R_dc) Va^2 with r = V_oc / Va^2, so eta / P_av = 16 r^2
R_ant^2 / (R_dc V_unit^2) in 1/W, the knee sits at V_a ~ 0.5 V_unit, i.e.
P_knee = (V_unit / 2)^2 / (8 R_ant), and the peak is eta_peak ~ 2 r^2
(R_ant / R_dc) x 0.49 x S with S the S5 saturation factor (0.86, 0.89, 0.60
at 100 K and 1.14, 1.10, 0.81 at 300 K for 0.1, 0.3, 1 THz). The peak is a
pure number; n and d enter it only through the dimensionless groups (Gamma,
q, 1/Re_s, Omega, R_ant / R_unit) that set r, and set the knee through
V_unit ~ U_0 ~ n d. This stage maps both over n = 1e12, 5e11, 2e11, 1e11,
5e10 cm^-2 and d = 30, 10, 5 nm with the H2 perturbation solver on the
production mesh (hw 80 / hm 480 nm, 18601 nodes), viscoelastic as in S2,
scattering times held at the set values (constant tau) with a constant-
mobility variant tau ~ sqrt(n) at 100 K, a 75 ohm antenna with a matched
variant R_ant = Re Z_ant, and a contact resistance R_c in series with each
contact as a port-only change (ac and dc; the dc Thevenin resistance becomes
R_dc + 2 R_c and the NEP uses it). Validity is carried as flags: Fermi
liquid E_F >= 3 k_B T (excludes n <= 2e11 at 300 K), gating U_0 = e n /
C_g >= 4 k_B T (excludes the thin-gate low-density corner: (1e11, 5 nm),
(5e10, 10 nm), (5e10, 5 nm) at 100 K, everything below U_0 = 0.1 V at
300 K), and q = s_q^2 / s0^2 > 0.3 marked (quantum capacitance dominant).
The operating-point table (E_F / kT, U_0 / kT, q, V_unit, R_unit, R_dc,
P_knee, flags) is the first table of `tables_H4.md`. Cost: 20 s per
frequency; D1 to D2 are 240 solves, 12 CPU-minutes; D3 is the time solver,
8 amplitudes, about an hour wall on two cores.

| run | what | set | f (THz) | R_c (ohm um) | R_ant |
|---|---|---|---|---|---|
| D1 | the (n, d) grid, dss termination | 100 K, 300 K | 0.1, 0.3, 1.0 | 0 | 75 ohm |
| D1m | D1 with the antenna matched to Re Z_ant at each point | both | same | 0 | matched |
| D1t | D1 at 100 K with tau_mr, tau_ee ~ sqrt(n / 1e12) (constant mobility) | 100 K | same | 0 | 75 ohm |
| D2 | contact resistance at (1e12, 30 nm), (5e11, 10 nm) and (5e10, 30 nm) | both | same | 100, 300, 1000 | 75 ohm and matched |
| D3 | time-domain ladder at (5e11, 10 nm), Va 0.1, 0.3, 0.5, 0.7 (gd_thz_power) | both | 0.3 | 0 | 75 ohm |

### Ladder T15 to T17 (`thz/ladder_H4.txt`; quick, 9 s)

| test | what it checks | result | criterion |
|---|---|---|---|
| T15 | R_c = 300 ohm um (75 ohm per contact) at (1e12, 30 nm), 100 K, 0.3 THz, coarse mesh: I_sc (R_dc + 2 R_c) = -V_oc; R_c = 0 reproduces the S2 ports; port relations V_s = Va - (R + R_c) I_s, V_d = -R_c I_d; passivity | 6.7e-15; 0; 3.9e-16; P_in / P_av 0.386 (0.902 at R_c = 0); I_sc 3.05e-2 -> 1.09e-2, V_oc -7.69e-2 -> -5.27e-2 | 1e-3; 1e-6; 1e-9; P_in <= P_av |
| T16 | the physical layer enters only through the groups: (4e12, 15 nm) and (1e12, 30 nm, W, tau, nu doubled) co-scaled to the same (Gamma, q, 1/Re_s, Omega, R_ant / R_unit) as (1e12, 30 nm, 75 ohm, 0.3 THz) | groups, nondimensional I_sc, V_oc, Z, R_dc identical to 0; R_I scales as I_unit R_ant / V_unit^2 to 0 (0.189, 0.095, 0.189 A/W at 75, 37.5, 37.5 ohm) | 1e-9; 1e-6; 1e-6 |
| T17 | units at (5e11, 10 nm): closed forms V_unit = U_0 / (1 - q), R_unit = m0 s0 / (e^2 n W), f_unit = s0 / 2 pi W; hand values E_F 82.5 meV, s0 1.840e6 m/s, q 0.148, U_0 0.238 V, V_unit 0.279 V, R_unit 47.4 ohm, f_unit 73.2 GHz; channel DS response with the graphene closure at this q against the classical U_a^2 f / 4 U_0 | 2e-16; 2.8e-4; graphene 0.599 V_unit vs shallow water 0.724 U_0, ratio in volts 0.970 | 1e-9; 1e-3; within 10 % |

### Results

Reading (figures `D1_maps.png`, `D1_lines.png`; the per-point numbers are in
`tables_H4.md`; all efficiencies are for the matched dc load):

1. **The knee moves as (n d)^2, as predicted.** P_knee = (V_unit / 2)^2 /
   8 R_ant runs from 0.92 mW at the H3 point to 33 uW at (5e11, 10 nm),
   3.0 uW at (5e10, 30 nm) and 0.26 uW at (5e10, 5 nm); with it the
   small-signal efficiency per watt rises from 1.9 /W (100 K, 0.3 THz) to
   51 /W at (5e11, 10 nm) and 1770 /W at (5e10, 30 nm), a factor 950 of
   which (n d)^-2 accounts for 400, the growth of r from 0.088 to 0.324
   for 13.7, and the rise of R_dc from 168 to 751 ohm for a factor 4.5 the
   other way. At 300 K the valid grid ends at 5e11, where eta / P_av is
   4.8 /W (30 nm), 11 /W (10 nm) and 16 /W (5 nm) against 1.2 /W at the H3
   point.
2. **The ceiling barely moves, and it moves the wrong way for thin
   gates.** The predicted peak efficiency over the valid grid is 3.0e-4 to
   9.2e-3 at 100 K and 1.3e-4 to 2.4e-3 at 300 K (the low ends at 1 THz
   with the 5 nm gate). At fixed n a thinner
   gate *lowers* it: at 1e12, 0.3 THz, 100 K, eta_peak is 3.0e-3 (30 nm),
   2.1e-3 (10 nm), 9.9e-4 (5 nm), because r falls from 0.088 to 0.073 to
   0.050 (a thinner gate lowers s0, raising Gamma and q, and the 75 ohm
   antenna is increasingly mismatched to Z_ant ~ 30 ohm). At fixed d a
   lower density raises r (0.088 to 0.32 at 30 nm, 100 K) and the ceiling
   with it, to 9.2e-3 at (5e10, 30 nm), 0.3 THz, the edge of the Fermi-
   liquid limit (E_F = 3.0 kT); at 300 K the H3 point is already the ceiling
   (2.4e-3 at 300 GHz; 2.4e-3 at (5e11, 30 nm)). The hypothesis of the plan
   holds at 300 K and holds within a factor 3 at 100 K: the reachable peak
   is below 1 % (0.03 to 0.9 %), r^2-limited, and no operating point of gated graphene
   reaches per-cent efficiency.
3. **What the sweep buys is efficiency at low power.** At 10 uW of
   available power the H3 device converts 1.9e-5 (100 K) and 1.2e-5
   (300 K); the best valid points convert 9.2e-3 at (5e10, 30 nm) and
   3.0e-3 at (1e11, 30 nm) or (2e11, 10 nm) at 100 K, and 1.6e-4 at (5e11,
   5 nm) at 300 K; at 100 uW, 8.0e-4 at (5e11, 10 nm), 300 K, against
   1.2e-4. At 1 uW the best is 1.8e-3 (100 K) and 1.6e-5 (300 K). The
   design rule is therefore "put the knee at the available power": with the
   knee there the element converts at its r^2-limited peak, a few tenths of
   a per cent, and nothing in n or d raises that number.
4. **Matching the antenna is not neutral but changes nothing.** R_ant =
   Re Z_ant raises eta / P_av by 1.0 to 1.5 (most at the thin gates, Z_ant
   30 to 40 ohm) and moves eta_peak both ways: up by 3.7 at (1e12, 5 nm),
   where the 75 ohm antenna was mismatched, down by 1.5 at the low-density
   30 nm points with Z_ant above 100 ohm. The best matched eta_peak on the
   valid grid is 1.3e-2 at (5e11, 30 nm), 100 K, 0.3 THz (R_ant 51 ohm) and
   2.7e-3 at 300 K: one per cent at 100 K with a matched antenna at the
   knee of 0.5 mW, still two to three tenths of a per cent at 300 K.
5. **Constant mobility takes most of the low-density gain back.** With
   tau ~ sqrt(n) at 100 K the dc resistance at (5e10, 30 nm) is 2509 ohm
   instead of 751, r is 0.24 instead of 0.32, and eta / P_av is 286 /W
   instead of 1770 (0.16 of the constant-tau value), eta_peak 1.5e-3 instead
   of 9.2e-3; at (1e11, 30 nm) 0.40 and 2.3e-3; at (5e11, 10 nm) 0.65 and
   1.9e-3. The per-watt figures of items 1 and 3 at the low-density end
   are therefore bounds that assume the 5 ps momentum relaxation survives
   to 5e10 cm^-2; the ceiling conclusion of item 2 is unchanged (0.15 to
   0.23 % everywhere on the constant-mobility grid).
6. **Contact resistance is the dominant loss, and it acts on the
   asymmetry, not only in series.** At the H3 point, 0.3 THz, 100 K,
   R_c = 100, 300, 1000 ohm um (25, 75, 250 ohm per 4 um contact) cuts
   eta / P_av from 1.86 /W to 1.15, 0.40 and 0.024 /W (factors 1.6, 4.6,
   78), the responsivity from 0.211 A/W to 0.145, 0.071 and 0.012 A/W and
   raises the NEP from 27 to 35, 59 and 240 pW/Hz^1/2; at 300 K from
   1.21 /W to 0.59, 0.18, 0.012 /W and the NEP from 59 to 84, 152, 600
   pW/Hz^1/2. Matching the antenna to the new Z_ant recovers little (0.53
   instead of 0.40 /W at 300 ohm um). The loss is larger than the series
   picture gives: with R_c the drain is no longer at the gate potential at
   ac (|I_d| ~ |I_s| through the gate capacitance, so V_d = -R_c I_d is a
   large fraction of V_s), which is the Dyakonov-Shur boundary condition
   being removed, and the absorbed fraction P_in / P_av falls to 0.39 at
   300 ohm um. The relative damage is similar at the other two points
   (factors 4 and 11 at 300 ohm um at (5e10, 30 nm) and (5e11, 10 nm),
   where R_dc is 751 and 237 ohm). The NEP of the best valid point, (5e10,
   30 nm) at 100 K, is 0.9 pW/Hz^1/2 with ideal contacts, 1.8 with 300
   ohm um, and 2.2 with ideal contacts and constant mobility; at 300 K the
   best is 3.7 pW/Hz^1/2 at (5e10, 30 nm), outside the Fermi-liquid limit,
   and 15 pW/Hz^1/2 at the best valid point (5e11, 5 nm). A graphene
   rectenna element needs contacts at the 100 ohm um level, where the
   penalty is a factor 1.6 to 2.5, and the S4 and S5 figures assume ideal
   ones.
7. **The saturation factor transfers between operating points (D3).**
   The time-domain ladder at (5e11, 10 nm), 0.3 THz
   (V_unit 0.279 V, R_dc 237 and 819 ohm, dc load matched, 60 steps per
   period, started from the BDF2-consistent perturbative state as in S5)
   gives, against its own BDF2-discrete quadratic law, eta / law = 1.007,
   1.023, 1.055, 1.104 at Va 0.1, 0.3, 0.5, 0.7 at 300 K, where the H3
   device gave 0.992, 1.020, 1.054, 1.101: the saturation factor transfers
   to 0.2 % at 300 K. At 100 K it gives 0.996, 0.935, 0.843, 0.764 against
   1.017, 1.106, 1.012, 0.891: the second operating point saturates
   earlier (Gamma 0.43 and q 0.15 instead of 0.22 and 0.04), and S at
   Va 0.7 is 0.76 instead of the 0.89 assumed, 14 % lower, inside the 20 %
   the plan asked for. The density extrema at Va 0.7 are n_min 0.69 and
   0.67 against 0.59 at the H3 point, no breakdown. The absolute numbers
   confirm the scaling: the same Va = 0.7 that is 1.8 mW at the H3 point is
   64 uW here, converted at 1.4e-3 (100 K) and 8.0e-4 (300 K) against
   2.9e-3 and 2.4e-3 there. The eta_peak values of items 2 to 4 at 100 K
   are therefore upper estimates by 10 to 20 % at the lower densities and
   thinner gates, where Gamma and q are larger still; at 300 K they are
   good to the S5 accuracy.
8. **Verdict.** The density and gate sweep closes the question S5 left
   open. Lowering n d moves the knee of the efficiency curve from a
   milliwatt to a microwatt exactly as (n d)^2, so a gated-graphene funnel
   can be designed to convert any given available power between 1 uW and
   1 mW at its peak; but the peak is 0.2 to 0.9 % (1.3 % with a matched
   antenna at 100 K, 0.3 % at 300 K) at every operating point the fluid
   model is valid for, falls for thinner gates, loses a factor 4 to 6 at
   the low-density end if the mobility rather than the scattering time is
   constant, and loses a further factor 2 to 5 for realistic 100 to 300 ohm
   um contacts. As a detector the same lever is worth having: the NEP of the
   matched 1 um funnel falls from 26 pW/Hz^1/2 at (1e12, 30 nm) to 1 to 2
   pW/Hz^1/2 at (5e10 to 1e11, 30 nm) at 100 K with 5 ps scattering, and
   the responsivity reaches 3 A/W. As a harvester the geometric diode has no
   regime: the per-cent question is answered by the dimensionless curvature
   r alone, which the sweep moves by a factor 3.7 at most.

## Limitations

Local-capacitance electrostatics (d = 30 nm against a 175 nm neck is the
worst case); no electron heating or thermoelectric response; the ballistic
transit-time nonlinearity of the funnel is outside the fluid model; the
antenna enters only as R_ant and the available power; the hydrodynamic
description is quantitative to about 300 GHz at 100 K and 1 THz at 300 K
(omega tau_ee = 1), beyond which the viscous term is carried by the Maxwell
form and the results are indicative.
