# Measured graphene THz detectors: literature table for the comparison figure

Compiled 2026-11-10. Every number below was read on that date from the source
named in the last two columns of its row. "arXiv" means the author preprint was
read, not the typeset journal version; bibliographic data (authors, journal,
volume, pages, year, DOI) are from the Crossref API for every entry. Nothing in
the table is filled from memory. Where a field could not be read it says so.

Conventions that matter when comparing with our simulation:

- **Power reference.** "Optical, total beam" = responsivity is photosignal over
  the whole measured beam power, no area correction. "Delivered to antenna" =
  total power corrected only for window and lens loss. "Spot-normalised" = power
  scaled by (diffraction-limited area)/(beam area), i.e. the power a perfectly
  focused beam would deliver; the area used differs between groups (lambda^2/4,
  lambda^2/pi, lambda^2/4pi). "Active-area normalised" = scaled to the
  geometric device area, which can inflate responsivity by 10^2 to 10^4.
  "Absorbed" = power absorbed in the graphene. Our simulated figures assume
  ideal contacts and perfect coupling, so they are closest to "delivered to
  antenna" or "spot-normalised", and are optimistic against "optical".
- **Noise.** "Johnson est." = NEP computed as sqrt(4 k T R)/responsivity from the
  measured resistance; noise was not measured. "Measured" = a noise spectrum was
  measured. Only three rows report measured noise.

## 1. Table

| # | First author, year | Journal reference, DOI | Device and mechanism | Frequency | T | Responsivity (as stated) | NEP (as stated) | Power reference; noise basis | Where in source | Verification |
|---|---|---|---|---|---|---|---|---|---|---|
| **(a)** | **Antenna-coupled FET / plasmonic (Dyakonov-Shur type)** | | | | | | | | | |
| 1 | Vicarelli 2012 | Nat. Mater. 11, 865-871; 10.1038/nmat3417 | Log-periodic-antenna-coupled exfoliated monolayer (SLG) and bilayer (BLG) FETs on SiO2; overdamped plasma-wave rectification with thermoelectric and photoconductive contributions | 0.3 THz | RT | about 100 mV/W (SLG), about 150 mV/W (BLG) | min. about 200 nW/Hz^1/2 (SLG), about 30 nW/Hz^1/2 (BLG); abstract: < 30 nW/Hz^1/2 | Spot-normalised (S_t/S_lambda, S_lambda = lambda^2/4), authors call it optical and a lower limit on responsivity; Johnson est. | Abstract; responsivity paragraphs and Fig. 3; NEP paragraph and Fig. 4 | full text (arXiv:1203.3232v1) |
| 2 | Spirito 2014 | Appl. Phys. Lett. 104, 061111; 10.1063/1.4864082 | Antenna-coupled bilayer-graphene FETs, wide top gate or buried gate; overdamped plasma waves | 0.29-0.38 THz | RT | about 1.2 V/W (device A, 0.292 THz); 1.12 V/W and 1.3 mA/W (device B, 0.358 THz) | about 4e-9 W/Hz^1/2 (photovoltage mode), about 2e-9 W/Hz^1/2 (photocurrent mode) | Spot-normalised (lambda^2/4); Johnson est. | Abstract; text around Figs. 4 and 5 | full text (arXiv:1312.3737v1) |
| 3 | Zak 2014 | Nano Lett. 14, 5834-5838; 10.1021/nl5027309 | Top-gated CVD-graphene FET with split bow-tie antenna | 0.6 THz | RT | better than 14 V/W (maximum optical) | 515 pW/Hz^0.5 (minimum optical) | Stated as optical; Ludwig 2024 Table 1 lists it as Si lens, no normalisation, and Generalov 2017 Table I marks the NEP as calculated (both secondhand); full text not read | Abstract | abstract only (PubMed record via Europe PMC) |
| 4 | Generalov 2017 | IEEE Trans. THz Sci. Technol. 7, 614-616; 10.1109/TTHZ.2017.2722360 | CVD-graphene FET, split bow-tie antenna, Si lens | 400 GHz | not stated explicitly in the text read; listed as room temperature in Ludwig 2024 Table 1 (secondhand) | 74 V/W (maximum measured optical) | 130 pW/Hz^1/2 (minimum) | Optical: power measured at the input of the WR-2.2 horn with a calorimetric meter, no normalisation; Johnson est. (their Eq. 2, table entry marked as calculated) | Abstract; Sec. IV, Fig. 5; Table I | full text (author version, Chalmers Publication Library) |
| 5 | Bandurin 2018a | Appl. Phys. Lett. 112, 141101; 10.1063/1.5018151 | hBN-encapsulated dual-gated graphene FET, broadband antenna, Si lens; plasmonic rectification plus thermoelectric and p-n junction contributions | 0.13 THz (source tunable to 0.45 THz) | RT; also 77 K | 20 V/W max (RT); 30 V/W (77 K, lowest power) | 0.6 nW/Hz^1/2 min (RT) | Delivered to antenna (Golay-measured power corrected for window and lens loss); Johnson est. | Main text near Fig. 2 and Fig. 3b; Supporting Information sec. II, Fig. S2b | full text (arXiv:1712.02144v2) |
| 6 | Bandurin 2018b | Nat. Commun. 9, 5392; 10.1038/s41467-018-07848-w | hBN-encapsulated bilayer-graphene FET, broadband antenna, Si lens; broadband at 0.13 THz, resonant plasmon (Fabry-Perot) detection at 2 THz | 0.13 THz (responsivity values); 2 THz (resonances, relative units) | 10 K for the 240 V/W value; T of the dual-gated result is given only in Supplementary Fig. 2b | about 240 V/W max (single-gated, 10 K); > 3 kV/W (dual-gated, D = 0.1 V/nm) | about 0.2 pW/Hz^1/2 (dual-gated device) | Delivered to antenna (P = P_full/3.5 for about 5.5 dB lens and window loss), called extrinsic, lower bound; Johnson est. | Results text, Fig. 2a; Methods; Supplementary Note 2 | full text (Europe PMC PMC6300605, and arXiv:1807.04703v2 for the supplement) |
| 7 | Gayduchenko 2021 | Nat. Commun. 12, 543; 10.1038/s41467-020-20721-z | hBN-encapsulated dual-gated bilayer-graphene tunnel FET, broadband antenna; interband tunnelling rectification | 0.13 THz (2.026 THz only in relative units) | 10 K (also 77 K) | about 200 V/W at V_bg = 0; > 3 kV/W at V_bg = 1.5 V (abstract: > 4 kV/W) | 0.2 pW/Hz^1/2 at about 10 K | Delivered to antenna (P_in = P_full/3.5), called extrinsic; Johnson est. (sqrt(4 k T r_2pt)) | Abstract; Fig. 2a text; Fig. 3c; Methods | full text (Europe PMC PMC7822863) |
| 8 | Delgado-Notario 2022 | Nanophotonics 11, 519-529; 10.1515/nanoph-2021-0573 | hBN/graphene/hBN FET with asymmetric dual-grating top gate and graphite back gate, no antenna; plasmonic ratchet / differential drag | 0.3 THz | 4.5 K to RT | max 0.216 A/W (4.5 K); 1.9 mA/W (RT) | min 0.81 pW/Hz^0.5 (4.5 K); 0.67 nW/Hz^0.5 (RT) | Active-area normalised: responsivity multiplied by S_T/S_D with beam spot about 7 mm^2 and active area about 31 x 10 um^2, described by the authors as intrinsic (perfect lossless coupling); Johnson est. Not comparable with optical values. (Ludwig 2024 Table 1 labels the same numbers as normalised to the diffraction-limited area.) | Sec. 3 text and Fig. 5; Eq. 4 | full text (Europe PMC PMC11501572) |
| 9 | Caridad 2024 | Nano Lett. 24, 935-942; 10.1021/acs.nanolett.3c04300 | hBN-encapsulated single-layer graphene FET, short top-gated cavity; Dyakonov-Shur rectification, resonant above about 2.5 THz | 0.3 THz (overdamped); 2.5-4.7 THz (resonant) | 10 K for the quoted numbers; resonances shown up to 300 K only as normalised photocurrent | about 0.29 A/W at 0.3 THz and about 1.8 mA/W at 4.7 THz if normalised to active area; the as-measured R_I = I/P (total incoming power) is in Fig. 1c,d only, text implies order 1 uA/W | not reported | Active-area normalised for the quoted values; no noise analysis | Fig. 1 caption; Fig. 3 caption | full text (Europe PMC PMC10811671) |
| 10 | Ludwig 2024 | ACS Appl. Electron. Mater. 6, 2197-2212; 10.1021/acsaelm.3c01511 | Wafer-scale CVD-graphene FETs with asymmetric (AAC) or symmetric (SAC) bow-tie antenna coupling, Si lens; resistive self-mixing plus photothermoelectric (PTE dominant above 100 GHz) | 0.1-1.2 THz, best at 0.4 THz | RT | 59 V/W (AAC) and 63 V/W (SAC) at 400 GHz on high-resistivity Si; 36 and 30 V/W on 10-20 ohm cm Si | 101 pW/Hz^1/2 (AAC) and 114 pW/Hz^1/2 (SAC) at 400 GHz; 232 and 299 pW/Hz^1/2 on doped Si; 1340 pW/Hz^1/2 at 100 GHz and 479 pW/Hz^1/2 at 1 THz (AAC) | Optical, total beam power in front of the Si lens, explicitly without normalisation; Johnson est. | Abstract; Sec. 3.5, Fig. 5; Table 1 | full text (arXiv:2311.12382v2) |
| **(b)** | **Photothermoelectric** | | | | | | | | | |
| 11 | Cai 2014 | Nat. Nanotechnol. 9, 814-819; 10.1038/nnano.2014.182 | Graphene with dissimilar-metal (Cr/Au) contacts, no antenna; hot-electron PTE | 2.5 THz (119 um) | RT | > 10 V/W incident; 700 V/W absorbed (peak 715 V/W, Fig. 3a) | < 1100 pW/Hz^1/2 incident; 20 pW/Hz^1/2 absorbed (minimum 16 pW/Hz^1/2, Fig. 3c) | Both given. Here incident means power on the small device area, not the whole beam: Tong 2015 restates it as 0.48 mV/W against the 17 mW beam and 10 V/W against 0.75 uW on the graphene, and Castilla 2019 Table 1 gives the normalisation area as about lambda^2/3350 (both secondhand). Noise measured, slightly above the Johnson floor | Abstract; Fig. 3; Supplementary | full text (arXiv:1305.3297v4) |
| 12 | Tong 2015 | Nano Lett. 15, 5295-5301; 10.1021/acs.nanolett.5b01635 | Half-edge-contacted graphene FET, double-patch antenna, on-chip Si lens; thermoelectric | 1.9 THz gas laser | RT | 4.9 V/W max (1.1 V/W at zero gate voltage) | 1.7 nW/Hz^1/2 min | Optical, normalised to total incident power; noise measured (close to Johnson) | Text around Fig. 4f and Fig. 5b | full text (arXiv:1504.03058v1) |
| 13 | Castilla 2019 | Nano Lett. 19, 2765-2773; 10.1021/acs.nanolett.8b04171 | hBN-encapsulated graphene p-n junction over a split-gate dipole antenna with 100-200 nm gap; PTE | 2.52 THz (measured 1.8-4.25 THz) | RT | Device A: 14 mA/W (32 V/W); device B: 25 mA/W (105 V/W) | Device A: 200 pW/Hz^1/2; device B: 80 pW/Hz^1/2 | Spot-normalised to a diffraction-limited spot of area lambda^2/pi (A_diff/A_focus about 1/60); Johnson est. from the measured resistance | Abstract; Fig. 2 and text; Methods; Table 1 | full text (arXiv:1905.01881v1) |
| 14 | Viti 2020 | Nano Lett. 20, 3169-3177; 10.1021/acs.nanolett.9b05207 | hBN-encapsulated single-gate graphene FET, asymmetric bow-tie antenna; PTE combined with overdamped plasma-wave | 2.8 THz QCL | RT and 77 K | 49 V/W (RT); about 180 V/W (77 K) | about 160 pW/Hz^1/2 (RT); 18 pW/Hz^1/2 (77 K) | Spot-normalised (A_diff = lambda^2/4); Johnson est. | Abstract; text and Fig. 3a,d | full text (arXiv:2004.10011v1, authors' version) |
| 15 | Asgari 2021 | ACS Nano 15, 17966-17976; 10.1021/acsnano.1c06432 | Scalable CVD (single-crystal and polycrystalline) graphene, single-gate or split-gate p-n, bow-tie or dipole antenna; zero-bias PTE | 2.8 THz | RT | not stated numerically in the text read (Ludwig 2024 Table 1 lists 8 and 4.5 V/W, spot-normalised; secondhand) | min about 600 pW/Hz^1/2 (substrate-side illumination) and about 3 nW/Hz^1/2 (air side) for one device; about 1.3 nW/Hz^1/2 for a p-n device; over 21 devices mean 4.3 and minimum 1.1 nW/Hz^1/2 | Spot-normalised (S_lambda = lambda^2/4); Johnson est. | Abstract (< 1 nW/Hz^1/2); Fig. 2e and text; Fig. 7 and text | full text (Europe PMC PMC8613901) |
| **(c)** | **Ballistic rectifiers and geometric diodes** | | | | | | | | | |
| 16 | Auton 2017 | Nano Lett. 17, 7015-7020; 10.1021/acs.nanolett.7b03625 | hBN-encapsulated four-terminal graphene ballistic rectifier with local gate, antenna-coupled (rectenna) | no cutoff up to 0.45 THz; imaging at 0.685 THz | RT | 764 V/W (peak extrinsic) | 34 pW/Hz^1/2 | Called extrinsic in the abstract; Castilla 2019 Table 1 gives the normalisation area as lambda^2/4pi and the range as 0.07-0.69 THz (secondhand); noise basis not stated in the abstract | Abstract | abstract only (Crossref and Europe PMC records; full text is paywalled) |
| 17 | Zhu 2014 (with Zhu 2013) | IEEE J. Sel. Top. Quantum Electron. 20(6), 70-78; 10.1109/JSTQE.2014.2318276. Earlier: J. Phys. D: Appl. Phys. 46, 185101 (2013); 10.1088/0022-3727/46/18/185101 | Graphene geometric (funnel) diode coupled to metal or graphene bow-tie antenna; geometric rectification, rectenna | 28 THz (10.6 um CO2 laser) | RT | Rectenna responsivity not given in either abstract. Diode zero-bias dc responsivity 0.0285 A/W for the 75 nm neck is secondhand (quoted by Wang 2021, full text read) | of order 10^-9 W/Hz^1/2, stated as calculated; D* of order 10^8 cm Hz^1/2/W; simulated projection 10^-11 W/Hz^1/2 | Not stated in abstract; NEP is calculated, not measured | Abstracts (the 2013 abstract contains no responsivity or NEP number) | abstract only (publisher abstracts as indexed by OpenAlex; the 2013 abstract also seen on the IOP page) |
| 18 | Wang 2024 | Adv. Electron. Mater. 10, 2300695; 10.1002/aelm.202300695 | hBN/graphene/hBN geometric diode, 23 and 26 nm necks | no THz measurement mentioned in the abstract (responsivities are zero-bias diode figures) | not stated in abstract | 0.6 A/W zero-bias current responsivity; 12,000 V/W voltage responsivity; asymmetry ratio 1.97 | not reported in abstract | Presumably from dc I-V curvature (not confirmed; abstract only). For scale, Wang 2021 (Nanomaterials 11, 1986; 10.3390/nano11081986; full text read) gives 0.0628 A/W zero-bias, explicitly from the dc I-V curve of a 50 nm-neck CVD diode | Abstract | abstract only (OpenAlex index and Manchester repository record) |
| **(d)** | **Commercial room-temperature incumbents, for scale** | | | | | | | | | |
| 19 | Virginia Diodes zero-bias Schottky detectors (vendor table; Hesler and Crowe 2007) | VDI product page https://www.vadiodes.com/en/products/detectors (read 2026-11-10); J. L. Hesler and T. W. Crowe, "Responsivity and Noise Measurements of Zero-Bias Schottky Diode Detectors", VDI-hosted ISSTT 2007 paper | Waveguide zero-bias GaAs Schottky diode detectors | 50 GHz to 1.7 THz by waveguide band | RT | Typical: 2200 V/W (WR3.4, 220-330 GHz); 1000 V/W (WR1.5, 500-750 GHz); 750 V/W (WR1.0, 750-1100 GHz); 100 V/W (WR0.8, 0.9-1.4 THz; WR0.65 fast version, 1.1-1.7 THz). Hesler and Crowe: about 4000 V/W at 100 GHz to 400 V/W at 900 GHz | Typical (standard / fast version): 12.0 / 4.8 pW/Hz^1/2 (WR3.4); 20.3 / 13.5 (WR1.5); 27.1 / 27.1 (WR1.0); 203 / 81.2 (WR0.8); 243.6 (WR0.65, fast only). Hesler and Crowe: about 1.5 pW/Hz^1/2 near 150 GHz rising to about 20 pW/Hz^1/2 at 800 GHz | Waveguide input power (no free-space coupling loss); Hesler and Crowe: NEP from measured noise voltage at low input power, where thermal noise dominates, and they warn that flicker noise rises with input power | Vendor table; Hesler and Crowe abstract, Fig. 6 text, conclusion | full text (vendor page and VDI-hosted PDF) |
| 20 | Golay cell and pyroelectric detector (vendor data) | Tydex GC-1P page https://www.tydexoptics.com/products/thz_devices/golay_cell/ ; Gentec-EO THZ5I-BL-BNC page https://www.gentec-eo.com/products/THZ5I-BL-BNC (both read 2026-11-10) | Opto-acoustic Golay cell; pyroelectric detector with integrated amplifier | Golay: 15-8000 um wavelength; pyro: 10-3000 um (projected) | RT | Golay: 1e5 V/W typical at 20 Hz; pyro: 140 kV/W typical | Golay: 1.4e-10 W/Hz^1/2 typical, 0.8e-10 minimum, at 20 Hz; pyro: 1 nW (at 632 nm, 5 Hz chopping; the page gives nW, not per root Hz) | Optical; vendor specification | Specification tables | full text (vendor pages) |

Entries read but kept out of the main table (same rules apply):

- Viti 2021, Nanophotonics 10, 89-98, 10.1515/nanoph-2020-0255 (full text, arXiv:2006.10622v2): hBN-encapsulated graphene FETs on high-bandwidth chips, 3.4 THz, RT. Single-gate sample A: 30 V/W, about 350 pW/Hz^1/2; split-gate p-n sample B: about 50 V/W, 120 pW/Hz^1/2. Spot-normalised (lambda^2/4), Johnson est.
- Liu 2018, NPG Asia Mater. 10, 318-327, 10.1038/s41427-018-0032-7 (full text, publisher page): CVD-graphene FET with bias-field readout, 0.12 THz, RT, up to 280 V/W, stated as relative to the antenna area, NEP 100 pW/Hz^0.5 (photovoltaic mode), about 0.5 nW/Hz^0.5 (photoconductive mode). The device is biased and the NEP is a Johnson estimate that the authors call a lower limit.
- Delgado-Notario 2020, APL Photonics 5, 066102, 10.1063/5.0007249 (full text of the publisher page, read through the WebFetch text extractor, quotes not re-parsed by me): dual-grating-gate hBN/graphene FET without antenna, 0.3 THz. RT: 1.96 uA/W (spot-normalised) or about 1.28 mA/W (active-area "internal"), NEP 0.9 uW/Hz^1/2; 4 K: 85 uA/W or 55.6 mA/W, NEP 1.3 nW/Hz^1/2; thermal noise assumed.
- Auton 2016, Nat. Commun. 7, 11670, 10.1038/ncomms11670 (full text): the same ballistic rectifier at low frequency (190 Hz ac input, not THz), RT: 23,000 mV/mW intrinsic responsivity (output dc voltage over input ac power measured across the device), NEP 0.64 pW/Hz^1/2 from measured noise. This is an electrical, impedance-matched figure and is the natural upper bound for row 16.
- El Fatimy 2016, Nat. Nanotechnol. 11, 335-338, 10.1038/nnano.2015.303 (abstract only): epitaxial-graphene quantum-dot bolometer, 1e10 V/W and electrical NEP about 2e-16 W/Hz^1/2 at 2.5 K. A cryogenic bolometer, outside the classes above.
- Skoblin 2018, Appl. Phys. Lett. 112, 063501, 10.1063/1.5009629 (abstract only): split-gate thermoelectric graphene bolometer, 94 GHz, 4-100 K, optical responsivity about 700 V/W; no NEP in the abstract.
- Yang 2017, Appl. Phys. Lett. 111, 021102, 10.1063/1.4993434 (abstract only): flexible graphene FET, 487 GHz, RT, above 2 V/W, estimated NEP below 3 nW/Hz^1/2 (unit as indexed reads nW/Hz).

### Rows added on 2026-11-11 (after the prior-work search)

| # | Reference | Device | Frequency | T | Responsivity | NEP | Power reference | How read |
|---|---|---|---|---|---|---|---|---|
| 21 | Shi, Viti, Watanabe, Taniguchi, Vitiello, Small Science 6, e202500654 (2026), doi:10.1002/smsc.202500654 | hBN-encapsulated four-terminal cross-junction ballistic rectifier, bow-tie antenna, channel 100 nm (A) and 40 nm (B) | 3.0 THz | 4 K (NEP data); 300 K | sample B: 77 V/W at 4 K, 6 V/W at room temperature | 20 to 30 pW/Hz^1/2, Johnson estimate, from the 4 K data | diffraction-limited area lambda^2/4 (1/20 of the beam power through the cryostat window) | full text (Europe PMC). The abstract attributes 20 pW/Hz^1/2 to room temperature; the body does not support that. |
| 22 | Hemmetter et al., Discover Electronics 3, 128 (2026), doi:10.1007/s44291-026-00280-1 | CVD-graphene four-terminal single- and triple-funnel rectifiers, 150 um bow-tie | 0.49 to 0.68 THz | 300 K | 0.21 (single) and 0.77 V/W (triple) against beam power; 3.3 and 11.8 V/W with isotropic-radiator normalisation | 108 and 29.2 nW/Hz^1/2 un-normalised; 6.8 and 1.9 normalised; Johnson estimate | total available beam power, or isotropic normalisation | full text. Mean free path at most 65 nm. |
| 23 | Qin et al., Carbon 116, 760 (2017), doi:10.1016/j.carbon.2017.02.037 | bilayer graphene (epitaxial on SiC) FET, gate length 145 nm | 0.33 THz | 300 K | 30 V/W | 51 pW/Hz^1/2, "estimated" | not verified | abstract only |

## 2. What the table says

**Room temperature, class (a), FET / plasmonic.** With an antenna and silicon
lens and with the whole beam power in the denominator, measured responsivities
run from 14 V/W (row 3) through 20 V/W (row 5) and 59-63 V/W (row 10) to 74 V/W
(row 4), and the corresponding NEPs from 515 pW/Hz^1/2 down to 101-130
pW/Hz^1/2, all at 0.13-0.6 THz. The first devices (rows 1 and 2) were at
0.1-1.2 V/W and 2-200 nW/Hz^1/2 even after spot normalisation. Away from the
antenna resonance the same technology is at 0.5-1.3 nW/Hz^1/2 (row 10 at 0.1 and
1 THz). Every NEP in this class is a Johnson-noise estimate. No room-temperature
absolute responsivity or NEP is given for the resonant devices (row 9).

**Room temperature, class (b), photothermoelectric.** At 1.9-3.4 THz,
referred to the total beam, 4.9 V/W and 1.7 nW/Hz^1/2 with measured noise
(row 12). Referred to a diffraction-limited spot, 30-105 V/W and 80-350
pW/Hz^1/2 for hBN-encapsulated exfoliated graphene (rows 13, 14 and Viti 2021),
and 0.6-4 nW/Hz^1/2 for scalable CVD graphene (row 15). Referred to absorbed
power, 700 V/W and 16-20 pW/Hz^1/2 (row 11); that is an internal figure.

**Room temperature, class (c), ballistic and geometric.** One antenna-coupled
THz measurement exists with an NEP: 764 V/W and 34 pW/Hz^1/2, extrinsic, up to
0.45 THz (row 16, abstract only). For the geometric (funnel) diode proper, the
only illuminated measurement is at 28 THz with a calculated NEP of order
1 nW/Hz^1/2 (row 17); everything else is a dc I-V figure: 0.0285 A/W
(secondhand), 0.0628 A/W and 0.6 A/W with 12,000 V/W (row 18). No measured THz
responsivity or NEP of a graphene geometric diode below 28 THz was found.

**Cryogenic.** At 10 K and 0.13 THz, bilayer-graphene FETs reach about
200-240 V/W single-gated and more than 3-4 kV/W dual-gated or tunnelling, with a
Johnson-estimated NEP of 0.2 pW/Hz^1/2 referred to power delivered to the
antenna (rows 6 and 7). At 77 K: 30 V/W at 0.13 THz (row 5) and 180 V/W with
18 pW/Hz^1/2, spot-normalised, at 2.8 THz (row 14). The 4.5 K figures of row 8
(0.216 A/W, 0.81 pW/Hz^1/2) and the 10 K figure of row 9 (0.29 A/W) are
active-area normalised and should not be plotted with the others.

**Incumbents.** Zero-bias Schottky detectors: typical 5-27 pW/Hz^1/2 from 0.22
to 1.1 THz, then 81-244 pW/Hz^1/2 from 0.9 to 1.7 THz, referred to waveguide
power (row 19). Golay cell 0.14 nW/Hz^1/2, pyroelectric about 1 nW (row 20).

**Corrected version of the from-memory sentence.** The earlier draft said
room-temperature graphene THz detectors sit at hundreds of pW/Hz^1/2 to
nW/Hz^1/2 with 0.1 to 30 V/W. The NEP range is right for most devices but
misses both ends, and the responsivity ceiling is too low. Replace with:

> Measured room-temperature graphene THz detectors have noise-equivalent powers
> from about 100 pW/Hz^1/2 at best (101-130 pW/Hz^1/2 optical at 0.4 THz
> [Generalov 2017; Ludwig 2024]; 80-160 pW/Hz^1/2 at 2.5-3.4 THz referred to a
> diffraction-limited spot [Castilla 2019; Viti 2020]; a single
> ballistic-rectifier report of 34 pW/Hz^1/2 [Auton 2017]) up to 0.5-2
> nW/Hz^1/2 for most other devices [Zak 2014; Bandurin 2018a; Tong 2015;
> Asgari 2021] and 30-200 nW/Hz^1/2 for the first devices [Vicarelli 2012],
> with voltage responsivities from 0.1 V/W to 74 V/W optical, 105 V/W
> spot-normalised and 764 V/W extrinsic for the ballistic rectifier; nearly all
> of these NEPs are Johnson-noise estimates rather than measured noise. Cooled
> to 10 K, bilayer-graphene transistors reach 3-4 kV/W and 0.2 pW/Hz^1/2 at
> 0.13 THz [Bandurin 2018b; Gayduchenko 2021], and at 77 K 180 V/W and
> 18 pW/Hz^1/2 at 2.8 THz [Viti 2020].

**Against our simulated element** (0.07-0.45 A/W, roughly 50-300 V/W,
Johnson-limited 4-60 pW/Hz^1/2 at 0.3-2 THz and 300 K with ideal contacts and
perfect coupling; about 1 pW/Hz^1/2 at 100 K): the simulated NEP is 2 to 25
times below the best measured optical values at the same frequencies and
overlaps the best spot-normalised and extrinsic ones (34-80 pW/Hz^1/2); the
simulated voltage responsivity brackets the best measured optical (74 V/W) and
spot-normalised (105 V/W) values and is below the one extrinsic
ballistic-rectifier figure (764 V/W); the simulated current responsivity is 3
to 20 times above the best room-temperature THz value stated in A/W (25 mA/W,
spot-normalised, row 13) and in
the range of dc geometric-diode figures (0.03-0.6 A/W). If realised it would be comparable to
catalogue Schottky detectors below 1 THz and better than them above about
1 THz. The 100 K figure sits between the measured 77 K (18 pW/Hz^1/2 at
2.8 THz) and 10 K (0.2 pW/Hz^1/2 at 0.13 THz) points. Since the simulation and
almost every experimental NEP use the same Johnson estimate, the gap is mainly
coupling and contact loss, not noise.

**Suggested figure bands** (NEP against frequency): graphene RT optical,
100 pW to 2 nW/Hz^1/2 over 0.13-2 THz with points at rows 3, 4, 5, 10, 12;
graphene RT spot-normalised, 80-350 pW/Hz^1/2 at 2.5-3.4 THz (rows 13, 14,
Viti 2021) plus 1-200 nW/Hz^1/2 at 0.3 THz for rows 1 and 2; one open marker
for row 16 (34 pW/Hz^1/2, abstract only); cryogenic markers at 0.2 pW/Hz^1/2
(10 K, 0.13 THz) and 18 pW/Hz^1/2 (77 K, 2.8 THz); Schottky line from row 19.
Rows 8, 9, 11 (absorbed) and 18 should be left off or drawn in a visibly
different style.

## 3. Review and comparison articles

1. J. Liu, X. Li, R. Jiang, K. Yang, J. Zhao, S. A. Khan, J. He, P. Liu,
   J. Zhu and B. Zeng, "Recent Progress in the Development of Graphene Detector
   for Terahertz Detection", Sensors 21(15), 4987 (2021),
   doi:10.3390/s21154987. Read in full (PMC8347591). Its Table 2 spans, at room
   temperature, NEP from 34 pW/Hz^1/2 (Auton 2017) to 150 nW/Hz^1/2 and
   responsivity from 0.15 V/W to 764 V/W, and for cooled devices 0.1-0.2
   pW/Hz^1/2 with 3000 V/W at 10 K and 0.2 fW/Hz^1/2 at 2.5 K (quantum-dot
   bolometer). Its Table 1 puts commercial pyroelectric, Golay and Schottky
   detectors all at 10^-10 W/Hz^1/2. Use with care: it lists Bandurin 2018b at
   0.1 pW/Hz^1/2 (the paper says about 0.2), Generalov 2017 at 0.35 THz (the
   paper says 400 GHz), and pairs Viti 2020's room-temperature NEP with its
   77 K responsivity. It is the only source found for Qin 2017 (see section 4).
2. L. Viti and M. S. Vitiello, "Tailored nano-electronics and photonics with
   two-dimensional materials at terahertz frequencies", J. Appl. Phys. 130,
   170903 (2021), doi:10.1063/5.0065595. Read in full (arXiv:2204.02862,
   authors' version). Quotes room-temperature antenna-coupled PTE p-n junctions
   in hBN-encapsulated graphene at NEP about 80 pW/Hz^1/2 with sub-ns response
   at 3 THz, and gives no wider tabulated range (it defers to other reviews).
3. A. Rogalski, M. Kopytko and P. Martyniuk, "Two-dimensional infrared and
   terahertz detectors: Outlook and status", Appl. Phys. Rev. 6, 021316 (2019),
   doi:10.1063/1.5088578. Bibliographic data from Crossref; text not read
   (paywalled; the open 2020 companion in Opto-Electronics Review 28, 107-154,
   doi:10.24425/opelre.2020.134459, was blocked by a bot wall). Range quoted:
   not verified.
4. F. H. L. Koppens, T. Mueller, Ph. Avouris, A. C. Ferrari, M. S. Vitiello and
   M. Polini, "Photodetectors based on graphene, other two-dimensional
   materials and hybrid systems", Nat. Nanotechnol. 9, 780-793 (2014),
   doi:10.1038/nnano.2014.215. Bibliographic data from Crossref; text not read.
   Range quoted: not verified.

Not reviews, but they contain the most useful like-for-like comparison tables
and were read in full: Ludwig 2024 (row 10), Table 1, which separates true
optical values from spot-normalised ones and concludes that broadband
graphene FET detectors have reached the order of 100 pW/Hz^1/2 below 1 THz but
remain behind Si CMOS and AlGaN/GaN FET detectors, which are below
50 pW/Hz^1/2; and Castilla 2019 (row 13), Table 1, which lists the
normalisation area used by each group. Statements about incumbents found in
the papers read disagree with each other by an order of magnitude and should
not be cited as data: pyroelectric and Golay about 1 nW/Hz^1/2 and Schottky
10-100 pW/Hz^1/2 (Castilla 2019 introduction), pyroelectric and Schottky
about 100 pW/Hz^1/2 and microbolometers about 20 pW/Hz^1/2 (Viti 2020
introduction), Schottky 0.3-10 nW/Hz^1/2 at 1 THz (Cai 2014).

## 4. Looked for and not verified

- **Auton 2017, full text.** Paywalled (ACS returned 403, the HAL copy sits
  behind a bot wall, the Manchester record has no file). Not verified: the
  frequency at which 764 V/W was measured, the power normalisation, whether
  the noise was measured. Secondhand only: lambda^2/4pi and 0.07-0.69 THz
  (Castilla 2019 Table 1); 0.45 THz (Liu 2021).
- **Zhu 2013 and Zhu 2014, full text.** Only abstracts read. The 2013 abstract
  has no responsivity or NEP. The rectenna responsivity at 28 THz is not
  verified. A web-search snippet attributed 0.012 A/W and 2.3 nW/Hz^1/2 to a
  Moddel-group thesis; I could not open that source and did not use it. The
  Moddel group pages returned 404. The JSTQE page range 70-78 is as Crossref
  gives it.
- **Wang 2024, full text.** Wiley returned 403. Not verified: that 0.6 A/W and
  12,000 V/W are derived from dc I-V curves, the temperature, the bias, and
  whether any THz or RF illumination was used.
- **Qin et al. 2017**, Carbon 116, 760-765, doi:10.1016/j.carbon.2017.02.037
  (bibliographic data from Crossref). No abstract or full text could be read.
  Secondhand (Liu 2021, text and Table 2): bilayer epitaxial graphene FET,
  30 V/W and 51 pW/Hz^1/2 at 0.33 THz, room temperature. If correct this would
  be the lowest optical NEP in class (a); it is left out of the table and the
  summary for that reason.
- **Zak 2014, full text.** Abstract only; the measurement geometry and the
  noise basis are secondhand.
- **Generalov 2017.** Operating temperature is not stated explicitly in the
  text I read; room temperature is implied and is stated by Ludwig 2024.
- **Bandurin 2018b.** The temperature of the > 3 kV/W, 0.2 pW/Hz^1/2
  dual-gated measurement is given only in Supplementary Fig. 2b (figure
  label); Liu 2021 lists it as 10 K (secondhand).
- **Caridad 2024.** No NEP is reported; the as-measured responsivity against
  total power and any room-temperature absolute responsivity are in figures
  only.
- **Asgari 2021.** Numerical responsivity not found in the text extraction
  (figure only); 8 and 4.5 V/W are secondhand from Ludwig 2024.
- **Vicarelli 2012 responsivity.** I read about 100 and 150 mV/W in arXiv v1.
  Later papers restate this paper differently: 0.015 V/W (Spirito 2014
  introduction), about 2 uA/W or 0.05 V/W (Delgado-Notario 2020). I did not
  check the typeset Nature Materials text.
- **Preprint versus published text.** Rows 1, 2, 5, 10, 11, 12, 13, 14 and
  Viti 2021 were read as arXiv or author versions; abstracts were checked
  against Crossref where Crossref carries one, the body text was not checked
  against the typeset version.
- **Delgado-Notario 2020.** Publisher page read only through the WebFetch
  extractor (model-returned quotes), not parsed directly.
- **Hesler and Crowe 2007.** I read the VDI-hosted PDF (file name
  VDI_Detector_Char_ISSTT2007.pdf). Its proceedings volume and pages are not
  verified. Crossref has a related two-page record that I did not read:
  "NEP and responsivity of THz zero-bias Schottky diode detectors", IRMMW-THz
  2007, pp. 844-845, doi:10.1109/ICIMW.2007.4516758.
- **Reviews not read:** Rogalski 2019 (Appl. Phys. Rev.), Rogalski 2020
  (Opto-Electron. Rev.), Koppens 2014, Mittendorff, Winnerl and Murphy 2021
  (Adv. Opt. Mater. 9, 2001500, doi:10.1002/adom.202001500), Lewis 2019
  (J. Phys. D 52, 433001, doi:10.1088/1361-6463/ab31d5; abstract only, says
  detector parameters are tabulated), Sizov and Rogalski 2010 (Prog. Quantum
  Electron. 34, 278-347, doi:10.1016/j.pquantelec.2010.06.002). No peer-reviewed
  tabulation of Golay, pyroelectric or Schottky NEP was read; row 20 and most of
  row 19 rest on vendor pages.
- **Pyroelectric NEP unit.** The Gentec-EO page states 1 nW without a
  bandwidth unit; a distributor datasheet reportedly gives 1.0e-9 W/Hz^1/2
  (search snippet, not opened).
- **A measured THz NEP for a graphene geometric (funnel) diode at 0.1-10 THz.**
  Searched for; none found.
