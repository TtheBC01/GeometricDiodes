# Plan: electron-hydrodynamic Tesla valve (Stage H)

Written 2026-10-29, before any code. Companion to `results/KILLTEST.md`, which
closed the ballistic question. This stage asks the one question the ballistic
model cannot: **does a Tesla-shaped graphene channel rectify in the
electron-hydrodynamic window (20 to 100 K, hBN-encapsulated), and by how much
at currents a real device survives?**

## 0. The number that frames everything

For a 2D electron fluid driven by current I at density n, the Reynolds number
is independent of channel width:

    u = I / (n e W),   Re = u W / nu = I / (n e nu)

| case | I | n (cm^-2) | nu (m^2/s) | Re |
|---|---|---|---|---|
| Geurs 2020, device 1 (D = 1.05 measured) | 6.3 uA | 8.3e11 | 0.02 to 0.1 | **0.05 to 0.24** |
| our 1 um channel, comfortable | 100 uA | 1e12 | 0.02 to 0.1 | 0.6 to 3 |
| hard ceiling, u = v_F, W = 1 um | 1.6 mA | 1e12 | 0.02 to 0.1 | 10 to 50 |
| Sarypov 2026 GaAs (D up to 40 claimed) | 300 uA | 6.8e11 | ~0.04 (their Re >~ 7) | ~7 |

Classical Tesla valves (Nguyen, Abouezzi, Ristroph 2021) have D = 1 to within
a few percent for Re < 100 and turn on near Re = 200. Stokes flow (Re = 0) is
kinematically reversible, so D = 1 exactly, and the first correction is
linear in Re: V(I) = R I + beta I^2 with D - 1 = 2 beta I / R + O(I^2). The
Geurs measurement therefore sits two to three decades below the classical
turn-on, and the first quantitative result of this stage will be either

- (a) electron Navier-Stokes gives D - 1 ~ 1e-3 or less at Re = 0.1, in which
  case the measured 5 % is not inertial (gate-density background, thermal, or
  contact effects), and the Tesla-hydro route gives no useful diode at
  survivable currents; or
- (b) the electron fluid (Gurzhi damping, slip, 2D confinement) produces a
  much larger low-Re coefficient than water does, which would be new and
  worth a paper.

Either way the answer is cheap to get once the solver exists, and the solver
is the same one a THz plasmonic rectifier design needs (section 6).

## 1. Model

Fermi-liquid regime (E_F >> k_B T, so single-component fluid, no Dirac-fluid
enthalpy corrections), steady, isothermal, gate-screened so n = const
(incompressible). Per unit area, with m* = hbar k_F / v_F:

    m* n (u . grad) u = - grad p + m* n nu lap u - m* n u / tau_mr
    div u = 0
    p = e n phi        (electrochemical potential; V = delta p / (e n))

Walls: impermeable, Navier slip with slip length l_s:

    u . n = 0,    t . sigma n = - (m* n nu / l_s) (u . t)

l_s -> 0 is no-slip, l_s -> inf is perfect slip (specular edges). Kiselev and
Schmalian give l_s ~ 0.6 to 1.2 l_ee for diffuse edges, l_s >> l_ee for
specular ones.

Contacts: current bias, as in the experiments. Inflow lead gets the analytic
fully developed Brinkman-Poiseuille profile with slip (closed form, also test
H2); outflow lead gets the do-nothing traction condition. Both leads are long
enough (>= 3 W) that the probes sit in developed flow. Forward and reverse
are two separate solves with inlet and outlet swapped.

Nondimensionalisation (lengths by the channel width W, velocity by the mean
inlet speed U, pressure by m* n nu U / W):

    Re (u . grad) u = - grad p + lap u - Da u,   Da = W^2 / (nu tau_mr)
    slip number  S = l_s / W

Three dimensionless groups plus geometry. The physical layer converts
(n, T, mobility, edge quality, I) to (Re, Da, S) and the results back to
(V, R, D, beta, gamma).

Physical inputs for the hBN window, with sources in the research notes:
nu = 0.02 to 0.3 m^2/s (Bandurin 2016 ~0.1 at 100 to 200 K; nu = v_F l_ee/4);
l_ee = 0.1 to 1 um for 50 to 100 K at 1e12; tau_mr = l_mr / v_F = 2 to 20 ps
(l_mr 2 to 20 um); Gurzhi length sqrt(nu tau_mr) = 0.2 to 2.5 um, so Da = 0.2
to 25 at W = 1 um; m* = 0.0205 m_e at 1e12.

Outputs per geometry and (Re, Da, S): R_f, R_b (total, four-terminal
probe planes in the leads), D = R_b / R_f (reverse = hard direction), the
excess resistance of the valve after subtracting the analytic lead drop,
beta from a fit over the Re sweep, gamma = I''/I' at zero bias
(= -2 beta / R^2), and VTK fields (u, p, vorticity) for ParaView.

Deliberately excluded at this stage, each a stated limitation:
compressibility under the gate (delta n / n ~ e delta phi / E_F, a few
percent at the currents above), Joule heating of the electron temperature
(changes nu and l_ee along the channel), Knudsen corrections when l_ee ~ W,
the Dirac-fluid regime near neutrality, Hall viscosity (B = 0 here), and
time dependence (section 6).

## 2. Solver

**Discretisation.** Taylor-Hood P2/P1 triangles, steady Navier-Stokes with
the Brinkman term, Newton with the analytic Jacobian, continuation in Re
from the Stokes solution. Inf-sup stable, second-order in h for velocity
gradients, and P2 reproduces the Poiseuille parabola exactly, which makes
test H1 a machine-precision check of assembly and boundary conditions.
Expected mesh sizes 30k to 150k triangles (P2 node count 60k to 300k,
~0.5 M unknowns at the top end). No stabilisation is needed at Re <= 50 on
these meshes; if it is, that is a finding, not a tuning knob.

**Wall conditions.** u . n = 0 imposed weakly by Nitsche's method (symmetric,
penalty ~ 10 nu / h), with the Navier slip term as a natural boundary
integral. Nitsche avoids the corner-normal ambiguity of a polygonal boundary
and lets S run from 0 to infinity in one code path. Strong no-slip is kept
as a second path for S = 0 so the two can be cross-checked (test H1b).

**Mesh.** The existing C++ generators remain the geometry source of truth.
A small exporter writes each `Boundary` as loop polygons with contact tags
(`gd_export`), and `tools/mesh.py` turns that into a gmsh linear
triangulation with a wall-distance size field (fine at walls and necks,
coarse in lead interiors) and physical tags for walls, inlet, outlet. gmsh
4.15 installs from pip (verified today); no brew/cmake needed. Midside nodes
for P2 are created in C++ from the edge map, so the mesh file stays P1 and
simple. The 2 deg arc polygonisation of the generators is the geometry, as
it was in the kill test, so no curved elements.

**Linear algebra.** Eigen (header-only, brew) SparseLU with COLAMD for the
Newton systems. At 0.5 M unknowns in 2D the factor fits in a few GB and
takes tens of seconds; adequate for a parameter sweep of a few hundred
solves on 10 cores run as independent processes. If it becomes the
bottleneck, Apple Accelerate's multithreaded sparse LU is a drop-in on this
machine. No iterative solver, no preconditioner work at this stage.

**Postprocessing.** Cross-section averages of p at probe planes, wall shear
and dissipation integrals (to see where the asymmetry is generated,
bifurcation versus recombination junction), VTK legacy output.

**Code layout** (new, beside the ballistic code, sharing `geometry` and
`generators`):

    src/hydro/mesh.hpp/.cpp      gmsh reader, P2 edge map, boundary tags
    src/hydro/fem.hpp/.cpp       P2/P1 shape functions, quadrature, assembly
    src/hydro/ns_solver.hpp/.cpp Newton, continuation, Nitsche/slip BCs
    src/hydro/post.hpp/.cpp      probes, integrals, VTK
    src/hydro/units.hpp          (n, T, mu, l_s, I) <-> (Re, Da, S), V, R
    apps/gd_export.cpp           Boundary -> polygon file
    apps/hydro_tests.cpp         ladder below
    apps/hydro_sweep.cpp         parameter sweep, CSV
    tools/mesh.py, requirements.txt

Estimated size ~2000 lines of C++ and 150 of Python.

## 3. Acceptance ladder (all must pass before any Tesla result is quoted)

| # | test | pass criterion |
|---|---|---|
| H1 | Poiseuille in a straight channel, no-slip, Re = 0 | profile and dp/dx exact to 1e-10 (P2 is exact) |
| H1b | same with Navier slip S = 0.5, 2 via Nitsche; S = 0 via Nitsche vs strong | shifted parabola exact; Nitsche vs strong agree to 1e-8 |
| H2 | Brinkman-Poiseuille, Da = 1, 10, with slip (cosh profile, closed form) | L2 velocity error converges O(h^3), pressure O(h^2) |
| H3 | **Stokes reciprocity** in tesla_1stage and tesla_3stage, Re = 0, Da and S varied | D = 1 to 1e-8 (hydro analogue of ballistic test T3) |
| H4 | Kovasznay flow, Re = 40, exact Navier-Stokes solution | O(h^3) velocity convergence, Newton quadratic, residual < 1e-10 |
| H5 | Backward-facing step, expansion ratio 2, Re = 100 | reattachment length within 3 % of Erturk 2008 (x_r/h ~ 2.9; pin the exact value from the paper when the test is written) |
| H6 | Classical Tesla valve D(Re) for a published geometry (Thompson 2014 T45-R or Nguyen 2021) at Re = 25, 50, 100, 200 | within 10 % of the published D - 1 (the convective term produces Tesla diodicity at all) |
| H7 | Mesh convergence of D - 1 at the target point (Re = 1, Da = 1, S = 0.5), three refinements | D - 1 changes < 10 % between the last two |
| H8 | Symmetry control: the symmetric constriction and the straight channel at Re = 10 | D = 1 to 1e-6 |
| H9 | Linear-response check: D - 1 is linear in Re for Re < 0.3 with the sign set by geometry, and flips when the geometry is mirrored | slope agrees between +Re and mirrored -Re to 1 % |

H3, H8 and H9 are the ones that catch a wrong boundary condition or a
mis-assembled convective term producing fake diodicity; they come before any
Tesla number is believed, exactly as T3 to T6 did for the ballistic code.

## 4. Science runs, in order

S1. **Geurs replication at their Re.** tesla_1stage scaled to W = 1 um, Re in
{0.05, 0.1, 0.2, 0.3}, Da in {0.5, 2, 8}, S in {0.1, 0.5, 2, inf}. Deliverable:
D - 1 at the measured operating point. Decides (a) versus (b) of section 0.

S2. **Survivable-current map.** Re in {0.3, 1, 3, 10, 30} for tesla_1stage,
tesla_3stage, tesla_3stage_v2 and the Nguyen funnel (a converging/diverging
nozzle is itself a hydrodynamic valve, so the apples-to-apples comparison of
the kill test is repeated here). Deliverable: D(Re, Da, S) tables and the
beta coefficient; R_f, R_b in ohms at n = 1e12, T = 60 K for a 1 um channel.

S3. **Where the asymmetry comes from.** Dissipation split by region
(bifurcation, loop, recombination) for the best case, to see whether a
different loop geometry could help before any optimisation is considered.

S4 (only if S2 shows D - 1 > 5 % somewhere survivable). Shape variations of
entry angle, loop radius, re-entry angle at fixed neck, a dozen cases, not an
optimiser.

Verdict file `results/HYDRO.md` in the same form as `KILLTEST.md`.

## 5. Effort and risks

Build and ladder: roughly three working sessions. S1 and S2: one session of
compute (a few hundred steady solves at tens of seconds each, 10 cores).

Risks: (i) Newton stalls at Re ~ 30 with recirculation in three-stage
devices; mitigated by Re continuation and, if needed, pseudo-transient
continuation, both standard. (ii) D - 1 at Re = 0.1 is ~1e-4 and sits at the
discretisation error; mitigated by H7 and by extracting beta from the slope
over the Re sweep rather than from a single point. (iii) The polygonised 2 deg
arcs create artificial corners that dominate separation at high Re; checked
by re-meshing with 0.5 deg arcs in H7.

## 6. Relation to the THz rectenna goal

The steady solver answers the low-frequency question. The time scales say
how low: viscous equilibration W^2/nu = 10 ps at W = 1 um and nu = 0.1 m^2/s
(0.3 ps at a 175 nm neck), convective transit W/u = 20 ps at 100 uA, momentum
relaxation 2 to 20 ps. A steady-state Tesla or funnel diodicity therefore
holds up to roughly 10 to 100 GHz and says nothing by itself about 1 THz,
where the period is shorter than every one of those times and the gate
capacitance makes the electron fluid compressible (plasmon speed several
v_F, transit across 1 um ~ 0.3 ps). In that regime rectification is the
Dyakonov-Shur plasma-wave mechanism: the same continuity and momentum
equations, with n variable and p set by the gate capacitance, driven at
omega with asymmetric boundary conditions, which is how graphene THz
detectors already work (Vicarelli 2012, Bandurin 2018).

Stage H2, after this one, is therefore the unsteady gated-compressible
version of the same code: same mesh, elements, assembly and BC machinery,
plus a time integrator (BDF2) and the continuity equation with
dn/dt. It yields rectified DC voltage versus frequency and drive amplitude
for arbitrary 2D shapes, Tesla-like or not, which is the design tool the
rectenna goal needs. Building Stage H as planned, with the physics layer and
BCs separated from the discretisation, is the direct route to it.
