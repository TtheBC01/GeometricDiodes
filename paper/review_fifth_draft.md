# Review of the fifth draft (2026-11-16)

Draft reviewed: `paper/main.tex`, `paper/sections/*.tex`, `paper/supplement.tex` at commit 704b2d6.
Nothing in the paper was changed by this review.

## How it was done

Three reads, then a check of each finding against the sources.

- A referee read of the whole manuscript and all figures by an independent
  agent that was not told what I thought the weak points were.
- An audit of every repeated number, derived statement, cross-reference and
  caption against the text and against `results/`, by a second independent
  agent.
- My own read of the abstract, introduction, Secs. III A and V, the
  discussion and the conclusion.

Marks used below: **[V]** I checked the finding against the text, a result
file or by arithmetic and it holds. **[A]** reported by the audit with file
and line on both sides, not rechecked by me. **[S]** a suspicion, not
established.

Verdict a referee would probably give: **major revision**. The results are
not in question in most places; the abstract and conclusion state several of
them more strongly than the body does, one equation of the fluid model may
be missing a term, and about twenty numbers disagree between places.

---

## A. Claims the body does not support

### A1. The length limit for the 6 um funnel [V]

- `main.tex:71-74` "limits the 6 um funnel ... to tens of gigahertz";
  `conclusion.tex:25-26` "too long by a factor of fifteen at 1 THz";
  `discussion.tex:43-45`; verdict table row "length: fL/vF = 6".
- This rests on the uniform imposed field. Sec. III A now computes the
  static field of this device and finds 55 % of the bias on the 500 nm at
  the neck (`dc.tex:33-34`). Sec. V B says that with a 500 nm drop the same
  device gives 0.24 A/W at 1 THz at 700 nm, 0.75 A/W per absorbed watt
  (`ballistic.tex:114-127`).
- `ballistic.tex:128-129` still says "the terahertz response is decided by
  electrostatics that the model does not contain", and `ballistic.tex:83-85`
  "With the field model in which the dc asymmetry was established". Both are
  out of date since the device D run.
- The only computed-field test of the roll-off is G3, whose leads are
  100 nm, so the length between contacts and the length of the field region
  cannot be told apart there.
- "The transit roll-off ... survives the computation of the field"
  (`ballistic.tex:334`) holds for the current under the 300 nm gate. Per
  absorbed watt the response rises: 0.19 uA of 0.8 uW at 3 THz is 0.24 A/W,
  twice the 1 THz headline (`ballistic.tex:338-340`, visible in Fig. 13(a)).
  Under the 30 nm gate the current is 0.50 uA at 3 THz against 0.48 at
  1 THz (`ballistic.tex:355`).
- What would settle it: device D at 1 THz with the field computed. The
  quasi-static rectified current implied by Eq. (11) is about 0.47 uA; the
  statistical error of the static runs was 0.06 uA. Otherwise the claim has
  to be restated as holding for a uniform field only, everywhere it appears.
- [S] The ac field need not resemble the static one. No ac potential
  profile is shown for any device.

### A2. "In no regime does the conversion efficiency reach one per cent" [V]

- `main.tex:68-69, 91`; `intro.tex:133-134`; `conclusion.tex:19-20, 52`;
  `plasmonic.tex:110-111`.
- `plasmonic.tex:183-184`: "A matched antenna in place of 75 Ohm gives at
  best 1.3 % at 100 K". The blanket statement is true only for the 75 Ohm
  antenna (0.03 to 0.9 %).
- The "peak" is not a computed peak. Appendix B defines it as the value at
  0.7 V_u, the largest amplitude simulated (`appendix.tex:47-51`). At 300 K
  the response is still above the square law there (`plasmonic.tex:122-123`).
- The grid is evaluated at 0.3 THz only.
- "For every carrier density" holds inside E_F >= 3 k_B T; the prefactor
  rises toward that edge (`plasmonic.tex:182-183`).
- The ballistic 1e-4 is the last point computed, where the current is 1.5
  times the square law and unexplained (`ballistic.tex:537-544`). It is not
  a ceiling.
- Supportable: "of order one per cent or below within the degenerate window
  and up to 0.7 of the gate overdrive".

### A3. "Nearly all of it the Dyakonov-Shur asymmetry of its terminations" [V]

- `main.tex:65-66`; `discussion.tex:26-28` "as it would in a plain channel
  of the same impedance".
- No plain channel with the DS termination and the antenna is reported. The
  uniform channel in `results/THZ.md` is the closed-form benchmark with an
  ideal source.
- At 100 K the shape-only response of the 1 um funnel is 0.08 to 0.13 A/W
  from 0.3 to 1 THz against 0.21 to 0.34 A/W for DS (Fig. 6(b),
  `plasmonic.tex:23-25`). That is a third to a half, not "nearly nothing".
- The 1 um funnel has straight leads 12 um long on each side
  (`apps/thz_geo.hpp`), about 27 um in all. The manuscript does not say so
  (`models.tex:23-26`).
- The short-lead control in `results/THZ.md:167-170` gives a different
  split: at 300 K shape-only -0.072 A/W against DS 0.14 at 1 THz, and at
  100 K a DS term of -0.036 A/W at 1 THz. The paper cites this control only
  for the resonance frequency (`plasmonic.tex:44-45`).
- [S] At 300 K the plasmon decay length is shorter than the leads, so "the
  shape adds nothing at 300 K" may be a statement about lead length.

### A4. Rough edges: "vanishes at 0.2", "keeps nothing" [V]

- `main.tex:80-83`; `ballistic.tex:452-453` "an edge with p0 of 0.2 or less
  keeps nothing"; `discussion.tex:68`; `conclusion.tex:35-36`; verdict table.
- The current passes through zero and reverses. At p0 = 0.05 it is
  -0.10 +- 0.02 and -0.18 +- 0.02 uA (`ballistic.tex:414-418`), two to
  nearly four times the floor and a quarter to a third of the specular
  magnitude. A detector does not care about the sign.
- No computed-field point lies below p0 = 0.05 or, for one specularity,
  below p = 0.5. The mechanism of the reversed current is not identified.
- The measured values are read as head-on values at the device's k_F
  without rescaling to the density of each experiment, and the 0.8 end of
  the range is a twisted bilayer (`ballistic.tex:443-445`).
- The design rule quotes rms roughness of 2 and 3 nm
  (`discussion.tex:260-261`) while the limitations say no measured rms
  roughness of an etched edge was found (`discussion.tex:318-319`).

### A5. The Tesla valve statements [V]

- "does not rectify ballistically" (`main.tex:57-58`): the body reports
  A = 0.89 +- 0.01 at 0.1 V in the clean specular limit (`dc.tex:68-69`),
  as far from one as the funnel's 1.09 at 700 nm.
- "rectifies a steady viscous flow only at Reynolds numbers above the
  drift-velocity limit" (`main.tex:58-60`) against "Inside the admissible
  window the diodicity is 1e-3 to 3e-2" (`dc.tex:163`).
- "adds 1e-5 to 1e-3 A/W at terahertz frequencies" (`main.tex:60`,
  `intro.tex:102-103`): the body gives that range below 300 GHz and 2e-2 at
  a resonance near 0.7 THz (`plasmonic.tex:50-51`).
- Verdict table, ballistic 1 THz: "none (A = 1 at dc)". The ballistic valve
  was never run at terahertz frequencies, and the paper itself shows a
  funnel with static A = 1.01 giving 0.45 A/W at 1 THz
  (`ballistic.tex:116-119`).
- The ballistic null has standard errors of 0.01 to 0.05 (`dc.tex:65-66`),
  coarser than the 0.017 the paper reports for the funnel with the field
  computed.

### A6. "A square-law detector at the level of the plasmonic element" [V]

- `main.tex:91-94`. 0.12 A/W is per absorbed watt; the plasmonic 0.07 to
  0.34 A/W is per available watt. One funnel on the antenna gives 0.012 to
  0.026 A/W. Parity needs nineteen devices with their reactance tuned out,
  which was not modelled. The body says all this; the abstract does not.
- 0.12 A/W is at 0.1 V, 0.7 E_F/e. A fit of a V^2 and a V^4 term through the
  0.1 and 0.2 V points reproduces the 0.05 V point and gives a small-signal
  value near 0.10 A/W.
- The figure is for 100 nm leads; with 300 nm leads the current is 0.54 and
  0.67 of it (`ballistic.tex:531-533`).

### A7. Smaller statements [V]

- `main.tex:77-78`: "a sixth ... because a quarter of the bias drops at the
  contacts". A quarter lost accounts for a factor 0.56. The body and the
  conclusion give the full reason (`ballistic.tex:315-327`,
  `conclusion.tex:32-34`).
- "A sixth" compares 0.124 A/W by the definition of Eq. (18) with 0.70 by
  that of Eq. (16). In one definition it is 0.10 against 0.70, a seventh,
  and `ballistic.tex:329-330` says "a seventh to a ninth".
- `main.tex:88-91`, `conclusion.tex:39-41`: under the 30 nm gate the strip
  "rectifies more than the funnel shape". True at 1 THz (1.15 against
  0.48 uA), not at 0.3 THz (0.67 against 0.89 uA).
- `intro.tex:99`: "separates cleanly into shape and circuit". Under the
  30 nm gate the funnel with its drain tied to the gate gives 1.01 uA, less
  than the strip's 1.15 (`ballistic.tex:357-359`); the two terms do not add.
- `ballistic.tex:119-121`: "robust against edge roughness and bulk
  scattering" (imposed field, 6 um) is never withdrawn after
  `ballistic.tex:387-389` finds the opposite with the field computed.

---

## B. Physics a referee would press

### B1. The momentum equation of the Dirac fluid [V for the algebra; literature not checked]

- `models.tex:236-241`, Eq. (9): the Galilean convective term with a
  density-dependent mass in the force only.
- If the conserved momentum density is n m*(n) u with m* proportional to
  n^(1/2), momentum conservation and continuity give, to second order,
  `d_t u + (u.grad)u - (1/2) u (div u) = -grad h`. I rederived this; in one
  dimension it halves the convective term.
- The missing term is of the same order as the one that rectifies. It would
  change the Dirac-closure numbers (the 40 % and factor two of
  `plasmonic.tex:12-14`, the 100 K responsivities) by an amount of order
  one, not by an order of magnitude. The shallow-water benchmark and the
  gradual-channel polarity term of Eq. (13) should not be affected.
- `plans/hydro_thz_notes.md:15-22` shows the term was not considered, and
  `paper/refs_verification.md:88` records that Svintsov 2013 notes the loss
  of Galilean invariance.
- Needed: check the form used by Tomadin and Polini (2013) and Svintsov
  (2013), then either rerun the plasmonic points with the term or bound it.

### B2. The fluid model outside the hydrodynamic regime [V]

- Table I gives tau_ee = 1 ps at 100 K, a mean free path of 0.4 to 1 um for
  electron-electron collisions, against a 175 nm channel and a 1 um neck.
  The manuscript states no Knudsen number.
- The same 175 nm funnel is ballistic in Secs. III A and V and a viscous
  fluid in Secs. III B and IV.
- `discussion.tex:343-344`: "quantitative to about 0.3 THz at 100 K and
  1 THz at 300 K, where omega tau_ee = 1". With Table I that is 0.16 and
  1.6 THz. Every 100 K headline (0.34 A/W, the taper resonance at 1.87 THz,
  17 pW/Hz^1/2) lies above omega tau_ee = 1.

### B3. The density of the particle model is not the stated one [V]

- The runs hold the chemical potential at 0.138 eV at 300 K, which is
  1.56e12 cm^-2 (`results/bpic/P2.csv`: 0.01561 per nm^2), where
  `models.tex:67`, Table I and `discussion.tex:332` say 1.4e12.
- `models.tex:96` "about 2900" electrons: the run has 3182.
- `ballistic.tex:31`: "(ge/h)Phi is 1.43 mA" is the zero-temperature value;
  the runs use 1.59 mA (Phi = 10.275 eV in `results/bshort/S4.csv`).

### B4. Electron-electron collisions at room temperature [V]

- Table I gives a collision length of 100 nm at 300 K, a fifth of the
  0.55 um funnel. They are represented by a momentum-relaxing mean free
  path of 100 nm, called "a rough bound" (`ballistic.tex:244-245`).
  Collisions that conserve momentum randomise direction without relaxing
  the current, and the run is not shown to bound their effect. The abstract
  says "at the room-temperature mean free path" without this.

### B5. The numerical state of the particle model [V]

- Collimating-contact runs carry an abstract-level claim and are single
  runs that are not steady: "still rising after 20 periods"
  (`ballistic.tex:528`).
- The mirror control has chi^2 = 14.4 on six runs, "so the batch errors are
  too small by about 1.5" (`supplement.tex:198-200`). Single-run errors in
  the text carry that factor.
- Device D: Eq. (11) quotes +-0.0008, statistical only. One grid (7.5 nm,
  where the supplement asks for 7.5 nm resolution "or better"), a 2.5 %
  conductance difference between particle numbers, a gas still warming.
- T24 could not be met at 3 % and was split and loosened to 10 %; T26
  passes at 2.5 standard errors where two were required
  (`supplement.tex:140-147`). Disclosed, and "validated" in the abstract
  will be read against it.

### B6. Results reported without an explanation [V]

- The strip's current of -1.77 uA at 3 THz under the 30 nm gate, the
  largest number in the data set (`ballistic.tex:361-363`).
- The current at 0.2 V, 1.5 times the square law, which supplies the
  efficiency figure.
- The reversed current with diffuse or very rough walls.

### B7. Disclosed limits a referee will still raise

- Ray optics at a neck of five to seven modes, with the drain metal one
  neck width from the aperture.
- No experimental anchor: the 5 % of Geurs et al. is missed by three orders
  of magnitude, the 28 THz response is not reproduced, device D is 13 to 22
  times too conductive, and the one agreement is declared not a validation.

---

## C. Numbers that disagree

| # | Where | What | Status |
|---|---|---|---|
| C1 | `conclusion.tex:54-56` | 60 pW/Hz^1/2 paired with 100 Ohm um contacts; 60 is for ideal contacts, 84 with 100 Ohm um (`results/THZ.md:567-569`) | [V] |
| C2 | `main.tex:79`, `discussion.tex:64-65, 112`, `conclusion.tex:32` | "0.10 A/W at 100 nm" is the 30 nm-gate value; under the 300 nm gate of the headline it is 0.107 | [V] |
| C3 | `ballistic.tex:553-554` | "0.10 to 0.13 A/W" mixes the two gates | [V] |
| C4 | `ballistic.tex:508-509` | "9 to 14 standard errors": from the pooled values quoted beside it, 11 to 17; 9 to 14 is first seeds only | [V] |
| C5 | `dc.tex:8, 10` against Table II | 1.62 and 1.09 against 1.61 and 1.10 (also 1.602 and 1.622 in the supplement) | [V] |
| C6 | `ballistic.tex:167-168` | "one neck width long in every device": the G0 points use the 500 nm drop | [V] |
| C7 | `ballistic.tex:165-167` | "22 control points" from "G2 to G4": the 22 include G1 | [V] |
| C8 | `models.tex:174` | "Thirteen tests": Table S4 has fifteen rows, T22 to T32 | [V] |
| C9 | Fig. 11(f) | imposed-field points 0.96, 0.87, 0.71, 0.74, 0.43 against 0.95, 0.83, 0.70, 0.71, 0.42 in the text; `tools/pic_common.py:160` divides by a different conductance | [V] |
| C10 | Fig. 13(b), caption | says the detectors are those of Table IV; four plotted points are not in it (Ludwig at 1 THz, Cai 2014, Asgari 2021, VDI WR0.8 at 203 pW), and Asgari is not cited | [V] |
| C11 | `discussion.tex:182-183`, Table IV | commercial "5 to 27 pW": the plotted set runs 12 to 203 | [V] |
| C12 | `ballistic.tex:51` against `plasmonic.tex:19` | 87 to 100 % against 80 to 95 % absorbed; different terminations, neither named | [A] |
| C13 | `plasmonic.tex:21` against `ballistic.tex:555` | 0.15 to 0.34 against 0.21 to 0.34 A/W at 100 K; the second is the 0.3 to 1 THz window, unstated | [A] |
| C14 | `discussion.tex:11-12, 21-23, 106-107` | "less than 1e-3", "1e-3 to 3e-2" and "a per cent or less" for the steady-flow shape term, at different parameters that are not stated | [A] |
| C15 | `dc.tex:141-142` | "every geometry ... below 3e-5" up to Re = 0.3: the funnel is at 1.4e-4 (`results/HYDRO.md:109-110`) | [A] |
| C16 | `plasmonic.tex:51-52` | Tesla "ten to twenty times below the funnel at every frequency": the ratio runs from 1 to over 1000 | [A] |
| C17 | `plasmonic.tex:164`, `discussion.tex:233-235` | "pure number r^2 R_ant/R_dc" without the factor 2 of Eq. (14); without it the value is not "of order 1e-2" | [A] |
| C18 | `ballistic.tex:174-175` | 0.4 v_F/L for 6 um is 67 GHz, quoted as 60 | [V] |
| C19 | `ballistic.tex:313`, `494` | ratios 0.70 and 0.44 use 0.845 mS, where the text quotes 0.86 mS | [A] |
| C20 | `supplement.tex:198` | weighted mean +0.008 +- 0.023: the weighted error is 0.010 | [A] |
| C21 | `supplement.tex:327-329` | the fold near Re = 85 belongs to the side-loop valve; the classical valve reaches Re = 100 in `dc.tex:119` | [A] |
| C22 | Table I | 300 K viscosity appears as 0.02, 0.025 (the stated estimate) and 0.03 m^2/s | [A] |
| C23 | `plasmonic.tex:54`, `:39` | absorbed fractions 3 to 50 % and 15 to 80 % are the six tabulated frequencies; the full sweeps reach 97 and 99 % | [A] |
| C24 | `dc.tex:72-73` | Tesla resistance 1.4 to 22 kOhm omits the one-stage valve at 1.07 kOhm | [A] |
| C25 | Table S4 | T32a is in the results and not in the table | [A] |
| C26 | Table III, row 1 | ranges cover the uniform field only; the text has 1.40 to 1.52 and 0.94 to 0.97 for confined drops | [A] |
| C27 | `ballistic.tex:6-7` | G2 = 3.8 mA/V^2 gives 19 uA by the stated formula; 18 includes the quartic term | [A] |

Cross-references: all hard-coded figure, equation and table numbers in the
supplement are right against the built PDF. Every cited key exists; ten bib
entries are never cited. `check_tex.py` reports 0 problems.

---

## D. Presentation

- **Abstract**: 531 words and about thirty numbers. The main result cannot
  be recovered from it.
- **The results are stated six or seven times**: abstract, introduction
  (`intro.tex:99-134`), the summary at `ballistic.tex:551-562`, discussion
  text, verdict table, designer's paragraph, conclusion. Each copy has
  drifted (items C2, C13, A7, and the two different lists of "conditions"
  in `intro.tex:110-128` and `discussion.tex:62-85`).
- **Sec. V is told in order of discovery** (6000 words). Three intermediate
  verdicts are later overturned ("Taken at face value", "suggests ... the
  field does not matter", "robust against"). Sec. V E alone is 3000 words
  without a subheading, and its three figures follow the text.
- **Lists of numbers in prose** that duplicate a figure, e.g.
  `ballistic.tex:412-431`. They belong in a table.
- **"Shortest funnel"** means G3 throughout; G4 is shorter.
- **Symbols with several meanings**: h (Planck, roughness, h(n), mesh), T
  (temperature, transmission, transparency), s (plasma speed, voltage
  share), N, G, g, p, Phi. V_u is used in Eq. (13) and defined two sections
  later; device G2 and the 500 nm profile are used in Sec. III A before
  they are introduced.
- **R_abs has two definitions**, which Table II, Fig. 11(f) and the "sixth"
  mix. Fig. 13(a) plots a per-absorbed-watt curve on an axis labelled
  "75 Ohm source".
- **Figures**: Fig. 6(a) samples the resonances too sparsely for the
  connecting line and stops at 1.3 THz where the text says 3 THz; Fig. 4(b)
  and Fig. 13(a) are crowded; Fig. 11(c) hides the zero at 3 THz that the
  text relies on; Fig. 11(g) is cited for currents and plots responsivity;
  Fig. 12 caption says red, the squares are orange; Fig. 5(c) uses filled
  and open markers differently from its legend.
- **Likely requests to cut**: most of Secs. V A, V B and V D now that V E
  supersedes them; the three side-loop valves; Sec. III C down to a
  paragraph and the closed form; the designer's paragraph; table entries
  read from an abstract only.

## E. Journal

The paper is set for Physical Review Applied.

- No length limit; length should fit the content.
- A data availability statement is required. The repository is named in the
  acknowledgments and the commit is "to be tagged at submission".
- Appendices are preferred to a separate supplement.
- Still open: affiliation, the unread sources (Soffer 1967, the supplement
  of Wang et al. 2013, the two-page Stearns 2020 paper).

---

## What each item costs

**Text only** (no new runs): A2, A4, A5, A6, A7, B2 (state the regime and
correct the frequencies), B3 (state the simulated density), B5 and B6 as
disclosures, all of C except C9 and C10 (figure scripts), all of D.

**A literature check first**: B1.

**New runs, each needing a plan**:
- device D at 1 THz with the field computed (A1);
- a plain channel with the DS termination and the antenna, and the 1 um
  funnel with short leads (A3);
- the plasmonic points with the corrected momentum equation, if B1 stands;
- steady, repeated collimating-contact runs and a grid check for device D
  (B5);
- rough-edge points below p0 = 0.05 and efficiency beyond 0.7 V_u, if those
  claims are to be kept in their present strength (A2, A4).

---

## Status after the text corrections (2026-11-17, sixth draft)

Text only; no runs, no figure was regenerated. Both PDFs rebuilt
(`main.pdf` 32 pages, `supplement.pdf` 7 pages), `check_tex.py` 0 problems.

**Corrected in the text**

- A1: restated everywhere as holding for a field spread over the device;
  the computed static field of device D and the missing terahertz run are
  named in the abstract, introduction, Secs. V A and V B, the discussion,
  the verdict table, the limitations and the conclusion. The two
  out-of-date sentences in Sec. V are replaced. The rise of the response
  per absorbed watt at 3 THz is stated.
- A2: "about one per cent or less, up to the amplitudes computed"
  everywhere; "peak" is defined as the value at 0.7 V_u at 0.3 THz; the
  1.3 % with a matched antenna is in the discussion; the ballistic 1e-4 is
  said not to be a ceiling.
- A3: "mainly" the terminations, with the shape-only share (a third to a
  half at 100 K, a few per cent at 300 K); the 12 um leads are stated; the
  short-lead control at 1 THz and the missing plain-channel control are
  named.
- A4: zero "near 0.2" and a reversed current of 0.24 to 0.38 of the
  specular magnitude at 0.05; the density rescaling and the twisted
  bilayer are added to the weaknesses; the 2 and 3 nm are tied to Eq. (3).
- A5: the three Tesla statements, the subsection title and the verdict
  table ("not computed" for the ballistic valve at 1 THz); the coarser
  errors of the Tesla runs are stated.
- A6: the array of about nineteen devices and the small-signal value of
  0.10 A/W are stated.
- A7: all five.
- B2: 0.16 and 1.6 THz; the electron-electron mean free path against the
  channel width is stated in Table I and the limitations.
- B3: chemical potential 0.138 eV, density 1.56e12 at 300 K, about 3200
  electrons, 1.59 mA; k_F defined as E_F/(hbar v_F).
- B4, B5, B6: stated as limits (indication and not a bound; batch errors
  too small by about 1.5; single collimating runs; three unexplained
  results).
- C1 to C8, C11 to C27. C9 and C10 were handled in the text: the caption of
  Fig. 11(f) says which conductance its imposed-field points use, and
  Table IV gained the plotted detectors that were missing (Cai 2014,
  Asgari 2021, the 1 THz value of Ludwig 2024, the 0.9 to 1.4 THz
  commercial band).
- D: abstract cut from 531 to about 330 words; "shortest funnel" replaced;
  V_u defined at Eq. (13); the voltage share renamed beta; captions of
  Figs. 8(b), 11(f), 12 and 13(a).

**Not done**

- B1 (the momentum equation): untouched, pending the literature check.
- D: Sec. V is still told in order of discovery and Sec. V E has no
  subheadings; no cuts were made; the number lists in prose remain; the
  symbols h, T, N, G, g, p and Phi still carry several meanings; the
  figures themselves are unchanged (Figs. 4(b), 5(c), 6(a), 11(c), 13(a)).
- E: data availability statement, affiliation, tagged commit, the three
  unread sources.
- Everything listed under "New runs" above.

---

## B1 checked against the cited papers (2026-11-17)

**Result: B1 stands, and is larger than first estimated.** Nothing in the
paper or the code was changed by this check.

What the three forms are, in one dimension, to second order in the drift
velocity, written as `d_t u + alpha u d_x u = -d_x h(n)`:

| Source | alpha | How it is stated there |
|---|---|---|
| The manuscript, Eq. (9) (`thz_solver.cpp:229`) | 1 | Galilean convective term |
| Tomadin and Polini 2013, Eq. (6) (arXiv:1307.0734) | 1/2 | keeps a term `-(v/2n) d_t n`, from the time derivative of the pressure; their Eq. (13) drops it and is called "a crude approximation" there. Their instability rate is 3/4 of the 2DEG value, which is alpha = 1/2 |
| Svintsov 2013, Eq. (19) (arXiv:1310.3963) | 3 - 4 xi, which is 0 for degenerate electrons | "in the degenerate electron system this term vanishes"; wave speeds with a drift u0 are u0/2 +- s0, Eq. (39) |

- The manuscript cites Tomadin and Polini for the density-dependent mass
  and uses the form they call crude. The two cited papers also disagree
  with each other.
- Own check [V]: the moments of a drifting degenerate Dirac distribution,
  integrated numerically at fixed laboratory density, give a momentum
  density `n m(n) u` and a momentum flux
  `Pi_xx = P0(n) + (3/4) n m u^2`, `Pi_yy = P0(n) - (1/4) n m u^2`
  (a Galilean fluid has 1 and 0). With continuity this gives alpha = 0 in
  one dimension, as Svintsov finds. Tomadin and Polini take `P = eps_F n/3`
  without the correction of relative order `u^2/v_F^2`, whose gradient is of
  the same order as the convective term; that accounts for their 1/2.
- In two dimensions the consistent second-order equation is
  `d_t u + (u.grad)u - (1/2) u (div u) - (1/4) grad(u^2) = -grad h(n)`.
  The solver has the first two terms on the left only.
- Read through a summarising fetch of the ar5iv full text of both papers,
  with the equations quoted back; I did not read the PDFs page by page.

**Size of the effect** [V for the uniform channel]. For the benchmark of
Fig. 6(a) (uniform channel, ideal source, open drain, no viscosity) the dc
voltage has the closed form
`dV = (Phi'' - h'')/4 (|n1(L)|^2 - |n1(0)|^2) + (alpha/4)|u1(0)|^2 + (Gamma/2) int Re(n1 u1*) dx`.
With alpha = 1 it reproduces the solver's own table
(`results/thz/tables.md`, S1: 0.012, 0.158 at 100 K; 0.016, 0.150, 0.386,
0.625 at 300 K). Relative to that:

| | alpha = 1/2 | alpha = 0 |
|---|---|---|
| 100 K, 10 GHz to 3 THz | 0.67 to 0.76 | 0.34 to 0.52 |
| 300 K, 10 GHz to 3 THz | 0.77 to 0.80 | 0.53 to 0.60 |

So the Dyakonov-Shur response of a uniform channel is lower than the
manuscript's by a factor of 1.3 to 1.5 in the Tomadin-Polini form and 1.7
to 3 in the consistent one. The "40 % higher near resonance" of
Sec. IV A becomes "half" (0.5 of the shallow-water value at resonance
instead of 1.4). The efficiency goes as the square.

**What is and is not affected**

- Affected, needs the solver changed and the runs repeated: all of
  Sec. IV (small signal, large signal, operating-point grid, contacts,
  NEP), Figs. 6, 7, 8 and 13, the plasmonic rows of Tables III and IV, and
  every comparison of the ballistic 0.12 A/W with the plasmonic 0.09 A/W.
  The split between shape and termination for the funnel cannot be
  estimated in one dimension.
- Not affected: the shallow-water benchmark; the steady viscous results of
  Sec. III B (incompressible, and the extra terms are a gradient that
  cancels between contacts of equal width); the polarity term of
  Eq. (13), which has no inertia; all ballistic results.
- Direction: the plasmonic responsivities fall, the efficiency bound gets
  stronger, and the ballistic funnel moves from parity with the plasmonic
  element at room temperature to above it per absorbed watt.
- Open in the model: the boundary condition of a contact on a drifting
  Dirac fluid (the chemical potential at fixed density also depends on the
  drift; a 4 % effect at the reference point through the quantum
  capacitance), and the fact that above omega tau_ee = 1 the distribution
  is not a drifted Fermi function at all (B2).

The estimate script is not in the repository.

## Plan for B1, and a second finding (2026-11-17)

Plan: `plans/hydro_momentum.md`, figure `plans/hydro_momentum.png`
(Stage H6; not yet approved, nothing run).

Found while preparing it [V for a one-dimensional stand-in only]: the wave
speed at the reference point is 3.57 v_F, so the large-signal runs of
Sec. IV B drive the carriers past 0.3 v_F at an antenna amplitude of 0.07
to 0.18 of the gate overdrive and past v_F at 0.23 to 0.60. The top one to
three of the eight amplitudes (0.01 to 0.7) are outside any fluid model
without a velocity limit, and "a square law up to the gate overdrive" and
the efficiency quoted at 0.7 of the overdrive are outside the model. The
steady-flow sections apply a 0.3 v_F limit; the large-signal section does
not. The plan proposes the ideal-fluid equation exact in the drift speed
for the time-domain and steady solvers.

## B1 carried out (2026-11-18, Stage H6)

The plan was approved and carried out: `results/HYDRO_MOMENTUM.md`, tables
`results/thz2/tables_H6.md`, figure `results/thz2/H6_before_after.png`.
The solvers now have the momentum equation of the degenerate Dirac gas
(exact in the drift speed in the time-domain and steady solver), every
check passes, and every fluid run was repeated into `results/thz2` and
`results/dcgate2`. The paper still carries the earlier numbers; the list
of what would change is the last section but one of the results file.
Headline: the funnels' Dyakonov-Shur responsivities fall to 0.6 to 1.0 of
their earlier values (not to the 0.3 to 0.6 estimated above from the
uniform channel); the large-signal results of Sec. IV B are replaced by a
square law up to 0.3 v_F and an efficiency below 0.1 % everywhere; the
contact condition and the finite temperature are each a 20 to 35 %
uncertainty of the fluid numbers.

## Seventh draft (2026-11-19)

The paper now carries the corrected fluid results (B1 closed): Eq. (8),
Appendices A and B, Sec. III C, Sec. IV, Figs. 5 to 8, 10, 11, 13,
Tables III and IV, abstract, introduction, discussion, conclusion,
supplement. Details at the end of `results/HYDRO_MOMENTUM.md`. Still open
from this review: device D at 1 THz with a computed field; the Sec. V
restructure and cuts; the symbol overloads; data availability statement,
affiliation, tagged commit; the unread sources.
