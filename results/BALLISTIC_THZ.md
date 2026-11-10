# Stage B2 results: the ballistic funnel under THz drive (2026-11-09)

Plan: `plans/ballistic_thz.md` (figure `plans/ballistic_thz.png`). Code:
`src/transport.*` (time-dependent imposed potential, two-sided estimator),
`apps/bthz.cpp` (`gd_bthz`), ladder T12 to T16 in `apps/tests.cpp`. Runs:
`tools/ballistic_thz_runs.sh`, results in `results/bthz/` (`B1.csv`,
`B2u.csv`, `B2l.csv`, `B3.csv`, `B0.csv`, `B4.csv`, histograms, logs), tables
`results/bthz/tables_B2.md`, figures `results/bthz/B2_transfer.png`,
`B2_ellF_control.png`, `B4_amplitude.png`.

The question: the ballistic kill test (Stage 1, `KILLTEST.md`) found that
the Device D funnel rectifies through its shape alone, A = 1.62 at 0.1 V in
the clean limit, a curvature responsivity near 1 A/W if it held at THz.
Stages H2 to H4 found the hydrodynamic funnel to be a Dyakonov-Shur
detector at 0.09 to 0.45 A/W with no shape contribution at THz. Stage B2
asks whether the ballistic asymmetry survives an ac drive of 0.3 GHz to
1 THz, in the same model as the kill test plus time dependence.

## Method in one paragraph

The Stage 1 event-driven solver with an imposed potential U(x, t) =
-e V(t) g(x), V(t) = V_a cos(2 pi f t). Two profiles g: the Stage 1 uniform
ramp over the 6 um device, and a localised ramp of length l_F centred on
the neck (175, 500, 1600 nm). Flights integrate dk/dt = F(x, t) along chords
capped at v_F / (40 f) and at l_F / 20 inside the localised region, ending
on the region's planes; the energy re-imposition of Stage 1 is off when the
field is time dependent. Injection from both contacts with the equilibrium
kinetic distribution N(E) f(E - E_F), phase uniform (stratified) over one
period; I_dc = (g e^2 / h) Phi [<T_SD> - <T_DS>] with Phi = 1.43 mA for the
2 um contacts, and the first harmonic of the drain terminal current from
the arrival and departure phases gives the transport admittance Y_1 =
I_1 / V_a. The responsivity uses the available power of a 75 ohm source
with the device's own Y_1 setting the mismatch. Device D funnel, n =
1.4e12 cm^-2, 300 K smearing, specular walls, lambda = inf and 700 nm, 1e6
particles per side (4e6 at 700 nm and at V_a = 0.05 V). The quasi-static
reference I_qs(V_a) = <I_static(V_a cos phi)> comes from the static curves
of the same field model (B1: even and odd polynomial fits to +-0.025,
0.05, 0.1, 0.2 V), and S_B(f) = I_dc(f) / I_qs is the transfer function.

## Acceptance ladder (`bthz/ladder_B2.txt`, 1e6 particles per side, T16 4e6, T17 2e6; 9/9 pass)

| test | what | result | criterion |
|---|---|---|---|
| T12 | dk/dt = F integration without energy re-imposition, static field: T(+0.1), T(-0.1), A against the kill test | T+ 0.14141 (ref 0.14130, z 0.19), T- 0.04148 (z 0.20), A 1.6217 (ref 1.6236) | 3.5 sigma, 1 % |
| T13a | two-sided equilibrium estimator = bias-window estimator at V = 0.1 V | 188.71 +- 0.64 vs 189.23 +- 0.46 uA, z 0.65 | 3.5 sigma |
| T13b | two-sided estimator at V = 0 | -0.15 +- 0.66 uA | 3.5 sigma of zero |
| T14a | quasi-static anchor: I_dc at 300 MHz, V_a 0.1 V against <I(V_a cos phi)> from the static curve | 17.78 +- 0.59 vs 18.27 +- 0.18 uA (0.973) | 3.5 sigma or 5 % |
| T14b | |Y_1| at 300 MHz against the static conductance G1 + 3 I3 V_a^2 / 4 | 1.589 +- 0.013 vs 1.580 mS; phase 0.014 rad, omega tau_mean 0.015 | 3.5 sigma or 5 % |
| T15 | straight control at 3 GHz and 1 THz: I_dc = 0, trapped + leaked < 1e-4, dirac < 1e-2, heated < 1e-3 | -0.80 +- 0.57 and -0.20 +- 0.62 uA; lost 1.5e-6, 5e-7; dirac 1.4e-3, 7.9e-4; heated 0 | as stated |
| T16 | chord cap v_F / (40 f) vs v_F / (160 f), localised 500 nm field, 1 THz | 6.32 +- 0.30 vs 5.55 +- 0.32 uA (z 1.8) | 3.5 sigma or 2 % |
| T17 | Dirac cut against the classical turning point, localised 500 nm field, 1 THz | 5.94 +- 0.45 (dirac 2.9e-3) vs 5.69 +- 0.45 uA (dirac 1.8e-4), z 0.40 | 3.5 sigma |

The Stage 1 ladder T1 to T11 still passes bit for bit (the static uniform
path of the solver is unchanged).

## Deviations from the plan, and what they taught

1. **The quasi-static anchor is 300 MHz, not 3 GHz.** At 3 GHz the dc
   current is 0.82 of the quasi-static value (14.8 vs 18.1 uA), seven
   sigma away, although the transmitted electrons cross the device in
   7 ps. The flight-time histogram (`B1_thist_uniform_laminf.csv`, figure
   `B2_ellF_control.png` c) explains it: the taper scatters source
   electrons into near-grazing directions and those drift back to the
   source contact over tens of picoseconds; 5.3 % of source-injected
   electrons live longer than 20 ps, 0.9 % longer than 50 ps, 0.2 %
   longer than 100 ps. At 3 GHz that population sees the field change
   during its flight, gains or loses energy between wall hits, and both
   T_SD and T_DS rise. At 300 MHz (period 3.3 ns) the dc current is 0.98
   +- 0.04 of the quasi-static value and |Y_1| is the static conductance
   to 3 %. The anchor moved a decade down and the frequency grid gained
   0.3 and 1 GHz. The lesson: the response time of a ballistic device is
   set by its slowest population, not by its mean transit, and a
   converging taper manufactures slow electrons.
2. **The phase of Y_1 at the anchor is not zero.** Im Y_1 / Re Y_1 = 0.18
   at 3 GHz and 0.01 at 300 MHz: the transit lag omega tau with tau the
   mean flight time (0.154 rad predicted from the mean, 0.175 measured at
   3 GHz). T14b tests |Y_1|, not Re Y_1 with Im = 0.
3. **The lost fraction of the two-sided estimator is the Dirac tail, not a
   leak.** The equilibrium injection samples the kinetic distribution down
   to zero energy (weight proportional to E), and 1e-3 to 7e-3 of the
   injected electrons are driven below the 2 meV cut by the field. In a
   static landscape those would have turned back to their own contact and
   carry no current (T13a agrees with the bias window to 0.3 %), so the
   Dirac fraction is reported and bounded at 1e-2 while trapped + leaked
   stays below 1e-4. The plan's single "< 1e-3" bound was written for the
   bias-window estimator.
4. **T16 runs on the localised field at 1 THz,** not the uniform field,
   because the uniform-field response at 1 THz is zero within noise and a
   2 % criterion on zero means nothing.
5. **Negative transport conductance at THz.** Re Y_1 < 0 appears for the
   uniform field at 300 GHz and for the localised field at 1 THz. This is
   the transit-time phase of the conduction current alone; the displacement
   current that would make the total admittance passive is not in an
   imposed-field model. It enters the responsivity only through the
   mismatch factor |1 + R_ant Y_1|, by 10 to 20 %.
6. **The harmonic estimator must book Dirac-cut drain electrons as
   returned.** The first version counted every drain-injected electron as a
   departure and only those reaching a contact as a return, so the 1e-2 of
   drain electrons killed by the cut on the decelerating half-cycle acted
   as a current in phase with V and |Y_1| at 300 MHz came out 6 % below the
   static conductance (T14b failed at seven sigma). Booking the kill as a
   return at the kill time, which is what the dc estimator already assumes,
   gives 1.589 vs 1.580 mS. All ac stages were rerun with the fix; with
   fixed seeds no dc current changed and no responsivity moved by more than
   2 %. T17 and B5 then show that the dc current itself is insensitive to
   the cut.
7. The driver is `apps/bthz.cpp` (the Makefile maps `build/gd_<name>` to
   `apps/<name>.cpp`), not `apps/ballistic_thz.cpp`.

## Runs (`results/bthz/`, 300 K smearing, p = 1, n = 1.4e12 cm^-2)

| run | what | field | lambda (nm) | f | V_a (V) | points | CPU |
|---|---|---|---|---|---|---|---|
| B1 | static I(V), both estimators, flight-time histograms | uniform; localised 175, 500, 1600 nm | inf, 700 | 0 | +-0.025, 0.05, 0.1, 0.2 | 64 | 2.1 h |
| B2u | frequency sweep | uniform | inf, 700 | 0.3 GHz to 1 THz (8) | 0.1 (+0.05 at 0.3 and 300 GHz) | 20 | 1.0 h |
| B2l | frequency sweep | localised 500 nm | inf, 700 | same | same | 20 | 1.0 h |
| B3 | length of the drop | localised 175, 500, 1600 nm | inf | 300 GHz, 1 THz | 0.1 | 6 | 5 min |
| B0 | straight 175 nm control | uniform; localised 500 nm | inf | 8 frequencies | 0.1 | 16 | 15 min |
| B4 | amplitude ladder | localised 500 nm | inf | 300 GHz | 0.05, 0.1, 0.2, 0.3 | 4 | 5 min |
| B5 | Dirac cut against the classical turning point | localised 500 nm; uniform | inf, 700 | 10 GHz, 300 GHz, 1 THz | 0.1 | 10 | 20 min |
| B6 | half-diffuse walls, p = 0.5 (added 2026-11-10) | uniform; localised 500, 175 nm | inf | 0, 0.3 GHz, 300 GHz, 1 THz | 0.1 | 15 | 5 min |

Each point is 1e6 particles per side (4e6 at 700 nm and at V_a = 0.05 V,
2e6 for the 700 nm static points), 16 to 60 us per particle depending on
the chord cap and the mean free path; the whole stage is about five
CPU-hours, forty minutes of wall time on ten cores. Tables for every row
are in `bthz/tables_B2.md`.

## Results

1. **The static reference, and what the field model does to it.** With the
   two-sided estimator the clean uniform-field funnel gives I(+0.1) =
   188.7, I(-0.1) = -117.3 uA (A = 1.61; bias window 189.0 and -117.7),
   an even part of 35.7 uA at 0.1 V and a quasi-static rectified current
   for a 0.1 V cosine drive of I_qs = 18.1 +- 0.3 uA, i.e. 0.87 A/W of
   the available power of a 75 ohm source (P_av 20.9 uW at the 1.73 mS
   static conductance). Confining the drop to the neck lowers the static
   asymmetry, as the self-consistent literature says it should: A(0.1 V)
   = 1.52, 1.50 and 1.40 for l_F = 1600, 500 and 175 nm (I_qs 15.4, 14.7,
   13.1 uA). At lambda = 700 nm the uniform-field asymmetry is 1.10 (I_qs
   2.8 uA, 0.15 A/W) and the localised fields reverse it: A = 0.97, 0.96,
   0.94 (I_qs -0.8 to -1.3 uA), so those rows have no quasi-static
   reference and are reported as responsivities only.
2. **Uniform field, clean limit: the response dies in two steps.** S_B =
   0.98 +- 0.04 at 0.3 GHz, 0.93 at 1 GHz, 0.82 at 3 GHz, 0.67 at 10 GHz,
   0.65 at 30 GHz, 0.41 at 100 GHz, -0.06 +- 0.03 at 300 GHz and 0.05 +-
   0.04 at 1 THz (figure `B2_transfer.png` a). The first step, between 1
   and 10 GHz, is the slow-drift population of deviation 1: it costs a
   third of the response before the transit time of a direct flight
   matters at all. The second step is the transit roll-off of the direct
   electrons, and its zero sits at 300 GHz, where sin(pi f tau) with tau =
   3.6 ps (source contact to neck) has its first zero. R_I at 1 THz is
   0.05 +- 0.04 A/W: with the Stage 1 field model the ballistic funnel is
   not a THz rectifier. At lambda = 700 nm the same curve runs 0.93, 0.90,
   0.85, 0.67, 0.33, 0.35, 0.12, 0.00: dead by 30 GHz.
3. **Localised field, clean limit: alive to 1 THz at a third of the static
   value.** With the drop over 500 nm at the neck S_B = 1.02 +- 0.04 from
   0.3 to 3 GHz (the slow electrons in the lead never see the neck field,
   so the GHz step is gone), 0.87 at 10 GHz, 0.66 at 30 GHz, 0.54 at 100
   GHz, 0.64 at 300 GHz and 0.38 +- 0.04 at 1 THz. The decline from 10 to
   100 GHz is not in the sinc picture either; the transfer is set by the
   whole flight between the contacts, not by the 0.5 ps crossing of the
   field region. R_I = 0.73 A/W below 3 GHz, 0.40 at 100 GHz, 0.73 at 300
   GHz (the mismatch factor |1 + R_ant Y_1| is 0.78 there, Re Y_1 being
   negative, deviation 5) and 0.37 +- 0.04 A/W at 1 THz. The V_a = 0.05 V
   points give S_B 0.98 (0.3 GHz) and 0.57 +- 0.10 (300 GHz): the square
   law holds at THz as at dc.
4. **Where the drop lives decides the number (B3, figure
   `B2_ellF_control.png` a).** At 1 THz S_B = 0.51 +- 0.05 (l_F 175 nm),
   0.38 (500 nm), -0.04 (1600 nm, the taper length), 0.05 (uniform); at
   300 GHz 0.35, 0.64, 0.40, -0.06. The most favourable case the model
   allows, the drop confined to one neck width in the clean ballistic
   limit, gives R_I(1 THz) = 0.45 +- 0.04 A/W. The sinc guess of the plan
   is right about the ordering and the uniform-field zero and wrong by
   up to a factor two elsewhere; the 175 nm drop at 300 GHz (0.35) is
   lower than the 500 nm one (0.64), which no single-transit picture
   gives.
5. **At the room-temperature mean free path the THz response is a
   different effect.** With lambda = 700 nm and the localised drop the dc
   current is -1.4 uA at 0.3 GHz (the reversed static asymmetry), passes
   through zero between 30 and 100 GHz and comes back positive: +1.1 uA at
   100 GHz, +3.4 at 300 GHz, +3.8 +- 0.2 uA at 1 THz, i.e. R_I = 0.20 and
   0.24 A/W. This is a transit-time rectification that does not exist
   quasi-statically; the sign is that of the clean funnel. B5 shows it is
   not an artefact of the Dirac cut: with the classical turning point
   instead, 3.68 +- 0.23 vs 3.80 +- 0.23 uA at 1 THz and 3.40 vs 3.37 at
   300 GHz; the clean points move by less than one sigma too (5.64 vs
   5.53 uA at 1 THz, 8.93 vs 9.32 at 300 GHz, 11.7 vs 12.2 at 10 GHz for
   the uniform field).
6. **The control is zero at every frequency.** Sixteen points, |z| <= 2,
   with the same admittance structure as the funnel (Re Y_1 negative at
   100 GHz for the localised field), figure `B2_ellF_control.png` b. The
   two-sided time-dependent estimator produces no asymmetry of its own.
7. **Amplitude (B4, 300 GHz, 500 nm drop, figure `B4_amplitude.png`).**
   I_dc = 2.1, 9.3, 23.5, 31.6 uA at V_a = 0.05, 0.1, 0.2, 0.3 V against
   a square law of 3.7, 14.9, 59.5, 134 uA: the ac response follows V_a^2
   to 0.1 V and saturates above, 0.40 of the law at 0.2 V and 0.24 at
   0.3 V, with R_I falling from 0.73 to 0.49 and 0.30 A/W (P_av 48 and
   106 uW). No electron is heated beyond its injection energy plus two
   drops; the exit-energy histograms stay within the Fermi tail plus
   e V_a.
8. **Half-diffuse walls do not remove the THz response (B6, `B6.csv`).**
   With p = 0.5 in the clean limit the static asymmetry falls to 1.17
   (uniform field), 1.01 (500 nm drop) and 0.94 (175 nm drop), and the
   uniform-field response is 0.31 A/W at 0.3 GHz and zero at 300 GHz and
   1 THz (0.0 +- 0.6 and -0.1 +- 0.6 uA). The confined-drop response at
   1 THz is nevertheless 6.8 +- 0.7 uA, 0.45 A/W (500 nm) and 4.5 +- 0.6
   uA, 0.31 A/W (175 nm), although at 0.3 GHz it is 0.5 +- 0.6 and -1.5 +-
   0.6 uA. As at lambda = 700 nm (item 5), the THz rectification of the
   neck-confined field is a transit-time effect that does not need the
   static asymmetry, and it is robust to edge roughness at 0.3 to 0.45 A/W.
   The specular clean case is therefore not the unique best case, but no
   variant exceeds 0.45 A/W.
9. **Verdict against the criteria fixed in the plan.** Not alive: the
   best clean-limit responsivity at 1 THz is 0.45 A/W under the
   neck-width drop and 0.37 under the 500 nm drop, against the 1 A/W
   required, and S_B(300 GHz) is 0.35 to 0.64. Not closed either, by the
   letter: the localised fields keep R_I(1 THz) above 0.3 A/W and
   S_B(300 GHz) above 0.3, only the uniform field is dead. The result is
   the plan's "in between": in the ballistic limit the shape asymmetry
   survives to 1 THz only if the potential drop is confined to the neck,
   and then it equals the hydrodynamic funnel's Dyakonov-Shur
   responsivity at 100 K (0.34 to 0.45 A/W) rather than exceeding it, and
   it does so in the most favourable idealisation the model has: infinite
   mean free path, specular walls, an imposed drop of chosen length. At
   the room-temperature mean free path it is 0.2 A/W from a transit-time
   effect, twice the hydrodynamic 300 K figure and far below the
   amplifier-free detectors of the literature. **For the paper: neither
   transport limit gives a shape-based THz rectenna.** The hydrodynamic
   stages put the funnel at a few tenths of a per cent conversion
   efficiency through the DS circuit asymmetry; the ballistic stage puts
   the shape mechanism at the same responsivity at best, with the
   quasi-static factor-of-two advantage of the kill test spent on the
   transit. No self-consistent Poisson stage is proposed: no choice of
   drop length reaches the factor two that would have justified it.

## Limitations

The potential is imposed, not computed: the localised profile stands in
for the self-consistent drop of a ballistic constriction, and the length
l_F is the parameter the model cannot fix. No plasmons, no gate, no
displacement current, no electron-electron scattering (at 300 K the e-e
length is about 100 nm, so the clean ballistic limit is an upper bound by
construction), electrons only (the Dirac cut removes 1e-3 of the injected
electrons instead of letting them Klein-tunnel), specular walls, and the
2D classical trajectories of Stage 1 with their 50 nm feature-size floor.
