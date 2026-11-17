# Stage B9 results: the short funnel with rough edges (2026-11-15)

Plan: `plans/ballistic_edges.md` (figure `plans/ballistic_edges.png`;
section 10 is the amendment made after the reading and before the runs).
Code: `Pic::reflect` and the wall counters in `src/pic.*`, flag `--rough`
in `apps/bpic.cpp`, tests T31 in `apps/pic_tests.cpp`. Runs:
`tools/pic_ext.sh P9c P9 P9s`, results `results/bpic/P9c.csv`, `P9.csv`,
`P9s.csv` and `ext.log`; ladder `results/bpic/ladder_B9.log`. Tables
`results/bpic/tables_B9.md` and figure `results/bpic/B9_edges.png`
(`tools/edges_results.py`). The reading is recorded in
`paper/refs_verification.md` (addendum of 2026-11-15).

The question: the specularity scan of Stage B8 gave every wall hit the
same chance p of a mirror reflection, and found the rectified current of
the 0.55 um funnel at 1 THz halved at p = 0.84 and gone near p = 0.6. A
real rough edge is more mirror-like the more obliquely it is met, and the
sloping walls of the funnel are met obliquely by electrons that travel
along the axis. Does the funnel keep its rectification with edges as rough
as etched graphene edges have been measured to be?

The edge model: a wall hit is mirror-like with probability
p = exp[-(2 h k cos(theta))^2], with h the rms roughness, k the wave
number of that electron and theta the angle from the wall normal. An edge
is named below by h or by its head-on specularity at the Fermi wave
vector, exp[-(2 h k_F)^2]; a magnetic-focusing measurement is taken to
measure the latter (see Limits).

## Verdict

1. **It depends on which end of the measured range the edge is at.**
   Rectified current at 1 THz, 0.1 V, 700 nm mean free path, shape-only
   drive (uA; in brackets the fraction of the mirror-wall value):

   | rms roughness | head-on specularity | gate at 300 nm | gate at 30 nm | R_abs (A/W), 300 / 30 nm |
   |---|---|---|---|---|
   | 0 (mirror walls, B8) | 1 | +0.429 +- 0.012 | +0.483 +- 0.013 | 0.124 / 0.132 |
   | 1.1 nm | 0.81 | +0.344 +- 0.015 (0.80) | +0.372 +- 0.018 (0.77) | 0.101 / 0.104 |
   | 1.3 nm | 0.74 | +0.309 +- 0.020 (0.72) | +0.378 +- 0.014 (0.78) | 0.091 / 0.106 |
   | 2.0 nm | 0.49 | +0.212 +- 0.018 (0.49) | +0.203 +- 0.016 (0.42) | 0.064 / 0.058 |
   | 2.4 nm | 0.36 | +0.140 +- 0.014 (0.33) | +0.143 +- 0.014 (0.30) | 0.042 / 0.041 |
   | 3.0 nm | 0.21 | +0.041 +- 0.015 (0.09) | -0.024 +- 0.013 (-0.05) | 0.012 / -0.007 |
   | 4.1 nm | 0.05 | -0.102 +- 0.015 (-0.24) | -0.181 +- 0.018 (-0.38) | -0.032 / -0.054 |

   The errors are statistical; the numerical floor of Stage B8,
   0.05 uA, applies to every entry. The 2.4 nm row pools two seeds, the
   other rough-edge rows are single runs.
2. **Against the plan's decision table.** The plan asked about head-on
   specularities of 0.2 to 0.5, the range of the one measurement the
   paper cites. At 0.5 the current is 0.21 and 0.20 uA, above the 0.15 uA
   line: outcome 2, a usable part remains. At 0.2 it is +0.04 and
   -0.02 uA, within the floor: outcome 1, nothing remains. At 0.36 it sat
   on the line (0.152 and 0.140 uA), was run again with a second seed as
   the plan required (0.125 and 0.145 uA), and pools to 0.140 +- 0.014
   and 0.143 +- 0.014 uA, a third of the mirror-wall value: outcome 3,
   marginal. So the answer for that range is not
   one outcome: it runs from nothing at its lower end to half of the
   mirror-wall current at its upper end. The reading widened the measured
   range to about 0 to 0.8 (point 7). At 0.8, the best etched edge found,
   the funnel keeps four fifths: 0.34 and 0.37 uA, 0.10 A/W.
3. **Halved at a head-on specularity of 0.5, gone at 0.2.** Interpolated
   linearly between the points, the current is half its mirror-wall value
   at a head-on specularity of 0.50 (300 nm gate) and 0.55 (30 nm), an
   rms roughness of 2.0 and 1.8 nm, and crosses zero at 0.16 and 0.23,
   3.2 and 2.9 nm. With one specularity for all angles (Stage B8) the
   same two marks were at 0.84 and near 0.6.
4. **Guess B was right, and I had leaned to guess A.** The plan recorded
   two guesses: that the average over all hits decides, or that the
   oblique hits on the taper decide. Each rough-edge current can be
   placed on the one-specularity scan to find the single specularity
   that rectifies as much (panel b of the figure). That equivalent
   specularity is 0.94, 0.91, 0.84, 0.77 and 0.63 for 1.1 to 3.0 nm
   under the 300 nm gate, against 0.95, 0.93, 0.84, 0.78 and 0.67 for
   the formula at 60 degrees and 0.87, 0.82, 0.64, 0.53 and 0.39 for
   the average over all hits. Expressed as an angle it is 56 to 60
   degrees from the wall normal (300 nm gate) and 50 to 60 degrees
   (30 nm): the angle at which an electron moving along the axis meets a
   wall inclined at 30 degrees to it. As a rule for this device: a rough
   edge rectifies like an edge whose one specularity is the fourth root
   of its head-on specularity.
5. **Why guess A failed.** Its premise is true and the runs confirm it:
   the gas as a whole hits every wall with the same cosine-law spread of
   angles. The share of mirror-like hits counted in the runs is the same
   on the sloping walls, the walls along the axis and the walls across
   it to 0.005 (for 2.4 nm: 0.649, 0.654, 0.652), and equals the formula
   averaged over the whole gas (0.659). But the rectified current is not
   carried by the gas as a whole. It is carried by the small driven part
   of the distribution, which moves along the axis. This is my reading
   of the result, not something the runs measure directly: no run
   tagged the electrons that carry the current.
6. **With very rough edges the current reverses.** At 4.1 nm (head-on
   specularity 0.05) the current is -0.10 and -0.18 uA, two and four
   times the floor and of the opposite sign, -0.03 and -0.05 A/W. This
   continues what Stage B8 found with half-diffuse walls (-0.07 and
   -0.13 uA) and, as there, it is larger under the near gate.
7. **The reading** (details in `paper/refs_verification.md`).
   *Confirmed:* the formula, as Eq. (4) of Shao, Rong, Hu and Bao,
   J. Appl. Phys. 122, 155104 (2017), arXiv:1705.02059, read in full,
   which attributes it to Ziman and, for the angle dependence, to
   Soffer. *Not confirmed:* Soffer's paper itself (J. Appl. Phys. 38,
   1710 (1967)) is behind a paywall and was not read. *New caveat:* the
   formula holds within the Kirchhoff approximation, which is least
   reliable towards grazing incidence, where it makes the edge most
   mirror-like. *Measurements:* the specularity of etched graphene
   edges has been reported from about zero (Masubuchi et al., PRL 109,
   036601 (2012); Barnard et al., Nat. Commun. 8, 15418 (2017), for
   electrons) through 0.2 to 0.5 (Taychatanapat et al., Nat. Phys. 9,
   225 (2013)) to 0.65 and 0.8 (Berdyugin et al., Sci. Adv. 6, eaay7838
   (2020), etched twisted bilayer). No experiment found resolves the
   angle of incidence, and no measured rms roughness of an etched
   graphene edge was found. The search was bounded (about ten queries),
   not a survey.
8. **Tests and controls pass.** T31a: with `--rough 0` the new code
   reproduces the mirror-wall run in all 44 columns. T31b: the
   mirror-like share at 0, 30, 60 and 80 degrees matches the formula
   within 3 standard errors at two wave numbers, and the scattered
   angles follow (1 - p) cos (chi-square 16.8 and 17.4 on 18 bins).
   T31b2 and T31c: a closed funnel with 2.4 nm walls and no field stays
   uniform and isotropic (largest deviation 1.7 standard errors over 1e8
   wall hits); re-emitting the scattered electrons with the plain cosine
   law, the obvious shortcut, fails the same test by 179 standard errors.
   Full ladder: 14 passed, 0 failed. Controls: the mirror-symmetric
   funnel with 2.4 nm walls gives +0.020 +- 0.020 and -0.034 +- 0.023 uA,
   the funnel with no drive +0.007 +- 0.017 uA, and the repeat of a
   Stage B8 scan point 0.290 +- 0.018 against 0.291 +- 0.013 uA.

## What this changes

Stage B8's statement, carried by the paper since 2026-11-13, was: the
response vanishes near a specularity of 0.6, above the 0.2 to 0.5
measured for etched edges, so the shape rectifies only "with edges more
specular than etched edges have been measured to be". Two parts of that
no longer hold.

- The measured range is wider. Etched edges have been measured from
  about 0 to about 0.8, not 0.2 to 0.5.
- With the angle dependence of a rough edge, the funnel tolerates much
  rougher edges than the one-specularity scan said: the current is
  halved at a head-on specularity of 0.5, not 0.84, and gone at 0.2,
  not 0.6.

Together: an etched edge at the upper end of what has been measured
(0.5 to 0.8) keeps half to four fifths of the mirror-wall response,
0.06 to 0.10 A/W per absorbed watt; one at the lower end (0.2 and
below) keeps nothing or reverses the sign. The ballistic funnel is
therefore not ruled out by its edges. It needs the better half of the
etched edges that have been measured, where before it appeared to need
edges better than any.

What does not change: the mirror-wall figures (0.12 to 0.13 A/W), the
efficiency of order 1e-4, the finding that under a 30 nm gate the gate
connection of a straight strip rectifies more than the shape, and the
comparison with the plasmonic element.

**Change to the paper (proposed here; applied on 2026-11-15 after the go-ahead).**

1. Abstract, introduction, Sec. V E, discussion and conclusion: replace
   "0.2 to 0.5 measured for etched edges" by the measured range 0 to
   0.8 with its sources, and replace the statement that the shape needs
   edges more specular than any measured by the result above.
2. Sec. V E: a paragraph on the rough-edge model and its result, and
   one new panel or a small figure from `B9_edges.png` (current against
   head-on specularity for both edge models). The existing caveat on
   oblique incidence (Sec. V E and the limitations) is replaced by the
   result and by the Kirchhoff caveat.
3. Sec. II C: two sentences on the wall rule; supplement: tests T31a to
   T31c and the controls.
4. References: Berdyugin et al. (2020), Barnard et al. (2017) and Shao
   et al. (2017) to be added and verified as the others were; Masubuchi
   et al. is now read in full.
5. `OVERVIEW.md` and the Stage B8 section of `README.md` carry the old
   statement and need the same correction.

The plan's outcome 2 also said that the contact-sensitivity run "becomes
worth planning": a quarter of the bias drops at the contact planes in
the computed field, and whether another contact model changes the
0.12 A/W has not been tested. That is a separate stage and needs its
own plan.

## Deviations from the plan

1. **Two roughness values added** (1.1 and 4.1 nm, head-on 0.8 and
   0.05), after the reading widened the measured range and before any
   production run; recorded in section 10 of the plan with guesses for
   them.
2. **A fourth control**, the repeat of the Stage B8 point p = 0.9. The
   new wall routine differs from the old expressions in the last bit of
   some reflections (the compiler now calls sine and cosine separately),
   so runs with a constant specularity below 1 no longer repeat the old
   random sequence; trajectories diverge, statistics should not. The
   repeat confirms it: 0.290 +- 0.018 against 0.291 +- 0.013 uA.
3. **The plan's no-drive control is in `P9c.csv`** with the others; the
   plan called it P9z.
4. **The count of mirror-like hits is not "the specularity the
   electrons actually met"**, as section 9 of the plan put it. The
   model's sample particles fill the whole Fermi sea, and most of them
   lie below the Fermi level, have a longer wavelength and see a
   smoother edge. The count (0.65 at 2.4 nm) therefore matches the
   formula averaged over the whole gas (0.659), not over the electrons
   near the Fermi level that carry the response (0.49). It is reported
   as a check of the wall rule and of the equal spread of angles on
   every wall, and table 3 of `tables_B9.md` gives the Fermi-level
   values from the formula. Simulating the whole sea is still correct:
   wall scattering is elastic and acts on each electron separately.
5. **Second seed** for 2.4 nm only (`P9s.csv`), as the plan's outcome 3
   requires.

## Tables and figure

`results/bpic/tables_B9.md`: 1 controls; 2 current, absorbed power and
R_abs against roughness; 3 the specularity of each roughness (head-on,
at 60 degrees, averaged; at the Fermi wave vector and for the thermal
spread at 300 K) and the counted mirror-like shares by wall orientation;
4 the two guesses against the result; 5 the equivalent one specularity
and angle; 6 the half and zero marks; 7 the second seed; 8 bookkeeping.

`results/bpic/B9_edges.png`: (a) rectified current against the head-on
specularity for both gates, with the one-specularity scan of Stage B8,
the measured range and the numerical floor; (b) the equivalent one
specularity against the head-on specularity, with the formula at 60
degrees, averaged over all hits and head-on.

One regularity I cannot explain and would not use elsewhere: under the
300 nm gate the current as a fraction of its mirror-wall value is close
to the head-on specularity itself from 0.36 to 0.81 (0.33, 0.49, 0.72,
0.80 at 0.36, 0.49, 0.74, 0.81). It follows only approximately from the
fourth-root rule and the near-linear scan, and fails below 0.36.

## Limits

- **The edge formula.** It is the Kirchhoff result for a boundary with
  Gaussian height statistics; it ignores the correlation length of the
  roughness and shadowing, and is least reliable towards grazing
  incidence. The equivalent angle found here is about 60 degrees, not
  grazing, but the formula has not been tested against any measurement
  on graphene, because none resolves the angle.
- **Reading a measured specularity as the head-on value.** In magnetic
  focusing the electrons that form the peaks meet the edge near normal
  incidence, so the ratio of peaks is taken as the head-on specularity.
  This is my reading, as in Stage B8. The wire measurements (Masubuchi
  et al.) weight all angles, so "nearly zero" there means an edge
  rougher than any run here.
- **The electrons are classical.** The formula carries the wavelength,
  but diffraction at the 100 nm neck and the five to seven modes in it
  are not in the model.
- **Wave number.** Each electron reflects with its own k. For the
  electrons within kT of the Fermi level at 300 K the specularities are
  a little lower than at k_F (head-on 0.44 against 0.49 at 2.0 nm). The
  equivalent specularity found lies between the two 60 degree values or
  on the k_F one; the difference is within what the linear
  interpolation of the scan can resolve.
- **The equivalent specularity** rests on the one-specularity scan,
  whose points at 0.7 to 0.9 are single runs, and on linear
  interpolation between them; its errors in table 5 are from the
  rough-edge runs only. Below 0.5 the scan has no points, so the 4.1 nm
  runs have no equivalent.
- **One device, one frequency, one amplitude, one mean free path.** All
  runs are the 0.55 um funnel at 1 THz, 0.1 V and 700 nm; all but the
  2.4 nm point are single runs with a statistical error of 0.013 to
  0.020 uA and the floor of 0.05 uA.
- Everything in the Limits of Stage B8 still applies, the contact model
  first.

## Cost

18 production and control runs, 1.96 hours of wall time on 10 cores
(about 6.5 minutes each); the tests T31 about 3 minutes and the full
ladder about 12. Particles lost through a wall: at most 2 of 8 million
per run. Largest difference between the currents at the source and
drain planes: 0.003 uA.
