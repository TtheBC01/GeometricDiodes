"""Stage H6 result figure (results/thz2/H6_before_after.png): the fluid results with the momentum
equation used before and with the corrected one.
(a) uniform channel against the closed forms; (b) 1 um funnel and (c) 175 nm funnel, current
responsivity with the Dyakonov-Shur and the symmetric termination; (d) large signal: efficiency
against available power, with the drift speed.
usage: .venv/bin/python tools/momentum_results_fig.py results/thz2/H6_before_after.png
"""
import csv, math, os, sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker

BLUE, VERM, GREEN, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "results/thz2/H6_before_after.png"
OLD, NEW = "results/thz", "results/thz2"
plain = matplotlib.ticker.FuncFormatter(lambda v, _: f"{v:g}")


def load(path):
    if not os.path.exists(path): return []
    rows = []
    for r in csv.DictReader(open(path)):
        rows.append({k: (v if k in ("geo", "set", "term") else float(v)) for k, v in r.items() if k is not None and v != ""})
    return rows


def dV(Om, Gam, L, alpha, kappa, phi2, N=6001):
    k = np.sqrt(Om * (Om - 1j * Gam) + 0j); x = np.linspace(0, L, N)
    n1 = np.cos(k * (L - x)) / np.cos(k * L); u1 = -(k * np.sin(k * (L - x)) / np.cos(k * L)) / (1j * Om + Gam)
    return (phi2 - kappa) / 4 * (abs(n1[-1]) ** 2 - 1) + alpha / 4 * abs(u1[0]) ** 2 + Gam / 2 * np.trapezoid((n1 * np.conj(u1)).real, x)


fig = plt.figure(figsize=(15.5, 10.4))
gs = fig.add_gridspec(2, 2, left=0.06, right=0.945, top=0.955, bottom=0.075, wspace=0.2, hspace=0.34)

# ---------------------------------------------------------------- (a) channel
ax = fig.add_subplot(gs[0, 0]); ax.set_title("(a)  Uniform channel, 100 K: solver (symbols) and closed forms (lines)", loc="left")
S0, W, Q = 3.57e6, 1e-6, 0.5 / 3.57 ** 2
f = np.logspace(10, math.log10(6e11), 500); Om = 2 * np.pi * f * W / S0; Gam = W / (S0 * 5e-12)
for alpha, kap, ph, col, ls, lab in ((1.0, 0.0, 0.0, GREY, (0, (1.5, 1.5)), "classical gas"), (1.0, -(1 + Q) / 2, -Q / 2, VERM, "-", "graphene, before"),
                                     (0.0, -(1 + Q) / 2, -Q / 2, BLUE, "-", "graphene, now")):
    ax.plot(f / 1e12, [dV(o, Gam, 10.0, alpha, kap, ph) for o in Om], color=col, lw=1.5, ls=ls, label=lab)
for d, fn, col, mk in ((NEW, "S1_chan_100K_sw_ds", GREY, "s"), (OLD, "S1_chan_100K_inviscid_ds", VERM, "o"), (NEW, "S1_chan_100K_inviscid_ds", BLUE, "o")):
    r = [x for x in load(f"{d}/{fn}.csv") if x["f_Hz"] <= 6e11]
    ax.plot([x["f_Hz"] / 1e12 for x in r], [x["Voc_per_Va2"] for x in r], mk, color=col, ms=5, mfc="white", mew=1.3)
ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.01, 0.6); ax.set_ylim(3e-3, 60)
ax.set_xticks([0.01, 0.03, 0.1, 0.3]); ax.xaxis.set_major_formatter(plain); ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
ax.set_xlabel("frequency (THz)"); ax.set_ylabel("dc drain voltage per squared amplitude (1 / voltage unit)")
ax.legend(loc="upper left", frameon=False, fontsize=9, handlelength=2.8)


def device(ax, geo, title):
    ax.set_title(title, loc="left")
    for st, col in (("100K", BLUE), ("300K", VERM)):
        for term, lw in (("dss", 2.0), ("sym", 1.1)):
            for d, ls, alpha in ((OLD, (0, (4, 2)), 0.55), (NEW, "-", 1.0)):
                r = load(f"{d}/S2_{geo}_{st}_{term}.csv")
                if not r: continue
                x = np.array([q["f_Hz"] / 1e12 for q in r]); y = np.array([q["RI_A_per_W"] for q in r])
                ax.plot(x, np.abs(y), color=col, lw=lw, ls=ls, alpha=alpha)
                if d == NEW:
                    neg = y < 0
                    ax.plot(x[neg], np.abs(y[neg]), "o", color=col, ms=4, mfc="white", mew=1.0)
    ax.axhline(0.12, color=GREEN, lw=1.2, ls=(0, (4, 2)))
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.01, 3); ax.set_ylim(1e-5, 20)
    ax.text(0.011, 9.0, "thick: Dyakonov-Shur termination;  thin: symmetric termination (shape only)\nsolid: now;  dashed: before;  open circles: negative sign;  blue 100 K, orange 300 K",
            fontsize=8.6, color=INK, va="center")
    ax.text(0.011, 1.35, "green: ballistic short funnel, 0.12 A/W per absorbed watt", fontsize=8.6, color=GREEN, va="center")
    ax.set_xticks([0.01, 0.03, 0.1, 0.3, 1, 3]); ax.xaxis.set_major_formatter(plain); ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
    ax.set_xlabel("frequency (THz)"); ax.set_ylabel("current responsivity, absolute value (A/W)")


ax = fig.add_subplot(gs[0, 1]); device(ax, "funnel_w1000", "(b)  1 um funnel, 75 ohm antenna")
ax = fig.add_subplot(gs[1, 0]); device(ax, "funnel", "(c)  175 nm funnel, 75 ohm antenna")

# ---------------------------------------------------------------- (d) large signal
ax = fig.add_subplot(gs[1, 1]); ax.set_title("(d)  Large signal, 1 um funnel, 0.3 THz: efficiency and drift speed", loc="left")
ax2 = ax.twinx(); ax2.spines["top"].set_visible(False)
for st, run, col in (("100K", "P1", BLUE), ("300K", "P2", VERM)):
    for d, ls, alpha, mk in ((OLD, (0, (4, 2)), 0.55, None), (NEW, "-", 1.0, "o")):
        r = sorted([q for q in load(f"{d}/{run}_funnel_w1000_{st}_dss_f3e11.csv") if q.get("ok", 0) == 1], key=lambda q: q["Va"])
        if not r: continue
        ax.plot([q["Pav_W"] * 1e6 for q in r], [q["eta_av"] for q in r], color=col, lw=1.8, ls=ls, alpha=alpha, marker=mk, ms=5, mfc="white")
        if d == NEW and "umax_over_vF" in r[0]:
            ax2.plot([q["Pav_W"] * 1e6 for q in r], [q["umax_over_vF"] for q in r], color=col, lw=1.0, ls=(0, (1, 1.5)), marker="^", ms=4, mfc="white")
ax2.axhline(0.3, color=GREY, lw=0.8); ax2.axhline(1.0, color=GREY, lw=0.8)
ax.set_xscale("log"); ax.set_yscale("log"); ax2.set_yscale("log"); ax2.set_ylim(0.01, 300); ax.set_ylim(1e-7, 1e-1)
ax2.set_yticks([0.01, 0.1, 0.3, 1]); ax2.yaxis.set_major_formatter(plain); ax2.yaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
ax.set_xlabel("available power (uW)"); ax.set_ylabel("efficiency into a matched load"); ax2.set_ylabel("largest drift speed / Fermi velocity (triangles)")
ax.text(0.03, 0.97, "circles: now (exact Dirac gas; the run stops where the drift reaches $v_F$);  dashed: before\ntriangles, right axis: largest drift speed now;  grey lines: 0.3 $v_F$ and $v_F$\nblue 100 K, orange 300 K", transform=ax.transAxes, fontsize=8.6, color=INK, va="top")

fig.savefig(out, dpi=170)
print("wrote", out)
