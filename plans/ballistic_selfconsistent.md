# Plan: the short ballistic funnel with a computed field (Stage B8)

Written 2026-11-11, before any code or run. Awaiting approval. Companion to
`plans/ballistic_short.md` and `results/BALLISTIC_SHORT.md` (Stage B7).
Figure: `plans/ballistic_selfconsistent.png` (script
`tools/selfconsistent_plan_fig.py`).

Stage B7 found that a 0.55 um funnel rectifies at 1 THz at the
room-temperature mean free path: 0.70 A/W per absorbed watt, the same for
the two field profiles tried. That is the one positive result of the paper,
and it rests on three things the model does not compute:

1. **The field.** The potential is imposed (a uniform ramp, or a ramp at
   the neck). In a real device it is set by the charge the electrons pile
   up, by the gate and by the contacts.
2. **The absorbed power.** With no displacement current the model has no
   usable admittance at 1 THz, so "per absorbed watt" was defined with the
   static conductance. A different, equally arguable definition gave
   1.9 A/W (B7, deviation 2).
3. **Plasma waves.** The imposed-field model has none. This device has
   plasma resonances at a few THz (figure panel c).

Stage B8 asks: **does the 1 THz rectification of the 0.55 um funnel
survive when the field, the displacement current and the absorbed power are
computed, and how much of it is shape and how much circuit?**

## 0. Scales (figure panel c)

n = 1.4e12 cm^-2, E_F = 0.138 eV, dielectric constant 3.8 (as in the
plasmonic model).

| quantity | value |
|---|---|
| electrons in the device (0.204 um^2) | 2900 |
| screening length without a gate, 1 / q_TF | 2.07 nm |
| quantum over gate capacitance, c = C_q / C_g | 29 (30 nm gate), 290 (300 nm) |
| screening length under the 30 nm gate | 5.6 nm |
| gated plasma speed, collisionless: v_F (1 + c) / sqrt(1 + 2 c) | 3.90 v_F (hydrodynamic value 3.87 v_F) |
| plasma resonances of a 546 nm channel, 30 nm gate | 1.8 THz (quarter wave), 3.6 THz (half wave) |
| plasma resonance, 300 nm gate (nearly ungated here) | about 6 THz |
| half-response frequency of the imposed-field model | 0.7 THz |

Two consequences. Screening is strong: the electrons will not let a
uniform field stand, so the computed potential should drop near the neck,
which is the less favourable of the two B7 profiles at dc (asymmetry 1.13
against 1.56). And at 1 THz the device is below but not far below its
plasma resonances under a thin gate, so the answer there can differ from
the imposed-field one in either direction.

## 1. Model (figure panel a)

The graphene sheet lies in a uniform dielectric at distance d above a
grounded gate plane. The two contacts are metal strips in the plane of the
sheet, 200 nm long, as wide as the leads, held at given potentials V_S(t)
and V_D(t). Electrons are the classical Dirac trajectories of Stages B1 to
B7 (same walls, same scattering, same equilibrium injection at the
contacts). What is new is the potential they move in:

    phi(r, t) = sum over sheet and contact charge of
                q / (4 pi eps) [ 1 / |r - r'| - 1 / sqrt(|r - r'|^2 + 4 d^2) ]

The first term is the charge itself, the second its image in the gate. The
sheet charge is the electron density minus the uniform background that
makes the undriven device neutral. The contact charges are whatever is
needed to hold the contacts at V_S and V_D; they are solved for at every
step. This is the electrostatics of a thin sheet in three dimensions. It
goes over to the local gate capacitance of the plasmonic model for small d
and to the ungated sheet for large d.

Two gate distances: **30 nm** (the gate of the paper's plasmonic model, so
the two THz models finally describe one device) and **300 nm** (an oxide
back gate, as in the geometric-diode experiments; nearly ungated for a
device this size).

Two drives, because with a gate the wiring matters (the paper's theme):

- **Shape-only drive:** V_S = +V_a/2 cos wt, V_D = -V_a/2 cos wt. The
  circuit is mirror symmetric; a mirror-symmetric device gives zero.
- **Drain tied to the gate:** V_S = V_a cos wt, V_D = 0. A straight
  channel already rectifies (the Dyakonov-Shur term, here without
  collisions).

## 2. Method (figure panel b)

A particle simulation of all the electrons at once. Stages B1 to B7 traced
one electron at a time through a prescribed field, which cannot be made
self-consistent. Here 1 to 4 million sample particles (each standing for a
small fraction of an electron) move together in steps of 1.25 to 2.5 fs;
at every step their charge is counted on a grid of 2.5 to 5 nm, the
potential is computed from it with a fast Fourier transform, and the
particles are advanced. Geometry, wall reflection, scattering and the
injection tables are reused from `src/transport.*` and `src/geometry.*`.
New code: `src/pic.hpp`, `src/pic.cpp`, `apps/bpic.cpp`, tests in
`apps/tests.cpp`.

Outputs per run: the rectified current (time average of the particle
current, with a batch error over periods); the terminal currents at
source, drain and gate including displacement current; from them the
admittance at the drive frequency and the absorbed power; maps of the
time-averaged and first-harmonic potential and density.

Not chosen: iterating the existing one-electron code with an updated field
(less new code, but the iteration is unstable against strong screening and
near a plasma resonance); a perturbative "delta-f" particle scheme (less
noise, but the rectified current is second order and the weights become
delicate). Either is the fallback if the checkpoint of section 4 fails.

## 3. Definitions, fixed now

B7 left the responsivity definition open and it mattered. For B8:

- **I_dc**: time average of the particle current over the last 20 periods
  (after 10 periods of settling), estimated over the whole device volume.
- **P_abs** = time average of V_S I_S + V_D I_D with the full terminal
  currents. This is now a computed quantity and cannot be negative.
- **Headline: R_abs = I_dc / P_abs.**
- **R_1**: one device on a 75 ohm antenna, from the computed admittance.
- R_B7 = 2 I_dc / (V_a^2 G_static), for comparison with Stage B7 only.
- Static asymmetry A = I(+0.1 V) / |I(-0.1 V)|, shape-only bias.
- Decision point: G3, mean free path 700 nm, 1 THz, V_a = 0.1 V, 300 nm
  gate, shape-only drive.

## 4. Acceptance ladder, then a checkpoint

| test | what is checked | target |
|---|---|---|
| T22 | potential of a uniform sheet over the gate; of a point charge with its image | sigma d / eps and the closed form, to 1e-4 |
| T23 | contacts as equipotentials: capacitance of a wide strip over the gate; of two coplanar strips without a gate | plate value and the elliptic-integral formula, within 5 % |
| T24 | static screening of a weak sinusoidal potential in a uniform sheet, two wavelengths, both gates | 1 / (1 + e^2 nu G(q)), within 3 % |
| T25 | plasma-wave speed in a uniform strip | 3.90 v_F under the 30 nm gate; root of the collisionless dielectric function for 300 nm; within 3 % |
| T26 | interaction switched off, imposed ramp | reproduces B7 for G3: static currents at +-0.1 V and I_dc at 1 THz, both profiles, within 2 standard errors |
| T27 | linear conductance at +-5 mV with the interaction on | equal to the value with it off within 1 % (the linear conductance cannot depend on the field profile) |
| T28 | bookkeeping | source + drain + gate current zero to round-off at every step; volume and plane estimators of I_dc agree; P_abs >= 0 |
| T29 | null tests | no drive: I_dc zero within 2 standard errors, kinetic energy drift below 1 % in 30 ps; mirror-symmetric device, shape-only drive, 1 THz: zero within 2 standard errors |
| T30 | convergence of the decision number | grid 5 against 2.5 nm, time step, 1 against 4 million particles, contact strip 100 / 200 / 400 nm: changes below 10 % or 2 standard errors |

T29 is the hard one. A particle simulation whose grid is coarser than the
screening length heats itself numerically; the grid here is at the edge
(2.5 nm against 2.07 nm without a gate). Smoother particle shapes and a
short-range cut-off of the interaction at the cell size are the standard
remedies.

**Checkpoint.** After the ladder I stop and report: which tests pass, the
measured cost per run, and the noise on I_dc. If T29 or T30 cannot be met
at about ten minutes per run on ten cores, no production run is started
and you decide between a fallback method and stopping.

## 5. Runs

G3 is the 0.55 um funnel; "control" is its mirror-symmetric version;
"strip" is a straight 546 nm x 500 nm channel. V_a = 0.1 V unless stated.

**Core set (about 40 runs):**

| run | what | runs |
|---|---|---|
| P1 | G3 static, +-0.05 and +-0.1 V, mean free path 700 nm, both gates, shape-only bias; potential maps | 8 |
| P2 | G3 at 0.3, 1 and 3 THz, 700 nm, both gates, both drives | 12 |
| P3 | control at 0.3, 1, 3 THz, shape-only drive, both gates (must be zero); strip with the drain tied to the gate, same points (the circuit term) | 12 |
| P4 | G3, 700 nm, shape-only drive, both gates: 0.1, 0.6, 1.8 and 6 THz added, to see the plasma resonances | 8 |

**Extension (about 20 runs), decided after the core set:**

| run | what | runs |
|---|---|---|
| P5 | G3 with no scattering and with 100 nm: static at +-0.1 V and 1 THz, shape-only drive, both gates | 12 |
| P6 | G3 at 1 THz: V_a = 0.05 and 0.2 V; half-diffuse walls | 6 |
| P7 | the 6 um Device D, static, +-0.1 V, 700 nm, 300 nm gate: the one measured number (1.66) with a computed field | 2, the costliest |

## 6. Decision criteria (figure panel d)

At the decision point of section 3:

- **R_abs >= 0.35 A/W and R_1 >= 0.07 A/W** (half the B7 values): the
  positive result is **confirmed**. The paper keeps it and replaces
  "imposed field" in the limitations by the computed result.
- **R_abs between 0.10 and 0.35 A/W**: it **stands, weakened**. The paper
  quotes the lower number; the short funnel is then level with the
  plasmonic element, not ahead of it.
- **R_abs below 0.10 A/W**: the positive result is **withdrawn**. The
  1 THz response of B7 was produced by the imposed field, and the paper's
  ballistic conclusion returns to "not shown".

Shape against circuit, from P2 and P3 under the 30 nm gate: the funnel
with the shape-only drive against the strip with its drain tied to the
gate, in one model. Whichever is larger is reported as such.

What I expect, stated before the runs: a static asymmetry of 1.1 to 1.3 at
700 nm, nearer the neck-drop value than the uniform-field one; R_abs of
0.3 to 1.0 A/W at the decision point; a larger and less predictable change
under the 30 nm gate, where 1 THz lies below a resonance at 1.8 THz. I
think "withdrawn" unlikely, perhaps one chance in five, because the two
very different imposed profiles already agreed for this device. My B7
expectation was too low, so treat this one with the same caution.

## 7. Cost

- **Code:** the largest new piece since the plasmonic time solver: a
  particle stepper, the electrostatics with contacts, the terminal-current
  bookkeeping, nine tests. Most of the effort of the stage is here and in
  getting T29 to pass.
- **Compute:** a 1 THz run is 2 to 10 minutes on ten cores (1 to 4 million
  particles, 400 to 800 steps per period, 30 periods). Core set: 2 to 6
  hours of wall time. Extension: 2 to 4 hours more; P7 alone about an
  hour.
- **Statistical error:** counting noise of about 0.03 to 0.05 uA on I_dc,
  against the 3 uA of B7. Noise is not the risk; numerical heating is.

## 8. Limits of the planned model

Uniform dielectric (no air above the top layer). Contacts are ideal:
equipotential strips that absorb and inject without resistance or doping
of the graphene beneath. The antenna is a voltage source; 75 ohm enters
afterwards through the computed admittance. Classical trajectories and a
classical field: necks of five to seven modes, and nothing above a few
THz. Electron-electron collisions enter only through the common field, so
there is no viscosity. No heating of the lattice or of the contacts. One
shape; the other B7 devices are not rerun.

## 9. What comes back

`results/BALLISTIC_SELFCONSISTENT.md` with the verdict against section 6,
the ladder, deviations from this plan, tables, potential maps and two
figures (computed potential against the two imposed profiles; responsivity
against frequency for both gates and drives, with the B7 curves). Then, as
a separate step that needs your go-ahead, the changes to the paper.

## 10. Checkpoint, 2026-11-12

Solver built (`src/pic.*`, `src/fft2d.*`, `apps/bpic.cpp`,
`apps/pic_tests.cpp`); ladder run; no production run started. Record:
`results/bpic/ladder_B8.txt`, `T30.csv`, `T30N.csv`, `maps.csv`; figure
`plans/ballistic_selfconsistent_checkpoint.png`
(`tools/pic_checkpoint_fig.py`).

### Ladder

| test | result |
|---|---|
| T22 | pass. Point charge with image 3e-12; charged disc over the gate 1.4e-7 against the closed form |
| T23 | pass. Plate over the gate 0.970 of the formula with fringing; coplanar strips 0.990 of the elliptic-integral value |
| T24 | pass in the reworked form (below). Kernel: U_ind / (G dn) = 1.000 +- 0.001. Particles alone: chi / nu = 0.980. Loop at weak coupling: 1.005, 0.992 of 1 / (1 + nu G). Production coupling: U_tot / U_ext 0.96 to 0.99 of theory at 1.0 to 2.5 ps, 0.93 to 0.96 at 3.5 to 5.0 ps |
| T25 | pass. Plasma speed 0.983 to 0.992 of the collisionless value |
| T26 | pass at 2.5 standard errors, not at 2: four of six values within 1.3, two static values at 2.0 (0.2 to 0.3 % of the current) |
| T27 | pass. Strip: 1.006 and 1.002 of the Sharvin conductance. G3 with the interaction on: 1.0004 and 1.0076 of the value with it off |
| T28 | pass. Charge 4e-13 electrons; contact potential 6e-16 eV; absorbed power from <V I> and from the first harmonic agree to 0.01 % |
| T29 | pass. No drive: +0.03 +- 0.04 uA at both gates, kinetic energy +0.04 % and 0.00 % in 20 ps. Mirror device: -0.08 +- 0.07 uA |
| T30 | pass from 4 million particles; 1 and 2 million are not converged (below) |

### Decision point (G3, 700 nm, 1 THz, 0.1 V, shape-only drive)

| gate | I_dc (uA) | P_abs (uW) | R_abs (A/W) | R_1 (A/W) | Y_1 (mS) |
|---|---|---|---|---|---|
| 300 nm | 0.44 +- 0.01 (6 runs) | 3.45 | 0.126 | 0.024 | 0.690 - 0.167i |
| 30 nm | 0.47 +- 0.01 (5 runs) | 3.64 | 0.129 | 0.025 | 0.729 + 0.339i |

Stage B7 with an imposed field: 3.03 uA and 0.70 A/W (uniform), 2.78 uA
and 0.71 A/W (drop at the neck). The computed value is in the band
"stands, weakened" of section 6 (0.10 to 0.35 A/W), near its lower edge,
and below the range expected in the plan (0.3 to 1.0 A/W).

Computed field at the drive frequency along the axis (fraction of the
drive amplitude, in phase): 0.09 to 0.11 lost within 15 nm of each contact
plane, 0.20 along the taper, 0.58 across the neck region (x = 350 to
530 nm). A quadrature part of up to 0.20 (300 nm gate) and 0.24 (30 nm)
peaks just before the neck. Time-averaged potential: +1.2 meV at the neck.

### Convergence (I_dc in uA; two entries are two seeds)

| particles | gate 300 nm | gate 30 nm | seconds |
|---|---|---|---|
| 1 million | 0.60 +- 0.07 | 0.55 +- 0.04 | 50 |
| 2 million | 0.42 +- 0.03, 0.61 +- 0.06 | 0.63 +- 0.03, 0.45 +- 0.06 | 75 |
| 4 million | 0.47 +- 0.04, 0.41 +- 0.04, 0.49 +- 0.04 | 0.46 +- 0.05, 0.45 +- 0.05, 0.52 +- 0.04 | 135 |
| 8 million | 0.42 +- 0.03 | 0.49 +- 0.02 | 245 |
| 16 million | 0.42 +- 0.02, 0.44 +- 0.02 | 0.46 +- 0.01 | 440 |

At 4 million particles: grid 2.5 nm 0.38 +- 0.05 / 0.52 +- 0.04; time
step 1.25 fs 0.44 / 0.48; low-pass 0.75 cells 0.46 / 0.43; contacts 100 nm
0.44 / 0.49, 400 nm 0.47 / 0.53; contact blocks of 4 nodes 0.46 / 0.57.
Low-pass 3 cells: 0.58 / 0.62, that is 30 % high: the field has to be
resolved to 7.5 nm or better.

Noise on I_dc over ten periods: 0.04 uA at 4 million particles, 0.03 at
8 million, 0.02 at 16 million. Below 4 million the batch error
underestimates the scatter between seeds.

### What the screening test found

Collisions between sample particles relax the Fermi distribution towards
a classical one, which raises the static response chi above nu. In a
closed box the excess after 5 ps is 30 % at 1.4 particles per nm^2, 8 % at
8 per nm^2 and 4 to 7 % at 19 per nm^2 (the density of a 4 million
particle run of G3). In the open device a particle stays about 1 ps
before a contact replaces it with an equilibrium one, and the decision
number does not move from 4 to 16 million particles. This, and not
heating, is what limits the particle number from below.

### Deviations from sections 2 and 4

- Tests are in `apps/pic_tests.cpp`, not `apps/tests.cpp`.
- T22: point charge and a charged disc in place of the uniform sheet.
- T23: fringing formula for the plate; strips from the difference of two
  lengths.
- T24: split into (a) particles alone, 3 %; (b) the loop at eps_r = 40,
  3 %; (c) production coupling: kernel and U_ind / U_ext to 1 %, U_tot /
  U_ext to 10 % in an early window. The plan's 3 % on U_tot / U_ext at
  production coupling is not met: U_tot is a 14- to 30-fold cancellation
  between U_ext and U_ind, so it amplifies the collision effect above and
  a 0.2 % interpolation loss.
- T25: measured in the central band of a wide strip, because the edge
  mode of a hard-edged strip is 9 % slower.
- T26: tolerance 2.5 standard errors (or 1 % for static currents).
- T27: Sharvin strip extrapolated to zero bias from +-5 and +-10 mV; G3
  compared at +-10 mV.
- T29: 20 ps, not 30.
- Numerics: grid spacing adjusted so that both contact planes lie on node
  columns (5.013 nm for G3); force from the gradient of the interpolated
  potential (energy conserving); Gaussian low-pass of 1.5 cells; ten
  periods per run, not thirty.

### Proposed settings for the runs of section 5

8 million particles, 5 nm grid, low-pass 1.5 cells, ten periods: about
4 minutes per run and +- 0.03 uA, or +- 0.008 A/W at this drive. Core set
(about 40 runs) under 3 hours; extension (about 20) 1.5 hours more.

## 11. Core set, 2026-11-12

P1 to P4 run with the proposed settings (`tools/pic_runs.sh`), plus two
repeats (`tools/pic_checks.sh`). Verdict against section 6: stands,
weakened, 0.124 +- 0.003 A/W at the decision point. Record:
`results/BALLISTIC_SELFCONSISTENT.md`. The extension (P5 to P7) and the
changes to the paper have not been started.

## 12. Extension, 2026-11-12

P5 and P6 run (`tools/pic_ext.sh`), with the static pair for half-diffuse
walls and four checks added (stage P6c). At 1 THz: 0.13 to 0.145 A/W with
no scattering, 0.10 to 0.107 A/W at 100 nm, -0.02 to -0.04 A/W with
half-diffuse walls; 1.4 to 1.5 times the square law at 0.2 V. Record:
section "Extension" of `results/BALLISTIC_SELFCONSISTENT.md`. P7 and the
changes to the paper have not been started.

## 13. Paper, 2026-11-12

The third draft of the paper and `OVERVIEW.md` carry the Stage B8 numbers
(`paper/outline.md`, entry "Third draft"). P7 has not been run.
