"""Stage H5 figure (results/dcgate/H5_shape_vs_circuit.png): (a) the two
asymmetries against bias for every device, (b) the swap-contacts asymmetry of
the gated solver against the incompressible Stage H value.
usage: .venv/bin/python tools/dc_gate_plot.py"""
import sys, os, math
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
sys.path.insert(0, os.path.dirname(__file__))
from dc_gate_common import rows, ref, ref_of, NAME, R

BLUE, VERM, GREEN, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False,
                     "axes.grid": True, "grid.color": "#e6e6e6", "grid.linewidth": 0.6})
MK = {"chan": "x", "symcon": "+", "funnel": "o", "funnel_w1000": "s", "tesla_classic": "^", "tesla_classic_w1000": "v"}
allr = rows("D1") + rows("D2") + rows("D3")
refd = ref()
fig, axs = plt.subplots(1, 3, figsize=(18.5, 5.8), gridspec_kw=dict(wspace=0.24, left=0.05, right=0.99, top=0.92, bottom=0.11))

ax = axs[0]; ax.set_title("(a)  300 K parameters, slip walls: asymmetry against bias", loc="left")
sel = [r for r in allr if r["set"] == "300K" and math.isinf(r["slip"])]
v = np.logspace(-3, np.log10(0.3), 50)
one = sorted([r for r in sel if r["geo"] == "chan"], key=lambda r: r["v_target"])
ax.plot([r["v_target"] for r in one], [r["D_1d_flip"] - 1 for r in one], color=INK, lw=1.2, ls=(0, (4, 3)), label="1-D closed form, any shape", zorder=2)
for geo in MK:
    sub = sorted([r for r in sel if r["geo"] == geo], key=lambda r: r["v_target"])
    if not sub: continue
    x = np.array([r["v_target"] for r in sub]); inside = np.array([r["umax_vF"] <= 0.3 for r in sub])
    yf = np.array([abs(r["D_flip0"] - 1) for r in sub])
    ax.plot(x, yf, color=VERM, lw=0.8, alpha=0.6, zorder=3)
    ax.scatter(x[inside], yf[inside], marker=MK[geo], s=36, color=VERM, zorder=4)
    if MK[geo] in "x+": ax.scatter(x[~inside], yf[~inside], marker=MK[geo], s=36, color=VERM, alpha=0.45, zorder=4)
    else: ax.scatter(x[~inside], yf[~inside], marker=MK[geo], s=36, fc="none", ec=VERM, alpha=0.45, zorder=4)
    ys = np.array([abs(r["D_swap_dn"] - 1) for r in sub])
    if geo == "chan": continue
    c = GREY if geo == "symcon" else BLUE
    ax.plot(x, ys, color=c, lw=0.8, alpha=0.6, zorder=3)
    ax.scatter(x[inside], ys[inside], marker=MK[geo], s=36, color=c, zorder=4, label=NAME[geo])
    if MK[geo] in "x+": ax.scatter(x[~inside], ys[~inside], marker=MK[geo], s=36, color=c, alpha=0.45, zorder=4)
    else: ax.scatter(x[~inside], ys[~inside], marker=MK[geo], s=36, fc="none", ec=c, alpha=0.45, zorder=4)
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlabel("bias  V / U_0   (U_0 = 1.43 V)"); ax.set_ylabel("| D - 1 |")
ax.set_ylim(3e-10, 0.6)
ax.text(0.0012, 2.2e-3, "flip the polarity:\nevery device, one curve", color=VERM, fontsize=9, va="bottom")
ax.text(0.0012, 2.6e-5, "swap the contacts,\n20 nm wall mesh", color=BLUE, fontsize=9, va="bottom")
ax.legend(loc="lower right", frameon=False, fontsize=8.6, ncol=2)
ax.text(0.02, 0.97, "faint markers: drift speed above 0.3 v_F", transform=ax.transAxes, ha="left", va="top", fontsize=8.4, color=GREY)

ax = axs[1]; ax.set_title("(b)  Shape term with the gate against the Stage H value without it", loc="left")
lo, hi = 1e-8, 1
ax.plot([lo, hi], [lo, hi], color=INK, lw=1.0)
ax.fill_between([lo, hi], [lo / 2, hi / 2], [lo * 2, hi * 2], color=GREEN, alpha=0.12, lw=0, label="within a factor two")
for geo in ("funnel", "funnel_w1000", "tesla_classic", "tesla_classic_w1000"):
    for st, c in (("300K", BLUE), ("100K", VERM)):
        for slip_inf in (True, False):
            sub = [r for r in allr if r["geo"] == geo and r["set"] == st and math.isinf(r["slip"]) == slip_inf]
            pts = [(abs(ref_of(refd, r)[0] - 1), abs(r["D_swap_dn"] - 1), r["umax_vF"] <= 0.3) for r in sub if not math.isnan(ref_of(refd, r)[0])]
            if not pts: continue
            X = np.array([p[0] for p in pts]); Y = np.array([p[1] for p in pts]); ins = np.array([p[2] for p in pts])
            lab = f"{NAME[geo]}, {st}" if slip_inf else None
            ax.scatter(X[ins], Y[ins], marker=MK[geo], s=40, fc=c if slip_inf else "white", ec=c, lw=1.2, zorder=4, label=lab)
            ax.scatter(X[~ins], Y[~ins], marker=MK[geo], s=40, fc=c if slip_inf else "white", ec=c, lw=1.2, alpha=0.3, zorder=3)
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(lo, hi); ax.set_ylim(lo, hi)
ax.set_xlabel("| D - 1 |, incompressible (same Re, damping, walls, mesh)"); ax.set_ylabel("| D - 1 |, gated, swap the contacts")
ax.legend(loc="upper left", frameon=False, fontsize=8.4, ncol=1)
ax.text(0.99, 0.02, "filled: slip walls; open: no-slip; faint: above 0.3 v_F", transform=ax.transAxes, ha="right", fontsize=8.4, color=GREY)
ax = axs[2]; ax.set_title("(c)  Mesh refinement: the low-bias shape term is numerical", loc="left")
d4 = rows("D4")
for geo, c in (("funnel", BLUE), ("tesla_classic", GREEN)):
    for vv, ls, fill in ((0.01, "-", True), (0.03, (0, (4, 3)), False)):
        sub = sorted([r for r in d4 if r["geo"] == geo and abs(r["v_target"] - vv) < 1e-9], key=lambda r: r["hw"])
        if not sub: continue
        ax.plot([r["hw"] for r in sub], [abs(r["D_swap_dn"] - 1) for r in sub], color=c, ls=ls, marker=MK[geo], ms=6.5,
                mfc=c if fill else "white", mec=c, lw=1.4, label=f"{NAME[geo]}, swap, V / U_0 = {vv:g}")
sub = sorted([r for r in d4 if r["geo"] == "funnel" and abs(r["v_target"] - 0.03) < 1e-9], key=lambda r: r["hw"])
ax.plot([r["hw"] for r in sub], [abs(r["D_flip0"] - r["D_1d_flip"]) for r in sub], color=VERM, marker="o", ms=5, lw=1.2,
        label="funnel, flip minus 1-D closed form, 0.03")
h = np.array([9.0, 30.0]); ax.plot(h, 1.2e-7 * h**2, color=GREY, lw=1.0, ls=":"); ax.text(22, 1.2e-7 * 22**2 * 0.55, "h$^2$", color=GREY, fontsize=9)
ax.axhline(0.0528 * 0.03**2, color=BLUE, lw=0.8, ls=(0, (1, 2)))
ax.text(9.2, 0.0528 * 0.03**2 * 1.12, "inertial part at 0.03, mesh independent", color=BLUE, fontsize=8.2, va="bottom")
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(8.5, 32); ax.set_xticks([10, 14, 20, 28]); ax.set_xticklabels(["10", "14", "20", "28"]); ax.minorticks_off()
ax.set_xlabel("wall mesh size (nm)"); ax.set_ylabel("| D - 1 |"); ax.legend(loc="lower right", frameon=False, fontsize=8.4)
fig.savefig(f"{R}/H5_shape_vs_circuit.png", dpi=150)
print("wrote", f"{R}/H5_shape_vs_circuit.png")
