"""Figures for Stage B8 (results/bpic/B8_potential.png, B8_responsivity.png).
usage: .venv/bin/python tools/pic_plot.py"""
import math, os, sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
sys.path.insert(0, os.path.dirname(__file__))
import pic_common as c

BLUE, VERM, GREY, INK, PALE = "#0072B2", "#D55E00", "#6e6e6e", "#222222", "#a8a8a8"
COL = {300: BLUE, 30: VERM}
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False,
                     "axes.grid": True, "grid.color": "#e6e6e6", "grid.linewidth": 0.6, "legend.frameon": False})
fits = c.static_fits(); B7 = c.b7_g3(); dec = c.decision()
L, XF, ELL = c.L, c.XF, c.ELL


def outline(ax, **kw):
    xs = [0, 100, XF, XF, L, L, XF, XF, 100, 0, 0]; ys = [250, 250, 50, 250, 250, -250, -250, -50, -250, -250, 250]
    ax.plot(xs, ys, **kw)


# =============================== figure 1: potential ===============================
fig = plt.figure(figsize=(12.6, 7.6))
gs = fig.add_gridspec(2, 2, height_ratios=[1.0, 1.0], hspace=0.42, wspace=0.30, left=0.065, right=0.93, top=0.90, bottom=0.08)
cmap = LinearSegmentedColormap.from_list("pg", ["#1b7837", "#a6dba0", "#eeeeee", "#c2a5cf", "#762a83"])

for k, (d, lab) in enumerate(((300, "(a)"), (30, "(b)"))):
    ax = fig.add_subplot(gs[0, k]); ax.grid(False)
    x, y, m = c.load_map(f"{c.R}/P1_d{d}_V0.1.txt")
    u = m["mean"] / 0.1
    pm = ax.pcolormesh(x, y, u, cmap=cmap, vmin=-0.5, vmax=0.5, shading="nearest", rasterized=True)
    ax.contour(x, y, u, levels=np.arange(-0.4, 0.41, 0.1), colors=INK, linewidths=0.45, alpha=0.55)
    outline(ax, color=INK, lw=1.3)
    for x0 in (-200, L):
        ax.plot([x0, x0 + 200, x0 + 200, x0, x0], [-250, -250, 250, 250, -250], color=INK, lw=0.8, ls=":")
    ax.text(-100, 0, "source\nmetal", ha="center", va="center", fontsize=8.3, color="white")
    ax.text(L + 100, 0, "drain\nmetal", ha="center", va="center", fontsize=8.3, color="white")
    ax.set_xlim(-210, L + 210); ax.set_ylim(-262, 262); ax.set_aspect("equal")
    ax.set_title(f"{lab}  Static potential at 0.1 V, gate at {d} nm", loc="left")
    ax.set_xlabel("x (nm)"); ax.set_ylabel("y (nm)")
    cb = fig.colorbar(pm, ax=ax, fraction=0.035, pad=0.02, ticks=[-0.5, -0.25, 0, 0.25, 0.5])
    cb.set_label("potential energy / (e × bias)", fontsize=8.6); cb.outline.set_visible(False)

xr = np.linspace(-200, L + 200, 900)
ramp_u = np.clip(0.5 - xr / L, -0.5, 0.5); ramp_n = np.clip(0.5 - (xr - (XF - ELL / 2)) / ELL, -0.5, 0.5)


def dress(ax):
    ax.axvspan(-200, 0, color="#f1efe8", lw=0); ax.axvspan(L, L + 200, color="#f1efe8", lw=0)
    ax.axvline(XF, color=INK, lw=0.6); ax.text(XF - 6, 0.57, "neck", ha="right", va="top", fontsize=8.3, color=INK)
    ax.text(-100, 0.33, "source\nmetal", ha="center", va="center", fontsize=8.3, color=GREY)
    ax.text(L + 100, -0.28, "drain\nmetal", ha="center", va="center", fontsize=8.3, color=GREY)
    ax.set_xlim(-200, L + 200); ax.set_ylim(-0.62, 0.62); ax.set_xlabel("position along the axis (nm)")


ax = fig.add_subplot(gs[1, 0]); ax.set_title("(c)  Static potential along the axis", loc="left")
ax.plot(xr, ramp_u, color=GREY, lw=1.2, ls="--", label="imposed: uniform field (Stage B7)")
ax.plot(xr, ramp_n, color=GREY, lw=1.2, ls=":", label="imposed: drop at the neck (Stage B7)")
prof = {}
for d in c.GATES:
    for V, ls, lw, al in ((0.1, "-", 1.7, 1.0), (-0.1, "-", 0.9, 0.55)):
        x, y, m = c.load_map(f"{c.R}/P1_d{d}_V{V:g}.txt")
        u = c.axis(x, y, m["mean"]) / V; prof[(d, V)] = (x, u)
        ax.plot(x, u, color=COL[d], lw=lw, alpha=al, ls=ls, label=f"computed, gate at {d} nm, {'+' if V > 0 else '−'}0.1 V")
dress(ax); ax.set_ylabel("potential energy / (e × bias)")
ax.legend(fontsize=7.8, loc="lower left", handlelength=1.8)

ax = fig.add_subplot(gs[1, 1]); ax.set_title("(d)  Potential at 1 THz along the axis", loc="left")
ax.plot(xr, ramp_u, color=GREY, lw=1.2, ls="--", label="imposed: uniform field (Stage B7)")
ax.plot(xr, ramp_n, color=GREY, lw=1.2, ls=":", label="imposed: drop at the neck (Stage B7)")
for d in c.GATES:
    x, y, m = c.load_map(f"{c.R}/map_d{d}.txt")
    ax.plot(x, c.axis(x, y, m["cos"]) / 0.1, color=COL[d], lw=1.7, label=f"computed, gate at {d} nm, in phase")
    ax.plot(x, c.axis(x, y, m["sin"]) / 0.1, color=COL[d], lw=0.9, alpha=0.55, label=f"computed, gate at {d} nm, quadrature")
dress(ax); ax.set_ylabel("potential energy / (e × drive amplitude)")
ax.legend(fontsize=7.8, loc="lower left", handlelength=1.8)

fig.suptitle("Stage B8: the computed potential in the 0.55 µm funnel against the two profiles imposed in Stage B7\n"
             "mean free path 700 nm, shape-only bias; contours every tenth of the bias",
             x=0.07, ha="left", fontsize=11, y=0.985)
fig.savefig(f"{c.R}/B8_potential.png", dpi=170)
plt.close(fig)

# fractions of the bias dropped in each region (printed for the record)
for (d, V), (x, u) in sorted(prof.items()):
    at = lambda xx: float(np.interp(xx, x, u))
    print(f"gate {d:3d} V {V:+.1f}: source contact {0.5 - at(15):.3f}, lead and taper {at(15) - at(350):.3f}, "
          f"neck region 350-531 {at(350) - at(531):.3f}, drain contact {at(531) + 0.5:.3f}, at the neck {at(XF):+.3f}")

# =============================== figure 2: responsivity ===============================
fig = plt.figure(figsize=(12.6, 8.6))
gs = fig.add_gridspec(2, 2, hspace=0.36, wspace=0.22, left=0.065, right=0.98, top=0.90, bottom=0.07)


def detected(ax, f, v, e, **kw):
    """Points above two standard errors with bars; the others as two-sigma upper limits. For log axes."""
    f, v, e = map(np.asarray, (f, v, e)); ok = v > 2 * e
    mk = {k: kw[k] for k in ("color", "marker", "ms", "mfc", "mec") if k in kw}
    ax.errorbar(f[ok], v[ok], yerr=e[ok], capsize=2, lw=kw.get("lw", 1.4), ls=kw.get("ls", "-"), label=kw.get("label"), **mk)
    if (~ok).any():
        up = np.maximum(v[~ok], 0) + 2 * e[~ok]
        ax.errorbar(f[~ok], up, yerr=0.35 * up, uplims=True, fmt="none", ecolor=kw["color"], elinewidth=1.0, capsize=2.5, alpha=0.8)


# ---- (a) against Stage B7, common definition ----
ax = fig.add_subplot(gs[0, 0]); ax.set_title("(a)  Against the imposed-field model, shape-only drive", loc="left")
for fm, ls, lab in (("uniform", "--", "imposed: uniform field (Stage B7)"), ("local", ":", "imposed: drop at the neck (Stage B7)")):
    fit, ac = B7[fm]; fg = [c.b7.figures(r, fit) for r in ac]
    detected(ax, [q["f"] / 1e12 for q in fg], [q["R_abs"] for q in fg], [q["eR_abs"] for q in fg], color=GREY, marker="s", ms=4, ls=ls, lw=1.2, label=lab)
for d in c.GATES:
    rs = c.series("funnel", "sym", d); f = [r["f"] for r in rs]; v, e = [], []
    for r in rs:
        a, b = c.r_static(r, fits[d])
        if abs(r["f"] - 1) < 1e-6: a, b = 2 * dec[d]["I"] * 1e-6 / (c.VA ** 2 * fits[d]["Gqs"]), 2 * dec[d]["e"] * 1e-6 / (c.VA ** 2 * fits[d]["Gqs"])
        v.append(a); e.append(b)
    detected(ax, f, v, e, color=COL[d], marker="o", ms=5, label=f"computed field, gate at {d} nm")
    q = 2 * fits[d]["Iqs"] / (c.VA ** 2 * fits[d]["Gqs"])
    ax.plot([0.012, 0.03], [q, q], color=COL[d], lw=2.2, alpha=0.6)
ax.text(0.0125, 0.30, "from the static curve", fontsize=8.0, color=GREY, va="bottom")
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.008, 40); ax.set_ylim(0.004, 5)
ax.set_xlabel("frequency (THz)"); ax.set_ylabel("A/W per watt absorbed at the static conductance")
ax.legend(fontsize=8.0, loc="lower left"); ax.text(0.985, 0.97, "arrows: two-sigma upper limits", transform=ax.transAxes, ha="right", va="top", fontsize=8.0, color=GREY)

# ---- (b) per absorbed watt ----
ax = fig.add_subplot(gs[0, 1]); ax.set_title("(b)  Per absorbed watt, computed field", loc="left")
ax.axhspan(0.10, 0.35, color="#f1efe8", lw=0, zorder=0)
ax.text(0.092, 0.342, "plan: 0.10 to 0.35 A/W at 1 THz, 300 nm gate, shape-only drive = stands, weakened", fontsize=7.8, color=GREY, va="top")
for d in c.GATES:
    for mode, ls, mfc, lab in (("sym", "-", COL[d], "shape-only drive"), ("gated", "--", "white", "drain tied to the gate")):
        rs = c.series("funnel", mode, d); v, e = [], []
        for r in rs:
            a, b = r["I"] / r["P"], r["e"] / r["P"]
            if mode == "sym" and abs(r["f"] - 1) < 1e-6: a, b = dec[d]["R"], dec[d]["eR"]
            v.append(a); e.append(b)
        ax.errorbar([r["f"] for r in rs], v, yerr=e, color=COL[d], marker="o", ms=5, mfc=mfc, mec=COL[d], ls=ls, lw=1.4, capsize=2,
                    label=f"gate at {d} nm, {lab}")
ax.plot([1], [dec[300]["R"]], "o", ms=11, mfc="none", mec=INK, mew=1.0)
ax.annotate(f"decision point\n{dec[300]['R']:.3f} ± {dec[300]['eR']:.3f} A/W", (1, dec[300]["R"]), xytext=(0.36, 0.055), fontsize=8.3, color=INK,
            arrowprops=dict(arrowstyle="-", color=INK, lw=0.7))
ax.axhline(0, color=INK, lw=0.6)
ax.set_xscale("log"); ax.set_xlim(0.08, 8); ax.set_ylim(-0.14, 0.37)
ax.set_xticks([0.1, 0.3, 1, 3, 6]); ax.set_xticklabels(["0.1", "0.3", "1", "3", "6"]); ax.minorticks_off()
ax.set_xlabel("frequency (THz)"); ax.set_ylabel("rectified current per absorbed watt (A/W)")
ax.legend(fontsize=8.0, loc="lower left", ncol=1)

# ---- (c), (d) shape against circuit ----
chk = c.rows("checks.csv")
for k, d in enumerate(c.GATES):
    ax = fig.add_subplot(gs[1, k]); ax.set_title(f"({'cd'[k]})  Shape against circuit, gate at {d} nm", loc="left")
    ax.axhline(0, color=INK, lw=0.6)
    for geo, mode, col, mk, ls, mfc, lab in (("funnel", "sym", COL[d], "o", "-", COL[d], "funnel, shape-only drive"),
                                             ("funnel", "gated", COL[d], "o", "--", "white", "funnel, drain tied to the gate"),
                                             ("strip", "gated", INK, "s", "--", "white", "straight strip, drain tied to the gate"),
                                             ("symmetric", "sym", PALE, "D", "-", PALE, "mirror-symmetric control, shape-only drive")):
        rs = c.series(geo, mode, d); v = [r["I"] for r in rs]; e = [r["e"] for r in rs]
        if geo == "funnel" and mode == "sym":
            for i, r in enumerate(rs):
                if abs(r["f"] - 1) < 1e-6: v[i], e[i] = dec[d]["I"], dec[d]["e"]
        ax.errorbar([r["f"] for r in rs], v, yerr=e, color=col, marker=mk, ms=5, mfc=mfc, mec=col, ls=ls, lw=1.3, capsize=2, label=lab)
    for r in chk:
        if r["d"] == d:
            col = INK if r["geo"] == "strip" else COL[d]
            ax.plot([r["f"] * 1.07], [r["I"]], marker="x", ms=6, color=col, mew=1.3, ls="none",
                    label="repeat with a 10 ps settling time" if r["geo"] == "strip" else None)
    ax.set_xscale("log"); ax.set_xlim(0.08, 8); ax.set_ylim(-2.05, 1.6)
    ax.set_xticks([0.1, 0.3, 1, 3, 6]); ax.set_xticklabels(["0.1", "0.3", "1", "3", "6"]); ax.minorticks_off()
    ax.set_xlabel("frequency (THz)"); ax.set_ylabel("rectified current at 0.1 V drive (µA)")
    ax.legend(fontsize=8.0, loc="lower left")

fig.suptitle("Stage B8: rectification of the 0.55 µm funnel with the field computed, mean free path 700 nm, 0.1 V drive\n"
             f"1 THz, shape-only drive: {dec[300]['R']:.3f} ± {dec[300]['eR']:.3f} A/W per absorbed watt (gate at 300 nm), "
             f"{dec[30]['R']:.3f} ± {dec[30]['eR']:.3f} A/W (30 nm); imposed-field model: 0.70 A/W",
             x=0.065, ha="left", fontsize=11, y=0.985)
fig.savefig(f"{c.R}/B8_responsivity.png", dpi=170)

# =============================== figure 3: extension ===============================
keys = [k for k, *_ in c.CASES if all(c.ext_static(k, d) and c.ext_ac(k, d) for d in c.GATES)]
if len(keys) > 1:
    NAME = {"inf": "no\nscattering", "700": "700 nm", "100": "100 nm", "p05": "700 nm,\nhalf-diffuse\nwalls"}
    MK = {300: "o", 30: "s"}; OFF = {300: -0.09, 30: 0.09}
    B7M = {"uniform": ("^", -0.27, "imposed: uniform field (Stage B7)"), "local": ("D", 0.27, "imposed: drop at the neck (Stage B7)")}
    fig, axs = plt.subplots(1, 3, figsize=(12.6, 4.9))
    fig.subplots_adjust(left=0.06, right=0.985, top=0.74, bottom=0.2, wspace=0.27)
    xs = np.arange(len(keys))

    def cats(ax):
        ax.set_xticks(xs); ax.set_xticklabels([NAME[k] for k in keys]); ax.set_xlim(-0.55, len(keys) - 0.45)
        ax.grid(axis="x", visible=False); ax.set_xlabel("mean free path and walls")

    ax = axs[0]; ax.set_title("(a)  Static asymmetry at 0.1 V", loc="left")
    ax.axhline(1, color=INK, lw=0.7)
    for fm, (mk, off, lab) in B7M.items():
        ax.plot(xs + off, [c.b7_case(k)[fm]["A"] for k in keys], mk, color=GREY, mfc="white", ms=6.5, mew=1.2, ls="none", label=lab)
    for d in c.GATES:
        v = [c.ext_static(k, d) for k in keys]
        ax.errorbar(xs + OFF[d], [q["A"] for q in v], yerr=[q["eA"] for q in v], fmt=MK[d], color=COL[d], ms=6.5, capsize=0, lw=1.2, label=f"computed field, gate at {d} nm")
    cats(ax); ax.set_ylabel("I(+0.1 V) / |I(−0.1 V)|")

    ax = axs[1]; ax.set_title("(b)  Rectification at 1 THz, 0.1 V", loc="left")
    ax.axhspan(0.10, 0.35, color="#f1efe8", lw=0); ax.axhline(0, color=INK, lw=0.7)
    ax.text(len(keys) - 0.5, 0.345, "“stands, weakened”", ha="right", va="top", fontsize=8.3, color=GREY)
    for fm, (mk, off, lab) in B7M.items():
        ax.plot(xs + off, [c.b7_case(k)[fm]["Rs"] for k in keys], mk, color=GREY, mfc="white", ms=6.5, mew=1.2, ls="none")
    for d in c.GATES:
        v = [c.ext_ac(k, d) for k in keys]
        ax.errorbar(xs + OFF[d], [q["R"] for q in v], yerr=[q["eR"] for q in v], fmt=MK[d], color=COL[d], ms=6.5, capsize=0, lw=1.2)
    cats(ax); ax.set_ylabel("A/W per absorbed watt")
    ax.text(len(keys) - 0.5, 1.0, "grey: per watt absorbed at\nthe static conductance", ha="right", va="top", fontsize=8.3, color=GREY)

    ax = axs[2]; ax.set_title("(c)  Drive amplitude at 1 THz, 700 nm", loc="left")
    ax.axhline(1, color=INK, lw=0.7)
    for fm, (mk, off, lab) in B7M.items():
        pts = [q for q in c.b7_amp()[fm] if q[0] <= 0.2]; ref = [q for q in pts if q[0] == 0.1][0][1] / 0.01
        ax.plot([q[0] for q in pts], [q[1] / q[0] ** 2 / ref for q in pts], mk, color=GREY, mfc="white", ms=6.5, mew=1.2, ls="--", lw=1.0)
    for d in c.GATES:
        ref = c.ext_ac("700", d); pts = [c.ext_ac("700", d, V) for V in (0.05, 0.1, 0.2)]; pts = [q for q in pts if q]
        k0 = ref["I"] / 0.01
        ax.errorbar([q["V"] * (1 + 0.25 * OFF[d]) for q in pts], [q["I"] / q["V"] ** 2 / k0 for q in pts],
                    yerr=[math.hypot(q["e"] / q["V"] ** 2 / k0, 0 if q["V"] == 0.1 else 0) for q in pts], fmt=MK[d] + "-", color=COL[d], ms=6.5, capsize=0, lw=1.2)
    ax.set_xscale("log"); ax.set_xticks([0.05, 0.1, 0.2]); ax.set_xticklabels(["0.05", "0.1", "0.2"]); ax.minorticks_off(); ax.set_xlim(0.04, 0.25)
    ax.set_xlabel("drive amplitude (V)"); ax.set_ylabel("rectified current / amplitude², relative to 0.1 V")
    h, l = axs[0].get_legend_handles_labels(); order = [2, 3, 0, 1]
    fig.legend([h[i] for i in order], [l[i] for i in order], loc="upper center", ncol=4, fontsize=9, bbox_to_anchor=(0.5, 0.885), handletextpad=0.4, columnspacing=2.0)
    q1, qp = c.ext_ac("100", 300), c.ext_ac("p05", 300)
    fig.suptitle("Stage B8 extension: the 0.55 µm funnel with the field computed; mean free path, wall roughness, drive amplitude\n"
                 f"1 THz, gate at 300 nm: {q1['R']:.2f} A/W per absorbed watt at a 100 nm mean free path; "
                 + f"{qp['R']:+.2f}".replace("-", "−") + " A/W with half-diffuse walls",
                 x=0.06, ha="left", fontsize=11, y=0.985)
    fig.savefig(f"{c.R}/B8_extension.png", dpi=170)
    print("wrote B8_extension.png")
