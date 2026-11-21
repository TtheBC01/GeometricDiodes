# Stage H6 results: the momentum equation of the Dirac fluid, corrected (2026-11-17 and 18)

Plan: `plans/hydro_momentum.md` (figure `plans/hydro_momentum.png`). Code:
`src/hydro/thz_common.hpp` (the closure), `thz_solver.cpp` (second-order
forcing), `thz_time.cpp` (time-domain and steady solver), ladder T33 to T39
in `apps/thz_tests.cpp` and `tools/momentum_regress.sh`. Runs:
`tools/momentum_runs.sh`; results in `results/thz2/` and
`results/dcgate2/`; tables `results/thz2/tables_H6.md`
(`tools/momentum_report.py`); figure `results/thz2/H6_before_after.png`.
The files of Stages H2 to H5 (`results/thz`, `results/dcgate`) and the
paper are untouched.

The question: the gated-fluid solvers used the convective term of an
ordinary fluid. What do the fluid results of the paper become with the
momentum equation of a degenerate Dirac gas?

## Verdict

1. **The correction is confirmed and implemented.** Both cited papers,
   the relativistic Euler equation and a direct evaluation of the stress
   agree on the equation; seven new checks and the old ladder pass; the
   legacy switch reproduces the stored files bit for bit.
2. **The Dyakonov-Shur numbers of the paper fall, by less than the plan
   guessed.** The uniform channel falls to 0.34 to 0.62 of its earlier
   response, but the funnels with an antenna only to 0.6 to 1.0 at their
   best frequencies: the
   1 um funnel gives 0.14 to 0.24 A/W at 100 K and 0.052 to 0.066 A/W at
   300 K (0.15 to 0.34 and 0.07 to 0.09 before); the 175 nm funnel at
   its taper resonance 0.43 and 0.066 A/W (0.45 and 0.10).
3. **The paper's qualitative statements about the fluid survive.** The
   terminations carry the response; the shape gives 30 % of it at 100 K
   and 2 to 3 % at 300 K for the 1 um funnel; the Tesla valve stays more
   than ten times below the funnel; in steady gated flow the polarity
   term is the same and the exchange term stays below 1e-3.
   A new control sharpens one of them: at 300 K the funnel shape takes
   15 % away from what a plain channel with the same terminations gives.
4. **At room temperature the fluid funnels are now below the ballistic
   short funnel.** 0.05 to 0.07 A/W (0.06 to 0.08 with the
   finite-temperature coefficients) against 0.12 A/W per absorbed watt;
   the paper has them level. The noise-equivalent power of the 1 um
   funnel at 300 K with ideal contacts is about 100 pW/Hz^(1/2) between
   0.3 and 1 THz, not 60.
5. **The large-signal section does not survive as written.** The old runs
   drove the carriers faster than v_F. With the exact gas the square law
   holds up to the 0.3 v_F limit, where the efficiency is 1e-5 to 4e-5 at
   10 to 80 uW; beyond it the ideal fluid's response grows faster than
   the law until the drift reaches v_F, and the largest efficiency found
   anywhere is 0.09 %. "No harvesting regime" stands, on firmer ground;
   "a square law up to the gate overdrive" and "about one per cent" do
   not.
6. **Two uncertainties of the fluid numbers are now measured, and both
   are of the size of the correction.** The contact condition: a lossless
   contact instead of a fixed density lowers the funnels' DS response by
   up to a third (and the ideal-source channel's by a factor of 1.5 to
   20). Finite temperature: the isothermal coefficients raise the
   300 K numbers by about 20 %.

## Step 0: what the reading settled (`tools/momentum_step0.py`)

1. **Both cited papers were read in full** (ar5iv text with the equations).
   Svintsov et al. (arXiv:1310.3963): in one dimension the convective term
   has the factor 3 - 4 xi, zero for degenerate electrons, and waves on a
   drift u0 travel at u0/2 +- s0. Tomadin and Polini (arXiv:1307.0734):
   their Eq. (6) is (c_div, c_grad) = (-1/2, 0) in two dimensions; their
   general equations in the appendix, Eqs. (53) and (56), are the exact
   form used here once the pressure is evaluated at fixed laboratory
   density, which their Eq. (5) does not do.
2. **The exact form is the textbook relativistic Euler equation** with v_F
   for the speed of light, as Tomadin and Polini quote it in their
   Eq. (3): the two differ by the invertible factor
   1 + gamma^2 u u^T / v_F^2, shown algebraically, force term included.
   Numerically, the wave speeds of the ungated fluid on a drift of 0.05 to
   0.8 v_F from the exact one-dimensional equation equal the relativistic
   addition of velocities to six digits.
3. **Two slips in Svintsov's paper, neither affecting what is used.** The
   angular integral for the energy density is (1 + beta^2/2)(1 -
   beta^2)^(-5/2), not (1 - beta^2)^(-5/2), so the transverse stress of
   their Eq. (13) is wrong at order beta^2 (10 % at 0.3 v_F); and the
   beta^2 part of the convective coefficient in their Eq. (16) has the
   opposite sign to the one that satisfies the addition of velocities.
   Their momentum density, longitudinal stress and second-order equation
   are right.
4. **The closed form used for the channel equals a published one.** With
   alpha = 1/2 and the density fixed at the source it reproduces
   Eq. (43) of Tomadin and Polini to 3e-10 at 45 points.
5. **Finite temperature matters more than the plan assumed.** In the
   isothermal form of Svintsov's Eq. (16) the coefficients are c_div =
   1 - 2 xi and c_grad = (1 - 2 xi)/2 with xi = n^2 / (eps <1/eps>). At
   1e12 cm^-2 that is (-0.47, -0.24) at 100 K and (-0.34, -0.17) at
   300 K, against (-1/2, -1/4) at zero temperature: the one-dimensional
   coefficient is 0.05 and 0.32 instead of 0. The inertial mass is 3 % and
   21 % above m(n). The plan said to switch coefficients above a 10 %
   effect. That rule could not be applied cleanly: the mass and the
   pressure of the model are also those of zero temperature, the exact
   form exists only there, and isothermal or adiabatic is a choice the
   model has no equation for. **Decision taken:** the zero-temperature
   gas is the main result, as everywhere else in the model, and the
   isothermal coefficients are run as a variant for the reference devices
   (section "Variants").

## Deviations from the plan

1. **Finite temperature:** as item 5 above.
2. **T37 needs a small viscosity.** The steady solver does not converge at
   zero viscosity. With nu = 0.002 the only viscous contribution to a
   radial flow at this order is the do-nothing condition of the viscous
   term at the contacts, a potential nu J (1/r1^2 - 1/r2^2), which is
   added to the reference; the result extrapolated to zero viscosity
   agrees with the unmodified reference to 0.2 % of the inertial part.
3. **Production started before the last old test had finished.** T6, a
   classical-gas time-domain test whose code path T33 had already shown
   to be bit-identical, was still running when the small-signal runs
   (which do not use that solver) and later the others were started.
   T6 passed afterwards (ratios 0.9997 and 0.9994).
4. **A second contact variant** (coefficient -1) was added to C2, to show
   that the response is linear in the contact coefficient.
5. **The machine swapped.** Seventeen solver processes on 16 GB of memory;
   eight large-signal jobs were paused and resumed one by one. No result
   is affected; the wall time is longer than planned.
6. **More amplitudes in the large-signal runs** (eleven instead of eight:
   0.15, 0.25 and 0.4 added), to resolve the approach to v_F. Two 1 THz
   runs at 100 K were cut short to save about four hours: the DS run has
   0.01, 0.02, 0.05, 0.1, 0.2 and 0.3 (restarted after 0.02 with the
   shorter list), and the shape-only run stops at 0.2, inside the
   0.3 v_F limit.
7. **The steady solver was changed after the checks.** With the exact
   form, a bias whose steady state would need a drift at or above v_F
   made the continuation creep toward the limit for twenty minutes per
   point. It now stops after three shortened attempts that hit the limit
   and reports the point as not converged. The steady production runs
   used this version, and the checks that use the steady solver (T1, T5,
   T18 to T20, T36 to T38) were run again on the final build:
   all eight pass with the same numbers (last block of `thz2/ladder_H6.txt`).

## Checks (gate 1; `thz2/ladder_H6.txt`)

| check | what | result | criterion |
|---|---|---|---|
| T33 | legacy switch against the stored files: channel (25 frequencies), 1 um funnel with both terminations (3 frequencies each), time domain at two amplitudes, steady gated flow (2 biases) | 123 numbers identical to the last printed digit | 1e-8 |
| T34 | channel, new terms, against the closed form: alpha = 0, 1/2 and 1, q = 0.04 and 0.3, damping 1 and 0.1, five frequencies | 60 points, worst error 1.3e-3 | 1 % |
| T35 | pulse on a drifting fluid: (s+ + s-) / u0 | Dirac gas 0.997 at 0.07 v_F and 1.012 at 0.18 v_F (linearised equation 1.002 and 1.016); Galilean 1.999 (2.000) | 5 % of the drift |
| T36 | steady gated channel against the one-dimensional quadrature: exact, second order, legacy | worst error 1.3e-5 below 0.3 v_F, 6.8e-5 up to 0.8 v_F; legacy quadrature against the old closed form 8e-13 | 1e-3, 1e-2 |
| T37 | radial flow in a ring sector against the conservative radial quadrature, outward and inward, at 0.3 v_F | exact form 0.02 and 0.03 % of the inertial part; second order 0.2 and 0.4 %; Tomadin-Polini 0.01 and 0.02 %; same on both meshes | 3 % |
| T38 | 1 um funnel mesh rotated by 30 degrees | small-signal current and steady voltage equal to 4e-15 | 1e-6 |
| T39 | time domain (exact) against perturbation (second order), graphene channel | ratios 0.9997 and 1.0016 at amplitudes 0.01 and 0.03; legacy 1.0000 and 0.9999 | 2 % plus twice the amplitude |
| T10, T14 | the same for the 1 um funnel, both terminations | 1.0048, 1.0063; 0.9964 | 3 %, 5 % |
| T1 to T5, T7 to T9, T11 to T13, T15 to T20 | old ladder with the new default | all pass | as before |
| T6 | classical gas, time domain against perturbation | ratios 0.9997 and 0.9994 | 2 % |

T17 and T18 have new references (the closed form and the quadrature for
the closure in use); the old T17 criterion, graphene within 10 % of the
classical gas, no longer holds and is not expected to. T12, the time-step
check at an amplitude of 0.3, passes at 2.1 % (0.9 % before): the exact
form is stiffer near v_F.

## Runs

| block | what | files |
|---|---|---|
| small signal | S1 channel (zero-temperature gas, classical gas, Tomadin-Polini, finite-temperature coefficients, two contact variants); S2 three funnels; S3 Tesla valve; C1 plain channel with the leads and length of the 1 um funnel; C2 contact variants and finite-temperature coefficients on the two funnels | 60 sweeps of 15 to 25 frequencies (`S*`, `C1_*`, `C2*`, `FT_*`) |
| density | D1, D1m, D1t, D2, D2m: 111 operating points at three frequencies | as Stage H4 |
| steady | D1 to D4 of Stage H5 | 128 points, 4 solves each |
| large signal | P1 to P4, D3, and P1 at 0.3 THz with the second-order form | 12 runs, 96 amplitudes, the ones that hit v_F included (`P*`, `D3_*`) |

## Results

### Small signal (`thz2/tables_H6.md`, sections 1 to 4; figure panels a to c)

1. **The uniform channel falls to a third to a half** (Fig. 6a of the
   paper). With an ideal source the response is 0.34 to 0.58 of the
   earlier one at 100 K and 0.53 to 0.62 at 300 K. At the largest
   resonance graphene gives 0.47 of the classical gas; the paper says
   40 % more (the stored figure was 1.31). The solver lies on the closed
   form at 60 points (T34).
2. **The funnels with an antenna fall less.** Current responsivity in
   A/W, 75 ohm antenna:

   | device, termination | 100 K before | now | 300 K before | now |
   |---|---|---|---|---|
   | 1 um funnel, DS, 10 GHz to 1 THz | 0.15 to 0.34 | 0.14 to 0.24 | 0.07 to 0.09 | 0.052 to 0.066 |
   | 1 um funnel, DS, 0.3 to 1 THz | 0.21 to 0.34 | 0.18 to 0.24 | 0.089 to 0.092 | 0.052 to 0.055 |
   | 1 um funnel, symmetric, 0.3 to 1 THz | 0.08 to 0.15 | 0.057 to 0.071 | 0.0015 to 0.005 | 0.0008 to 0.0023 |
   | 175 nm funnel, DS, at its taper resonance (1.87 THz) | 0.45 | 0.43 | 0.10 | 0.066 |
   | 175 nm funnel, symmetric, same frequency | 0.22 | 0.21 | 0.015 | 0.013 |
   | 175 nm funnel with 1 um leads, 1 THz: DS; symmetric | | | 0.14; -0.07 | 0.10; -0.05 |
   | Tesla valve, symmetric, 1 THz | 0.010 | 0.010 | 1.5e-4 | 4.5e-4 |
   | Tesla valve, DS, best | 0.105 | 0.094 | 0.023 | 0.013 |

   Before minus now, over before, is the share the old convective term
   had in each number: 30 to 45 % for the funnels at 300 K and at most
   frequencies at 100 K, but 4 % at the taper resonance of the 175 nm
   funnel at 100 K. The plan's guesses took the channel's 40 to 65 % for
   every device, which is why the 100 K numbers came out above them.
3. **The share of the shape is nearly as it was.** Symmetric over DS
   termination for the 1 um funnel between 0.3 and 1 THz: 0.30 to 0.32 at
   100 K (0.38 to 0.44 before) and 0.02 to 0.03 at 300 K (0.03 to 0.04).
   For the 175 nm funnel at 1 THz: 0.61 at 100 K (0.64) and 0.20 at 300 K
   (0.26). The Tesla valve's shape-only response at 1 THz is 18 and 28
   times below the 175 nm funnel's at 100 K and 300 K (20 and 180 before;
   the 300 K Tesla value rose from 1.5e-4 to 4.5e-4 A/W).
4. **The plain-channel control (C1, new).** A uniform channel with the
   lead width (4 um) and the full length (26.6 um) of the 1 um funnel,
   same antenna and terminations. With the DS termination it gives 0.061
   to 0.066 A/W between 0.3 and 1 THz at 300 K, and the funnel gives 0.83
   to 0.85 of that at every frequency from 0.1 to 1 THz: **at 300 K the
   funnel shape takes 15 % away from what the terminations give.** At
   100 K the channel gives 0.10 to 0.18 A/W and the funnel 1.05 to 2.3
   times that (2.3 at 0.91 THz). With the symmetric termination the
   channel must give zero; what it gives is the numerical floor of this
   mesh: below 5e-6 A/W to 0.3 THz, 3e-4 at 0.57 THz and 0.008 at
   0.91 THz at 100 K (9e-5 at 300 K). The funnel's shape-only 0.07 A/W
   at 1 THz and 100 K therefore carries an 11 % floor.
5. **Noise-equivalent power, ideal contacts** (pW/Hz^(1/2)): 1 um funnel
   at 300 K 58 to 81 at its best frequency, which is now 10 GHz, and 97
   to 103 between 0.3 and 1 THz (60 before); at 100 K 17 to 24; 175 nm
   funnel at its resonance 3.8 to 4.0 at 100 K and 36 to 56 at 300 K.
   With contact resistances of 0, 100, 300 and 1000 ohm um at 300 K (best
   of 0.1, 0.3 and 1 THz): 59, 84, 151, 555 before; 96, 132, 228, 840 now.
6. **Operating-point grid** (Fig. 8). At the reference point the
   responsivity is 0.58 to 0.68 of the earlier one and the small-signal
   efficiency per watt 0.33 to 0.46 (300 K: 1.0 to 1.2 per watt before,
   0.37 to 0.45 now). The low-density, thin-gate corner at 1 THz falls
   much further, to 0.05 to 0.3 of the earlier responsivity at (2e11,
   5 nm), (1e11, 10 nm), (5e11, 5 nm) and (2e11, 10 nm) at 100 K: there
   the wave speed is close to v_F and the convective term carried most of
   the response. The trend of Fig. 8 toward low density and thin gates is
   weaker than drawn.
7. **Drift speed, now from the two-dimensional fields.** The largest
   first-order drift reaches 0.3 v_F at an antenna amplitude of 0.05 to
   0.13 of the voltage unit for the 1 um funnel at 100 K (10 to 60 uW of
   available power) and 0.17 to 0.21 at 300 K (110 to 160 uW), between
   0.1 and 1 THz. For the 175 nm funnel, whose neck concentrates the
   current: 0.017 to 0.07 at 100 K and 0.08 to 0.09 at 300 K. The
   small-signal efficiency at the limit of the 1 um funnel is 2e-5 at
   100 K and 4e-5 to 7e-5 at 300 K.

### Variants (`tables_H6.md`, sections 1 and 3)

8. **The contact condition is a first-order uncertainty.** The default
   fixes the density at a contact, as Dyakonov and Shur and both cited
   papers do. If the contact instead fixes the energy per particle of the
   drifting gas, Phi(n) + u^2/4 (a lossless contact; the same choice
   exists for an ordinary fluid, where it is u^2/2), the ideal-source
   channel falls to 0.05 to 0.46 of the default at 100 K and to 0.55 to
   0.67 at 300 K. The funnels with an antenna are far less sensitive:
   0.64 to 0.93 of the default with the DS termination for both funnels,
   0.58 to 1.44 for the shape-only response of the 1 um funnel. The
   response is linear in the contact coefficient (the variant with -1
   reproduces the Tomadin-Polini column of the channel to every digit),
   so any other condition can be read off the two columns.
9. **Finite-temperature coefficients** raise the 300 K numbers by 16 to
   23 % with the DS termination (both funnels) and 7 to 36 % for the
   shape-only response; at 100 K by 0 to 5 %.

### Large signal (`tables_H6.md`, section 6; figure panel d)

10. **The square law now ends at the Fermi velocity, not at the gate
    overdrive.** With the exact gas a run exists only while the drift
    stays below v_F. For the 1 um funnel with the DS termination that is
    up to an amplitude of 0.3 to 0.4 of the voltage unit at 100 K (0.33
    to 0.59 mW available) and 0.5 at 300 K (0.92 mW); at the next
    amplitude tried the drift reaches v_F and there is no state. The old
    runs went to 0.7 (1.8 mW) with nothing to stop them.
11. **Inside the 0.3 v_F limit the device is a square-law rectifier with
    an efficiency of 1e-5 to 4e-5.** The law holds to 5 % there. The
    limit is reached at 9 to 37 uW at 100 K and at about 83 uW at 300 K
    (amplitudes 0.05 to 0.1 and 0.15; the grid of amplitudes is coarse).
    The small-signal efficiency per watt is 0.46 to 0.58 of the earlier
    one at 0.1 and 0.3 THz at 100 K and 0.37 to 0.44 at 300 K: 1.0 to
    1.1 per watt at 100 K, 0.45 at 300 K (1.7 to 2.4 and 1.0 to 1.2
    before).
12. **Between 0.3 v_F and v_F the response grows faster than the square
    law, and the largest efficiency found is below 0.1 %.** DS
    termination, 0.3 THz: 1.36 times the law at 0.81 v_F and 1.45 times
    at 0.94 v_F at 100 K (efficiency 8.7e-4 at 0.59 mW); 2.1 times at
    0.96 v_F at 300 K (8.6e-4 at 0.92 mW). With the antenna as the dc
    load, 6.6e-4. The second-order form in the same solver stays on the
    law (0.95 at the same amplitude), so the rise is the gas approaching
    v_F, where its momentum per particle grows faster than its velocity;
    it is the ideal fluid extrapolated and not a prediction for a device
    (no heating, no optical phonons). The earlier figures, 0.14 to 0.38 %
    at 1.8 mW, came from amplitudes at which the drift exceeded v_F.
    The shape-only response (symmetric drive, 0.3 THz, 100 K) saturates
    as before: 0.77 of its law at 0.54 v_F and 0.45 at 0.79 v_F, with a
    small-signal efficiency per watt a third of the earlier one (0.19
    against 0.56 per watt). At the second operating point (5e11 cm^-2,
    10 nm) the wave speed is 1.84 v_F, the drift stays at 0.67 v_F even
    at an amplitude of 0.7, and the efficiency there is 4.3e-4 at 100 K
    and 3.7e-4 at 300 K (1.4e-3 and 8.0e-4 before).
    At 1 THz the picture is the same at 300 K (the law to 7 % inside the
    limit, 2.6 times the law at 0.97 v_F, efficiency 8.3e-4 at 0.92 mW).
    At 100 K and 1 THz the small-signal efficiency per watt is 0.09 of
    the earlier one in this solver, which has no viscoelastic term, and
    the time-domain and perturbative values differ by 10 % even at the
    smallest amplitude (3 % before: the same absolute difference on a
    response ten times smaller). Those rows were "indicative only" before
    and remain so.

### Steady flow with the gate (`tables_H6.md`, section 5; `dcgate2/tables_H5.md`)

13. **The two conclusions stand; several numbers behind them move.**
    (Corrected on 2026-11-19, when the paper was revised: the first
    version of this item said that nothing changed.) The flip coefficient
    at low bias is 0.4805 V/U_0 for the straight channel as before and
    within 0.6 % of its earlier value for every device. The swap
    asymmetry inside the drift limit is at most 3.0e-4 (4.1e-4 before).
    What moves:
    - The flip term now departs from the inertia-free closed form by up
      to 5 % inside the drift limit (8 % for the 1 um Tesla valve),
      against 2 % (4 %) before: the terms of order u^2/v_F^2 of the exact
      equation.
    - Gated against incompressible: inside the drift limit, 9 of the 10
      points with an incompressible asymmetry above 3e-5 agree within a
      factor two (10 of 10 before); the exception is the 1 um Tesla valve
      at 0.29 v_F, ten times smaller. The paper's earlier count, 21 of 23
      points above 1e-4, was made mostly of points outside the limit,
      where the comparison no longer means anything.
    - Twelve of the 112 points have no steady state (one before): their
      bias would need a drift above v_F.
    - The numerical linear term of the swap asymmetry is larger on every
      mesh and falls by a factor of 16 from 28 to 10 nm (45 before); the
      quadratic coefficient is -0.037 to -0.043 (-0.044 to -0.053). The
      low-bias bounds become 1.4e-5 for the funnel and 5e-7 for the
      Tesla valve at 14 mV (7e-6 and 1.5e-7).

## The guesses of the plan, scored

| quantity | before | guess | now | verdict |
|---|---|---|---|---|
| 1 um funnel, 100 K, best with the DS termination (A/W) | 0.344 | 0.10 to 0.20 | 0.238 | above the band, within a quarter |
| 175 nm funnel, 100 K | 0.446 | 0.14 to 0.27 | 0.428 | outside |
| 1 um funnel, 300 K | 0.092 | 0.045 to 0.060 | 0.066 at 10 GHz; 0.052 to 0.055 from 0.3 to 1 THz | inside between 0.3 and 1 THz |
| 175 nm funnel, 300 K | 0.103 | 0.050 to 0.065 | 0.066 | at the edge |
| noise-equivalent power, 1 um funnel, 300 K (pW/Hz^(1/2)) | 58 | 90 to 130 | 81 at 10 GHz; 97 to 103 from 0.3 to 1 THz | inside between 0.3 and 1 THz |
| shape-only response | | no guess | falls with the DS response; no change of sign where it is above the floor | |
| steady flow: flip term within 1 %, swap term below 1e-3 | | | 0.6 %; 3.0e-4 | inside |
| efficiency where the drift reaches 0.3 v_F | | 1e-6 to 1e-4 | 1e-5 to 4e-5 | inside |
| Tesla valve below the 175 nm funnel by ten or more at 1 THz | 20; 180 | | 18; 28 | inside |

The one number outside its band by more than a quarter is the 175 nm
funnel at 100 K. The guesses applied the uniform channel's ratio, in which
the convective term carries 40 to 65 % of the response, to every device.
At the taper resonance of the 175 nm funnel the old and the new equation
differ by 4 %: the response there is set by the terms in the density
(the density-dependent mass and the contact potential), which the
correction does not touch. This is read off the legacy and corrected
runs; the terms were not computed separately.

## What would change in the paper (not done; gate 2)

Nothing in `paper/` was changed. If the paper is brought into line:

- **Eq. (9)** and the paragraph around it: the corrected equation, the
  exact form, the contact condition named as a choice, the zero
  temperature of the gas.
- **Sec. IV A (small signal)** and **Fig. 6**: every responsivity and
  noise-equivalent power; "40 % higher near resonance" becomes "half";
  the plain-channel control replaces the sentence that says it was not
  computed; the numerical floor of the shape-only response.
- **Sec. IV B (large signal)**, its heading, **Fig. 7** and Eq. (14)'s
  discussion: rewritten around the two limits (0.3 v_F and v_F).
- **Sec. IV C (operating point)** and **Fig. 8**: the grid, the peak
  efficiencies (the saturation factors behind Fig. 8(b) came from the
  old large-signal runs), the contact-resistance numbers.
- **Sec. III C**, the gated paragraph, and **Fig. 5**: the comparison
  with the closed form and with the incompressible solver, the mesh
  numbers (item 13).
- **Fig. 13, Tables III and IV**: the plasmonic rows.
- **Abstract, introduction, discussion, conclusion**: "0.07 to 0.34 A/W",
  "a third to a half", "about one per cent", "60 pW/Hz^(1/2)", the
  comparison of the ballistic 0.12 A/W with the plasmonic element, the
  array figure of about 100 pW/Hz^(1/2).
- **Limitations**: the contact condition and the finite temperature as
  measured uncertainties.
- `OVERVIEW.md` repeats several of these numbers and would follow.

## Limitations

A zero-temperature gas at 100 K and 300 K (20 % at 300 K by the
isothermal estimate, section "Variants"); the contact condition is the
Dyakonov-Shur one and the alternative moves the numbers by up to a third
for the funnels with an antenna and far more for the ideal-source channel;
no heating, no optical-phonon limit on the drift, so everything above
0.3 v_F is the ideal fluid extrapolated; above omega tau_ee = 1 the
carriers are not a drifting equilibrium gas at all (review item B2), and
this stage does nothing about that; local-capacitance electrostatics; the
plain-channel control shows the numerical floor of the shape-only
response on the production mesh of the 1 um funnel.

## In the paper (2026-11-19)

The paper was brought into line as its seventh draft: Eq. (8) and the
paragraph after it, Appendix A (second-order forcing, the channel closed
form, the exact equation), Appendix B (the drift limit on the grid),
Sec. III C, all of Sec. IV, Figs. 5 to 8, 10, 11 and 13, Tables III and
IV, the abstract, introduction, discussion and conclusion, and in the
supplement the test tables (P21 to P27 are T33 to T39 here), the mesh
table and the run parameters. `paper/make_figures.py` reads
`results/thz2` and `results/dcgate2`. Fig. 7 is new (efficiency against
the drift speed) and Figs. 8(a) and 8(b) now show the power and the
efficiency at a drift of 0.3 v_F in place of the saturation power and
the efficiency at 0.7 of the voltage unit.

## Four checks after the review of the seventh draft (2026-11-19 and 20)

Both follow `paper/review_seventh_draft.md`, which has the detail
(sections 1.1 and 3.3 to 3.5).

1. **The 175 nm funnel and the Tesla valve had been run on a 20 nm wall
   mesh, not the 14 nm one of the earlier stages.** My error in
   `tools/momentum_runs.sh`, now corrected; the 22 sweeps were re-run
   (`tools/momentum_runs.sh mesh14`; 65 927 and 97 491 nodes; 1918.6 and
   10 942.8 ohm). Every number for these two devices in this file and in
   `thz2/tables_H6.md` is now from the 14 nm mesh; the 20 nm results are
   in `thz2/hw20` and the comparison in `thz2/tables_mesh14.md`. With
   the DS termination the two meshes differ by 1 to 3.5 % of the largest
   response; the shape-only response by 2.5 to 4 % for the funnel and 8
   to 11 % for the Tesla valve, and where it is below about 5e-4 A/W it
   is not resolved on either mesh. The numbers above that changed: the
   Tesla valve at 300 K and 1 THz (4.1e-4 to 4.5e-4 A/W), its best DS
   value at 100 K (0.092 to 0.094), the ratios 0.62 to 0.61 and 31 to
   28, and the drift range of the 175 nm funnel at 300 K. The 4 % at the
   taper resonance stands (0.970 and 0.960 on equal meshes).
2. **Corner and viscosity check on the 1 um funnel**
   (`tools/momentum_corner.sh`, 80 runs; `thz2/corner/tables.md`).
   Without viscosity the time-averaged equations of the Dirac gas are
   those of the classical gas with current and voltage halved, in any
   geometry. The solver gives 0.50 for a plain channel and 0.16 to 0.43
   for the funnel on the production mesh.
   - That failure is numerical. The ratio rises with the mesh (0.155,
     0.251, 0.310 at 0.1 THz on 80, 40 and 20 nm; 0.428, 0.450, 0.464 at
     1 THz) at the slow rate of a re-entrant corner. Leaving the corner
     nodes free (`--corner_deg 181`) does not cure it. The solver is not
     to be used without viscosity for a channel with corners; no
     production run of a funnel or a valve is.
   - At the production viscosity, free and pinned corners agree to 3 %
     on the 20 nm mesh for the DS termination and to 8 % for the shape
     alone where it is resolved, and approach each other as the mesh is
     refined.
   - **The 80 nm production mesh of the 1 um funnel is too coarse at
     100 K**: the DS responsivity is low by 3, 12 and 12 % at 0.1, 0.3
     and 1 THz against the extrapolated value (0.148, 0.218, 0.211 A/W
     against 0.153, 0.249, 0.240), and the shape-only one by 35 and 24 %
     at 0.1 and 0.3 THz. At 300 K the DS responsivity is converged to
     1 % (2 % at 1 THz) and the shape-only one to 7 % at 0.3 THz. The
     100 K numbers of items 2, 3 and 7 above carry this error; the
     ratios between runs on the same mesh do not.
   - **At 100 K the response depends on the assumed viscosity.** With a
     fifth of it the DS responsivity is 1.37, 0.75 and 0.79 of the
     production value at 0.1, 0.3 and 1 THz, and the shape-only one 4.8
     and 0.58 with a change of sign at 1 THz (40 nm mesh, indicative).
     At 300 K the changes are 3 to 10 %.

3. **Fine-mesh sweeps of the 1 um funnel** (`tools/momentum_runs.sh
   fine`; `thz2/tables_fine.md`; `thz2/fine/mesh_80_20.png`). The four
   headline sweeps on the 20 nm mesh (98 963 nodes) and on the 40 nm
   mesh.
   - Up to 1 THz the 20 nm values are within 6 % of the mesh limit for
     the DS termination at 100 K and within 0.3 % at 300 K. **The 100 K
     numbers of items 2 and 3 above become 0.14 to 0.26 A/W from 10 GHz
     to 1 THz (0.20 to 0.26 from 0.3 to 1 THz), the shape-only response
     0.034 to 0.100 A/W from 0.1 to 1 THz, and the noise-equivalent
     power 22 to 28 pW/Hz^(1/2) from 0.3 to 1 THz.** The 300 K numbers
     and the share of the shape are unchanged.
   - **Above about 1.2 THz the 80 nm mesh is wrong and no mesh used is
     converged.** Its interior elements reach 480 nm, 2.5 per plasma
     wavelength at 3 THz. At 3 THz it gives -0.148 A/W at 100 K where
     the 20 nm mesh gives +0.115, and 0.025 at 300 K against 0.044.
   - **The grid (item 6) is sound at 0.1 and 0.3 THz and not at 1 THz.**
     At the point with the slowest plasma wave (5e10 cm^-2, 5 nm) the
     two meshes agree to 2 % at 0.1 and 0.3 THz and give -0.36 against
     +1.83 A/W at 1 THz at 100 K. What item 6 says about the
     low-density, thin-gate corner at 1 THz is withdrawn.
   - A check with 60 nm interior elements shows that the 20 nm mesh is
     good to about 3 % up to 3 THz at 300 K; at 100 K it is not
     converged above 1.2 THz.
4. **The other small-signal runs of the 1 um funnel on the 20 nm mesh**
   (2026-11-20; `tools/momentum_runs.sh fine2`, 127 runs;
   `thz2/tables_fine2.md`). Contact variants, finite-temperature
   coefficients, plain channel, grid, matched antenna, scattering time,
   contact resistance.
   - The variants keep their ratios: the energy-conserving contact gives
     0.65 to 0.93 of the default at 100 K and 0.65 to 0.74 at 300 K
     (DS termination, 0.1 to 1 THz); the finite-temperature
     coefficients 1.16 to 1.23 at 300 K.
   - The funnel gives 0.83 to 0.85 of the plain channel at 300 K on
     either mesh. The "numerical floor" of item 4 was the interior
     mesh: the symmetric channel gives at most 3e-6 A/W up to 1 THz on
     the 20 nm mesh (7.9e-3 before).
   - The grid at 0.1 and 0.3 THz agrees between the meshes to 2.5 %
     except at the reference point at 100 K (9 % at 0.3 THz). At 1 THz
     the 80 nm values were wrong by factors of 1.1 to 15; on the 20 nm
     mesh the efficiency at a drift of 0.3 v_F is 2.2e-5 to 5.4e-4 at
     100 K and 4.5e-5 to 1.7e-4 at 300 K over the valid points and all
     three frequencies.
   - Contact resistance at the reference point: unchanged at 300 K; at
     100 K the noise-equivalent power is 2 to 16 % lower.
   - All large-signal runs are still on the 80 nm mesh.

Still to be corrected in this file: items 3, 7, 8 and 12 of "Small
signal" and "Large signal" quote values at three sampled frequencies as
ranges.

## In the paper, eighth draft (2026-11-20)

The text and the figures were corrected in one pass after the review
(`paper/review_seventh_draft.md`, section 4, item 3).

- **Where the numbers come from now.** `tools/paper_numbers.py` writes
  `results/thz2/paper_numbers.md`: every fluid number of the paper as a
  range over a stated band. It reads the small-signal runs of the 1 um
  funnel, of the plain channel and of the grid from `results/thz2/fine`
  (20 nm mesh), the 175 nm funnel and the Tesla valve from
  `results/thz2` (14 nm), and the large-signal runs from `results/thz2`
  (80 nm). `paper/make_figures.py` reads the same files (`FINE_DIR`).
- **Results items 3, 7, 8 and 12 above** quote values at three sampled
  frequencies as if they were ranges, on the 80 nm mesh. They are kept as
  the record of Stage H6; `paper_numbers.md` supersedes them. The band
  values: share of the shape in the 1 um funnel between 0.3 and 1 THz
  0.27 to 0.38 at 100 K and 1.6 to 4.2 % at 300 K (item 3); drift per unit
  amplitude 3.9, 4.5 and 4.1 v_F per V_u at 0.1, 0.3 and 1 THz at 100 K in
  the small-signal solver against 3.6, 2.9 and 1.7 in the time-domain
  solver, equal to 3 % at 300 K (item 7); energy-conserving contact 0.65
  to 0.93 of the default at 100 K and 0.65 to 0.74 at 300 K between 0.1
  and 1 THz (item 8); the drift of 0.3 v_F is reached, interpolated
  between ladder amplitudes, at 26, 38 and 104 uW at 100 K and 100 to
  134 uW at 300 K, with efficiencies of 1.3e-5 to 6.4e-5 (item 12).
- **Figures.** Fig. 6(b), Fig. 8 and the plasmonic curves of Fig. 13 are
  drawn from the 20 nm runs; the 100 K curves of the 1 um funnel end at
  1 THz; a grey band marks shape-only responses that the mesh does not
  resolve (below 1e-4 A/W for the 1 um funnel, 5e-4 A/W for the 175 nm
  devices). Figs. 6(c, d) are from the 14 nm runs. Fig. 7 is unchanged.
- **The comparison with the ordinary convective term, on the 20 nm mesh
  (2026-11-20).** No new run was made for the correction pass itself, and
  the comparison of the 1 um funnel with the ordinary convective term
  then used the 80 nm mesh for both equations. Two sweeps with
  `--closure legacy` on the 20 nm mesh (`tools/momentum_runs.sh legacy20`,
  files `results/thz2/fine/L_funnel_w1000_{100K,300K}_dss`) put it on the
  fine mesh. Dirac gas over ordinary term, DS termination, 10 GHz to
  1 THz: 0.574 to 1.04 at 100 K and 0.584 to 0.944 at 300 K (80 nm: 0.573
  to 1.05 and 0.579 to 0.944); 0.3 to 1 THz: 0.69 to 1.04 and 0.584 to
  0.615 (80 nm: 0.631 to 1.05 and 0.579 to 0.615). The ordinary-fluid
  response itself rises by up to 12 % at 100 K and 1 % at 300 K from the
  80 nm to the 20 nm mesh, as the Dirac one does. The sentences of the
  paper ("up to 1.7 times larger", "between nothing and four tenths",
  "about four tenths") are unchanged.
