# Plan: short funnels and the length scaling of the ballistic THz response (Stage B7)

Written 2026-11-10, before any code or run. Awaiting approval. Companion to
`plans/ballistic_thz.md` and `results/BALLISTIC_THZ.md` (Stages B2 to B6) and
to the prior-work record `paper/prior_work.md`, section 1.2. Figure:
`plans/ballistic_short.png` (script `tools/short_funnel_plan_fig.py`).

Stage B2 concluded that the ballistic funnel is "not a THz rectifier": with
a uniform field the rectified current of the 6 um Device D funnel loses a
third of its value between 1 and 10 GHz and is zero at 300 GHz. The
prior-work search found the opposite statement in the literature for
shorter devices: a time-dependent Monte Carlo with rectification into the
THz range (Stearns and Moddel 2020), a measured response at 28 THz in a
diode about 500 nm long (Zhu et al. 2013), and ballistic graphene
rectifiers reported at 0.68 and about 3 THz (2026). Stage B7 asks: **is
the B2 roll-off a property of the funnel or of its length, and does a
funnel short enough to be compared with those devices rectify at 1 THz at
the room-temperature mean free path?**

## 0. What the existing data already say: the model is scale free

The Stage B2 model has no intrinsic length. An electron moves at v_F, its
momentum changes by the force of an imposed potential, and the walls
reflect it. Multiply every length by s and every time by s: the force
falls by 1/s, acts for s times as long, and gives the same momentum change.
The injected energy distribution does not change. So in the clean limit

    S_B(f; device scaled by s)  =  S_B(s f; original device)

exactly, and the static asymmetry A is independent of s. The same holds at
finite mean free path if lambda scales with the device, and for the
confined field if the drop length scales with the device. The response is
a function of four pure numbers: the shape, f L / v_F, lambda / L and
l_F / L (plus e V_a / E_F and k T / E_F, which B2 held fixed).

Three consequences need no new run (figure panel b, the B2 clean-limit
data on the axis f L / v_F):

1. **The clean-limit curves of B2 are already the curves of every Device D
   funnel of any length.** For a 0.55 um copy the uniform-field response
   is 0.65 of the quasi-static value at 0.33 THz, 0.41 at 1.1 THz and zero
   at 3.3 THz; with the drop confined to one twelfth of the length it is
   0.54 at 1.1 THz, 0.64 at 3.3 THz and 0.38 at 11 THz. The gigahertz step
   of the slow, taper-scattered electrons moves to 10 to 100 GHz.
2. **"Not a THz rectifier" is a statement about 6 um.** In its own model
   the B2 result predicts THz rectification for sub-micrometre funnels,
   which is what the literature reports. The B2 conclusion has to be
   restated as a length criterion whatever B7 finds.
3. **Current and conductance scale with the width, their ratio does not.**
   The rectified current per absorbed watt, I_2 / G, is scale free. A
   single short device is far from 75 ohm (a 0.5 um wide funnel is about
   2 kohm), so its responsivity against available power is low; a
   parallel array restores the match. Both figures will be reported.

What scaling cannot answer, and what the runs are for:

- **The mean free path does not scale.** At 700 nm (hBN, 300 K) lambda / L
  is 0.12 for the 6 um device and 1.3 for a 0.55 um one. The 6 um device
  lost almost all its asymmetry to scattering (A = 1.10 against 1.61); the
  short one should keep most of it. This is the main unknown.
- **The neck cannot be scaled.** Device D at one eleventh scale has a
  16 nm neck, half a Fermi wavelength (30 nm at 1.4e12 cm^-2): not a
  classical device and not a device anyone makes. A real short funnel has
  a neck of 75 to 100 nm and a shoulder of 400 to 500 nm, a width ratio of
  5 where Device D has 11. Its asymmetry is not known from B2.
- **The leads.** The slow electrons live in the source lead and the taper.
  A rectenna contacts the diode close to the neck; the lead length is a
  design choice, and its effect on the gigahertz step is not known.
- **The top of the frequency range.** B2 stops at f L / v_F = 6. The
  28 THz measurement on a 0.5 um diode sits at 14.

## 1. Model and code

No change to the physics of B2: classical trajectories at v_F, specular
walls, imposed time-dependent potential (uniform, or a drop confined at
the neck), two-sided equilibrium injection, 300 K smearing, n = 1.4e12
cm^-2, V_a = 0.1 V. Code changes are bookkeeping:

- `apps/bthz.cpp`: funnel dimensions from the command line (`--Wc --Lc
  --alpha --Wn --Wd --Ld`), a `--scale s` shorthand, and a matching
  mirror-symmetric constriction as the control for each shape.
- `apps/tests.cpp`, four tests:

| test | what it checks | criterion |
|---|---|---|
| T18 | scale invariance, clean: G1 (half scale) at 2 f against G0 at f, at 0, 10 and 300 GHz (G0 frequencies); static A equal | 3 sigma |
| T19 | scale invariance with scattering: G1 with lambda = 350 nm at 2 f against G0 with 700 nm at f | 3 sigma |
| T20 | each new shape's symmetric control gives zero dc current at 0.1, 1 and 10 THz | 3 sigma of zero |
| T21 | chord cap at the highest reduced frequency (G3, 30 THz): v_F / (40 f) against v_F / (160 f) | 3 sigma and 3 % |

T18 and T19 are tests of the code against an exact property of the model;
if they fail, something in the solver has a hidden length (the chord cap,
the Dirac cut, the 50 nm geometry tolerance) and the stage stops there.

## 2. Devices (figure panel a)

| | shape | shoulder / neck (nm) | leads (nm) | L (um) | neck in modes | purpose |
|---|---|---|---|---|---|---|
| G0 | Device D | 2000 / 175, 30 deg | 2000, 2420 | 6.00 | 12 | done; extended to f L / v_F = 30 |
| G1 | Device D at half scale | 1000 / 88, 30 deg | 1000, 1210 | 3.00 | 6 | code check; smallest valid copy |
| G2 | Device D, short leads | 2000 / 175, 30 deg | 250, 250 | 2.08 | 12 | role of the leads |
| G3 | short funnel | 500 / 100, 30 deg | 100, 100 | 0.55 | 7 | **the decision device** |
| G4 | Zhu-like | 400 / 75, 45 deg | 169, 169 | 0.50 | 5 | comparison with the 28 THz measurement |

G4 uses the neck and shoulder widths quoted second-hand for Zhu et al.
(2013); the taper angle and length are my assumptions, and at five modes
the neck is at the edge of a classical description. It is reported as a
sensitivity case, not as a simulation of that experiment.

## 3. Runs (figure panel c)

For each of G1 to G4: static curves at +-0.05 and +-0.1 V, then eight
frequencies (0.01, 0.03, 0.1, 0.3, 1, 3, 10, 30 THz), for three mean free
paths (100 nm, 700 nm, infinite) and two field profiles (uniform; drop
over one neck width at the neck).

| run | what | points | cost (CPU-h) |
|---|---|---|---|
| S1 | static curves, G1 to G4, 2 fields x 3 lambda x 4 voltages | 96 | 3 |
| S2 | frequency sweeps, G1 to G4, 2 fields x 3 lambda x 8 f | 192 | 9 |
| S3 | G0 clean limit extended to 2, 3 and 5 THz, 2 fields | 6 | 2 |
| S4 | symmetric controls, 4 shapes x 2 fields x 3 f | 24 | 1 |
| S5 | G3 only: half-diffuse walls (4 f, 2 fields); amplitude ladder at 1 THz (0.05 to 0.3 V) | 12 | 1 |

About 330 points and 16 CPU-hours: two hours of wall time on ten cores
(B2 to B6 together were 150 points and five CPU-hours; the new points cost
more because high reduced frequency means more chords per flight).
1e6 particles per side in the clean limit, 4e6 at finite lambda, as in B2.

The 100 nm rows do two jobs. They are the mean free path of graphene on
SiO2 and of CVD graphene, the material of two of the reported devices.
They also bound electron-electron scattering, whose length is about
100 nm at 300 K and which the model does not have: treating it as ordinary
momentum-relaxing scattering overstates its damage, so the truth for an
encapsulated device lies between the 100 nm and the 700 nm rows.

## 4. What is reported

For every point: I_dc and its error, the first-harmonic admittance, the
fate fractions and the heated fraction, as in B2. Derived:

- the static asymmetry A(0.1 V) and the quasi-static curvature I_2;
- S_B(f) = I_dc / (I_2 V_a^2 / 2), against f and against f L / v_F;
- the half-response frequency against L and against lambda, to test
  whether the cut-off follows the flight time v_F / L (our reading) or the
  scattering rate v_F / (2 pi lambda) (the reading attributed to Stearns
  and Moddel);
- responsivity per absorbed watt; responsivity against the available
  power of a 75 ohm antenna for one device and for a matched parallel
  array (with the number of devices); the Johnson-noise-limited
  noise-equivalent power of the matched array at 300 K.

## 5. Decision criteria, fixed now

The decision device is G3 at lambda = 700 nm; the figure of merit is the
responsivity of the matched array against available power at 1 THz, the
better of the two field profiles. The reference is the best plasmonic
figure of the paper, 0.34 A/W.

- **Closed for all lengths:** below 0.1 A/W. The paper's ballistic
  conclusion then holds for short devices as well, now with the reason
  (the smaller width ratio a valid neck allows), and the reported THz
  devices must work by something else.
- **Alive, no better than the circuit:** 0.1 to 1 A/W. The ballistic
  section is rewritten as a length criterion: geometric rectification
  reaches THz when the device is shorter than both v_F / f and the mean
  free path, at a responsivity comparable to the plasmonic one. The title
  and the abstract change.
- **Alive and better:** above 1 A/W. Shape beats circuit in the ballistic
  regime for short devices and the paper's thesis fails there. This would
  justify the self-consistent field calculation B2 declined, as a separate
  plan.

My expectation, stated before running: the middle outcome, 0.1 to
0.5 A/W. A Device D copy would give 0.2 to 0.4 A/W at 1 THz by scaling (the
B2 values at 100 GHz for the two field profiles), and G3 has less than half
the width ratio but nearly ten times the lambda / L.

The 100 nm rows, G4 and everything above 10 THz are reported but do not
enter the decision.

## 6. Limits

- **The field is imposed.** Where the potential drops in a 0.5 um device
  between metal contacts is not computed. Uniform and one-neck-width are
  the two ends of the bracket, and the bracket is narrower than at 6 um
  because the whole device is only five neck widths long.
- **Classical trajectories.** Valid for necks of several Fermi
  wavelengths: 7 modes in G3, 6 in G1, 5 in G4.
- **Semiclassical drive.** The model needs the photon energy well below
  the Fermi energy (138 meV, 33 THz). Results above 10 THz are indicative;
  at 28 THz (116 meV) photon-assisted processes the model lacks are of
  the same order as what it computes.
- No gate, no plasmons, no displacement current, no electron-electron
  scattering beyond the bound above, no heating. In a real short gated
  device the plasmonic and ballistic mechanisms act together; this stage
  computes one of them.

## 7. Deliverables

`apps/bthz.cpp` and `apps/tests.cpp` as above; `tools/short_funnel_runs.sh`,
`tools/short_funnel_plot.py`, `tools/short_funnel_tables.py`;
`results/BALLISTIC_SHORT.md` with the ladder, the runs and the verdict
against section 5; the scale-free replot of B2 added to
`results/BALLISTIC_THZ.md`. For the paper: a rewritten ballistic THz
section and a new figure (response against f L / v_F for all devices, and
responsivity at 1 THz against length), whichever outcome.

## 8. Out of scope

Self-consistent electrostatics; quantum transport through the neck; the
Tesla geometries (no ballistic asymmetry at dc, so none to scale); bias
other than zero; any fit to a specific published device.
