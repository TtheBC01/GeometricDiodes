# Review of the ninth draft (2026-11-21)

Two independent reads of the draft committed as `8315239`, neither shown
the earlier reviews:

- a **referee read** of the main text and all thirteen figures, with the
  supplement, the results files and the earlier reviews withheld;
- a **number check**: every quantity that appears in more than one place,
  the fluid numbers against the generated tables
  (`results/thz2/paper_numbers.md`, `h7/tables.md`, `h7/ft_coeffs.md`,
  `drift_limit.md`), and every hard-coded cross-reference.

I then checked their findings against the source lines and the run data.
**[V]** marks a finding I verified myself; the others I have read and
judged but not recomputed. Nothing in the paper has been changed.

## Summary

The arithmetic is sound: one real disagreement with the tables, which I
introduced on 2026-11-21, and no wrong cross-reference. The defects are
of three other kinds.

1. **Eleven statements are wrong as written** (section 1). Two are in
   the abstract.
2. **The summaries claim more than the body shows** (section 2). The
   body qualifies its results carefully; the abstract, introduction and
   conclusion drop the qualifications. The four that matter: the verdict
   on the Tesla valve, the length criterion, the 100 K numbers, and the
   300 K split between shape and circuit.
3. **The paper is too long and repeats itself** (section 4): 37 pages,
   about 27 000 words, a 400-word abstract with 25 numbers, and each
   caveat stated between three and seven times.

My statement before this review, that only editorial work was left, was
too optimistic. Sections 1 and 4 are editorial. Section 2 is not: it
needs either narrower claims or two to four more runs (section 5).

## 1. Wrong as written

| # | Where | What it says | What is true |
|---|---|---|---|
| 1 **[V]** | `supplement.tex:595`, `appendix.tex:136` | time-domain drift per amplitude 3.58, 2.96, 1.73 v_F/V_u; the solvers "agree on the drift to 1 % ... and 5 % at 1 THz" | 3.58, 2.93, 1.67 at small amplitude (I took two of the three at an amplitude of 0.1). The agreement is 0.6, 1.7 and 1.8 % |
| 2 **[V]** | `main.tex:86`, `intro.tex:147`, `ballistic.tex:385`, `discussion.tex:318` | "under a gate closer than the device is long the gate connection rectifies more than the shape" | the 300 nm gate is closer than the 550 nm device, and there the shape is larger at every frequency. The circuit wins only under the 30 nm gate, and only at 1 THz; at 0.3 THz the shape is still larger |
| 3 **[V]** | `discussion.tex:71` | "The 0.55 um funnel meets the criterion" | the criterion gives 0.4 um at 1 THz (`discussion.tex:55`); the funnel is at fL/v_F = 0.55 |
| 4 **[V]** | `dc.tex:72` | "The straight control gives 0.99 +- 0.01" | that is one of eighteen conditions. Over all of them the control runs from 0.94 +- 0.02 to 1.06 +- 0.04, and three points are more than two standard errors from one (0.94 +- 0.02, 1.04 +- 0.02, 0.95 +- 0.02) (`results/killtest.csv`). Either the errors are too small or the control is not symmetric; both weaken "sixteen of the eighteen points ... within two standard errors" for the valve |
| 5 **[V]** | `ballistic.tex:320-324` | mirror control "up to 0.06 uA in single runs at 0.3 and 1 THz"; floor 0.05 uA | the control under the 30 nm gate at 3 THz gives -0.093 +- 0.052 uA (`results/bpic/P3.csv`). Several results are then one to two times the largest control excursion, not "two to nearly four times the floor" |
| 6 **[V]** | `plasmonic.tex:122`, Table IV | constant viscosity, "0.3 to 1 THz": 0.09 to 0.18 A/W, NEP 32 to 60 | the sweep ends at 0.914 THz. At 1 THz the same section gives 0.055 A/W, an NEP of 103. The band should read 0.055 to 0.18 A/W and 32 to 100 |
| 7 **[V]** | `main.tex:76`, `intro.tex:121` | the drift limit "is reached at tens of microwatts" | 26, 38 and 105 uW at 100 K; 100 to 130 uW at 300 K |
| 8 **[V]** | `plasmonic.tex:389` | matched antenna "at best ... 1.3e-4 at 300 K" | 1.25e-4 at 0.3 THz only; two sentences earlier the 75 ohm value over three frequencies is 1.7e-4, and the matched one is 1.8e-4 |
| 9 **[V]** | `plasmonic.tex:403`, `discussion.tex:454` | efficiency at the limit "three times lower" without the viscoelastic term at 1 THz | 4.7e-5 against 1.26e-5 in the time domain: 3.7. (3.0 if both are from the small-signal solver; the text mixes the two) |
| 10 **[V]** | `supplement.tex:451` | "Over the 25 frequencies up to 1 THz" | 20; the 25 run to 3 THz |
| 11 | `supplement.tex:460` | 300 K shape-only mesh error "within 7 % above 0.2 THz" | the supplement's own table gives 8 to 9 % at 0.279 THz |

## 2. The summaries claim more than the body

Each of these is true to the body in the place where the result is
given and overstated where it is summarised.

1. **The Tesla valve: "not a useful rectifier in any limit"**
   (`main.tex:63`; `discussion.tex:7` "fails in graphene in every
   regime"). Table III says "not computed" for the valve in all three
   ballistic terahertz rows; `dc.tex:73-76` concedes that a per cent or
   two is not excluded and that no valve was run with a computed field
   or an ac drive; the terahertz fluid result is for the 175 nm valve at
   11 kilohm on 75 ohm only, although Sec. II says each shape was scaled
   to 1 um. What was shown: no asymmetry of the valve's sign above a few
   per cent at dc, and a small shape term in the viscous and plasmonic
   limits for one mismatched device.
2. **The length criterion** (Eq. (19); `main.tex:76-79`,
   `conclusion.tex:41-45`). It is derived for a uniform field. The
   paper's own static calculation puts 55 % of the drop on 500 nm at the
   neck, and with a drop confined to the neck the 6 um device gives 0.75
   A/W at 1 THz (`ballistic.tex:129-134`), six times the computed result
   of the short funnel. The question the introduction poses, whether
   long devices keep the speed of short ones, is therefore not answered,
   and the paper says "remains open" in seven places.
3. **The 100 K numbers carry the headlines but are outside the regime
   the paper claims.** The limitations paragraph says the 100 K set is
   quantitative only to 0.16 THz, is "outside" the fluid regime in the
   175 nm devices, and is "not ... a prediction for a device at that
   temperature". Yet the top of the abstract's range, the "matched horn"
   of the conclusion (175 nm, 100 K, 1.87 THz), the design advice and a
   row of Table IV beside measured detectors are all 100 K results. The
   100 K set is also not self-consistent: Table I takes the 300 K
   viscosity as v_F^2 tau_ee / 4, and the same estimate at 100 K gives
   0.25 m^2/s against the 0.1 assumed, while tau_ee = 1 ps sets the
   viscoelastic roll-off that decides a factor of four at 1 THz.
4. **"The shape gives a few per cent at 300 K"** (`main.tex`,
   `intro.tex:111`, `conclusion.tex:18`). The body says the 12 um leads
   are longer than the 3.6 um decay length and that the 175 nm funnel
   with 1 um leads gives half by its shape (`plasmonic.tex:55-60`). The
   split may be a property of the leads. Stage H7 points the same way:
   at 300 K the largest drift is at the source contact, not in the
   funnel.
5. **The upper end of the efficiency, 5e-4**, is a square law
   extrapolated to 0.8 of the gate overdrive at the thin-gate grid
   points (`plasmonic.tex:383-387`), where the introduction itself says
   the law saturates. The time-domain check exists at the reference
   point and one other only. The abstract calls this "the range in which
   the models are trusted".
6. **The ballistic efficiency of 1e-4** is at 0.2 V, above E_F / e,
   where the current is 1.5 times its square law for a reason the paper
   lists as unexplained; at 0.1 V it is 2e-5. It is per absorbed watt,
   and the abstract sets it beside a fluid figure per available watt.
7. **The edge condition in the abstract**, "specularly about half the
   time or more", holds only with the angle-dependent law of Eq. (3),
   which the paper calls untested. With one specularity for all angles
   half the response is gone at 0.84.
8. **"At room temperature"** for the ballistic funnel: the
   electron-electron mean free path is 100 nm, shorter than the device,
   and momentum-conserving collisions were not simulated
   (`ballistic.tex:257-261`). The summaries drop this.
9. Smaller ones: the conclusion says the device D asymmetry is
   "consistent with the value measured" where `dc.tex:52-58` says "that
   is not a validation"; "about twice what the circuit makes"
   (`conclusion.tex:74`) compares across models and normalisations;
   "contacts take little" (`ballistic.tex:605`) where the current falls
   from 0.43 to 0.27 uA; the headline 0.12 A/W is at 0.1 V and the
   small-signal value is 0.10; the introduction's "up to 1.7 times
   larger" is the 1 um funnel only (the channel: 1.7 to 2.9).

## 3. Definitions, notation and figures

- **Never defined:** "reference point" and "reference device";
  R_dc of the 1 um funnel (172 ohm appears only inside Fig. 6(b), the
  300 K value nowhere), though Eq. (14), every NEP and every V/W figure
  need it.
- **Laboratory words:** "production point", "production mesh", "ladder"
  in two senses, "valid grid", "the horn", "tests T22 to T32".
- **Used before defined:** "drift limit" (`intro.tex:157`, defined in
  Sec. II D).
- **One symbol, several meanings:** h (Planck, roughness, h(n), mesh
  size); T (temperature, transmission, transparency); Phi; G; N; p;
  "diodicity" for both D and D - 1; R_abs with two definitions, mixed in
  Table III.
- **Fig. 8:** open symbols unexplained in (a) and with a different
  meaning in (b); a per-absorbed-watt curve on an axis labelled for a
  75 ohm source.
- **Fig. 6:** the caption says the 100 K curves "end at 1 THz"; they end
  at 0.914 THz, and the 1 THz values in the text are separate runs.
- **Efficiency bounds differ between places:** lower bound 1e-5, 2e-5
  or 0.4e-4; at the reference point "at most 6e-5" in Sec. IV B and
  "7e-5" in Table III and the discussion (the 7 is the small-signal
  solver at 300 K and 0.1 THz, which Sec. IV B does not give); upper
  bound 5e-4, 5.4, 5.6 or "at most 6e-4".
- **Two normalisations side by side** (`plasmonic.tex:265-272`):
  "within 7 %" of the square law (normalised at the smallest amplitude)
  and "2 to 8 % above" it (against the small-signal value).

To check, not yet verified:

- "At most three per cent" for the viscous valve against Fig. 3(b),
  where some curves inside the unshaded window appear to reach 0.1 to
  0.25. The claim needs the pairing of viscosity, damping and slip
  stated.
- `ballistic.tex:73`: "5 %" of electrons fly longer than 20 ps. The
  results file says 5.3 %; the referee read 8.8 % off the curve of
  Fig. 9(c).
- Tesla-valve bound 5e-7 (`dc.tex:276`) against 4e-7
  (`supplement.tex:577`).
- `models.tex:357`: "0.41 v_F" beside a formula "to lowest order" that
  gives 0.43; the 0.41 is the full formula.
- G3 array size 15 in Table II and 19 in the text (two definitions).
- A dozen further rounding or wording mismatches from the number check,
  none changing a conclusion: 13 % against 11 % for the same mesh
  change; "above 1 THz" against "above 1.2 THz"; "five to seven times"
  against "six- to sevenfold"; "a twentieth" for 1/17; "three valid
  points" where four are valid.

## 4. Length and repetition

37 pages. The five worst passages, each 600 to 720 words in one or two
paragraphs:

1. **Sec. IV A** (`plasmonic.tex:26-82`, `127-189`): results, three
   sensitivity studies and forward references inline. One table of
   responsivities (termination by temperature and viscosity model, at
   0.1, 0.3 and 1 THz) and one of sensitivities would replace most of
   it; the adiabatic and isothermal argument belongs in the supplement.
2. **Limitations** (`discussion.tex:348-473`): nearly every item
   restates something given with the result. A list of at most eight
   limitations that could change a conclusion.
3. **The results paragraph of the introduction** (`intro.tex:101-153`):
   a second abstract with about forty numbers.
4. **Sec. V, specularity and contacts** (`ballistic.tex:415-569`): lists
   of six currents for two gates; a table, with each caveat stated once.
5. **Sec. IV B, opening** (`plasmonic.tex:250-313`): fifteen lines
   reconciling two solvers before any result. Since Stage H7 this can be
   one sentence and a supplement reference.

Repeated across sections: the uniform-field caveat (seven times), the
short-funnel summary (six), the Kachorovskii framework (three), "scales
as its square" (four), the same four specularity citations (four).

Sentences that read as replies to a critic and not as exposition:
"that is not a validation ... We claim no quantitative agreement"
(`dc.tex:52-58`), "is not offered as an explanation", "We do not offer",
"holds no hidden order of magnitude", "assumptions, not fits", "had to
be relaxed". They are the residue of the earlier reviews.

## 5. Runs a referee would ask for

| # | Run | Settles | Cost | My view |
|---|---|---|---|---|
| a | Device D (6 um) at 1 THz with the field computed | the length criterion (2.2), the paper's central open point | large: the static run was 25 to 50 million particles; needs a plan | the one run that changes what the paper can say |
| b | 1 um funnel with leads of 1 to 3 um at 300 K, small signal | the 300 K split (2.4) | about 30 minutes | do it |
| c | Time-domain ladder at a thin-gate grid point | the 5e-4 (2.5) | about 1 hour | do it |
| d | 1 um Tesla valve, small signal, both terminations | the terahertz verdict on the valve (2.1) | about 30 minutes; the mesh exists | do it |
| e | A Tesla valve with the field computed, dc | the dc verdict on the valve (2.1) | large | or narrow the claim |
| f | Electron-electron collisions in the particle code | "at room temperature" (2.8) | new code and tests | state as a limitation |
| g | Mode-matching check of the 100 nm neck | classical trajectories in the neck | outside this code | state as a limitation |
| h | A self-contained derivation of Eq. (8) against Tomadin and Polini | the correction to a published theory | writing only; the checks exist in `tools/momentum_step0.py` | an appendix or the supplement |
| i | Read the three table sources marked "abstract only" | Table IV | reading | do it |

## 6. What I would do, in order

1. **Fix section 1** and the mismatches of section 3. Small, no runs.
2. **Bring the abstract, introduction and conclusion into line with the
   body** (section 2): the Tesla verdict narrowed to what was computed;
   the length criterion stated as conditional on a spread field; the
   headline range split by temperature, with the 100 K part labelled as
   the fluid limit; the gate and edge conditions as computed.
3. **Runs b, c and d** (about two hours of machine time, after a plan),
   then decide on a.
4. **Restructure for length** (section 4): tables in Sec. IV A and V,
   the introduction's results paragraph cut to a few sentences, the
   limitations as a list. This is the largest edit and should come after
   the numbers have stopped moving.
5. **Your decisions:** whether to run device D at 1 THz or leave the
   long device open; whether this is one paper or two (the referee
   suggested a fluid paper and a ballistic paper); the journal.

## 7. Applied on 2026-11-21 (tenth draft)

No new run. Both PDFs build; `check_tex.py` reports 0 problems.

**Section 1, all eleven:**

1. Supplement and Appendix C: drift per amplitude 3.58, 2.93, 1.67;
   the solvers agree on the drift to 2 %.
2. The gate criterion, in the abstract, the introduction, Sec. V and
   the design advice: the gate connection wins at 1 THz under a gate at
   a twentieth of the device length; with the gate at half the length
   the shape is the larger.
3. The 0.55 um funnel is "close to" the criterion, at fL/v_F = 0.55; the
   introduction gives both marks (half at 0.4 of a period, gone at two);
   the conclusion says "near the half-response point".
4. The straight control: both clean-limit values, its range of 0.94 to
   1.06 over the eighteen conditions, that the errors are too small by
   about 1.3, and that the valve's two exceptions remain at 2.7 and 8
   standard errors.
5. The mirror control at 3 THz (-0.09 +- 0.05 uA) is stated; the floor
   of 0.05 uA is "at 1 THz"; the reversed current is also given against
   the largest control current.
6. Constant viscosity: NEP 32 to 60 up to 0.9 THz and 100 at 1 THz;
   Table IV 0.055 to 0.18 A/W and 32 to 100; the caption of Fig. 6 says
   the curves are drawn to 0.91 THz.
7. "26 to 130 microwatts" in the abstract and the introduction.
8. Matched antenna at 300 K: 1.8e-4.
9. "Three to four times lower" in Sec. IV C and the limitations.
10. "20 frequencies up to 1 THz".
11. 300 K shape-only mesh error: within 9 % above 0.25 THz.

**Section 2:**

- Tesla valve: "does not rectify usefully in any case computed", with
  what was not computed, in the abstract, introduction, discussion and
  conclusion.
- The headline range is split by temperature in the abstract (0.05 to
  0.07 A/W at room temperature; 0.14 to 0.26 A/W at 100 K, "at the edge
  of the fluid regime"); the introduction and conclusion say the same
  and label the taper resonance as a 100 K result outside the regime;
  the design advice makes the resonance conditional on weak damping.
- The 300 K split: the long leads are named as a possible cause in the
  introduction and the conclusion.
- Efficiencies: "at the drift limit ... by the square law", the upper
  end "at amplitudes not checked in the time domain"; the ballistic
  figure as 2e-5 at 0.1 V and 1.5e-4 at 0.2 V of the absorbed power;
  "trusted" removed.
- Edges: 0.84 with one specularity, 0.5 head-on in the rough-edge
  model, which is called untested in the introduction.
- "Without collisions between electrons" in the abstract, introduction
  and conclusion.
- Conclusion: the device D comparison "is no test of the model"; the
  ballistic and fluid figures are set side by side with their
  normalisations in place of "about twice".
- Contacts of transparency 0.25: a third of the current, unchanged per
  absorbed watt under the distant gate, half under the near one.
- The length criterion was already conditional in the abstract and is
  unchanged; the long device remains open (run a of section 5).

**Section 3:**

- "Reference point" and "reference device" defined in Sec. II D; R_dc of
  the 1 um funnel given (172 and 582 ohm).
- One range for the efficiency at the limit in the design rule (1e-5 to
  5e-4); Sec. IV B gives the small-signal values at 300 K that Table III
  quotes (5e-5 to 7e-5); the second normalisation of the square law is
  removed.
- The viscous valve: the pairing of viscosity and damping behind "three
  per cent" is stated (Re = 10 needs 0.02 m^2/s, for which Da = 50 at
  1 um).
- Tesla-valve bound 4.5e-7 in both places; "production" removed; "a
  seventeenth"; "the three least degenerate valid points"; 0.41 v_F said
  to be from the full expression.

**Not done:**

- Notation: the symbols with several meanings (h, T, Phi, G, N, p),
  "diodicity" for D and D - 1, the two definitions of R_abs.
- Fig. 8: the open symbols of panel (a) and the mixed normalisation.
- "5 %" of electrons longer than 20 ps against the curve of Fig. 9(c)
  (the results file says 5.3 %; the figure was not re-read).
- G3 array size 15 in Table II against 19 in the text; a few rounding
  mismatches ("five to seven" against "six- to sevenfold", "0.70" of the
  ideal conductance for 0.68, "within 0.4 standard errors" for 0.47,
  "1 to 100 V/W" for a table entry of 105).
- The 100 K parameter set is still not self-consistent (2.3); only its
  status is now stated where its numbers are quoted.
- Section 4 (length) and section 5 (runs).

The main text is 600 words longer than the ninth draft: the corrections
added qualifications and removed nothing. Section 4 is now more needed,
not less.
