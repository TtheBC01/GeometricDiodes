# Plan: the ballistic funnel under THz drive (Stage B2)

Written 2026-11-09, before any code; results and deviations in
`results/BALLISTIC_THZ.md`. Companion to `results/KILLTEST.md`
(Stage 1, the ballistic kill test) and `results/THZ.md` (Stages H2 to H4,
the hydrodynamic-plasmonic answer). Figure: `plans/ballistic_thz.png`.

The hydrodynamic stages closed their question: at THz the gated graphene
funnel is a Dyakonov-Shur detector whose rectification is the circuit
asymmetry of any gated channel, the Tesla shape adds nothing, and no
density or gate reaches per-cent efficiency. One mechanism was excluded
from that answer by construction. In the ballistic kill test the funnel
rectifies through its shape alone, A = 1.62 at 0.1 V in the clean limit,
and S4 of `THZ.md` noted that this asymmetry, if it held at THz, would be a
curvature responsivity of about 1 A/W, several times the hydrodynamic
figures. Whether it holds is a transit-time question the fluid model cannot
ask. Stage B2 asks it: **does the dc current of the ballistic funnel under
an ac drive of 0.1 to 1 THz keep its quasi-static value, or does it roll
off with the time an electron needs to cross the region where the field
lives?**

## 0. What Stage 1 already says, and the prediction

From `results/killtest.csv` (Device D funnel, p = 1, 300 K, n = 1.4e12,
E_F = 0.138 eV):

| lambda | I(+0.1 V) | I(-0.1 V) | I_2 = (I_+ - I_-) / 2 V^2 | quasi-static I_dc at V_a = 0.1 V | R_I (75 ohm available power) |
|---|---|---|---|---|---|
| inf | 189.5 uA | 116.7 uA | 3.64 mA/V^2 | 18.2 uA | 0.87 A/W |
| 700 nm | 108.4 uA | 99.8 uA | 0.43 mA/V^2 | 2.2 uA | 0.10 A/W |

Quasi-static means V(t) = V_a cos(2 pi f t) slow enough that I(t) =
I_static(V(t)); then I_dc = I_2 V_a^2 / 2. The available power uses the
device chord resistance of 650 ohm into a 75 ohm antenna (P_av = 21 uW at
V_a = 0.1 V across the device). Two things follow before any run:

1. **The live case is the clean ballistic limit.** At the room-temperature
   hBN mean free path (700 nm) the quasi-static ballistic responsivity is
   already 0.1 A/W, equal to the hydrodynamic 300 K figures, before any
   transit roll-off. Only lambda of several micrometres (low temperature,
   encapsulated) can make the shape mechanism win.
2. **The roll-off is set by where the field lives, not by the neck.** In
   the uniform-field model of Stage 1 the electron gains its extra energy
   over the whole 3.6 um from the source contact to the neck (3.6 ps at
   v_F); an oscillating field averages that gain to
   sin(pi f tau) / (pi f tau), half power near 120 GHz and a zero at 280 GHz.
   If the potential drop sits in the constriction, as the self-consistent
   Poisson results of the geometric-diode literature say it does, the
   relevant time is l_F / v_F with l_F the length of the drop, 0.5 ps for
   500 nm, and the half-power point moves to 0.9 THz.

So the uniform-field model, taken at face value, predicts the ballistic
funnel is dead above 100 GHz, and the localised-field model predicts it
survives to 1 THz only if the drop is confined to a few neck widths.
Neither is a result yet, and the sinc guess ignores the reflected electrons
that bounce in the taper, which the Monte Carlo resolves. B2 runs both
field models and brackets the answer.

## 1. Model

The Stage 1 event-driven solver (`src/transport.*`) with three additions.

**Time-dependent imposed potential.** U(x, t) = -e V(t) g(x), V(t) = V_a
cos(2 pi f t), with two profiles g: the Stage 1 ramp g = x / L (uniform
field) and a localised ramp g = clamp((x - x_neck + l_F / 2) / l_F, 0, 1)
centred on the neck, l_F in {175, 500, 1600} nm (one neck width, three
neck widths, the taper length). No self-consistency: the field is imposed,
as in Stage 1 and in the published geometric-diode Monte Carlo work. The
device is 6 um, far below the free-space wavelength at 1 THz (300 um), so
the instantaneous field is the right electrostatic limit for an imposed
potential; what the model leaves out is the charge response (plasmons),
which is the hydrodynamic stages' subject.

**Non-conservative flights.** With the field time dependent the kinetic
energy is no longer E_tot - U(x); the chord stepper integrates dk/dt =
F(x, t) / hbar along each chord and drops the energy re-imposition. The
chord length gains a cap v_F / (40 f) so the field changes by less than
2.5 % of a period per chord. Electrons whose kinetic energy falls below
2 meV keep the Dirac fate; electrons heated above 2 E_F are counted and
reported (the ac field can pump energy into a bouncing electron), and the
exit kinetic-energy histogram is written for every run.

**Two-sided equilibrium injection.** The Stage 1 estimator injects only the
bias window |f_S - f_D| from one side and relies on flux conservation in
a fixed landscape. In a time-varying landscape that identity fails, so B2
injects from both contacts with the equilibrium kinetic distribution
N(E) f(E - E_F) (each contact at its own instantaneous electrostatic
potential, the same kinetic distribution at both), with the injection
phase uniform over one drive period, and forms

    I_dc = (g e / h) Phi [ <T_SD> - <T_DS> ],    Phi = int N(E) f(E - E_F) dE = 1.43 mA for W = 2 um,

the phase averages taken over all injected particles. The first harmonic
I_1 of the drain particle current is accumulated from the arrival phases
(each transmitted particle contributes exp(-i 2 pi f t_arrival) with the
same weight), which gives the device's transport admittance Y_1 = I_1 /
V_a at the drive frequency and hence the antenna mismatch. Displacement
current is not in the model; Y_1 is the conduction part.

**Statistics.** The signal is a difference of two transmitted fluxes of
about 140 uA each (Phi x T, T ~ 0.1); the quasi-static signal at V_a =
0.1 V is 18 uA. With N particles per side the error is 1.4 mA x
sqrt(2 T (1 - T) / N), so N = 1e6 per side gives 0.7 uA, 4 % of the
signal in the clean limit, 30 % at lambda = 700 nm; the 700 nm rows use
4e6. The kill test runs at 16 us per particle (10 cores: 7.2e6 particles
in 115 CPU-s), so a 1e6-per-side point is about one CPU-minute even with
the finer chords.

## 2. Devices and parameters

- `funnel_nguyenD` (Stage 1 geometry: 2 um leads, 30 degree taper to a
  175 nm neck, abrupt shoulder, L = 6.0 um) and `straight_control` (175 nm
  channel between the same leads), which must give I_dc = 0 at every
  frequency and is the systematic check of the two-sided estimator.
- n = 1.4e12 cm^-2 (E_F = 0.138 eV), 300 K Fermi smearing, specular walls
  (p = 1), as in the kill test's headline row. Mean free path lambda = inf
  (clean limit) and 700 nm. Specularity variants are out of scope: the kill
  test showed p enters at the 10 % level once lambda is finite, and the
  question here is the frequency dependence.
- V_a = 0.1 V across the device (the kill test's bias), with V_a = 0.05 V at
  two frequencies to confirm I_dc ~ V_a^2, and V_a = 0.2 and 0.3 V at the
  highest frequency where the response is alive, to see where the ballistic
  square law breaks.
- f = 3, 10, 30, 100, 300, 1000 GHz. The 3 GHz point (period 330 ps against
  a 7 ps transit) is the quasi-static anchor and must reproduce
  I_2 V_a^2 / 2 from the static curve of the same field model.

## 3. Acceptance ladder (joins `apps/tests.cpp` as T12 to T16)

| test | what it checks | criterion |
|---|---|---|
| T12 | non-conservative chord integration in a static field: funnel A(0.1 V) and T(+0.1), T(-0.1) with the energy re-imposition off against the kill-test values | within 3 sigma and 1 % |
| T13 | two-sided equilibrium estimator at static bias equals the bias-window estimator (same V, same landscape); at V = 0 it gives I = 0 | 3 sigma; exactly 0 |
| T14 | quasi-static limit: I_dc at 3 GHz, V_a = 0.1 V, uniform field, equals I_2 V_a^2 / 2 from the static I(V) at +-0.05 and +-0.1 V; Y_1 at 3 GHz equals the static chord conductance | 3 sigma and 5 % |
| T15 | straight control under ac drive, 3 GHz and 1 THz: I_dc = 0; Dirac, trapped and leaked fractions < 1e-3; no electron above 2 E_F | 3 sigma of zero |
| T16 | chord cap: 1 THz, uniform field, v_F / (40 f) against v_F / (160 f) | same I_dc within 3 sigma and 2 % |

T12 and T13 are the bookkeeping of the new estimator against the old one
and run in seconds; T14 is the physics anchor, T15 the symmetry check, T16
the time resolution. Each uses 1e6 particles per side on the coarse
setting; the ladder is a minute.

## 4. Science runs

| run | what | field | lambda | f (GHz) | V_a (V) | points | cost |
|---|---|---|---|---|---|---|---|
| B1 | static I(V) curves, both estimators, for the quasi-static reference | uniform; localised l_F 175, 500, 1600 | inf, 700 | 0 | +-0.025, 0.05, 0.1, 0.2 | 64 | 10 CPU-min |
| B2u | frequency sweep, uniform field | uniform | inf, 700 | 3 to 1000 | 0.1 | 12 (+2 at 0.05) | 20 CPU-min |
| B2l | frequency sweep, localised field | l_F 500 | inf, 700 | 3 to 1000 | 0.1 | 12 (+2 at 0.05) | 20 CPU-min |
| B3 | where the drop lives: l_F sweep at the two decision frequencies | l_F 175, 500, 1600 | inf | 300, 1000 | 0.1 | 6 | 10 CPU-min |
| B4 | amplitude at the highest live frequency | best of B2 | inf | one | 0.05, 0.1, 0.2, 0.3 | 4 | 10 CPU-min |
| B0 | straight control at every B2 frequency | uniform, l_F 500 | inf | 3 to 1000 | 0.1 | 12 | 20 CPU-min |

About 1.5 CPU-hours, ten minutes of wall time on ten cores; every point
writes I_dc, its error, Re and Im Y_1, the fate fractions, the heated
fraction and the exit-energy histogram. The quantities reported per
point: S_B(f) = I_dc(f) / (I_2 V_a^2 / 2) with I_2 from B1 of the same
field model and lambda (the transfer function of the plan figure), and the
responsivity R_I = I_dc / P_av with P_av = V_a,ant^2 / (8 R_ant), R_ant =
75 ohm, V_a,ant = V_a |R_ant + 1 / Y_1| |Y_1| from the measured first
harmonic, so the mismatch is the device's own at that frequency.

## 5. Decision criteria, fixed now

The decision frequency band is 0.3 to 1 THz, where the hydrodynamic stages
place the 1 um and 175 nm funnels at 0.09 to 0.45 A/W (S4: 1 um funnel 0.34
A/W at 100 K and 0.09 at 300 K; 175 nm funnel 0.45 and 0.10), all with the
DS termination, 75 ohm, ideal contacts.

- **Alive:** R_I(1 THz) >= 1 A/W in the clean limit under the localised
  field with l_F <= 500 nm, and S_B(300 GHz) >= 0.5. Then the shape-based
  mechanism beats the best hydrodynamic figure by at least a factor two at
  the frequency of interest and deserves the self-consistent Poisson stage
  (B3) that decides the actual l_F.
- **Closed:** R_I(1 THz) <= 0.3 A/W for both field models in the clean
  limit, or S_B(300 GHz) <= 0.3 for the uniform field and the localised
  field at l_F = 500 nm. Then the transit time kills the ballistic
  asymmetry before the hydrodynamic mechanism takes over, and the paper
  can state that neither transport limit gives a shape-based THz rectenna.
- **In between:** the result is reported as comparable to the hydrodynamic
  figure, with the l_F dependence from B3 as the statement of what a
  self-consistent calculation would have to decide; no further stage is
  proposed from this project.

The 700 nm rows are reported alongside but do not enter the decision: they
only state how far below the clean limit a room-temperature device sits.

## 6. Deliverables

- `src/transport.*`: `TransportParams` gains `field` (uniform | localised,
  `ell_F`), `f_Hz`, `Va` (ac drive replacing `V` when `f_Hz > 0`), and
  `two_sided`; `trace` integrates the time-dependent force and caps the
  chord; `run` records arrival phases and the exit-energy histogram;
  a new `ac_current()` returns I_dc, its error, Y_1 and the diagnostics.
- `apps/tests.cpp`: T12 to T16.
- `apps/bthz.cpp` (`gd_bthz`): the B0 to B4 driver with flags
  `--geo`, `--field`, `--ellF`, `--lambda`, `--f`, `--Va`, `--N`, writing one
  CSV row per point and the exit-energy histogram beside it.
- `tools/ballistic_thz_runs.sh`, `tools/ballistic_thz_plot.py` (S_B(f) and
  R_I(f) against the hydrodynamic reference and the decision lines, the
  l_F dependence, the amplitude ladder), `tools/ballistic_thz_tables.py`.
- `results/BALLISTIC_THZ.md`: setup, ladder, runs, the verdict against the
  criteria above; `README.md` gains the Stage B2 section; `THZ.md` S4 item 3
  gets a pointer to the result.

## 7. Out of scope

Self-consistent electrostatics (Stage B3, only if B2 says alive); the gate
and the plasmon response (the hydrodynamic stages); electron-electron
scattering and the crossover between the two transport limits (at 300 K
the e-e length is about 100 nm, so the ballistic picture there is an upper
bound by construction); hole transport and interband processes at the
amplitudes of B4; displacement current and the antenna's reactance; edge
specularity below 1; the Tesla geometries, whose ballistic asymmetry is
already 1 at dc and cannot grow with frequency.
