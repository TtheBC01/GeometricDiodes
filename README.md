# GeometricDiodes

Event-driven semiclassical electron transport in two-dimensional graphene
devices, built to answer one question first: **does a Tesla-valve-inspired
channel rectify better than a funnel under the same physics?** (the "kill
test"), answered in `results/KILLTEST.md`. Stage H, the electron-hydrodynamic
solver (`src/hydro/`, `plans/hydro_tesla.md`, `results/HYDRO.md`), asks the
follow-up question the ballistic model cannot: does the Tesla shape rectify in
the viscous electron-fluid window (20 to 100 K), and at what current.

## Physics model (Stage 0 + Stage 1)

* Massless Dirac electrons, `E = hbar v_F |k|`, `v_F = 1e6 m/s`. Electrons only.
* **Contacts** are ideal reservoirs at `mu_S = E_F` and `mu_D = E_F - eV`.
  Carriers are injected with the flux-weighted (cosine) angular law and with
  total energies drawn from `N(E) |f_S(E) - f_D(E)|`, where `N(E) = E W/(pi hbar v_F)`
  is the mode number of the injecting contact. The current is the Landauer
  bias-window estimator `I = g (e^2/h) sign(V) Z <T>`, `g = 4`, with
  `Z = int N(E) |f_S - f_D| dE`. This gives exactly zero current at zero bias.
* **In-device potential**: Stage 1 uses the same assumption as the published
  geometric-diode Monte Carlo papers, a uniform in-plane field
  `F = (eV/L, 0)` over the whole device, i.e. `U(x) = -eV x/L`. Flights in the
  field are chord-stepped with a sagitta tolerance (default 0.05 nm) and the
  kinetic energy is re-imposed from energy conservation at every step.
  Field-free flights are analytic (one ray/segment query per flight).
* **Walls**: specular reflection with probability `p`, otherwise diffuse
  (cosine-law) re-emission about the inward normal. `p` is a free parameter
  because no measured value exists for etched graphene edges (inferred range
  0 to 0.5).
* **Bulk scattering**: isotropic momentum relaxation after an exponentially
  distributed free path `lambda_mfp` (elastic; `|k|` preserved).
* **Fates**: `source`, `drain`, `trapped` (residence time cutoff),
  `dirac` (kinetic energy fell below 2 meV; a classical stand-in for the
  Klein-tunnelling region), `leaked` (numerical escape; must be zero).

Sign convention: `V > 0` means the drain electrostatic potential is higher
than the source, so electrons are accelerated from source toward drain.
Positive `I` is net electron flow source to drain. The asymmetry ratio is
`A(V) = I(+V) / |I(-V)|`. Hole transport in the same device is obtained by
`V -> -V` (`A_h = 1/A_e`), which is the polarity reversal seen in experiments.

Why the field-free engine alone cannot rectify: for any fixed potential
landscape, flux conservation gives `N_S(E) T_SD(E) = N_D(E) T_DS(E)`, so with
no in-device potential `I(V)` is odd in `V` for every shape (test T3). Only a
bias-dependent potential (here the imposed uniform field) breaks the symmetry
between `+V` and `-V` landscapes (test T4 checks reciprocity still holds within
each landscape).

## Layout

```
src/core.hpp        units, Vec2, xoshiro RNG, running statistics
src/geometry.*      polygon boundary with holes, inward normals, grid-accelerated
                    ray/segment query, validation (crossings, orientation,
                    contacts, minimum feature), SVG export
src/generators.*    straight channel, stepped channel, funnel (Nguyen 2021 /
                    Truccolo 2024 style), symmetric constriction, Tesla stages
src/material.hpp    graphene k_F, E_F, Fermi function, modes, Sharvin conductance
src/transport.*     injection, flights, reflection, scattering, batch statistics,
                    Landauer current
apps/geom.cpp       builds and validates every device, writes results/geom_*.svg
apps/tests.cpp      acceptance ladder T1..T11
apps/killtest.cpp   funnel vs Tesla sweep -> results/killtest.csv
tools/svg2png.py    rasteriser for the SVGs (needs Pillow)
```

Units everywhere: nm, fs, eV. `hbar = 0.6582 eV fs`, `v_F = 1 nm/fs`.

## Build and run

```bash
make            # clang++ -std=c++20, no dependencies beyond pthread
make test       # acceptance ladder (default 200000 particles per run)
./build/gd_geom
./build/gd_killtest 400000
```

Runs are multi-threaded over 64 independent batches; the batch means give the
standard errors. Results are reproducible for a given seed and batch count.

## Acceptance ladder (all pass)

| test | what | target |
|---|---|---|
| T1 | specular straight channel, field-free | T = 1 exactly |
| T2 | diffuse-wall channel, L/W = 0.5 .. 16 | 2D Clausing factors 0.804, 0.683, 0.541, 0.400, 0.274, 0.179 |
| T3 | asymmetric funnel, field-free, p = 1 and 0.3, lambda = 700 nm | `W_S T_SD = W_D T_DS` |
| T4 | same funnel with the field on, injection from both sides at one total energy | `N_S T_SD = N_D T_DS` |
| T5 | mirror-symmetric constriction under bias | A = 1 |
| T6 | zero bias | Z = 0, I = 0 |
| T7 | Tesla 3-stage, diffuse walls | trapped + leaked + dirac < 1e-3 |
| T8 | ballistic 1 um channel at n = 1e12 | Sharvin 8.74 mS |
| T9 | 10 um channel, lambda = 300 nm | `T = l1/(L + l1)`, `l1 = pi lambda/2` (Drude limit) |
| T10 | chord tolerance 0.05 vs 0.005 nm | same T |
| T11 | Truccolo-like funnel (Wc 100 / Wn 10 / Wd 40 nm) | T = Wn/Wc = 0.10 at zero field (info only) |

## Geometries in the kill test

All with 2 um wide leads, 175 nm minimum feature, n = 1.4e12 cm^-2
(E_F = 0.138 eV), 300 K:

* `funnel_nguyenD`: 30 degree taper to a 175 nm neck, abrupt shoulder, 6 um long
  (Nguyen et al. 2021, Device D; measured gamma = 1.66 at 80 uA).
* `straight_control`: 175 nm channel of the same length as the Tesla main channel.
* `tesla_1stage`, `tesla_3stage`, `tesla_3stage_v2`: 175 nm channel with
  Tesla-inspired bypass loops (branch peels off at 30 degrees against the
  reverse direction, turns through a 210 nm radius, re-enters at 45 degrees).
  These are a straight through-channel with side loops, **not** the classical
  Tesla valve; see the next bullet and `results/CLASSIC.md`.
* `tesla_classic`: the classical zig-zag Tesla valve (Tesla 1920 / Wikipedia
  principle drawing) generated by `tools/tesla_classic.py` and loaded from
  `meshes/tesla_classic.poly` with `gd::load_boundary`. Select geometries and
  the output file with `./build/gd_killtest 400000 tesla_classic,funnel_nguyenD out.csv`.

## Known limitations of Stage 1

* The uniform field is an assumption, not a solution; the Udine group's
  self-consistent 3D Poisson results roughly halve the uniform-field asymmetry
  and can reverse it for shallow funnel angles.
* No electron-electron scattering, so no hydrodynamic effects. The Tesla
  geometry's only demonstrated rectification mechanism (viscous flow at
  20 to 100 K) is therefore outside this model by construction.
* Classical trajectories are not valid for features below roughly 50 nm at
  n ~ 1e12 cm^-2 (Fermi wavelength 30 to 35 nm).
* Arcs are discretised in 2 degree chords, which adds a small artificial
  angular spread to specular reflections on curved walls.

## Stage H: electron-hydrodynamic solver

Steady incompressible Navier-Stokes with a Brinkman (momentum-relaxation)
term and Navier-slip walls, nondimensionalised by the inlet width `W` and the
mean inlet speed `U = I/(n e W)`:

    Re (u.grad)u = -grad p + lap u - Da u,  div u = 0
    Re = I/(n e nu)     (width independent in 2D)
    Da = W^2/(nu tau_mr)  (Gurzhi number)
    S  = l_s / W          (slip length; 0 no-slip, inf perfect slip)
    R  = Pi m* nu/(e^2 n W^2),  Pi = nondimensional pressure drop, m* = hbar k_F/v_F

* Taylor-Hood P2/P1 triangles, Newton with the analytic Jacobian, continuation
  in Re from the Stokes solution, Eigen SparseLU.
* Current bias as in the experiments: developed Brinkman-Poiseuille profile
  with slip at the inlet, do-nothing at the outlet (sets the pressure level).
* Walls: `u.n = 0` strongly in a rotated nodal frame with length-weighted
  (Engelman-Sani-Gresho) vertex normals, which makes the discrete wall flux
  vanish exactly; the Navier condition is a boundary integral. Vertices
  sharper than 25 degrees are pinned to `u = 0`.
* Meshes from gmsh (`tools/mesh.py`, pip-installed into `.venv`), generated
  on demand from the same `Boundary` polygons as the ballistic code and
  cached in `meshes/`. Midside nodes are built in C++.
* `D = Pi_reverse / Pi_forward` with reverse = flow from contact 1 to 0.

Build and run:

    make venv            # once: python venv with gmsh and pillow
    make                 # adds build/gd_hydro_tests, gd_hydro_sweep, gd_hydro_mesh
    make hydro-test      # acceptance ladder H1-H9 (several minutes; "quick" arg skips H5-H9)
    ./build/gd_hydro_sweep --geo tesla1,funnel --Re 0.3,1,3 --Da 0,1 --S 0.5,inf --threads 4

`src/hydro/units.hpp` converts (n, T, mobility, edge quality, I) to
(Re, Da, S) and `Pi` back to volts and ohms. VTK output (`write_vtk`) opens in
ParaView.

## Classical Tesla valve geometry

`tools/tesla_classic.py` builds the classical valve from channel centrelines
(shapely union of buffered polylines, so junction overlaps are exact): a
zig-zag main channel at +-theta, and at every vertex the arriving
(blocking-direction) segment continues straight into a loop leg of length `a`,
a half circle of radius `R`, and an anti-parallel return that rejoins the main
channel `2R / sin(2 theta)` downstream. The island sits between loop and main
channel with its tip facing the blocking flow. Forward (contact 0 -> 1) is the
unimpeded direction.

    .venv/bin/python tools/tesla_classic.py --png results/geom_tesla_classic.png      # 2 um x 1 um leads (ballistic)
    .venv/bin/python tools/tesla_classic.py --Lc 6000 --Ld 6000 --out meshes/tesla_classic_h.poly   # hydro leads
    ./build/gd_geom meshes/tesla_classic.poly 175      # validate (min feature) and draw with sample trajectories
    ./build/gd_killtest 400000 tesla_classic results/killtest_classic.csv
    ./build/gd_hydro_sweep --geo tesla_classic --Re 0.3,1,3,10,30 --Da 0 --S 0,0.5,inf --threads 3

Approved parameters: w 175 nm, theta 25 deg, R 175 nm, a 600 nm, Ls 1000 nm,
4 loops. Results: `results/CLASSIC.md`. Every geometry is drawn and approved
before it is simulated.

## Stage H2: unsteady gated-compressible (plasmonic) solver

Plan: `plans/hydro_thz.md`; derivation and discretisation: `plans/hydro_thz_notes.md`;
results: `results/THZ.md`. The electron fluid of Stage H made compressible by
the gate capacitance and the quantum capacitance, written in barotropic form
(`d_t u + (u.grad)u + u/tau = -grad h(n) + nu lap u`, `d_t n + div(n u) = 0`)
and solved as a second-order perturbation about rest: one complex linear
solve per frequency for the plasmonic response, one real Stokes-Brinkman
solve for the rectified dc flow, with lumped ports (Thevenin source, current
source, open, ground) as boundary tractions. A BDF2 time-domain solver on the
full nonlinear system anchors the perturbation solver in the ladder.

    ./build/gd_thz_tests                 # ladder T1-T9, 9/9 pass, ~80 min (T1, T5, T6, T8 are slow; "quick" skips them)
    ./build/gd_thz_sweep --geo funnel --set 100K --term sym --out results/thz/x.csv
    .venv/bin/python tools/thz_plot.py out.png label=results/thz/x.csv

Results: `results/THZ.md` (tables in `results/thz/tables.md`, CSVs, logs and
figures alongside). Headline: with the Dyakonov-Shur termination a 1 um
graphene funnel reaches 0.1 to 0.3 A/W from 10 GHz to 1 THz at an impedance a
75 ohm antenna can drive; the funnel's own geometric rectification matters
only near its taper resonance (1.87 THz for the 175 nm neck) and at 100 K;
the classical Tesla valve is an order of magnitude below the funnel.

Terminations: `sym` splits the antenna +-Va/2 with R/2 at each contact (the gate
at the midpoint potential: geometric rectification only); `dss` / `dsd` put the
Thevenin source at the source / drain with the other contact grounded to the
gate (the Dyakonov-Shur configuration, which rectifies any channel); `ds` is
the ideal-source / open-drain detector of the DS papers. Density is P1
(Taylor-Hood) with the density gradient integrated by parts, which gives
second-order dispersion accuracy and keeps the quasi-static limit stable.
