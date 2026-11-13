"""Plan figure for Stage B8 (plans/ballistic_selfconsistent.png): the device
with its gate and contacts, the loop of the particle simulation, the length
scales that set the grid, and the decision bands against the Stage B7 values.
usage: .venv/bin/python tools/selfconsistent_plan_fig.py plans/ballistic_selfconsistent.png
"""
import sys, math
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon, Rectangle, FancyBboxPatch, FancyArrowPatch

BLUE, VERM, GREEN, GREY, INK, PURP, GOLD = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222", "#5e2a84", "#B8860B"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "plans/ballistic_selfconsistent.png"
fig = plt.figure(figsize=(14.5, 9.6))
gs = fig.add_gridspec(2, 2, left=0.045, right=0.985, top=0.95, bottom=0.07, wspace=0.16, hspace=0.24, width_ratios=[1.05, 1.0])

# ---------------------------------------------------------------- (a) device
sub = gs[0, 0].subgridspec(2, 1, height_ratios=[1.55, 1.0], hspace=0.08)
ax = fig.add_subplot(sub[0]); ax.set_title("(a)  What is simulated: the 0.55 um funnel with contacts and a gate", loc="left")
Lt = 0.5 * (500 - 100) / math.tan(math.radians(30)); L = 100 + Lt + 100; xn = 100 + Lt
top = [(0, 250), (100, 250), (xn, 50), (xn, 250), (L, 250)]
pts = np.array(top + [(x, -y) for x, y in reversed(top)])
ax.add_patch(Rectangle((-200, -250), 200, 500, fc="#e3c56b", ec=INK, lw=0.8)); ax.add_patch(Rectangle((L, -250), 200, 500, fc="#e3c56b", ec=INK, lw=0.8))
ax.add_patch(Polygon(pts, closed=True, fc="#d9c8ea", ec=INK, lw=0.9))
for gx in np.arange(-200, L + 200.1, 50): ax.plot([gx, gx], [-250, 250], color="white", lw=0.35, alpha=0.7, zorder=3)
for gy in np.arange(-250, 250.1, 50): ax.plot([-200, L + 200], [gy, gy], color="white", lw=0.35, alpha=0.7, zorder=3)
ax.text(-100, 0, "source\ncontact\n$V_S(t)$", ha="center", va="center", fontsize=9, zorder=5); ax.text(L + 100, 0, "drain\ncontact\n$V_D(t)$", ha="center", va="center", fontsize=9, zorder=5)
ax.text(205, 0, "graphene\n2900 electrons", ha="center", va="center", fontsize=9, color=PURP, zorder=5)
ax.annotate("neck, 100 nm", xy=(xn - 4, 0), xytext=(L + 330, 150), ha="left", va="center", fontsize=9, color=INK, arrowprops=dict(arrowstyle="-", color=GREY, lw=0.7))
ax.text(L + 330, 20, "Top view.\nCharge and potential live\non a grid of 2.5 to 5 nm\n(drawn here at 50 nm).", ha="left", va="center", fontsize=8.8, color=GREY)
ax.plot([L + 330, L + 530], [-200, -200], color=INK, lw=1.6, solid_capstyle="butt"); ax.text(L + 545, -200, "200 nm", va="center", fontsize=8.8)
ax.set_xlim(-520, L + 900); ax.set_ylim(-275, 275); ax.set_aspect("equal"); ax.axis("off")
# side view, schematic
ax = fig.add_subplot(sub[1]); ax.set_xlim(0, 10); ax.set_ylim(0, 4); ax.axis("off")
ax.add_patch(Rectangle((1.6, 0.75), 5.6, 2.2, fc="#eef3f8", ec="none"))
ax.add_patch(Rectangle((1.6, 0.45), 5.6, 0.30, fc="#9aa3ad", ec=INK, lw=0.8)); ax.text(4.4, 0.32, "gate, held at 0 V", ha="center", va="top", fontsize=9)
ax.add_patch(Rectangle((1.6, 2.95), 1.2, 0.30, fc="#e3c56b", ec=INK, lw=0.8)); ax.add_patch(Rectangle((6.0, 2.95), 1.2, 0.30, fc="#e3c56b", ec=INK, lw=0.8))
ax.plot([2.8, 6.0], [3.0, 3.0], color=PURP, lw=3.4, solid_capstyle="butt")
ax.text(2.2, 3.36, "source", ha="center", va="bottom", fontsize=8.8); ax.text(6.6, 3.36, "drain", ha="center", va="bottom", fontsize=8.8); ax.text(4.4, 3.36, "graphene", ha="center", va="bottom", fontsize=8.8, color=PURP)
for xq, sgn in ((3.2, "+"), (3.8, "+"), (5.1, "\u2212"), (5.6, "\u2212")):
    ax.text(xq, 3.02, sgn, ha="center", va="bottom", fontsize=12, color=VERM)
    ax.text(xq, 0.78, "\u2212" if sgn == "+" else "+", ha="center", va="bottom", fontsize=12, color=BLUE)
ax.annotate("", xy=(7.5, 2.95), xytext=(7.5, 0.75), arrowprops=dict(arrowstyle="<->", color=INK, lw=0.9))
ax.text(7.65, 1.85, "d = 30 nm or 300 nm,\ndielectric constant 3.8", va="center", fontsize=9)
ax.text(4.4, 1.85, "The charge the electrons pile up, and its\nimage in the gate, set the potential\nthe electrons move in.", ha="center", va="center", fontsize=8.8, color=INK)
ax.text(1.45, 1.85, "Side view,\nnot to scale", ha="right", va="center", fontsize=8.8, color=GREY)

# ---------------------------------------------------------------- (b) loop
ax = fig.add_subplot(gs[0, 1]); ax.set_title("(b)  One time step (1.25 to 2.5 fs), repeated for about 30 periods", loc="left")
boxes = [(0.50, 0.90, "1. Count the charge of all electrons in each grid cell", "#efe6f6"),
         (0.50, 0.70, "2. Potential everywhere: sheet charge and its gate image,\nplus the charge the contacts need to stay at $V_S(t)$, $V_D(t)$", "#e4eef7"),
         (0.50, 0.50, "3. Move every electron at $v_F$ under that field;\nreflect at the edges, scatter with mean free path $\\lambda$", "#efe6f6"),
         (0.50, 0.30, "4. Contacts absorb what arrives and inject the\nequilibrium flux (as in Stages B2 to B7)", "#f6ecd0"),
         (0.50, 0.10, "5. Record terminal currents, displacement current included:\nrectified current, admittance, absorbed power", "#e2f1ea")]
for x, y, t, c in boxes:
    ax.add_patch(FancyBboxPatch((x - 0.40, y - 0.072), 0.80, 0.144, boxstyle="round,pad=0.008,rounding_size=0.02", fc=c, ec=INK, lw=0.8, transform=ax.transAxes))
    ax.text(x, y, t, ha="center", va="center", fontsize=9.2, transform=ax.transAxes)
for (x, y, *_), (x2, y2, *_) in zip(boxes[:-1], boxes[1:]):
    ax.add_patch(FancyArrowPatch((x, y - 0.074), (x2, y2 + 0.074), arrowstyle="-|>", mutation_scale=12, color=INK, lw=0.9, transform=ax.transAxes))
ax.add_patch(FancyArrowPatch((0.905, 0.30), (0.905, 0.90), connectionstyle="arc3,rad=0.0", arrowstyle="-|>", mutation_scale=12, color=GREY, lw=0.9, transform=ax.transAxes))
ax.plot([0.90, 0.905], [0.30, 0.30], color=GREY, lw=0.9, transform=ax.transAxes); ax.plot([0.90, 0.905], [0.90, 0.90], color=GREY, lw=0.9, transform=ax.transAxes)
ax.text(0.925, 0.60, "next step", rotation=90, ha="left", va="center", fontsize=8.8, color=GREY, transform=ax.transAxes)
ax.text(0.02, 0.60, "new in this stage:\nsteps 1, 2 and 5", rotation=90, ha="center", va="center", fontsize=8.8, color=BLUE, transform=ax.transAxes)
ax.set_xlim(0, 1); ax.set_ylim(0, 1); ax.axis("off")

# ---------------------------------------------------------------- (c) scales
ax = fig.add_subplot(gs[1, 0]); ax.set_title("(c)  Lengths that set the grid, and frequencies that set the physics", loc="left")
items = [("screening length, no gate", 2.07, VERM), ("grid cell (planned)", 2.5, INK), ("screening length under a 30 nm gate", 5.6, VERM), ("Fermi wavelength", 30.0, GREY),
         ("gate distance, thin", 30.0, BLUE), ("neck", 100.0, PURP), ("gate distance, thick", 300.0, BLUE), ("device length", 546.0, PURP), ("mean free path, 300 K", 700.0, GREY)]
for i, (lab, v, c) in enumerate(items):
    yy = len(items) - 1 - i
    ax.plot([1, v], [yy, yy], color=c, lw=2.2, solid_capstyle="butt"); ax.plot([v], [yy], "o", color=c, ms=5)
    ax.text(v * 1.18, yy, f"{lab}: {v:g} nm", va="center", fontsize=9, color=INK)
ax.set_xscale("log"); ax.set_xlim(1, 3e4); ax.set_ylim(-3.6, len(items) - 0.4); ax.set_yticks([]); ax.set_xlabel("length (nm)")
ax.spines["left"].set_visible(False)
ax.text(1.15, -1.25, "Frequencies for this device: drive 1 THz;  half response of the imposed-field model 0.7 THz;\n"
        "plasma resonances 1.8 and 3.6 THz under the 30 nm gate, about 6 THz under the 300 nm gate.\n"
        "The imposed-field model has no plasma resonances. This one does.", fontsize=9, va="top", color=INK)

# ---------------------------------------------------------------- (d) decision
ax = fig.add_subplot(gs[1, 1]); ax.set_title("(d)  Decision at 1 THz, mean free path 700 nm, 300 nm gate, shape-only drive", loc="left")
bands = [(0.0, 0.10, "#f7dcd0", "below 0.10: the positive result is withdrawn"), (0.10, 0.35, "#f6ecd0", "0.10 to 0.35: stands, weakened"),
         (0.35, 1.6, "#dff0e6", "0.35 and above: confirmed")]
for lo, hi, c, lab in bands:
    ax.axvspan(lo, hi, color=c, lw=0)
ax.text(0.050, 2.9, "withdrawn", ha="center", va="top", rotation=90, fontsize=9.2, color=VERM); ax.text(0.225, 2.9, "stands, weakened", ha="center", va="top", rotation=90, fontsize=9.2, color=GOLD); ax.text(0.95, 2.72, "confirmed", ha="center", fontsize=9.2, color=GREEN)
ref = [(0.70, 2.0, "Stage B7, uniform field: 0.70", PURP, "o"), (0.71, 1.55, "Stage B7, drop at the neck: 0.71", PURP, "s"),
       (0.42, 1.10, "Stage B7, rough edges: 0.14 and 0.42", GREY, "o"), (0.14, 1.10, None, GREY, "o"),
       (0.34, 0.55, "plasmonic funnel: 0.09 (300 K) and 0.34 (100 K)", BLUE, "D"), (0.09, 0.55, None, BLUE, "D")]
for v, yy, lab, c, m in ref:
    ax.plot([v], [yy], m, color=c, ms=7, zorder=4)
    if lab: ax.text(max(v, 0.44) + 0.035, yy, lab, va="center", fontsize=9, color=INK)
ax.annotate("", xy=(1.0, 0.0), xytext=(0.3, 0.0), arrowprops=dict(arrowstyle="<->", color=INK, lw=1.3))
ax.text(0.65, -0.28, "what I expect: 0.3 to 1.0", ha="center", va="top", fontsize=9.2, color=INK)
ax.set_xlim(0, 1.6); ax.set_ylim(-0.8, 3.0); ax.set_yticks([]); ax.spines["left"].set_visible(False)
ax.set_xlabel("rectified current per absorbed watt (A/W); the absorbed power is now computed, not assumed")
fig.savefig(out, dpi=140); print("wrote", out)
