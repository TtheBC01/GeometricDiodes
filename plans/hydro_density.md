# Plan: carrier density, gate distance and contact resistance (Stage H4)

Written 2026-11-06, before any code. Companion to `plans/hydro_thz.md`
(H2, small signal), `plans/hydro_largesignal.md` (H3, finite amplitude) and
`results/THZ.md`. Stages H2 and H3 were run at one operating point, n =
1e12 cm^-2 under a 30 nm gate, and found the 1 um graphene funnel to be a
square-law detector whose efficiency is proportional to the available power
up to about 1 mW and peaks at 0.1 to 0.4 %. H3 also showed that the ceiling
is not saturation or breakdown but the small-signal curvature. Stage H4
asks: **does any gated-graphene operating point reach per-cent conversion
efficiency at the power a THz antenna delivers, and what do realistic
contacts do to the detector figures?**

## 0. What H3 already says about the lever

In nondimensional units (V in V_unit, R in R_unit) the matched-load
efficiency is eta = 2 r^2 (R_ant / R_dc) Va^2 with r = V_oc / Va^2. With
the ac amplitude V_a in volts and P_av = V_a^2 / (8 R_ant):

    eta / P_av = 16 r^2 R_ant^2 / (R_dc V_unit^2)       (1/W)

(2.4 /W at 100 K, 0.1 THz: r 0.101, R_ant 75 ohm, R_dc 168 ohm, V_unit
1.487 V). H3 found that the law holds to V_a ~ 0.5 V_unit and that the
efficiency peaks near V_a ~ 0.7 V_unit at a saturation factor S = 0.6 to
0.9 of the law, so

    eta_peak ~ 2 r^2 (R_ant / R_dc) (0.7)^2 S,     P_peak ~ (0.7 V_unit)^2 / (8 R_ant).

The first expression contains only dimensionless quantities. The density
and the gate distance enter the solver through the groups Gamma = W / (s0
tau), q = s_q^2 / s0^2, 1/Re_s = nu / (s0 W), Omega = f / f_unit and
R_ant / R_unit, and through the physical scales V_unit ~ U_0 ~ n d and
R_unit. So the hypothesis to test is:

1. **The peak efficiency is a pure number of order r^2, a few per cent at
   best, at every (n, d).** Lowering n or d cannot raise it beyond what the
   dimensionless response r(Gamma, q, Omega, R_ant/R_unit) allows.
2. **What n and d set is the power at which the peak occurs,** P_peak ~
   V_unit^2 / R_ant ~ (n d)^2, and therefore the efficiency at a given
   available power: below the knee eta ~ P_av / P_peak x eta_peak, so a
   device with its knee at 1 uW converts 1 uW at eta_peak while the H3
   device converts it at 2e-6.

If 1 and 2 hold, the design rule for a rectenna element is "put the
saturation knee at the available power", the reachable efficiency is
r^2-limited at a few per cent, and the geometric diode question is closed
with a quantitative map. If r itself grows at low density (through q and
Gamma) the map says by how much. Either way the answer is cheap: the H2
perturbation solver costs 20 s per frequency on the 1 um funnel.

## 1. Operating points and validity

Grid: n = 1e12, 5e11, 2e11, 1e11, 5e10 cm^-2; d = 30, 10, 5 nm; eps_r 3.8;
W_ref = 4 um (1 um funnel, contacts 4 um wide). Scattering: the H2 sets
(100 K: tau_mr 5 ps, tau_ee 1 ps, nu 0.1 m^2/s; 300 K: 1 ps, 0.1 ps, 0.03)
held fixed across the grid, which is the constant-tau assumption; the
constant-mobility alternative tau ~ sqrt(n) is run as a variant at 100 K.
Physical scales on the grid (ThzUnits; E_F = hbar v_F sqrt(pi n)):

| n (cm^-2) | d (nm) | E_F (meV) | U_0 (V) | V_unit (V) | s0 (1e6 m/s) | q | R_unit (ohm) | f_unit (THz) | Gamma 100 K / 300 K | P_knee at 75 ohm (uW) |
|---|---|---|---|---|---|---|---|---|---|---|
| 1e+12 | 30 | 117 | 1.429 | 1.487 | 3.57 | 0.04 | 65 | 0.142 | 0.22 / 1.12 | 921 |
| 1e+12 | 10 | 117 | 0.476 | 0.535 | 2.14 | 0.11 | 39 | 0.085 | 0.37 / 1.87 | 119 |
| 1e+12 | 5 | 117 | 0.238 | 0.296 | 1.59 | 0.20 | 29 | 0.063 | 0.50 / 2.51 | 36.6 |
| 5e+11 | 30 | 82 | 0.714 | 0.756 | 3.03 | 0.05 | 78 | 0.120 | 0.26 / 1.32 | 238 |
| 5e+11 | 10 | 82 | 0.238 | 0.279 | 1.84 | 0.15 | 47 | 0.073 | 0.43 / 2.17 | 32.5 |
| 5e+11 | 5 | 82 | 0.119 | 0.160 | 1.39 | 0.26 | 36 | 0.055 | 0.57 / 2.87 | 10.7 |
| 2e+11 | 30 | 52 | 0.286 | 0.312 | 2.44 | 0.08 | 100 | 0.097 | 0.33 / 1.64 | 40.5 |
| 2e+11 | 10 | 52 | 0.095 | 0.121 | 1.52 | 0.22 | 62 | 0.061 | 0.52 / 2.62 | 6.13 |
| 2e+11 | 5 | 52 | 0.048 | 0.074 | 1.19 | 0.35 | 48 | 0.047 | 0.67 / 3.37 | 2.26 |
| 1e+11 | 30 | 37 | 0.143 | 0.161 | 2.09 | 0.11 | 120 | 0.083 | 0.38 / 1.91 | 10.8 |
| 1e+11 | 10 | 37 | 0.048 | 0.066 | 1.34 | 0.28 | 77 | 0.053 | 0.60 / 2.99 | 1.82 |
| 1e+11 | 5 | 37 | 0.024 | 0.042 | 1.07 | 0.44 | 62 | 0.043 | 0.75 / 3.74 | 0.744 |
| 5e+10 | 30 | 26 | 0.071 | 0.084 | 1.80 | 0.15 | 146 | 0.072 | 0.44 / 2.22 | 2.97 |
| 5e+10 | 10 | 26 | 0.024 | 0.037 | 1.19 | 0.35 | 97 | 0.047 | 0.67 / 3.37 | 0.566 |
| 5e+10 | 5 | 26 | 0.012 | 0.025 | 0.98 | 0.52 | 80 | 0.039 | 0.82 / 4.09 | 0.259 |

Validity limits, drawn on the plan figure and carried as flags in the CSV:

- **Fermi liquid:** E_F >= 3 k_B T. At 300 K (26 meV) that excludes
  n <= 2e11 (52 meV is 2 k_B T); at 100 K (8.6 meV) all five densities
  pass, 5e10 (26 meV) on the line.
- **Gating:** U_0 = e n / C_g >= 4 k_B T, so the gate controls the density
  against thermal smearing; at 300 K this excludes U_0 < 0.1 V (the 5 nm
  gate below 5e11 and the 10 nm gate below 2e11), at 100 K U_0 < 34 mV
  (the three corner points (1e11, 5 nm), (5e10, 10 nm), (5e10, 5 nm)).
- **Local capacitance:** d << neck width (1 um) and d << plasmon
  wavelength s0 / f (above 1 um at 1 THz everywhere on the grid); always
  satisfied here, which is why the 1 um funnel is the right device for
  this sweep.
- **Quantum capacitance:** q = s_q^2 / s0^2 rises from 0.04 to 0.5 across
  the grid; the graphene closure carries it exactly, but points with q >
  0.3 are where the local-capacitance model is least like a real gate
  stack (the quantum capacitance then dominates the geometric one) and are
  marked.

## 2. Contact resistance

A contact resistance R_c (ohm um, divided by the 4 um contact width) in
series with each contact is a port change only: the source port becomes
Thevenin(Va, R_ant + R_c, R_L + R_c) and the grounded drain becomes
Thevenin(0, R_c, R_c). Nothing in the solver changes. The antenna then
sees Z_dev + 2 R_c, the dc Thevenin resistance is R_dc + 2 R_c, the
Johnson noise of the NEP uses R_dc + 2 R_c, and the open-circuit dc voltage
is read across the device (the drain carries no dc current at open
circuit, so V_d = 0 still). Values: 0, 100, 300, 1000 ohm um (0, 25, 75,
250 ohm), spanning the best reported graphene contacts to ordinary ones.
In second-order perturbation the dc problem is linear, so I_sc (R_dc + 2
R_c) = V_oc must hold exactly for every R_c: that is the ladder check.

## 3. Numerics

Everything is the H2 perturbation solver on the S2 production mesh of the
1 um funnel (hw 80, hm 480 nm, 18601 nodes), tau_ee as in the sets (viscoelastic), per
frequency one complex linear solve and one dc solve, 20 s. The quantities
per (n, d, set, f, R_c): r = V_oc/Va^2, I_sc/Va^2, R_dc, Z seen by the
antenna, P_in/P_av, R_I, R_V, NEP (Johnson, R_dc + 2 R_c), eta/P_av (1/W,
matched dc load R_L = R_dc + 2 R_c, from the formula above), the predicted
knee P_knee = (0.5 V_unit)^2 / (8 R_ant), the predicted peak eta_peak = 2
r^2 R_ant / (R_dc + 2 R_c) x 0.49 x S(f, T) with the H3 saturation factors
(S = 0.86, 0.89, 0.60 at 100 K and 1.14, 1.10, 0.81 at 300 K for 0.1, 0.3,
1 THz), and the efficiency at 1, 10, 100 uW as min(eta/P_av x P, eta_peak).

The transfer of S from H3 assumes the saturation sits at the same V_a /
V_unit at every (n, d). That is the one thing the small-signal solver
cannot check, so one time-domain ladder (gd_thz_power, Va 0.1, 0.3, 0.5,
0.7, 0.3 THz, both sets) is run at a second operating point, n = 5e11, d =
10 nm (valid at both temperatures), where V_unit is 5 times smaller and the
predicted knee is 30 uW instead of 0.9 mW; the saturation factor at Va 0.5
and 0.7 must agree with H3's to 20 % (the time solver is Newtonian, as in
H3, and the comparison is made against its own BDF2-discrete law).

## 4. Acceptance ladder

| test | what it checks | criterion |
|---|---|---|
| T15 | contact-resistance bookkeeping at (1e12, 30 nm), 100 K, 0.3 THz, R_c = 300 ohm um: I_sc (R_dc + 2 R_c) = V_oc; R_c = 0 reproduces the S2 row; P_in <= P_av | 1e-3; 1e-6; passive |
| T16 | the physical layer only enters through the groups: (n, d, tau, nu, R_ant) co-scaled to the same (Gamma, q, 1/Re_s, Omega, R_ant/R_unit) must give the same nondimensional r and Z | 1e-6 |
| T17 | units at (5e11, 10 nm): s0, U_0, V_unit, R_unit, f_unit vs hand values; the DS dU vs Ua^2 / 4 U_0 closure check of T9 at this point | 1e-6; within 10 % |

T15 to T17 join the quick ladder (seconds each).

## 5. Science runs

| run | what | set | f (THz) | R_c | cost |
|---|---|---|---|---|---|
| D1 | the (n, d) grid, 15 points, dss, 75 ohm antenna | 100 K, 300 K | 0.1, 0.3, 1.0 | 0 | 90 solves, 30 min CPU |
| D1m | D1 with the antenna matched to the device, R_ant = Re Z at each point | both | same | 0 | 90 solves (second pass) |
| D1t | D1 at 100 K with tau_mr, tau_ee ~ sqrt(n) (constant mobility) | 100 K | same | 0 | 45 solves |
| D2 | contact resistance at the H3 point and at the best D1 point | both | same | 100, 300, 1000 ohm um | 36 solves |
| D3 | saturation transfer: time-domain ladder at (5e11, 10 nm), Va 0.1 to 0.7 | both | 0.3 | 0 | 8 points, 1.5 h wall |

D1 is the result: a map of eta/P_av, P_knee and eta_peak over (n, d) with
the validity boundaries, and the efficiency at 1, 10 and 100 uW. D1m says
whether a matched antenna changes the conclusion (the efficiency formula
has R_ant^2 / R_dc in it, so matching is not neutral). D2 gives the
realistic NEP and efficiency for the paper. D3 validates the transfer of
the H3 saturation factors. Total under two hours of wall time on ten cores.

## 6. Deliverables

- `apps/thz_sweep.cpp`: `--Rc` (ohm um); `--tauscale` for D1t; CSV gains
  E_F/kT, U_0/kT, q, the validity flags, NEP, eta/P_av, P_knee, eta_peak,
  and eta at 1/10/100 uW.
- `apps/thz_tests.cpp`: T15 to T17 in the quick ladder.
- `tools/thz_density_runs.sh`: D1, D1m, D1t, D2 in parallel; D3 through
  `gd_thz_power --n --d`.
- `tools/thz_density_plot.py`: the (n, d) maps (eta/P_av at 10 uW, P_knee,
  eta_peak) with validity boundaries, one panel per set and frequency; the
  contact-resistance curves; the D3 ladder against H3.
- `tools/thz_density_tables.py`: the tables for `results/THZ.md`, section
  S6, which closes with the design rule and the per-cent verdict.

## 7. Out of scope

Density-dependent viscosity beyond the tau ~ sqrt(n) variant; the 175 nm
funnel (its 30 nm gate violates d << neck at the thin-gate end of this
grid); contact capacitance and the antenna's reactance; electron heating,
which at U_0 ~ 0.1 V and milliwatt drive is the next physics but needs an
energy equation the solver does not have; and any claim about the
ballistic transit-time nonlinearity (Stage B2).
