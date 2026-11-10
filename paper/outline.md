# Paper outline: geometry does not make a graphene THz rectenna

Drafted 2026-11-09 for review, before any manuscript text. Every number
below is taken from a results file in this repository and names it; nothing
here is new computation. Sources: `results/KILLTEST.md` (K), `HYDRO.md`
(H), `CLASSIC.md` (C), `THZ.md` sections S1 to S6 (T-S1 ...),
`BALLISTIC_THZ.md` (B), and the literature review in
`reports/Tesla valve graphene diode feasibility.md` (R).

## 1. The claim, in one sentence

Across ballistic, viscous and plasmonic transport, a Tesla-valve channel in
graphene never rectifies usefully, a funnel does, and at THz the funnel's
rectification is the circuit asymmetry of a gated channel (Dyakonov-Shur),
not its shape: the element is a low-noise square-law detector with a
conversion efficiency below one per cent, and no operating point, gate,
density or transport limit turns it into a harvester.

## 2. Working title and abstract

Title options (first is the recommendation):

1. *Shape versus circuit: why graphene geometric diodes and Tesla valves do
   not make terahertz rectennas*
2. *Geometric rectification in graphene from dc to terahertz: ballistic,
   viscous and plasmonic limits of the funnel and the Tesla valve*
3. *Fields rectify, shapes do not: a three-regime study of graphene
   geometric diodes*

Abstract draft (about 190 words; numbers to be frozen with the figures):

> Geometric diodes, in which an asymmetric channel shape is meant to
> rectify, and electronic Tesla valves have both been proposed as
> ultrafast rectifiers for terahertz rectennas in graphene. We test the
> proposal in three transport limits with one set of device geometries. A
> semiclassical ballistic model reproduces the measured asymmetry of a
> funnel diode in the clean limit and gives none for any Tesla geometry.
> Steady viscous electron flow makes the classical Tesla valve work, but
> only at Reynolds numbers above the drift-velocity limit of the Fermi
> liquid. At terahertz frequencies the gated electron fluid is plasmonic:
> the funnel responds at 0.1 to 0.3 A/W, and nearly all of it is the
> Dyakonov-Shur asymmetry of the source and drain terminations, which any
> channel has. The rectified power follows the square law up to the gate
> overdrive and peaks below one per cent conversion efficiency for every
> density and gate distance; lowering either only moves the saturation
> power, from a milliwatt to a microwatt. Driving the ballistic funnel at
> terahertz closes the remaining route: its shape asymmetry survives the
> transit time only if the potential drop is confined to the neck, and
> then merely equals the plasmonic figure. The graphene funnel is a
> picowatt-per-root-hertz detector, not an energy harvester.

## 3. Decisions for you, with my recommendation

| decision | recommendation | alternative |
|---|---|---|
| Format | One full article, about 12 pages and 8 figures, with a supplement carrying the acceptance ladders | A 4-page letter (figures 1, 4, 6, 7, 8) plus a long supplement |
| Venue | Physical Review Applied, with arXiv posting | IEEE Trans. Electron Devices (where the geometric-diode Monte Carlo work appears) or J. Appl. Phys. |
| Framing | Lead with the rectenna question and the funnel; the Tesla valve is the second geometry, not the headline | Lead with the Tesla valve, as the project began |
| Geurs et al. | State plainly that steady incompressible electron hydrodynamics does not reproduce their D = 1.05 (we get below 3e-5), and that we do not explain it | Omit; I advise against, a referee will ask |
| Code and data | Cite the public repository and tag the commit; the supplement lists the ladder tests | Data on request |
| Disclosure | A methods statement that the code and analysis were developed with an AI coding assistant, as most journals now require | Your call on wording |

## 4. Section plan

Each section is listed with its single message, the numbers that carry it,
and where they come from.

**I. Introduction (1 page).** Rectennas need a rectifier with no junction
capacitance; geometric diodes (Zhu and Moddel; Nguyen et al. 2021, measured
asymmetry 1.66) and the electronic Tesla valve (Geurs et al. 2020, D = 1.05
in the viscous window) are the two shape-based candidates. Reciprocity
(Christen and Buttiker) forbids shape-only rectification in linear
response, so the question is which nonlinearity each shape feeds, and
whether it is fast and strong enough. Contribution: one geometry set, three
transport models with validated solvers, one answer per regime. (R)

**II. Devices and models (2 pages).** The funnel of Nguyen et al. Device D
(175 nm neck, 2 um leads, 30 degree taper), the classical four-loop Tesla
valve at equal minimum feature, a straight control, and 1 um scaled
versions that an antenna can drive. Three models, each one paragraph plus
its validation: (a) event-driven semiclassical trajectories with an imposed
potential and Landauer bias-window current; (b) steady incompressible
Navier-Stokes with Brinkman damping and Navier slip; (c) gated compressible
(barotropic) electron fluid, second-order perturbation plus a nonlinear
time-domain solver, lumped antenna ports. Parameter table: n, T, tau_mr,
tau_ee, nu, gate distance per regime. (K, H, T method, B method)

**III. dc: which shape rectifies, and by what (2 pages).**
- Ballistic. Funnel A = 1.62 at 0.1 V in the clean specular limit and 1.06
  to 1.09 at the measured 700 nm mean free path; every Tesla variant,
  including the classical valve, is 0.89 to 1.08 with no systematic sign,
  at 1.4 to 22 kohm against 0.5 to 2 kohm for the funnel. (K, C)
- Viscous. At the currents of Geurs et al. (Re 0.05 to 0.3) every geometry
  gives |D - 1| < 3e-5. The classical valve does work as a Tesla valve,
  D - 1 = 0.15 to 0.23 at Re 10 and above 1 at Re 30 with slip walls,
  growing as Re^2, but the drift-velocity ceiling caps Re at 0.5 to 2.6
  (175 nm) and 3 to 15 (1 um), where realistic damping and slip leave 1e-3
  to 3e-2.
  The funnel is a Borda-Carnot diode of the opposite sign, stronger than
  the valve below Re 3. (H, C)
- Message: the Tesla mechanism is inertial and needs a Reynolds number the
  Fermi liquid cannot carry; the funnel rectifies in both limits but weakly
  in the fluid.

**IV. Terahertz: the plasmonic element (3 pages).**
- Validation against the Dyakonov-Shur closed form to three digits in
  physical units; the Dirac closure adds 40 % near resonance. (T-S1)
- Small signal. The 1 um funnel with the DS termination gives 0.15 to 0.34
  A/W at 100 K and 0.07 to 0.09 A/W at 300 K from 10 GHz to 1 THz into 75
  ohm; with the symmetric termination that isolates the shape it gives
  0.13 A/W at 1 THz at 100 K and milliamps per watt at 300 K. The 175 nm
  funnel has a taper resonance at 1.87 THz where the shape term reaches
  0.22 A/W. The Tesla valve gives 1e-5 to 1e-3 A/W from its geometry below
  300 GHz, 1e-2 at one 100 K resonance, at 6 to 11 kohm. (T-S2, T-S3)
- Noise. Johnson-limited NEP 4 to 60 pW/Hz^1/2 with ideal contacts.
  (T-S4)
- Large signal. The square law holds to 100 uW everywhere and to 1 mW
  below 300 GHz; efficiency is (1 to 2.4 per watt) times the available
  power and peaks at 0.14 to 0.38 % at 1.8 mW; no breakdown up to the gate
  overdrive; the shape-only response saturates first. (T-S5)
- Operating point. The saturation power scales as (n d)^2, from 0.9 mW to 3
  uW; the peak stays at 0.03 to 0.9 % over the Fermi-liquid and gating
  window, thinner gates lower it, constant mobility takes back a factor 4
  to 6 at low density. Contact resistance acts on the DS asymmetry itself:
  300 ohm um costs a factor 4 to 11. Detector NEP improves to about 1
  pW/Hz^1/2 at low density at 100 K. (T-S6)

**V. Terahertz: the ballistic element (1.5 pages).** The kill-test funnel
under an ac imposed field with a two-sided estimator. Uniform field: a
third of the response is lost between 1 and 10 GHz to a slow-drift
population the taper feeds into the lead (5 % of source electrons fly
longer than 20 ps), the rest at the transit roll-off, zero at 300 GHz, 0.05
A/W at 1 THz. Drop confined to the neck: 0.37 A/W (500 nm) to 0.45 A/W
(neck width) at 1 THz in the clean limit; 0.2 A/W from a transit-time
effect at the 700 nm mean free path, where the static asymmetry is
reversed. Straight control zero at all frequencies. (B)

**VI. Discussion (1.5 pages).** The synthesis figure and table: mechanism,
magnitude and bandwidth per geometry and regime. Why the answer is the
same three times: the Tesla loss is inertial and long compared with the
plasmon wavelength; the funnel's fast nonlinearity is the gate, not the
taper. What a device designer should take away: use the funnel as a
matched horn, terminate it asymmetrically, put the saturation knee at the
available power, and spend the effort on contacts. Comparison with reported
graphene THz detectors. Limitations, taken unchanged from the results
files: local-capacitance electrostatics, no electron heating or
photothermoelectric response, imposed potential in the ballistic model, no
crossover model between the limits, classical trajectories above 50 nm
features. (T limitations, B limitations)

**VII. Conclusion (0.3 page).** The one-sentence claim, the detector
figures, and the two things that would change the answer: a mechanism
outside these models (thermoelectric, interband), or a material with a
higher drift-velocity ceiling.

## 5. Figures

All figures are redrawn to one style from the listed data; "exists" means a
stage plot already shows the content and only restyling is needed.

| # | content | data | status |
|---|---|---|---|
| 1 | Geometries to scale (funnel, classical valve, 1 um funnel, control) and a regime map: channel width against l_mr and l_ee, frequency against 1/tau_mr, 1/tau_ee and the plasmon and transit frequencies | `results/geom_*.svg`, parameter sets | new schematic |
| 2 | Ballistic dc: asymmetry against mean free path and specularity, funnel against all Tesla variants, with resistance | `killtest.csv`, `killtest_classic.csv` | new plot of existing data |
| 3 | Viscous dc: D - 1 against Re for the classical valve and the funnel at three slip lengths, survivable window shaded, Geurs point marked; inset speed fields forward and reverse | `hydro_S2*.csv`, `hydro_S4_classic_w1000.csv`, `hydro_tesla_classic_Re30_Sinf_{f,r}` | plot new, fields exist |
| 4 | Plasmonic small signal: R_I(f) for the 1 um funnel, DS against symmetric termination, 100 K and 300 K; the 175 nm taper resonance; the Tesla valve; inset DS validation | `thz/S1_*`, `S2_*`, `S3_*` | exists as four plots, to merge |
| 5 | Large signal: efficiency against available power with the square law, saturation factor, density swing | `thz/P1_*` to `P4_*` | exists |
| 6 | Operating point: saturation power and peak efficiency over (n, d) with the validity limits; efficiency and NEP against contact resistance | `thz/D1_*`, `D2_*` | exists, to condense from 18 panels to 4 |
| 7 | Ballistic THz: transfer S_B(f) for the two field models; R_I(f) against the plasmonic reference; flight-time tail | `bthz/B1`, `B2u`, `B2l`, `B3` | exists |
| 8 | Synthesis: responsivity against frequency for every mechanism and geometry on one axis, dc to 2 THz, with the detector state of the art as a band | all of the above plus literature | new |

Tables: (1) parameter sets and nondimensional groups; (2) verdict matrix,
geometry by regime, with magnitude and limiting factor; (3) detector
comparison with published graphene THz detectors.

## 6. Supplementary material

- The three acceptance ladders with their numerical results: T1 to T17
  (ballistic), H1 to H9 and U1 (viscous), T1 to T17 (plasmonic; renumber
  to avoid the clash with the ballistic ladder).
- The derivation of the second-order perturbation scheme
  (`plans/hydro_thz_notes.md`) and the port model.
- Full tables: `thz/tables.md`, `tables_H3.md`, `tables_H4.md`,
  `bthz/tables_B2.md`.
- The lessons that are results in their own right: contact resistance is
  not a series element at ac; the BDF2-consistent start of the time
  solver; the slow-drift population; the Dirac-cut bookkeeping.

## 7. What must be said plainly, because a referee will ask

1. **No experiment here.** Validation is against analytic limits and
   benchmarks (Sharvin, Clausing, Drude, reciprocity, Poiseuille,
   Kovasznay, backward-facing step, the Dyakonov-Shur formula) and one
   measured number, the asymmetry 1.66 of Nguyen et al.
2. **The agreement with that number is conditional.** The model gives 1.62
   only in the clean specular limit; at the reported 700 nm mean free path
   it gives 1.09. The uniform-field assumption and the edge specularity
   matter at the factor-two level (K). The paper states this and does not
   claim a quantitative fit.
3. **Geurs et al. is not reproduced.** Their signal is 2000 times our
   steady incompressible result at their current. The candidates our dc
   model excludes are gate compressibility and electron heating (H).
4. **The two THz models describe different idealisations of different
   devices.** The plasmonic element is gated, n = 1e12 cm^-2, 100 and 300
   K scattering sets; the ballistic element is the ungated kill-test
   device at 1.4e12 cm^-2 with an imposed field. The comparison in section
   V is of responsivities into the same 75 ohm source, not of one device in
   two regimes.
5. **Idealisations favour the device.** Ideal contacts in S2 to S5, perfect
   antenna coupling, no 1/f or amplifier noise, local capacitance, specular
   walls and infinite mean free path in the best ballistic case. Every
   headline number is an upper bound, which is the right direction for a
   negative result.

## 8. Work needed before the manuscript is complete

None of this is new physics; each item is under an hour of compute or a
short literature task.

| item | why | cost |
|---|---|---|
| Mesh-convergence run on the classical valve at Re 3, 10, 30 | C carries the tesla1 convergence figures over without its own study | minutes of CPU |
| 300 K large-signal check at 120 steps per period | T-S5 item 3 notes it was only done at 100 K | about 30 min wall |
| One ballistic THz point with partly diffuse walls (p = 0.5) | B is specular only; shows the best case is the best case | minutes |
| Literature table of graphene THz detector responsivity and NEP | Table 3 and figure 8; the S4 comparison currently quotes ranges from memory | half a day, sources to be read |
| Reference check | R marks several sources as abstract-only or secondhand, and two as 2026 preprints; each citation must be verified against the paper before it goes in | with the literature task |
| Unify figure style and units | stage plots use different palettes and label conventions | with the figure redraw |

## 9. Reference skeleton (to be verified against sources)

Reciprocity and nonlinear transport: Christen and Buttiker 1996; Sanchez
and Buttiker 2004; Song 1999; Lofgren et al. 2004. Geometric diodes: Zhu
and Moddel 2013; Dragoman and Dragoman 2013; Auton et al. 2016; Nguyen et
al. 2021 and 2022; Stearns and Moddel 2021; Wang et al. 2021 and 2024;
Truccolo et al. 2024 and 2026. Tesla valves: Tesla 1920; Nguyen, Abouezzi
and Ristroph 2021; Geurs et al. 2020; Sarypov et al. 2026 (preprint); Huang
et al. 2024. Electron hydrodynamics in graphene: Bandurin et al. 2016 and
2018; Krishna Kumar et al. 2017; Kiselev and Schmalian 2019. Plasmonic
detection: Dyakonov and Shur 1993 and 1996; Vicarelli et al. 2012;
Bandurin et al. 2018 (resonant detection); a review of graphene THz
detectors. Methods: Jacoboni and Reggiani 1983; Taylor-Hood elements;
Engelman, Sani and Gresho on slip normals.

## 10. After approval

Order of work: the closing checks of section 8, then the eight figures in
one style, then the LaTeX draft section by section in `paper/`, with the
abstract and numbers frozen last. The draft comes back for review before
any submission step.

## 11. Status (2026-11-10)

Approved on 2026-11-09 with the recommended format, venue and framing. Done
since:

- Closing checks of section 8: mesh convergence of the classical valve
  (`results/checks/classic_mesh_hw*.csv`; converged to about 10 %, not the
  3 % carried over), 300 K large signal at 120 steps per period (shape
  unchanged to 1.2 %), ballistic THz with half-diffuse walls
  (`results/bthz/B6.csv`; the 1 THz response of the neck-confined field
  stays at 0.31 to 0.45 A/W although the static asymmetry vanishes).
- Literature: `literature_detectors.md` (20 verified rows) and
  `refs_verification.md` (41 of 43 entries verified, 2 corrected). Ten
  corrections changed statements in the draft; the two that matter most
  are that the Reynolds number attached to Geurs et al. is our estimate
  (they quote none; D = 1.05 is for holes at 6.3 uA) and that measured
  room-temperature graphene detectors are already at 100 to 130 pW/Hz^1/2,
  the level of our idealised 300 K figure, not one to two orders worse.
- Figures 1 to 8 (`make_figures.py`), manuscript (`main.tex`,
  `sections/`), supplement (`supplement.tex`).

- Compiled on 2026-11-10 with Tectonic 0.17: clean build, 16 pages plus a
  3-page supplement (`main.pdf`, `supplement.pdf`). Fixed on first
  compilation: three tables and one equation too wide, two undefined month
  abbreviations and four missing article numbers in `refs_detectors.bib`,
  and label collisions in Figs. 1, 2, 4, 5 and 7 at print size. The article
  is 4 pages over the 12 planned; nothing has been cut.

Open:

- Author affiliation and contact in `main.tex`.
- Unverified details listed at the end of `refs_verification.md` and
  `literature_detectors.md`: the backward-facing-step literature bands,
  Auton 2017 and Zak 2014 beyond their abstracts, Wang 2024 beyond its
  abstract, whether Geurs et al. has a journal version.
- Section 9 of this outline (reference skeleton) is superseded by
  `refs.bib`.
