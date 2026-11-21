"""Plan figure for Stage H6 (plans/hydro_momentum.png): the three forms of the
momentum equation of the gated Dirac fluid, what the difference does to the
Dyakonov-Shur response of a uniform channel, how fast the carriers drift in
the large-signal runs already made, the order of the work, and the guesses
registered before any run. Prints the numbers quoted in the plan.
usage: .venv/bin/python tools/momentum_plan_fig.py plans/hydro_momentum.png
"""
import sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker
from matplotlib.patches import FancyBboxPatch

BLUE, VERM, GREEN, GREY, INK, PURP = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222", "#5e2a84"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "plans/hydro_momentum.png"

# ---------------------------------------------------------------- numbers
S0, VF, W = 3.57e6, 1.0e6, 1e-6          # wave speed at (1e12 cm^-2, 30 nm hBN), Fermi velocity, channel width
Q = 0.5 * VF ** 2 / S0 ** 2              # quantum-capacitance share of s0^2
KAPPA, PHI2 = -(1 + Q) / 2, -Q / 2       # h''(1), Phi''(1) of the graphene closure
R_UNIT = 130.0                           # ohm, for W = 1 um
SETS = {"100 K": 5e-12, "300 K": 1e-12}  # momentum-relaxation times


def fields(Om, Gam, L, N=4001):
    """First-order density and velocity, ideal source at x = 0 (n1 = 1), open drain at x = L."""
    k = np.sqrt(Om * (Om - 1j * Gam) + 0j); x = np.linspace(0, L, N)
    n1 = np.cos(k * (L - x)) / np.cos(k * L)
    u1 = -(k * np.sin(k * (L - x)) / np.cos(k * L)) / (1j * Om + Gam)
    return x, n1, u1


def dV(Om, Gam, L, alpha, kappa, phi2):
    """dc drain voltage per squared source amplitude for d_t u + alpha u d_x u + Gamma u = -d_x h(n)."""
    x, n1, u1 = fields(Om, Gam, L)
    drag = np.trapezoid((n1 * np.conj(u1)).real, x)
    return (phi2 - kappa) / 4 * (abs(n1[-1]) ** 2 - 1) + alpha / 4 * abs(u1[0]) ** 2 + Gam / 2 * drag


def proxy_umax(Om, Gam, L, Rs):
    """Largest first-order drift speed (units of s0, per unit antenna amplitude) in a uniform lead
    driven by a Thevenin source of resistance Rs with the drain grounded: a stand-in for the
    large-signal runs P1 and P2, without the neck."""
    k = np.sqrt(Om * (Om - 1j * Gam) + 0j); x = np.linspace(0, L, 4001)
    n = np.sin(k * (L - x)); u = k * np.cos(k * (L - x)) / (1j * Om + Gam)
    return abs(u / (n[0] + Rs * u[0])).max()


f = np.logspace(10, np.log10(3e12), 600)
resp = {}
for name, tau in SETS.items():
    Gam = W / (S0 * tau)
    Om = 2 * np.pi * f * W / S0
    resp[name] = {
        "sw": np.array([dV(o, Gam, 10.0, 1.0, 0.0, 0.0) for o in Om]),
        1.0: np.array([dV(o, Gam, 10.0, 1.0, KAPPA, PHI2) for o in Om]),
        0.5: np.array([dV(o, Gam, 10.0, 0.5, KAPPA, PHI2) for o in Om]),
        0.0: np.array([dV(o, Gam, 10.0, 0.0, KAPPA, PHI2) for o in Om]),
    }
print(f"q = {Q:.4f}, h''(1) = {KAPPA:.4f}, Phi''(1) = {PHI2:.4f}, s0 / v_F = {S0 / VF:.2f}")
for name in SETS:
    r = resp[name]
    for a in (0.5, 0.0):
        ratio = r[a] / r[1.0]
        print(f"{name}: response with alpha = {a} over the manuscript's, 10 GHz to 3 THz: {ratio.min():.2f} to {ratio.max():.2f}; "
              + ", ".join(f"{ff / 1e12:g} THz {np.interp(ff, f, ratio):.2f}" for ff in (1e11, 3e11, 1e12)))
    ipk = np.argmax(r[1.0])
    print(f"{name}: at the largest peak ({f[ipk] / 1e9:.0f} GHz) graphene / classical: manuscript {r[1.0][ipk] / r['sw'][ipk]:.2f}, "
          f"alpha 1/2 {r[0.5][ipk] / r['sw'][ipk]:.2f}, alpha 0 {r[0.0][ipk] / r['sw'][ipk]:.2f}")
VA_RUN = [0.01, 0.02, 0.05, 0.1, 0.2, 0.3, 0.5, 0.7]
drift = {}
for name, tau in SETS.items():
    Gam = W / (S0 * tau)
    for ff in (1e11, 3e11, 1e12):
        um = proxy_umax(2 * np.pi * ff * W / S0, Gam, 9.0, 75.0 / R_UNIT) * S0 / VF
        drift[(name, ff)] = um
        inside = [v for v in VA_RUN if v * um <= 0.3]
        print(f"{name}, {ff / 1e12:g} THz: drift / v_F = {um:.2f} x Va; 0.3 v_F at Va = {0.3 / um:.3f}, v_F at Va = {1 / um:.2f}; "
              f"amplitudes inside the limit: {inside}")

fig = plt.figure(figsize=(15.5, 10.8))
top = fig.add_gridspec(1, 3, left=0.055, right=0.985, top=0.955, bottom=0.585, wspace=0.27)
bot = fig.add_gridspec(1, 2, left=0.012, right=0.985, top=0.445, bottom=0.115, wspace=0.05, width_ratios=[2.45, 1.0])
plain = matplotlib.ticker.FuncFormatter(lambda v, _: f"{v:g}")

# ---------------------------------------------------------------- (a) the response of a uniform channel
ax = fig.add_subplot(top[0]); ax.set_title("(a)  Uniform channel at 100 K: rectified voltage, three forms", loc="left")
r = resp["100 K"]
ax.plot(f / 1e12, r[1.0], color=VERM, lw=2.0, label="manuscript, Eq. (9): full convective term")
ax.plot(f / 1e12, r[0.5], color=BLUE, lw=1.5, ls=(0, (5, 2)), label="Tomadin and Polini, Eq. (6): half of it")
ax.plot(f / 1e12, r[0.0], color=BLUE, lw=2.2, label="degenerate Dirac gas (Svintsov; own check): none")
ax.plot(f / 1e12, r["sw"], color=GREY, lw=1.4, ls=(0, (1.5, 1.5)), label="classical electron gas (Dyakonov and Shur)")
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.01, 0.5); ax.set_ylim(3e-3, 400)
ax.set_xticks([0.01, 0.03, 0.1, 0.3]); ax.xaxis.set_major_formatter(plain); ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
ax.set_xlabel("frequency (THz)"); ax.set_ylabel("dc drain voltage per squared amplitude\n(units of 1 / gate overdrive)")
ax.legend(loc="upper left", ncol=1, frameon=False, fontsize=8.8, handlelength=3.0, borderaxespad=0.3)
ax.set_yticks([1e-2, 1e-1, 1, 10])
ax.text(0.5, -0.15, "Ideal source, open drain, 10 um long, no viscosity.", transform=ax.transAxes, ha="center", va="top", fontsize=8.8, color=INK)

# ---------------------------------------------------------------- (b) the ratio
ax = fig.add_subplot(top[1]); ax.set_title("(b)  The same, relative to the manuscript's form", loc="left")
for name, col in (("100 K", BLUE), ("300 K", VERM)):
    r = resp[name]
    ax.plot(f / 1e12, r[0.5] / r[1.0], color=col, lw=1.5, ls=(0, (5, 2)))
    ax.plot(f / 1e12, r[0.0] / r[1.0], color=col, lw=2.2)
ax.axhline(1.0, color=INK, lw=0.8)
ax.set_xscale("log"); ax.set_xlim(0.01, 1.0); ax.set_ylim(0, 1.12)
ax.set_xticks([0.01, 0.03, 0.1, 0.3, 1.0]); ax.xaxis.set_major_formatter(plain); ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
ax.set_xlabel("frequency (THz)"); ax.set_ylabel("response / response with the manuscript's equation")
ax.text(0.0112, 1.015, "manuscript", fontsize=8.8, color=INK, va="bottom")
ax.text(0.0112, 0.785, "Tomadin and Polini, 300 K", fontsize=8.8, color=VERM, va="bottom")
ax.text(0.0112, 0.685, "Tomadin and Polini, 100 K", fontsize=8.8, color=BLUE, va="bottom")
ax.text(0.0112, 0.52, "degenerate Dirac gas, 300 K", fontsize=8.8, color=VERM, va="top")
ax.text(0.0112, 0.355, "degenerate Dirac gas, 100 K", fontsize=8.8, color=BLUE, va="bottom")
ax.text(0.5, -0.15, "The efficiency goes as the square of this ratio.", transform=ax.transAxes, ha="center", va="top", fontsize=8.8, color=INK)

# ---------------------------------------------------------------- (c) drift speed in the large-signal runs
ax = fig.add_subplot(top[2]); ax.set_title("(c)  Drift speed in the large-signal runs already made", loc="left")
va = np.logspace(-2, np.log10(0.8), 200)
ax.axhspan(1.0, 20, color="#f3d7c9", lw=0); ax.axhspan(0.3, 1.0, color="#f6ead0", lw=0)
ax.text(0.0108, 1.9, "faster than the Fermi\nvelocity: not possible", fontsize=8.8, color=INK, va="center")
ax.text(0.0108, 0.55, "above 0.3 $v_F$: outside the\nlimit used in Sec. III B", fontsize=8.8, color=INK, va="center")
for (name, ff), um in drift.items():
    ax.plot(va, um * va, color=BLUE if name == "100 K" else VERM, lw=1.3, alpha=0.85)
for v in VA_RUN:
    lo = min(u * v for u in drift.values()); hi = max(u * v for u in drift.values())
    ax.plot([v, v], [lo, hi], color=INK, lw=3.0, solid_capstyle="butt")
ax.text(0.085, 0.022, "bars: the eight amplitudes run,\nspread over 0.1, 0.3 and 1 THz\nand both temperatures", fontsize=8.6, color=INK, va="center", ha="left")
ax.text(0.0112, 0.070, "100 K", fontsize=9, color=BLUE, ha="left", va="center"); ax.text(0.019, 0.0215, "300 K", fontsize=9, color=VERM, ha="left", va="center")
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.01, 0.8); ax.set_ylim(0.01, 4)
ax.set_xticks([0.01, 0.02, 0.05, 0.1, 0.2, 0.5]); ax.xaxis.set_major_formatter(plain); ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
ax.set_yticks([0.01, 0.1, 1]); ax.yaxis.set_major_formatter(plain)
ax.set_xlabel("antenna amplitude / gate overdrive"); ax.set_ylabel("largest drift speed / Fermi velocity")
ax.text(0.5, -0.15, "One-dimensional stand-in: 75 ohm source on a uniform lead,\ndrain grounded. The neck of the funnel concentrates the\ncurrent, so the real values are higher.",
        transform=ax.transAxes, ha="center", va="top", fontsize=8.8, color=INK)

# ---------------------------------------------------------------- (d) order of the work
ax = fig.add_subplot(bot[0]); ax.set_title("(d)  Order of the work, with the two gates", loc="left", x=0.035)
ax.set_xlim(-4, 121); ax.set_ylim(0, 66); ax.axis("off")


def box(x, y, w, h, title, body, fc="#ece3f5", ls="-"):
    ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0.2,rounding_size=1.0", fc=fc, ec=INK, lw=0.9, ls=ls))
    ax.text(x + w / 2, y + h - 2.0, title, ha="center", va="top", fontsize=9.0, color=INK, weight="bold")
    ax.text(x + w / 2, y + h - 7.0, body, ha="center", va="top", fontsize=8.3, color=INK, linespacing=1.35)


def arrow(x0, y0, x1, y1):
    ax.annotate("", xy=(x1, y1), xytext=(x0, y0), arrowprops=dict(arrowstyle="-|>", color=INK, lw=1.1, shrinkA=0, shrinkB=0))


box(0, 27, 16, 31, "0  Read first", "both papers,\npage by page;\nthe stress of a\nmoving Dirac fluid;\nfinite-temperature\nterms\n\nno code, no runs", fc="#eeeeee")
box(19.5, 27, 16.5, 31, "1  Code", "two new terms\nin the closure;\nperturbation forcing;\ntime and steady\nsolver, exact in\nthe drift speed;\nold form kept\nas an option")
box(39.5, 27, 17, 31, "2  Checks", "T33 to T39:\nold numbers back;\nchannel closed forms;\nwaves on a drift;\nradial flow;\nrotated mesh;\ntwo solvers agree;\nthen the old ladder")
arrow(16.4, 42.5, 19.1, 42.5); arrow(36.4, 42.5, 39.1, 42.5)
ax.plot([59.3, 59.3], [9.5, 59.5], color=VERM, lw=2.6)
ax.text(59.3, 61.0, "gate 1: every check passes, or stop and report", ha="center", va="bottom", fontsize=8.8, color=VERM)
box(62.5, 44, 24, 14, "3a  Small signal", "S1 to S3, D1, D2, one control:\n950 solves, under 1 h", fc="#d9e8f5")
box(62.5, 27, 24, 14, "3b  Steady gated flow", "D1 to D4 of Stage H5:\n128 points, 30 min", fc="#d9e8f5")
box(62.5, 10, 24, 14, "3c  Large signal", "P1 to P4, D3: 74 amplitudes,\n13 CPU-hours (2 h, ten cores)", fc="#d9e8f5")
for y in (51, 34, 17): arrow(59.6, y, 62.1, y)
box(90, 27, 13, 31, "4  Report", "each number the\npaper takes from\nthe fluid model:\nold, new, ratio;\n\nguesses of (e)\nscored", fc="#d3ebe2")
arrow(86.9, 51, 89.6, 47); arrow(86.9, 34, 89.6, 40); arrow(86.9, 17, 89.6, 30)
ax.plot([105.2, 105.2], [9.5, 59.5], color=VERM, lw=2.6)
ax.text(105.2, 61.0, "gate 2: separate go-ahead", ha="center", va="bottom", fontsize=8.8, color=VERM)
box(107.5, 27, 13, 31, "5  Paper", "Sec. IV;\nFigs. 5 to 8, 13;\nTables III, IV;\nabstract and\nconclusion", fc="#ffffff", ls=(0, (4, 2)))
arrow(103.4, 42.5, 107.1, 42.5)
ax.text(0, 21.5, "New results go to their own folders (results/thz2, results/dcgate2).\nThe old files, the paper and its figures stay as they are until gate 2.",
        ha="left", va="top", fontsize=8.8, color=INK)
ax.text(0, 10.5, "Not touched: the classical-gas benchmark, the incompressible steady\nflow of Sec. III B, every ballistic and particle result.",
        ha="left", va="top", fontsize=8.8, color=INK)

# ---------------------------------------------------------------- (e) registered guesses
ax = fig.add_subplot(bot[1]); ax.set_title("(e)  Guesses registered before the runs", loc="left")
rows = [("1 um funnel, 100 K", 0.34, (0.10, 0.20)),
        ("175 nm funnel, 100 K", 0.45, (0.14, 0.27)),
        ("1 um funnel, 300 K", 0.09, (0.045, 0.060)),
        ("175 nm funnel, 300 K", 0.10, (0.050, 0.065))]
for k, (lab, now, (lo, hi)) in enumerate(rows):
    y = len(rows) - 1 - k
    ax.plot([lo, hi], [y, y], color=BLUE, lw=9, solid_capstyle="butt", alpha=0.85)
    ax.plot([now], [y], "o", color=VERM, ms=8)
    ax.text(0.0105, y, lab, fontsize=8.8, color=INK, va="center", ha="left")
ax.axvline(0.12, color=GREEN, lw=1.4, ls=(0, (4, 2)))
ax.text(0.128, 3.62, "ballistic short funnel,\nper absorbed watt", fontsize=8.6, color=GREEN, va="center", ha="left")
ax.plot([0.30], [-0.85], "o", color=VERM, ms=8); ax.text(0.34, -0.85, "now", fontsize=8.8, color=VERM, va="center")
ax.plot([0.0125, 0.02], [-0.85, -0.85], color=BLUE, lw=9, solid_capstyle="butt", alpha=0.85); ax.text(0.0225, -0.85, "guess", fontsize=8.8, color=BLUE, va="center")
ax.set_xscale("log"); ax.set_xlim(0.01, 0.6); ax.set_ylim(-1.3, 4.1); ax.set_yticks([]); ax.spines["left"].set_visible(False)
ax.set_xticks([0.01, 0.03, 0.1, 0.3]); ax.xaxis.set_major_formatter(plain); ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
ax.set_xlabel("best current responsivity with the Dyakonov-Shur termination (A/W)")
ax.text(0.5, -0.155, "No guess for the shape-only response: the terms that\nchange are the ones that produce it; its sign may change.",
        transform=ax.transAxes, ha="center", va="top", fontsize=8.8, color=INK)

fig.savefig(out, dpi=170)
print("wrote", out)
