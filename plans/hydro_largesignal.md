# Plan: large-signal conversion efficiency of the plasmonic funnel (Stage H3)

Written 2026-10-30, before any code. Companion to `plans/hydro_thz.md`
(Stage H2, second-order perturbation) and `results/THZ.md`. Stage H2 gave
the small-signal answer: with the Dyakonov-Shur termination a 1 um graphene
funnel delivers 0.1 to 0.3 A/W at 100 K across 10 GHz to 1 THz into a load an
antenna can drive. That is a detector figure. The project asks for a rectenna
element, and a rectenna is judged by **conversion efficiency**: dc power out
per THz power in, at the power an antenna actually collects. Stage H3 asks:
**how does the rectified dc power of the funnel grow with incident power, where
does the quadratic law break, what is the peak efficiency, and does the
electron fluid survive the amplitude needed to reach it?**

Stage H2's plan (section 0, point 3) asserted that small signal is the
regime that matters. That is true for detection and false for energy
conversion; this stage is the correction.

## 0. What the small-signal result already says

In second-order perturbation the open-circuit dc voltage is V_oc = r Va^2 and
the device is a dc Thevenin source of resistance R_dc. Into a matched dc load
R_L = R_dc the dc power is V_oc^2 / (4 R_dc); the available antenna power is
Va^2 / (8 R_ant). Hence, in nondimensional units (V in V_unit = 1.49 V, R in
R_unit = 65 ohm for W_ref = 4 um, R_ant = 75 ohm = 1.154),

    eta_small = 2 r^2 R_ant / R_dc * Va^2

| set | f | r = V_oc/Va^2 | R_dc | eta_small / Va^2 | eta at Va 0.1 (37 uW) | eta at Va 0.3 (0.33 mW) |
|---|---|---|---|---|---|---|
| 100 K | 0.11 THz | 0.101 | 2.58 | 9.2e-3 | 9e-5 | 8e-4 |
| 100 K | 0.28 THz | 0.088 | 2.58 | 6.9e-3 | 7e-5 | 6e-4 |
| 100 K | 0.91 THz | 0.143 | 2.58 | 1.8e-2 | 1.8e-4 | 1.6e-3 |
| 300 K | 0.11 to 0.91 THz | 0.12 to 0.13 | 8.91 | 3.9e-3 | 4e-5 | 3.5e-4 |

(Va = 1 is an ac amplitude equal to the 1.49 V unit, above the 1.43 V gate
overdrive: the channel would be fully depleted on the negative half cycle.)

So the quadratic law alone bounds the efficiency below one per cent for any
amplitude the fluid can carry, and the only question large-signal theory can
change is **downward**: where saturation sets in and how fast. The study is
therefore cheap insurance against a referee's first question, and its most
likely outcome is a quantified statement that the gated-plasmonic funnel is a
detector, not a harvester, at n = 1e12 cm^-2. The expensive alternative (lower
density, where r grows as 1/U_0) is item 4 of the research agenda and out of
scope here.

## 1. Quantities

All from the full nonlinear time-domain solver (`ThzTimeSolver`, BDF2) run to
periodic steady state under a drive of amplitude Va at frequency Omega, with
the port voltages V_c(t) and currents I_c(t) recorded every step.

- P_av = Va^2 / (8 R_ant): available antenna power.
- P_in = < V_s I_s + V_d I_d > over the last period: power absorbed by the
  graphene (includes the dc part, which is small).
- I_dc = < I_dev > over the last period at the loaded contact; P_dc = I_dc^2 R_L.
- eta_av = P_dc / P_av (the rectenna figure), eta_abs = P_dc / P_in.
- n_min, n_max over the last period: the fluid-validity indicator. The
  gated closure needs n > 0 everywhere; below n ~ 0.3 the model (local
  capacitance, Drude mass at fixed n) is no longer credible and the point is
  flagged.
- The small-signal asymptote eta_small Va^2 from the Stage H2 perturbation
  solver on the same mesh, computed in the same run for the same (Omega,
  R_L), so the saturation onset is read as the departure of eta_av from it.

## 2. Circuit

Antenna: Thevenin source Va cos(Omega t) with R_ant = 75 ohm at the source
contact; drain contact grounded to the gate (the Stage H2 `dss` termination,
the one that rectifies). Two ways to take the dc power out:

**A. Antenna as load (no bias tee).** The single port relation the solver has
today, V = Va cos(Omega t) - R_ant I. The rectified dc current flows back
through the antenna resistance, so R_L = R_ant and P_dc = I_dc^2 R_ant. The
zero-hardware bound; mismatched at dc by 4 R_ant R_dc / (R_ant + R_dc)^2
(0.85 at 100 K, 0.60 at 300 K).

**B. Ideal bias tee.** Capacitor in the antenna branch, inductor to a dc
load R_L. At periodic steady state the capacitor passes only the ac current
and the inductor only the dc current, which gives the port relation

    V(t) = Va cos(Omega t) - R_ant [I(t) - I_dc] - R_L I_dc,   I_dc = <I(t)>

i.e. the ac current sees R_ant and the dc current sees R_L. The unknown I_dc
is in principle a fixed point on the period-mean current. Two smoke tests
showed why that is the wrong tool here. The dc charge of the 12 um leads
relaxes through R_ant + R_dc with the RC time of the device, about 22 time
units at 100 K and 60 at 300 K (much longer than the 1/Gamma + Gamma
(2L/pi)^2 estimate of Stage H2, which assumed grounded contacts), and the
abrupt start of the drive injects a charge transient linear in the
amplitude while the signal is quadratic, so small-amplitude runs needed
tens of periods and every bias update re-excited the slow mode. The
implementation therefore:

1. starts the time-domain run from the perturbative periodic state, x(t) =
   rest + Va Re(x_1 e^{i Omega t}) + Va^2 x_20 at t = 0 and t = -dt (n_20 =
   P - (kappa/4) |n_1|^2), so the only transient is the nonlinear
   correction and BDF2 runs from the first step. The linear state must be
   the one of the *time-stepped* operator (i Omega replaced by the BDF2
   symbol (3 - 4/z + 1/z^2)/(2 dt), z = e^{i Omega dt}): the continuous
   solution sits about (Omega dt)^2 ~ 1 % away from the discrete periodic
   orbit, and at Va = 0.01 that 1 % of the ac amplitude is still twenty
   times the dc signal (third smoke test). The small-signal law is reported
   both ways (continuous and BDF2-discrete); their ratio is the
   time-discretisation error of the run;
   The time-domain solver is Newtonian (no Maxwell viscoelastic term), so
   the H3 reference solutions are computed with tau_ee = 0 as well; at
   these parameters (1/Re_s = 7e-3 on the 1 um device) the viscous
   correction to the response is small (Stage H2, S1: inviscid within a
   few per cent of viscous away from the resonances);
2. holds the dc load current I_b fixed at the small-signal prediction
   I_20 Va^2 (the port relation above with a constant I_b);
3. extrapolates the period means over the last three periods (Aitken, one
   slow exponential) and reports the remaining tail;
4. converts the measured point to the resistor fixed point through the
   device's dc Thevenin equivalent at that pumping, V_rect = V_dc - R_dc
   I_dc with the small-signal R_dc, I* = -V_rect / (R_dc + R_L), and
   reports the relative correction (second order in the seed error, so a
   few per cent of R_dc uncertainty costs well under a per cent).

The iterative fixed point remains available (`--bias_updates`) but is not
used for the runs.
R_L = R_dc (matched)
is the main case; R_L = 0 and R_L -> infinity must reproduce I_sc and V_oc
of the perturbation solver at small Va (ladder L2).

Implementation: one new field `Port::Idc_bias` and one flag in
`ThzTimeSolver::run` (`bias_tee`), about thirty lines in `thz_time.cpp`.
`TimeTrace` gains `P_in`, `n_min`, `n_max` and the bias convergence measure.

## 3. Numerics

- BDF2, 60 steps per period (the Stage H2 T6 setting), Newton tolerance
  1e-10, started from the perturbative state. Settling before the three
  measurement periods: three e-folds of the cavity ringing 2/Gamma, at
  least 10 time units (27 at 100 K, 10 at 300 K): 4 periods at 0.1 THz
  (period 8.3), 10 at 0.3 THz (3.0), 28 at 1 THz (0.98), then 3 periods
  for the extrapolation. The run reports the Aitken tail and the dc drift
  between the last two periods.
- Large amplitude steepens plasma waves (shallow-water shocks). The solver
  has no shock capturing. Every point is checked by repeating the largest
  converged amplitude at 120 steps per period (ladder L3) and by the
  Newton convergence flag; a point where Newton fails or n_min < 0 ends the
  amplitude ladder at that frequency and is reported as the breakdown
  amplitude.
- Mesh: the production mesh of Stage H2 (hw 80, hm 480 nm for the 1 um
  device, 18601 nodes), where the second-order current is converged to 2 %.

Cost (measured, one period of 60 steps at 0.3 THz, Va 0.05, 42043 unknowns):
0.57 s per BDF2 step. One amplitude point is 7 periods = 4 min at 0.1 THz,
13 periods = 7.5 min at 0.3 THz and 31 periods = 18 min at 1 THz, times up
to 1.5 for the extra Newton iterations at large amplitude; at 300 K the
settling is shorter. The matrix of section 5: P1 about 4 h, P2 about 2.5 h,
P3 about 1 h, P4 about 2 h, the ladder (coarse mesh) about 20 min; 10
CPU-hours in all. Eigen's SparseLU is single threaded (250 MB per
process), so each (set, f) goes as its own background process, nine at
once on the 10-core machine, for a wall time set by the 1 THz runs at
100 K, about 2.5 h.

## 4. Acceptance ladder

- **L1 small-signal limit.** 1 um funnel, dss, 100 K, 0.3 THz, bias tee with
  R_L = R_dc, Va = 0.01: eta_av from the time domain within 2 % + 2 Va of
  eta_small Va^2 from the perturbation solver on the same mesh; V_dc and
  I_dc scale as Va^2 between Va = 0.01 and 0.02 to 3 %.
- **L2 bias-tee limits.** Same point with R_L = 0: I* within 2 % of
  I_sc Va^2; with R_L = 1e4 (open): V_rect within 2 % of V_oc Va^2; both
  from a deliberately wrong bias seed (half the prediction), so the
  Thevenin correction itself is tested.
- **L3 time-step convergence.** The largest amplitude that converges at
  0.3 THz, 100 K, repeated at 120 steps per period: eta_av within 3 %;
  n_min within 0.02.
- **L4 power bookkeeping.** At every point P_in >= P_dc and P_in <= P_av
  (passivity); the ratio P_in / P_av at Va = 0.01 within 2 % of the
  Stage H2 linear value.

## 5. Science runs

| run | device | set | term | f (THz) | Va ladder | R_L |
|---|---|---|---|---|---|---|
| P1 | 1 um funnel | 100 K | dss, bias tee | 0.1, 0.3, 1.0 | 0.01, 0.02, 0.05, 0.1, 0.2, 0.3, 0.5, 0.7 (stop at breakdown) | R_dc |
| P2 | 1 um funnel | 300 K | dss, bias tee | 0.1, 0.3, 1.0 | same | R_dc |
| P3 | 1 um funnel | 100 K | dss, antenna as load | 0.3 | same | R_ant |
| P4 | 1 um funnel | 100 K | sym (geometric only), bias tee | 0.3, 1.0 | 0.05, 0.1, 0.2, 0.3, 0.5 | R_dc |

P1 and P2 are the result. P3 says what a bias tee buys. P4 asks whether the
geometric (symmetric-termination) response saturates at the same amplitude
as the circuit (DS) response; if the geometric response holds up to larger
amplitude it would matter for the paper's redirect, and it is cheap because
only two frequencies and five amplitudes are needed to see the trend.

Each run writes one CSV row per (f, Va): Va, P_av (W), P_in (W), I_dc (A),
V_dc (V), P_dc (W), eta_av, eta_abs, eta_small, n_min, n_max, dc drift,
bias change, steps, periods, Newton ok, wall time.

## 6. Deliverables

- `apps/thz_power.cpp` (`gd_thz_power`): one run of the table above per
  invocation, CSV and stdout, perturbation asymptote computed alongside.
- `apps/thz_tests.cpp` gains L1 to L4 as tests T10 to T13 (slow; skipped by
  "quick"), and T14 repeats L1 for the sym termination, where both contacts
  carry a bias tee.
- `tools/thz_power_plot.py`: eta_av against P_av (log-log) with the
  small-signal line, one panel per temperature, one curve per frequency;
  n_min against P_av on a second panel.
- `results/THZ.md` gains a section "S5: conversion efficiency" with the
  table of peak efficiency, power at peak, saturation onset (P_av where
  eta_av falls 10 % below the quadratic law) and breakdown amplitude per
  (set, f), and the verdict: detector or harvester.

## 7. Out of scope

Lower carrier density and gate distance (the only lever that raises r),
contact resistance, the 175 nm funnel at its 1.87 THz geometric resonance
(66k nodes; four times the cost per step, and the 1 um device is the one an
antenna can drive), and any shock-capturing scheme: if the amplitude ladder
ends in a Newton failure before saturation is reached, that is reported as
the limit of the method and the saturation onset is given as a lower bound.
