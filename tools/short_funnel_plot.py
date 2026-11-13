"""Stage B7 figures (results/bshort/): B7_transfer.png (clean-limit transfer on
the scale-free axis, all devices), B7_responsivity.png (matched-array
responsivity against frequency for the two short devices) and B7_length.png
(static asymmetry against lambda / L; responsivity at 1 THz against length).
usage: .venv/bin/python tools/short_funnel_plot.py"""
import sys, os, math
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
sys.path.insert(0, os.path.dirname(__file__))
from short_funnel_common import *

BLUE, VERM, GREEN, GREY, INK, PURP, YEL = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222", "#CC79A7", "#E69F00"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False,
                     "axes.grid": True, "grid.color": "#e6e6e6", "grid.linewidth": 0.6})
COL = {"G0": GREY, "G1": INK, "G2": BLUE, "G3": VERM, "G4": GREEN}
MK = {"G0": "o", "G1": "v", "G2": "D", "G3": "s", "G4": "^"}
FLAB = {"uniform": "uniform field", "local": "drop confined at the neck"}
PLASMONIC = 0.34
static, ac = load_all()

# ---- transfer function, clean limit ------------------------------------------
fig, axs = plt.subplots(1, 2, figsize=(13.5, 5.2), sharey=True, gridspec_kw=dict(wspace=0.06, left=0.06, right=0.985, top=0.92, bottom=0.12))
for ax, fm in zip(axs, FIELDS):
    for g in DEV:
        rows = ac.get((g, fm, "inf")); fit = static_fit(static.get((g, fm, "inf")))
        if not rows or not fit: continue
        o = [figures(r, fit) for r in rows]
        x = [r["f_Hz"] * LEN[g] * 1e-9 / VF for r in rows]
        ax.errorbar(x, [q["S"] for q in o], yerr=[q["eS"] for q in o], color=COL[g], marker=MK[g], ms=5, lw=1.4, capsize=2, label=f"{g}  {NAME[g]}")
    ax.axhline(0, color=INK, lw=0.7); ax.set_xscale("log"); ax.set_xlim(1e-3, 40); ax.set_ylim(-0.5, 1.6)
    ax.set_xlabel("reduced frequency  f L / v_F"); ax.set_title(f"({'ab'[FIELDS.index(fm)]})  No scattering, {FLAB[fm]}", loc="left")
axs[0].set_ylabel("rectified current / quasi-static value"); axs[0].legend(loc="lower left", frameon=False, fontsize=8.8)
fig.savefig(f"{R}/B7_transfer.png", dpi=150); plt.close(fig)

# ---- responsivity against frequency, G3 and G4 -------------------------------
LC = {"inf": INK, "700": VERM, "100": BLUE}
LL = {"inf": "no scattering", "700": "mean free path 700 nm", "100": "mean free path 100 nm"}
fig, axs = plt.subplots(2, 2, figsize=(13.5, 8.6), sharex=True, sharey=True, gridspec_kw=dict(wspace=0.06, hspace=0.16, left=0.065, right=0.985, top=0.94, bottom=0.08))
for i, g in enumerate(("G3", "G4")):
    for j, fm in enumerate(FIELDS):
        ax = axs[i, j]
        for lam in LAMS:
            rows = ac.get((g, fm, lam)); fit = static_fit(static.get((g, fm, lam)))
            if not rows or not fit: continue
            o = [figures(r, fit) for r in rows]; f = [r["f_Hz"] / 1e12 for r in rows]
            ax.errorbar(f, [q["R_abs"] for q in o], yerr=[q["eR_abs"] for q in o], color=LC[lam], marker="o", ms=4.5, lw=1.5, capsize=2, label=LL[lam])
            ax.plot(f, [q["R_single"] for q in o], color=LC[lam], lw=1.0, ls=(0, (2, 2)), alpha=0.8)
        ax.axhline(0, color=INK, lw=0.7)
        ax.axhline(PLASMONIC, color=GREEN, lw=1.1, ls=(0, (4, 3)))
        ax.axhspan(0.1, 1.0, color=GREEN, alpha=0.07, lw=0)
        ax.axvspan(10, 40, color=GREY, alpha=0.10, lw=0)
        ax.set_xscale("log"); ax.set_xlim(0.008, 40)
        ax.set_title(f"({'abcd'[2 * i + j]})  {g}, {NAME[g]}: {FLAB[fm]}", loc="left")
        if i == 1: ax.set_xlabel("frequency (THz)")
        if j == 0: ax.set_ylabel("responsivity (A/W)")
axs[0, 0].legend(loc="upper right", frameon=False, fontsize=8.8)
axs[0, 0].text(0.0085, -0.95, "solid: matched array (per absorbed watt)\ndotted: one device on a 75 ohm antenna\n"
               "dashed green: best plasmonic figure of the paper (100 K), 0.34 A/W\ngreen band: 0.1 to 1 A/W, the middle outcome of the plan",
               color=INK, fontsize=8.6, va="bottom")
axs[0, 1].text(11, axs[0, 1].get_ylim()[1] * 0.93, "indicative\nonly", color=GREY, fontsize=8.6, va="top")
fig.savefig(f"{R}/B7_responsivity.png", dpi=150); plt.close(fig)

# ---- asymmetry against lambda / L, and responsivity at 1 THz against length ---
fig, axs = plt.subplots(1, 2, figsize=(13.5, 5.2), gridspec_kw=dict(wspace=0.22, left=0.06, right=0.985, top=0.92, bottom=0.12))
ax = axs[0]; ax.set_title("(a)  Static asymmetry against mean free path / device length, uniform field", loc="left")
for g in DEV:
    pts = []
    for lam in LAMS:
        fit = static_fit(static.get((g, "uniform", lam)))
        if fit: pts.append((30.0 if lam == "inf" else float(lam) / LEN[g], fit["A"]))
    pts.sort()
    if pts: ax.plot([p[0] for p in pts], [p[1] for p in pts], color=COL[g], marker=MK[g], ms=6, lw=1.3, label=f"{g}  {NAME[g]}")
ax.axvspan(15, 60, color="#f3f3f3", lw=0, zorder=0); ax.text(29, 1.03, "no\nscattering", ha="center", fontsize=8.6, color=GREY)
ax.axhline(1, color=INK, lw=0.7); ax.set_xscale("log"); ax.set_xlim(0.01, 60); ax.set_xlabel("mean free path / device length"); ax.set_ylabel("A = I(+0.1 V) / |I(-0.1 V)|")
ax.legend(loc="upper left", frameon=False, fontsize=8.8)
ax = axs[1]; ax.set_title("(b)  Responsivity at 1 THz, mean free path 700 nm, against device length", loc="left")
for fm, c in (("uniform", BLUE), ("local", VERM)):
    for key, ls, lab in (("R_abs", "-", "matched array"), ("R_single", (0, (4, 3)), "one device at 75 ohm")):
        pts = []
        for g in DEV:
            rows = [r for r in ac.get((g, fm, "700"), []) if abs(r["f_Hz"] - 1e12) < 1]
            fit = static_fit(static.get((g, fm, "700")))
            if rows and fit:
                o = figures(rows[0], fit); pts.append((LEN[g] / 1e3, o[key], o["e" + key]))
        pts.sort()
        if pts: ax.errorbar([p[0] for p in pts], [p[1] for p in pts], yerr=[p[2] for p in pts], color=c, ls=ls, marker="o", ms=5, lw=1.4, capsize=2, label=f"{FLAB[fm]}, {lab}")
ax.axhline(PLASMONIC, color=GREEN, lw=1.1, ls=(0, (4, 3))); ax.text(0.47, PLASMONIC + 0.015, "best plasmonic figure (100 K)", color=GREEN, fontsize=8.6, ha="left", va="bottom")
ax.axhline(0, color=INK, lw=0.7); ax.set_xscale("log"); ax.set_xlabel("device length (um)"); ax.set_ylabel("responsivity at 1 THz (A/W)")
ax.set_xticks([0.5, 1, 2, 3, 6]); ax.set_xticklabels(["0.5", "1", "2", "3", "6"]); ax.minorticks_off(); ax.set_xlim(0.44, 7); ax.set_ylim(-0.08, 1.12)
ax.legend(loc="upper center", frameon=False, fontsize=8.6, ncol=2)
fig.savefig(f"{R}/B7_length.png", dpi=150); plt.close(fig)
print("wrote B7_transfer.png, B7_responsivity.png, B7_length.png")
