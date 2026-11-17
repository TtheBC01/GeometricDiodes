# Plan: the contacts of the short funnel, and the measured device with a computed field (Stage B10)

Written 2026-11-15, before any code or run. Not yet approved. Companion to
`plans/ballistic_selfconsistent.md`, `results/BALLISTIC_SELFCONSISTENT.md`
(Stage B8) and `results/BALLISTIC_EDGES.md` (Stage B9).
Figure: `plans/ballistic_contacts.png` (script `tools/contacts_plan_fig.py`,
which also prints the numbers quoted here).

The paper's one positive result is the 0.55 um funnel at 1 THz: 0.12 A/W
per absorbed watt with mirror walls, less with rough ones. Two things
behind that number have never been tested.

- **The contacts.** Every run so far has used one contact: a plane that
  absorbs every electron reaching it and injects an equilibrium gas,
  100 nm from the taper on one side and 100 nm from the neck on the other,
  inside a mean free path of 700 nm. A quarter of the bias drops at those
  two planes. This is the first item in the limits of Stages B8 and B9.
- **The measurement.** The model has been compared with one measured
  number, the static asymmetry 1.66 of the 6 um Device D of Nguyen et al.
  The comparison in the paper uses the uniform imposed field of the first
  stage, which Stage B8 showed to be the wrong field for the short funnel
  (asymmetry 1.56 with it, 1.053 with the computed one). The run with a
  computed field was planned in Stage B8 as P7 and never made.

Stage B10 asks two questions. **A: does the 1 THz response of the short
funnel depend on the contact model beyond a series resistance? B: what
static asymmetry does the measured device give when its field is
computed?** The two parts are independent and can be approved separately.

## 1. Part A: contact variants (figure panels a and b)

A real contact differs from the ideal one in what it lets through. Three
one-parameter variations, each changing one thing:

- **Transparency.** An electron reaching the contact plane is absorbed
  with probability T and otherwise reflected like a mirror; the injected
  flux is reduced by the same T. This is a contact resistance. For a wide
  clean strip between two such contacts the conductance is T / (2 - T)
  of the ideal value, so that T = 0.75, 0.5 and 0.25 correspond to a
  contact resistance of 81, 145 and 338 ohm um per contact as a
  transfer-length measurement would report it (48 ohm um at T = 1, the
  ballistic floor at this density). As I remember it, the edge contacts
  to encapsulated graphene of Wang et al. (Science 342, 614 (2013)) are
  about 150 ohm um for electrons, and Device D has contacts of that kind;
  section 3 checks this before any code.
- **Collimation.** The same contact resistance, but with the reflected
  share taken from the slanting electrons: T(theta) = exp(-pi k d
  sin^2 theta), the classical result for a p-n junction of width d in
  front of a contact (Cheianov and Fal'ko 2006, from memory; see
  section 3). The junction widths are chosen so that the strip
  conductance equals that of T = 0.5 and T = 0.25: d = 7 nm and 38 nm,
  which pass half of the electrons at 23 and at 10 degrees from the
  normal. Comparing each with its matched transparency isolates the
  angular distribution a contact feeds into the taper, which is what a
  rectifier that works by geometry could be sensitive to.
- **Distance.** The ideal contacts moved out: leads of 300 nm on both
  sides instead of 100 nm, so that the device is 0.95 um long.

In both reflecting variants the injected electrons are those of the
equilibrium gas thinned by the same T(theta), which keeps the gas at rest
in equilibrium (the reflected and the injected flux add up to the
cosine law at every angle). Each electron uses its own wave vector in
the junction formula. The metal strips, their potentials and the field
solver are unchanged.

Not modelled separately: doping of the graphene next to the metal with
the same sign as the channel. For classical trajectories a density step
in front of a contact leaves the electrons that enter the channel with
the equilibrium distribution, so its effect comes through reflection at
the step, which the two variants above represent. That is an argument,
not a test, and it is listed in section 9.

## 2. Part B: Device D with a computed field (figure panel d)

Device D as in the first stage: leads 2 um wide, 30 degree taper to a
175 nm neck, abrupt shoulder, 6.0 um long; 1.4e12 per cm^2; mean free
path 700 nm (the experimenters' estimate); mirror walls; gate at 300 nm
(the measured lever arm, 7.2e10 per cm^2 per volt, is that of about
300 nm of oxide); ideal contacts; static bias of +0.1 and -0.1 V in the
shape-only wiring. Output: the asymmetry I(+0.1 V) / |I(-0.1 V)| and the
map of where the bias drops.

- The device has 52 times the area of the short funnel and holds
  148 000 electrons. At the particle density of the short-funnel runs it
  would need 200 million particles and about four hours per run. The
  plan uses a grid of 7.5 nm (23 cells across the neck; the short funnel
  has 20) and 50 million particles, a quarter of that density, and
  checks the particle number with a second pair at 25 million.
- What this buys and what it risks: Stage B8 found that too few
  particles bias a rectified current high (by 40 % at a quarter of the
  production density in the short funnel), because sample particles
  collide. Electrons also stay at least ten times longer in the 6 um
  device. A
  bias of that size does not matter for telling 1.05 from 1.66. It would
  matter if the answer came out between 1.15 and 1.4, and then the run is
  repeated at 100 million particles before anything is concluded.
- The measurement was at 80 uA, which is 0.076 V at the model's
  resistance of 0.95 kilohm. The runs are at 0.1 V so that they compare
  directly with every earlier model number (1.62, 1.09, 1.053); the
  asymmetry is then also quoted scaled to 80 uA.
- The measurement's own wiring (one contact and the gate at fixed
  potentials) is not run. At a gate 300 nm away, 0.1 V changes the
  density by 0.5 %, which gives an asymmetry of about 0.003.
- **Conditional pair.** If the 700 nm result is "not reproduced"
  (section 7), the pair is repeated with no scattering, to learn whether
  any mean free path could reproduce the measurement. The uniform-field
  model gave 1.62 there.

## 3. Reading before any code

Checked first, because the plan rests on them and I have not read the
sources. Searches carry no personal or project details; what is read
and what is only a record goes into `paper/refs_verification.md`.

1. **Measured contact resistance** of edge contacts to encapsulated
   graphene, and its dependence on density, so that the transparencies
   of section 1 can be placed against real contacts. If measured
   contacts at this density lie outside 80 to 340 ohm um, the values are
   changed and I say so before running.
2. **The junction formula** and its condition (a junction wide against
   the wavelength, as I recall; at 7 nm it is not, and that variant is
   then a model of angular filtering only).
3. **Which way metal contacts dope encapsulated graphene** near an edge
   contact, which decides whether the collimating variant describes the
   measured devices or is a sensitivity test only.
4. **Nguyen et al., reread** for the definition of the asymmetry, the
   forward direction, which contact was grounded, and any contact
   resistance they state.

## 4. Code

- `src/pic.hpp`, `src/pic.cpp`: two contact parameters (transparency,
  junction width); in the mover an electron at a contact segment is
  absorbed with probability T(theta) and otherwise mirrored; in the
  injection a candidate drawn as now is kept with probability T(theta).
  With T = 1 and no junction no random number is drawn, so the present
  runs are reproduced bit for bit. About 40 lines.
- `apps/bpic.cpp`: flags `--ct T` and `--cjunction d_nm`, mutually
  exclusive; two columns appended to the result files.
- `apps/pic_tests.cpp`: tests T32a to T32d.
- `tools/pic_ext.sh`: stages P10 and P7; a results script and figure as
  for Stage B9.
- Device D needs no new geometry code: the shape flags of `gd_bpic`
  already build it.

## 5. Acceptance tests, before any production run

- **T32a, unchanged limits.** Transparency 1 reproduces the present
  binary with the same seed; the ladder T22 to T31 still passes.
- **T32b, the contact law.** Electrons sent at fixed angles onto a
  contact: the absorbed fraction follows T(theta) within three standard
  errors, for both variants.
- **T32c, the contact resistance.** A clean straight strip with mirror
  walls at +-5 mV: conductance over the ideal value equal to
  T / (2 - T), that is 0.600, 0.333 and 0.143, within 2 % or three
  standard errors; for the two junctions the test computes the target
  from the formula with the thermal spread of wave vectors (0.333 and
  0.143 at the Fermi wave vector).
- **T32d, equilibrium.** The funnel with each contact and no drive:
  no current within two standard errors, the particle number steady at
  the equilibrium fill, kinetic energy constant within 1 % over 20 ps.
  Run once with the injection deliberately not thinned, to show that the
  test catches the error it is there for.
- **P7 pilot.** Device D for 2 ps: the measured time per step, the
  memory, and the contact potentials held to round-off.

If a test fails and I cannot fix it within the model of sections 1 and
2, I stop and report.

## 6. Runs

Part A, all on the 0.55 um funnel with mirror walls, 700 nm, shape-only
wiring; at 1 THz and 0.1 V with 8 million particles and 20 periods
unless stated:

| stage | runs | purpose |
|---|---|---|
| P10t | 6: transparency 0.75, 0.5, 0.25, gate at 300 and 30 nm | the answer for contact resistance |
| P10k | 4: junction 7 and 38 nm, both gates | the same contact resistance with other angles |
| P10l | 2: leads of 300 nm, ideal contacts, both gates, 16 million particles | whether the distance to the contacts matters |
| P10c | 3: mirror-symmetric funnel at transparency 0.25, both gates; the funnel with the 38 nm junction and no drive | must give zero |
| P10n | 1: transparency 0.25, gate at 300 nm, 16 million particles | electrons stay four times longer behind such contacts; checks the particle number |
| P10s | 4: static, +-0.1 V, transparency 0.5 and 0.25, gate at 300 nm, with potential maps | conductance and share of the bias at the contacts, to test the series picture directly |

Part B:

| stage | runs | purpose |
|---|---|---|
| P7 | 2: Device D at +0.1 and -0.1 V, 50 million particles, 15 ps of settling and 15 ps of averaging | the asymmetry |
| P7n | 2: the same at 25 million particles | particle-number check |
| P7x | 2, conditional: the same with no scattering | only if P7 is "not reproduced" |

## 7. Guesses and decision (figure panels c and d)

Recorded now so that the result cannot be read to fit.

**Part A, guess S: the contact is only a series resistor.** It takes its
share of the applied voltage and does nothing else, so the rectified
current falls as the square of the voltage left on the device and the
response per absorbed watt as its first power:

| transparency | contact resistance | voltage left | I_dc, 300 / 30 nm gate | R_abs, 300 / 30 nm gate |
|---|---|---|---|---|
| 1 (measured) | 48 ohm um | 1 | 0.429 / 0.483 uA | 0.124 / 0.132 A/W |
| 0.75 | 81 ohm um | 0.90 | 0.35 / 0.39 uA | 0.112 / 0.119 A/W |
| 0.5 | 145 ohm um | 0.75 | 0.24 / 0.27 uA | 0.093 / 0.099 A/W |
| 0.25 | 338 ohm um | 0.50 | 0.11 / 0.12 uA | 0.062 / 0.066 A/W |

Under guess S each junction gives the same as its matched transparency,
and the long leads give the short-lead value. I lean to S for the
transparency runs. For the junctions I have no confident guess: a
collimated beam meets the taper at a different angle from a cosine-law
gas, and I cannot say which way that goes. Even odds that they fall
within a quarter of S.

| outcome | meaning | what follows |
|---|---|---|
| 1. every run within a quarter of guess S, or two standard errors | to this device a contact is a series resistor | the limitation is replaced by the result; the paper states 0.12 A/W as the ideal-contact value and gives the value at the measured contact resistance |
| 2. the transparency runs follow S, a junction or the long leads do not | the response depends on how the contact feeds the taper | the paper reports the range and names it as an open device parameter |
| 3. at transparency 0.5 the current is below half of guess S (0.12 uA) at either gate | reflecting contacts remove the shape rectification beyond a series loss | a further condition on the positive result, beside the edges |
| 4. above guess S by more than a quarter | reflection at the contacts helps | a second seed first, then reported |

**Part B.** From the short funnel, where the computed field cut the
excess asymmetry about tenfold at every mean free path (1.83 to 1.09
with no scattering, 1.56 to 1.053 at 700 nm, 1.28 to 1.02 at 100 nm), and
from the uniform-field value of 1.09 for Device D, my guess is **1.01 to
1.05**: not reproduced.

| asymmetry at 0.1 V | meaning | what follows |
|---|---|---|
| below 1.15 | not reproduced | the paper says so in Sec. III: the agreement of the first model with 1.66 came from its imposed field; the conditional pair P7x is run; the paper states that its model does not account for the one measured dc asymmetry and lists what it leaves out |
| 1.15 to 1.4 | partly | repeated at 100 million particles, then reported with its uncertainty |
| 1.4 and above | reproduced | the paper gains a check against experiment in the model that gives its headline number |

The controls decide whether Part A is believed: if a run of P10c gives
more than 0.1 uA, the stage P10 numbers are not used until the cause is
found. For Part B the pair at 25 million particles plays that part: if
it differs from the pair at 50 million by more than 0.03 in the
asymmetry, the result is reported as not converged.

## 8. Cost

- Reading: about ten searches and three or four papers.
- Code and tests: small; the tests run in minutes.
- Part A: 20 runs, about 2 hours 40 minutes of machine time on 10
  cores (6.5 to 8.5 minutes for a run at 8 million particles, twice
  that at 16 million, 4 minutes for a static run).
- Part B: about an hour per run at 50 million particles and half that
  at 25 million, from the measured speed of the present code; the pilot
  gives the real figure. 3 to 4 hours, and 2 more if the conditional
  pair is run. This part is best left to run overnight.
- Write-up: tables, one figure, a results document, and the proposed
  change to the paper.

## 9. Limits of the planned model

- A contact is described by what it lets through as a function of
  angle, with mirror reflection of the rest. Nothing depends on energy
  except through the wave vector in the junction formula. Contacts that
  scatter diffusely, or that differ at source and drain, are not
  covered.
- Doping next to the metal enters only through the argument of
  section 1.
- The contact resistance of the transparency variant is exact for a
  wide clean strip. In the funnel it is the quantity being tested.
- The contact variants are run with mirror walls only. Rough walls and
  imperfect contacts together are not run; if both act as simple
  factors the paper can combine them, and it will say that this is
  assumed.
- Part B: a quarter of the particle density of the short-funnel runs,
  one seed, one mean free path unless the conditional pair is run, and
  the experimenters' estimate of that mean free path.
- Everything in the limits of Stage B8 other than the contact model
  still applies: classical trajectories, a uniform dielectric, a
  voltage source for the antenna.

## 10. What comes back

- The reading, with what was confirmed and what was not.
- T32a to T32d and the pilot with their numbers.
- Part A: rectified current and response per absorbed watt for every
  contact, against guess S; the static conductance and the share of
  the bias at the contacts; which outcome holds.
- Part B: the asymmetry of Device D with its error, at both particle
  numbers, the map of the bias, and which band it falls in.
- The proposed change to the paper. The paper is not changed and
  nothing is committed without a further go-ahead.

## 11. Amendment after the reading (2026-11-15, before any production run)

Approved the same day ("run both parts"). The reading was done by a
subagent; what it read in full and what in part goes into
`paper/refs_verification.md` with the results. I looked at Fig. 5 of
Nguyen et al. and the text on it myself.

**Contact resistance.** Wang et al. (Science 342, 614 (2013), main text
read in full; the supplement was not accessible) give about 150 ohm um
for electrons at high density (their figure is at 2.5e12 per cm^2), a
value that "includes the intrinsic limit set by the quantum resistance
of the channel", two to three times more for holes, and a rise towards
neutrality. In the convention of section 1 that is a transparency of
about 0.4 (my conversion), or about 200 ohm um at the density used
here if the transparency does not depend on density. Self-aligned Cr/Au
edge contacts have been quoted at 50 to 100 ohm um at 1.6 K (Handschin
et al. 2017), and scalable processes at 1 to 2 kilohm um (Pandey et al.
2018), a transparency of 0.05 to 0.1. The planned values stand: they
bracket the best measured contacts. Ordinary contacts lie below the
lowest planned transparency, where guess S already puts the current at
twice the numerical floor; no run is added there.

**Junction formula.** Confirmed as written in section 1 (Cheianov and
Fal'ko, Eq. (1), read in the arXiv text). Their d is defined through
the slope of the potential at the centre, which for a linear ramp is
half its width, and the formula is for k_F d above 1. The 38 nm
junction meets that (k_F d = 8); the 7 nm one barely (1.5) and is a
model of angular filtering only.

**Doping.** Cr-based edge contacts dope the graphene next to them
n-type (Wang et al., from the electron-hole asymmetry and the work
function; Handschin et al., stated directly). For an electron channel
there is then no p-n junction in front of the contact. The collimating
variant is a sensitivity test here, not a description of the measured
devices; it would describe a hole channel.

**The measured device. This changes Part B.**

- The asymmetry of Nguyen et al. is a ratio of voltages at plus and
  minus the same current, and its sense agrees with ours: electrons
  pass more easily from the taper through the neck.
- Device D at the gate voltage of the 1.66 passes +100 uA at +2.0 V and
  -100 uA at -1.2 V (Fig. 5(c)). Its resistance is 12 to 20 kilohm,
  about 15 kilohm at low bias, against 0.95 kilohm in the model, and
  the 1.66 is reached at about 1.5 V, not at the 0.076 V that section 2
  assumed. The abstract speaks of "bias voltages up to a few volts".
- Below 20 uA (0.3 V) the paper calls the curves "quite linear";
  Fig. 5(d) shows an asymmetry of about 1.05 at 10 uA and 1.17 at
  20 uA. At the 0.1 V of the model the measured asymmetry is therefore
  about 1.03 to 1.04, read from a figure.
- The Dirac point of Device D is not given. The 1.4e12 per cm^2 of the
  model takes it from device A and is an upper bound; if Device D is
  like device B the density is 0.7e12 to 0.9e12.
- Which contact was grounded is not stated. With 1 to 2 V along the
  channel and a gate overdrive of 10 to 20 V, the gate changes the
  density along the channel by ten per cent or more, a source of
  asymmetry that needs no shape and reverses with the carrier type as
  the measured one does.

So the comparison of section 2, and the comparison in Sec. III of the
paper, set a model at 0.1 V against a measurement at 1.5 V on a device
fifteen times more resistive than the model's. The decision bands of
section 7 for Part B do not apply. **Part B is held** until this has
been reported; Part A does not depend on it and proceeds. The pilot is
still run, because it costs minutes and gives the real cost of any
Device D run.

**Correction to guess S (2026-11-15, 23:40, before any Part A run had
finished).** The table of section 7 is wrong as a series-resistor
prediction, and I noticed it while writing the analysis script. A
contact resistance lowers the voltage on the device, which lowers the
rectified current as the square of the voltage share f; but the
rectified current must also flow through the contacts, with no dc
voltage across the whole, which costs one more factor f. A series
resistor therefore gives I_dc as the cube of f and the response per
absorbed watt as its square:

| transparency | voltage share f | I_dc, 300 / 30 nm gate | R_abs, 300 / 30 nm gate |
|---|---|---|---|
| 0.75 | 0.90 | 0.31 / 0.35 uA | 0.101 / 0.107 A/W |
| 0.5 | 0.75 | 0.18 / 0.21 uA | 0.070 / 0.074 A/W |
| 0.25 | 0.50 | 0.054 / 0.061 uA | 0.031 / 0.033 A/W |

The results give both: guess S as registered in section 7, and this
corrected series-resistor value. The outcomes of section 7 are read
against the corrected value, and I say where the registered one would
have read differently. At a transparency of 0.25 the corrected value is
at the numerical floor, so those runs can only show whether the current
is larger than a series resistor allows.

## 12. Outcome of Part A (2026-11-16)

Run as amended; results in `results/BALLISTIC_CONTACTS.md`, tables in
`results/bpic/tables_B10.md`, figure `results/bpic/B10_contacts.png`.
Four runs were added as the table of section 7 required (stage P10r: a
second seed at transparencies 0.5 and 0.25, both gates), and one control
after test T32d (the funnel with no drive at a transparency of 0.25).

Guess S is rejected in both directions. Contacts that reflect all
angles alike give 1.1 to 1.2 (T = 0.75), 1.6 to 2.0 (0.5) and 2.4 to 4.8
(0.25) times the corrected series-resistor current: outcome 4 at 0.5 and
0.25, confirmed by second seeds within 0.4 standard errors. Collimating
contacts of the same conductance give a current at the numerical floor.
The long leads give 0.54 and 0.67 of the short-lead current, but that
run also changes the transit time and does not isolate the contact. Read
against the guess as registered in section 7 (f squared), the
transparency runs under the 30 nm gate would have been outcome 1; under
the 300 nm gate outcome 4 either way.

I leaned to S for the transparency runs and was wrong; for the
collimating contacts I gave even odds, and they fell far below; the
long-lead guess overlooked the transit time.

Part B was held for a decision (section 11) and then run; see
section 13.

## 13. Outcome of Part B (2026-11-16)

Run after Todd's "Run it first" (before the paper is revised). Stages P7
and P7n as in section 6; results in `results/BALLISTIC_CONTACTS.md`,
tables `results/bpic/tables_B10b.md`, figure
`results/bpic/B10_deviceD.png`.

- Asymmetry of Device D at 0.1 V with a computed field, 700 nm:
  **1.0174 +- 0.0008** at 50 million particles, 1.0172 +- 0.0014 at 25
  million. Inside my guess of 1.01 to 1.05. In the bands of section 7
  this is "not reproduced", but section 11 had already withdrawn those
  bands: the 1.66 belongs to 1.5 V. At 0.1 V the measurement reads 1.03
  to 1.04 (+-0.02), which the model does not contradict and, the device
  being 13 to 22 times more resistive than the model, does not confirm.
- 55 % of the bias falls on the 500 nm centred on the neck (uniform
  field: 8 %).
- Convergence rule of section 7: met for the asymmetry (difference
  0.0002 against a limit of 0.03). The conductance differs by 2.5 %
  between the two particle numbers, which the rule did not cover; it is
  reported as uncertain by a few per cent.
- The gas was still heating at the end of the window (kinetic energy
  +3.5 % to +4.5 % across it).
- P7x (no scattering) was not run: its condition was tied to the
  withdrawn bands. It is left for a decision.

