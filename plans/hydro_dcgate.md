# Plan: steady flow with the gate, shape versus circuit at dc (Stage H5)

Written 2026-11-10, before any code or run. Awaiting approval. Companion to
`results/HYDRO.md` and `results/CLASSIC.md` (steady incompressible flow),
`results/THZ.md` (the gated-compressible solver) and `paper/prior_work.md`,
section 1.4. Figure: `plans/hydro_dcgate.png` (script
`tools/dcgate_plan_fig.py`).

The steady-flow stages used an incompressible fluid: the gate was absent
and the carrier density could not respond to the bias. Their limitation
list named this as a candidate for the 5 % asymmetry measured by Geurs et
al., which our model misses by three orders of magnitude. The prior-work
search then found a simulation that includes the gate (Gungor et al.,
Materials 15, 4141 (2022)) and reports a Tesla-valve diodicity of 1.0655,
"qualitatively" matching that experiment. Stage H5 asks: **when the
density is allowed to respond to the gate, does the shape of a funnel or a
Tesla valve rectify a steady current, or is the asymmetry that appears the
circuit asymmetry of any gated channel?**

## 0. What is already known

**The published simulation, read in full.** Channel width "in the order of
10 nm", 300 K, Fermi level 100 meV, momentum relaxation 0.27 ps, bias
50 mV applied for 1 ps in each direction, time-domain discontinuous
Galerkin with a local Lax-Friedrichs flux, no viscosity value, no Reynolds
number, no mesh study, comparison with experiment stated as qualitative.
A 10 nm channel is a tenth of the electron-electron length, so a fluid
description does not apply to it, and 1 ps is shorter than the transit.
As I read it, it reverses the device by swapping the driven contact,
which is the shape-only protocol defined below. It is not a result we must reproduce,
but a referee will ask what our solver gives with the gate switched on.

**A one-dimensional theorem.** In a gated channel that is long compared
with its width, the momentum balance is Gamma j / W(x) = -n h'(n) dn/dx,
with j the current, W the local width and h the electrochemical potential.
Integrating along the channel gives F(n_source) - F(n_drain) = Gamma j
times the integral of dx / W: the relation between current and voltage
depends on the shape only through the total small-signal resistance R.
For the linear-capacitance closure with the gate tied to the drain,

    V(I) = sqrt(1 + 2 R I) - 1  (units of U_0),   so   V(+I) / |V(-I)| differs from 1 by V / U_0 to leading order

for every shape, and V(I) is the same whichever end is driven. In one
dimension, then: reversing the polarity with the gate fixed gives an
asymmetry V / U_0 with no shape at all (the gate overdrive U_0 is 1.43 V
at 1e12 cm^-2 under 30 nm of hBN, so 10 to 100 mV gives 0.7 to 7 %), and
swapping the contacts gives exactly none. Whatever the shape contributes
is a two-dimensional correction: inertia, viscosity, the lateral density
profile.

**One existing data point.** Ladder test T5 of the gated solver computed
the steady V(I) of the funnel with the gate tied to the drain:
R = 11.391, curvature b = -65.2 (extrapolated to zero current). The
theorem gives b = -R^2 / 2 = -64.9. The two agree to 0.5 %: in that run
the gated funnel's whole steady nonlinearity is the circuit term, and the
shape contributes at most half a per cent of it. So the answer is largely
known, and this stage is confirmatory. Its job is to turn one test into a
result with controls: both protocols, all shapes, finite amplitude, mesh
convergence.

## 1. Model and code

The existing solver, `ThzTimeSolver::solve_steady` (`src/hydro/thz_time.*`):
steady two-dimensional flow with convection, viscosity, momentum
relaxation and a local gate capacitance, graphene closure, current or
voltage ports. It has been tested against the incompressible solver (T1)
and the perturbation solver (T5, T6). No change to the solver. New:

- `apps/dc_gate.cpp` (`gd_dc_gate`): for a mesh, a parameter set and a
  list of currents, the four steady solves (drive either contact, either
  sign), written as one CSV row with both asymmetries.
- three tests in `apps/thz_tests.cpp`:

| test | what it checks | criterion |
|---|---|---|
| T18 | straight channel, gate tied to drain: V(I) against the closed form, both closures, up to V = 0.3 U_0 | 1 % |
| T19 | mirror-symmetric constriction: swap-contacts asymmetry is zero | below 1e-8 |
| T20 | incompressible limit: funnel, Re 10, slip walls, Mach number to zero reproduces the Stage H value D - 1 = 0.15 | 2 % of D |

## 2. The two protocols (figure panel a)

- **Swap the contacts.** Drive the left contact with the right one and the
  gate grounded, then the right contact with the left one and the gate
  grounded. The two states are mirror images in everything except the
  shape. D_swap - 1 is the shape term.
- **Flip the polarity.** Same contacts, opposite sign of current, gate
  fixed. This is what a two-terminal measurement with a grounded gate
  does. D_flip - 1 is the circuit term plus the shape term.

## 3. Runs

| run | devices | parameters | amplitudes | solves |
|---|---|---|---|---|
| D0 | timing probe: one solve per mesh | 300 K set | one | 4 |
| D1 | straight channel, symmetric constriction (controls) | 100 K and 300 K sets of `THZ.md` | V / U_0 = 0.003 to 0.3, six values | 96 |
| D2 | funnel (Device D), 1 um funnel | same; no-slip and slip walls | same | 192 |
| D3 | classical Tesla valve (four loops) | same; slip walls | same, stopping at the drift limit | 48 |
| D4 | mesh convergence of the headline points (funnel and Tesla at V / U_0 = 0.03) | two finer meshes | one | 16 |

Each amplitude is also expressed as a current, a peak drift speed and a
Reynolds number, and rows beyond a drift speed of 0.3 v_F are marked as
outside the fluid model's range, as in `HYDRO.md`.

Cost: about 350 steady solves. I do not have a reliable time per solve on
the large Tesla mesh; D0 measures it first. If the whole stage would
exceed four CPU-hours I stop after D0 and come back with the number.
Expected: one to three CPU-hours, under half an hour of wall time.

## 4. Decision criteria, fixed now

- **Shape adds nothing (expected):** D_swap - 1 stays within a factor two
  of the incompressible value of `HYDRO.md` and `CLASSIC.md` at every
  amplitude inside the drift limit, and D_flip - 1 equals V / U_0 to 10 %
  for every device including the straight channel. The paper's dc section
  gains one paragraph and one figure: the dc counterpart of the THz
  result, the 6.5 % of Gungor et al. is not reproduced as a shape effect,
  and a percent-level asymmetry is shown to arise in a gated two-terminal
  measurement from the circuit alone.
- **Shape matters with the gate:** D_swap - 1 above 1 % inside the drift
  limit for the funnel or the Tesla valve. That would be new, would
  contradict test T5, and would reopen the dc verdict and the large-signal
  THz stage. I would stop and report before doing anything else.

What this stage will not claim: that the circuit term explains the Geurs
measurement. That experiment used four-terminal resistances and reports
that the signal vanishes outside the viscous regime, which a circuit term
would not do. The paper can say only that the term exists, how large it
is, and that a shape-only protocol removes it.

## 5. Limits and out of scope

Local-capacitance electrostatics (as in `THZ.md`); no electron heating;
steady states only, so no instability or turbulence; monolayer graphene,
where the wave speed is about 3.6 v_F and the flow cannot approach it, so
the choked-nozzle regime of the bilayer experiment (Geurs et al. 2025) is
out of reach by construction. The GaAs result of Sarypov et al.
(diodicity 40 above a threshold current, attributed to turbulence, at a
drift speed near the Fermi velocity) is outside this model and stays a
matter for the paper's discussion, not for a run.

## 6. Deliverables

`apps/dc_gate.cpp`, tests T18 to T20, `tools/dc_gate_runs.sh`,
`tools/dc_gate_plot.py`, `results/HYDRO_GATE.md` with the ladder, the runs
and the verdict against section 4; a pointer from `HYDRO.md` (limitations)
and `CLASSIC.md`. For the paper: one paragraph and one figure in the dc
section, the theorem of section 0 in the supplement.
