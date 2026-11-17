# Stage B10 results: the short funnel between contacts that reflect, and Device D with a computed field (2026-11-16)

Plan: `plans/ballistic_contacts.md` (figure `plans/ballistic_contacts.png`;
section 11 is the amendment made after the reading, with the correction
to the registered guess). Code: `Pic::contact_transmission`, the contact
branch of `Pic::move` and the thinned injection in `src/pic.*`, flags
`--ct` and `--cjunction` in `apps/bpic.cpp`, tests T32 in
`apps/pic_tests.cpp`. Runs: `tools/pic_ext.sh P10c P10t P10k P10s P10l
P10n P10r`, results `results/bpic/P10*.csv` and `ext.log`; test logs
`results/bpic/ladder_B10_2M.log` and `ladder_B10_8M.log`. Tables
`results/bpic/tables_B10.md` and figure `results/bpic/B10_contacts.png`
(`tools/contacts_results.py`). The reading is recorded in
`paper/refs_verification.md` (addendum of 2026-11-15 for Stage B10).

Part A (the contacts) fills the document up to "What this changes".
**Part B (Device D with a computed field) was run afterwards** and is the
last section: asymmetry 1.017 at 0.1 V, beside a measured 1.03 to 1.04
at the same voltage on a device fifteen times more resistive.

The question: every earlier run ended the device at ideal contacts, planes
that absorb each arriving electron and inject an equilibrium gas. A real
contact reflects part of what arrives. Does the shape rectification of the
0.55 um funnel survive that, and is the loss what a contact resistance in
series would give?

The contact models (all with mirror walls, 700 nm mean free path, 1 THz,
0.1 V, shape-only drive):

- *Transparency T.* Each arriving electron is absorbed with probability T
  whatever its angle, and mirror-reflected otherwise; the injected gas is
  thinned by the same T. Between two such contacts a clean strip conducts
  T / (2 - T) of the ideal value, which a transfer-length measurement
  would report as 48, 81, 145 and 338 ohm um per contact for T = 1, 0.75,
  0.5 and 0.25.
- *Collimating.* Absorption probability exp(-pi k d sin^2(theta)), the
  transmission of a p-n junction of half width d in front of the contact
  (Cheianov and Fal'ko): electrons near the normal pass, slanting ones are
  reflected. d = 7 and 38 nm, chosen so that a clean strip conducts as
  with T = 0.5 and 0.25.
- *Distance.* Ideal contacts, leads of 300 nm instead of 100 nm.

"Series" below is what a series resistor of the same contact resistance
gives: with f the share of the voltage left on the device (0.90, 0.75,
0.50), the current falls as f cubed and the response per absorbed watt as
f squared.

## Verdict

1. **To this device a contact is not a series resistor, in either
   direction.** Rectified current (uA) and response per absorbed watt
   (A/W); in brackets the ratio to the series value:

   | contact | resistance per contact | I_dc, gate 300 nm | I_dc, gate 30 nm | R_abs, 300 / 30 nm | series R_abs |
   |---|---|---|---|---|---|
   | ideal (Stage B8) | 48 ohm um | +0.429 +- 0.012 | +0.483 +- 0.013 | 0.124 / 0.132 | |
   | T = 0.75 | 81 ohm um | +0.345 +- 0.017 (1.10) | +0.411 +- 0.014 (1.16) | 0.107 / 0.123 | 0.101 / 0.108 |
   | T = 0.5 | 145 ohm um | +0.358 +- 0.013 (1.95) | +0.328 +- 0.012 (1.58) | 0.125 / 0.115 | 0.071 / 0.075 |
   | T = 0.25 | 338 ohm um | +0.266 +- 0.007 (4.8) | +0.147 +- 0.010 (2.4) | 0.120 / 0.071 | 0.032 / 0.034 |
   | collimating, 7 nm | as T = 0.5 | +0.075 +- 0.016 (0.41) | -0.049 +- 0.019 (-0.24) | 0.027 / -0.019 | 0.071 / 0.075 |
   | collimating, 38 nm | as T = 0.25 | +0.036 +- 0.012 (0.65) | -0.061 +- 0.016 (-0.97) | 0.017 / -0.033 | 0.032 / 0.034 |
   | ideal, leads of 300 nm | 48 ohm um | +0.231 +- 0.014 | +0.323 +- 0.015 | 0.088 / 0.103 | |

   Errors are statistical; the numerical floor of 0.05 uA applies to
   every entry. T = 0.5 pools two seeds at each gate; T = 0.25 pools two
   seeds and a run with 16 million particles (300 nm gate) and two seeds
   (30 nm gate); the other rows are single runs.
2. **A contact that reflects all angles alike costs far less than its
   resistance in series.** With the gate at 300 nm the response per
   absorbed watt is 0.12 A/W at every transparency down to 0.25, a
   contact resistance of 338 ohm um: the current falls by 38 per cent
   and the absorbed power by 36. With the gate at 30 nm the response
   holds to T = 0.5 (0.115 A/W) and falls to 0.071 A/W at T = 0.25, still
   twice the series value. At the best contact resistance found in the
   reading, 150 to 200 ohm um (T about 0.4), interpolation between the
   rows gives about 0.12 and 0.10 A/W. Per watt available from a 75 ohm
   antenna the single device gives less than with ideal contacts, because
   it absorbs less: 0.020 and 0.015 A/W at T = 0.5 and 0.25 against 0.023
   (300 nm gate, first seed).
3. **A contact that passes only electrons near its normal removes the
   rectified current.** Both collimating contacts conduct like their
   matched transparency (tested on a strip, point 8) and absorb about as
   much power, but the current is +0.08 and +0.04 uA with the gate at
   300 nm and -0.05 and -0.06 uA at 30 nm: at the numerical floor, 9 to
   14 standard errors below the matched transparency runs. The negative
   values are 2.6 and 3.8 standard errors from zero and equal to the
   floor in size; I do not claim a reversed sign from them.
4. **Against the plan's decision table** (section 7 of the plan).
   Outcome 1, a series resistor: holds only at T = 0.75 (1.10 +- 0.05 and
   1.16 +- 0.04 of the series value). Outcome 4, above the guess by more
   than a quarter: T = 0.5 and 0.25 at both gates. The plan asked for a
   second seed before that is reported; the second seeds differ from the
   first by 0.1 to 0.4 standard errors. Outcome 3 was written for the
   transparency runs and does not occur there; the collimating contact
   matched to T = 0.5 is below half the series value at both gates.
   Outcome 2 does not apply as written, since the transparency runs do
   not follow the guess. In one sentence: the guess is rejected, upwards
   for contacts that reflect all angles alike and downwards for
   contacts that select by angle.
5. **Against the guess as I registered it.** The plan's section 7 had the
   current falling as f squared, which left out that the rectified current
   must itself flow through the contacts; I corrected this to f cubed in
   the plan before any run had finished (section 11). The registered
   numbers read differently in one place: with the gate at 30 nm the
   transparency runs are 1.05, 1.19 and 1.19 of the registered guess,
   inside its quarter band, and would have been read as outcome 1. With
   the gate at 300 nm they are 0.99, 1.47 and 2.43 of it, outcome 4
   either way.
6. **The static curve shows the same thing** (gate 300 nm, +-0.1 V).

   | contacts | G (mS) | G over ideal (series) | asymmetry (series) | share of the bias at the two contact planes (series) |
   |---|---|---|---|---|
   | ideal | 0.845 | 1 | 1.053 +- 0.001 | 0.241 |
   | T = 0.5 | 0.587 | 0.695 (0.754) | 1.065 +- 0.001 (1.030) | 0.428 at +0.1 V, 0.442 at -0.1 V (0.428) |
   | T = 0.25 | 0.375 | 0.444 (0.505) | 1.057 +- 0.001 (1.014) | 0.613 at +0.1 V, 0.625 at -0.1 V (0.617) |

   The voltage divides as a series resistor says: the contact planes take
   the predicted share to 0.01. The conductance is 8 and 12 per cent
   below the series value. But the asymmetry does not fall with the
   voltage left on the device; it stays at the ideal-contact value or
   above. Each contact plane takes a larger share when the electrons
   enter through it than when they leave through it (at T = 0.25: the
   source plane 0.304 against 0.224, the drain plane 0.401 against
   0.308), and the two planes are not equivalent because the drain plane
   faces the neck.
7. **Why, as far as the runs show.** They do not identify a mechanism,
   and what follows is my reading. The device is shorter than the mean
   free path, so an electron reflected at a contact returns to the taper
   with its direction intact: device and contacts are one scatterer, and
   their resistances do not add. A contact that reflects all angles
   alike still absorbs a fixed share of the electrons the taper turns
   back, at whatever angle they return. The collimating contact reflects
   exactly those: an electron turned back by the sloping walls returns to
   the source at a wide angle, is sent back into the taper, and gets a
   second and third try at the neck. On that reading a geometric
   rectifier needs contacts that absorb at wide angles, and the contact
   resistance is the wrong number to characterise them by. No run tagged
   the electrons, so this is not tested.
8. **Tests and controls pass, after one failure that changed the plan.**
   T32a: with ideal contacts the new binary reproduces the old one in all
   51 columns on four short runs. T32b: the absorbed share at fixed
   angles follows both contact laws (largest deviation 2.7 standard
   errors). T32c: a clean strip conducts 0.600, 0.331 and 0.143 of the
   ideal value for T = 0.75, 0.5, 0.25 (targets 0.600, 0.333, 0.143) and
   0.331 and 0.140 for the two junctions (targets 0.325 and 0.140; the
   7 nm junction is 1.7 per cent, 3.6 standard errors, high). T32d, the
   funnel with reflecting contacts and no drive: **failed at 2 million
   particles** (a current of 2.3 standard errors at T = 0.25 and heating
   of 1.6 per cent behind the 38 nm junction) and passes at the 8 million
   used for every run here (+0.022 +- 0.016 and -0.011 +- 0.015 uA,
   heating 0.03 and 0.5 per cent); injection that is not thinned, the
   error the test is for, is caught by a 268 per cent rise in particle
   number. Controls of the production set: the mirror-symmetric funnel
   at T = 0.25 gives +0.010 +- 0.014 and -0.032 +- 0.018 uA; the funnel
   with no drive gives +0.014 +- 0.010 uA (38 nm junction) and
   +0.022 +- 0.010 uA (T = 0.25). All are below the plan's limit of
   0.1 uA. The no-drive value at T = 0.25 has now come out at +0.022 uA
   twice; it is under half the floor, and I take it as part of the floor.
9. **The reading** (details in `paper/refs_verification.md`). The best
   edge contacts to encapsulated graphene are about 150 ohm um for
   electrons at high density (Wang et al., Science 342, 614 (2013), main
   text read in full, supplement not accessible), a transparency of
   about 0.4 by my conversion: inside the range run here. Scalable
   contacts are 1 to 2 kilohm um (Pandey et al. 2018), a transparency of
   0.05 to 0.1: below the range run here. Cr-based edge contacts dope
   the graphene next to them n-type (Handschin et al. 2017), so an
   electron channel has no p-n junction in front of its contacts: the
   collimating contact is a sensitivity test, and would describe a hole
   channel. The junction formula was confirmed in Cheianov and Fal'ko's
   text. The reading also changed what is known about the measured
   device; see the last section.

## Limits

- **The gas heats, and the model has no way to cool it but the
  contacts.** There is no loss of energy to the lattice. Between ideal
  contacts the drive raises the mean kinetic energy by 2.5 per cent (the
  energy of a gas at rest at 328 K). Behind reflecting contacts it is
  3.2, 4.5 and 7.4 per cent for T = 0.75, 0.5 and 0.25 (337, 351 and
  381 K), steady over the measuring window and the same with a settling
  time of 3 and of 10 ps. Behind the collimating contacts it is 6.5 and
  14 per cent (372 and 447 K) **and still rising at the end of the run**
  (4.8 and 11.6 per cent in the first period of the window): those two
  rows are for a gas that has not reached a steady temperature. A real
  device also cools through phonons, which would reduce all of this.
  Whether the heating itself changes the rectified current was not
  tested.
- **The long-lead run does not isolate the contact.** I guessed that
  moving ideal contacts 200 nm further out would change nothing. It
  cuts the current to 0.54 and 0.67 of its value. But it also lengthens
  the device from 0.55 to 0.95 um, and Stage B7 found the response to
  depend on f L / v_F, which this raises from 0.55 to 0.95. The run
  cannot say how much of the loss is transit time and how much is the
  position of the contact; I should have seen that when planning it. It
  does show that the 0.12 A/W belongs to a device whose contacts sit
  100 nm from the taper.
- One transparency for both contacts, with mirror reflection of what is
  not absorbed. Contacts that scatter diffusely, that differ at source
  and drain, or with a transparency below 0.25 (ordinary contacts) were
  not run.
- Mirror walls throughout. Rough walls and imperfect contacts together
  were not run. Given that the contact does not act as a simple factor,
  I would not assume that the two losses multiply.
- The 7 nm junction is outside the stated validity of its formula
  (k_F d = 1.5); it is a model of angular filtering only.
- One mean free path, one frequency, one drive amplitude. Everything in
  the limits of Stage B8 other than the contact model still applies:
  classical trajectories, a uniform dielectric, a voltage source for the
  antenna.

## What this changes

The paper lists the ideal contacts as a limitation of its one positive
result. That can now be replaced by a result with a condition:

- The 0.12 A/W per absorbed watt holds between contacts as resistive as
  the best that have been measured (150 to 200 ohm um), and with a
  distant gate up to 340 ohm um, provided the contact absorbs electrons
  at all angles alike.
- It does not hold for a contact that passes only electrons near its
  normal: such contacts, conducting equally well, leave nothing above
  the floor.
- So the condition on the positive result is not a contact resistance
  but what the contact does with electrons arriving at wide angles,
  which no measurement found in the reading reports.

Changes to the paper, proposed here and **applied on 2026-11-16** after the go-ahead:
a short description of the two contact models in Sec. II C; a paragraph
and a figure panel in Sec. V E; the limitation in the discussion and the
condition in the abstract and conclusion reworded as above; supplement
entries for T32 and the table of point 1; four references (Wang 2013,
Cheianov and Fal'ko 2006, Handschin 2017, Pandey 2018).

## Part B: Device D with a computed field

Run on 2026-11-16 after the decision to run it before the paper is
revised. Runs: `tools/pic_ext.sh P7 P7n`, results `results/bpic/P7.csv`
(50 million particles, with potential maps `P7_V0.1.txt` and
`P7_V-0.1.txt`) and `P7n.csv` (25 million). Tables
`results/bpic/tables_B10b.md` and figure `results/bpic/B10_deviceD.png`
(`tools/deviced_results.py`).

The device: the 6 um funnel of Nguyen et al. as in every earlier stage
(leads 2 um wide, 2000 and 2420 nm long, 30 degree taper to a 175 nm
neck, abrupt shoulder), 1.4e12 per cm^2, 300 K, mean free path 700 nm,
mirror walls, ideal contacts, gate at 300 nm, static +-0.1 V, grid
7.5 nm, 15 ps of settling and a 15 ps window.

### What the reading changed before the run

The reading for this stage included the paper that reports the measured
asymmetry of 1.66 (Nguyen et al., npj 2D Mater. Appl. 5, 89 (2021), read
in full with its supplement; I read Fig. 5(c) and 5(d) myself).

- The 1.66 is a ratio of voltages at +-80 uA. Device D passes +100 uA at
  about +2.0 V and -100 uA at about -1.2 V, so the 1.66 is reached at
  about 1.5 V, not near the 0.1 V of our models.
- The device is 12 to 20 kilohm; the model's shape at the assumed
  density is 0.95 kilohm.
- At 0.1 V the measured asymmetry, read from Fig. 5(d), is about 1.03 to
  1.04 (+-0.02).
- The density of Device D is not given; the 1.4e12 per cm^2 used
  throughout is an upper bound.

So the decision bands the plan set for Part B (reproduce 1.66 or not)
did not apply. The run gives the model's asymmetry at 0.1 V to set
beside the measurement at the same voltage.

### Result

| | particles | I(+0.1 V) (uA) | I(-0.1 V) (uA) | asymmetry | resistance (kilohm) |
|---|---|---|---|---|---|
| computed field, 700 nm | 50 million | +108.69 +- 0.06 | -106.83 +- 0.06 | **1.0174 +- 0.0008** | 0.928 |
| computed field, 700 nm | 25 million | +111.41 +- 0.11 | -109.53 +- 0.09 | 1.0172 +- 0.0014 | 0.905 |
| imposed uniform field, no scattering (Stage B2) | | | | 1.62 | |
| imposed uniform field, 700 nm (Stage B7) | | | | 1.09 | |
| imposed drop on 500 nm at the neck, 700 nm (Stage B7) | | | | 0.96 | |
| my guess before the run | | | | 1.01 to 1.05 | |
| measured at 0.1 V | | | | 1.03 to 1.04 (+-0.02) | 12 to 20 |

1. **With its own field Device D has an asymmetry of 1.017 at 0.1 V.**
   The forward current exceeds the reverse by 1.86 +- 0.09 uA in 108 uA.
   It is inside my guess. The excess over 1 is a fifth of what the
   uniform field gave at the same mean free path (0.09) and a thirty-fifth
   of the first model's (0.62).
2. **The field is neither of the two that were imposed, and the
   asymmetry lies between theirs.** 55 % of the bias falls on the 500 nm
   centred on the neck, where a uniform field puts 8 %; the two leads
   take 9 % and 15 % (uniform: 33 % and 36 %), the taper up to 250 nm
   before the neck 15 % (22 %), and 3 % falls within 22 nm of each
   contact plane. The uniform field gave 1.09 and the drop confined to
   the neck 0.96; the computed field gives 1.017.
3. **It agrees with the measurement at the same voltage, and that is not
   a validation.** 1.017 is within one reading error (0.02) of the
   measured 1.03 to 1.04. But the measured device is 13 to 22 times more resistive than
   the model, so most of its bias falls on something the model does not
   contain (contacts, a lower density, disorder, or all three), and an
   asymmetry this close to 1 is what almost any model would give. The
   agreement shows no conflict; it supports nothing further.
4. **The model has nothing to say about the 1.66.** That number is
   reached at about 1.5 V. At 0.1 V the bias is already 0.7 of the Fermi
   energy, and the model has no optical phonons, no interband processes
   and no loss of heat to the lattice, so it was not run higher.
5. **The asymmetry is converged in particle number; the conductance is
   not.** The two particle numbers agree to 0.0002 in the asymmetry (the
   plan's limit was 0.03) and in the forward-minus-reverse current (1.86
   +- 0.09 against 1.88 +- 0.15 uA). The conductance differs by 2.5 %
   (1.078 against 1.105 mS), thirty times its statistical error, with the
   cause not identified. If the error falls as one over the particle
   number the limit is 1.05 mS (0.95 kilohm); two points cannot test
   that. The resistance is therefore quoted as 0.93 kilohm with an
   uncertainty of a few per cent.
6. **Steadiness.** The source-plane and drain-plane currents agree to
   0.06 uA and the standard error of the current over the 15 ps window
   is 0.06 %. The
   gas is still heating: the mean kinetic energy is 3.5 % above its
   starting value in the first picosecond of the window and 4.5 % in the
   last (340 to 351 K as a gas at rest), the same in all four runs. No
   particle was lost through a wall; the particle number stayed within
   0.02 %.

### What follows for the paper (applied on 2026-11-16)

- Sec. III A calls the first model's 1.62 at 0.1 V "close to the 1.66
  measured at 80 uA". That sets a model at 0.1 V against a measurement at
  1.5 V on a device fifteen times more resistive. The sentence has to be
  rewritten, and Sec. II A should give the voltage with the 1.66.
- The paper can now say what its best model gives for the one measured
  device: 1.017 at 0.1 V, beside a measured 1.03 to 1.04, with the
  caveat of point 3; and that the first model's 1.62 came from its
  imposed field and absent scattering, as for the short funnel.
- A panel or a supplement figure from `B10_deviceD.png`.

### Not run

The pair with no scattering (stage P7x, about 1.6 hours) was conditional
in the plan on an outcome that no longer exists. It would say how much
of the fall from 1.62 to 1.017 is the field and how much the scattering.
For the short funnel the answer was the field (1.83 to 1.09 with no
scattering). It is not needed for any statement above.

## Cost

Part A: 25 production runs, 3.1 hours of wall time on 10 cores (21
planned, 4 second seeds); tests and pilot about an hour more. Part B: 4
runs, 2.5 hours.
