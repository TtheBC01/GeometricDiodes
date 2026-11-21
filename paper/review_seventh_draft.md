# Review of the seventh draft (2026-11-19)

Three reviewers that had not seen the reasoning behind the revision each
read the draft for one thing: (1) every fluid number against the raw
result files, (2) the manuscript read cold for contradictions, leftovers
and claims beyond the evidence, (3) the equations re-derived from first
principles. This file collects their findings. **[V]** marks a finding I
re-checked against the files myself; the rest are as reported. Nothing in
the paper has been changed in response.

**Update, 2026-11-19 (evening).** The two runs of section 4, items 1 and
2, are done. Their outcomes are at the end of sections 1.1 and 3.3, and
section 4 is revised. The paper is still unchanged: its Figs. 6(c, d)
and 13 and the cells of Table III for the 175 nm funnel and the Tesla
valve still show the 20 nm data.

**Second update, 2026-11-19 (night).** The four headline sweeps of the
1 um funnel were re-run on the 20 nm mesh (section 3.3, "Outcome of the
fine-mesh sweeps"). They found a second mesh error, in the interior of
the production mesh, which makes every 1 um funnel number above about
1.2 THz, and the density and gate grid at 1 THz, unreliable.

**Third update, 2026-11-20.** Every other small-signal run of the 1 um
funnel is now also on the 20 nm mesh (section 3.5): 127 runs. All
small-signal results of the 1 um funnel therefore exist on one mesh
that is converged to about 6 % at 100 K and 1 % at 300 K up to 1 THz.
The paper still reads the 80 nm files.

**Fourth update, 2026-11-20 (evening).** The text and the figures have
been corrected in one pass (section 4, item 3; what was done is listed
at the end of section 4). The paper is now the eighth draft. Statements
above that "nothing in the paper has been changed" describe the state
before this pass.

## 1. Numbers against the result files

The reviewer recomputed from `results/thz2/*.csv`, `results/dcgate2/*.csv`
and `results/thz2/ladder_H6.txt`. One cause recurs: many ranges in the
text are values at three sampled frequencies (0.108, 0.279, 0.914 THz, or
the grid files at 0.1, 0.3, 1 THz) written as if they held over a band.
That habit is mine, from `tools/momentum_report.py`, whose tables sample
those frequencies.

### 1.1 An error in the runs, not in the text **[V]**

**The 175 nm funnel and the Tesla valve were re-run on a coarser mesh
than before.** `tools/momentum_runs.sh` passes no mesh size for
`S2_funnel_*`, `S3_tesla_classic_*`, `C2_funnel_*`, `C2m_funnel_*` and
`FT_funnel_*`, so they ran at the default wall spacing of 20 nm (42 411
and 63 464 nodes). The earlier runs used 14 nm (65 927 and 97 491
nodes), and the supplement says 14 nm. The dc resistance shows it: 1871
against 1919 ohm for the funnel, 10 514 against 10 943 ohm for the Tesla
valve, a quantity the equation cannot change.

Consequences:
- every number for these two devices in Sec. IV A, Fig. 6(c, d), Fig. 13
  and Table III is on a mesh 2.5 to 4 % different in resistance from the
  one stated;
- the sentence "the correction of the momentum equation changes these two
  numbers by 4 %" (`plasmonic.tex:76-78`) attributes a difference that is
  at least partly the mesh;
- every before/after ratio for these devices in
  `results/HYDRO_MOMENTUM.md` and `tables_H6.md` has the same confound.

The 1 um funnel, the short-lead funnel, the channel, the grid and the
large-signal runs are on the same meshes as before (18 601, 19 497,
18 937 nodes).

**Fix:** re-run those 22 sweeps with `--hw 14 --hm 84` (small-signal
solver only; about an hour at eight jobs), regenerate the tables and
figures, and re-check the affected sentences.

**Outcome (done).** `tools/momentum_runs.sh mesh14` now carries the mesh
size for these lines and has re-run the 22 sweeps: 65 927 and 97 491
nodes, 1918.6 and 10 942.8 ohm (1207.9 and 5988.1 ohm at 300 K), the
mesh and the resistances of the sixth draft. The 20 nm results are kept
in `results/thz2/hw20`; `tools/momentum_mesh14.py` compares the two
(`results/thz2/tables_mesh14.md`), and `results/thz2/tables_H6.md` is
regenerated from the 14 nm data.

- **Responses with the DS termination** differ between the meshes by 1
  to 3.5 % of their largest value, for both devices and both
  temperatures. At single frequencies the Tesla valve at 100 K differs
  by up to 13 % (0.0114 against 0.0131 A/W at 0.28 THz).
- **Shape-only responses** differ by 2.5 to 4 % of their largest value
  for the funnel and 8 to 11 % for the Tesla valve. Where the response
  is small the difference is as large as the value: the 175 nm funnel at
  300 K below 0.1 THz gives -1.5e-4 to +6e-4 A/W on one mesh and -3.7e-4
  to +3.6e-4 on the other, an offset of 2.3e-4 A/W; at 100 K below
  0.1 THz, 2.5e-4 to 9.6e-4 against 3.7e-4 to 1.1e-3. **Shape-only
  values of these two devices below about 5e-4 A/W are not resolved on
  either mesh**, and the paper should not quote them to more than an
  order of magnitude (it does not, for the Tesla valve; the "1 to 2 mA/W"
  and "2 to 3 %" statements concern the 1 um funnel, see 3.3).
- **Sentences of Sec. IV A**, recomputed from the 14 nm data. Unchanged
  at the precision quoted: 0.21 and 0.43 A/W at the taper resonance
  (0.209, 0.428); 0.013 against 0.066 A/W at 300 K; the Tesla valve at
  1 THz, 1.0e-2 and 4e-4 A/W (1.03e-2, 4.5e-4), "twenty and thirty
  times below" the funnel (18 and 28); 0.005 to 0.09 A/W with the DS
  termination (0.0045 to 0.094); 3 to 30 % absorbed below 50 GHz. **To
  change:** the Tesla valve's shape-only response below 0.3 THz is 1e-5
  to 8e-4 A/W, not "to 6e-4", and its resonance near 0.7 THz reaches
  1.8e-2, not 1.6e-2.
- **"The correction changes these two numbers by 4 %" stands**: on
  equal meshes the ratios are 0.970 and 0.960.
- The ratios that do not involve the absolute level are the same on
  both meshes to 1 %: the contact variants (0.65 to 0.97 and 0.67 to
  0.74 of the DS response at the sampled frequencies), the
  finite-temperature coefficients (1.00 to 1.04 and 1.17 to 1.21) and
  the amplitude at which the drift reaches 0.3 v_F.
- Not regenerated, because that changes the paper: Figs. 6(c, d) and 13
  and Table III.

### 1.2 Wrong as written

| # | Where | Written | The files give |
|---|---|---|---|
| a **[V]** | `plasmonic.tex:101`, `discussion.tex:191`, Table IV 100 K row | NEP "24 to 26 pW/Hz^1/2" for the 1 um funnel at 100 K, 0.3 to 1 THz | 24 to 32 between 0.3 and 1 THz, 24 to 38 between 0.3 and 2 THz; 24 and 26 are the two end points |
| b **[V]** | `discussion.tex:175`, Table IV 300 K row, `conclusion.tex:71` | "230 with 300 ohm um contacts", "130 with 100 ohm um", next to an ideal-contact 100 for 0.3 to 1 THz | 228 and 132 are the 0.1 THz values; between 0.3 and 1 THz they are 250 to 278 and 138 to 153 |
| c **[V]** | `plasmonic.tex:30-34` | at 100 K the two terms cost the funnel "at most a third" and the channel "a half or more" | funnel: up to 0.43 below 1 THz (0.37 between 0.3 and 1 THz); channel: 0.42 to 0.66 |
| d **[V]** | `plasmonic.tex:110-112, 121`, `discussion.tex:404`, `conclusion.tex:23` | lossless contact lowers the DS response to "0.64 to 0.92" / "by up to a third" | 0.56 to 0.99 for the 1 um funnel at 100 K between 0.1 and 1 THz (a loss of up to 44 %); 0.64 to 0.97 at 300 K |
| e **[V]** | `ballistic.tex:8-10` | 0.87 A/W is "two to ten times the plasmonic figures" | 3.6 to 17 times with the present 0.05 to 0.24 A/W; the sentence is left from the sixth draft |
| f | `supplement.tex:407-410`, `HYDRO_MOMENTUM.md` item 12 | at 1 THz the time-domain and small-signal values differ by "the same absolute difference as with an ordinary fluid" | the absolute difference is 3.3 times smaller now at 100 K and 1.9 times smaller at 300 K; the 10 % and 3 % are right, the explanation is not |
| g | `plasmonic.tex:222` (Fig. 7 caption), `supplement.tex:536` | "each curve ends at the last amplitude for which a periodic state exists" | not for the two 1 THz, 100 K ladders, which were cut short at a drift of 0.56 and 0.26 v_F |
| h | `supplement.tex:392` (P15) | 7e-15 | 3.5e-15 in the new ladder; 7e-15 is the old one |
| i | `supplement.tex:526-528` | steady gated flow "20 nm for the 175 nm devices and 80 nm for the 1 um devices" | the 1 um Tesla valve is at 114 nm and the straight channel at 50 nm |

### 1.3 True only with an unstated restriction

- **Efficiency "1e-5 to 4e-5 at a drift of 0.3 v_F"**
  (`plasmonic.tex:185`, Table III, `discussion.tex:263`). These are
  time-domain values at the last amplitude of the ladder below the limit
  (drift 0.17 to 0.30 v_F), and the 100 K, 1 THz value is 4e-6. By the
  paper's own Appendix B definition the reference point gives 2e-5 to
  5e-5 at 100 K and 4e-5 to 7e-5 at 300 K, and Sec. IV C gives 0.6e-4
  for the same point at 300 K.
- **The two solvers disagree on the drift at 100 K [V].** Largest drift
  per unit amplitude, small-signal solver against time-domain solver:
  3.84 against 3.58 v_F at 0.1 THz, 4.34 against 2.93 at 0.3 THz, 3.67
  against 1.67 at 1 THz; at 300 K they agree to 2 %. So Sec. IV C puts
  the drift limit of the reference device at 18 uW (0.3 THz, 100 K) and
  Sec. IV B at 37 uW, and Appendix B says the time-domain runs "justify"
  the grid estimate. The cause is presumably the viscoelastic term that
  the time-domain solver lacks; the paper does not say that the level
  differs.
- **"2 to 3 % at 300 K"** for the shape share (`plasmonic.tex:41`,
  `discussion.tex:32`): 1.5 to 4.1 % between 0.3 and 1 THz. **"1 to
  2 mA/W"**: 0.4 to 2.3.
- **"Its impedance is 80 to 200 ohm"** (`plasmonic.tex:24`): that is the
  symmetric termination; with the DS termination Re Z is 36 to 155 ohm
  at 100 K. The absorbed fractions are right.
- **"r is 0.06 to 0.09 and eta/P_av 0.9 to 2.0 per watt"**
  (`plasmonic.tex:171`): three frequencies; over the sweep 0.059 to
  0.099 and 0.84 to 2.4 per watt.
- **Gated against incompressible, "nine of ten within a factor two"**
  (`dc.tex:256-261`): true for magnitudes and for one of the two gate
  connections. Two of the nine have the opposite sign, and with the
  other connection three fail and the "exception" is 0.63, not a tenth.
- **"within 5 % of the closed form"** (`dc.tex:247`): 5.08 % for the
  straight channel; the Tesla valve's 8 % is at its largest bias inside
  the limit, not its largest bias.
- **"at most 3e-4"** (`dc.tex:251`): over the production points; the
  28 nm mesh point of the convergence set has 5e-4.
- **"the polarity term is 1.7 %"** (`dc.tex:279`): that is at 50 mV; the
  sentence now says 43 mV, where it is 1.45 %.
- **"1.6 to 3 times larger, above the classical one"**
  (`plasmonic.tex:20`): the ratio is right; "above the classical one"
  fails at four frequencies (0.83 to 1.45 of the classical response).
- **The closed form "to three digits from 10 GHz to 3 THz ... and at
  300 K"** (`plasmonic.tex:7-14`): to 1e-4 up to 1.2 THz, 6e-3 at 3 THz;
  there is no classical-gas run at 300 K. "To 0.13 %" is the test, not
  the production sweep (2e-4 up to 1 THz, 1 % at 3 THz).
- **Plain channel, "0.10 to 0.18 A/W and the funnel 1.05 to 2.3 times as
  much"** (`plasmonic.tex:53`, `discussion.tex:37`): two different bands;
  for 0.2 to 1 THz the ratio is 1.28 to 2.3, and below 0.1 THz the funnel
  gives less than the channel at half the points.
- **"costs the expected 15 % and nothing more"** (antenna as dc load,
  `plasmonic.tex:200`): up to 0.3 v_F; 20 to 24 % above it.
- **Conclusion, "a twentieth to a fifth of the gate overdrive" and "1e-5
  to 5e-4 at every density and gate distance"**: the first is the
  reference point only (on the grid it reaches the overdrive itself); the
  second is 0.3 THz only (1.7e-4 at 0.1 THz at 300 K; 1.7e-7 at 1 THz at
  some valid 100 K points).
- **"up to 1.7 times larger"** (`intro.tex:113`): for the DS numbers; the
  shape-only numbers in the same sentence, up to 2.2.
- **"an order of magnitude or more below the funnel at 1 THz"**
  (`conclusion.tex:10`): for the shape; with the DS termination at 300 K
  the Tesla valve is 6 to 7 times below.
- Smaller slips of rounding or band are listed in the reviewer's report
  (item 27): 5.2e-6 for "below 5e-6", 0.012 for "0.013", 0.48 for "half
  or more", "7e-5 up to 0.8 v_F" where the test reaches 0.64 v_F, 4e-7
  against 5e-7 for one quantity in two places.

### 1.4 To look at

- **P17 in the supplement, "0.59 of the classical response in volts".**
  The test compares one unit of amplitude in each fluid's own voltage
  unit; at equal amplitude in volts the same two numbers give 0.43. One
  of the two is meant; the label says the second.
- The mesh table of the supplement reproduces for one gate connection;
  for the other the quadratic coefficient moves the other way, by 24 %.

### 1.5 Checked and found correct

About 120 statements, listed in the reviewer's report: the abstract's
four fluid numbers; all of Sec. IV A apart from the items above
(responsivities, the 175 nm resonance, the Tesla numbers, the control's
floor, linearity in the contact coefficient, the finite-temperature
coefficients); Sec. IV B (28 %, the small-signal levels, 9 to 37 and
80 uW, the rise above the law, the control run, 0.09 %, the shape-only
curve); Sec. IV C (all grid, matched-antenna, mobility and contact
numbers); Sec. III C (0.48, 12 of 112, factor sixteen, the bounds); the
ballistic comparisons other than (e); Tables III and IV other than the
cells above; the appendices; and tests P1 to P14, P16 to P27 against the
ladder output.

## 2. Cold read

The reviewer read the whole manuscript in order, looked at Figs. 5 to 8,
10, 11 and 13, and rebuilt the paper to check every cross-reference. All
numbered references resolve correctly, including those from the
supplement to the main text. None of the old phrases ("knee", "0.7 of
the voltage unit", "about one per cent", "40 % higher", "not computed")
survives. The findings, most serious first:

### 2.1 The two drift limits are swapped in the summaries **[V]**

The abstract (`main.tex:68`), the introduction (`intro.tex:115`) and the
conclusion (`conclusion.tex:24`) say that the square law "ends where the
drift speed approaches the Fermi velocity, at tens of microwatts". Sec.
IV B says something else: tens of microwatts is where the drift reaches
0.3 v_F, and the law still holds to 5 % there; the drift reaches v_F at
0.3 to 0.9 mW. Nothing in the model ends the square law at 0.3 v_F:
between the two speeds the ideal gas rises above it. The opening of
Sec. IV C (`plasmonic.tex:234`) has the same defect. What is true: the
model is trusted up to 0.3 v_F, reached at tens of microwatts, with the
square law intact; the ideal gas has no state beyond v_F.

### 2.2 The "tenth of a per cent" ceiling is not established

Claimed in the abstract, introduction (twice), discussion and conclusion,
in the strongest form as "below a tenth of a per cent at any amplitude
the ideal gas allows" and "no mechanism ... at any amplitude computed".
Four reasons it does not hold as written:

1. **It comes from one device.** The 0.09 % is the reference device near
   v_F. The grid values (up to 5e-4) are at 0.3 v_F. The reference device
   gains a factor of 20 to 90 between 0.3 v_F and v_F; the same factor on
   the grid would give about 1 %. No run covers the grid near v_F.
2. **It is the last point of a coarse ladder.** The amplitudes go 0.4,
   0.5, then 0.7, and the efficiency is rising steeply at the last
   converged point (2.9 times for 1.56 times the power at 300 K, 1 THz).
   The value as the drift approaches v_F is not bracketed, and 8.7e-4
   needs 16 % more to cross 1e-3.
3. **The paper's own ballistic numbers exceed it [V].** With an imposed
   field the short funnel gives 0.70 A/W per absorbed watt and 0.80 of
   its square law at 0.2 V (`ballistic.tex:275`), which is an efficiency
   of 0.16 % of the absorbed power. The paper presents the imposed field
   as an overestimate, but "any amplitude computed" includes it. The
   sixth draft's "about one per cent" covered these numbers; the new
   wording does not.
4. The discussion itself says the ballistic figure "is not a ceiling".

What can be said: at a drift of 0.3 v_F the fluid element converts 1e-5
to 5e-4 depending on the operating point; the ideal gas of the reference
device stays below 1e-3 up to 0.95 v_F; the computed-field ballistic
funnel gives 1e-4 at 0.2 V.

### 2.3 The efficiency limit rests on an assumed cutoff

The 0.3 v_F limit has no citation, estimate or test, and every power and
efficiency "at the limit" scales with its square. It also has two
justifications in the paper: "where a Fermi-liquid description is still
tenable" for the mean drift in Sec. III (`dc.tex:155`), and heating and
optical phonons for the largest local speed in Sec. II
(`models.tex:327`). The sentence in the discussion that "most
idealisations favour the device", so that the negative results are safe
(`discussion.tex:314`), is not true of this cutoff, nor of the
zero-temperature coefficients, which lower the 300 K response.

### 2.4 Contradictions between places

| # | Places | What disagrees |
|---|---|---|
| a | `plasmonic.tex:183-186` and Table III against `plasmonic.tex:249-256` and Fig. 8 | power and efficiency of the reference device "at 0.3 v_F": 9 to 37 uW and 1e-5 to 4e-5 in one place; 18 uW, 125 uW and up to 0.6e-4 in the other. Two causes: the first set is the last ladder amplitude below the limit, not the crossing (interpolated: 26, 38, 104 uW at 100 K, 103 to 133 uW at 300 K), and the two solvers differ at 100 K (section 1.3) |
| b | `plasmonic.tex:41` against `:204` | the shape gives "a third" of the DS response (current, small-signal solver) and "a fifth" (efficiency, time-domain solver). A third in current is a ninth in efficiency; neither sentence says which measure it uses |
| c **[V]** | `discussion.tex:32`, `plasmonic.tex:80` against `plasmonic.tex:46-48` and Fig. 6(c) | "a comparable response only near a taper resonance at low temperature" / "the one place": the short-lead funnel gives half at 1 THz and 300 K, and Fig. 6(c) shows 0.18 against 0.30 A/W at 0.91 THz, off resonance |
| d | `discussion.tex:271-274` | "the ballistic funnel has ... a lower efficiency still: 1e-4 at 13 uW". At 13 uW the plasmonic element gives 6e-6 to 3e-5. The ballistic figure is higher at equal power |
| e **[V]** | `ballistic.tex:277-279` | "a tenth of the gate overdrive that limits the plasmonic element": Sec. IV B now says the drift limits it, well short of the overdrive |
| f | `conclusion.tex:21-24` | "these figures ... move by up to a third with the condition imposed at the contacts": "these figures" include the shape-only share, which moves by 0.58 to 1.44 (and see 1.2 d) |
| g | `dc.tex:262` against `models.tex:269` | "the Dirac gas departs from an ordinary fluid there [beyond 0.3 v_F]"; Sec. II says the departure is of the same order as the convective term at any speed |

### 2.5 Left over from the earlier version **[V]**

- **"Saturation".** The fluid no longer saturates: the DS response rises
  above the law and the shape response falls below it. Still present:
  `intro.tex:153`, `conclusion.tex:6`, `discussion.tex:33`, Table III
  ("saturates first").
- **"The correction of the momentum equation changes these two numbers
  by 4 %"** (`plasmonic.tex:76`): a reader of the paper knows of no
  correction. (Also wrong for another reason, section 1.1.)
- **`intro.tex:65-67`**: "a plasmonic cavity whose convective and density
  nonlinearities rectify", forty lines before the same introduction says
  the model has no convective term in one dimension.
- **The ordinary convective term is still the model of the viscous
  liquid** (Eq. (5), Sec. III B) with no bridging sentence. The bridge
  exists and is short: for an incompressible flow the two new terms are a
  gradient absorbed in the pressure. Without it a reader will think the
  Tesla valve's "three per cent" was computed with the equation the paper
  has just called a crude approximation.
- **Model names are not fixed.** "Dirac closure", "classical closure",
  "shallow-water closure", "classical gas", "ordinary fluid" are all
  used; three models are in play (shallow water; Dirac h(n) with an
  ordinary convective term; the Dirac gas of Eq. (8)).
- **The supplement speaks of earlier drafts** ("with which the results of
  earlier drafts were computed", "stored results of the earlier runs").

### 2.6 Claims beyond the evidence

- **Causes stated without a test.** The rise above the square law is
  "because the momentum per particle grows faster than the velocity"
  (`plasmonic.tex:188`): the control run shows that terms beyond second
  order cause it, not which. The fall of the shape-only response has no
  explanation. "The difference is the terms of relative order
  u^2/v_F^2" (`dc.tex:250`): the supplement says those terms add 1 % at
  that bias while the mesh moves the same number by 3 %.
- **"Rectifies mainly through its terminations"** rests on one measure of
  the shape's share. The plain-channel control is a second measure and
  gives a different picture (the funnel adds up to 2.3 times at 100 K,
  takes 15 % at 300 K). The split also depends on the contact condition,
  the solver, the 12 um leads and a numerical floor of a ninth. The
  introduction and conclusion give "a third" and "a few per cent" with
  none of this.
- **"The limit moves as expected"** (`plasmonic.tex:249`): Fig. 8(a)
  shows that at 1e12 cm^-2 and 100 K a thinner gate raises the limit (18
  to 40 to 65 uW). Only the density moves it down. And the subsection
  title says the ceiling does not move, over a ceiling that moves
  fifteen-fold.
- **"Only if ... its gate is farther away than the device is long"**
  (abstract): Sec. V E gives 0.13 A/W by shape under the near gate. What
  changes is that the circuit term becomes larger; it is not a condition
  for the shape to rectify.
- **Mixed normalisations, unflagged in the sentence**
  (`discussion.tex:106`, `conclusion.tex:65`, `intro.tex:128`): 0.12 A/W
  per absorbed watt of a 19-device array against 0.05 A/W per available
  watt of one device.
- **"Whose convective term vanishes in one dimension"**
  (`conclusion.tex:21`) is a zero-temperature statement placed beside a
  room-temperature headline; with the paper's own 300 K coefficients the
  one-dimensional coefficient is 0.32.

### 2.7 Figures against captions and text

- **Fig. 7(b) [V]:** the caption says "relative to the square law of
  Eq. (14)"; the curves are relative to each solver's own time-discrete
  law, normalised at the smallest amplitude.
- **Fig. 7 caption and text:** "each curve ends at the last amplitude for
  which a periodic state exists" is false for the 100 K, 1 THz curve
  (section 1.2 g). The dotted square laws are not in the caption.
- **Fig. 7(c)** is cited for powers but is plotted against volts.
- **Fig. 6(a):** the axis ends at 1.3 THz while the text says "to 3 THz";
  the classical solver is drawn at 100 K only while the text says "and on
  the broadband plateau at 300 K"; the appendix says "these are the lines
  of Fig. 6(a)" directly after the alpha = 1/2 sentence, and the figure
  has no such line.
- **Fig. 6(d):** the Tesla shape-only response is 3e-6 to 5e-6 at some
  frequencies below 300 GHz; the text says "1e-5 to 6e-4".
- **Fig. 5:** filled and open symbols mean three different things in the
  three panels; the caption defines two.
- Consistent: Fig. 5 numbers, Fig. 8(b) to (d), the plasmonic bands in
  Figs. 10 and 11, Fig. 13.

### 2.8 Definitions and small errors

- **`models.tex:269` [V]:** "the second and third terms on the left of
  Eq. (8)"; the terms meant are the third and fourth.
- P_0(n) is used and never defined or related to Phi(n) and h(n).
- "Drift limit" is used as a term from Sec. III on without a defining
  sentence.
- The two model choices are said to be "tested" in Sec. IV A (they are
  varied) and to enter "at first order" in one place and "at the same
  order as these terms" in another.
- The viscoelastic viscosity is introduced in Sec. II without saying that
  only the small-signal solver has it.
- The supplement lists a symmetric-drive ladder at 1 THz that the main
  text never reports; it is 55 % above its law at 0.26 v_F, inside the
  limit.

### 2.9 What a referee would ask for

1. **The relation to Tomadin and Polini in words.** The appendix says
   their result corresponds to alpha = 1/2; the paper's equation has
   alpha = 0 and cites them as a source. The manuscript never says that
   it disagrees with a cited theory, or why, and gives no device numbers
   for their form.
2. **The 0.3 v_F criterion:** a reference or an estimate; why a criterion
   for heating applies to a peak ac drift at one point; where in the
   device the largest drift sits; whether it converges with the mesh
   (re-entrant corners; the mesh test covers the response only).
3. **A small-signal 100 K curve without the viscoelastic term** in
   Fig. 6(b). The term doubles the efficiency at 0.3 THz and changes it
   seventeen-fold at 1 THz, so the upper end of the abstract's "0.05 to
   0.24 A/W" exists only with that ansatz, above omega tau_ee = 1.
4. **A finer amplitude ladder** around 0.3 v_F and near v_F, or
   interpolated limits.
5. **Finite-temperature coefficients at the low-density edge**, where the
   gas is less degenerate than the 300 K reference case and where the
   best noise figures and efficiencies come from; and the scattering
   times held fixed over a twenty-fold range of density.
6. **The alternative contact condition at large signal**, and a remark
   that the friction and viscosity of Eq. (A6) are not relativistic.
7. **The plain-channel control drawn in Fig. 6(b)**, with its frequency
   range stated.
8. **In the limitations paragraph:** that 0.3 v_F is an assumption and
   how the conclusions scale with it; that the large-signal solver lacks
   the viscoelastic term; the disagreement with Tomadin and Polini; that
   the ideal-gas ceiling is one device on a coarse ladder; the numerical
   floor of the shape-only response; the dependence of the 300 K split
   on the lead length; the zero-temperature coefficients and fixed
   scattering times on the grid; no mesh convergence of the drift speed.

## 3. Equations

The reviewer derived everything from the drifting Fermi sea and checked
it with its own numerical codes (quadrature over the drifted sea at fixed
laboratory density; a time-domain integration of the one-dimensional and
of the axisymmetric nonlinear equations; linear wave speeds at several
drifts and angles). It read `tools/momentum_step0.py` only afterwards.

**No equation is wrong.** Verdicts:

| Item | Verdict | Agreement reached |
|---|---|---|
| momentum density and flux of the drifting gas (3/4, -1/4) | correct | 1e-7 at small drift; exact forms to 1e-12 up to 0.95 v_F |
| Eq. (8), coefficients -1/2 and -1/4, cancellation in one dimension | correct, with a caveat of wording | on paper; its consequences tested below |
| Eq. (A6), the exact form, and its relation to the relativistic Euler equation | correct | residual identities to 5e-13; wave speeds against the addition of velocities to 1e-15, five angles |
| Appendix A: closure, c_V = 1/8, first- and second-order equations, forcing | correct | closure to 8 digits; forcing against a time-domain integration in an annulus, 1e-10 to 8e-8 |
| Eq. (A5), the channel closed form, and the Dyakonov-Shur limit | correct | 6e-11 to 5e-10 against an independent time-domain code; 9e-13 against the Dyakonov-Shur formula |
| Eqs. (12), (13), Appendix C | correct | expansion to 1e-10; coefficient 0.4805 |
| contact that fixes Phi(n) + m* u^2/4 | correct, with a caveat of wording | invariant to 2e-11 along a steady radial flow |
| finite-temperature coefficients (-0.34, -0.17; -0.47, -0.24) | correct, with a caveat on the assumption | reproduced by quadrature |
| reference-point numbers (3.6 v_F, 0.04, 1.43 V, 1.49 V, 28 %) | correct | |

Not checked by this reviewer: that Eq. (A5) with alpha = 1/2 is Eq. (43)
of Tomadin and Polini (no access to the paper). I checked that in Stage
H6 (3e-10 at 45 points).

### 3.1 Caveats that need a change of wording

- **"To second order in u/v_F"** (`models.tex`, after Eq. (8)). Eq. (8)
  is exact to second order in the amplitude about uniform rest, which is
  what Appendix A needs. At finite density gradient it drops terms of the
  form u^2 grad n. No number depends on it.
- **"Conserves the energy per particle"** (`plasmonic.tex`, contact
  variant). Phi + m* u^2/4 is the energy carried per particle, the
  Bernoulli function (energy flux over particle flux). The energy density
  per particle is a different quantity, with u^2/2.
- **The finite-temperature variant is the isothermal limit and should be
  called an upper bound.** For adiabatic flow the two coefficients are
  exactly -1/2 and -1/4 at any temperature, and only the mass changes
  (by 1.215 at 300 K). At 0.3 to 1 THz, exchange of energy with the
  lattice within a period is doubtful, so adiabatic is at least as
  defensible. "A fifth upward" is therefore the most the temperature can
  do to these two coefficients, not an estimate. Which temperature is
  held fixed also matters at this order, and the paper should say it is
  that of the laboratory frame. A consistent isothermal variant would
  also change the mass, the plasma speed and c_V; the variant run changed
  the two coefficients only.
- `models.tex`: replacing h by s_0^2 n/n_0 gives the shallow-water model
  only if the two new terms of Eq. (8) are also removed.
- The symbol nu is a dimensional viscosity in Eq. (8) and a dimensionless
  one in Appendix A; Appendix A does not say that n_1 in Eq. (A5) is per
  unit source amplitude; the caption of Fig. 6(a) says "units of the
  inverse gate overdrive" where the appendix uses V_u (they differ by
  4 %).

### 3.2 Two sentences of Sec. IV A that are not generally true

Using the first-order identity d/dx(|n1|^2 + |u1|^2) = -2 Gamma Re(n1 u1*),
the reviewer removed the integral from Eq. (A5):

    dU / U_a^2 = (1/4) [ (1 + alpha) |u1(0)|^2 - (1 - Phi'' + kappa) (|n1(L)|^2 - 1) ]

From this:
- "the density-dependent mass and the quantum capacitance add
  second-order terms that raise the response" (`plasmonic.tex:15-18`):
  they add (1/8)(|n1(L)|^2 - 1), which is positive only where the drain
  swing exceeds the source swing, near resonances. At 300 K it is
  negative over the whole band.
- "above the classical one" (`plasmonic.tex:22`): at 300 K the ordinary
  convective term with the same mass gives 0.79 to 0.93 of the classical
  response. (Also found by the numbers reviewer.)

### 3.3 "Half the classical response" is an exact identity, and a test the funnel does not pass at zero viscosity **[V, and open]**

With alpha = 0 the form above is exactly half the Dyakonov-Shur result
for every frequency, friction, length and q. The reviewer extended it:
without viscosity the first-order velocity is a gradient, and the
time-averaged equations of the Dirac gas become those of the classical
gas with the dc current and voltage both halved, in any geometry and for
any linear terminations. I re-derived this and it holds: both problems
reduce to Gamma J + grad Psi = c Gamma Re(n1 u1*) with c = 1/4 and 1/2
and contact offsets |u1|^2/8 and |u1|^2/4.

I then ran the small-signal solver with zero viscosity for both gases:

| device, DS termination, 100 K | 0.1 THz | 0.3 THz | 1 THz |
|---|---|---|---|
| plain channel with the antenna (Dirac over classical) | 0.4997 | 0.4973 | 0.4927 |
| 1 um funnel | 0.155 | 0.288 | 0.428 |

and the 1 um funnel at 0.1 THz with a small viscosity (Dirac, classical,
ratio): 0.0316, 0.0554, 0.57 at 0.02 m^2/s; 0.0267, 0.0499, 0.54 at
0.005; 0.0192, 0.0431, 0.45 at 0.001; 0.0050, 0.0321, 0.155 at zero. The
production value at 0.1 m^2/s is 0.0239.

So the solver satisfies the identity in the channel and does not approach
it in the funnel as the viscosity goes to zero; the funnel's response
changes six-fold between 0.02 m^2/s and zero. Two readings, not yet
separated:
1. **Numerical.** Corner nodes are pinned (zero velocity) even for slip
   walls. Without viscosity that constraint makes the discrete flow
   rotational near the corners, where the identity needs it to be a
   gradient. The viscous boundary layer sqrt(nu/omega) is 40 nm at
   0.001 m^2/s and 0.1 THz, below the 80 nm wall mesh, so the low
   viscosity runs above are not resolved.
2. **Physical, within the model.** With viscosity, pinned corners and
   curved slip walls generate vorticity, and the rectified current of the
   funnel depends on it. Then the paper's statement that viscosity "is
   invisible below 1 THz" holds for the channel only, and the funnel
   numbers depend on the viscosity and on the corner condition more than
   the paper says.

What is established: the production runs passed a mesh test at their own
viscosity (1.8 % and 1.1 %, test P8, 175 nm funnel). What is not: the
same test for the 1 um funnel at 300 K and 1 THz, where sqrt(nu/omega) is
about 70 nm against an 80 nm wall mesh; and how much of each funnel
number is carried by the corner condition. This is the one finding of
the equations review that could move results, and it predates the
seventh draft.

**Proposed check** (small-signal solver, well under an hour): the 1 um
funnel at 100 K and 300 K, three frequencies, on wall meshes of 80, 40
and 20 nm, at the production viscosity and at a fifth of it, for both
gases; and the same with the corner nodes left free to slip.

**Outcome (done).** `tools/momentum_corner.sh`, 80 runs of the
small-signal solver at 0.1, 0.3 and 1 THz, on wall meshes of 80, 40 and
20 nm (18 601, 41 509 and 98 963 nodes); `gd_thz_sweep` has a new
option `--corner_deg 181` that leaves the corner nodes free to slide
along the mean tangent. Tables: `results/thz2/corner/tables.md` from
`tools/momentum_corner.py`. A fifth of the viscosity was run on the 80
and 40 nm meshes only.

1. **The failure of the identity is numerical.** Without viscosity the
   ratio of the Dirac to the classical current, DS termination, rises
   toward 1/2 as the mesh is refined:

   | corners | f (THz) | 80 nm | 40 nm | 20 nm | extrapolated |
   |---|---|---|---|---|---|
   | pinned | 0.1 | 0.155 | 0.251 | 0.310 | 0.41 |
   | pinned | 0.3 | 0.288 | 0.343 | 0.378 | 0.44 |
   | pinned | 1 | 0.428 | 0.450 | 0.464 | 0.49 |
   | free | 0.1 | 0.203 | 0.300 | 0.375 | 0.62 |
   | free | 0.3 | 0.313 | 0.372 | 0.419 | 0.60 |
   | free | 1 | 0.436 | 0.458 | 0.474 | 0.52 |

   The observed order is 0.3 to 0.7, which is what a re-entrant corner
   gives, and the extrapolations (Richardson with the observed rate,
   so rough) fall on both sides of 1/2. Freeing the corners moves the
   numbers by a fifth and does not restore the identity on any mesh.
   Without viscosity neither gas is converged: the Dirac current at
   0.1 THz doubles between 80 and 20 nm. With the symmetric termination
   the two currents at 0.1 THz fall by a third to a half over the same
   refinement and nothing can be read from their ratio (1.4 on every
   mesh). My reading, not a proof: at zero viscosity the corners feed a
   spurious current that is 1.4 times larger for the Dirac gas and
   shrinks slowly; the combination of the two DS currents that cancels
   it is the same on all six mesh and corner variants to 1 %.
   **Consequence:** the solver must not be used without viscosity for a
   channel with corners. No number in the paper is: its zero-viscosity
   runs are the uniform channel of Fig. 6(a), where the flow is
   one-dimensional and the identity holds to 1.5 % or better, and every
   run of a funnel or a valve has the production viscosity (0.1 m^2/s
   at 100 K, 0.03 m^2/s at 300 K).

2. **At the production viscosity the corner condition does not
   matter**, and the difference vanishes with the mesh: free over pinned
   for the DS termination is 0.92 to 1.01 at 80 nm and 0.97 to 1.00 at
   20 nm; for the shape alone at 100 K, 0.79 and 0.81 at 80 nm and 0.94
   at 20 nm (0.1 and 0.3 THz). The pinned variant, which every run
   uses, is the one closer to the fine-mesh value.

3. **The production mesh of the 1 um funnel (80 nm) is too coarse at
   100 K.** Current responsivity in A/W, pinned corners:

   | set, termination | f (THz) | 80 nm | 40 nm | 20 nm | extrapolated | 80 nm is low by |
   |---|---|---|---|---|---|---|
   | 100 K, DS | 0.1 | 0.148 | 0.151 | 0.152 | 0.153 | 3 % |
   | 100 K, DS | 0.3 | 0.218 | 0.230 | 0.238 | 0.249 | 12 % |
   | 100 K, DS | 1 | 0.211 | 0.231 | 0.237 | 0.240 | 12 % |
   | 100 K, shape only | 0.1 | 0.0192 | 0.0243 | 0.0269 | 0.030 | 35 % |
   | 100 K, shape only | 0.3 | 0.0777 | 0.0875 | 0.0933 | 0.102 | 24 % |
   | 100 K, shape only | 1 | 0.0028 | 0.0061 | 0.0081 | 0.011 | not resolved |
   | 300 K, DS | 0.1 | 0.0559 | 0.0557 | 0.0556 | 0.0555 | under 1 % (high) |
   | 300 K, DS | 0.3 | 0.0556 | 0.0555 | 0.0555 | 0.0553 | under 1 % (high) |
   | 300 K, DS | 1 | 0.0506 | 0.0515 | 0.0516 | 0.0517 | 2 % |
   | 300 K, shape only | 0.1 | 1.7e-4 | 2.6e-4 | 2.9e-4 | 3.1e-4 | 46 % |
   | 300 K, shape only | 0.3 | 0.00183 | 0.00191 | 0.00194 | 0.00197 | 7 % |
   | 300 K, shape only | 1 | 3.8e-4 | 4.0e-4 | 4.2e-4 | - | not resolved |

   So the 300 K numbers with the DS termination stand. The 100 K ones
   are low by 3 to 12 % and the shape-only ones by a quarter to a
   third; the share of the shape at 100 K and 0.3 THz is 0.36 on the
   production mesh and 0.39 to 0.41 on the fine one, so "a third"
   survives. The supplement gives a mesh uncertainty of 2 % (test P8,
   which is the 175 nm funnel) and applies it to the 1 um funnel
   (`supplement.tex:405`); that is wrong for 100 K. The large-signal
   runs and the density and gate grid are on the same 80 nm mesh; their
   ratios to the square law are not affected, their absolute levels at
   100 K are.

4. **At 100 K the funnel's response depends on the value of the
   viscosity**, which the paper takes as an assumption. With a fifth of
   it (0.02 m^2/s; 40 nm mesh, pinned corners), relative to the
   production value: DS termination 1.37, 0.75 and 0.79 at 0.1, 0.3 and
   1 THz; shape alone 4.8 and 0.58 at 0.1 and 0.3 THz and a change of
   sign at 1 THz. These runs are less converged than the production
   ones (the shape-only values change by a quarter between 80 and
   40 nm), so the factors are indicative. At 300 K a fifth of the
   viscosity changes the DS response by 3 to 10 % and the shape-only
   response by 4 to 10 % at 0.3 and 1 THz (0.45 at 0.1 THz, a number of
   2e-4 A/W). The sentence quoted in reading 2 above, that viscosity is
   "invisible below 1 THz", is not in the draft; it was the reviewer's
   paraphrase. But the limitations paragraph of Sec. VI names two
   choices that move the fluid numbers (the contact condition and the
   finite-temperature coefficients) and not this one, which at 100 K is
   the largest of the three for the shape-only response. It also
   supports the reading of 1.3 that the two solvers disagree on the
   drift at 100 K because the time-domain solver lacks the viscoelastic
   term.

### 3.4 Outcome of the fine-mesh sweeps of the 1 um funnel (2026-11-19) **[V]**

`tools/momentum_runs.sh fine`: the four sweeps `S2_funnel_w1000_*` (100 K
and 300 K, DS and symmetric, 25 frequencies) on the 20 nm mesh (98 963
nodes) and on the 40 nm mesh (41 509 nodes), three checks of the interior
element size, and one grid point. Results in `results/thz2/fine`, tables
in `results/thz2/tables_fine.md`, figure `results/thz2/fine/mesh_80_20.png`,
all from `tools/momentum_fine.py`. Each 20 nm solve takes 20 s, not the
minutes I had estimated.

1. **Up to 1 THz the 20 nm sweeps are close to the mesh limit**, and the
   three meshes give an error estimate at every frequency:

   | 1 um funnel, 10 GHz to 1 THz unless stated | 80 nm (paper) | 20 nm | left at 20 nm |
   |---|---|---|---|
   | 100 K, DS (A/W) | 0.142 to 0.238 | 0.143 to 0.259 | 0 to 6 % |
   | 100 K, DS, 0.3 to 1 THz | 0.18 to 0.24 | 0.20 to 0.26 | 0 to 6 % |
   | 100 K, shape only, 0.1 to 1 THz | 0.025 to 0.084 | 0.034 to 0.100 | 0 to 11 % |
   | 100 K, share of the shape, 0.3 to 1 THz | 0.29 to 0.37 | 0.27 to 0.38 | |
   | 100 K, NEP, 0.3 to 1 THz (pW/Hz^1/2) | 24 to 32 | 22 to 28 | |
   | 100 K, efficiency per watt, 0.3 to 1 THz (1/W) | 1.4 to 2.4 | 1.8 to 2.9 | |
   | 300 K, DS (A/W) | 0.052 to 0.066 | 0.053 to 0.066 | 0.3 % |
   | 300 K, shape only, 0.3 to 1 THz | 0.8e-3 to 2.3e-3 | 0.9e-3 to 2.3e-3 | 0 to 5 % |
   | 300 K, share of the shape, 0.3 to 1 THz | 1.5 to 4.1 % | 1.6 to 4.2 % | |
   | 300 K, NEP, 0.3 to 1 THz | 97 to 103 | 97 to 102 | |

   The 100 K error of the 80 nm mesh is larger between the three
   frequencies of the corner check than at them: 12, 15 and 21 % at
   0.45, 0.57 and 0.72 THz. The amplitude at which the drift reaches
   0.3 v_F, the absorbed fraction and every 300 K number below 1 THz
   are the same on both meshes to 1 to 5 %. The dc resistance at 100 K
   is 172.1 ohm against 167.9 ohm (2.5 %).
   **For the text:** the 100 K range becomes 0.14 to 0.26 A/W, and with
   it the abstract's and the introduction's "0.05 to 0.24"; "a third"
   for the share of the shape and all the 300 K statements stand.

2. **The shape-only response at the lowest frequencies is not resolved
   on the 80 nm mesh, even in sign.** At 300 K below 0.085 THz it is
   -1.3e-4 to -1e-5 A/W on the 80 nm mesh and -1e-5 to +4e-5 on the
   20 nm one; at 100 K below 0.02 THz, -1.4e-3 to -6e-4 against +6e-4 to
   +1.5e-3. Fig. 6(b) draws these with open symbols as a negative
   response. With the earlier finding for the 175 nm devices: nothing
   below about 5e-4 A/W is established on the production meshes.

3. **Above about 1.2 THz no mesh used resolves the 1 um funnel.** The
   production mesh has elements of up to 480 nm inside the leads, and
   the plasma wavelength is 3.6 um at 1 THz and 1.2 um at 3 THz: 7.4
   and 2.5 elements per wavelength. DS termination:

   | f (THz) | 100 K: 80 nm | 40 nm | 20 nm | 300 K: 80 nm | 40 nm | 20 nm |
   |---|---|---|---|---|---|---|
   | 1.16 | 0.163 | 0.185 | 0.196 | 0.0488 | 0.0505 | 0.0507 |
   | 1.47 | 0.152 | 0.175 | 0.192 | 0.0445 | 0.0482 | 0.0490 |
   | 1.87 | 0.182 | 0.149 | 0.215 | 0.0387 | 0.0450 | 0.0472 |
   | 2.37 | 0.021 | 0.129 | 0.179 | 0.0320 | 0.0378 | 0.0454 |
   | 3.0 | -0.148 | 0.048 | 0.115 | 0.0249 | 0.0274 | 0.0435 |

   A mesh with 80 nm at the walls and 120 nm inside gives the 20 nm
   values at 300 K (0.0474 and 0.0440 at 1.87 and 3 THz), so at 300 K
   the interior size is the whole cause; at 100 K both sizes matter.
   The fall of the 300 K response above 1 THz and the change of sign at
   3 THz at 100 K in Fig. 6(b) are artifacts. A further check with 60 nm
   inside (80 nm walls, 105 889 nodes, 300 K) gives 0.0476 and 0.0446
   at 1.87 and 3 THz against 0.0474 and 0.0440 with 120 nm, so ten
   elements per wavelength are enough to 1.5 %: **at 300 K the 20 nm
   sweep is good to about 3 % up to 3 THz; at 100 K it is good to 6 %
   up to 1.2 THz and not converged above**, where the wall mesh still
   moves the answer (0.157 against 0.115 A/W at 3 THz between 80 and
   20 nm walls).
   **Affected in the paper:** Fig. 6(b) above 1.2 THz; the NEP quoted
   "between 0.3 and 2 THz" for the 1 um funnel (`plasmonic.tex:101`);
   the "numerical floor" of the plain-channel control, which grows with
   frequency for this reason (`plasmonic.tex:57-58`); the large-signal
   ladders at 1 THz (7.4 elements per wavelength: 1.4 % at 300 K, about
   12 % at 100 K in the small-signal level). The 175 nm devices are not
   affected: their mesh has 84 nm inside, 14 elements per wavelength at
   3 THz, and the 14 and 20 nm meshes agree at the taper resonance to
   0.2 %.

4. **The density and gate grid is sound at 0.1 and 0.3 THz and not at
   1 THz.** The plasma speed on the grid goes down to 0.98e6 m/s (5e10
   cm^-2, 5 nm), where the production mesh has 6.8 elements per
   wavelength at 0.3 THz and 2.0 at 1 THz. That point on the 20 nm
   mesh: 0.98 and 1.00 of the 80 nm responsivity at 0.1 and 0.3 THz at
   100 K (1.00 and 1.01 at 300 K); at 1 THz +1.83 against -0.36 A/W at
   100 K and 0.36 against 0.23 at 300 K. Figure 8 uses the grid at
   0.3 THz and the reference point at 1 THz, so it stands. What does
   not: any statement about grid points other than the reference at
   1 THz, among them the conclusion's range "at every density and gate
   distance" where it includes 1 THz (1.3), and item 6 of
   `results/HYDRO_MOMENTUM.md` on the low-density, thin-gate corner at
   1 THz.

5. **What is still on the 80 nm mesh:** all large-signal runs. They
   cannot be repeated on the 20 nm mesh here; their ratios to the
   square law of the same mesh stand, and their absolute levels at
   100 K carry the errors above. (The other small-signal runs were
   re-run on 2026-11-20, section 3.5.)

### 3.5 The other small-signal runs of the 1 um funnel on the 20 nm mesh (2026-11-20) **[V]**

`tools/momentum_runs.sh fine2`, 127 runs, two and a quarter hours at two
jobs: the contact variants and the finite-temperature coefficients (12
sweeps), the plain-channel control (4 sweeps, 94 887 nodes), the density
and gate grid with the 75 ohm antenna, the matched antenna and the
density-dependent scattering time (75 runs at 0.1, 0.3 and 1 THz), and
the contact resistances (36 runs). Files in `results/thz2/fine` under
the names of the 80 nm runs; tables in `results/thz2/tables_fine2.md`
from `tools/momentum_fine2.py`.

1. **The variants are ratios and hardly move.** Over 0.1 to 1 THz, DS
   termination, variant over default on the same mesh:

   | variant | 100 K, 80 nm | 100 K, 20 nm | 300 K, 80 nm | 300 K, 20 nm |
   |---|---|---|---|---|
   | contact conserves the energy per particle | 0.56 to 0.93 | 0.65 to 0.93 | 0.64 to 0.74 | 0.65 to 0.74 |
   | the opposite sign | 1.08 to 1.44 | 1.07 to 1.35 | 1.26 to 1.36 | 1.26 to 1.36 |
   | finite-temperature coefficients | 1.00 to 1.03 | 1.00 to 1.02 | 1.16 to 1.23 | 1.16 to 1.23 |

   The paper's "lowers the DS responsivities of the funnels by up to a
   third" (item d of 1.2 had found 44 % on the 80 nm mesh) is right on
   the 20 nm mesh: 35 % at most, at both temperatures. "Raise them by a
   fifth" at 300 K stands.

2. **The plain-channel control.** With the DS termination the channel
   is the same on both meshes to 4 % (0.108 to 0.190 A/W at 100 K and
   0.062 to 0.067 at 300 K from 0.1 to 1 THz). The funnel gives 0.83 to 0.85
   of it at 300 K on either mesh, so **"at room temperature the
   constriction takes 15 % away" stands**; at 100 K the funnel gives
   1.09 to 2.36 times the channel from 0.1 to 1 THz (1.39 to 2.36 from
   0.3 to 1 THz; 1.05 to 2.29 on the 80 nm mesh). **The "numerical
   floor" was the interior mesh:** the symmetric channel, whose exact
   response is zero, gives at most 3e-6 A/W up to 1 THz on the 20 nm
   mesh at 100 K (7.9e-3 on the 80 nm mesh) and 7e-7 at 300 K (8.6e-5).
   The sentence of `plasmonic.tex:55-59` that puts an 11 % floor under
   the funnel's shape-only response at 1 THz can go.

3. **The grid at 0.1 and 0.3 THz is confirmed.** The two meshes agree
   to 1 % at every point at 300 K and to 2.5 % at all but two points
   at 100 K:
   the reference point (20 nm over 80 nm: 1.02 and 1.09 at 0.1 and
   0.3 THz) and (2e11 cm^-2, 30 nm) (1.05 at 0.3 THz). The quantities
   of Sec. IV C at 0.3 THz, over the valid points:

   | | 100 K, 80 nm | 100 K, 20 nm | 300 K, 80 nm | 300 K, 20 nm |
   |---|---|---|---|---|
   | power at a drift of 0.3 v_F (uW) | 0.40 to 67 | 0.38 to 64 | 19 to 125 | 20 to 126 |
   | efficiency there | 3.5e-5 to 5.2e-4 | 4.0e-5 to 5.4e-4 | 5.6e-5 to 1.1e-4 | 5.6e-5 to 1.1e-4 |
   | NEP (pW/Hz^1/2) | 1.35 to 26 | 1.32 to 24 | 27 to 96 | 27 to 96 |
   | matched antenna, efficiency | 3.6e-5 to 5.4e-4 | 4.1e-5 to 5.6e-4 | 5.6e-5 to 1.2e-4 | 5.6e-5 to 1.25e-4 |

   So Fig. 8 and the numbers of Sec. IV C change only at the reference
   point at 100 K: its NEP at 0.3 THz is 24, not 26 pW/Hz^1/2, and its
   efficiency at the limit 4.0e-5, not 3.5e-5.

4. **The grid at 1 THz was wrong on the 80 nm mesh and is now usable.**
   20 nm over 80 nm responsivity at 1 THz: 1.13 at the reference point
   and 1.3 to 15 elsewhere at 100 K, with the opposite sign at two
   points; 1.02 to 1.58 at 300 K. On the 20 nm mesh the efficiency at
   the drift limit at 1 THz is 4.7e-5 to 3.5e-4 at 100 K and 4.5e-5 to
   5.5e-5 at 300 K over the valid points; the 1.7e-7 that contradicted
   the conclusion's range (1.3) was a mesh artifact. Over 0.1, 0.3 and
   1 THz together the range is 2.2e-5 to 5.4e-4 at 100 K and 4.5e-5 to
   1.7e-4 at 300 K, so **"1e-5 to 5e-4 at every density and gate
   distance" now holds at all three frequencies.** The slowest plasma
   wave has 8 interior elements per wavelength on this mesh at 1 THz;
   by the 60 nm check of 3.4 that is an error of a few per cent, on top
   of the wall-mesh error at 100 K.

5. **Contact resistance.** At 300 K the NEP with 100, 300 and
   1000 ohm um is the same on both meshes (132, 228, 840 pW/Hz^1/2 at
   0.1 THz; 138, 250, 986 at 0.3 THz; 149, 271, 1080 against 153, 278,
   1107 at 1 THz; reference point). At 100 K it is 2 to 16 % lower on
   the 20 nm mesh (35, 66, 318 against 38, 71, 336 at 0.3 THz). The
   loss of efficiency relative to ideal contacts is the same to 9 %.

## 4. What I would do, in order

1. ~~Re-run the 175 nm funnel and the Tesla valve on the 14 nm mesh~~
   Done (section 1.1, outcome).
2. ~~Run the corner and viscosity check~~ Done (section 3.3, outcome).
   ~~Re-run the four headline sweeps of the 1 um funnel on the 20 nm
   mesh~~ Done (section 3.4). ~~Re-run its other small-signal runs~~
   Done (section 3.5). No run is outstanding. The numbers are settled
   as far as this machine allows: the large-signal runs stay on the
   80 nm mesh.
3. **Correct the text** in one pass afterwards (done, see the end of
   this item):
   - new, from the two runs: the two Tesla numbers of 1.1; the mesh
     uncertainty of the 1 um funnel in the supplement (3.3, outcome 3);
     the viscosity among the choices that move the 100 K numbers (3.3,
     outcome 4); Figs. 6(c, d) and 13 and Table III regenerated;
   - `paper/make_figures.py` and `tools/momentum_report.py` read the
     1 um funnel's small-signal files from `results/thz2/fine` (3.4,
     3.5), and the supplement states the 20 nm mesh for them and the
     80 nm mesh for the large-signal runs;
   - new, from 3.5: the reference point's NEP and efficiency at 100 K
     in Sec. IV C; the floor sentence of the plain-channel control
     removed; the conclusion's efficiency range kept, now with support
     at 1 THz;
   - new, from the fine-mesh sweeps (3.4): the 1 um funnel numbers of
     Sec. IV A, the abstract and the introduction from the 20 nm mesh
     (0.14 to 0.26 A/W at 100 K); Fig. 6(b) from the 20 nm sweeps and
     cut at 1.2 THz, or shown beyond with the caveat; no statement about
     the 1 um funnel above 1.2 THz or about the grid at 1 THz away from
     the reference point; the supplement's mesh paragraph rewritten;
   - the two limits in the abstract, introduction and conclusion (2.1);
   - the efficiency ceiling restated as what was computed (2.2), with the
     0.3 v_F cutoff named as an assumption (2.3);
   - band ranges in place of sampled values (1.2, 1.3), which needs
     `tools/momentum_report.py` to report ranges over stated bands;
   - the solver named wherever the two disagree (1.3, 2.4 a, b);
   - the leftovers (2.5), the unsupported causes (2.6), the captions
     (2.7), the definitions (2.8) and the wording caveats (3.1, 3.2);
   - a sentence on the disagreement with Tomadin and Polini (2.9, item 1)
     and the bridge to the incompressible model of Sec. III B (2.5).
   **Done, 2026-11-20 (eighth draft).** What the pass changed:
   - *Sources.* `tools/paper_numbers.py` (new) writes every fluid number
     as a band range to `results/thz2/paper_numbers.md`;
     `tools/momentum_report.py` is left as the before/after record of
     Stage H6. `paper/make_figures.py` reads the 1 um funnel's
     small-signal files, the grid and the contact resistances from
     `results/thz2/fine`.
   - *Figures.* Fig. 6: (a) axis in units of 1/V_u; (b) from the 20 nm
     runs, 100 K curves ending at 1 THz; (c, d) from the 14 nm runs; a
     grey band where the shape-only response is not resolved. Fig. 8 and
     the plasmonic curves of Fig. 13 from the 20 nm runs. The band of the
     plasmonic funnel in Figs. 10 and 11 is 0.05 to 0.26 A/W.
   - *Sec. IV A* rewritten with band ranges (1.2 a, c, d; 1.3; 3.4; 3.5):
     0.14 to 0.26 A/W, the share of the shape as 0.27 to 0.38 and 1.6 to
     4.2 %, the control without the floor sentence, the two Tesla numbers,
     the NEP by band, the contact and temperature variants as ranges, the
     viscosity as a third choice, the mesh error stated.
   - *Sec. IV B:* the crossing of 0.3 v_F interpolated (26, 38, 104 uW at
     100 K; 100 to 130 uW at 300 K), the disagreement of the two solvers
     at 100 K said in the text and in Appendix B (1.3, 2.4 a), the cause
     of the rise above the law no longer asserted (2.6), the 0.09 % as
     the last point of a coarse ladder (2.2), the symmetric ladder at
     1 THz reported (2.8), the antenna as dc load as 15 to 24 %.
   - *Sec. IV C:* new title, the reference point from the 20 nm mesh, the
     thinner gate raising the limit said (2.6), the range over all three
     frequencies, the scaling with the square of the assumed limit (2.3).
   - *Abstract, introduction, conclusion, discussion:* the two limits no
     longer swapped (2.1); the ceiling restated as what was computed
     (2.2); "saturation" removed (2.5); the gate as a condition removed
     from the abstract (2.6); mixed normalisations flagged (2.6); the
     ballistic efficiency no longer called lower (2.4 d); Tables III and
     IV; the limitations paragraph extended (2.9, item 8).
   - *Sec. II and III:* the drift limit defined and called an assumption
     (2.3, 2.8); P_0 defined; "third and fourth terms" (2.8); the relation
     to Tomadin and Polini and the bridge to the incompressible model
     (2.5, 2.9); the three models named (classical gas, ordinary
     convective term, Dirac gas); the gated against incompressible
     comparison for both gate connections; 1.7 % at 50 mV.
   - *Appendix A:* the form without the integral and the half-response
     identity (3.3), as a new Eq. (A6); the exact equation is now (A7).
   - *Supplement:* P15, P17, the mesh of the gated runs (1.2 h, i; 1.4), a
     new Table S10 and three paragraphs on mesh convergence (3.3, 3.4),
     the production meshes, the earlier-draft wording removed (2.5).
   - *Left as it was:* Fig. 7 (the runs are unchanged); the ballistic
     sections apart from the plasmonic numbers they quote; no reference
     or estimate was added for the 0.3 v_F cutoff, which is now stated as
     an assumption (2.9, item 2); the location of the largest drift is
     not given.
   - *The one comparison that was still on the 80 nm mesh* (the 1 um
     funnel against the ordinary convective term) was repeated on the
     20 nm mesh on 2026-11-20 (`tools/momentum_runs.sh legacy20`, two
     sweeps). Dirac gas over ordinary term, DS termination: 0.57 to 1.04
     at 100 K and 0.58 to 0.94 at 300 K from 10 GHz to 1 THz (0.57 to
     1.05 and 0.58 to 0.94 on the 80 nm mesh). The three sentences that
     rest on it ("up to 1.7 times larger", "between nothing and four
     tenths", "about four tenths") stand unchanged; the supplement now
     names the 20 nm mesh.
4. **Decide** whether to add the runs a referee would ask for: a finer
   amplitude ladder near the two limits, a 100 K small-signal curve
   without the viscoelastic term, the contact variant at large signal
   (2.9).

Also to be corrected, in `results/HYDRO_MOMENTUM.md`: items 3, 7, 8 and
12 quote sampled values as ranges, and every before/after ratio for the
175 nm funnel and the Tesla valve.
