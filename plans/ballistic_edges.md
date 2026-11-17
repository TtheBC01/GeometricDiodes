# Plan: the short funnel with angle-dependent edges (Stage B9)

Written 2026-11-15, before any code or run. Approved the same day; section 10
records what the reading changed before the runs. Companion to
`plans/ballistic_selfconsistent.md` and
`results/BALLISTIC_SELFCONSISTENT.md` (Stage B8 and its specularity scan).
Figure: `plans/ballistic_edges.png` (script `tools/edges_plan_fig.py`).

Stage B8 found that the 0.55 um funnel rectifies at 1 THz by its shape
only if its edges are close to mirror-like: the response is half at a
specularity of 0.84 and zero near 0.6. Magnetic focusing gives 0.2 to 0.5
for etched graphene edges. The paper therefore says that an etched funnel
would show nothing, and then qualifies it: the focusing number is for
electrons that hit the edge nearly head-on, a rough surface reflects
slanting electrons more cleanly, and the model used one specularity for
all angles. That qualification is the one open point that could reverse
the paper's conclusion about the short funnel.

Stage B9 asks: **with a specularity that depends on the angle of the hit,
and edges as rough as the focusing measurement implies, how much of the
1 THz response of the 0.55 um funnel is left?**

## 1. Model (figure panels a and b)

The standard model of a rough surface (Soffer, J. Appl. Phys. 38, 1710
(1967), as I remember it; see section 2) gives the probability of a
mirror-like reflection as

    p = exp[-(2 h k_n)^2],      k_n = k cos(theta)

with h the rms height of the roughness, k the electron's wave vector and
theta the angle of the hit from the wall normal. Head-on (theta = 0) the
edge is at its roughest; at grazing incidence p goes to 1.

- **One parameter, the roughness h.** It is chosen so that the head-on
  specularity at the Fermi wave vector of the model (0.21 per nm, a
  wavelength of 30 nm) is 0.2, 0.35, 0.5 and 0.75: h = 3.0, 2.4, 2.0 and
  1.3 nm. The first three span the measured range. These are the
  roughnesses that reproduce the measured head-on numbers at our density.
  They are not measured roughnesses.
- **Each electron uses its own k_n**, so faster electrons see a rougher
  edge. No further parameter.
- **The scattered part must respect detailed balance.** With an
  angle-dependent p, sending the scattered electrons out with the usual
  cosine law would be wrong: the gas at rest would no longer stay
  isotropic near a wall, and an asymmetric shape could then produce a
  current with no drive. The outgoing angle of a scattered electron is
  instead drawn from (1 - p(theta')) cos(theta'), which makes the wall
  reciprocal for any p(theta). In practice: draw from the cosine law and
  accept with probability (1 - p(theta')) / (1 - p(0)).
- Everything else is the Stage B8 model unchanged: computed field, gate at
  300 or 30 nm, contacts, mean free path 700 nm, 1.4e12 per cm^2.

What the two extremes of the model give for the measured range (computed
in the figure script, not simulated):

| head-on specularity | roughness | at 60 degrees | average over the hits of a gas at rest |
|---|---|---|---|
| 0.2 | 3.0 nm | 0.67 | 0.39 |
| 0.35 | 2.4 nm | 0.77 | 0.52 |
| 0.5 | 2.0 nm | 0.84 | 0.64 |
| 0.75 | 1.3 nm | 0.93 | 0.83 |

60 degrees is the angle at which an electron moving along the axis meets
the 30 degree taper.

## 2. Reading before any code

Two things are checked first, because the plan rests on them and I have
not read the sources:

1. **The formula.** Soffer's paper was cited in the manuscript from its
   record only. I will read it, or a review that states its result, and
   confirm the form above and the condition under which it holds (as I
   recall, roughness with no correlation along the surface). If the
   formula or its range of validity is different from what this plan
   assumes, I stop and come back before writing code.
2. **The measurements.** A bounded search (about ten queries) for other
   measurements of edge specularity in graphene: later focusing
   experiments, encapsulated and gate-defined edges, anything at oblique
   incidence, and Masubuchi et al. in full. If the measured head-on range
   turns out wider than 0.2 to 0.5, the roughness values of section 1 are
   adjusted and I say so before running.

Searches carry no personal or project details. What is read and what is
only a record goes into `paper/refs_verification.md` as before.

## 3. Code

- `src/pic.hpp`, `src/pic.cpp`: a roughness parameter beside the existing
  specularity; in `Pic::move` the reflection uses the rule of section 1
  when the roughness is set, and the present rule otherwise. About 30
  lines.
- Counters of wall hits and mirror-like reflections, separately for the
  sloping walls and the others, so that every run reports the specularity
  its electrons actually met. This is what lets the result be compared
  with the one-specularity scan.
- `apps/bpic.cpp`: flag `--rough h_nm`, mutually exclusive with `--p`.
- `tools/pic_ext.sh`: stage P9. Tables, figure and results document
  extended as for the scan.

The imposed-field solver (`src/transport.cpp`) is not changed.

## 4. Acceptance tests, before any production run

- **T31a, unchanged limits.** Roughness 0 reproduces the mirror-wall run
  with the same seed; the existing ladder T22 to T29 still passes.
- **T31b, the reflection law.** Electrons sent at fixed angles onto a
  wall: the mirror-like fraction follows the formula within three
  standard errors at each angle, and the scattered electrons follow the
  stated distribution.
- **T31c, detailed balance.** The funnel as a closed box with rough
  walls and no field: the gas stays uniform and isotropic within
  statistical error. The same test is run once with the cosine-law
  re-emission to show that it would catch the error it is there for.

If a test fails and I cannot fix it within the model of section 1, I stop
and report.

## 5. Runs (figure panel d)

All at 1 THz, 0.1 V, 700 nm, shape-only drive, 8 million particles, 20
periods, as in the scan.

| stage | runs | purpose |
|---|---|---|
| P9 | 8: roughness 3.0, 2.4, 2.0, 1.3 nm, each with the gate at 300 and 30 nm | the answer |
| P9c | 2: mirror-symmetric funnel, 2.4 nm, both gates | must give zero |
| P9z | 1: the funnel with no drive, 2.4 nm, gate at 30 nm | must give zero; catches a wall that rectifies heat |

Eleven runs at about 7.5 minutes each: about 85 minutes on 10 cores. A
second seed is added only for points that fall under outcome 3 below.

## 6. Guesses and decision (figure panels c and d)

Two guesses, recorded now so that the result cannot be read to fit:

- **Guess A: the average decides.** A gas at rest hits every wall,
  sloping or not, with the same cosine-law spread of angles, mostly
  near head-on. Averaged over those hits the measured edges have a
  specularity of 0.39 to 0.64, where the one-specularity scan gives
  nothing. Under this guess the 1.3 nm point gives about 0.2 uA.
- **Guess B: the slanting hits decide.** The electrons that carry the
  rectified current move mostly along the axis and meet the taper at
  about 60 degrees, where the same edges have a specularity of 0.67 to
  0.84. The scan then gives 0.07 to 0.2 uA, up to half of the mirror
  value.

I lean to guess A, because the cosine law holds for any wall
orientation, but I do not trust the argument enough to skip the run.

| outcome at head-on 0.2 to 0.5 | meaning | what follows |
|---|---|---|
| 1. within 0.1 uA of zero (twice the floor) | the slant does not rescue etched edges | the paper's caveat on oblique incidence is replaced by the result; conclusion hardens |
| 2. above 0.15 uA (a third of the mirror value) | etched edges may keep a usable part | the paper reports both edge models; the contact-sensitivity run becomes worth planning |
| 3. between, or the two gates disagree | marginal | second seed for those points, then reported as marginal |

The controls decide whether any of this is believed: if P9c or P9z give
more than 0.1 uA, the stage P9 numbers are not used until the cause is
found.

## 7. Cost

- Reading: about ten searches and two or three papers.
- Code and tests: small; the tests run in minutes.
- Runs: about 85 minutes of machine time, plus up to 30 minutes for
  second seeds.
- Write-up: one table, one figure panel or a replacement of the present
  one, a paragraph in the results document, and a paragraph and a
  limitations sentence in the paper.

## 8. Limits of the planned model

- The formula is for roughness with no correlation along the edge. Real
  edges have some, and correlated roughness scatters into a narrow cone
  about the mirror direction instead of a broad lobe. In that respect
  the model is pessimistic about rough edges.
- The focusing "specularity" is the ratio of two focusing peaks. Taking
  it as the head-on p at our Fermi wave vector is an identification, not
  a derivation: the injected electrons have a spread of angles, and the
  density of that experiment is not ours.
- Roughness is the only edge physics included. Charge at the edge,
  scattering between valleys and edge states are not.
- The electrons are classical particles; the neck holds five to seven
  modes, and the wavelength (30 nm) is not small against the neck.
- Single runs, with a numerical floor of 0.05 uA.

## 9. What comes back

- The reading, with what was confirmed and what was not.
- T31a to T31c with their numbers.
- The table of rectified current and response per absorbed watt against
  roughness for both gates, with the specularity the electrons actually
  met on the sloping walls and elsewhere.
- Which outcome of section 6 holds, which guess was right, and the
  proposed change to the paper. The paper is not changed and nothing is
  committed without a further go-ahead.

## 10. Amendment after the reading (2026-11-15, before any production run)

**The formula.** Soffer's paper is not openly available and was not read.
The form used here, p = exp(-4 h^2 k^2 cos^2(theta)), was confirmed in Shao,
Rong, Hu and Bao, J. Appl. Phys. 122, 155104 (2017) (arXiv:1705.02059, the
passage read in the PDF text), who give it as Ziman's formula extended by
Soffer to include the angle. They add that the formula holds only within
the Kirchhoff (tangent-plane) approximation, which asks for a surface that
is smooth on the scale of the wavelength and is known to fail towards
grazing incidence. Section 8 should be read with that added: the model is
least reliable exactly where it makes the edge most mirror-like. The
condition of no correlation along the surface was not confirmed at the
source; a 1970 paper by Soffer on "weak surface autocorrelation" exists
and was seen as a record only.

**The measurements.** The range measured for etched edges is wider than
the 0.2 to 0.5 this plan assumed: from about 0 to about 0.8.

- Near 0: the magnetoresistance of oxygen-plasma-etched wires on hBN
  (Masubuchi et al., now read in full: "nearly zero specularity", no fitted
  number), the bend resistance of etched encapsulated crosses (Mayorov et
  al. 2011, qualitative), and collimated focusing in reactive-ion-etched
  encapsulated graphene (Barnard et al. 2017: fully diffuse for electrons,
  a third mirror-like for holes).
- 0.8 at 1.8e12 per cm^2 and 0.65 at 6.6e12: the ratio of focusing-peak
  areas in reactive-ion-etched encapsulated twisted bilayer graphene
  (Berdyugin et al. 2020). This is the highest value found for an etched
  edge.
- Close to 1 only for edges defined by gates in bilayer graphene, which
  are not physical edges.
- No experiment resolves the angle of incidence. The wire and bend
  methods, which average over all angles, give the lowest values.

The reading was done by a subagent; what it read in full, as PDF text or
only through a summary is recorded in `paper/refs_verification.md` with
the results.

**Change to the runs.** Two roughness values are added so that the model
spans the wider range: 1.1 nm (head-on 0.8, the highest measured) and
4.1 nm (head-on 0.05). Stage P9 is then 12 runs. One further control is
added: one point of the one-specularity scan (0.9, gate at 300 nm) is
repeated with the new binary. Reason: the mirror-wall path of the new
code reproduces the old binary bit for bit, but the one-specularity path
differs from it in the last bit of 0.1 per cent of the scattered wave
vectors (the compiler now calls sine and cosine separately), which is
enough to change individual trajectories; the repeat shows the statistics
are unchanged. Sixteen runs in all, about two hours.

The decision table of section 6 is read against the wider range: the
question is now what is left across head-on 0.05 to 0.8, and in
particular at 0.8.

Guesses for all six roughness values, recorded before the runs (gate at
300 nm; read off the one-specularity scan at the average specularity for
guess A and at the 60 degree specularity for guess B):

| roughness | head-on | guess A | guess B |
|---|---|---|---|
| 1.1 nm | 0.81 | 0.25 uA | 0.36 uA |
| 1.3 nm | 0.74 | 0.19 uA | 0.33 uA |
| 2.0 nm | 0.50 | 0.05 uA | 0.21 uA |
| 2.4 nm | 0.36 | -0.04 uA | 0.14 uA |
| 3.0 nm | 0.21 | -0.07 uA | 0.08 uA |
| 4.1 nm | 0.05 | at or below -0.07 uA | at or below -0.07 uA |

## 11. Outcome (2026-11-15)

Run as amended; results in `results/BALLISTIC_EDGES.md`. Guess B was
right. Against the table of section 6: outcome 2 at a head-on
specularity of 0.5 and above, outcome 3 (marginal, two seeds) at 0.36,
outcome 1 at 0.2, and a reversed current at 0.05. The paper was revised
the same day, after a separate go-ahead.
