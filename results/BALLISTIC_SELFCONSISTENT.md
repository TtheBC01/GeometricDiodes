# Stage B8 results: the short funnel with the field computed (2026-11-12)

Plan: `plans/ballistic_selfconsistent.md` (figure
`plans/ballistic_selfconsistent.png`; checkpoint in its section 10, figure
`plans/ballistic_selfconsistent_checkpoint.png`). Code: `src/pic.*`,
`src/fft2d.*`, `apps/bpic.cpp`, ladder T22 to T30 in `apps/pic_tests.cpp`.
Runs: `tools/pic_runs.sh` (core set P1 to P4), `tools/pic_checks.sh`
(two repeats), `tools/pic_ext.sh` (extension P5, P6 and four checks),
results in `results/bpic/` (`P1.csv` to `P6.csv`, `P6c.csv`,
`checks.csv`, `runs.log`, `ext.log`, potential maps `P1_d*_V*.txt`,
`map_d*.txt`), tables `results/bpic/tables_B8.md`
(`tools/pic_tables.py`), figures `B8_potential.png`,
`B8_responsivity.png` and `B8_extension.png` (`tools/pic_plot.py`).
The extension (points 10 to 12 of the verdict and the section
"Extension") was run after the core set, on the same day.

The question: Stage B7 found that the 0.55 um funnel G3 rectifies at 1 THz
at the room-temperature mean free path, 0.70 A/W per absorbed watt, with
the field inside the device imposed. Does that survive when the field is
computed from the charge of the electrons themselves, with a gate and
metal contacts?

## Verdict

1. **It stands, weakened, near the bottom of that band.** At the decision
   point (G3, mean free path 700 nm, 1 THz, 0.1 V, gate at 300 nm,
   shape-only drive) the device rectifies 0.429 +- 0.012 uA of 3.44 uW
   absorbed: **R_abs = 0.124 +- 0.003 A/W** (seven runs), and
   R_1 = 0.023 A/W for one device on a 75 ohm antenna. Under the 30 nm
   gate: 0.483 +- 0.013 uA, 0.132 +- 0.003 A/W, R_1 = 0.026 A/W (six
   runs). The plan's band for "stands, weakened" is 0.10 to 0.35 A/W;
   "confirmed" needed 0.35 A/W and R_1 of 0.07 A/W. Stage B7 gave
   0.70 A/W and R_1 of 0.17 and 0.14 A/W. In B7's own definition (per watt
   absorbed at the static conductance) the computed figure is 0.100 A/W
   against 0.70: a factor of seven.
2. **Numerical spread, not counting noise, sets the margin above
   "withdrawn".** The errors above are statistical. The numerical
   variations of the checkpoint moved the current between 0.38 and
   0.49 uA, and the symmetric control scatters by up to 0.06 uA
   (point 6). Taking +- 0.05 uA as the systematic uncertainty, R_abs is
   0.124 +- 0.015 A/W: above the 0.10 line, by less than two of those.
3. **The loss is already there at dc.** The static asymmetry is 1.053 at
   0.1 V under both gates, against 1.56 (uniform field) and 1.13 (drop at
   the neck) in B7. The quasi-static rectified current at 0.1 V is
   1.07 +- 0.02 uA (300 nm gate) and 1.04 +- 0.02 uA (30 nm), against 9.5
   and 2.4 uA. The driven runs at 0.1 THz give 1.04 +- 0.01 and
   1.02 +- 0.01 uA, so the driven and static calculations agree. The
   low-frequency responsivity is 0.24 A/W.
4. **Why: the computed potential is neither B7 profile**
   (`B8_potential.png`). About 0.61 of the bias drops within 90 nm of the
   neck, centred on it; 0.14 along the lead and taper; and 0.25 at the two
   contact planes (0.06 to 0.09 at the source, 0.17 to 0.19 at the drain),
   where it does nothing for the asymmetry. The gate distance changes
   these fractions by 0.02 at most. If the rectified current of the B7
   neck-drop model is scaled by the square of the fraction at the neck it
   gives 2.42 x 0.61^2 = 0.90 uA, against 1.07 uA found; that is a rough
   account, not a derivation.
5. **The frequency dependence is that of the uniform-field model, at a
   seventh of its height** (`B8_responsivity.png`, panel a). Under the
   300 nm gate the current is 1.04, 0.94, 0.71, 0.43, 0.15 uA at 0.1,
   0.3, 0.6, 1 and 1.8 THz: half the low-frequency value near 0.9 THz
   (B7 uniform field: 0.7 THz), and zero at 6 THz (-0.01 +- 0.03 uA). The
   rise of the B7 neck-drop curve towards 3 THz is not there. The
   agreement of the two imposed profiles at 1 THz, which B7 read as "the
   answer no longer depends on where the field drops", was a falling
   curve crossing a rising one.
6. **The control is zero within its scatter, and the scatter is larger
   than the batch errors say.** Six runs of the mirror-symmetric device
   (0.3, 1, 3 THz, both gates): mean +0.008 +- 0.023 uA, none above
   2.0 standard errors, but four of the six between 1.7 and 2.0
   (chi-squared 14.4 for 6). For these runs the batch error is too small
   by a factor of about 1.5, or there is a floor near 0.05 uA. The seven
   and six runs at the decision point scatter as their errors predict
   (chi-squared per degree of freedom 1.0 and 1.6).
7. **Shape against circuit: under the 30 nm gate at 1 THz the circuit
   term is the larger.** A straight 546 nm x 500 nm strip with its drain
   tied to the gate rectifies 1.15 +- 0.04 uA (0.200 A/W); the funnel
   with the shape-only drive 0.48 +- 0.01 uA (0.132 A/W). The funnel with
   its drain tied to the gate gives 1.01 +- 0.02 uA (0.214 A/W): no more
   current than the strip, and the same per absorbed watt within 7 %. At
   0.3 THz the order is reversed (shape 0.89 uA and 0.208 A/W, circuit
   0.67 uA and 0.052 A/W). Under the 300 nm gate the shape term is the
   larger at 0.3, 1 and 3 THz (0.94, 0.43, 0.19 uA against 0.13, 0.34,
   0.01 uA). B7's statement that the shape mechanism is at least as
   strong as the circuit mechanism holds for a distant gate and fails for
   a close one at 1 THz.
8. **Under the 30 nm gate there are plasma resonances, and at 3 THz the
   strip's circuit term changes sign.** With the drain tied to
   the gate the funnel absorbs 16.7 uW at 3 THz, four times its
   low-frequency value, and the strip rectifies -1.79 +- 0.02 uA there
   against +1.15 uA at 1 THz. Both 3 THz runs were repeated with a longer
   settling time and did not move (below). The three frequencies sampled
   are too few to place the resonance or the zero crossing, so the
   close-gate numbers above 1 THz are points on a curve I have not
   resolved. None of this touches the decision point.
9. **My expectations were too high on every count.** Stated before the
   runs: static asymmetry 1.1 to 1.3 (found 1.05); R_abs 0.3 to 1.0 A/W
   (found 0.12); "withdrawn" one chance in five (not withdrawn, but
   within two systematic errors of it). The argument I gave for
   confidence, that two very different imposed profiles agreed, was the
   coincidence of point 5.
10. **Extension: the mean free path matters little per absorbed watt.**
   At 1 THz R_abs is 0.130 +- 0.005 A/W with no scattering, 0.124 at
   700 nm and 0.107 +- 0.007 at 100 nm (300 nm gate; 0.145, 0.132 and
   0.099 under the 30 nm gate). In B7 the same step from 700 to 100 nm
   cost a factor of three with the uniform field. Scattering lowers the
   static asymmetry (1.09, 1.05, 1.02) and the quasi-static current
   (2.1, 1.1, 0.23 uA), but the 1 THz current falls much less (0.50,
   0.43, 0.22 uA): the roll-off between dc and 1 THz is a factor of four
   with no scattering and absent at 100 nm. The conductance halves at
   100 nm, so the single device on 75 ohm does lose a factor of two
   (R_1 0.0125 against 0.023 A/W). With the 0.05 uA floor the 100 nm
   figure is 0.107 +- 0.024 A/W, on the "withdrawn" line.
11. **Extension: half-diffuse walls remove the effect.** With half of the
   wall reflections diffuse (700 nm) the static asymmetry is 1.005 and
   the 1 THz current is -0.073 +- 0.013 uA (300 nm gate) and
   -0.134 +- 0.011 uA (30 nm gate): 17 and 28 % of the specular
   current, with the opposite sign. Two seeds agree, and the
   mirror-symmetric control with the same walls is zero within the
   floor. So |R_abs| is at most 0.04 A/W, and the positive result of
   this stage is a result for specular edges. B7 gave 0.42 and
   0.14 A/W for this case. No specularity between 0.5 and 1 was run.
12. **Extension: amplitude.** At 0.05 V the current is 0.099 uA under
   both gates, 0.92 +- 0.17 and 0.82 +- 0.13 of the square law through
   the 0.1 V point (a weak test: the floor is half the signal). At
   0.2 V it is 1.50 +- 0.04 and 1.38 +- 0.04 times the square law
   (2.56 and 2.66 uA; R_abs 0.20 A/W). Both imposed fields of B7 fell
   below the square law at 0.2 V; the computed field rises above it.

**For the paper** (done in the third draft of 2026-11-12, `paper/outline.md`).
The short-funnel headline of 0.70 A/W has to become 0.12 A/W, and R_1 of
0.17 has to become 0.023 A/W. The short ballistic funnel is then level
with the plasmonic funnel at room temperature (0.09 A/W at 300 K) and
below its 100 K figure (0.34 A/W), not ahead by a factor of eight. The
length criterion survives: the response still rolls off near
f = 0.5 v_F / L, half at 0.9 THz for 0.55 um (B7 had 0.4 v_F / L). "At least as strong as the
circuit mechanism" has to be limited to a distant gate. The limitation
"imposed field" goes; the limitations of section "Limits" below replace
it. B7's "defensible room-temperature range" of 0.14 to 0.7 A/W
becomes: 0.10 to 0.13 A/W per absorbed watt for specular edges at any
mean free path from 100 nm up (0.012 to 0.023 A/W for one device on
75 ohm), and nothing usable, at most 0.04 A/W with the sign reversed,
for half-diffuse edges. The paper has to say that the short-funnel
result depends on the edges being close to specular, and that how close
has not been computed.

## Acceptance ladder (`bpic/ladder_B8.txt`; 11 passed, 0 failed)

| test | result |
|---|---|
| T22 kernel | Point charge with image to 3e-12; charged disc over the gate to 1.4e-7 of the closed form |
| T23 capacitance of metal blocks | Plate over the gate 0.970 of the formula with fringing; coplanar strips 0.990 of the elliptic-integral value |
| T24 static screening | Reworked (deviation 3). Kernel: U_ind / (G dn) = 1.000 +- 0.001. Particles alone: 0.980 of the density of states. Loop at weak coupling: 1.005 and 0.992 of 1 / (1 + nu G). Production coupling: U_tot / U_ext 0.96 to 0.99 of theory at 1.0 to 2.5 ps, 0.93 to 0.96 at 3.5 to 5.0 ps |
| T25 plasma wave | Speed 0.983 to 0.992 of the collisionless value |
| T26 B7 reproduced with the field imposed | Four of six values within 1.3 standard errors, two static values at 2.0 (0.2 to 0.3 % of the current); 1 THz: 2.92 +- 0.05 against 3.03 +- 0.10 uA and 2.63 +- 0.04 against 2.78 +- 0.11 uA |
| T27 conductance | Strip: 1.006 and 1.002 of the Sharvin value. G3 with the interaction on: 1.0004 and 1.0076 of the value with it off |
| T28 conservation | Charge to 4e-13 electrons; contact potentials to 6e-16 eV; absorbed power from <V I> and from the first harmonic agree to 0.01 % |
| T29 no drive, and mirror device | +0.03 +- 0.04 uA at both gates, kinetic energy +0.04 % and 0.00 % in 20 ps; mirror device -0.08 +- 0.07 uA |
| T30 convergence | Converged from 4 million particles; 1 and 2 million are biased high (table below) |

Decision current against particle number (uA; several entries are
several seeds):

| particles | gate 300 nm | gate 30 nm |
|---|---|---|
| 1 million | 0.60 +- 0.07 | 0.55 +- 0.04 |
| 2 million | 0.42 +- 0.03, 0.61 +- 0.06 | 0.63 +- 0.03, 0.45 +- 0.06 |
| 4 million | 0.47, 0.41, 0.49 (+- 0.04) | 0.46, 0.45, 0.52 (+- 0.05) |
| 8 million | 0.42 +- 0.03, 0.40 +- 0.02 | 0.49 +- 0.02, 0.52 +- 0.02 |
| 16 million | 0.42 +- 0.02, 0.44 +- 0.02 | 0.46 +- 0.01 |

At 4 million particles: grid 2.5 nm 0.38 +- 0.05 / 0.52 +- 0.04; time
step 1.25 fs 0.44 / 0.48; low-pass 0.75 cells 0.46 / 0.43; contacts
100 nm long 0.44 / 0.49, 400 nm 0.47 / 0.53; contact blocks of 4 nodes
0.46 / 0.57. A low-pass of 3 cells gives 0.58 / 0.62, 30 % high: the field
has to be resolved to 7.5 nm or better.

## Deviations from the plan

1. **Tests** are in `apps/pic_tests.cpp`, not `apps/tests.cpp`. T22 uses a
   point charge and a charged disc in place of the uniform sheet; T23 the
   fringing formula for the plate and the difference of two lengths for
   the strips; T25 the central band of a wide strip, because the edge mode
   of a hard-edged strip is 9 % slower; T27 extrapolates the strip to zero
   bias from +-5 and +-10 mV.
2. **T26 passes at 2.5 standard errors, not 2,** and T29 ran 20 ps, not
   30.
3. **T24 does not meet the plan's 3 % at production coupling.** The total
   potential is a 14- to 30-fold cancellation between the applied and the
   induced one, so it amplifies a 0.2 % interpolation loss and the
   relaxation below. The test was split: particles alone (3 %), the loop
   at weak coupling (3 %), and at production coupling the kernel and the
   induced potential to 1 % and the total to 10 % in an early window.
4. **Sample particles collide.** Collisions between them relax the Fermi
   distribution towards a classical one. In a closed box the static
   response is 30 % too large after 5 ps at 1.4 particles per nm^2, 8 % at
   8 per nm^2 and 4 to 7 % at 19 per nm^2 (a 4 million particle run of
   G3). In the open device a particle stays about 1 ps before a contact
   replaces it, and the decision number does not move from 4 to
   16 million particles. This, not heating, limits the particle number
   from below.
5. **Averaging window.** The plan's definition was 20 periods after 10 of
   settling, estimated over the device volume. The runs use at least
   10 ps and at least six periods after 3 ps or more of settling (10 ps at
   0.1 THz), and the mean of the source-plane and drain-plane estimators.
   The volume estimator is in the result files: it agrees within 0.03 uA
   in the driven runs, 0.06 uA in the two resonant ones, and is 0.8 to
   1.0 % low in the static runs.
6. **Numerics.** Grid spacing adjusted so that both contact planes lie on
   node columns (5.013 nm for G3); force from the gradient of the
   interpolated potential (energy conserving); Gaussian low-pass of
   1.5 cells; 8 million particles for the driven runs, 4 million for the
   static runs and at 0.1 THz.
7. **The plasma resonances are not where section 0 of the plan put them.**
   It listed a quarter-wave resonance at 1.8 THz under the 30 nm gate.
   Both contacts hold the potential, so there is no quarter-wave mode;
   see "Plasma resonances" below.
8. **Two runs were repeated** with a 10 ps settling time
   (`tools/pic_checks.sh`), outside the planned set, because their two
   current estimators disagreed.
9. **Extension.** The 1 THz runs of P5 and P6 average over 20 periods,
   not 10, except at 0.2 V. Added to the plan's list: the static pair at
   +-0.1 V with half-diffuse walls (4 runs), and four checks on the
   half-diffuse result (the control with the same walls and the funnel
   with a second seed, both gates; stage P6c of `tools/pic_ext.sh`).
   P5 has static points at +-0.1 V only, so its quasi-static current is
   half the even part, with the even part taken as V^2; for the 700 nm
   case this is 3 % above the four-point value. P7 (the 6 um device)
   was not run.

## Static curve (P1)

| model | I(+0.05 V) | I(-0.05 V) | I(+0.1 V) | I(-0.1 V) | A at 0.05 V | A at 0.1 V | G (mS) | quasi-static I_dc at 0.1 V (uA) |
|---|---|---|---|---|---|---|---|---|
| computed field, gate 300 nm | 44.96 | -43.95 | 86.65 | -82.26 | 1.023 | 1.053 | 0.859 | 1.07 +- 0.02 |
| computed field, gate 30 nm | 44.97 | -44.02 | 86.45 | -82.13 | 1.022 | 1.053 | 0.859 | 1.04 +- 0.02 |
| B7, uniform field | 49.65 | -39.58 | 103.67 | -66.57 | 1.254 | 1.557 | 0.865 | 9.54 +- 0.05 |
| B7, drop at the neck | 44.13 | -41.49 | 79.85 | -70.60 | 1.064 | 1.131 | 0.787 | 2.42 +- 0.05 |

Currents in uA; errors on the computed-field currents are 0.02 to 0.05 uA.
The conductance is the same as with an imposed field; only the even part
of the curve changed.

Fraction of the bias along the axis (`P1_d*_V*.txt`, averaged over
|y| < 26 nm):

| gate, bias | within 15 nm of the source contact | lead and taper, to x = 350 nm | neck region, 350 to 531 nm | within 15 nm of the drain contact |
|---|---|---|---|---|
| 300 nm, +0.1 V | 0.076 | 0.137 | 0.620 | 0.167 |
| 300 nm, -0.1 V | 0.062 | 0.149 | 0.609 | 0.180 |
| 30 nm, +0.1 V | 0.090 | 0.129 | 0.604 | 0.178 |
| 30 nm, -0.1 V | 0.075 | 0.143 | 0.590 | 0.192 |

The neck itself sits at the midpoint of the bias to within 0.015. The drop
at the source plane is the size expected for a ballistic contact: half of
I / G_lead with the Sharvin conductance of the 500 nm lead (5.18 mS) is
0.083 of the bias. The drain drop is twice that; I have not checked why
(the beam leaving the neck is not isotropic there). Both drops sit in
three grid cells, so their width is set by the grid, not resolved. At
1 THz the in-phase potential has the same form (0.09 to 0.11 at each
contact, 0.20 along the taper, 0.58 across the neck region), with a
quadrature part of up to 0.20 (300 nm gate) and 0.24 (30 nm) that peaks
just before the neck (panel d).

## Frequency dependence, shape-only drive (P2, P4)

I_dc in uA, P_abs in uW, R in A/W. The 1 THz rows pool all runs with
4 million particles or more; the others are single runs.

| f (THz) | gate 300 nm: I_dc | P_abs | R_abs | R_1 | gate 30 nm: I_dc | P_abs | R_abs | R_1 |
|---|---|---|---|---|---|---|---|---|
| 0.1 | 1.041 +- 0.011 | 4.30 | 0.242 | 0.055 | 1.023 +- 0.008 | 4.30 | 0.238 | 0.054 |
| 0.3 | 0.937 +- 0.020 | 4.27 | 0.220 | 0.050 | 0.890 +- 0.014 | 4.29 | 0.208 | 0.047 |
| 0.6 | 0.708 +- 0.014 | 4.04 | 0.175 | 0.038 | 0.660 +- 0.013 | 4.14 | 0.159 | 0.035 |
| 1 | 0.429 +- 0.012 | 3.44 | 0.124 | 0.023 | 0.483 +- 0.013 | 3.64 | 0.132 | 0.026 |
| 1.8 | 0.149 +- 0.023 | 1.88 | 0.079 | 0.008 | 0.347 +- 0.022 | 2.15 | 0.161 | 0.020 |
| 3 | 0.187 +- 0.023 | 0.77 | 0.243 | 0.011 | 0.497 +- 0.030 | 2.28 | 0.218 | 0.027 |
| 6 | -0.007 +- 0.027 | 0.69 | -0.009 | -0.000 | 0.148 +- 0.055 | 10.72 | 0.014 | 0.006 |

R_abs rises again at 3 THz under the 300 nm gate only because the device
absorbs little there (Re Y_1 = 0.15 mS against Im Y_1 = 0.44 mS): the
current is a fifth of its low-frequency value and R_1 is 0.011 A/W. Per
absorbed watt is not a usable figure at that point without a matching
network for a 6.5 kilohm device.

For comparison, B7 at the same device and mean free path gave 9.29, 7.27,
3.03 and 0.16 uA at 0.1, 0.3, 1 and 3 THz with a uniform field and 2.41,
2.11, 2.78 and 3.35 uA with the drop at the neck
(`bpic/tables_B8.md`, section 9).

## Drain tied to the gate, the control and the strip (P2, P3)

I_dc in uA with R_abs in brackets.

| gate | f (THz) | funnel, shape-only drive | strip, drain tied to gate | funnel, drain tied to gate | control |
|---|---|---|---|---|---|
| 300 nm | 0.3 | +0.94 +- 0.02 (0.220) | +0.13 +- 0.02 (0.010) | +1.01 +- 0.02 (0.236) | -0.00 +- 0.02 |
| 300 nm | 1 | +0.43 +- 0.01 (0.124) | +0.34 +- 0.03 (0.060) | +0.49 +- 0.02 (0.136) | +0.06 +- 0.03 |
| 300 nm | 3 | +0.19 +- 0.02 (0.243) | +0.01 +- 0.04 (0.012) | +0.11 +- 0.02 (0.080) | +0.03 +- 0.03 |
| 30 nm | 0.3 | +0.89 +- 0.01 (0.208) | +0.67 +- 0.02 (0.052) | +1.25 +- 0.01 (0.285) | -0.05 +- 0.03 |
| 30 nm | 1 | +0.48 +- 0.01 (0.132) | +1.15 +- 0.04 (0.200) | +1.01 +- 0.02 (0.214) | +0.04 +- 0.02 |
| 30 nm | 3 | +0.50 +- 0.03 (0.218) | -1.79 +- 0.02 (-0.121) | +0.81 +- 0.03 (0.049) | -0.09 +- 0.05 |

The strip is 500 nm wide over its whole length, so it conducts three
times better than the funnel (Re Y_1 of 2.57 mS at 0.3 THz against
0.86 mS) and absorbs more for the same voltage; the comparison per
absorbed watt is the one in brackets. The last row's strip and drain-tied
funnel values are the repeats with a 10 ps settling time. The two terms do not add: under the
30 nm gate at 1 THz the funnel with both (1.01 uA) is below the sum
(1.63 uA) and below the strip alone.

## Plasma resonances

Absorbed power in uW at 0.1 V drive:

| device, drive, gate | 0.1 | 0.3 | 0.6 | 1 | 1.8 | 3 | 6 THz |
|---|---|---|---|---|---|---|---|
| funnel, shape-only, 300 nm | 4.30 | 4.27 | 4.04 | 3.44 | 1.88 | 0.77 | 0.69 |
| funnel, shape-only, 30 nm | 4.30 | 4.29 | 4.14 | 3.64 | 2.15 | 2.28 | 10.72 |
| funnel, drain tied to gate, 300 nm | | 4.28 | | 3.57 | | 1.39 | |
| funnel, drain tied to gate, 30 nm | | 4.41 | | 4.74 | | 16.72 | |
| strip, drain tied to gate, 300 nm | | 12.84 | | 5.65 | | 0.94 | |
| strip, drain tied to gate, 30 nm | | 12.83 | | 5.76 | | 14.85 | |
| control, shape-only, 30 nm | | 4.06 | | 2.71 | | 6.78 | |

Under the 30 nm gate the absorption peaks at or near 3 THz when the drain
is tied to the gate and at or above 6 THz with the shape-only drive. That
fits the plan's plasma speed (3.9 v_F) with both contacts holding the
potential: a half-wave mode at 3.6 THz and a full-wave mode at 7.1 THz.
In a symmetric channel the shape-only drive is antisymmetric about the
middle and cannot excite the half-wave mode; tying the drain to the gate
adds a common part that can. This reading rests on three and seven
frequencies and is not a fit. Under the 300 nm gate the absorption only
falls over this range.

The two runs whose source-plane and drain-plane currents differed after a
3 ps settling time (by 0.19 and 0.12 uA, against 0.03 uA or less in every
other driven run) were repeated with 10 ps:

| run (3 THz, 30 nm gate, drain tied to gate) | settle | I_dc (uA) | planes differ by (uA) | P_abs (uW) | E_kin, first to last period (eV) |
|---|---|---|---|---|---|
| strip | 3 ps | -1.770 +- 0.026 | 0.186 | 14.85 | 0.11714 to 0.11746 |
| strip | 10 ps | -1.794 +- 0.024 | 0.0005 | 14.86 | steady at 0.11747 |
| funnel | 3 ps | +0.780 +- 0.033 | 0.119 | 16.72 | 0.12215 to 0.12220 |
| funnel | 10 ps | +0.812 +- 0.026 | 0.0015 | 16.72 | steady at 0.12223 |

The disagreement was the device still charging as it warmed; the currents
are unchanged within their errors, and the sign reversal of the strip is
a property of the model. I have not looked for its cause. At a resonance a
3 ps settling time is too short.

## Extension: mean free path, walls and amplitude (P5, P6)

Figure `bpic/B8_extension.png`; tables 10 and 11 of
`bpic/tables_B8.md`. Shape-only bias and drive, 1 THz, 0.1 V unless
stated. A is I(+0.1 V) / |I(-0.1 V)|; G the odd part over 0.1 V; "dc
limit" the quasi-static rectified current from the static pair. The
700 nm row is the pooled decision point; the half-diffuse row pools two
seeds.

| case | gate | A | G (mS) | dc limit (uA) | I_dc at 1 THz (uA) | 1 THz / dc | P_abs (uW) | R_abs (A/W) | R_1 (A/W) |
|---|---|---|---|---|---|---|---|---|---|
| no scattering | 300 nm | 1.092 | 0.952 | 2.10 +- 0.02 | 0.499 +- 0.018 | 0.24 | 3.84 | 0.130 +- 0.005 | 0.027 |
| no scattering | 30 nm | 1.100 | 0.947 | 2.25 +- 0.01 | 0.591 +- 0.017 | 0.26 | 4.07 | 0.145 +- 0.004 | 0.032 |
| 700 nm | 300 nm | 1.053 | 0.845 | 1.10 +- 0.01 | 0.429 +- 0.012 | 0.39 | 3.44 | 0.124 +- 0.003 | 0.023 |
| 700 nm | 30 nm | 1.053 | 0.843 | 1.08 +- 0.01 | 0.483 +- 0.013 | 0.45 | 3.64 | 0.132 +- 0.003 | 0.026 |
| 100 nm | 300 nm | 1.021 | 0.442 | 0.23 +- 0.01 | 0.221 +- 0.015 | 0.96 | 2.06 | 0.107 +- 0.007 | 0.0125 |
| 100 nm | 30 nm | 1.020 | 0.442 | 0.22 +- 0.01 | 0.221 +- 0.015 | 1.02 | 2.22 | 0.099 +- 0.007 | 0.0124 |
| 700 nm, half-diffuse walls | 300 nm | 1.005 | 0.819 | +0.11 +- 0.01 | -0.073 +- 0.013 | | 3.25 | -0.022 +- 0.004 | -0.004 |
| 700 nm, half-diffuse walls | 30 nm | 1.004 | 0.818 | +0.08 +- 0.01 | -0.134 +- 0.011 | | 3.42 | -0.039 +- 0.003 | -0.007 |

Errors are statistical; the floor of 0.05 uA is 0.013 to 0.024 A/W in
R_abs here. The same cases with the imposed fields of B7 (uniform, then
drop at the neck): A 1.83 and 1.41 with no scattering, 1.56 and 1.13 at
700 nm, 1.28 and 0.81 at 100 nm, 1.43 and 0.90 with half-diffuse walls;
per watt absorbed at the static conductance 0.96 and 0.87, 0.71 and
0.74, 0.22 and 0.43, 0.42 and 0.14 A/W. The computed static asymmetry
lies below both imposed profiles in the two specular cases with little
scattering, and between them at 100 nm and with half-diffuse walls,
where the neck-drop profile had reversed the sign.

**Mean free path.** The conductance is that of B7 (0.95, 0.85,
0.44 mS). The dc limit falls by a factor of nine to ten from no
scattering to 100 nm; the 1 THz current by 2.3 to 2.7; the current per
absorbed watt by 1.2 to 1.5. The part of the static asymmetry that a long mean free path
adds is therefore not available at 1 THz. A reading I have not tested:
it comes from electrons that cross the taper after several wall
reflections, which take longer than a period. At 100 nm the 1 THz
current equals the dc limit within the errors (7 %), so a shorter mean free path
moves the roll-off above 1 THz while it lowers the plateau.

**Walls.** Half-diffuse walls lower the conductance by 3 % and remove
the static asymmetry (1.005, 1.004). The single runs at 1 THz:

| device | gate | seed | I_dc (uA) |
|---|---|---|---|
| funnel | 300 nm | 5 | -0.093 +- 0.022 |
| funnel | 300 nm | 11 | -0.066 +- 0.013 |
| funnel | 30 nm | 5 | -0.125 +- 0.015 |
| funnel | 30 nm | 11 | -0.143 +- 0.016 |
| mirror-symmetric control | 300 nm | 1 | +0.018 +- 0.017 |
| mirror-symmetric control | 30 nm | 1 | -0.052 +- 0.025 |

The negative sign is reproduced under both gates. Under the 30 nm gate
it is 2.7 times the floor; under the 300 nm gate 1.5 times, so there I
would call it zero or negative. The dc limit is positive (+0.1 uA), so
the sign changes between dc and 1 THz. Only a specularity of 0.5 was
run: where between 0.5 and 1 the response is lost is not known.

**Amplitude** (700 nm, specular walls):

| gate | V_a (V) | I_dc (uA) | relative to the square law through 0.1 V | P_abs (uW) | R_abs (A/W) | R_1 (A/W) |
|---|---|---|---|---|---|---|
| 300 nm | 0.05 | 0.099 +- 0.018 | 0.92 +- 0.17 | 0.87 | 0.113 +- 0.020 | 0.021 |
| 300 nm | 0.1 | 0.429 +- 0.012 | 1 | 3.44 | 0.124 +- 0.003 | 0.023 |
| 300 nm | 0.2 | 2.564 +- 0.016 | 1.50 +- 0.04 | 12.83 | 0.200 +- 0.001 | 0.035 |
| 30 nm | 0.05 | 0.099 +- 0.015 | 0.82 +- 0.13 | 0.93 | 0.107 +- 0.017 | 0.021 |
| 30 nm | 0.1 | 0.483 +- 0.013 | 1 | 3.64 | 0.132 +- 0.003 | 0.026 |
| 30 nm | 0.2 | 2.659 +- 0.019 | 1.38 +- 0.04 | 13.61 | 0.195 +- 0.001 | 0.036 |

The absorbed power follows V_a^2 within 7 % (the admittance changes
little), so the rise at 0.2 V is in the rectified current. 0.2 V is
above the Fermi energy over e (0.138 V); the mean kinetic energy rises
to 0.1215 and 0.1225 eV there. B7 at 0.2 V: 0.80 of the square law
(uniform field) and 0.30 (drop at the neck). The small-signal
responsivity is 0.11 to 0.13 A/W; I have not looked for the cause of
the rise.

## Limits

Those of the plan's section 8, unchanged: a uniform dielectric; ideal
equipotential contacts without resistance or doping of the graphene
beneath; a voltage source for the antenna, with 75 ohm entering afterwards
through the computed admittance; classical trajectories in a classical
field, with five to seven modes in the neck; electron-electron collisions
only through the common field; no lattice heating; one shape. Added by
this stage:

- A quarter of the bias drops at the contact planes in three grid cells.
  The contact length and the block size did not move the decision current
  at the checkpoint, but a contact model with doping under the metal
  could change that fraction, and the rectified current goes roughly as
  the square of what is left at the neck.
- Two wall specularities only, 1 and 0.5, and the response is lost
  between them. The other mean free paths and the walls were run at
  1 THz and dc only, with the shape-only drive.
- The mean kinetic energy rises from 0.1112 to 0.1137 eV at the decision
  point (steady, consistent with the absorbed power times the residence
  time), and to 0.122 eV in the resonant run at 3 THz and at 0.2 V,
  where the drive is not a small signal.
- Systematic floor of about 0.05 uA on any rectified current (the control
  scatter and the numerical variations), 0.015 A/W at this drive.

## Cost

40 production runs in 3.6 hours of wall time on ten cores, 2 to 12
minutes each; two repeats in a quarter of an hour. At most two sample
particles in 8 million were lost through a wall in any run. The
extension: 26 runs (P5, P6 and the four checks) in 2.0 hours, 2 to
7 minutes each; the two current estimators agree within 0.005 uA in the
driven runs and 0.011 uA in the static ones. P7 (the 6 um Device D) has not been run.
