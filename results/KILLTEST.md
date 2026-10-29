# Kill test result (2026-10-29)

Model: Stage 1 (uniform in-device field, isotropic momentum relaxation with
free path lambda, edge specularity p, cosine Fermi-Dirac injection, Landauer
bias-window current). n = 1.4e12 cm^-2 (E_F = 0.138 eV), 300 K, 400 000
particles per bias point, 64 batches. All devices: 2 um leads, 175 nm minimum
feature. A(V) = I(+V)/|I(-V)|; R+ = V/I(+V). Full table in
`killtest_stdout.txt`, raw numbers in `killtest.csv`.

## Headline numbers at V = 0.1 V

| device | p | lambda (nm) | A | +- | R+ (kOhm) |
|---|---|---|---|---|---|
| funnel (Nguyen D) | 1.0 | inf | **1.62** | 0.015 | 0.53 |
| funnel (Nguyen D) | 1.0 | 700 | 1.09 | 0.011 | 0.92 |
| funnel (Nguyen D) | 0.5 | 700 | 1.07 | 0.010 | 0.96 |
| funnel (Nguyen D) | 0.0 | 700 | 1.06 | 0.012 | 1.03 |
| straight 175 nm control | 1.0 | inf | 0.99 | 0.010 | 0.85 |
| Tesla 1 stage | 1.0 | inf | 1.08 | 0.012 | 1.07 |
| Tesla 1 stage | 1.0 | 700 | 0.99 | 0.014 | 2.03 |
| Tesla 3 stage | 1.0 | inf | 1.01 | 0.012 | 1.59 |
| Tesla 3 stage | 0.5 | 700 | 0.98 | 0.025 | 5.8 |
| Tesla 3 stage | 0.0 | 700 | 0.96 | 0.028 | 8.2 |
| Tesla 3 stage v2 | 1.0 | inf | 0.95 | 0.012 | 1.70 |
| Tesla 3 stage v2 | 0.0 | 700 | 0.93 | 0.033 | 12.4 |

## Reading

1. **Every Tesla variant has A = 1 within statistical error** (all 54 Tesla
   points lie in 0.93 to 1.08 with errors of 0.01 to 0.045), for both bias
   magnitudes, all three specularities and all three mean free paths. The
   only point clearly above 1 is the single-stage, perfectly specular,
   infinite-mean-free-path case (1.08 +- 0.01), and it vanishes as soon as
   any diffuse reflection or bulk scattering is present. Several three-stage
   points sit slightly *below* 1 (reverse asymmetry), consistent with the
   re-entry junction acting as a weak funnel in the "wrong" direction.
2. **The funnel rectifies in the same model**: A = 1.62 ballistic/specular,
   1.06 to 1.09 at the room-temperature hBN mean free path of 700 nm, with
   two to four times lower resistance than any Tesla variant.
3. **The straight 175 nm control gives A = 1**, confirming that the leads and
   step junctions contribute no asymmetry of their own.
4. **Tesla loops cost resistance in both directions**: 1.4 to 22 kOhm versus
   0.5 to 2 kOhm for the funnel, the penalty growing with diffuse edges and
   with the number of stages, exactly as the feasibility report predicted.
5. The "dirac" fate (kinetic energy driven below 2 meV) stayed below 4e-4 at
   all points, so the electrons-only approximation is not distorting the
   numbers at these biases. Trapped and leaked fractions were zero.

## Verdict

Under the physics the geometric-diode literature itself uses (uniform
field, momentum relaxation, specular-to-diffuse edges), a Tesla-valve-shaped
two-terminal graphene channel does **not** rectify, while a funnel of equal
neck width does. The Tesla geometry adds series resistance and no asymmetry.
This closes the ballistic Tesla question at Stage 1; there is no case for
carrying it to Poisson coupling, CUDA or an optimiser. The Tesla shape
remains interesting only in the electron-hydrodynamic regime (20 to 100 K),
which this model excludes by construction.

## Observation for the benchmark

Nguyen et al. measured gamma = 1.66 for Device D at 80 uA. The Stage 1
model gives A = 1.62 in the ballistic/specular limit at 0.1 V (about 190 uA)
but only 1.09 at lambda = 700 nm. The uniform-field assumption and the
lambda / specularity choice therefore matter at the factor-of-two level for
the funnel. That calibration question belongs to the next stage, not to the
Tesla question, which is insensitive to it (A stays at 1 for every
combination tried).
