"""Plan figure for Stage B10 (plans/ballistic_contacts.png): the contact as
modelled now and the two variants, what each lets through by angle, the
series-resistor guess registered before the runs, and the measured device
with its decision bands. Prints the numbers quoted in the plan.
usage: .venv/bin/python tools/contacts_plan_fig.py plans/ballistic_contacts.png
"""
import sys, math
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon, Rectangle

BLUE, VERM, GREEN, GREY, INK, PURP, GOLD = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222", "#5e2a84", "#B8860B"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "plans/ballistic_contacts.png"

# ---------------------------------------------------------------- numbers
KF = math.sqrt(math.pi * 1.4e12 * 1e-14)          # 1/nm
G0 = 2 * 7.748091729e-5                           # 4 e^2 / h, S
GSH_W = G0 * KF / math.pi                         # Sharvin conductance per nm of width, S/nm
RSH_UM = 1e-3 / GSH_W                             # ohm um
G_LEAD = 5.18e-3                                  # S, Sharvin conductance of the 500 nm lead at 300 K (Stage B8)
G_DEV = 0.859e-3                                  # S, static conductance of G3 with ideal contacts (Stage B8, P1)
BASE = {300: (0.429, 0.012, 0.124), 30: (0.483, 0.013, 0.132)}   # I_dc uA, s.e., R_abs A/W at 1 THz, mirror walls, ideal contacts
TC = [0.75, 0.5, 0.25]
s = np.linspace(0, 1, 200001)                     # sine of the angle from the contact normal


def strip(T):
    """Conductance of a clean strip between two such contacts, over the Sharvin value."""
    return float(np.trapezoid(T / (2 - T), s))


def series(T):
    """Guess S: fraction of the applied voltage left across the device."""
    return 1.0 / (1.0 + 2 * (G_DEV / G_LEAD) * (1 - T) / T)


def tlm(T):
    """Contact resistance per contact as a transfer-length measurement would report it, ohm um."""
    return 0.5 * RSH_UM * (2 - T) / T


def junction(target):
    """a = pi k_F d of the collimating contact whose strip conductance equals `target`."""
    lo, hi = 1e-3, 500.0
    for _ in range(80):
        mid = 0.5 * (lo + hi)
        if strip(np.exp(-mid * s * s)) > target: lo = mid
        else: hi = mid
    return 0.5 * (lo + hi)


A_PN = [junction(t / (2 - t)) for t in (0.5, 0.25)]
D_PN = [a / (math.pi * KF) for a in A_PN]
print(f"k_F {KF:.4f} /nm; Sharvin {GSH_W * 500 * 1e3:.2f} mS for 500 nm at T = 0, {RSH_UM:.1f} ohm um")
for T in TC:
    f = series(T)
    print(f"T {T}: strip G/G_Sh {T / (2 - T):.3f}; contact resistance {tlm(T):.0f} ohm um; voltage fraction {f:.3f}; "
          + "; ".join(f"gate {d} nm I_dc {BASE[d][0] * f * f:.3f} uA R_abs {BASE[d][2] * f:.3f} A/W" for d in (300, 30)))
for t, a, d in zip((0.5, 0.25), A_PN, D_PN):
    Tm = float(np.trapezoid(np.exp(-a * s * s), s))
    half = math.degrees(math.asin(min(1.0, math.sqrt(math.log(2) / a))))
    print(f"collimating contact matched to T = {t}: pi k_F d = {a:.2f}, d = {d:.1f} nm, mean transmission {Tm:.3f}, half transmission at {half:.0f} deg")

fig = plt.figure(figsize=(14.5, 10.2))
gs = fig.add_gridspec(2, 2, left=0.055, right=0.985, top=0.955, bottom=0.065, wspace=0.17, hspace=0.40, width_ratios=[1.05, 1.0])

# ---------------------------------------------------------------- (a) the contact now
ax = fig.add_subplot(gs[0, 0]); ax.set_title("(a)  The contacts of the short funnel as modelled now", loc="left")
Lt = 0.5 * (500 - 100) / math.tan(math.radians(30)); L = 100 + Lt + 100; xn = 100 + Lt
top = [(0, 250), (100, 250), (xn, 50), (xn, 250), (L, 250)]
pts = np.array(top + [(x, -y) for x, y in reversed(top)])
ax.add_patch(Rectangle((-200, -250), 200, 500, fc="#e3c56b", ec=INK, lw=0.8)); ax.add_patch(Rectangle((L, -250), 200, 500, fc="#e3c56b", ec=INK, lw=0.8))
ax.add_patch(Polygon(pts, closed=True, fc="#ece3f5", ec=INK, lw=1.0))
ax.text(-100, 0, "source\nmetal", ha="center", va="center", fontsize=9); ax.text(L + 100, 0, "drain\nmetal", ha="center", va="center", fontsize=9)
ax.plot([0, 0], [-250, 250], color=VERM, lw=2.4); ax.plot([L, L], [-250, 250], color=VERM, lw=2.4)
ax.annotate("contact plane: every electron\nthat reaches it is absorbed;\nan equilibrium gas is injected", xy=(0, 150), xytext=(-320, 350), fontsize=8.8, color=VERM, va="center",
            arrowprops=dict(arrowstyle="-", color=VERM, lw=0.8))
for y0, dy in ((60, 0), (-40, 55), (-130, -60)):   # injected electrons, cosine law
    ax.annotate("", xy=(78, y0 + dy), xytext=(3, y0), arrowprops=dict(arrowstyle="-|>", color=BLUE, lw=1.4, shrinkA=0, shrinkB=0))
ax.annotate("", xy=(L - 2, -120), xytext=(L - 80, -160), arrowprops=dict(arrowstyle="-|>", color=BLUE, lw=1.4, shrinkA=0, shrinkB=0))
ax.annotate("", xy=(102, -300), xytext=(0, -300), arrowprops=dict(arrowstyle="<->", color=GREY, lw=0.9, shrinkA=0, shrinkB=0))
ax.text(50, -322, "100 nm", ha="center", va="top", fontsize=8.5, color=GREY)
ax.annotate("", xy=(L, -300), xytext=(xn, -300), arrowprops=dict(arrowstyle="<->", color=GREY, lw=0.9, shrinkA=0, shrinkB=0))
ax.text(0.5 * (xn + L), -322, "100 nm", ha="center", va="top", fontsize=8.5, color=GREY)
ax.text(255, 0, "mean free path\n700 nm", ha="center", va="center", fontsize=8.8, color=GREY)
# where the bias drops (Stage B8, static, 300 nm gate, +0.1 V)
yb, hb = -470, 46
seg = [(-8, 22, 0.076, VERM, "0.08"), (22, 350, 0.137, "#b9b9b9", "0.14 lead and taper"), (350, 531, 0.620, PURP, "0.62 neck region"), (531, L + 8, 0.167, VERM, "0.17")]
for x0, x1, fr, col, lab in seg:
    ax.add_patch(Rectangle((x0, yb), x1 - x0, hb, fc=col, ec="white", lw=1.2, alpha=0.9))
ax.text(186, yb + hb / 2, "0.14 lead and taper", ha="center", va="center", fontsize=8.6, color=INK)
ax.text(440, yb + hb / 2, "0.62 neck", ha="center", va="center", fontsize=8.6, color="white")
ax.annotate("0.08 at the\nsource plane", xy=(7, yb + hb / 2), xytext=(-70, yb + hb / 2), fontsize=8.6, color=VERM, ha="right", va="center", arrowprops=dict(arrowstyle="-", color=VERM, lw=0.8))
ax.annotate("0.17 at the\ndrain plane", xy=(L, yb + hb / 2), xytext=(L + 70, yb + hb / 2), fontsize=8.6, color=VERM, ha="left", va="center", arrowprops=dict(arrowstyle="-", color=VERM, lw=0.8))
ax.text(0.5 * L, yb - 18, "share of the bias along the axis, computed field (Stage B8)", ha="center", va="top", fontsize=8.8, color=INK)
ax.text(0.5 * L, yb - 62, "A quarter of the bias drops at the two contact planes. No other contact has been tried.", ha="center", va="top", fontsize=9.2, color=INK)
ax.set_xlim(-330, L + 330); ax.set_ylim(-590, 420); ax.set_aspect("equal"); ax.axis("off")

# ---------------------------------------------------------------- (b) what a contact lets through
ax = fig.add_subplot(gs[0, 1]); ax.set_title("(b)  What each contact lets through, by angle of the hit", loc="left")
th = np.linspace(-90, 90, 1441); c = np.cos(np.radians(th)); sn = np.sin(np.radians(th))
ax.plot(th, c, color=INK, lw=2.0, label="ideal contact (now)")
ax.plot(th, 0.5 * c, color=BLUE, lw=2.0, label="transparency 0.5"); ax.plot(th, 0.25 * c, color=BLUE, lw=2.0, ls=(0, (5, 2)), label="transparency 0.25")
ax.plot(th, np.exp(-A_PN[0] * sn * sn) * c, color=VERM, lw=2.0, label=f"collimating, junction {D_PN[0]:.0f} nm")
ax.plot(th, np.exp(-A_PN[1] * sn * sn) * c, color=VERM, lw=2.0, ls=(0, (5, 2)), label=f"collimating, junction {D_PN[1]:.0f} nm")
ax.legend(loc="upper center", ncol=3, frameon=False, fontsize=8.8, handlelength=2.6, columnspacing=1.6)
ax.set_xlabel("angle of the hit from the contact normal (degrees)"); ax.set_ylabel("electrons let through, per angle (head-on, ideal contact = 1)")
ax.set_xlim(-90, 90); ax.set_ylim(0, 1.27); ax.set_yticks([0, 0.2, 0.4, 0.6, 0.8, 1.0]); ax.set_xticks(range(-90, 91, 30))
ax.text(0.5, -0.135, "The rest are reflected like a mirror. Each collimating contact has the contact resistance of the\ntransparency drawn with the same line style; only the angles differ.",
        transform=ax.transAxes, ha="center", va="top", fontsize=8.8, color=INK)

# ---------------------------------------------------------------- (c) the registered guess
ax = fig.add_subplot(gs[1, 0]); ax.set_title("(c)  Guess S, registered before the runs: the contact is only a series resistor", loc="left", pad=24)
T = np.linspace(0.2, 1.0, 400); f = np.array([series(t) for t in T])
ax.axhspan(-0.05, 0.05, color="#dddddd", lw=0); ax.text(0.985, 0.0, "numerical floor", fontsize=8.5, color=GREY, ha="left", va="center")
ax.fill_between(T, 0.75 * BASE[300][0] * f * f, 1.25 * BASE[300][0] * f * f, color=BLUE, alpha=0.13, lw=0)
ax.plot(T, BASE[300][0] * f * f, color=BLUE, lw=2.0); ax.plot(T, BASE[30][0] * f * f, color=BLUE, lw=2.0, ls=(0, (5, 2)))
ax.errorbar([1.0], [BASE[300][0]], yerr=[BASE[300][1]], fmt="o", color=INK, ms=7, capsize=3); ax.errorbar([1.0], [BASE[30][0]], yerr=[BASE[30][1]], fmt="o", mfc="white", color=INK, ms=7, capsize=3)
for t in TC:
    ax.plot([t], [BASE[300][0] * series(t) ** 2], "D", mfc="white", mec=BLUE, ms=7, mew=1.4)
    ax.plot([t], [BASE[30][0] * series(t) ** 2], "D", mfc="white", mec=BLUE, ms=7, mew=1.4)
ax.text(0.93, 0.50, "30 nm gate", color=BLUE, fontsize=9, ha="right", va="bottom"); ax.text(0.86, 0.335, "300 nm gate", color=BLUE, fontsize=9, ha="left", va="top")
ax.text(0.99, 0.17, "band: within a quarter of\nthe guess (300 nm gate)", color=BLUE, fontsize=8.6, ha="left", va="center")
ax.text(0.99, 0.56, "measured,\nideal contacts", color=INK, fontsize=8.6, ha="left", va="center")
ax.text(0.47, 0.375, "open diamonds: the planned runs,\ndrawn where the guess puts them", color=INK, fontsize=8.6, ha="left", va="center")
ax.set_xlim(1.03, 0.19); ax.set_ylim(-0.08, 0.62)
ax.set_xlabel("transparency of each contact"); ax.set_ylabel("rectified current at 1 THz, 0.1 V (uA)")
ax.set_xticks([1.0, 0.75, 0.5, 0.25])
sec = ax.secondary_xaxis("top", functions=(lambda t: 0.5 * RSH_UM * (2 - np.clip(t, 1e-3, None)) / np.clip(t, 1e-3, None), lambda r: RSH_UM / (np.clip(r, 1e-3, None) + 0.5 * RSH_UM)))
sec.set_xticks([50, 100, 150, 250, 350]); sec.set_xlabel("contact resistance of each contact (ohm um)")
ax.spines["top"].set_visible(False)

# ---------------------------------------------------------------- (d) the measured device
g = gs[1, 1].subgridspec(2, 1, height_ratios=[1.0, 1.05], hspace=0.05)
ax = fig.add_subplot(g[0]); ax.set_title("(d)  Device D, the one measured asymmetry, with a computed field", loc="left")
LtD = 0.5 * (2000 - 175) / math.tan(math.radians(30)); LD = 2000 + LtD + 2420; xnD = 2000 + LtD
topD = [(0, 1000), (2000, 1000), (xnD, 87.5), (xnD, 1000), (LD, 1000)]
ptsD = np.array(topD + [(x, -y) for x, y in reversed(topD)])
ax.add_patch(Rectangle((-200, -1000), 200, 2000, fc="#e3c56b", ec=INK, lw=0.6)); ax.add_patch(Rectangle((LD, -1000), 200, 2000, fc="#e3c56b", ec=INK, lw=0.6))
ax.add_patch(Polygon(ptsD, closed=True, fc="#ece3f5", ec=INK, lw=1.0))
x0 = LD + 1000
ax.add_patch(Polygon(pts + np.array([x0, 0]), closed=True, fc="#ece3f5", ec=INK, lw=0.8))
ax.text(1000, 0, "Device D\n6.0 um long, 2 um wide\nneck 175 nm", ha="center", va="center", fontsize=8.8)
ax.text(x0 + 0.5 * L, -420, "the short funnel,\nsame scale", ha="center", va="top", fontsize=8.6, color=GREY)
ax.text(4700, 0, "52 times the area:\n148 000 electrons", ha="center", va="center", fontsize=8.8)
ax.set_xlim(-400, x0 + L + 500); ax.set_ylim(-1150, 1150); ax.set_aspect("equal"); ax.axis("off")

ax = fig.add_subplot(g[1])
for a0, a1, col, lab in ((1.0, 1.15, "#f3d7c9", "not reproduced"), (1.15, 1.40, "#eeeeee", "partly"), (1.40, 1.78, "#d3ebe2", "reproduced")):
    ax.axvspan(a0, a1, color=col, lw=0); ax.text(0.5 * (a0 + a1), 0.93, lab, ha="center", va="center", fontsize=9, color=INK)
marks = [(1.66, 0.62, VERM, "measured, 80 uA", "center"), (1.62, 0.36, GREY, "uniform field,\nno scattering", "right"),
         (1.09, 0.36, GREY, "uniform field,\n700 nm", "left"), (1.053, 0.62, BLUE, "short funnel,\ncomputed field", "left")]
for a, y, col, lab, ha in marks:
    ax.plot([a, a], [0.0, y - 0.07], color=col, lw=1.6); ax.plot([a], [0.0], "o", color=col, ms=6, clip_on=False)
    dx = 0.012 if ha == "left" else (-0.012 if ha == "right" else 0.0)
    ax.text(a + dx, y, lab, ha=ha, va="center", fontsize=8.8, color=col)
ax.annotate("", xy=(1.01, 0.80), xytext=(1.05, 0.80), arrowprops=dict(arrowstyle="|-|", color=PURP, lw=1.6, mutation_scale=4, shrinkA=0, shrinkB=0))
ax.text(1.065, 0.80, "my guess for Device D", ha="left", va="center", fontsize=8.8, color=PURP)
ax.set_xlim(1.0, 1.78); ax.set_ylim(0, 1.02); ax.set_yticks([]); ax.spines["left"].set_visible(False)
ax.set_xlabel("static asymmetry I(+0.1 V) / |I(-0.1 V)|")

fig.savefig(out, dpi=170)
print("wrote", out)
