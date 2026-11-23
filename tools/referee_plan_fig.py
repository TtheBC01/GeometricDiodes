"""Plan figure for Stage H8 (plans/hydro_referee.png): the three checks the referee read of the ninth
draft asked for (paper/review_ninth_draft.md, section 5, runs b, c and d). Reads results on disk only;
prints the numbers quoted in the plan.
usage: .venv/bin/python tools/referee_plan_fig.py plans/hydro_referee.png
"""
import csv, sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

BLUE, VERM, GREEN, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222"
FILL, FILL2 = "#e9eef4", "#c9d6e4"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
out = sys.argv[1] if len(sys.argv) > 1 else "plans/hydro_referee.png"
C, F = "results/thz2", "results/thz2/fine"


def load(p):
    return [{k: (v if k in ("geo", "set", "term") else float(v)) for k, v in r.items()} for r in csv.DictReader(open(p))]


fig = plt.figure(figsize=(11.5, 8.3))
gs = fig.add_gridspec(2, 2, height_ratios=(1.0, 0.66))

# ---------------------------------------------------------------- (a) the share of the shape at 300 K
a = fig.add_subplot(gs[0, 0])
SH = ((f"{F}/S2_funnel_w1000_300K", BLUE, "-", "1 um funnel, 12 um leads"),
      (f"{C}/S2_funnel_300K", GREEN, "--", "175 nm funnel, 6 um leads"),
      (f"{C}/S2_funnel_short_300K", VERM, "-", "175 nm funnel, 1 um leads"))
for stem, col, ls, lab in SH:
    s, d = load(stem + "_sym.csv"), load(stem + "_dss.csv")
    f = np.array([r["f_Hz"] / 1e12 for r in s]); sh = np.array([abs(u["RI_A_per_W"] / v["RI_A_per_W"]) for u, v in zip(s, d)])
    k = (f >= 0.09) & (f <= 1.05)
    a.plot(f[k], sh[k], ls, color=col, lw=2, label=lab)
    b3 = (f >= 0.27) & (f <= 1.05)
    print(f"(a) {lab}: share of the shape, 0.3 to 1 THz: {sh[b3].min():.3f} to {sh[b3].max():.3f}; at 0.914 THz {sh[np.argmin(abs(f - 0.914))]:.3f}")
a.set_xscale("log"); a.set_yscale("log"); a.set_xlim(0.09, 1.05); a.set_ylim(2e-3, 2.5)
a.set_xlabel("frequency (THz)"); a.set_ylabel("shape-only response over DS response")
a.set_title("(a) 300 K: is the small share of the shape the leads?", loc="left")
a.legend(frameon=False, fontsize=8.8, loc="lower left", bbox_to_anchor=(0.0, 0.0))
a.text(0.095, 1.45, "run A: the 1 um funnel with leads of 1, 3 and 6 um", color=INK, fontsize=9)
a.set_xticks([0.1, 0.2, 0.3, 0.5, 1.0]); a.set_xticklabels(["0.1", "0.2", "0.3", "0.5", "1"]); a.minorticks_off()

# ---------------------------------------------------------------- (b) the grid: efficiency at the limit
b = fig.add_subplot(gs[0, 1])
rows = []
on = False
for ln in open(f"{C}/paper_numbers.md"):
    if ln.startswith("## 7."): on = True; continue
    if on and ln.startswith("Over the valid"): break
    c = [x.strip() for x in ln.strip().strip("|").split("|")]
    if on and len(c) > 10 and c[0] in ("100K", "300K") and c[3] == "yes":
        rows.append((c[0], c[1], c[2], float(c[4]), float(c[9]), float(c[10])))   # set n d f eff amp/U0
for s, col in (("100K", BLUE), ("300K", VERM)):
    p = [r for r in rows if r[0] == s]
    b.plot([r[5] for r in p], [r[4] for r in p], "o", color=col, ms=4.5, alpha=0.55, mew=0, label=f"{s[:-1]} K, valid grid points")
HAVE = (("100K", "1e12", "30"), ("300K", "1e12", "30"), ("100K", "5e11", "10", 0.3), ("300K", "5e11", "10", 0.3))
for h in HAVE:
    p = [r for r in rows if r[:3] == h[:3] and (len(h) == 3 or r[3] == h[3])]
    b.plot([r[5] for r in p], [r[4] for r in p], "s", color=INK, ms=8, mfc="none", mew=1.4,
           label="time-domain ladder on disk" if h is HAVE[0] else None)
PLAN = (("100K", "5e11", "5", 0.3), ("100K", "2e11", "10", 0.3), ("100K", "1e12", "5", 0.3), ("300K", "5e11", "5", 0.1), ("300K", "1e12", "5", 0.3))
for i, h in enumerate(PLAN):
    r = [r for r in rows if r[:4] == h][0]
    b.plot(r[5], r[4], "o", color=BLUE if h[0] == "100K" else VERM, ms=13, mfc="none", mew=2.2, label="run B: planned ladder" if i == 0 else None)
    print(f"(b) planned: {h}: efficiency at the limit {r[4]:.3g}, amplitude over U_0 {r[5]:.3g}")
b.set_yscale("log"); b.set_xlim(0, 1.1); b.set_ylim(1.5e-5, 9e-4)
b.set_xlabel(r"antenna amplitude at the drift limit over the gate overdrive $U_0$")
b.set_ylabel("square-law efficiency at the drift limit")
b.set_title("(b) The top of the efficiency range is unchecked", loc="left")
b.legend(frameon=False, fontsize=8.8, loc="lower right")
b.axvspan(0.5, 1.1, color="#f3f3f3", zorder=0)
b.text(0.52, 7.0e-4, "no time-domain run here", color=GREY, fontsize=9, va="center")

# ---------------------------------------------------------------- (c) the devices, to scale
c = fig.add_subplot(gs[1, :])


def outline(path):
    L = [l.split() for l in open(path) if l.strip()]
    loops, i = [], 1
    while i < len(L):
        n = int(L[i][1]); loops.append(np.array([(float(q[0]), float(q[1])) for q in L[i + 1:i + 1 + n]]) / 1e3); i += 1 + n
    return loops


fun = outline("meshes/h_funnel_w1000.poly")[0]
TAPER = fun[:, 0].max() - 24.0        # length of the taper and neck, um
Y = (10.5, 4.5, -1.5, -7.5)
for L, y in zip((12.0, 6.0, 3.0, 1.0), Y):
    p = fun.copy()
    lo, hi = 12.0 - L, 12.0 + TAPER + L                      # cut the leads of the 12 um outline to L
    p[:, 0] = np.clip(p[:, 0], lo, hi) - lo
    new = L != 12.0
    c.fill(p[:, 0], p[:, 1] + y, color=FILL2 if new else FILL, ec=INK, lw=1.0)
    c.text(p[:, 0].max() + 0.8, y, f"{L:g} um leads" + ("" if new else ": on disk"), va="center", color=INK if new else GREY)
c.plot([0, 3.6], [-11.2, -11.2], color=VERM, lw=2.5, solid_capstyle="butt")
c.text(4.2, -11.2, "3.6 um: a plasmon's path in one momentum-relaxation time, 300 K", va="center", color=VERM, fontsize=9)
c.text(0, 14.3, "run A: the 1 um funnel with shorter leads", color=INK, fontsize=9.5, fontweight="bold")

X0 = 43.0
val = outline("meshes/tesla_classic_w1000_h.poly")
LV = max(l[:, 0].max() for l in val)
for y, cut, lab in ((7.0, 0.0, "12 um leads, as in the dc runs"), (-4.5, 11.0, "1 um leads")):
    for k, l in enumerate(val):
        p = l.copy(); p[:, 0] = np.clip(p[:, 0], cut, LV - cut) - cut + X0 + cut
        c.fill(p[:, 0], p[:, 1] + y, color=(FILL2 if cut else FILL) if k == 0 else "white", ec=INK, lw=0.8)
    c.text(X0 + LV / 2, y - 4.6, lab, ha="center", color=INK)
c.text(X0, 14.3, "run C: the Tesla valve with a 1 um channel, not yet run at terahertz frequencies", color=INK, fontsize=9.5, fontweight="bold")
c.plot([X0 + LV - 10, X0 + LV], [-11.2, -11.2], color=INK, lw=2); c.text(X0 + LV - 5, -12.6, "10 um", ha="center")
c.set_aspect("equal"); c.set_xlim(-1, X0 + LV + 1); c.set_ylim(-13.2, 15.6); c.axis("off")
c.set_title("(c) The devices of runs A and C, to one scale", loc="left")

# the valve results on disk (175 nm, 11 kilohm on 75 ohm), for the plan text
for s in ("100K", "300K"):
    sy, ds = load(f"{C}/S3_tesla_classic_{s}_sym.csv"), load(f"{C}/S3_tesla_classic_{s}_dss.csv")
    k = [i for i, r in enumerate(sy) if 0.27e12 <= r["f_Hz"] <= 1.05e12]
    print(f"(c) 175 nm valve, {s}: R_dc {ds[0]['Rdc_ohm']:.0f} ohm; shape only, 0.3 to 1 THz: {min(abs(sy[i]['RI_A_per_W']) for i in k):.2e} to "
          f"{max(abs(sy[i]['RI_A_per_W']) for i in k):.2e} A/W; DS {min(abs(ds[i]['RI_A_per_W']) for i in k):.3f} to {max(abs(ds[i]['RI_A_per_W']) for i in k):.3f}")
for s in ("100K", "300K"):
    sy = load(f"{F}/S2_funnel_w1000_{s}_sym.csv"); k = [r for r in sy if 0.27e12 <= r["f_Hz"] <= 1.05e12]
    print(f"(c) 1 um funnel, {s}: shape only, 0.3 to 1 THz: {min(abs(r['RI_A_per_W']) for r in k):.2e} to {max(abs(r['RI_A_per_W']) for r in k):.2e} A/W")

fig.tight_layout(h_pad=1.6, w_pad=2.0)
fig.savefig(out, dpi=170)
print("wrote", out)
