"""Plan figure for Stage B7 (plans/ballistic_short.png): the device series to
scale, the Stage B2 clean-limit transfer function replotted against the
reduced frequency f L / v_F, and the run matrix in the (f L / v_F, lambda / L)
plane. Existing points come from results/bthz/B2u.csv and B2l.csv.
usage: .venv/bin/python tools/short_funnel_plan_fig.py plans/ballistic_short.png
"""
import sys, math, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon, Rectangle
sys.path.insert(0, os.path.dirname(__file__))
from ballistic_thz_common import load

BLUE, VERM, GREEN, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False,
                     "axes.spines.right": False, "axes.grid": True, "grid.color": "#e6e6e6",
                     "grid.linewidth": 0.6})
vF = 1e6           # m/s
LAMF = 30.0        # Fermi wavelength at 1.4e12 cm^-2, nm


def funnel(Wc, Lc, alpha, Wn, Wd, Ld):
    Lt = (Wc - Wn) / 2 / math.tan(math.radians(alpha))
    x1, x2, x3 = Lc, Lc + Lt, Lc + Lt + Ld
    top = [(0, Wc / 2), (x1, Wc / 2), (x2, Wn / 2), (x2, Wd / 2), (x3, Wd / 2)]
    pts = top + [(x, -y) for x, y in reversed(top)]
    return np.array(pts), x3


# name, label, params, note
GEO = [
    ("G0", "G0  Device D, 6.0 um (done)", (2000, 2000, 30, 175, 2000, 2420)),
    ("G1", "G1  Device D at half scale, 3.0 um", (1000, 1000, 30, 87.5, 1000, 1210)),
    ("G2", "G2  Device D, leads cut to 250 nm, 2.1 um", (2000, 250, 30, 175, 2000, 250)),
    ("G3", "G3  short funnel, 0.55 um", (500, 100, 30, 100, 500, 100)),
    ("G4", "G4  Zhu-like, 0.50 um", (400, 169, 45, 75, 400, 169)),
]
LEN = {k: funnel(*p)[1] for k, _, p in GEO}
NECK = {k: p[3] for k, _, p in GEO}

fig = plt.figure(figsize=(14.5, 9.4))
gs = fig.add_gridspec(2, 2, height_ratios=[1.0, 1.12], hspace=0.40, wspace=0.20,
                      left=0.055, right=0.985, top=0.875, bottom=0.065)

# ---- (a) devices to one scale ------------------------------------------------
ax = fig.add_subplot(gs[0, 0]); ax.set_title("(a)  The device series, drawn to one scale", loc="left")
ax.set_aspect("equal"); ax.axis("off")
y = 0.0
cols = {"G0": GREY, "G1": BLUE, "G2": BLUE, "G3": VERM, "G4": VERM}
for k, lab, p in GEO:
    pts, L = funnel(*p)
    h = p[0]
    y -= h / 2
    ax.add_patch(Polygon(pts + [0, y], closed=True, fc=cols[k], ec="none", alpha=0.85))
    ax.text(L + 180, y, lab, va="center", ha="left", fontsize=9, color=INK)
    y -= h / 2 + 330
ax.plot([0, 1000], [y + 60, y + 60], color=INK, lw=2.2, solid_capstyle="butt")
ax.text(1120, y + 60, "1 um", ha="left", va="center", fontsize=9)
ax.plot([0, 700], [y - 330, y - 330], color=GREEN, lw=2.2, solid_capstyle="butt")
ax.text(820, y - 330, "mean free path 700 nm (hBN, 300 K)", ha="left", va="center", fontsize=8.5, color=GREEN)
ax.set_xlim(-150, 11800); ax.set_ylim(y - 520, 250)
# zoom of the two short devices
axz = ax.inset_axes([0.64, -0.06, 0.38, 0.36]); axz.set_aspect("equal"); axz.axis("off")
yy = 0
for k in ("G3", "G4"):
    p = [g for g in GEO if g[0] == k][0][2]
    pts, L = funnel(*p)
    yy -= p[0] / 2
    axz.add_patch(Polygon(pts + [0, yy], closed=True, fc=VERM, ec="none", alpha=0.85))
    xn = p[1] + (p[0] - p[3]) / 2 / math.tan(math.radians(p[2]))
    axz.text(L + 30, yy, f"{k}: neck {p[3]:.0f} nm\n= {2 * p[3] / LAMF:.0f} modes", va="center", ha="left", fontsize=8.5)
    yy -= p[0] / 2 + 110
axz.plot([0, 100], [yy + 40, yy + 40], color=INK, lw=2); axz.text(50, yy + 10, "100 nm", ha="center", va="top", fontsize=8)
axz.set_xlim(-20, 1050); axz.set_ylim(yy - 90, 30)
axz.set_title("enlarged x7", fontsize=8.5, loc="left", color=GREY, pad=2)

# ---- (b) existing transfer function against reduced frequency ---------------
ax = fig.add_subplot(gs[0, 1])
ax.set_title("(b)  Existing clean-limit result on the scale-free axis", loc="left", pad=68)
L0 = 6000.5e-9
series = [("B2u", "uniform field", 18.1, BLUE, "o"), ("B2l", "drop confined to L/12 at the neck", 14.7, VERM, "s")]
for fn, lab, Iqs, c, mk in series:
    rows = [r for r in load(f"results/bthz/{fn}.csv") if math.isinf(r["lambda_nm"]) and abs(r["V_V"] - 0.1) < 1e-9]
    rows.sort(key=lambda r: r["f_Hz"])
    x = np.array([r["f_Hz"] * L0 / vF for r in rows])
    s = np.array([r["Idc_A"] * 1e6 / Iqs for r in rows]); e = np.array([r["Idc_err_A"] * 1e6 / Iqs for r in rows])
    ax.errorbar(x, s, yerr=e, color=c, marker=mk, ms=5, lw=1.6, capsize=2, label=lab)
ax.axhline(0, color=INK, lw=0.7)
ax.axvspan(6.0, 40, color=GREEN, alpha=0.10, lw=0)
ax.text(15.5, 1.02, "not yet\ncomputed", ha="center", va="top", fontsize=8.5, color=GREEN)
ax.set_xscale("log"); ax.set_xlim(1e-3, 40); ax.set_ylim(-0.42, 1.2)
ax.set_xlabel("reduced frequency  f L / v_F"); ax.set_ylabel("rectified current / quasi-static value")
ax.legend(loc="center left", bbox_to_anchor=(0.0, 0.47), frameon=False, fontsize=8.8)
Ls = LEN["G3"] * 1e-9
for fT, lab in [(0.68, "0.68"), (3, "3"), (5, "5"), (28, "28 THz")]:
    xx = fT * 1e12 * Ls / vF
    ax.plot([xx, xx], [-0.42, -0.31], color=INK, lw=1.2)
    ax.text(xx, -0.29, lab, ha="center", va="bottom", fontsize=8)
ax.text(1.2e-3, -0.33, "frequencies reported in the literature,\nplaced for a 0.55 um device:", fontsize=8, va="center", color=INK)
sec = ax.secondary_xaxis("top", functions=(lambda x: x * vF / L0 / 1e12, lambda f: f * 1e12 * L0 / vF))
sec.set_xlabel("frequency for the 6 um device (THz)", fontsize=8.8)
sec2 = ax.secondary_xaxis(1.15, functions=(lambda x: x * vF / Ls / 1e12, lambda f: f * 1e12 * Ls / vF))
sec2.set_xlabel("frequency for a 0.55 um device (THz)", fontsize=8.8, color=VERM)
sec2.tick_params(colors=VERM); sec2.spines["top"].set_color(VERM)

# ---- (c) run matrix -----------------------------------------------------------
ax = fig.add_subplot(gs[1, :])
ax.set_title("(c)  Run matrix: every point is one device, one mean free path and one drive frequency", loc="left")
FREQ = [0.01, 0.03, 0.1, 0.3, 1, 3, 10, 30]      # THz
LAMS = [100.0, 700.0, math.inf]
CLEAN_Y = 30.0
mk = {"G0": "o", "G1": "v", "G2": "D", "G3": "s", "G4": "^"}
# existing G0 points
f0 = [3e8, 1e9, 3e9, 1e10, 3e10, 1e11, 3e11, 1e12]
for lam in (700.0, math.inf):
    yv = CLEAN_Y if math.isinf(lam) else lam / LEN["G0"]
    ax.scatter([f * L0 / vF for f in f0], [yv] * len(f0), marker="o", s=34, fc=GREY, ec="white", lw=0.6, zorder=3,
               label="G0, done (Stage B2)" if lam == 700.0 else None)
# planned
off = {"G0": 1.0, "G1": 0.80, "G2": 0.64, "G3": 1.25, "G4": 1.56}
for k, lab, p in GEO:
    L = LEN[k] * 1e-9
    fs = [2, 3, 5] if k == "G0" else FREQ
    for lam in LAMS:
        if k == "G0" and not math.isinf(lam): continue
        yv = CLEAN_Y * off[k] if math.isinf(lam) else lam / LEN[k]
        ax.scatter([f * 1e12 * L / vF for f in fs], [yv] * len(fs), marker=mk[k], s=34, fc="none" if k != "G0" else "none",
                   ec=cols[k] if k != "G0" else GREEN, lw=1.3, zorder=3,
                   label=(f"{k}, planned" if lam == LAMS[-1] else None))
ax.axhspan(15, 60, color="#f3f3f3", lw=0, zorder=0)
ax.text(1.2e-3, 47, "no scattering", fontsize=8.8, color=GREY, va="center")
ax.axhline(1.0, color=GREEN, lw=0.9, ls=(0, (4, 3)))
ax.text(82, 0.93, "mean free path = device length", fontsize=8.5, color=GREEN, va="top", ha="right")
ax.axvline(1.0, color=INK, lw=0.9, ls=(0, (4, 3)))
ax.text(1.06, 5.0, "drive period =\nflight time across", fontsize=8.5, color=INK, va="center", ha="left")
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(1e-3, 90); ax.set_ylim(0.0035, 60)
ax.set_yticks([0.01 * 0 + 0.03, 0.1, 0.3, 1, 3]); ax.set_yticklabels(["0.03", "0.1", "0.3", "1", "3"])
ax.set_xlabel("reduced frequency  f L / v_F"); ax.set_ylabel("mean free path / device length")
ax.legend(loc="lower left", frameon=True, framealpha=0.95, edgecolor="#dddddd", fontsize=8.8, ncol=3)
ax.text(0.995, 0.035, "Each device: 8 frequencies from 0.01 to 30 THz, mean free path 100 nm, 700 nm and infinite,\n"
        "two field profiles (uniform; drop over one neck width), plus static curves and a symmetric control.",
        transform=ax.transAxes, ha="right", va="bottom", fontsize=8.5, color=INK,
        bbox=dict(fc="white", ec="#dddddd", lw=0.6, boxstyle="round,pad=0.35"))

fig.savefig(sys.argv[1], dpi=150)
print("wrote", sys.argv[1], {k: round(v, 1) for k, v in LEN.items()})
