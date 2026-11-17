"""Plan figure for Stage B9 (plans/ballistic_edges.png): the angle-dependent
edge model, what the focusing experiment measures and what the funnel walls
see, the two predictions registered before the run, and the decision bands.
usage: .venv/bin/python tools/edges_plan_fig.py plans/ballistic_edges.png
"""
import sys, math, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon, Rectangle, FancyBboxPatch, Arc

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pic_common as c

BLUE, VERM, GREEN, GREY, INK, PURP, GOLD = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222", "#5e2a84", "#B8860B"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "plans/ballistic_edges.png"

KF = math.sqrt(math.pi * 1.4e12 * 1e-14)                 # 1/nm
P0 = [0.2, 0.35, 0.5, 0.75]                              # head-on specularity at the Fermi wave vector
COL = [VERM, GOLD, GREEN, BLUE]
H = [math.sqrt(-math.log(p)) / (2 * KF) for p in P0]     # rms roughness, nm
u = np.linspace(0, 1, 20001)
MEAN = [float(np.trapezoid(p ** (1 - u * u), u)) for p in P0]   # average over the hits of an equilibrium gas
GLANCE = [p ** 0.25 for p in P0]                         # at 60 degrees from the normal

fig = plt.figure(figsize=(14.5, 9.8))
gs = fig.add_gridspec(2, 2, left=0.05, right=0.985, top=0.95, bottom=0.07, wspace=0.17, hspace=0.27, width_ratios=[1.05, 1.0])

# ---------------------------------------------------------------- (a) geometry and angles
ax = fig.add_subplot(gs[0, 0]); ax.set_title("(a)  The same rough edge, met head-on and at a slant", loc="left")
Lt = 0.5 * (500 - 100) / math.tan(math.radians(30)); L = 100 + Lt + 100; xn = 100 + Lt
top = [(0, 250), (100, 250), (xn, 50), (xn, 250), (L, 250)]
pts = np.array(top + [(x, -y) for x, y in reversed(top)])
ax.add_patch(Rectangle((-120, -250), 120, 500, fc="#e3c56b", ec=INK, lw=0.8)); ax.add_patch(Rectangle((L, -250), 120, 500, fc="#e3c56b", ec=INK, lw=0.8))
ax.add_patch(Polygon(pts, closed=True, fc="#ece3f5", ec=INK, lw=1.0))
ax.text(-60, 0, "source", ha="center", va="center", fontsize=9, rotation=90); ax.text(L + 60, 0, "drain", ha="center", va="center", fontsize=9, rotation=90)
nx_, ny_ = -math.sin(math.radians(30)), -math.cos(math.radians(30))     # inward normal of the upper taper wall

def wall_pt(x): return np.array([x, 250 - (x - 100) * math.tan(math.radians(30))])

def hit(x_hit, d_in, col, lab, lab_xy, ha="left"):
    p = wall_pt(x_hit); d = np.array(d_in) / np.hypot(*d_in); n = np.array([nx_, ny_])
    start = p - 150 * d; r = d - 2 * np.dot(d, n) * n
    ax.annotate("", xy=p, xytext=start, arrowprops=dict(arrowstyle="-|>", color=col, lw=1.6, shrinkA=0, shrinkB=0))
    ax.annotate("", xy=p + 105 * r, xytext=p, arrowprops=dict(arrowstyle="-|>", color=col, lw=1.6, ls="-", shrinkA=0, shrinkB=0))
    ax.plot([p[0], p[0] + 95 * n[0]], [p[1], p[1] + 95 * n[1]], color=GREY, lw=0.8, ls=":")
    ax.text(*lab_xy, lab, color=col, fontsize=9, ha=ha, va="center")

# slanting: an electron moving along the axis meets the taper 60 degrees from the normal
def hit(x_hit, d_in, col, mirror=True):
    p = wall_pt(x_hit); d = np.array(d_in) / np.hypot(*d_in); n = np.array([nx_, ny_])
    ax.annotate("", xy=p, xytext=p - 150 * d, arrowprops=dict(arrowstyle="-|>", color=col, lw=1.8, shrinkA=0, shrinkB=0))
    if mirror:
        r = d - 2 * np.dot(d, n) * n
        ax.annotate("", xy=p + 110 * r, xytext=p, arrowprops=dict(arrowstyle="-|>", color=col, lw=1.8, shrinkA=0, shrinkB=0))
    ax.plot([p[0], p[0] + 90 * n[0]], [p[1], p[1] + 90 * n[1]], color=GREY, lw=0.8, ls=":")
    return p, n

hit(175, (1, 0), BLUE)
p, n = hit(335, (-nx_, -ny_), VERM, mirror=False)
for a in (-62, -38, 30, 58):
    t = math.radians(a); dvec = n * math.cos(t) + np.array([-n[1], n[0]]) * math.sin(t)
    ax.annotate("", xy=p + 75 * dvec, xytext=p, arrowprops=dict(arrowstyle="-|>", color=VERM, lw=0.9, alpha=0.75, shrinkA=0, shrinkB=0))
ax.annotate("neck, 100 nm", xy=(xn - 3, -30), xytext=(xn - 170, -215), fontsize=9, ha="center", color=INK, arrowprops=dict(arrowstyle="-", color=GREY, lw=0.7))
ax.text(L + 150, 245, "Head-on hit: mostly scattered.\nThis is what magnetic focusing\nmeasures. For etched edges it finds\na specularity of 0.2 to 0.5.", fontsize=9.2, va="top", color=VERM, linespacing=1.3)
ax.text(L + 150, 55, "Slanting hit, 60\u00b0 from the wall normal:\nmostly mirror-like, because a rough\nsurface is smoother to an electron\nthat skims it.", fontsize=9.2, va="top", color=BLUE, linespacing=1.3)
ax.text(L + 150, -135, "The runs so far used one\nspecularity for every angle.", fontsize=9.2, va="top", color=GREY, linespacing=1.3)
ax.plot([L + 150, L + 350], [-245, -245], color=INK, lw=1.6, solid_capstyle="butt"); ax.text(L + 365, -245, "200 nm", va="center", fontsize=8.8)
ax.set_xlim(-130, L + 600); ax.set_ylim(-275, 275); ax.set_aspect("equal"); ax.set_anchor("NW"); ax.axis("off")

# ---------------------------------------------------------------- (b) the model
ax = fig.add_subplot(gs[0, 1]); ax.set_title("(b)  The edge model: specularity against angle, for four roughnesses", loc="left")
th = np.linspace(0, 90, 361); cs = np.cos(np.radians(th))
ax.fill_between(th, 0, 0.15 * cs, color="#e9e9e9", lw=0, zorder=0)
ax.text(3, 0.05, "share of wall hits at each angle, no drive (cosine law)", fontsize=8.3, color=GREY, va="center")
for p0, h, col in zip(P0, H, COL):
    ax.plot(th, p0 ** (cs ** 2), color=col, lw=2.0, label=f"roughness {h:.1f} nm (head-on {p0:g})")
    ax.text(-1.5, p0, f"{p0:g}", color=col, fontsize=9, ha="right", va="center")
ax.plot([0, 0], [0.2, 0.5], color=VERM, lw=7, solid_capstyle="butt", alpha=0.35, zorder=1)
ax.text(1.8, 0.298, "measured by magnetic\nfocusing: 0.2 to 0.5", fontsize=8.6, color=VERM, va="center", linespacing=1.15)
ax.axvline(60, color=BLUE, lw=0.9, ls="--"); ax.text(61, 0.555, "an electron moving along the axis\nmeets the sloping wall here", fontsize=8.8, color=BLUE, va="center")
ax.text(1.5, 1.075, r"$p=\exp[-(2\,h\,k\cos\theta)^{2}]$" + ",   " + r"$h$: rms roughness,  $k$: the electron's wave vector", fontsize=9.2, color=INK, va="center")
ax.legend(loc="lower right", bbox_to_anchor=(1.0, 0.17), fontsize=8.6, frameon=False, handlelength=1.8, labelspacing=0.3)
ax.set_xlim(-8, 93); ax.set_ylim(0, 1.13); ax.set_yticks([0, 0.2, 0.4, 0.6, 0.8, 1.0]); ax.set_xticks([0, 30, 60, 90]); ax.set_xticklabels(["0\u00b0\nhead-on", "30\u00b0", "60\u00b0", "90\u00b0\ngrazing"])
ax.set_xlabel("angle of the hit from the wall normal"); ax.set_ylabel("probability of a mirror-like reflection")

# ---------------------------------------------------------------- (c) two predictions
ax = fig.add_subplot(gs[1, 0]); ax.set_title("(c)  Two guesses, written down before the run", loc="left")
ax.axhspan(-0.05, 0.05, color="#eeeeee", lw=0, zorder=0); ax.axhline(0, color=GREY, lw=0.6)
for d, ls, mfc in ((300, "-", INK), (30, "--", "white")):
    v = c.spec_scan(d)
    ax.errorbar([q[0] for q in v], [q[1]["I"] for q in v], [q[1]["e"] for q in v], color=INK, ls=ls, marker="D", ms=4.5, mfc=mfc, mec=INK, lw=1.2,
                label=f"computed, one specularity for all angles, gate {d} nm", zorder=3)
v = c.spec_scan(300); ps = np.array([q[0] for q in v]); Is = np.array([q[1]["I"] for q in v])
slope = (Is[1] - Is[0]) / (ps[1] - ps[0])
def curve(x): return float(np.interp(x, ps, Is)) if x >= ps[0] else float(Is[0] + slope * (x - ps[0]))
ax.plot([0.37, ps[0]], [curve(0.37), Is[0]], color=GREY, lw=1.0, ls=":", zorder=2)
for p0, m, g, col in zip(P0, MEAN, GLANCE, COL):
    ax.plot([m], [curve(m)], marker="v", color=col, ms=11, mec="white", mew=0.8, zorder=5)
    ax.plot([g], [curve(g)], marker="s", color=col, ms=9, mec="white", mew=0.8, zorder=5)
ax.plot([], [], marker="v", color=GREY, ms=10, ls="none", label="guess A: the average over all wall hits decides")
ax.plot([], [], marker="s", color=GREY, ms=8.5, ls="none", label="guess B: the slanting hits (60\u00b0) decide")
ax.text(0.345, 0.275, "Colours: the four roughnesses of (b), placed on the 300 nm curve.\n"
        "A: head-on 0.2 to 0.5 averages to 0.39 to 0.64. Nothing is left.\n"
        "B: the same edges are 0.67 to 0.84 at 60\u00b0. That leaves 0.07 to\n     0.2 \u00b5A, up to half of the mirror value.", fontsize=8.8, color=INK, va="top", linespacing=1.45)
ax.text(0.90, -0.085, "numerical floor", fontsize=8.3, color=GREY, va="center")
ax.text(0.375, -0.115, "extrapolated", fontsize=8.0, color=GREY, va="center", rotation=22)
ax.set_xlim(0.33, 1.03); ax.set_ylim(-0.2, 0.56); ax.set_xlabel("specularity (one value for all angles)"); ax.set_ylabel("rectified current at 1 THz (\u00b5A)")
ax.legend(loc="upper left", fontsize=8.5, frameon=False, bbox_to_anchor=(0.0, 1.0), handlelength=2.4, labelspacing=0.3)

# ---------------------------------------------------------------- (d) runs and decision
ax = fig.add_subplot(gs[1, 1]); ax.set_title("(d)  Runs, and what each outcome would mean", loc="left"); ax.axis("off")
ax.set_xlim(0, 1); ax.set_ylim(0, 1)

def box(y, hgt, head, body, fc):
    ax.add_patch(FancyBboxPatch((0.01, y - hgt), 0.98, hgt, boxstyle="round,pad=0.004,rounding_size=0.015", fc=fc, ec=INK, lw=0.8))
    ax.text(0.03, y - 0.022, head, fontsize=9.3, fontweight="bold", va="top")
    ax.text(0.03, y - 0.075, body, fontsize=8.9, va="top", linespacing=1.35)

box(0.99, 0.345, "Runs: 1 THz, 0.1 V, mean free path 700 nm, shape-only drive, as before",
    "8 runs: roughness 3.0, 2.4, 2.0 and 1.3 nm (head-on 0.2, 0.35, 0.5 and 0.75),\n"
    "   each with the gate at 300 nm and at 30 nm\n"
    "2 controls: mirror-symmetric funnel, roughness 2.4 nm, both gates (must give zero)\n"
    "1 control: the funnel with no drive, roughness 2.4 nm (must give zero; a wall model\n"
    "   that breaks detailed balance would rectify heat)\n"
    "About 85 minutes on 10 cores, after three new acceptance tests.", "#f3f3f3")
box(0.625, 0.175, "Outcome 1: within 0.1 \u00b5A of zero for head-on 0.2 to 0.5   (guess A)",
    "The slant does not rescue etched edges. The paper's caveat about oblique incidence\n"
    "is replaced by this result, and the conclusion hardens.", "#fbe9df")
box(0.43, 0.175, "Outcome 2: above 0.15 \u00b5A, a third of the mirror value   (guess B)",
    "Etched edges may keep a usable part of the response. The paper reports both edge\n"
    "models, and the contact-sensitivity run becomes worth doing.", "#e2f1ea")
box(0.235, 0.215, "Outcome 3: between 0.1 and 0.15 \u00b5A, or the two gates disagree",
    "Repeat those points with a second seed, then report as marginal.\n"
    "In every case the 1.3 nm point (head-on 0.75) shows whether the response follows\n"
    "the average specularity: about 0.2 \u00b5A if it does.", "#fdf6dc")
fig.savefig(out, dpi=150)
print("wrote", out)
for p0, h, m, g in zip(P0, H, MEAN, GLANCE): print(f"p0 {p0}: h {h:.2f} nm, average {m:.3f}, at 60 deg {g:.3f}")
