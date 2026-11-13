"""Checkpoint figure for Stage B8 (plans/ballistic_selfconsistent_checkpoint.png):
convergence of the decision number of the self-consistent particle simulation
(G3, mean free path 700 nm, 1 THz, 0.1 V, shape-only drive) and the computed
potential along the device axis against the two imposed profiles of Stage B7.
Reads results/bpic/T30.csv, T30N.csv, maps.csv and map_d300.txt, map_d30.txt.
usage: .venv/bin/python tools/pic_checkpoint_fig.py plans/ballistic_selfconsistent_checkpoint.png
"""
import sys, csv, math, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

BLUE, VERM, GREY, INK = "#0072B2", "#D55E00", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False,
                     "axes.spines.right": False, "axes.grid": True, "grid.color": "#e6e6e6",
                     "grid.linewidth": 0.6})
R = "results/bpic"
COL = {300: BLUE, 30: VERM}


def rows(name):
    p = os.path.join(R, name)
    return list(csv.DictReader(open(p))) if os.path.exists(p) else []


allrows = rows("T30.csv") + rows("T30N.csv") + rows("maps.csv")
for r in allrows:
    r["d"] = int(float(r["d_nm"])); r["I"] = float(r["Idc_A"]) * 1e6; r["e"] = float(r["Idc_err_A"]) * 1e6
    r["Nn"] = int(r["N"]); r["P"] = float(r["Pabs_W"]) * 1e6


def is_base(r):   # default numerics; only N and seed differ
    return (abs(float(r["h_nm"]) - 5) < 0.1 and abs(float(r["dt_fs"]) - 2.5) < 1e-6 and float(r["filter"]) == 1.5
            and float(r["clen_nm"]) == 200 and int(r["cblock"]) == 2)


fig = plt.figure(figsize=(14.5, 4.9))
gs = fig.add_gridspec(1, 3, width_ratios=[1.0, 0.92, 1.25], wspace=0.36, left=0.045, right=0.985, top=0.83, bottom=0.13)

# ---- (a) particle number ---------------------------------------------------
ax = fig.add_subplot(gs[0, 0]); ax.set_title("(a)  Rectified current against particle number", loc="left")
summary = {}
for d, off in ((300, 0.96), (30, 1.04)):
    byN = {}
    for r in allrows:
        if r["d"] == d and is_base(r): byN.setdefault(r["Nn"], []).append(r)
    xs, ys, es = [], [], []
    for N in sorted(byN):
        w = np.array([1 / q["e"] ** 2 for q in byN[N]]); v = np.array([q["I"] for q in byN[N]])
        m = float((w * v).sum() / w.sum()); e = float(1 / math.sqrt(w.sum()))
        if len(v) > 1: e = max(e, float(np.std(v, ddof=1) / math.sqrt(len(v))))   # scatter between seeds, if larger
        xs.append(N * off / 1e6); ys.append(m); es.append(e)
        for q in byN[N]: ax.plot(N * off / 1e6, q["I"], "o", ms=3, color=COL[d], alpha=0.35, mec="none")
    ax.errorbar(xs, ys, yerr=es, fmt="o-", color=COL[d], ms=5.5, lw=1.4, capsize=2.5, label=f"gate at {d} nm")
    big = [q for N in byN if N >= 4000000 for q in byN[N]]
    w = np.array([1 / q["e"] ** 2 for q in big]); v = np.array([q["I"] for q in big])
    summary[d] = (float((w * v).sum() / w.sum()), float(1 / math.sqrt(w.sum())), float(np.mean([q["P"] for q in big])), len(big))
ax.set_xscale("log"); ax.set_xticks([1, 2, 4, 8, 16]); ax.set_xticklabels(["1", "2", "4", "8", "16"]); ax.minorticks_off()
ax.set_xlabel("sample particles (millions)"); ax.set_ylabel("rectified current (µA)")
ax.set_ylim(0, 0.8); ax.legend(frameon=False, loc="upper right", fontsize=8.6)
ax.text(0.03, 0.05, "pale dots: single runs; markers: mean per particle number\nimposed-field model of Stage B7: 3.0 µA (off scale)",
        transform=ax.transAxes, fontsize=8.3, color=GREY, va="bottom")

# ---- (b) other numerical parameters at 4 million -----------------------------
ax = fig.add_subplot(gs[0, 1]); ax.set_title("(b)  Other numerical settings, 4 million particles", loc="left")
VAR = [("base", "base settings"), ("seed2", "second seed"), ("map", "third seed"), ("h2.5", "grid 2.5 nm"),
       ("dt", "time step 1.25 fs"), ("filt0.75", "low-pass 0.75 cells"), ("filt3", "low-pass 3 cells"),
       ("clen100", "contacts 100 nm long"), ("clen400", "contacts 400 nm long"), ("cblock4", "contact blocks 4 nodes")]
for d, dy in ((300, 0.13), (30, -0.13)):
    m, e, _, _ = summary[d]
    ax.axvspan(m - e, m + e, color=COL[d], alpha=0.10, lw=0)
    for k, (key, lab) in enumerate(VAR):
        for r in allrows:
            if r["d"] == d and r["label"] == f"{key}_d{d}":
                ax.errorbar(r["I"], -k + dy, xerr=r["e"], fmt="o", color=COL[d], ms=4.5, capsize=2, lw=1.1)
ax.set_yticks([-k for k in range(len(VAR))]); ax.set_yticklabels([lab for _, lab in VAR], fontsize=8.6)
ax.set_xlim(0.2, 0.9); ax.set_ylim(-len(VAR) + 0.4, 0.6); ax.set_xlabel("rectified current (µA)"); ax.grid(axis="y", visible=False)
ax.text(0.995, 0.80, "base: 5 nm grid, 2.5 fs step,\nlow-pass 1.5 cells, contacts\n200 nm long in 2-node blocks\n\nbands: mean of the runs with\n4 million particles or more",
        transform=ax.transAxes, fontsize=8.0, color=GREY, ha="right", va="top", linespacing=1.25)

# ---- (c) potential along the axis --------------------------------------------
ax = fig.add_subplot(gs[0, 2]); ax.set_title("(c)  Potential at the drive frequency along the axis", loc="left")
L, xF, ell = 546.41, 446.41, 100.0
x = np.linspace(-200, L + 200, 800)
ax.plot(x, np.clip(0.5 - x / L, -0.5, 0.5), color=GREY, lw=1.2, ls="--", label="imposed: uniform field (Stage B7)")
ax.plot(x, np.clip(0.5 - (x - (xF - ell / 2)) / ell, -0.5, 0.5), color=GREY, lw=1.2, ls=":", label="imposed: drop at the neck (Stage B7)")
for d in (300, 30):
    p = os.path.join(R, f"map_d{d}.txt")
    if not os.path.exists(p): continue
    a = np.loadtxt(p)
    sel = np.abs(a[:, 1]) < 26
    xs = np.unique(a[sel, 0])
    uc = np.array([a[sel & (a[:, 0] == xv), 3].mean() for xv in xs]) / 0.1
    us = np.array([a[sel & (a[:, 0] == xv), 4].mean() for xv in xs]) / 0.1
    ax.plot(xs, uc, color=COL[d], lw=1.6, label=f"computed, gate at {d} nm, in phase")
    ax.plot(xs, us, color=COL[d], lw=1.0, alpha=0.6, label=f"computed, gate at {d} nm, quadrature")
ax.axvspan(-200, 0, color="#f1efe8", lw=0); ax.axvspan(L, L + 200, color="#f1efe8", lw=0)
ax.axvline(xF, color=INK, lw=0.6)
ax.text(-100, 0.30, "source\nmetal", ha="center", va="center", fontsize=8.3, color=GREY)
ax.text(L + 100, -0.25, "drain\nmetal", ha="center", va="center", fontsize=8.3, color=GREY)
ax.text(xF - 6, 0.56, "neck", ha="right", va="top", fontsize=8.3, color=INK)
ax.set_xlim(-200, L + 200); ax.set_ylim(-0.62, 0.62)
ax.set_xlabel("position along the device (nm)"); ax.set_ylabel("electron potential energy / (e x drive amplitude)")
ax.legend(frameon=False, fontsize=7.8, loc="lower left", bbox_to_anchor=(0.0, 0.0), handlelength=1.8)

s300, s30 = summary[300], summary[30]
fig.suptitle("Stage B8 checkpoint: self-consistent field, 0.55 µm funnel, 1 THz, 0.1 V, mean free path 700 nm, shape-only drive\n"
             f"gate at 300 nm: {s300[0]:.2f} ± {s300[1]:.2f} µA, {s300[0] / s300[2]:.2f} A/W per absorbed watt ({s300[3]} runs);   "
             f"gate at 30 nm: {s30[0]:.2f} ± {s30[1]:.2f} µA, {s30[0] / s30[2]:.2f} A/W ({s30[3]} runs)",
             x=0.05, ha="left", fontsize=11, y=0.985)
out = sys.argv[1] if len(sys.argv) > 1 else "plans/ballistic_selfconsistent_checkpoint.png"
fig.savefig(out, dpi=170)
for d in (300, 30): print(d, summary[d])
