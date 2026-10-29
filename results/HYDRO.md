# Stage H result: electron-hydrodynamic Tesla valve (2026-10-29)

Model: steady incompressible Navier-Stokes with Brinkman damping and Navier
slip on Taylor-Hood P2/P1 meshes (`src/hydro/`, plan in
`plans/hydro_tesla.md`). Current bias, developed inlet profile, do-nothing
outlet, `D = Pi_reverse / Pi_forward` (reverse = flow from contact 1 to 0,
the "hard" Tesla direction). Nondimensional groups: `Re = I/(n e nu)`
(width independent), `Da = W_ref^2/(nu tau_mr)` with `W_ref` the 2 um lead
width (4 um for the 1 um device), `S = l_s/W_ref`. All 175 nm devices have
6 um leads; `tesla1w1000` is `tesla1` scaled by 5.71 to a 1 um channel.
Logs: `hydro_S1_*_stdout.txt`, `hydro_S2_stdout.txt`, `hydro_tests_stdout.txt`;
CSV: `hydro_S1_*.csv`, `hydro_S2.csv`; fields: `hydro_*_Re10_Sinf_{f,r}.png`.

## Verdict

1. **At the Geurs operating point (Re = 0.05 to 0.3) the electron fluid gives
   |D - 1| < 3e-5**, for both the 175 nm and the 1 um single-loop Tesla, for
   every damping (Da 0.5 to 128) and every edge condition (S 0.1 to inf). The
   measured D = 1.05 is 2000 times larger. Steady incompressible electron
   Navier-Stokes does not reproduce the reported viscous rectification; it is
   not the inertial (Reynolds-number) Tesla mechanism.
2. **The Tesla term appears only with slip walls and only at Re >~ 3**:
   D - 1 = +4e-4 to +4e-3 at Re 3, +1e-2 to +6e-2 at Re 10, +4e-2 to +1.7e-1
   at Re 30 (best: `tesla3v2`, perfect slip). With no-slip walls every Tesla
   stays within 6e-4 of D = 1 up to Re 30, and on the wrong side.
3. **Re >~ 3 is not survivable in graphene.** Drift speed is
   `u = Re nu / w`; in a 175 nm channel Re = 1 already means 0.57 v_F at
   nu = 0.1 m^2/s (0.11 v_F at nu = 0.02); in a 1 um channel Re = 10 means
   v_F (0.2 v_F at nu = 0.02). Keeping u below ~0.3 v_F, where Fermi-liquid
   hydrodynamics still applies, caps Re at 0.5 to 2.6 (175 nm) or 3 to 15
   (1 um), i.e. |D - 1| <~ 3e-4 for the 175 nm Tesla and <~ 2e-2 for a 1 um
   Tesla with specular edges.
4. **The funnel beats the Tesla by an order of magnitude, again.** The
   converging/diverging nozzle (Nguyen Device D shape) is the strongest
   hydrodynamic diode in the set at every Re: |D - 1| = 1e-2 at Re 3 and
   0.15 at Re 10 with slip, 2.4e-3 and 6.7e-2 with no-slip walls. Its
   asymmetry is the Borda-Carnot one: the forward exit through the abrupt
   shoulder dissipates 1089 (nondimensional) in the downstream lead versus
   643 for the reverse exit through the 30 degree taper acting as a diffuser.
5. **For the rectenna goal the hydrodynamic nonlinearity is weak.** At
   190 uA (Re = 1.2 at n = 1e12, nu = 0.1) the funnel's hydrodynamic
   asymmetry is 1e-3, where the ballistic kill test gave 1.09 to 1.62 for the
   same shape. The steady electron-fluid inertial term is two orders weaker
   than the ballistic bias-window mechanism at the same current, and it is
   band-limited to ~10 to 100 GHz (viscous and convective times 10 to 20 ps
   in a 1 um channel). The THz question needs the unsteady gated-compressible
   (plasmonic) model, Stage H2, not more steady runs.

## S1: Geurs regime, D - 1 at Re 0.05 to 0.3

`tesla1` (175 nm channel, 2 um leads), Da = 0.5 / 2 / 8 (lead based), from
`hydro_S1_tesla1_stdout.txt`:

| S | Re 0.05 | Re 0.1 | Re 0.2 | Re 0.3 |
|---|---|---|---|---|
| 0.1 | -1.7e-6 | -3.4e-6 | -6.7e-6 | -1.0e-5 |
| 0.5 | -2.3e-6 | -4.6e-6 | -9.0e-6 | -1.3e-5 |
| 2 | -2.5e-6 | -5.0e-6 | -9.8e-6 | -1.4e-5 |
| inf | -2.6e-6 | -5.1e-6 | -1.0e-5 | -1.5e-5 |

(Da changes these by less than 10 %.) `tesla1w1000` (1 um channel, 4 um
leads), Da = 0.5 to 128 (= channel Gurzhi number 0.03 to 8):

| Da | S | Re 0.05 | Re 0.1 | Re 0.3 | Re 1 | Re 3 |
|---|---|---|---|---|---|---|
| 0.5 | 0.5 | -2.2e-6 | -4.3e-6 | -1.3e-5 | -3.1e-5 | +1.9e-4 |
| 0.5 | inf | -3.1e-6 | -6.1e-6 | -1.8e-5 | -2.8e-5 | +6.3e-4 |
| 8 | 0.5 | -1.8e-6 | -3.5e-6 | -1.0e-5 | -2.7e-5 | +1.0e-4 |
| 32 | inf | -1.4e-6 | -2.8e-6 | -8.4e-6 | -2.4e-5 | +2.9e-5 |
| 128 | 0.5 | -7.6e-7 | -1.5e-6 | -4.6e-6 | -1.5e-5 | -2.8e-5 |

The small-Re values are proportional to Re on a given mesh but their
continuum value is below the mesh floor (H7b: the Re = 1 value moves from
-2.5e-5 to +6e-6 over three refinements). They are reported as a bound,
|D - 1| < 3e-5 for Re <= 0.3, not as a resolved linear coefficient. Strong
Gurzhi damping (Da 128) delays the positive Tesla term beyond Re 3.

## S2: survivable-current map, D - 1 (`hydro_S2_stdout.txt`)

Da = 0 (Da = 1 and 4 differ by < 15 %). R is the forward resistance at
n = 1e12 cm^-2, nu = 0.1 m^2/s (`R = 1.82 Pi ohm` for the 2 um leads).

| device | S | R (kOhm) | Re 0.3 | Re 1 | Re 3 | Re 10 | Re 30 |
|---|---|---|---|---|---|---|---|
| tesla1 | 0 | 29 | +7e-7 | +2e-6 | -9e-7 | -1.9e-4 | -5.1e-4 |
| tesla1 | 0.5 | 5.0 | -1.3e-5 | -2.5e-5 | +4.0e-4 | +1.0e-2 | +4.1e-2 |
| tesla1 | inf | 4.2 | -1.5e-5 | -1.7e-5 | +7.0e-4 | +1.4e-2 | +5.8e-2 |
| tesla3 | 0 | 68 | +2e-6 | +6e-6 | +8e-6 | -2.0e-4 | -6.2e-4 |
| tesla3 | 0.5 | 9.2 | -2.1e-5 | -3.9e-5 | +6.4e-4 | +1.5e-2 | +2.4e-2 |
| tesla3 | inf | 7.3 | -2.5e-5 | -2.9e-5 | +1.2e-3 | +2.0e-2 | +4.5e-2 |
| tesla3v2 | 0 | 107 | +9e-7 | +3e-6 | +1e-6 | -2.9e-4 | -4.6e-3 |
| tesla3v2 | 0.5 | 9.1 | -2.1e-5 | -1.6e-5 | +1.2e-3 | +2.9e-2 | +1.1e-1 |
| tesla3v2 | inf | 6.1 | -2.8e-5 | +6.6e-5 | +4.0e-3 | +6.5e-2 | +1.7e-1 |
| funnel | 0 | 2.9 | +8e-6 | -5.8e-5 | -2.4e-3 | -6.7e-2 | -1.8e-1 |
| funnel | 0.5 | 1.8 | -1.4e-4 | -7.8e-4 | -1.0e-2 | -1.5e-1 | -1.1e-1 |
| funnel | inf | 1.7 | -1.5e-4 | -8.7e-4 | -1.2e-2 | -1.6e-1 | -1.1e-1 |

Currents: Re = 1 is 160 uA at nu = 0.1 m^2/s and 32 uA at 0.02 m^2/s
(n = 1e12). All 180 points converged (Newton residual < 1e-10).

Reading: a Tesla needs slip (specular, hBN-quality edges) to work at all,
and then gives about 1 % at Re 10 for one stage and 3 to 6 % for the sharper
three-stage v2 shape; the loops cost 2 to 5 times the funnel's resistance.
The funnel's asymmetry has the opposite sign (forward, into the taper, is the
hard direction) and is both larger and present with diffuse edges.

## S3: where the Re 10 asymmetry is generated (perfect slip, Da 0)

Viscous dissipation `int |grad u|^2` by region, `gd_hydro_fields`:

| device | direction | Pi | lead in | stage | lead out |
|---|---|---|---|---|---|
| tesla1 | forward | 2925 | 597 | 1417 | 906 |
| tesla1 | reverse | 2965 | 901 | 1465 | 595 |
| tesla3v2 | forward | 4225 | 598 | 2711 | 908 |
| tesla3v2 | reverse | 4502 | 902 | 2992 | 601 |
| funnel | forward | 1415 | 1 | 321 | 1089 |
| funnel | reverse | 1186 | 1 | 541 | 643 |

Energy balance `Pi = dissipation` holds to 0.1 to 0.3 % (the remainder is
the outlet kinetic-energy flux). In the Tesla the whole asymmetry is the
3 to 10 % extra dissipation inside the stages in reverse; the 900 versus 600
lead values are the jet from the narrow channel into the wide lead and are
symmetric between directions. In the funnel the asymmetry is the exit
geometry alone: abrupt shoulder (jet, 1089) versus 30 degree diffuser (643).
Fields: `hydro_tesla1_Re10_Sinf_{f,r}.png`, `hydro_tesla3v2_Re10_Sinf_{f,r}.png`,
`hydro_funnel_Re10_Sinf_{f,r}.png` (speed top, pressure bottom).

## Acceptance ladder (`hydro_tests_stdout.txt`)

| # | test | result |
|---|---|---|
| H1 | Poiseuille, no-slip | exact, 2e-13 |
| H1b | Navier slip S = 0.5, 2, inf | exact, 1e-12 |
| H2 | Brinkman-Poiseuille Da 10, S 0.5 | velocity O(h^2.7), pressure O(h^1.7), 1.2e-6 at h = 25 nm |
| H2b | Brinkman plug flow | exact, 4e-13 |
| H3 | Stokes reciprocity, tesla1, three (Da, S) | D - 1 = 2e-12, 1e-11, 2e-11; wall flux exactly zero |
| H4 | Kovasznay Re 40 | velocity O(h^2.9), 2.2e-5 at h = 25 nm |
| H5 | backward-facing step, ER 2 | x_r/h = 2.90 at Re 100 (lit. 2.85-3.05), 4.96 at Re 200 (lit. 4.75-5.10) |
| H6 | classical Tesla trend, Da 0, no-slip | D = 0.9993, 1.0037, 1.0197 at Re 25, 50, 100 (monotonic, literature 1.01-1.05) |
| H7 | mesh convergence, tesla1, Re 10, Da 1, S 0.5 | D - 1 = 1.01e-2, 1.10e-2, 1.14e-2 (hw 20, 14, 10 nm): 3 % |
| H7b | same at Re 1 | -2.5e-5, -2.2e-5, +6e-6: unresolved, bound 1e-4 |
| H8 | symmetric constriction, Re 10 | D - 1 = 7e-6 (mesh asymmetry floor) |
| H9a | tesla1 Re 0.05-0.2 | (D-1)/Re constant to 2 % on one mesh |
| H9b | mirrored and re-meshed tesla1 | directional solver bias 6e-7 in D |
| U1 | units | Poiseuille R = Gurzhi formula, Brinkman R = Drude, both to 1e-12 |

Deviations from the plan: no fully specified 2D Tesla geometry with
tabulated D(Re) exists in the literature (the candidates are 3D, figure-only,
or paywalled), so H6 is a trend-and-magnitude check and the quantitative
separated-flow benchmark is the backward-facing step. Steady Newton with Re
continuation fails for the classical valve between Re 100 and 150 (probably
loss of a stable steady state); irrelevant for graphene.

## Numerical notes

* Length-weighted (Engelman-Sani-Gresho) vertex normals make the discrete
  wall flux vanish node by node; without them the polygonised arcs leaked
  7e-6 of the flux and produced a fake D - 1 of 1e-5 in Stokes flow.
* Pressure drops are normalised by the discrete inlet flux; otherwise the
  P2 interpolation of steep Brinkman profiles biases D at 1e-6.
* `Pi` converges only first order (re-entrant corners at the stage
  junctions: 2706, 2754, 2798, 2818 over four refinements), `D` at Re >= 10
  converges to 3 %, at Re 3 to ~15 %, and below Re 1 the asymmetry is
  under the discretisation floor of ~3e-5.
* Costs: 23k to 55k triangles per 175 nm device, 110k to 500k unknowns,
  3 to 60 s per Newton step with Eigen SparseLU, single-threaded; the 180
  point S2 sweep took about 3 h on 3 threads.

## Limitations carried from the plan

Incompressible (no gate-capacitance density response), isothermal (no Joule
heating of T_e), Navier-Stokes only (no Knudsen corrections when
l_ee ~ W), Fermi-liquid only, B = 0, steady. The first two are the
candidates for the Geurs signal that this model excludes; the last is the
THz question.
