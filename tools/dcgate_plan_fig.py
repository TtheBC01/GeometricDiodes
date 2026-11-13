"""Plan figure for Stage H5 (plans/hydro_dcgate.png): the two ways of
reversing a gated channel, and the sizes of the steady asymmetry D - 1 that
are measured, published, already computed here, and to be computed.
usage: .venv/bin/python tools/dcgate_plan_fig.py plans/hydro_dcgate.png
"""
import sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon, Rectangle

BLUE, VERM, GREEN, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})

fig = plt.figure(figsize=(14.5, 6.6))
gs = fig.add_gridspec(1, 2, width_ratios=[1.0, 0.72], wspace=0.52, left=0.01, right=0.985, top=0.92, bottom=0.11)

# ---- (a) the two reversals ---------------------------------------------------
ax = fig.add_subplot(gs[0, 0]); ax.axis("off"); ax.set_xlim(0, 100); ax.set_ylim(0, 100)
ax.set_title("(a)  Two ways to reverse a gated channel", loc="left")


def device(x0, y0, vl, vr, gate_side, arrow_dir, shade):
    """funnel (wide left, neck right) of width 34 at (x0, y0); contact labels vl, vr;
    gate tied to the contact on gate_side; shade = (left, right) carrier shading."""
    w, h, hn = 34.0, 11.0, 2.2
    body = np.array([(0, h / 2), (10, h / 2), (22, hn / 2), (22, h / 2), (w, h / 2),
                     (w, -h / 2), (22, -h / 2), (22, -hn / 2), (10, -h / 2), (0, -h / 2)])
    ax.add_patch(Polygon(body + [x0, y0], closed=True, fc=shade, ec="none"))
    for xs, lab in ((x0 - 2.2, vl), (x0 + w, vr)):
        ax.add_patch(Rectangle((xs, y0 - h / 2), 2.2, h, fc=INK, ec="none"))
    ax.text(x0 - 3.0, y0, vl, ha="right", va="center", fontsize=10, color=INK)
    ax.text(x0 + w + 3.0, y0, vr, ha="left", va="center", fontsize=10, color=INK)
    # gate plate under the channel, wired to one contact
    gy = y0 - h / 2 - 3.2
    ax.add_patch(Rectangle((x0, gy - 0.9), w, 0.9, fc=GREY, ec="none"))
    gx = x0 - 1.1 if gate_side == "l" else x0 + w + 1.1
    ax.plot([gx, gx, x0 if gate_side == "l" else x0 + w], [y0 - h / 2, gy - 0.45, gy - 0.45], color=GREY, lw=1.2)
    ax.text(x0 + w / 2, gy - 1.6, "gate", ha="center", va="top", fontsize=8, color=GREY)
    xa = (x0 + 3, x0 + 9) if arrow_dir > 0 else (x0 + w - 3, x0 + w - 9)
    ax.annotate("", xy=(xa[1], y0), xytext=(xa[0], y0), arrowprops=dict(arrowstyle="-|>", color="white", lw=1.6))


# row 1: swap the contacts
ax.text(2, 91, "Swap the contacts", fontsize=10.5, fontweight="bold", color=BLUE)
ax.text(2, 86.5, "The drive and the gate reference move to the other end. Only the shape differs.", fontsize=9, color=INK)
device(8, 71, "+V", "0", "r", +1, BLUE)
device(58, 71, "0", "+V", "l", -1, BLUE)
ax.text(50, 71, "vs", ha="center", va="center", fontsize=10, color=GREY)
ax.text(2, 55.5, "asymmetry between the two = shape term", fontsize=9.5, color=BLUE)
# row 2: flip the polarity
ax.text(2, 44, "Flip the polarity", fontsize=10.5, fontweight="bold", color=VERM)
ax.text(2, 39.5, "Same contacts, opposite sign. The channel sits above the gate, then below it,", fontsize=9, color=INK)
ax.text(2, 36.0, "so it holds fewer carriers, then more. A straight channel does this too.", fontsize=9, color=INK)
device(8, 21, "+V", "0", "r", +1, "#f0b48a")
device(58, 21, "-V", "0", "r", -1, "#a8431f")
ax.text(50, 21, "vs", ha="center", va="center", fontsize=10, color=GREY)
ax.text(2, 5.5, "asymmetry between the two = circuit term (plus the shape term)", fontsize=9.5, color=VERM)

# ---- (b) sizes ---------------------------------------------------------------
ax = fig.add_subplot(gs[0, 1])
ax.set_title("(b)  Size of the steady asymmetry, D - 1", loc="left")
rows = [
    ("Measured: graphene Tesla valve\n(Geurs et al., 6.3 uA)", 5e-2, 5e-2, GREY, "pt"),
    ("Published simulation: with gate\n(Gungor et al., 50 mV, 10 nm channel)", 6.55e-2, 6.55e-2, GREY, "pt"),
    ("Ours, no gate response:\nat the measured current", 1e-6, 3e-5, BLUE, "ub"),
    ("Ours, no gate response:\nat the drift-speed limit", 3e-4, 2e-2, BLUE, "bar"),
    ("To compute: shape term with gate\n(swap the contacts)", 1e-6, 2e-3, BLUE, "plan"),
    ("To compute: circuit term\n(flip the polarity, 10 to 100 mV)", 7e-3, 7e-2, VERM, "plan"),
]
for i, (lab, lo, hi, c, kind) in enumerate(rows):
    y = len(rows) - 1 - i
    if kind == "pt":
        ax.plot([lo], [y], marker="o", ms=9, color=c, mec="white", mew=1.2, zorder=3)
    elif kind == "bar":
        ax.barh(y, hi - lo, left=lo, height=0.42, color=c, ec="none")
    elif kind == "ub":
        ax.barh(y, hi - lo, left=lo, height=0.42, color=c, ec="none", alpha=0.35)
        ax.plot([hi, hi], [y - 0.3, y + 0.3], color=c, lw=2.2)
        ax.text(hi * 1.35, y, "below 3e-5", va="center", fontsize=8.8, color=c)
    else:
        ax.barh(y, hi - lo, left=lo, height=0.42, fc="none", ec=c, lw=1.5, hatch="////")
        ax.text(hi * 1.35, y, "expected" if c == VERM else "expected: small,\nno first-order term", va="center", fontsize=8.8, color=c)
ax.text(5e-2 * 1.45, 5, "about 5 %", va="center", fontsize=8.8, color=GREY)
ax.text(6.55e-2 * 1.45, 4, "6.5 %", va="center", fontsize=8.8, color=GREY)
ax.text(2e-2 * 1.35, 2, "0.03 to 2 %", va="center", fontsize=8.8, color=BLUE)
ax.set_yticks(range(len(rows))); ax.set_yticklabels([r[0] for r in rows][::-1], fontsize=9)
ax.set_xscale("log"); ax.set_xlim(1e-6, 3); ax.set_ylim(-0.6, len(rows) - 0.4)
ax.set_xlabel("D - 1  (reverse resistance / forward resistance, minus one)")
ax.grid(axis="x", color="#e6e6e6", lw=0.6); ax.set_axisbelow(True)
ax.tick_params(axis="y", length=0)
ax.axhline(1.5, color="#cccccc", lw=0.8)
fig.savefig(sys.argv[1], dpi=150)
print("wrote", sys.argv[1])
