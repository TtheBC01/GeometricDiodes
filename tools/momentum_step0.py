"""Stage H6, step 0 (plans/hydro_momentum.md, section 2): the checks made before any code.
1. Angular integrals of the drifting Dirac distribution (energy, momentum, stress).
2. Wave speeds on a drifting ungated fluid from the exact one-dimensional equation against
   the relativistic addition of velocities.
3. The closed form used for the uniform channel against Tomadin and Polini, Eq. (43).
4. The isothermal finite-temperature coefficients from Svintsov's xi.
usage: .venv/bin/python tools/momentum_step0.py
"""
import numpy as np

# ---------------------------------------------------------------- 1. angular integrals
th = np.linspace(0, 2 * np.pi, 400001)[:-1]
c, s = np.cos(th), np.sin(th)
print("1. angular factors of f(p (v_F - u cos theta) - mu), beta = u / v_F")
print("   beta   energy / (1-b^2)^-5/2   (1 + b^2/2)   momentum / [1.5 b (1-b^2)^-5/2]   Pi_xx / [(1+2b^2)/2 (1-b^2)^-5/2]   Pi_yy / [(1-b^2)^-3/2 / 2]   Svintsov (13) / true Pi_yy")
for b in (0.1, 0.3, 0.6):
    w = (1 - b * c) ** -3
    e = w.mean(); g = (c * w).mean(); pxx = (c * c * w).mean(); pyy = (s * s * w).mean()
    k = (1 - b * b) ** -2.5
    print(f"   {b:.1f}   {e / k:.6f}                {1 + b * b / 2:.6f}      {g / (1.5 * b * k):.6f}                         {pxx / ((1 + 2 * b * b) / 2 * k):.6f}"
          f"                              {pyy / (0.5 * (1 - b * b) ** -1.5):.6f}                     {(0.5 * (1 - 2 * b * b) * k) / pyy:.4f}")

# ---------------------------------------------------------------- 2. velocity addition
print("\n2. ungated fluid (sound speed v_F / sqrt 2) drifting at u0: wave speeds from")
print("   (1 + g) d_t u + g u d_x u + (1 - b^2) v_F^2 / (2 n) d_x n = 0,  g = b^2 / (2 (1 - b^2)),  with continuity")
cs = np.sqrt(0.5)
for b in (0.05, 0.2, 0.5, 0.8):
    g = b * b / (2 * (1 - b * b))
    # (s - b)((1 + g) s - g b) = (1 - b^2) / 2
    r = np.roots([1 + g, -(g * b + (1 + g) * b), g * b * b - (1 - b * b) / 2])
    exact = sorted([(b + cs) / (1 + b * cs), (b - cs) / (1 - b * cs)])
    sv = np.roots([1 + g, +(g * b - (1 + g) * b), -g * b * b - (1 - b * b) / 2])   # sign of Svintsov's Eq. (16) as printed
    print(f"   u0 = {b:.2f} v_F: {sorted(r.real)[0]:+.6f} {sorted(r.real)[1]:+.6f}   addition of velocities {exact[0]:+.6f} {exact[1]:+.6f}"
          f"   with the convective sign of Svintsov (16): {sorted(sv.real)[0]:+.6f} {sorted(sv.real)[1]:+.6f}")

# ---------------------------------------------------------------- 3. closed form against Tomadin-Polini (43)
def dV(Om, Gam, L, alpha, kappa, phi2, N=40001):
    k = np.sqrt(Om * (Om - 1j * Gam) + 0j); x = np.linspace(0, L, N)
    n1 = np.cos(k * (L - x)) / np.cos(k * L); u1 = -(k * np.sin(k * (L - x)) / np.cos(k * L)) / (1j * Om + Gam)
    return (phi2 - kappa) / 4 * (abs(n1[-1]) ** 2 - 1) + alpha / 4 * abs(u1[0]) ** 2 + Gam / 2 * np.trapezoid((n1 * np.conj(u1)).real, x)


def tp43(Om, tau, vF2, C1=0.25):
    C2 = (1 + vF2) / (1 + vF2 / 2); sv = np.sqrt(1 + vF2 / 2)
    K = Om / sv * np.sqrt(1 + 1j / (Om * tau)); K1, K2 = K.real, K.imag
    beta = 2 * Om * tau / np.sqrt(1 + (Om * tau) ** 2); den = np.cos(2 * K1) + np.cosh(2 * K2)
    return 0.25 * (1 - C2 / 2 - (2 - C2) / den + beta * (1 - C1) * (np.cosh(2 * K2) - np.cos(2 * K1)) / den)


print("\n3. uniform channel, source fixes the density, open drain: closed form of the plan against Tomadin and Polini, Eq. (43)")
worst = 0.0
for vF2 in (0.0785, 0.5, 2.0):          # (v_F / s_gate)^2; 0.0785 is the reference point
    sv = np.sqrt(1 + vF2 / 2); q = (vF2 / 2) / (1 + vF2 / 2)
    for tau in (0.5, 1.0, 5.0):
        for Om in (0.3, 1.0, 1.7, 3.0, 5.0):
            a = tp43(Om, tau, vF2); b = dV(Om / sv, 1 / (tau * sv), 1.0, 0.5, -(1 + q) / 2, 0.0)
            worst = max(worst, abs(a / b - 1))
    print(f"   (v_F/s)^2 = {vF2}: q = {q:.4f}; e.g. Omega 1, tau 1: Eq. (43) {tp43(1.0, 1.0, vF2):.6f}, closed form alpha = 1/2 {dV(1 / sv, 1 / sv, 1.0, 0.5, -(1 + q) / 2, 0.0):.6f},"
          f" alpha = 1 {dV(1 / sv, 1 / sv, 1.0, 1.0, -(1 + q) / 2, 0.0):.6f}, alpha = 0 {dV(1 / sv, 1 / sv, 1.0, 0.0, -(1 + q) / 2, 0.0):.6f}")
print(f"   largest relative difference over 45 points: {worst:.2e}")

# ---------------------------------------------------------------- 4. finite temperature
def fermi(j, eta, N=200001):
    t = np.linspace(0, eta + 60, N)
    return np.trapezoid(t ** j / (1 + np.exp(np.clip(t - eta, -700, 700))), t)


HBARVF = 0.6582119569          # eV nm at v_F = 1e6 m/s
print("\n4. isothermal coefficients from xi = n^2 / (eps <1/eps>) (Svintsov, Eq. (17)): c_div = 1 - 2 xi, c_grad = (1 - 2 xi) / 2, alpha = 3 - 4 xi")
print("   n (cm^-2)   T (K)   mu (meV)  mu/kT    xi     c_div    c_grad   alpha   inertial mass / m(n) at T = 0")
for n_cm2 in (1e12, 5e11, 2e11, 1e11, 5e10):
    for T in (100.0, 300.0):
        kT = 8.617333e-5 * T; n = n_cm2 * 1e-14
        target = n * np.pi * HBARVF ** 2 / (2 * kT * kT)
        lo, hi = -20.0, 200.0
        for _ in range(80):
            mid = 0.5 * (lo + hi)
            if fermi(1, mid) < target: lo = mid
            else: hi = mid
        eta = 0.5 * (lo + hi); F1, F2 = fermi(1, eta), fermi(2, eta)
        xi = F1 * F1 / (F2 * np.log1p(np.exp(eta)))
        mass = 1.5 * F2 / F1 / np.sqrt(2 * F1)       # (3 eps / 2 n) / E_F(n)
        print(f"   {n_cm2:.0e}      {T:3.0f}   {eta * kT * 1e3:7.1f}   {eta:5.2f}   {xi:.4f}  {1 - 2 * xi:+.4f}  {(1 - 2 * xi) / 2:+.4f}  {3 - 4 * xi:.3f}   {mass:.3f}")
