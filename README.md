# GeometricDiodes

New to the topic? Start with the plain-language summary in
[OVERVIEW.md](OVERVIEW.md).

Event-driven semiclassical electron transport in two-dimensional graphene
devices, built to answer one question first: **does a Tesla-valve-inspired
channel rectify better than a funnel under the same physics?** (the "kill
test"), answered in `results/KILLTEST.md`. Stage H, the electron-hydrodynamic
solver (`src/hydro/`, `plans/hydro_tesla.md`, `results/HYDRO.md`), asks the
follow-up question the ballistic model cannot: does the Tesla shape rectify in
the viscous electron-fluid window (20 to 100 K), and at what current. Stages
H2 to H4 (`results/THZ.md`) take the gated funnel to THz as a plasmonic
rectenna element, and Stage B2 (`results/BALLISTIC_THZ.md`) closes the loop
by driving the ballistic funnel of the kill test at THz.

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
| T12 | Stage B2: dk/dt = F integration without energy re-imposition, static field | kill-test T(+-0.1 V) and A within 3.5 sigma / 1 % |
| T13 | two-sided equilibrium estimator at V = 0.1 and 0 V | = bias-window estimator; 0 |
| T14 | quasi-static anchor at 300 MHz, V_a 0.1 V | I_dc = <I(V_a cos)> and |Y_1| = static conductance within 5 % |
| T15 | straight control under ac drive, 3 GHz and 1 THz | I_dc = 0; trapped + leaked < 1e-4, dirac < 1e-2, heated < 1e-3 |
| T16 | chord cap v_F/(40 f) vs v_F/(160 f), localised field, 1 THz | same I_dc within 3.5 sigma / 2 % |
| T17 | Dirac cut vs classical turning point, localised field, 1 THz | same I_dc within 3.5 sigma |

T12 to T17 take 1e6 particles per side and about three minutes
(`./build/gd_tests 200000 1000000 b2` runs them alone).

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

### Stage H3: large-signal conversion efficiency

Plan: `plans/hydro_largesignal.md`; results: `results/THZ.md`, section S5. The
full nonlinear BDF2 solver driven at finite antenna amplitude, with an ideal
bias tee (ac current through R_ant, dc current through a matched load), gives
the conversion efficiency P_dc / P_av against incident power, the saturation
onset and the amplitude at which the fluid model breaks down. Runs start from
the BDF2-consistent perturbative state (the small-signal solution of the
time-stepped operator), which removes the start-up transient that otherwise
swamps the quadratic dc signal at small amplitude. Result (S5): the quadratic
law holds to about 100 uW of available power at every temperature and
frequency and to 1 mW below 300 GHz; peak efficiency 0.1 to 0.4 % at 1.8 mW
with no breakdown up to the gate overdrive; the shape-only (sym) response
saturates first. A square-law detector, not a harvester.

    ./build/gd_thz_tests T10,T11,T12,T13,T14   # H3 ladder on a coarse mesh (~35 min)
    ./build/gd_thz_power --geo funnel_w1000 --set 100K --term dss --f 3e11 --out results/thz/x.csv
    ./tools/thz_power_runs.sh              # the nine production runs P1-P4, in parallel
    .venv/bin/python tools/thz_power_plot.py out.png label=results/thz/x.csv
    .venv/bin/python tools/thz_power_tables.py results/thz

### Stage H4: carrier density, gate distance and contact resistance

Plan: `plans/hydro_density.md`; results: `results/THZ.md`, section S6. The
small-signal solver swept over n = 1e12 to 5e10 cm^-2 and d = 30, 10, 5 nm on
the 1 um funnel with the Fermi-liquid (E_F >= 3 kT) and gating (U_0 >= 4 kT)
limits carried as flags, with a matched antenna and a constant-mobility
variant, plus a contact resistance R_c in series with each contact (a port
change only) and one time-domain ladder at a second operating point to check
that the H3 saturation factor transfers in units of V_unit. Result (S6): the
knee of the efficiency curve moves as (n d)^2, from 0.9 mW at the H3 point to
3 uW at (5e10, 30 nm); the predicted peak efficiency stays below 1 % (0.03 to
0.9 %) over the whole valid grid, 0.25 % at best at 300 K, so lowering n and d converts
microwatts at the same few tenths of a per cent that the H3 device needs a
milliwatt for, and no gated-graphene operating point reaches per-cent
efficiency. Contact resistance is severe: 300 ohm um costs a factor 4 to 10 in
eta/P_av and a factor 3 in responsivity, because R_c at the drain lifts it off
the gate at ac and weakens the Dyakonov-Shur asymmetry itself.

    ./build/gd_thz_tests T15,T16,T17       # H4 ladder (seconds)
    ./build/gd_thz_sweep --geo funnel_w1000 --set 100K --term dss --hw 80 --hm 480 --n 5e11 --d 10e-9 --Rc 300 --f 1e11,3e11,1e12
    ./tools/thz_density_runs.sh D1         # the (n, d) grid, matched antenna, tau ~ sqrt(n); D2 n d_nm; D3
    .venv/bin/python tools/thz_density_plot.py results/thz
    .venv/bin/python tools/thz_density_tables.py results/thz > results/thz/tables_H4.md

Terminations: `sym` splits the antenna +-Va/2 with R/2 at each contact (the gate
at the midpoint potential: geometric rectification only); `dss` / `dsd` put the
Thevenin source at the source / drain with the other contact grounded to the
gate (the Dyakonov-Shur configuration, which rectifies any channel); `ds` is
the ideal-source / open-drain detector of the DS papers. Density is P1
(Taylor-Hood) with the density gradient integrated by parts, which gives
second-order dispersion accuracy and keeps the quasi-static limit stable.

## Stage B2: the ballistic funnel under THz drive

Plan: `plans/ballistic_thz.md`; results: `results/BALLISTIC_THZ.md`, data in
`results/bthz/`. The Stage 1 solver gains a time-dependent imposed potential
U(x, t) = -e V_a cos(2 pi f t) g(x), with g the Stage 1 uniform ramp or a drop
of length l_F localised at the neck, non-conservative chord flights
(dk/dt = F, no energy re-imposition, chords capped at v_F / (40 f)), and a
two-sided equilibrium-injection estimator that gives the dc current and the
first harmonic of the drain current in a moving landscape (`src/transport.*`,
`apps/bthz.cpp`, ladder T12 to T17). Device D funnel, p = 1, lambda = inf and
700 nm, 0.3 GHz to 1 THz. Result: under the uniform field the ballistic
rectification dies in two steps, a third of it between 1 and 10 GHz through
a slow-drift population the taper feeds into the source lead (5 % of source
electrons fly longer than 20 ps), the rest at the transit roll-off with its
zero at 300 GHz; at 1 THz it is 0.05 A/W. Confining the drop to the neck
keeps it alive: 0.37 A/W at 1 THz for a 500 nm drop, 0.45 A/W for a
neck-width drop, equal to the hydrodynamic funnel's Dyakonov-Shur
responsivity at 100 K and not above it, in the most favourable idealisation
the model has. At lambda = 700 nm the THz response is a transit-time effect
of 0.2 A/W with no quasi-static counterpart. Neither transport limit gives a
shape-based THz rectenna; no self-consistent stage follows.

    ./build/gd_tests 200000 1000000 b2                       # T12 to T17
    ./build/gd_bthz --geo funnel_nguyenD --field local --ellF 500 --lambda inf --f 1e12 --V 0.1 --N 1000000 --out results/bthz/points.csv
    ./tools/ballistic_thz_runs.sh B1                         # then B2u B2l B3 B0 B5, and B4 3e11 local 500
    .venv/bin/python tools/ballistic_thz_plot.py results/bthz
    .venv/bin/python tools/ballistic_thz_tables.py results/bthz > results/bthz/tables_B2.md

## Stage B7: short funnels and the length scaling of the ballistic THz response

Plan `plans/ballistic_short.md`, results `results/BALLISTIC_SHORT.md`. The
ballistic model has no intrinsic length, so the Stage B2 roll-off is a
function of f L / v_F: half response near 0.4, nothing above 1.7. A 0.55 um
funnel at the room-temperature mean free path gives 0.70 A/W per absorbed
watt at 1 THz under both field models (0.17 and 0.14 A/W for one device on
a 75 ohm antenna), twice the best plasmonic figure. Stage B2's "not a THz
rectifier" holds for the 6 um device only.

    make build/gd_tests && ./build/gd_tests 200000 1000000 b7
    tools/short_funnel_runs.sh S1   # then S2 to S5
    .venv/bin/python tools/short_funnel_tables.py && .venv/bin/python tools/short_funnel_plot.py

## Stage H5: steady flow with the gate

Plan `plans/hydro_dcgate.md`, results `results/HYDRO_GATE.md`. With the
gate, reversing the polarity of any channel gives an asymmetry of
0.48 V / U_0 (1.45 % at 43 mV) whatever its shape; reversing by swapping
the contacts leaves only the inertial term of Stage H, below 4e-4 inside
the drift limit.

    make build/gd_dc_gate build/gd_dc_gate_ref && ./build/gd_thz_tests T18,T19,T20
    tools/dc_gate_runs.sh D1        # then D2 to D4
    .venv/bin/python tools/dc_gate_ref.py && .venv/bin/python tools/dc_gate_tables.py && .venv/bin/python tools/dc_gate_plot.py

## Stage H6: the momentum equation of the Dirac fluid, corrected

Plan `plans/hydro_momentum.md`, results `results/HYDRO_MOMENTUM.md`. The
gated-fluid solvers of Stages H2 to H5 used the convective term of an
ordinary fluid. A degenerate Dirac gas has two more terms,
`- u (div u) / 2 - grad(u^2) / 4`, and an exact form at any drift below
v_F; both are now in `src/hydro` (`ClosureFn` in `thz_common.hpp`), with
the old form kept as `--closure legacy`. Every fluid run was repeated into
`results/thz2` and `results/dcgate2`, which the paper uses from its
seventh draft on; `results/thz` and `results/dcgate` hold the earlier
numbers. The 1 um funnel with
the Dyakonov-Shur termination gives 0.14 to 0.26 A/W at 100 K and 0.05 to
0.07 A/W at 300 K (0.15 to 0.34 and 0.07 to 0.09 before), and the
large-signal runs now stop where the drift reaches v_F.

After the review of the seventh draft (`paper/review_seventh_draft.md`)
the small-signal runs of the 1 um funnel, of the plain-channel control and
of the density and gate grid were repeated on a mesh of 20 nm at the walls
(`results/thz2/fine`, `tools/momentum_runs.sh fine` and `fine2`), because
the 80 nm mesh is 3 to 21 % low at 100 K and wrong above 1.2 THz. The
paper reads those files from its eighth draft on. The 175 nm funnel and
the Tesla valve are on their 14 nm mesh (`tools/momentum_runs.sh mesh14`);
the large-signal runs remain on the 80 nm mesh. The 1 um funnel with the
ordinary convective term is on the 20 nm mesh too
(`tools/momentum_runs.sh legacy20`, files `fine/L_funnel_w1000_*`), for
the comparison of the two equations in Sec. IV A. Every fluid number of the
paper, as a range over a stated band:

    .venv/bin/python tools/paper_numbers.py > results/thz2/paper_numbers.md

    make && ./build/gd_thz_tests T34,T35,T36,T37,T38,T39 && tools/momentum_regress.sh
    tools/momentum_runs.sh small    # then density, contact, steady, large (at most eight jobs at a time on 16 GB)
    .venv/bin/python tools/momentum_report.py > results/thz2/tables_H6.md
    .venv/bin/python tools/momentum_results_fig.py results/thz2/H6_before_after.png

## Stage B8: the short funnel with the field computed

Plan `plans/ballistic_selfconsistent.md`, results
`results/BALLISTIC_SELFCONSISTENT.md`. A particle simulation in which the
field comes from the electrons' own charge, a gate and metal contacts
(`src/pic.*`, `src/fft2d.*`). The 0.55 um funnel of Stage B7 at 1 THz and
the room-temperature mean free path gives 0.12 A/W per absorbed watt, not
0.70, and 0.023 A/W for one device on a 75 ohm antenna: the static
asymmetry is 1.05 because a quarter of the bias drops at the contacts and
the rest is centred on the neck. Under a 30 nm gate at 1 THz a straight
strip with its drain tied to the gate rectifies more than the funnel
shape. Extension: 0.10 A/W at a 100 nm mean free path, and no usable
rectification with half-diffuse walls (at most 0.04 A/W, sign reversed),
and with one specularity for all angles the response falls almost
linearly with the diffuse fraction, is half at a specularity of 0.84 and
vanishes near 0.6. Stage B9 below replaces that edge model by a rough
edge, which changes the conclusion about etched edges.

    make build/gd_bpic build/gd_pic_tests && ./build/gd_pic_tests        # T22 to T30
    tools/pic_convergence.sh && tools/pic_nseries.sh                     # decision point, numerical settings
    tools/pic_runs.sh P1 P2 P3 P4 && tools/pic_checks.sh                 # core set, about 4 hours on 10 cores
    tools/pic_ext.sh P5 P6 P6c P8                                        # extension and specularity scan, about 3 hours
    .venv/bin/python tools/pic_tables.py && .venv/bin/python tools/pic_plot.py

## Stage B9: the short funnel with rough edges

Plan `plans/ballistic_edges.md`, results `results/BALLISTIC_EDGES.md`.
The specularity scan of Stage B8 gave every wall hit the same chance of a
mirror reflection. Here a wall of rms roughness h reflects an electron of
wave number k arriving at angle theta from the wall normal like a mirror
with probability exp[-(2 h k cos(theta))^2], so that oblique hits are more
mirror-like (`Pic::reflect`, flag `--rough`). The funnel then tolerates
rougher edges than the scan said: the 1 THz current is half its
mirror-wall value at a head-on specularity of 0.5 (roughness 2 nm) and
gone at 0.2 (3 nm), against 0.84 and 0.6 with one specularity for all
angles, because the edge acts like one whose specularity is that of a hit
at about 60 degrees. Etched graphene edges have been measured from about
0 to about 0.8, so the upper half of that range keeps half to four fifths
of the response (0.06 to 0.10 A/W per absorbed watt) and the lower end
keeps nothing. The paper (revised 2026-11-15) and `OVERVIEW.md` carry
these numbers.

    make build/gd_bpic build/gd_pic_tests && ./build/gd_pic_tests T31    # wall rule, about 3 min
    tools/pic_ext.sh P9c P9 P9s                                          # controls first; about 2 hours
    .venv/bin/python tools/edges_results.py

## Stage B10: the short funnel between contacts that reflect

Plan `plans/ballistic_contacts.md`, results `results/BALLISTIC_CONTACTS.md`.
Every earlier run ended the device at ideal contacts, which absorb each
arriving electron. Here a contact absorbs an electron with probability T
and mirrors it otherwise (flag `--ct`), or passes only electrons near its
normal, as a p-n junction in front of the metal would (flag
`--cjunction`, the junction half width in nm); the injected gas is thinned
by the same law (`Pic::contact_transmission`). A contact is not a series
resistor to this device. With one transparency for all angles the
response per absorbed watt at 1 THz stays at 0.12 A/W down to T = 0.25
(338 ohm um per contact) under the 300 nm gate, and at 0.12, 0.12 and
0.07 A/W for T = 0.75, 0.5 and 0.25 under the 30 nm gate, where a series
resistor would leave 0.03. The best measured edge contacts (about
150 ohm um) are at T of about 0.4. Collimating contacts that conduct
equally well leave no current above the numerical floor. The condition
on the positive result is therefore what the contact does with electrons
arriving at wide angles, not its resistance. The gas heats behind
reflecting contacts (the model has no loss to the lattice), and behind
the collimating ones it was still heating at the end of the run.

Part B of the plan is the 6 um Device D with a computed field. The
reading found that its measured asymmetry of 1.66 is reached at about
1.5 V on a device of about 15 kilohm, against 0.1 V and 0.95 kilohm in
the model, so the comparison in Sec. III of the paper needs rewriting.
Run at +-0.1 V, the model gives an asymmetry of 1.0174 +- 0.0008 (the
same at 25 and at 50 million particles), with 55 % of the bias on the
500 nm around the neck; the first model, with a uniform field and no
scattering, gave 1.62. The measurement at 0.1 V reads 1.03 to 1.04
(+-0.02). The two do not conflict, and with the measured device fifteen
times more resistive than the model the agreement confirms nothing.
Figure `results/bpic/B10_deviceD.png`. The paper carries both parts
since the revision of 2026-11-16 (Secs. II C, III A and V E, Figs. 3
and 12, Table S6 of the supplement).

    make build/gd_bpic build/gd_pic_tests && ./build/gd_pic_tests T32 --N 8000000   # contact rule; T32d fails at 2 million particles
    tools/pic_ext.sh P10c P10t P10k P10s P10l P10n P10r                  # controls first; about 3 hours
    .venv/bin/python tools/contacts_results.py
    tools/pic_ext.sh P7 P7n                                              # Device D, 50 and 25 million particles; about 2.5 hours, 2 GB
    .venv/bin/python tools/deviced_results.py
