"""Results figure of Stage H7 (results/thz2/h7/H7_results.png): the 100 K response with and without the
viscoelastic term, where the largest drift sits, and the effect of isothermal coefficients.
usage: .venv/bin/python tools/closeout_results_fig.py results/thz2/h7/H7_results.png
"""
import csv, sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

BLUE, VERM, GREEN, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "results/thz2/h7/H7_results.png"
H, F, C = "results/thz2/h7", "results/thz2/fine", "results/thz2"


def load(p):
    return [{k: (v if k in ("geo", "set", "term") else float(v)) for k, v in r.items()} for r in csv.DictReader(open(p))]


fig = plt.figure(figsize=(11.5, 8.0))
gs = fig.add_gridspec(2, 2, height_ratios=(1.25, 1.0))

# ---------------------------------------------------------------- (a) R1
a = fig.add_subplot(gs[0, 0])
for t, ls in (("dss", "-"), ("sym", "--")):
    ve = [r for r in load(f"{F}/S2_funnel_w1000_100K_{t}.csv") if r["f_Hz"] <= 1.05e12]
    nc = [r for r in load(f"{H}/R1_funnel_w1000_100K_{t}_nuconst.csv") if r["f_Hz"] <= 1.05e12]
    a.plot([r["f_Hz"] / 1e12 for r in ve], [abs(r["RI_A_per_W"]) for r in ve], ls, color=BLUE, lw=1.8)
    a.plot([r["f_Hz"] / 1e12 for r in nc], [abs(r["RI_A_per_W"]) for r in nc], ls, color=VERM, lw=1.8)
x = load(f"{H}/R1x_funnel_w1000_100K_dss_nuconst.csv")
a.plot([r["f_Hz"] / 1e12 for r in x], [r["RI_A_per_W"] for r in x], "o", color=VERM, ms=6, mfc="white", mew=1.6)
a.set_xscale("log"); a.set_yscale("log"); a.set_xlim(0.009, 1.15); a.set_ylim(5e-4, 0.6)
a.set_xlabel("frequency (THz)"); a.set_ylabel("current responsivity (A/W)")
a.set_title("(a) 1 um funnel, 100 K: the viscoelastic term", loc="left")
a.text(0.0105, 0.33, "viscosity falls with frequency (paper)", color=BLUE)
a.text(0.0105, 0.075, "constant viscosity", color=VERM)
a.text(0.05, 6.5e-4, "solid: Dyakonov-Shur termination; dashed: shape only\ncircles: constant viscosity at 0.1, 0.3 and 1 THz exactly", fontsize=8.5, color=INK, va="bottom")

# ---------------------------------------------------------------- (b) R3
b = fig.add_subplot(gs[0, 1])
pts = {}
for ln in open(f"{H}/ft_coeffs.txt"):
    if ln.startswith("#") or not ln.strip(): continue
    s, n, d, cdiv, _ = ln.split()
    ft, ref = load(f"{H}/R3_FT_{s}_n{n}_d{d}.csv"), load(f"{F}/D1_{s}_n{n}_d{d}.csv")
    for i, f in enumerate((1e11, 3e11, 1e12)):
        u = [r for r in ft if abs(r["f_Hz"] / f - 1) < 1e-6][0]; v = [r for r in ref if abs(r["f_Hz"] / f - 1) < 1e-6][0]
        pts.setdefault((s, i), []).append((v["EF_over_kT"], u["RI_A_per_W"] / v["RI_A_per_W"]))
for s, f in (("300K", "FT_funnel_w1000_300K_dss"),):          # the reference point, run in Stage H6
    ft, ref = load(f"{F}/{f}.csv"), load(f"{F}/S2_funnel_w1000_300K_dss.csv")
    for i, fq in enumerate((1.0772173e11, 2.7863e11, 9.142e11)):
        u = min(ft, key=lambda r: abs(np.log(r["f_Hz"] / fq))); v = min(ref, key=lambda r: abs(np.log(r["f_Hz"] / fq)))
        pts.setdefault((s, i), []).append((4.5, u["RI_A_per_W"] / v["RI_A_per_W"]))
MK = ("o", "s", "^"); LB = ("0.1 THz", "0.3 THz", "1 THz")
for (s, i), v in sorted(pts.items()):
    col = BLUE if s == "100K" else VERM
    b.plot([p[0] for p in v], [p[1] for p in v], MK[i], color=col, ms=7, mfc="white" if i else col, mew=1.5, ls="none",
           label=f"{s[:-1]} K, {LB[i]}")
b.axhline(1.0, color=INK, lw=0.8)
b.set_xlabel(r"$E_F / k_B T$"); b.set_ylabel("isothermal over adiabatic response")
b.set_xlim(2.6, 6.6); b.set_ylim(0.95, 1.6)
b.set_title("(b) Coefficients of the gas at finite temperature", loc="left")
b.legend(frameon=False, fontsize=8.5, ncol=2, loc="upper right")
b.text(2.7, 0.965, "adiabatic flow keeps the zero-temperature coefficients", fontsize=8.5, color=INK)

# ---------------------------------------------------------------- (c) R2
c = fig.add_subplot(gs[1, :])
L = [l.split() for l in open("meshes/h_funnel_w1000.poly") if l.strip()]
n = int(L[1][1]); p = np.array([(float(q[0]), float(q[1])) for q in L[2:2 + n]]) / 1e3
c.fill(p[:, 0], p[:, 1], color="#e9eef4", ec=INK, lw=1.2)
c.plot([0, 0], [-2, 2], color=VERM, lw=6, solid_capstyle="butt")
c.annotate("300 K, all three frequencies:\nat the source contact, across the lead;\na fifth of the area is above half of it", xy=(0.1, 1.2), xytext=(1.3, 3.0), color=VERM,
           arrowprops=dict(arrowstyle="->", color=VERM), va="bottom")
r2 = load(f"{H}/R2_100K_dss_hw20.csv")
for r in r2:
    c.plot(r["u1max_x_nm"] / 1e3, r["u1max_y_nm"] / 1e3, "o", color=BLUE, ms=8, mfc="white", mew=1.8)
c.annotate("100 K, 0.1 and 0.3 THz:\non the axis in the neck;\n1 % of the area above half of it", xy=(14.6, 0.05), xytext=(16.4, 0.3), color=BLUE,
           arrowprops=dict(arrowstyle="->", color=BLUE), va="bottom")
c.annotate("100 K, 1 THz:\n100 nm from a corner", xy=(14.6, -0.42), xytext=(16.4, -1.8), color=BLUE, arrowprops=dict(arrowstyle="->", color=BLUE))
c.text(6.0, -0.2, "source lead", ha="center", color=GREY); c.text(23.5, -0.2, "drain lead", ha="center", color=GREY)
c.plot([0.5, 5.5], [-2.9, -2.9], color=INK, lw=2); c.text(3.0, -3.7, "5 um", ha="center")
c.set_aspect("equal"); c.set_xlim(-1, 27.6); c.set_ylim(-4.0, 5.6); c.axis("off")
c.set_title("(c) Where the largest first-order drift is (Dyakonov-Shur termination, 20 nm mesh)", loc="left")

fig.tight_layout(h_pad=1.5, w_pad=2.0)
fig.savefig(out, dpi=170)
print("wrote", out)
