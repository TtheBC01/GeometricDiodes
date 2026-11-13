# Prior-work search (2026-11-10)

A novelty search run after the first draft was compiled. Three independent
searches, one per strand of the paper, each asked to find published work
that already reaches the draft's conclusions or contradicts them. The
closest hits were then checked again at the source. Verification level:

- **A**: abstract or record page read directly in the second check.
- **F**: full text read by the search, not re-read in the second check.
- **S**: search snippet or secondary source only. Check before citing.

**Summary.** The draft is not scooped: no paper compares the three
transport regimes for these shapes, and several individual results appear
new. But the search found eleven works the draft must cite and four places
where the draft's conclusions are stated more broadly than the literature
allows. The title claim ("do not make terahertz rectennas") does not
survive as written.

## 1. Claims that must be narrowed

### 1.1 "The Tesla valve fails in every regime" (discussion.tex, line 7)

| Work | Level | What it shows |
|---|---|---|
| Sarypov, Pokhabov, Pogosov, Zhdanov, Shevyrin, Bakarov, "Electron Tesla valve", arXiv:2603.16443, accepted in Phys. Rev. Lett. (doi:10.1103/bwxl-wwt6) | A, F | GaAs two-dimensional electron gas, channels 2.5 and 5 um, 4 K. Forward and reverse curves coincide below about 300 uA; above it the reverse resistance rises to a diodicity of about 40. Authors estimate Re >= 7 from Re = I/(n e nu) with nu <= 0.06 m^2/s, electron temperature about 100 K, and attribute it to the onset of turbulence. No simulation. Calls the 5 % of Geurs et al. "vanishing". |

This paper has been in `refs.bib` and in the research notes since before
the draft, and the draft does not cite it. That is an error in the draft.
Our steady incompressible model gives D - 1 of order 0.1 at Re = 7, not 40,
so either the mechanism there is not the one we model (unsteady flow,
heating, compressibility, velocity saturation: their quoted drift velocity
of 2e5 m/s is a large fraction of the GaAs Fermi velocity) or our model
misses the regime. Either way the conclusion has to become "in graphene,
within steady incompressible flow below the drift limit", and the paper has
to discuss this experiment.

### 1.2 "The ballistic funnel is not a terahertz rectifier" (ballistic.tex)

The roll-off we find is set by the 6 um device and its 2 um leads. Transit
time scales with length, and the literature on shorter devices reports the
opposite conclusion:

| Work | Level | What it shows |
|---|---|---|
| Stearns and Moddel, "High frequency characteristics of graphene geometric diodes", Device Research Conference 2020, doi:10.1109/DRC50226.2020.9135150 | S (poster page), abstract via search | Time-dependent Monte Carlo of a graphene geometric diode, presented as the first of its kind. Rectification extends into the THz range with a cutoff near the Drude damping rate; no rectification above about 5 THz. **Prior art for our method; opposite conclusion.** |
| Zhu, Joshi, Grover, Moddel, J. Phys. D 46, 185101 (2013) (already cited) | F | We cite it for the proposal and omit its measurement: a 500 nm diode with a 75 nm neck in a bow-tie antenna, CO2 laser at 28 THz, zero bias, 190 pA measured against 270 pA predicted from the dc responsivity. The authors' only speed limit is RC (100 THz). |
| Do, Ngirmang, Wu, Bosman, "Nonlinear terahertz resonances from ballistic electron funnelling", ACS Nano 19, 14150 (2025), arXiv:2411.09212 | A | Self-consistent particle-in-cell simulation of ballistic funnelling in graphene resonators; rectification and second-harmonic generation from below 10 THz to tens of THz; edge scattering is essential. |
| Shi, Viti, Watanabe, Taniguchi, Vitiello, Small Science 6, e202500654 (2026), doi:10.1002/smsc.202500654 | S (conference abstract confirmed, journal paper not) | hBN-encapsulated four-terminal graphene ballistic rectifier at 2.8 to 3 THz, noise-equivalent power about 17 to 20 pW/Hz^1/2 at room temperature. |
| Hemmetter et al., "Enhanced coupling efficiency in geometric terahertz rectennas based on scalable CVD graphene", Discover Electronics 3, 128 (2026), doi:10.1007/s44291-026-00280-1 | S | CVD-graphene funnel rectennas at zero bias up to 0.68 THz; intrinsic responsivity 11.8 V/W (triple funnel), noise-equivalent power 1.9 nW/Hz^1/2. |
| Custer et al., Science 368, 177 (2020), doi:10.1126/science.aay8663 | F | Silicon nanowire geometric diodes, flat response from 100 MHz to 40 GHz; flight time expected to allow beyond 1 THz. |
| Bai, Zhao, Xu, Guo, "Origin of robust rectification in geometric diodes", arXiv:2101.09483 (Commun. Phys. 4, 236 (2021) per the search) | A | Attributes geometric-diode rectification to asymmetric bias-induced barrier lowering, not ballistic geometry; intrinsic rectification to 1.1 THz. Close in spirit to our "needs the bias landscape" point. |
| Ermann and Shepelyansky, Eur. Phys. J. B 79, 357 (2011), arXiv:1010.4441 | F | Ac-driven Monte Carlo of Dirac electrons on asymmetric antidots; ratchet current flat below omega r/v_F = 1 and falling above. Same transit scaling, no slow population. |

What survives: the identification of a slow, taper-scattered population
that sets a gigahertz roll-off when the field fills the leads was not found
anywhere. What does not: any statement that geometric diodes cannot rectify
at THz. The honest statement is about this device, the one with the best
documented dc asymmetry, and it calls for a simulation of a short device
(500 nm class) before the paper can say anything general.

### 1.3 "No harvesting regime" and the efficiency ceiling (plasmonic.tex, discussion.tex line 145)

| Work | Level | What it shows |
|---|---|---|
| Kachorovskii, Rumyantsev, Knap, Shur, "Performance limits for field effect transistors as terahertz detectors", Appl. Phys. Lett. 102, 223505 (2013), arXiv:1302.7089 | A, F | Defines the conversion efficiency Q = P_dc/P_THz of a FET detector. Small signal: Q proportional to (U_a/U_T)^2 with a pure-number prefactor of at most about 0.05. Large signal: absolute maximum "on the order of 10 %" in the subthreshold regime. **This is our efficiency law and its ceiling, eleven years earlier, in one dimension and without graphene.** |
| Gutin, Kachorovskii, Muraviev, Shur, "Plasmonic terahertz detector response at high intensities", J. Appl. Phys. 112, 014508 (2012) | F (abstract) | Analytical model at arbitrary intensity; response changes from quadratic to linear in amplitude at about the gate swing. Anticipates "square law up to the gate overdrive". |
| Rudin, Rupper, Gutin, Shur, J. Appl. Phys. 115, 064503 (2014); But et al., J. Appl. Phys. 115, 164514 (2014), arXiv:1402.0259 | F / abstract | Nonlinear hydrodynamic theory with viscosity; measured saturation at high intensity. |
| Khan, Lee, Monroe, Chandrakasan, Han, IEEE RFIC Symp. 2022, doi:10.1109/RFIC54546.2022.9863171 | F | 263 GHz CMOS FinFET harvester, 13.6 % measured efficiency. A transistor rectifier can harvest at sub-THz. |

What survives: the graphene-specific statement. A gapless channel has no
threshold, so the voltage scale is the gate overdrive and the ceiling is
below 1 % for every density and gate distance; plus the resonant and
viscous regime, and referral to antenna power. What does not: presenting
the efficiency law as new, or "no harvesting regime" as a statement about
plasmonic or transistor rectennas in general.

### 1.4 The Reynolds-number cap (dc.tex)

| Work | Level | What it shows |
|---|---|---|
| Lucas and Fong, "Hydrodynamics of electrons in graphene", J. Phys.: Condens. Matter 30, 053001 (2018), arXiv:1710.08425, Sec. 4.3.1 | A (ar5iv) | Reynolds number scales as (v_flow/v_F)(l_flow/l_ee); flows above 0.1 v_F reachable; "R ~ 10 may be sufficient to observe hints of nonlinear hydrodynamics". Our cap, in a review. |
| Mueller, Schmalian, Fritz, Phys. Rev. Lett. 103, 025301 (2009), arXiv:0903.4178 | F | Re formula with the factor u/v explicitly, u about 0.1 v. |
| Hui, Oganesyan, Kim, "Beyond Ohm's law: Bernoulli effect and streaming in electron hydrodynamics", Phys. Rev. B 103, 235152 (2021), arXiv:2010.00019 | A (abstract), F | Proposes the Bernoulli effect in a Venturi wedge as a nonlinear signature; the search reports Re about I/(160 uA) for graphene in the text, the same scaling as ours. Anticipates the funnel as a weak convective rectifier. |
| Monari et al., "Electron hydrodynamics and Bernoulli effect in Venturi-shaped 2D systems", Phys. Rev. B 114, 045305 (2026), arXiv:2607.24386 | A | Experiment: nonlinear voltage and diodicity in Venturi wedges, attributed to convective acceleration. With unequal lead widths the asymmetry is linear in current; our equal-lead funnel gives a quadratic one. |
| Gungor, Koziel, et al., "Modeling hydrodynamic charge transport in graphene", Materials 15, 4141 (2022), doi:10.3390/ma15124141 | F (page blocked in second check) | Compressible hydrodynamic simulation of a graphene Tesla valve at 300 K, diodicity 1.0655, claimed to agree with Geurs et al. The only prior electron Tesla-valve simulation. Superficially contradicts our "not reproduced". |
| Geurs et al., "Supersonic flow and hydraulic jump in an electronic de Laval nozzle", arXiv:2509.16321 | F | Bilayer-graphene nozzle driven to the sound speed; polarity-dependent resistance. Shape asymmetry at high drive is a compressibility effect, which our dc model excludes. |

Geurs et al. (arXiv:2008.04862) is still a preprint: one version, no
journal reference, not withdrawn. Its abstract already says the
rectification disappears in the ballistic and diffusive regimes, so our
"no ballistic Tesla rectification" is a simulation of a reported
observation, and the draft should say so.

## 2. Smaller points

- **Shape in plasmonic detectors.** Zhang and Shur, Appl. Phys. Lett. 119,
  161104 (2021), arXiv:2108.12877 (A): a non-uniform channel raises the
  responsivity of a Si detector by about 10 %. Khavronin et al., Phys. Rev.
  Applied 13, 064072 (2020), arXiv:2005.04739 (F, partly): a Corbino
  geometry concentrates the field at the inner contact and raises the
  responsivity strongly. Both support "the shape is a modest modifier of
  the boundary asymmetry", and the second qualifies "a factor of order
  one".
- **Contacts.** Castello et al., Front. Optoelectron. (2024),
  arXiv:2412.07279 (F): a tenfold lower access resistance gives up to
  twenty times the photocurrent, described by a series-resistance model.
  Lisauskas et al., J. Appl. Phys. 105, 114511 (2009): the gate-drain
  shunt at the signal frequency is what creates the mixing. Our "not a
  series loss" argument appears new but must be set against these.
- **Detector table.** Qin et al., Carbon 116, 760 (2017),
  doi:10.1016/j.carbon.2017.02.037 (S): bilayer graphene FET, 51 pW/Hz^1/2
  at 0.33 THz and room temperature. If confirmed, "the best measured are
  at 100 to 130" is out of date.
- **A contrary efficiency claim.** Dragoman and Aldrigo, Appl. Phys. Lett.
  109, 113105 (2016) (S): a graphene rectenna with a claimed 58 % at
  897 GHz. Not verified; a referee may raise it.
- **Not found.** No two-dimensional hydrodynamic simulation of a funnel or
  Tesla-valve gated graphene channel at THz. No comparison of ballistic,
  viscous and plasmonic rectification for one device. No estimate of the
  Reynolds number of the Geurs experiment, and no test of it against
  Navier-Stokes.

## 3. What still looks new

1. One set of shapes carried through three transport regimes with tested
   solvers.
2. The slow, taper-scattered electron population and the gigahertz
   roll-off it causes in a large funnel with a uniform field.
3. The Reynolds number of the Geurs experiment (0.04 to 0.2) and the
   finding that steady incompressible flow falls three orders short of it.
4. The two-dimensional plasmonic calculation for a funnel and a Tesla
   valve, with the symmetric-drive control that separates shape from
   termination.
5. The graphene-specific efficiency ceiling over carrier density and gate
   distance, and the non-series effect of contact resistance.

## 4. What the draft needs before submission

1. Cite and discuss Sarypov et al.; restrict the Tesla-valve conclusion to
   graphene below the drift limit and say what our model cannot address.
2. Cite Stearns and Moddel (2020), confront the 28 THz measurement of Zhu
   et al., and cite the 0.68 to 3 THz ballistic-rectifier experiments.
   Simulate a short funnel (500 nm class) at THz, or state the ballistic
   conclusion for the 6 um device only.
3. Cite Kachorovskii et al. (2013) and Gutin et al. (2012) and present the
   efficiency law as theirs, extended here to graphene. Remove "no
   harvesting regime" as a general statement.
4. Cite Lucas and Fong, Mueller et al., Hui et al. and Monari et al. for
   the Reynolds-number argument and the Bernoulli effect, and Gungor et al.
   for the earlier Tesla-valve simulation.
5. Reconsider the title and the last sentence of the abstract.
6. Read in full, before citing: Stearns and Moddel (2020), Shi et al.
   (2026), Hemmetter et al. (2026), Qin et al. (2017), Dragoman and
   Aldrigo (2016), Gungor et al. (2022).

## 5. Status (2026-11-11)

Acted on in the second draft of the manuscript:

1. Sarypov et al. is cited in the introduction, discussed in Sec. III B and
   named in the limitations; the Tesla-valve conclusion is stated for
   graphene, steady laminar flow, below the drift limit.
2. Stage B7 (`results/BALLISTIC_SHORT.md`) simulated the short funnels. The
   ballistic conclusion is now a length criterion. Stearns and Moddel
   (2020), the 28 THz measurement of Zhu et al., Shi et al., Hemmetter et
   al., Custer et al., Do et al., Bai et al. and Ermann and Shepelyansky
   are cited.
3. The efficiency law is presented as that of Kachorovskii et al. with
   Gutin, Rudin and But for the saturation; "no harvesting regime" is
   restricted to the mechanisms studied; Khan et al. is cited as the
   counterexample with a threshold.
4. Lucas and Fong, Mueller et al., Hui et al., Monari et al., Gungor et
   al. and the nozzle preprint of Geurs et al. are cited; Stage H5
   (`results/HYDRO_GATE.md`) answers the gated-flow question.
5. Title changed to "Shape versus circuit: graphene geometric diodes and
   Tesla valves as terahertz rectifiers"; abstract rewritten.
6. Full-text reads: done for Shi, Hemmetter, Nguyen and Zhu; abstract plus
   poster for Stearns and Moddel; abstract only for Qin and for Dragoman
   and Aldrigo (details from the authors' later review). Record in
   `refs_verification.md`, addendum of 2026-11-11.

Corrections to the tables above, from the re-reads: Shi et al. measured at
3.0 THz and its 20 to 30 pW/Hz^1/2 is a 4 K figure (6 V/W at room
temperature); Hemmetter et al. is a four-terminal device and its 11.8 V/W
and 1.9 nW/Hz^1/2 use an isotropic-radiator normalisation (0.77 V/W and
29 nW/Hz^1/2 against beam power); the Nanomaterials paper of Stearns and
Moddel has no frequency dependence; Dragoman and Aldrigo simulate a
graphene/GaAs Schottky rectenna, not a geometric diode.
