# Reference verification record

Checked 2026-11-10. Companion to `refs.bib` (46 entries).

## How to read this

- **Bibliographic sources.** "Crossref" means the record returned by
  `api.crossref.org` for the DOI or for a title search; "arXiv" means the
  arXiv API record (title, authors, journal reference, DOI). Other sources are
  named in the row. No field in `refs.bib` was filled from memory.
- **Status.** *verified*: the list's details matched the record. *corrected*:
  an identifier, title or publication status in the list was wrong and has
  been fixed. *not found*: no record located (there are none).
- **Claim check.** "full text" means the article body was read (Europe PMC
  XML, the publisher's open-access HTML, the arXiv/ar5iv text, or an
  author-archive PDF); "abstract only" and "could not access" mean what they
  say. Where no claim was attached to a reference, the column says what was
  read, and nothing more should be inferred.
- **Caveats on the reading method.** Three publisher pages (the JETP archive
  pages for Sharvin and Gurzhi, and the APS accepted-paper page for Sarypov)
  were read through a fetch tool that returns a summary of the page, not the
  raw page. The NASA note is a scan; its table was read from OCR text.
  The two Truccolo papers were read from the authors' institutional-archive
  PDFs, which carry the IEEE running heads and page numbers.

Totals over the 43 reference rows: **41 verified, 2 corrected, 0 not found.**
Separately, 8 rows carry a correction or caveat to a *claim* (marked **CLAIM**).

## Table

### Reciprocity and nonlinear mesoscopic transport

| Key | Status | Corrected relative to the list | Claim check |
|---|---|---|---|
| `christen1996gauge` | verified (Crossref, arXiv) | Journal is Europhysics Letters 35, 523-528 (1996); the list gave only the arXiv number. | **Full text** (arXiv). **CLAIM, partly supported.** Eq. (3) and Eq. (6): the second-order conductance is an integral of the functional derivative of the transmission with respect to the local potential, weighted by the characteristic potentials, so the leading nonlinearity is set by how the internal potential responds to the applied voltages; the introduction says the potential must be treated self-consistently. After Eq. (8): the rectification coefficient vanishes for a symmetric barrier and comes from asymmetries of density of states, transmission and screening. The paper does not use the word "only", and it does not state "equal transmission in both directions": what it states (after Eq. (4)) is that unitarity gives the sum rules on the transmission functions, from which the two-terminal equality follows. Word the manuscript sentence as a consequence of unitarity, not as a quotation of this paper. |
| `sanchez2004magnetic` | verified (Crossref, arXiv) | Phys. Rev. Lett. 93, 106802 (2004). | Abstract only (no claim given). Abstract: the nonlinear I-V is not even in magnetic field, solely because the screening potential inside the conductor depends on the field. |
| `song1999formalism` | verified (Crossref) | Phys. Rev. B 59, 9806-9809 (1999). Single author A. M. Song. No arXiv eprint found. | Not read (no claim given). |
| `song1998nonlinear` | verified (Crossref) | Phys. Rev. Lett. 80, 3831-3834 (1998); six authors. No arXiv eprint found. | Not read (no claim given). |
| `lofgren2004symmetry` | verified (Crossref, arXiv) | Phys. Rev. Lett. 92, 046803 (2004); seven authors. | Abstract only (no claim given). Abstract: proposes symmetry relations in bias and magnetic field for two-terminal nonlinear conductance and confirms them on quantum dots. |

### Geometric diodes

| Key | Status | Corrected relative to the list | Claim check |
|---|---|---|---|
| `zhu2013graphene` | verified (Crossref) | None. J. Phys. D 46, 185101 (2013). No arXiv eprint found. | Not read (no claim given; abstract not in Crossref). |
| `dragoman2013geometrically` | verified (Crossref, arXiv) | J. Phys. D 46, 055306 (2013). The arXiv version is titled "Geometrical-induced ..."; the journal title is as in the list. | Abstract only (no claim given). Abstract: a ballistic 2D device with non-uniform cross-section (a taper) rectifies for both Schrodinger and Dirac carriers. |
| `auton2016graphene` | verified (Crossref) | Nat. Commun. 7, 11670 (2016). No arXiv eprint found. | Abstract only (no claim given). Note for the text: this is a four-terminal asymmetric cross-junction, not a two-terminal geometric diode, and the abstract reports a low-frequency input signal, not THz. |
| `auton2017terahertz` | verified (Crossref) | Nano Lett. 17, 7015-7020 (2017); twelve authors. | Abstract only (no claim given). Abstract: no cutoff seen up to 0.45 THz; imaging at 0.685 THz; 764 V/W and 34 pW/Hz^1/2 at room temperature. The manuscript's "0.7 THz" rests on this paper alone. |
| `nguyen2021geometric` | verified (Crossref) | Title is "Optimum design for the ballistic diode based on graphene field-effect transistors" (the words "geometric diode" are not in the title). npj 2D Mater. Appl. 5, 89 (2021). First author Van Huy Nguyen. Not on arXiv or PMC. | **Full text** (publisher HTML, including Table 1). hBN encapsulation: supported (abstract, Methods). Device D, neck 175 +/- 5 nm, L = 6 um, channel 2 um: supported (Table 1). Asymmetry 1.66 at 80 uA, room temperature: supported (text on Fig. 5(d), Table 1). **CLAIM caveat:** the abstract and the conclusion give the maximum as 1.58 at 60 uA, which is the Device B value; the paper is internally inconsistent, so cite 1.66 to "Table 1 / Fig. 5(d)". Mean free path about 0.7 um: supported (Results, fabrication paragraph), as an estimate from mobility and a threshold density, not a direct measurement. Mobility 130 000 cm^2/Vs: supported for electrons, with 47 000 for holes, from a transfer curve (Supplementary Fig. 4); not specific to Device D. Polarity reverses for holes: supported (Device D text: the forward direction is reversed by the gate; Device A: ratio above 1 for electrons, below 1 for holes). The paper also says the 30 degree taper is "around 30" and that the curves are linear within +/- 20 uA. A carrier density at the measurement point is not stated; only the gate lever arm (7.2e10 cm^-2/V) and V_g = +18 V are. |
| `nguyen2022rectifying` | verified (Crossref) | First author is Dinh Cong Nguyen (not the first author of the 2021 paper). Title "Rectifying effect in a high-performance ballistic diode bridge based on encapsulated graphene with a unique design". ACS Appl. Electron. Mater. 4, 1518-1524 (2022). | Could not access (no claim given). |
| `stearns2021simulation` | verified (Crossref, Europe PMC) | Full title is "Simulation of Z-shaped graphene geometric diodes using particle-in-cell Monte Carlo method in the quasi-ballistic regime". Nanomaterials 11, 2361 (2021). PMC8470659 confirmed. | Abstract only (no claim given). Abstract: particle-in-cell Monte Carlo with momentum relaxation; rectification is optimal at a specific mean free path, so an arbitrarily long one is not better. |
| `wang2021cvd` | verified (Crossref, Europe PMC) | Full title "CVD-grown monolayer graphene-based geometric diode for THz rectennas"; seven authors. Nanomaterials 11, 1986 (2021). PMC8399968 confirmed. | Abstract only (no claim given). Abstract: neck widths 250 to 50 nm; asymmetry 1.40 over +/- 2 V and 0.0628 A/W zero-bias responsivity at 50 nm. |
| `wang2024graphene` | verified (Crossref) | Adv. Electron. Mater. 10(4), 2300695; issue date April 2024, online 28 December 2023. Ten authors. | **Abstract only.** Asymmetry ratio 1.97: supported (abstract), for the better of two devices with 23 and 26 nm necks, with 0.6 A/W and 12 000 V/W. **CLAIM caveat:** the abstract does not give the temperature or bias at which 1.97 was measured, so "at room temperature" in the introduction is unconfirmed for this reference. |
| `truccolo2024comprehensive` | verified (Crossref; author-archive PDF) | Title "Comprehensive analysis of graphene geometric diodes: role of geometrical asymmetry and electrostatic effects". IEEE TED 71(2), 1294-1301 (Feb. 2024). Authors Truccolo, Palestri, Esseni, Boscolo, Midrio. **The manuscript cites the key `truccolo2024`, which does not exist in `refs.bib`.** | **Full text.** Claim supported, and stronger than stated. Abstract: asymmetry is mainly geometric but electrostatics tends to counterbalance it substantially. Sec. III-B (Figs. 4-5): with the full 3-D Poisson solution the asymmetry coefficient is roughly halved relative to the uniform-field case, because the field concentrates at the neck tips and draws carriers into the neck; at a shallow funnel angle (7.6 degrees) the asymmetry falls slightly below one, i.e. reverses. Conclusion: electrostatics can cancel or reverse the geometric rectification, and uniform-field analysis overestimates it. The simulations are purely ballistic (no scattering). |
| `truccolo2026impact` | verified (Crossref; author-archive PDF) | Title "Impact of scattering on graphene geometric diodes: a comparative analysis between simulations and experiments". IEEE TED 73(5), 2696-2702 (May 2026). Eight authors incl. Hemmetter, Lemme. | **Full text. CLAIM does not belong here.** This paper is about scattering, not about self-consistent versus uniform field. Abstract and Sec. III-A (Fig. 3): asymmetry survives while the mean free path is at least the device length, declines below that, and is gone when the ratio is below about 0.01. Sec. III-B: the CVD devices on SiO2 are nearly ohmic. Cite the electrostatics statement to the 2024 paper only. |

### Tesla valves

| Key | Status | Corrected relative to the list | Claim check |
|---|---|---|---|
| `tesla1920valvular` | verified (Google Patents record) | US 1,329,559, "Valvular conduit", inventor Nikola Tesla, filed 21 Feb. 1916, issued 3 Feb. 1920. | Not read (no claim given). |
| `nguyen2021early` | verified (Crossref, arXiv, Europe PMC) | Nat. Commun. 12, 2884 (2021); arXiv:2103.17222. PMC8128925 confirmed. | **Full text.** Diodicity near 1 at low Re: supported (text on Fig. 2(e): close to unity up to Re of about 100). **CLAIM, adjust the range:** the turn-on is over Re = 100 to 300 (text on Fig. 2(e)), with the abstract quoting Re of about 200, and diodicity of about 2 for Re = 300 to 1500. "100 to 200" understates the upper end. This is a laboratory experiment on a macroscopic conduit; how its Reynolds number maps onto the manuscript's current-based one was not examined. |
| `geurs2020rectification` | verified (arXiv) | None bibliographically. **Preprint only:** arXiv v1 of 11 Aug. 2020, comment "Submitted", no journal reference or DOI on the arXiv record; Crossref author and title searches found no journal version. | **Full text** (arXiv, incl. Methods). Diodicity about 1.05 for 20 K < T < 100 K: supported (text on Fig. 2(a)). **CLAIM caveats:** (i) the region is at large *hole* density (n below -5e11 cm^-2); the authors say the effect is barely visible, if at all, on the electron side. (ii) Diodicity is defined as a ratio of four-terminal dc resistances, with a single-loop valve. (iii) Currents (Methods): 6.3 uA for device 1, 8.3 uA and 690 nA for the two configurations of device 2. (iv) **No Reynolds number for the electron device is given anywhere**; the only flow parameters are those of the illustrative fluid simulation. Any Reynolds number attached to this experiment is ours, not theirs. |
| `sarypov2026electron` | **corrected** (arXiv; Crossref; APS accepted-paper page) | Exists: arXiv:2603.16443, "Electron Tesla valve", v1 17 Mar. 2026, v2 27 Apr. 2026, six authors. No longer preprint-only: accepted by Phys. Rev. Lett. on 26 Oct. 2026, DOI 10.1103/bwxl-wwt6, no volume or article number yet. The title differs between the Crossref deposit ("Current rectification at high currents in asymmetric two-dimensional electron structures") and the APS page (the same without "Current", with the subtitle "Electron Tesla valve"). Same authors and matching abstract; the arXiv record itself does not yet link to the DOI. | **Full text** (arXiv v1 via ar5iv). GaAs 2DEG, channel widths 2.5 and 5 um, lattice temperature 4 K. Forward and reverse I-V coincide below about 300 uA and diverge above (text on Fig. 2(b)); diodicity up to about 40 in the wider valve (Fig. 2(d)); rectification weakens above about 20 K and nearly vanishes at 70 K (Fig. 3). The authors estimate Re of at least 7 at 350 uA (Methods) and interpret the threshold as an onset of turbulence; that interpretation is theirs. |
| `huang2024graphite` | verified (Crossref, Europe PMC) | Title "A graphite thermal Tesla valve driven by hydrodynamic phonon transport"; Nature 634, 1086-1090 (2024); nine authors. PMC11525177 confirmed. | Abstract only (no claim given). Abstract: 15.2 % difference in thermal conductivity between directions at 45 K in 90-nm-thick isotopically enriched graphite. |
| `liosis2025diodicity` | verified (Crossref, Europe PMC) | The PMC12735025 article is Liosis, Papadatos, Pagonis, Peppa, Sarris, "Diodicity of MicroTesla valves under various Re numbers", Micromachines 16, 1329 (2025). | **Full text.** Claim supported (Appendix Table A2): three-stage 1.023 and five-stage 1.033 at Re 50; 1.103 and 1.150 at Re 100. Full range over two to ten stages: 1.012 to 1.046 at Re 50 and 1.053 to 1.215 at Re 100. These are 3-D laminar simulations (OpenFOAM) of water in a 100 um square channel, not measurements, with Re on the hydraulic diameter. The manuscript's "1.23 at Re 100" sits just above the largest tabulated value (ten-stage, 1.215). |

### Electron hydrodynamics in graphene

| Key | Status | Corrected relative to the list | Claim check |
|---|---|---|---|
| `bandurin2016negative` | verified (Crossref, arXiv) | Science 351, 1055-1058 (2016); arXiv:1509.04165; thirteen authors. | **Full text** (arXiv PDF). See the viscosity row. |
| `krishnakumar2017superballistic` | verified (Crossref, arXiv) | Nature Physics 13, 1182-1185 (2017); arXiv:1703.06672; sixteen authors. Family name is "Krishna Kumar". | **Full text** (arXiv PDF). See the viscosity row. Also uses the Sharvin formula for graphene point contacts (Eq. (1)). |
| `kiselev2019boundary` | verified (Crossref, arXiv) | Phys. Rev. B 99, 035430 (2019); arXiv:1806.03933. The arXiv title has a leading "The". | **Full text** (arXiv). Claim supported. Abstract: for fully diffuse edges the slip length is of the order of the electron-electron mean free path, and parametrically larger for nearly specular edges. Introduction: for realistic graphene parameters the slip length exceeds 1 um below 100 K, so no-stress is the better default. Sec. II.4 and Fig. 5: for doped graphene (0.25 eV) with diffuse edges, from 0.8 um at 100 K to 3 mm at 1 K. The manuscript's 0.4 um slip length is shorter than the paper's diffuse-edge value at 100 K (0.8 um at 0.25 eV); say which temperature and density it is meant to represent. |
| (viscosity of order 0.1 m^2/s) | verified | Both papers state it; cite `bandurin2016negative` for the round number. | **Full text.** Bandurin 2016: about 0.1 m^2/s in the abstract and in the main text (for carrier densities near 1e12 cm^-2, single- and bilayer). Krishna Kumar 2017: 0.16 m^2/s from a fit at 100 K and 1e12 cm^-2 (caption of Fig. 3(d)). Neither paper gives a 300 K value: the manuscript's 0.03 m^2/s at "300 K" is not from these sources. |
| `gurzhi1963minimum` | verified (JETP archive page) | Sov. Phys. JETP 17(2), 521 (Aug. 1963); Russian original Zh. Eksp. Teor. Fiz. 44, 771. No DOI. The archive gives the first page only and labels the journal "JETP"; Krishna Kumar 2017 cites it as Sov. Phys. JETP 17, 521-522. | Not read (no claim given). |
| `gurzhi1968hydrodynamic` | verified (Crossref) | Sov. Phys. Usp. 11(2), 255-270 (1968), DOI 10.1070/PU1968v011n02ABEH003815; Russian original Usp. Fiz. Nauk 94, 689 (1968). | Not read (no claim given). |

### Plasmonic detection

| Key | Status | Corrected relative to the list | Claim check |
|---|---|---|---|
| `dyakonov1993shallow` | verified (Crossref) | Phys. Rev. Lett. 71, 2465-2468 (1993), as listed. | Not read (title only matches the description in the list). |
| `dyakonov1996detection` | verified (Crossref) | IEEE TED 43(3), 380-387 (1996), as listed. | Not read (title only). |
| `vicarelli2012graphene` | verified (Crossref, arXiv) | Nature Materials 11, 865-871 (2012); arXiv:1203.3232; nine authors. | Abstract only (no claim given). Abstract (arXiv): antenna-coupled graphene FETs at 0.3 THz and room temperature, noise-equivalent power below 30 nW/Hz^1/2, with thermoelectric and photoconductive contributions alongside the FET nonlinearity. |
| `bandurin2018resonant` | **corrected** (Crossref, arXiv, Europe PMC) | **Wrong PMC number in the list.** PMC6208423 is a different paper (Bandurin et al., "Fluidity onset in graphene", Nat. Commun. 9, 4533 (2018)). "Resonant terahertz detection using graphene plasmons" is Nat. Commun. 9, 5392 (2018), PMC6300605, arXiv:1807.04703; sixteen authors. | Abstract only (no claim given). Abstract: antenna-coupled graphene transistors act as plasmonic Fabry-Perot cavities and rectifiers, gate-tuned through several resonances; plasmon wavelength and lifetime are measured in bilayer graphene. |
| `tomadin2013theory` | verified (Crossref, arXiv) | Phys. Rev. B 88, 205426 (2013); arXiv:1307.0734. | **Full text** (arXiv). Density-dependent cyclotron mass in the Euler equation, m_c = hbar k_F / v_F: supported (Sec. II.3, Eq. (13) and the sentence after it; Eq. (17) for the plasma-wave velocity). **CLAIM not supported for quantum capacitance:** the model uses the geometric gate capacitance in the gradual-channel approximation (Sec. II.2); "quantum capacitance" appears once, about an unrelated measurement. This paper cannot be the citation for the v_F^2/2 term in the manuscript's Eq. for s^2(n). |
| `svintsov2013hydrodynamic` (and `svintsov2012hydrodynamic`) | verified (Crossref, arXiv) | Chosen Svintsov paper: "Hydrodynamic electron transport and nonlinear waves in graphene", Phys. Rev. B 88, 245444 (2013), arXiv:1310.3963. Also verified: "Hydrodynamic model for electron-hole plasma in graphene", J. Appl. Phys. 111, 083715 (2012), arXiv:1201.0592, which Tomadin and Polini cite for the plasma-wave velocity. | **Full text** (arXiv) for both. 2013: hydrodynamic equations for massless carriers in gated graphene with a density- and velocity-dependent "fictitious" mass; no Galilean invariance. 2012: plasma waves in gated graphene with geometric capacitance. Neither includes quantum capacitance. |

### Methods

| Key | Status | Corrected relative to the list | Claim check |
|---|---|---|---|
| `jacoboni1983monte` | verified (Crossref) | Full title "The Monte Carlo method for the solution of charge transport in semiconductors with applications to covalent materials"; Rev. Mod. Phys. 55, 645-705. | Not read (no claim given). |
| `taylor1973numerical` | verified (Crossref) | Computers & Fluids 1(1), 73-100 (1973). | Not read. |
| `engelman1982implementation` | verified (Crossref) | Int. J. Numer. Methods Fluids 2(3), 225-238 (1982). | Not read. |
| `kovasznay1948laminar` | verified (Crossref) | Crossref lists the journal under its present name, Mathematical Proceedings of the Cambridge Philosophical Society, 44(1), 58-62 (1948). | Not read. |
| `erturk2008numerical` (and `armaly1983experimental`) | verified (Crossref) | Benchmark chosen to match `apps/hydro_tests.cpp`: E. Erturk, Computers & Fluids 37(6), 633-655 (2008). Armaly, Durst, Pereira, Schonung, J. Fluid Mech. 127, 473-496 (1983) is also verified. | **Could not access** (closed access; no arXiv version found). **CLAIM unverified:** the reattachment lengths and "literature bands" in the supplement and in `results/HYDRO.md` (2.85-3.05 at Re 100, 4.75-5.10 at Re 200) were not checked against either paper, and the expansion ratio of the Armaly experiment was not confirmed to be 2. |
| `geuzaine2009gmsh` | verified (Crossref) | Int. J. Numer. Methods Eng. 79(11), 1309-1331 (2009). | Not read. |
| `guennebaud2010eigen` | verified (Eigen project BibTeX page) | Entry copied from https://libeigen.gitlab.io/pages/bibtex/: authors "Gael Guennebaud and Benoit Jacob and others", title "Eigen", year 2010, howpublished https://libeigen.gitlab.io. The project's own key is `eigenweb`. | Not applicable. |
| `richley1964numerical` (with `clausing1932stromung`, `clausing1971flow`) | verified (NASA NTRS record and report cover; Crossref for Clausing) | The two-dimensional values are not in Clausing, whose papers concern tubes. Citable 2D source: E. A. Richley and T. W. Reynolds, NASA TN D-2330 (June 1964). Clausing: Ann. Phys. 404, 961-989 (1932) and the English-titled J. Vac. Sci. Technol. 8, 636-646 (1971). | **Full text** for the NASA note (scan, OCR). Summary: diffuse-wall transmission probabilities for slots of length-to-width 1/4 to 8. Table I(b), parallel-wall column, as read from OCR: 0.889, 0.804, 0.684, 0.541, 0.398, 0.271 for L/W = 1/4, 1/2, 1, 2, 4, 8, against the repository's own Monte Carlo 0.890, 0.804, 0.683, 0.541, 0.400, 0.274. The column assignment was inferred from the OCR layout and should be confirmed by eye on the PDF. L/W = 16 (0.179 in the README) is outside the NASA table. Clausing's papers were not read. |
| `sharvin1965possible` | verified (JETP archive page) | Yu. V. Sharvin, "A possible method for studying Fermi surfaces", Sov. Phys. JETP 21(3), 655 (Sept. 1965); Russian original Zh. Eksp. Teor. Fiz. 48, 984. No DOI. Journal name as cited in Krishna Kumar 2017. | Not read. For the graphene form of the Sharvin conductance, `krishnakumar2017superballistic` Eq. (1) is a source that was read (the equation itself is an image in the PDF, so its exact form was not transcribed). |

### arXiv identifiers dated 2026

| Identifier | Result |
|---|---|
| arXiv:2603.16443 | Exists; it is the Sarypov paper above. |
| arXiv:2608.21396 | Exists, but it is not a transport-physics reference: G. Galgoczi, "Mitigating warp divergence in GPU optical photon Monte Carlo: an order of magnitude speedup with Shader Execution Reordering" (3 Aug. 2026, preprint). That matches how the literature report uses it (a GPU implementation note). It is not in `refs.bib`; nothing in the manuscript needs it. |

## Could not be verified

1. **Backward-facing-step numbers.** Erturk 2008 and Armaly 1983 are paywalled;
   the reattachment values and bands quoted in the supplement are unchecked.
2. **Geurs et al. journal status.** No journal version was found, but a
   version under a different title could have been missed.
3. **Sarypov et al. final citation.** Volume, article number and final title
   are not assigned; two different titles are on record.
4. **Wang 2024 measurement conditions.** Only the abstract was read; the
   temperature and bias for the 1.97 ratio are unconfirmed.
5. **Content of entries with no claim attached.** Zhu 2013, Song 1998, Song
   1999, Nguyen 2022, Dyakonov-Shur 1993 and 1996, Gurzhi 1963 and 1968,
   Sharvin 1965, Clausing 1932/1971, Jacoboni-Reggiani, Taylor-Hood, Engelman
   et al., Kovasznay and Geuzaine-Remacle were verified as records only.
6. **Berman 1965** ("Free molecule transmission probabilities", J. Appl. Phys.
   36, 3356, DOI 10.1063/1.1702984, record verified) is named in the research
   notes as a possible source for 2D Clausing factors. Its content could not
   be accessed, so it is not in `refs.bib`.
7. **End pages** of Gurzhi 1963 and Sharvin 1965 are not on the archive page.
8. **Eigen's older citation page** (eigen.tuxfamily.org) timed out; the entry
   comes from the current project site.

## Missing references worth adding (records verified through Crossref; content not read unless stated)

1. **A source for the quantum capacitance of graphene**, since none of the
   hydrodynamic references includes it and the manuscript's plasma-speed
   equation has that term uncited. Candidate: T. Fang, A. Konar, H. Xing,
   D. Jena, "Carrier statistics and quantum capacitance of graphene sheets and
   ribbons", Appl. Phys. Lett. 91, 092109 (2007), DOI 10.1063/1.2776887.
2. **The earlier graphene geometric diode paper from the same group:**
   G. Moddel, Z. Zhu, S. Grover, S. Joshi, "Ultrahigh speed graphene diode
   with reversible polarity", Solid State Commun. 152, 1842-1845 (2012), DOI
   10.1016/j.ssc.2012.06.013. It predates the 2013 paper that the introduction
   cites for the concept and is in the reference list of Nguyen et al. 2021.
3. **Bandurin et al., "Fluidity onset in graphene"**, Nat. Commun. 9, 4533
   (2018), DOI 10.1038/s41467-018-07004-4, arXiv:1806.03231. This is the paper
   behind PMC6208423; the literature report uses it for the temperature window
   of viscous flow, which bears on the manuscript's "100 K" operating point.
4. **Guo, Ilseven, Falkovich, Levitov, "Higher-than-ballistic conduction of
   viscous electron flows"**, PNAS 114, 3068-3073 (2017), DOI
   10.1073/pnas.1612181114, arXiv:1607.07269 (abstract read). It gives the
   additive ballistic-plus-viscous conductance that the repository's
   acceptance tests use as the bound a non-interacting code must respect.
5. **Custer et al., "Ratcheting quasi-ballistic electrons in silicon geometric
   diodes at room temperature"**, Science 368, 177-180 (2020), DOI
   10.1126/science.aay8663. A room-temperature geometric diode in a material
   other than graphene, in the reference list of Nguyen et al. 2021; relevant
   to the claim that shape alone does not rectify usefully.


## Addendum of 2026-11-11: `refs_prior.bib` (24 entries)

Added after the prior-work search (`prior_work.md`). Bibliographic records
are from doi.org content negotiation (Crossref BibTeX) and the arXiv API,
fetched on 2026-11-11; nothing was filled from memory. Article numbers that
the record carries outside the `pages` field were added by hand from the
Crossref JSON. "Search" in the last column means the reading was done by the
prior-work search of 2026-11-10 and not repeated; "re-read" means read again
at the source on 2026-11-11.

| Key | Record | What was read, and the statement the manuscript rests on it |
|---|---|---|
| `stearns2020high` | Crossref: 2020 Device Research Conference, pp. 1-2 | **Abstract and the authors' poster of the same title (author archive), re-read; the two-page paper itself is paywalled and was not read.** Time-dependent particle-in-cell Monte Carlo with a Poisson solve; Z-shaped diode of about 100 nm; cut-off "near graphene's damping parameter" (abstract); no rectification above 5 THz and weakening above 1 THz (poster). Exact dimensions and mean free path not verified. The companion paper `stearns2021simulation` (Nanomaterials 11, 2361) is dc only and must not be cited for the cut-off. |
| `shi2026ballistic` | Crossref: Small Science 6 (4), 2026; article number e202500654 from the DOI, not in the Crossref record | **Full text (Europe PMC), re-read.** Four-terminal cross-junction rectifier, not a two-terminal funnel; channel widths 100 and 40 nm; 3.0 THz; 77 V/W at 4 K and 6 V/W at room temperature (sample B); NEP 20 to 30 pW/Hz^1/2 computed from Johnson noise from the 4 K data, referred to a diffraction-limited area. **CLAIM caveat:** the abstract attributes 20 pW/Hz^1/2 to room temperature; the body's NEP data are at 4 K. The manuscript and Table III use the 4 K attribution. |
| `hemmetter2026enhanced` | Crossref: Discover Electronics 3 (1), 2026; article number 128 from the publisher page | **Full text, re-read.** Four-terminal "single funnel" and "triple funnel" rectifiers of CVD graphene on a bow-tie; 0.49 to 0.68 THz, zero bias, room temperature; 0.77 V/W against beam power (11.8 V/W with the isotropic-radiator normalisation that the abstract quotes); NEP 29.2 nW/Hz^1/2 un-normalised (1.9 normalised), Johnson estimate; mean free path at most 65 nm; input arm below 100 nm, no neck width given. The manuscript uses the un-normalised values. |
| `qin2017room` | Crossref: Carbon 116, 760-765 (2017) | **Abstract only.** 30 V/W and 51 pW/Hz^1/2, "estimated", 0.33 THz, room temperature, bilayer graphene on SiC. Power reference not verified; marked with a dagger in Table III. |
| `dragoman2016graphene` | Crossref: Appl. Phys. Lett. 109, 113105 (2016) | **Abstract only; device details from the same authors' open-access review (Nanomaterials 13, 595 (2023)), read in full.** Simulation, no fabricated device; the rectifier is a graphene/n-GaAs Schottky junction under a graphene bow-tie; 58.43 % at 897 GHz. The manuscript says "predicted by simulation" and "Schottky junction", nothing more. |
| `kachorovskii2013performance` | Crossref: Appl. Phys. Lett. 102, 223505 (2013); arXiv:1302.7089 | **Abstract re-read (arXiv); full text by the search.** Conversion efficiency Q defined against the dissipated THz power; absolute maximum "on the order of 10%"; maximal responsivity in the subthreshold regime. The manuscript does not quote the small-signal prefactor. |
| `gutin2012plasmonic`, `rudin2014theory`, `but2014nonlinear` | Crossref | Search (abstracts). Cited together for the change from quadratic to linear response at high intensity. |
| `khan2022cmos` | Crossref: IEEE RFIC 2022, pp. 291-294 | Search (full text). 13.6 % at 263 GHz is in the title. |
| `sarypov2026electron` (in `refs.bib`) | arXiv:2603.16443, accepted in Phys. Rev. Lett. | Search and second check (abstract page and full text). Diodicity about 40 above about 300 uA; Re at least 7; turbulence; GaAs, 2.5 and 5 um, 4 K; heating to about 100 K. |
| `lucas2018hydrodynamics`, `muller2009graphene` | Crossref | Search (review Sec. 4.3.1 via ar5iv; PRL full text). Reynolds number scales as drift speed over Fermi velocity times flow size over the electron-electron length; order ten at most. |
| `hui2021beyond`, `monari2026electron` | Crossref; arXiv:2607.24386 with journal reference | Search (abstracts, Hui full text). Bernoulli nonlinearity in wedge and Venturi shapes; Monari is an experiment in GaAs. |
| `gungor2022modeling` | Crossref: Materials 15, 4141 (2022) | **Full text read on 2026-11-10/11** (recorded in `plans/hydro_dcgate.md`): 10 nm channel, 50 mV, 1 ps pulses, diodicity 1.0655, qualitative comparison with Geurs et al. |
| `geurs2025supersonic` | arXiv:2509.16321, no journal reference | Search (full text). Bilayer-graphene nozzle driven to the wave speed. |
| `do2025nonlinear` | Crossref: ACS Nano 19, 14150-14160 (2025). **Title corrected:** the published title is "Nonlinear Optical Resonances from Ballistic Electron Funnelling"; `prior_work.md` has the preprint title. | Search (abstract). |
| `bai2021asymmetric` | Crossref: Commun. Phys. 4, 236 (2021). **Title corrected** relative to `prior_work.md`, which has the preprint title. | Search (abstract). |
| `ermann2011relativistic` | Crossref: Eur. Phys. J. B 79, 357-362 (2011) | Search (full text). Ratchet current flat below omega r / v_F = 1 and falling above. |
| `custer2020ratcheting` | Crossref: Science 368, 177-180 (2020) | Search (full text). Flat response from 100 MHz to 40 GHz. |
| `zhang2021terafet`, `khavronin2020singularity` | Crossref | Search (abstract; Khavronin partly full text). |
| `castello2024impact`, `lisauskas2009rational` | Crossref; arXiv:2412.07279 | Search (Castello full text; Lisauskas not re-read). |

Two already-cited papers were re-read in full on 2026-11-11 for facts the
revised manuscript uses:

- `nguyen2021geometric`: device D is 6 um end to end (Table 1), with Cr/Au
  edge contacts on the two ends, measured two-terminal; asymmetry 1.66 at
  80 uA. The mean free path of about 0.7 um is derived from the electron
  mobility at a low "threshold" density, and the paper's own model uses
  0.25 um. The abstract quotes 1.58 (device B), the body 1.66 (device D).
- `zhu2013graphene`: neck 75 nm, shoulder 400 nm, rectenna diode 500 nm
  long, no taper angle stated (our G4 assumes 45 degrees); measured mean
  free path about 45 nm; zero-bias dc responsivity 0.12 A/W for the
  stand-alone diode (0.0285 A/W for the rectenna diode); 28 THz, zero bias,
  190 pA measured against 270 pA estimated; the only speed limit discussed
  is RC.
