"""Plan figure for Stage H7 (plans/hydro_closeout.png): what the runs on disk leave open in the fluid
model and which runs would close it. Reads results/thz2 only; prints the numbers quoted in the plan.
usage: .venv/bin/python tools/closeout_plan_fig.py plans/hydro_closeout.png
"""
import csv, sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

BLUE, VERM, GREEN, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "plans/hydro_closeout.png"
C, F = "results/thz2", "results/thz2/fine"


def load(p):
    return [{k: (v if k in ("geo", "set", "term") else float(v)) for k, v in r.items()} for r in csv.DictReader(open(p))]


fig, ax = plt.subplots(2, 2, figsize=(11.5, 8.6))

# ---------------------------------------------------------------- (a) the two solvers at 100 K
a = ax[0, 0]
ss = [r for r in load(f"{F}/S2_funnel_w1000_100K_dss.csv") if r["f_Hz"] <= 1.05e12]
a.plot([r["f_Hz"] / 1e12 for r in ss], [r["eta_per_W"] for r in ss], color=BLUE, lw=2)
td = []
for f in ("1e11", "3e11", "1e12"):
    r = load(f"{C}/P1_funnel_w1000_100K_dss_f{f}.csv")[0]
    td.append((r["f_Hz"] / 1e12, r["eta_small"] / r["Pav_W"]))
a.plot([t[0] for t in td], [t[1] for t in td], "s", color=VERM, ms=8, mfc="white", mew=2)
a.set_xscale("log"); a.set_yscale("log"); a.set_xlim(0.009, 1.3); a.set_ylim(0.05, 6)
a.set_xlabel("frequency (THz)"); a.set_ylabel(r"square-law efficiency per watt (W$^{-1}$)")
a.set_title("(a) Two solvers, 100 K: the level they disagree on", loc="left")
a.text(0.011, 3.4, "small-signal solver, 20 nm mesh\nviscosity falls with frequency", color=BLUE, va="top")
a.text(0.011, 0.34, "time-domain solver, 80 nm mesh\nconstant viscosity", color=VERM, va="top")
a.annotate("run R1: this curve with\nconstant viscosity, 20 nm mesh", xy=(0.6, 0.3), xytext=(0.035, 0.085), color=INK,
           arrowprops=dict(arrowstyle="->", color=GREY))
print("(a) small-signal eta/P at 0.1, 0.3, 1 THz:", [round(min(ss, key=lambda r: abs(np.log(r["f_Hz"] / f)))["eta_per_W"], 3) for f in (1e11, 3e11, 1e12)],
      "time domain:", [round(t[1], 3) for t in td])

# ---------------------------------------------------------------- (b) the amplitude ladders
b = ax[0, 1]
LAD = (("P1", "100K", "1e11", BLUE, "-", (0.07, 0.085)), ("P1", "100K", "3e11", BLUE, "--", ()), ("P1", "100K", "1e12", BLUE, ":", (0.15, 0.17)),
       ("P2", "300K", "1e11", VERM, "-", (0.18, 0.19)), ("P2", "300K", "3e11", VERM, "--", (0.17, 0.18)), ("P2", "300K", "1e12", VERM, ":", (0.165, 0.18)))
for pre, s, f, col, ls, new in LAD:
    rows = [r for r in load(f"{C}/{pre}_funnel_w1000_{s}_dss_f{f}.csv") if r["umax_over_vF"] > 0 and r["limit_hit"] < 0.5]
    va, um = [r["Va"] for r in rows], [r["umax_over_vF"] for r in rows]
    b.plot(va, um, ls, color=col, lw=1.3, marker="o", ms=4, label=f"{s[:-1]} K, {float(f) / 1e12:g} THz")
    if new: b.plot(new, np.interp(new, va, um), "D", color=col, ms=7, mfc="white", mew=1.6)
b.axhline(0.3, color=INK, lw=0.8); b.axhline(1.0, color=INK, lw=0.8)
b.text(0.0105, 0.33, "drift limit, 0.3 $v_F$", color=INK); b.text(0.0105, 1.08, "$v_F$", color=INK)
b.set_xscale("log"); b.set_yscale("log"); b.set_xlim(0.009, 0.9); b.set_ylim(0.01, 1.6)
b.set_xlabel(r"antenna amplitude $V_a$ (units of $V_u$)"); b.set_ylabel(r"largest drift speed / $v_F$")
b.set_title("(b) Amplitude ladders: points on disk and points to add", loc="left")
b.legend(frameon=False, fontsize=8.5, loc="lower right")
b.text(0.0, -0.2, "Open diamonds: run R4, two amplitudes on either side of the limit.\nRun R5: 0.3 THz at both temperatures on a 40 nm mesh.", color=INK, fontsize=8.5, va="top", transform=b.transAxes)

# ---------------------------------------------------------------- (c) the device and where the drift may peak
c = ax[1, 0]
L = [l.split() for l in open("meshes/h_funnel_w1000.poly") if l.strip()]
n = int(L[1][1]); p = np.array([(float(q[0]), float(q[1])) for q in L[2:2 + n]]) / 1e3
c.fill(p[:, 0], p[:, 1], color="#dfe9f3", ec=INK, lw=1.2)
for i in (2, 7):
    c.plot(*p[i], "o", color=VERM, ms=9, mfc="none", mew=1.8)
c.annotate("neck, 1 um wide:\ntwo re-entrant corners", xy=(14.75, 0.7), xytext=(15.6, 3.4), color=VERM, arrowprops=dict(arrowstyle="->", color=VERM))
c.text(1.0, 0, "source lead\n4 um wide", va="center"); c.text(19.5, 0, "drain lead", va="center")
c.plot([0.5, 5.5], [-3.2, -3.2], color=INK, lw=2); c.text(3.0, -3.9, "5 um", ha="center")
c.set_aspect("equal"); c.set_xlim(-1, 27.6); c.set_ylim(-5.2, 5.4); c.axis("off")
c.set_title("(c) Run R2: where the largest drift sits, and over what area", loc="left")
c.text(-0.5, -4.9, "The paper quotes one number per run, the largest drift anywhere.\nR2 writes its position and the share of the area above half of it,\non the 80, 40 and 20 nm meshes.", fontsize=8.5, va="top")

# ---------------------------------------------------------------- (d) the runs
d = ax[1, 1]; d.axis("off")
d.set_title("(d) The runs, in order, with machine time", loc="left")
rows = (("R0", "finite-temperature and adiabatic coefficients\nat every grid density (quadrature, no solver)", "minutes"),
        ("R1", "100 K sweeps with constant viscosity,\n1 um funnel, both terminations, 20 nm mesh", "35 min"),
        ("R2", "position and extent of the largest drift,\nthree frequencies, two temperatures, three meshes", "25 min"),
        ("R3", "finite-temperature coefficients at the\nlow-density grid points, 20 nm mesh", "20 min"),
        ("R4", "ten amplitudes around the drift limit,\n80 nm mesh (panel b)", "1 h"),
        ("R5", "large signal on the 40 nm mesh, 0.3 THz,\ntwo amplitudes per temperature", "2 to 3 h"))
y = 0.97
for tag, what, cost in rows:
    d.text(0.0, y, tag, weight="bold", color=BLUE, va="top", transform=d.transAxes)
    d.text(0.09, y, what, va="top", transform=d.transAxes)
    d.text(0.99, y, cost, va="top", ha="right", color=GREY, transform=d.transAxes)
    y -= 0.14
d.text(0.0, 0.02, "Not planned: the contact variant at large signal (it leaves the first-order\ndrift, and so the limit power, unchanged); 100 K above 1 THz (needs a\nmesh this machine cannot hold).",
       fontsize=8.5, va="bottom", transform=d.transAxes)

fig.tight_layout(h_pad=4.0, w_pad=2.0)
fig.savefig(out, dpi=170)
print("wrote", out)
