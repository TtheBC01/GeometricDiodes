# Stage B7 results: short funnels and the length scaling of the ballistic THz response (2026-11-11)

Plan: `plans/ballistic_short.md` (figure `plans/ballistic_short.png`). Code:
`apps/bthz.cpp` (shape and scale options), ladder T18 to T21 in
`apps/tests.cpp`. Runs: `tools/short_funnel_runs.sh`, results in
`results/bshort/` (`S1_G*.csv` static, `S2_G*.csv` sweeps, `S3.csv`,
`S4.csv`, `S5.csv`, `runs.log`), tables `results/bshort/tables_B7.md`,
figures `B7_transfer.png`, `B7_responsivity.png`, `B7_length.png`.

The question: Stage B2 found the 6 um ballistic funnel dead at THz under a
uniform field. Is that a property of the funnel or of its length, and does
a funnel short enough to compare with the THz devices of the literature
rectify at 1 THz at the room-temperature mean free path?

## Verdict

1. **The roll-off is a property of the length.** The model has no
   intrinsic length (T18, T19), so the response is a function of
   f L / v_F. With a uniform field every shape gives half its
   quasi-static current near f L / v_F = 0.4 and nothing above 1.7
   (`B7_transfer.png`). For 6 um that is 60 GHz and 280 GHz; for 0.55 um it
   is 0.7 THz and 3 THz.
2. **The decision device rectifies at 1 THz at room temperature, and the
   answer no longer depends on where the field drops.** G3 (0.55 um,
   100 nm neck), mean free path 700 nm, 1 THz: 0.70 +- 0.02 A/W per
   absorbed watt with a uniform field and 0.71 +- 0.03 A/W with the drop
   confined to the neck. For the 6 um device the same two field models
   gave 0.00 and 0.75 A/W. One device on a 75 ohm antenna gives 0.17 and
   0.14 A/W.
3. **Against the plan's criteria: alive, in the middle band (0.1 to
   1 A/W), twice the best plasmonic figure of the paper.** The reference
   0.34 A/W is the plasmonic funnel at 100 K; at 300 K that figure is
   0.09 A/W, so at room temperature the short ballistic funnel is ahead
   by a factor of roughly eight in these models. My stated expectation was
   0.1 to 0.5 A/W; the result is above it.
4. **It does not clearly reach the "better than the circuit" band.** If
   the available power is computed from the device's own conduction
   admittance at 1 THz the uniform-field figure is 1.5 to 1.9 A/W, above
   the 1 A/W bar. I do not rely on that: the excess comes from a drop of
   the conduction admittance that an imposed-field model without
   displacement current cannot be trusted on (deviation 2).
5. **Little survives 10 THz and nothing 30 THz.** (Corrected on
   2026-11-11; the first version of this record said nothing survives
   10 THz.) With a uniform field every device is at zero from 3 THz. With
   the drop confined to the neck a residue remains at 10 THz: 0.05 +- 0.03
   (G3) and 0.09 +- 0.03 A/W (G4) at 700 nm, and 0.23 +- 0.04 A/W for G4
   at 100 nm, which is 6 standard errors. At 30 THz no device, field model
   or mean free path gives a current distinguishable from zero (worst
   case 1.9 standard errors). The 28 THz response reported for a diode of
   this size is not reproduced; at that frequency the photon energy is
   close to the Fermi energy and the model is outside its range.
6. **The edges and electron-electron scattering decide how much is
   left.** Half-diffuse walls take the 1 THz figure to 0.42 A/W (uniform)
   and 0.14 A/W (confined). A 100 nm mean free path, which is also the
   crude bound for electron-electron scattering at 300 K, gives 0.22 and
   0.42 A/W. The defensible room-temperature range for an encapsulated
   0.55 um funnel is 0.14 to 0.7 A/W.
7. **Still a detector, not a harvester.** At 1 THz the matched-load
   conversion efficiency estimated from the short-circuit current and the
   static conductance is 0.06 % at 0.1 V and 0.2 % at 0.3 V drive.

**For the paper.** "The ballistic funnel is not a THz rectifier" is wrong
as a general statement and has to become a length criterion: geometric
rectification reaches THz when the device is shorter than about
0.4 v_F / f and no longer than the mean free path. For such a device the
shape mechanism is at least as strong as the circuit mechanism. The title
claim does not hold for the funnel in the ballistic regime. The Tesla
valve is unaffected: it has no ballistic asymmetry to scale.

## Acceptance ladder (`bshort/ladder_B7.txt`, 1e6 particles per side; 8/8 pass)

| test | result |
|---|---|
| T18a scale invariance, static: Device D against its half-scale copy (currents x 2), +-0.1 V | 188.0 vs 188.6 uA and -117.4 vs -117.6 uA; A = 1.602 vs 1.603 |
| T18b the same under ac drive: 10 GHz against 20 GHz, 300 GHz against 600 GHz | I_dc 11.9 +- 0.4 vs 12.1 +- 0.5 uA; 0.3 +- 0.5 vs -0.4 +- 0.4 uA; admittances equal within 2 sigma |
| T19 scale invariance with scattering: 700 nm against 350 nm at half scale | static 108.3 +- 0.6 vs 110.0 +- 0.5 uA (2.3 sigma); ac 2.27 +- 0.23 vs 1.82 +- 0.23 uA |
| T20 symmetric controls of G2, G3, G4 at 0.1, 1, 10 THz | zero; worst 1.3 sigma |
| T21 chord cap v_F / (40 f) against v_F / (160 f), G3, confined drop, 700 nm, 3 THz | 3.13 +- 0.10 vs 3.20 +- 0.10 uA |

## Deviations from the plan

1. **Frequency caps.** The sweeps stop where f L / v_F exceeds about 20:
   G1 (3 um) at 3 THz and G2 (2.1 um) at 10 THz. Beyond that the response
   is zero in every other device and the points are the slowest of the
   stage. 174 sweep points instead of 192.
2. **The responsivity definition was not fixed in the plan, and it
   matters.** The plan said "matched array against available power".
   Three definitions were computed:
   - R_abs, the rectified current per watt absorbed at the static
     conductance. This is an array of N devices in parallel matched to
     75 ohm (N = 15 for G3), with its impedance taken as independent of
     frequency. **This is the headline figure**, because it does not use
     the model's ac admittance.
   - R_1, one device on a 75 ohm antenna, using the device's own
     first-harmonic admittance. Robust, because one device is far above
     75 ohm at every frequency.
   - R_cm, per watt absorbed by the conduction current at the drive
     frequency. Not robust: in an imposed-field model the conduction
     admittance falls, vanishes and turns negative with frequency
     (Stage B2, deviation 5), and this figure then diverges. It is in the
     tables only where Re Y_1 exceeds 0.3 of the static conductance.
   The decision in verdict 3 uses R_abs. With R_cm the uniform-field
   figure would be 1.9 A/W and the outcome "alive and better".
3. **T21 moved from 30 THz to 3 THz** (with the 700 nm mean free path).
   At 30 THz the response is zero and the comparison tested nothing; the
   first run is kept as `ladder_B7_first.txt`.
4. **The reference has two values.** The plan compared with 0.34 A/W, the
   best plasmonic figure, which is a 100 K number. The 300 K plasmonic
   figure is 0.09 A/W. Both are quoted.

## Devices and runs

n = 1.4e12 cm^-2, 300 K smearing, specular walls, V_a = 0.1 V, Fermi
wavelength 30 nm. Field models: uniform, and a drop over one neck width
centred on the neck. Mean free paths: infinite, 700 nm, 100 nm.

| | shoulder / neck (nm) | leads (nm) | L (um) | N for 75 ohm (700 nm) |
|---|---|---|---|---|
| G0 Device D (Stage B2) | 2000 / 175 | 2000, 2420 | 6.00 | 13 |
| G1 Device D at half scale | 1000 / 88 | 1000, 1210 | 3.00 | 20 |
| G2 Device D, short leads | 2000 / 175 | 250, 250 | 2.08 | 10 |
| G3 short funnel | 500 / 100 | 100, 100 | 0.55 | 15 |
| G4 Zhu-like (assumed 45 deg taper) | 400 / 75 | 169, 169 | 0.50 | 21 |

| run | what | points |
|---|---|---|
| S1 | static curves at +-0.05 and +-0.1 V, G1 to G4, 2 fields x 3 mean free paths | 96 |
| S2 | frequency sweeps, 0.01 to 30 THz with the caps of deviation 1 | 174 |
| S3 | Device D, no scattering, 2, 3 and 5 THz (f L / v_F = 12 to 30) | 6 |
| S4 | mirror-symmetric controls, 0.1, 1 and 10 THz | 22 |
| S5 | G3: half-diffuse walls; amplitude ladder at 1 THz | 16 |

Wall time 48 minutes on ten cores.

## Results

1. **Static asymmetry (`B7_length.png` a).** With a uniform field:

   | | no scattering | 700 nm | 100 nm |
   |---|---|---|---|
   | G0, 6 um | 1.61 | 1.10 | not run |
   | G1, 3 um | 1.61 | 1.13 | 1.06 |
   | G2, 2.1 um | 1.91 | 1.55 | 1.33 |
   | G3, 0.55 um | 1.83 | 1.56 | 1.28 |
   | G4, 0.50 um | 1.60 | 1.30 | 1.15 |

   Two things follow. The asymmetry depends on mean free path over device
   length, as expected. And **the leads cost Device D most of its
   room-temperature asymmetry**: cutting them from 2 um to 250 nm (G2)
   raises A at 700 nm from 1.10 to 1.55 and the clean value from 1.61 to
   1.91. The paper's weak experimental anchor (1.09 computed against 1.66
   measured for Device D) therefore depends on where the field is taken
   to act.
   **Checked on 2026-11-11 against the source** (Nguyen et al., full text
   and Table 1): Device D is 6 um from end to end with Cr/Au edge contacts
   on the two ends, measured two-terminal. So the real device does have
   the 2 um leads, and G2 is a different device, not a better model of
   Device D. What G2 shows is the sensitivity to the field profile: a
   uniform drop over 6 um gives 1.10, a uniform drop over the taper and
   neck alone 1.55, and a 500 nm drop centred on the neck of the full
   device 0.96 (reversed). The anchor is not rescued; the model does not
   predict the dc asymmetry at 700 nm without a computed field. Two more
   facts from that paper: its 0.7 um mean free path is quoted at a low
   "threshold" density, and its own model uses 0.25 um.
2. **The confined field can reverse the sign.** With the drop confined to
   the neck the static asymmetry falls below 1 when scattering is strong:
   0.81 for G3 and 0.71 for G2 at 100 nm, 0.91 for G2 at 700 nm. Under ac
   drive the same rows are negative at low frequency and positive at THz
   (`B7_responsivity.png` b, d), as Stage B2 found for the 6 um device.
   The sign of the rectified current is not a robust prediction of this
   model; its size at THz is more so.
3. **Transfer function (`B7_transfer.png`).** With no scattering and a
   uniform field all five devices follow one curve within about 0.2 above
   f L / v_F = 0.2. The
   gigahertz step of Stage B2 (a plateau at 0.67 for f L / v_F of 0.06 to
   0.2) is present for G0 and its copy G1 and absent for the short-lead
   devices (G2: 0.94 at 0.06): it is the slow electrons of the leads, as
   B2 inferred. Device D extended to f L / v_F = 12, 18 and 30 gives zero
   within 0.04 for both fields.
4. **The cut-off follows the flight time when the device is shorter than
   the mean free path.** For G3 the transfer function at 700 nm is the
   clean one (0.76 and 0.32 at 0.3 and 1 THz in both). For G2, where the
   mean free path is a third of the length, it falls earlier (0.41
   against 0.75 at f L / v_F = 0.2).
5. **Responsivity at 1 THz, 700 nm (`B7_length.png` b), A/W:**

   | | uniform, R_abs | uniform, R_1 | confined, R_abs | confined, R_1 |
   |---|---|---|---|---|
   | G0, 6 um | 0.00 +- 0.04 | 0.000 | 0.75 +- 0.05 | 0.23 |
   | G1, 3 um | 0.08 +- 0.04 | 0.016 | 0.62 +- 0.05 | 0.11 |
   | G2, 2.1 um | 0.01 +- 0.05 | 0.004 | 0.80 +- 0.04 | 0.20 |
   | G3, 0.55 um | 0.70 +- 0.02 | 0.17 | 0.71 +- 0.03 | 0.14 |
   | G4, 0.50 um | 0.57 +- 0.03 | 0.10 | 0.70 +- 0.02 | 0.12 |

   The confined-drop column is nearly independent of length: that
   mechanism was already alive at 6 um (B2 reported it per single device,
   0.2 to 0.45 A/W). What the short devices add is the uniform-field
   column. Johnson-noise-limited NEP of the matched G3 array at 300 K:
   21 pW/Hz^1/2.
6. **Above 1 THz.** Uniform field: zero at 3 THz in G3 and G4. Confined
   drop: 0.85 (G3) and 1.03 (G4) A/W at 3 THz with 700 nm, then an order
   of magnitude less at 10 THz (verdict 5) and zero at 30 THz. A
   neck-confined drop in a 0.5 um device rectifies to a few THz.
7. **Controls.** All 22 symmetric-control points are zero within
   2.1 sigma.
8. **G3 extras (S5), 700 nm, 1 THz.** Half-diffuse walls: static
   asymmetry 1.43 (uniform) and 0.90 (confined); I_dc 1.70 and 0.54 uA
   against 3.03 and 2.78 uA specular. Amplitude: the uniform-field
   current follows V_a^2 to 0.1 V (0.77 uA at 0.05 V) and is 0.80 and 0.57
   of the law at 0.2 and 0.3 V; the confined-drop current is already 19 %
   below the law at 0.1 V and saturates at 0.2 V, so its small-signal
   responsivity is 0.87 A/W.

## Limitations

The potential is imposed. For a 0.55 um device the two field models agree
at 1 THz, which removes the largest uncertainty of Stage B2, but neither
is a computed field and neither includes the displacement current or any
capacitance, so the impedance the antenna sees is not known. Classical
trajectories through necks of 5 to 7 modes. No electron-electron
scattering beyond the 100 nm bound. No gate and no plasmons: in a real
short gated device both mechanisms act together. Semiclassical drive:
indicative above 10 THz. The matched array is an idealisation; no array
or antenna was modelled.
