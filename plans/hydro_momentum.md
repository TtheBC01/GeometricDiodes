# Plan: the momentum equation of the Dirac fluid, corrected, and the fluid runs repeated (Stage H6)

Written 2026-11-17, before any code or run. Approved the same day with the
exact large-signal model; carried out 2026-11-17 and 18, results in
`results/HYDRO_MOMENTUM.md` (which lists the deviations from this plan). Companion to
`paper/review_fifth_draft.md` (item B1 and the section "B1 checked against
the cited papers"), `plans/hydro_thz_notes.md` (the derivation the solvers
follow) and `results/THZ.md`, `results/HYDRO_GATE.md` (the runs to be
repeated). Figure: `plans/hydro_momentum.png` (script
`tools/momentum_plan_fig.py`, which also prints the numbers quoted here).

The gated-fluid solvers use the convective term of an ordinary fluid,
`d_t u + (u.grad)u`, with a mass that depends on the density. For a
degenerate Dirac gas that is not the momentum equation. Both papers the
manuscript cites for it have a weaker term, and a one-dimensional estimate
puts the Dyakonov-Shur responses 1.6 to 3 times too high (figure panels a
and b). Stage H6 corrects the equation in the three places it appears,
proves the correction with new checks, repeats every run that used it, and
reports old against new. **The paper is not touched in this stage.**

## 0. What is known

**The equation.** For electrons with `E = hbar v_F |k|` drifting at speed
`u`, at fixed laboratory density `n`, the momentum density is
`n m(n) u` and the momentum flux is `P0(n) + (3/4) n m u^2` along the
drift and `P0(n) - (1/4) n m u^2` across it, with `m(n) = hbar k_F / v_F`
(own numerical moments, 2026-11-17; an ordinary fluid has 1 and 0). With
continuity this gives, to second order in `u`,

    d_t u + (u.grad)u - (1/2) u (div u) - (1/4) grad(u^2) + u/tau = -grad h(n) + nu lap u.

The solvers have the first two terms on the left only. In one dimension
the three convective pieces add to `alpha u d_x u` with

| form | alpha |
|---|---|
| manuscript, Eq. (9), and all three solvers | 1 |
| Tomadin and Polini 2013, Eq. (6) | 1/2 |
| Svintsov 2013, Eq. (19), degenerate limit; own moments | 0 |

**The size.** For the uniform channel of Fig. 6(a) the response relative to
the manuscript's is 0.34 to 0.60 at 100 K and 0.54 to 0.63 at 300 K for
alpha = 0, between 10 GHz and 3 THz (0.67 to 0.82 for alpha = 1/2). At the
resonances the graphene response is half the classical one, not 1.4 times
it. The efficiency goes as the square.

**A second problem, found while preparing this plan** (panel c). The wave
speed at the reference point is 3.57 v_F, so a density swing of 28 % moves
the carriers at v_F. In a one-dimensional stand-in for the large-signal
runs (75 ohm source on a uniform lead, drain grounded) the drift speed is
1.7 to 4.3 v_F times the amplitude in units of the gate overdrive. It
passes 0.3 v_F, the limit the steady-flow sections apply, at an amplitude
of 0.07 to 0.18, and v_F itself at 0.23 to 0.60. Of the eight amplitudes
run (0.01 to 0.7), three or four are inside the limit and the top one to
three ask the carriers to move faster than v_F. The neck of the funnel
concentrates the current, so the real figures are worse. The section
heading "a square law up to the gate overdrive" and the efficiency quoted
at 0.7 of the overdrive are therefore outside the model, whatever the
convective term. This was not in the review.

## 1. The corrected model

**Small signal.** The second-order forcing of the perturbation solver
becomes

    f = -(1/2) Re[(u1* . grad) u1] + (1/4) Re[u1* (div u1)] + (1/8) grad |u1|^2.

Nothing else changes at this order: continuity, the first-order problem,
the contact condition and its dc term are as they were.

**Large signal and steady flow: exact in the drift speed (recommended).**
The ideal fluid has a closed form at any speed below v_F. With
`gamma = (1 - u^2/v_F^2)^(-1/2)`, momentum per particle
`m(n) gamma^(1/2) u` and pressure `P0(n) gamma^(-3/2)`,

    (1 + gamma^2 u u^T / 2 v_F^2)(d_t u + (u.grad)u) - (1/2) u (div u) - (1/4) grad(u^2) + u/tau
        = -(1/m) [gamma^(-1/2) grad(e phi_gate) + gamma^(-2) grad mu(n)] + nu lap u.

In the solver's units `u^2/v_F^2 = u^2 / 2q`, with `q` the
quantum-capacitance share that the closure already carries, so no new
parameter enters. It reduces to the second-order form above at small `u`,
and the inertia diverges as `u` approaches v_F, which is the saturation
the present runs lack. Every run records the largest `u / v_F`.

**The simpler alternative.** Put only the second-order terms in the time
and steady solvers, and use no result above 0.3 v_F. This is less code
(the exact form is the larger half of step 1) and leaves three or four
amplitudes per run, all on the square law. The large-signal section of the
paper would then say that the model is a square law up to its own limit,
near 40 uW per element, and give no saturation curve.

I recommend the exact form: it removes an arbitrary truncation, it gives
the perturbation solver an independent partner to be checked against in
two dimensions (check T39), and it lets the paper say what the ideal fluid
does up to v_F. Results above 0.3 v_F would still be labelled as the ideal
fluid extrapolated, because heating and optical-phonon scattering, which
the model lacks, set in there.

**Kept as they are, and why.**

- The classical (shallow-water) closure: an ordinary fluid, coefficients
  (0, 0). The Dyakonov-Shur benchmark does not move.
- The contact condition, `Phi(n) = V` at the laboratory density. A contact
  on a drifting Dirac fluid is not a settled problem; both cited papers
  fix the density at the source as here. A drift term in it would be of
  the same order as the correction itself. One variant run bounds it
  (section 4, C2).
- Friction `u/tau` (exact if momentum relaxes at rate 1/tau) and the
  viscous term `nu lap u` (a model at this order).
- The mass and pressure of the gas at zero temperature. Finite-temperature
  terms are of relative order `(k T / E_F)^2`: 0.05 at 300 K and 0.005 at
  100 K at 1e12 cm^-2, times a factor of order one that step 0 reads from
  Svintsov's paper. If it comes to more than 10 % at 300 K the two
  coefficients are set to their finite-temperature values, which the code
  change allows.

## 2. Step 0: before any code

No runs. Stop and report if any of these changes section 1.

1. Read both cited papers page by page (the check so far was through a
   summarising fetch of the full text) and write down their
   two-dimensional forms. The plan assumes Svintsov's agrees with the
   equation of section 0; Tomadin and Polini's is used only in one
   dimension.
2. Confirm the stress of a moving Dirac fluid, `w gamma^2 u u / v_F^2 + P`
   with `w = 3P`, against a review (Lucas and Fong 2018, or Narozhny 2019;
   from memory, to be verified). The exact form rests on it.
3. Read the finite-temperature coefficient from Svintsov at
   `E_F / kT` = 4.5 and 13.6.
4. Re-derive the second-order forcing from the conservative form
   independently of the velocity form, on paper, including the contact
   terms.

## 3. Step 1: code

| file | change |
|---|---|
| `src/hydro/thz_common.hpp` | `ClosureFn` gains the two coefficients (divergence, gradient): classical (0, 0); graphene (-1/2, -1/4); a legacy switch (0, 0) for graphene that reproduces every old number; (-1/2, 0) for the Tomadin-Polini column of the benchmark |
| `src/hydro/thz_solver.cpp:227-229` | the two new forcing terms |
| `src/hydro/thz_time.cpp:97-118` | residual and Jacobian of the new terms; with the exact option, the acceleration matrix and the two `gamma` factors; a guard that rejects a Newton step past v_F |
| `src/hydro/thz_time.hpp`, apps | `u_max / v_F` in every output row; `--closure legacy` option; output folder option |
| `apps/thz_tests.cpp` | checks T33 to T39 |
| `tools/*.py` | independent closed forms and quadratures for the checks; tables of old against new |

The steady gated solver is the time solver with no time derivative, so it
is covered by the third row. The incompressible solver of Sec. III B is
not touched: the gradient term joins the pressure and the divergence term
vanishes.

## 4. Step 2: checks (gate 1)

Every check must pass before a production run starts. If one fails and the
cause is not found, the work stops there and is reported.

| check | what | criterion |
|---|---|---|
| T33 | legacy switch: the Dyakonov-Shur check T4, one stored funnel point (1 um, 100 K, 0.3 THz, both terminations) and the time-domain check T6 give the stored numbers | 1e-8 relative |
| T34 | uniform channel, new terms, against the closed form with alpha = 0 and with alpha = 1/2, damping 1 and 0.1, ten frequencies | 1 % |
| T35 | a small pulse on a channel carrying a steady drift of 0.07 and 0.18 v_F: the up- and downstream speeds average to half the drift (Svintsov, Eq. (39); the legacy form gives the full drift) | 5 % of the drift |
| T36 | steady gated channel, voltage against current, both signs, against a quadrature of the exact one-dimensional equations, to 0.3 v_F and to 0.8 v_F | 1e-3 and 1e-2 |
| T37 | steady radial flow in a 60 degree sector of a ring (1 to 3 um, slip side walls, weak friction), against a radial quadrature written in the conservative form from the stress tensor. One dimension only tests the sum of the three terms; this tests them separately | inertial part of the voltage within 3 % |
| T38 | the 1 um funnel mesh rotated by 30 degrees: small-signal current and one steady point unchanged | 1e-6 relative |
| T39 | time-domain solver against the perturbation solver with the new terms: channel (as T6) and 1 um funnel with both terminations (as T10, T14) at amplitudes 0.01 and 0.02 | 2 %, 3 %, 5 % as before |

Then the old ladder T1 to T20 is run again with the new default. The
references of T9, T17 (graphene against classical in a channel) and T18
(closed form with inertia) change; the new values are computed from the
closed forms and written down before the run.

Two variant runs, not checks, reported with the benchmark:

- **C1, plain-channel control** (asked for in the review, never run): a
  uniform channel with the leads' width and the full length of the 1 um
  funnel, both terminations, both temperatures. It separates what the
  shape adds from what the terminations give.
- **C2, contact variant**: the uniform channel and the 1 um funnel at
  0.3 and 1 THz with a drift term `u^2/4` added to the contact condition,
  to bound the sensitivity to the one boundary condition that is a choice.

## 5. Step 3: runs

All outputs go to `results/thz2/` and `results/dcgate2/`. The old files
stay where they are and the paper's figure script keeps reading them.

| block | runs | what the paper takes from it | cost |
|---|---|---|---|
| 3a small signal | S1 channel (with a Tomadin-Polini column), S2 funnels (16 files), S3 Tesla valve (4), D1, D1m, D1t density and gate grid (75), D2, D2m contact resistance (36), C1, C2 | Fig. 6, Fig. 8, Fig. 13, the plasmonic rows of Tables III and IV, the noise-equivalent powers | about 950 solves of 20 s; under an hour on ten cores |
| 3b steady gated flow | D1 to D4 of Stage H5 and the reference | Fig. 5 and the swap and flip asymmetries of Sec. III B | 128 points; 30 min |
| 3c large signal | P1 to P4, D3, the 120-step repeats | Fig. 7, Fig. 8(b), the efficiency | 74 amplitudes; 13 CPU-hours, about 2 h on ten cores |

With the ladder (80 min as last time) this is four to five hours of
machine time. In 3c each run goes up in amplitude until the drift reaches
0.9 v_F or Newton fails, and says which; the top amplitudes of the old
runs may not be reachable.

## 6. Guesses registered before the runs (panel e)

- **Dyakonov-Shur termination, best value over frequency.** The channel
  ratios applied to the present numbers, widened: 1 um funnel 0.34 to
  0.10 to 0.20 A/W at 100 K and 0.09 to 0.045 to 0.060 at 300 K; 175 nm
  funnel 0.45 to 0.14 to 0.27 at 100 K and 0.10 to 0.050 to 0.065 at
  300 K. The noise-equivalent power at 300 K with ideal contacts goes from
  59 to 90 to 130 pW/Hz^(1/2).
- **Shape-only response** (symmetric termination): no guess. The terms
  that change are the ones that produce it, and its sign may change.
- **Steady gated flow.** The flip term, 0.48 V/U_0, has no inertia and
  should not move by more than 1 %. The swap term, at most 4e-4, should
  stay below 1e-3.
- **Efficiency.** At the amplitude where the drift reaches 0.3 v_F,
  between 1e-6 and 1e-4. Above it, no guess.
- **Tesla valve.** Stays below the 175 nm funnel by a factor of ten or
  more at 1 THz at both temperatures.

A number that lands outside its band by more than a quarter is explained
before it is used.

## 7. Step 4: the report (gate 2)

`results/HYDRO_MOMENTUM.md`: the checks; every number the paper takes from
the fluid model, old, new and ratio; the guesses scored; the largest drift
speed of every large-signal point; and a list of the sentences, figures
and table cells of the paper that would change. What the outcomes would
mean, fixed now:

| outcome | for the paper |
|---|---|
| termination numbers inside the bands | the statements stand with lower numbers; at 300 K the ballistic short funnel (0.12 A/W per absorbed watt) is ahead of both fluid funnels instead of level with them; the array figure of "about 100 pW/Hz^(1/2)" is re-derived |
| shape-only share of the funnel stays a third to a half at 100 K | "rectifies mainly through its terminations" stands |
| shape-only share falls below a tenth | the fluid funnel rectifies through its terminations only; simpler statement |
| shape-only response grows or changes sign | repeated on a finer mesh before it is believed; then the shape paragraph is rewritten |
| C2 moves the benchmark by more than a quarter | the contact condition is stated as a first-order uncertainty of every fluid number |

The paper is revised only after a separate go-ahead: Eq. (9) and a
paragraph on it, Sec. IV, the gated paragraph of Sec. III B, Figs. 5 to 8
and 13, Tables III and IV, and the fluid numbers in the abstract,
discussion and conclusion.

## 8. Limits that remain

A zero-temperature gas at 100 K and 300 K; one choice of contact
condition; no heating, and no optical-phonon limit on the drift, which in
real graphene acts below v_F; above `omega tau_ee = 1` the carriers are
not a drifting equilibrium gas at all (review item B2), and this stage
does nothing about that; local-capacitance electrostatics.

## 9. For approval

1. The stage as a whole, in the order of panel (d).
2. The large-signal model: exact in the drift speed (recommended) or the
   simpler alternative of section 1.
3. C1 and C2 are included unless struck.
