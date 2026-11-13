# Stage H5 results: steady flow with the gate, shape versus circuit at dc (2026-11-11)

Plan: `plans/hydro_dcgate.md` (figure `plans/hydro_dcgate.png`). Code:
`apps/dc_gate.cpp` (`gd_dc_gate`), `apps/dc_gate_ref.cpp`,
`apps/dc_gate_common.hpp`, ladder T18 to T20 in `apps/thz_tests.cpp`. Runs:
`tools/dc_gate_runs.sh`, `tools/dc_gate_ref.py`; results in
`results/dcgate/` (`D1.csv` to `D4.csv`, `ref.csv`, `runs.log`), tables
`results/dcgate/tables_H5.md`, figure `H5_shape_vs_circuit.png`.

The question: the steady-flow stages (`HYDRO.md`, `CLASSIC.md`) had no
gate. When the carrier density is allowed to respond to the gate, does the
shape of a funnel or a Tesla valve rectify a steady current, or is the
asymmetry that appears the circuit asymmetry of any gated channel?

## Verdict

1. **Shape adds nothing with the gate.** Reversing a device by swapping
   the driven contact, which removes the circuit term, leaves an
   asymmetry of at most 4e-4 in any device inside the drift-speed limit
   (0.3 v_F). Where that asymmetry is above the discretisation floor it
   is the inertial term of Stage H, unchanged: the gated and the
   incompressible solver agree within a factor two at 21 of the 23 points
   at which the incompressible |D - 1| is above 1e-4.
2. **There is no first-order shape term.** At low bias the swap asymmetry
   on the production meshes is linear in the bias, which the
   one-dimensional theorem forbids. It is numerical: for the funnel its
   coefficient falls from 9.7e-3 to 3.5e-3, 1.4e-3 and 2e-4 as the wall
   mesh goes from 28 to 20, 14 and 10 nm, while the quadratic
   coefficient stays at -0.05 (`H5_shape_vs_circuit.png` c). For the
   Tesla valve the whole low-bias term falls from 2e-6 to 1.5e-7.
3. **The circuit term is the same for every shape.** Flipping the
   polarity with the gate fixed gives D - 1 = 0.48 V / U_0 at small bias:
   1.45 % at 43 mV for the straight channel, the symmetric constriction,
   both funnels and both Tesla valves, equal to the one-dimensional
   closed form within 2 % inside the drift limit (4 % for the 1 um Tesla
   valve at its largest bias, where inertia enters).
4. **The published gated simulation is not reproduced as a shape
   effect.** At 50 mV the swap asymmetry of the Tesla valve is below
   1e-5, against the 6.5 % of Gungor et al.
5. **Against the plan's criteria:** the first outcome, "shape adds
   nothing". The second criterion (a swap asymmetry above 1 % inside the
   drift limit) is not met anywhere; the largest value is 0.04 %.

**For the paper.** The dc section gains one paragraph and one figure: a
gated channel of any shape shows a percent-level asymmetry under a
polarity flip, set by the bias over the gate overdrive, and none under a
contact swap. This is the steady counterpart of the THz result. It is not
offered as an explanation of the measurement of Geurs et al., which used
four-terminal resistances and reports a signal that disappears outside
the viscous regime.

## Acceptance ladder (`thz/ladder_H5.txt`; 3/3 pass, 150 s)

| test | result |
|---|---|
| T18 uniform gated channel, steady V(I) against the 1-D closed form with inertia, both closures, both signs, to V = 0.3 U_0 | worst relative error 1.1e-4 |
| T19 symmetric constriction at V = 0.1 U_0: swap against flip | swap 3.8e-6, flip 4.96e-2 (ratio 7.6e-5) |
| T20 incompressible limit: funnel, slip walls, Re 10, Mach 1e-3, against Stage H on the same mesh | D = 0.83845 against 0.83843 |

## Deviations from the plan

1. **The coefficient is 0.48, not 1.** The plan's closed form used the
   linear-capacitance closure, for which D_flip - 1 = V / U_0. With the
   graphene closure of the production runs the effective mass grows with
   density and the coefficient is 0.48. T18 tests both closures; the
   production comparison is with the closed form of the closure used. The
   plan's "0.7 to 7 % for 10 to 100 mV" becomes 0.34 to 3.4 %.
2. **T19 cannot be 1e-8.** The mesh of the symmetric constriction is
   unstructured and not mirror symmetric, so its swap asymmetry is the
   mesh asymmetry: 4e-6 at 0.1 U_0. The criterion became "below 1e-3 of
   the flip asymmetry"; the measured ratio is 7.6e-5. This run is the
   discretisation floor of panel (a) of the figure.
3. **"Within a factor two of the incompressible value" holds only above
   the floor.** Below about 1e-4 both solvers return their own
   discretisation error, which can differ in sign (the 175 nm funnel at
   100 K). The comparison is stated for points above 1e-4.
4. **Additions:** the 1 um copy of the Tesla valve in D3; a 10 nm funnel
   mesh in D4, added when the three planned meshes showed the low-bias
   term still falling; the incompressible reference for every row
   (`gd_dc_gate_ref`), needed to test the criterion point by point.
5. One point did not converge: the straight channel at 100 K and
   V = 0.3 U_0, where the drift speed would be 6.7 v_F. It is far outside
   the model's range and is marked in the tables.

## Runs

Gate 30 nm of hBN, n = 1e12 cm^-2 (U_0 = 1.43 V, wave speed 3.57 v_F),
parameter sets of `THZ.md` (300 K: nu 0.03 m^2/s, tau 1 ps; 100 K: 0.1,
5 ps), graphene closure. Bias V / U_0 = 0.001 to 0.3 (1.4 to 430 mV), four
steady solves per point.

| run | devices | conditions | points |
|---|---|---|---|
| D1 | straight channel, symmetric constriction | both sets, slip | 28 |
| D2 | funnel (175 nm neck), funnel (1 um neck) | both sets, slip and no-slip | 56 |
| D3 | classical Tesla valve, 175 nm and 1 um | both sets, slip | 28 |
| D4 | mesh convergence: funnel at 28, 20, 14, 10 nm; Tesla at 20, 14, 10 nm; symmetric constriction at 20, 14 nm | 300 K, slip | 16 |

112 production points, 78 of them inside the drift limit. Wall time 29
minutes, single threaded, plus the reference runs.

## Results

1. **Flip (`H5_shape_vs_circuit.png` a).** At small bias D_flip - 1 =
   0.4805 V / U_0 for the straight channel, and the same to four digits
   for every other device. Largest departure from the one-dimensional
   closed form inside the drift limit: 1.6 % (straight channel, 100 K,
   inertia, reproduced by the closed form with inertia in T18), 0.8 to
   0.9 % for the funnels, 1.9 and 3.9 % for the Tesla valves at their
   largest bias.
2. **Swap.** Largest values inside the drift limit, either gate
   reference:

   | device | largest \|D_swap - 1\| | where | share of the flip term |
   |---|---|---|---|
   | straight channel | 2e-12 | | 0 |
   | symmetric constriction | 3e-6 | mesh floor | 0.02 % |
   | funnel, 175 nm neck | 1.6e-4 | 43 mV, 300 K, slip | 1.1 % |
   | funnel, 1 um neck | 2.4e-4 | 43 mV, 100 K, slip | 1.6 % |
   | Tesla valve, 175 nm | 1.2e-4 | 274 mV, 300 K | 0.1 % |
   | Tesla valve, 1 um | 4.1e-4 | 275 mV, 100 K | 0.4 % |

   The 175 nm funnel figure is on the 20 nm mesh and is mostly the
   numerical linear term; on the 10 nm mesh it is 5.4e-5, of which the
   inertial part is 4.8e-5.
3. **Gated against incompressible (`H5_shape_vs_circuit.png` b).** Over
   five decades, from 1e-4 to 3e-2 (the upper end outside the drift
   limit), the swap asymmetry of the gated solver follows the Stage H
   value at the same Reynolds number, damping, wall condition and mesh.
   The two exceptions above 1e-4 are the 1 um funnel at 0.3 U_0 (2.4
   times, outside the drift limit) and the 175 nm funnel at 100 K and
   0.1 U_0 (opposite sign, at the floor of that mesh).
4. **Mesh convergence (`H5_shape_vs_circuit.png` c).** The flip term of
   the funnel converges to the closed form: 1.4246, 1.4434, 1.4498 and
   1.4535e-2 on the four meshes against 1.4521e-2. The swap term does
   not converge to a non-zero value at low bias (verdict 2).

## Limitations

Local-capacitance electrostatics; no electron heating; steady states
only; monolayer graphene, where the flow cannot approach the wave speed.
The GaAs result of Sarypov et al. (diodicity 40 above a threshold
current) is outside this model. The low-bias swap asymmetry is bounded,
not resolved: on the finest meshes it is 7e-6 at 14 mV for the funnel and
1.5e-7 for the Tesla valve, and still falling.
