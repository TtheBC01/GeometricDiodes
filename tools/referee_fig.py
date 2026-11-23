"""Results figure of Stage H8 (results/thz2/h8/H8_results.png): the three checks of plans/hydro_referee.md.
Reads results/thz2/h8 (tools/referee_runs.sh), results/thz2/fine and results/thz2.
usage: .venv/bin/python tools/referee_fig.py results/thz2/h8/H8_results.png
"""
import csv, math, os, sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

BLUE, VERM, GREEN, ORANGE, PURPLE, GREY, INK = "#0072B2", "#D55E00", "#009E73", "#E69F00", "#CC79A7", "#6e6e6e", "#222222"
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False,
                     "legend.frameon": False, "legend.fontsize": 8.6})
out = sys.argv[1] if len(sys.argv) > 1 else "results/thz2/h8/H8_results.png"
H, FINE = "results/thz2/h8", "results/thz2/fine"
KB_OVER_E = 8.617333e-5
LEADS = ((12000, BLUE, "o"), (6000, GREEN, "s"), (3000, ORANGE, "^"), (1000, VERM, "D"))


def load(d, name):
    p = f"{d}/{name}.csv"
    if not os.path.exists(p): return []
    return [{k: (v if k in ("geo", "set", "term") else float(v)) for k, v in r.items()} for r in csv.DictReader(open(p))
            if None not in r.values() and "" not in r.values()]


def fun(L, s, t):
    return load(FINE, f"S2_funnel_w1000_{s}_{t}") if L == 12000 else load(H, f"A_funnel_w1000_L{L}_{s}_{t}")


def col(rows, k): return np.array([r[k] for r in rows])


def fticks(ax):
    ax.set_xscale("log"); ax.set_xlim(0.095, 1.05)
    ax.set_xticks([0.1, 0.2, 0.3, 0.5, 1.0]); ax.set_xticklabels(["0.1", "0.2", "0.3", "0.5", "1"]); ax.minorticks_off()
    ax.set_xlabel("frequency (THz)")


fig, axs = plt.subplots(2, 3, figsize=(13.2, 8.2))

# ---------------------------------------------------------------- (a) share of the shape against frequency, 300 K
a = axs[0, 0]
share = {}
for s in ("300K", "100K"):
    for L, c, mk in LEADS:
        sy, ds = fun(L, s, "sym"), fun(L, s, "dss")
        if not (sy and ds): continue
        f = col(sy, "f_Hz") / 1e12; sh = np.abs(col(sy, "RI_A_per_W") / col(ds, "RI_A_per_W"))
        share[(s, L)] = sh[(f >= 0.27) & (f <= 1.05)]
        if s == "300K":
            k = (f >= 0.099) & (f <= 1.05)
            a.plot(f[k], sh[k], "-", color=c, lw=1.8, marker=mk, ms=4.5, label=f"{L / 1000:g} um leads")
a.set_yscale("log"); a.set_ylim(6e-4, 1.5); fticks(a)
a.set_ylabel("shape-only response over DS response")
a.set_title("(a) 300 K: the share of the shape grows\nas the leads are shortened", loc="left")
a.legend(loc="lower right", ncol=2)

# ---------------------------------------------------------------- (b) share against device length over decay length
b = axs[0, 1]
DEC = {"300K": 3.57, "100K": 17.85}
for s, c, dx in (("300K", VERM, 1.0), ("100K", BLUE, 1.0)):
    for L, _, mk in LEADS:
        sh = share.get((s, L))
        if sh is None: continue
        x = (2 * L + 1500 / math.tan(math.radians(30))) / 1000 / DEC[s]
        b.plot([x, x], [sh.min(), sh.max()], "-", color=c, lw=2.2, alpha=0.55, solid_capstyle="butt")
        b.plot(x, np.median(sh), "o", color=c, ms=7, mec="white", mew=1.0,
               label=f"{s[:-1]} K (decay length {DEC[s]:.3g} um)" if L == 12000 else None)
        side = s == "100K" and L == 12000        # this bar is short and next to the 300 K bar of the 1 um leads
        b.annotate(f"{L / 1000:g}", (x, sh.max()), textcoords="offset points", xytext=(9, -2) if side else (0, 4), ha="center", fontsize=8, color=c)
b.set_xscale("log"); b.set_yscale("log"); b.set_ylim(3e-3, 9); b.set_xlim(0.18, 11)
b.set_xticks([0.2, 0.5, 1, 2, 5, 10]); b.set_xticklabels(["0.2", "0.5", "1", "2", "5", "10"]); b.minorticks_off()
b.axhline(1, color=GREY, lw=0.8, ls=":")
b.set_xlabel("device length over the plasmon decay length")
b.set_ylabel("share of the shape, 0.3 to 1 THz")
b.set_title("(b) Median and range over six frequencies;\nlead length (um) above each bar", loc="left")
b.legend(loc="lower left")

# ---------------------------------------------------------------- (c) the control: DS funnel over plain channel, 300 K
c_ = axs[0, 2]
for L, c, mk in LEADS:
    ds = fun(L, "300K", "dss")
    ch = load(FINE, "C1_chan_w4000_300K_dss") if L == 12000 else load(H, f"A_chan_w4000_L{L}_300K_dss")
    if not (ds and ch): continue
    f = col(ds, "f_Hz") / 1e12; k = (f >= 0.099) & (f <= 1.05)
    c_.plot(f[k], col(ds, "RI_A_per_W")[k], "-", color=c, lw=1.8, marker=mk, ms=4.5, label=f"funnel, {L / 1000:g} um leads")
    c_.plot(f[k], col(ch, "RI_A_per_W")[k], "--", color=c, lw=1.3)
c_.plot([], [], "--", color=GREY, lw=1.3, label="plain channel of the same length")
fticks(c_); c_.set_ylim(0, 0.26)
c_.set_ylabel("DS current responsivity (A/W)")
c_.set_title("(c) 300 K, DS termination: the funnel\nagainst a plain channel", loc="left")
c_.legend(loc="upper left", ncol=2, fontsize=7.8, columnspacing=1.0)

# ---------------------------------------------------------------- (d) the valve against the funnel, shape only
d = axs[1, 0]
for s, ls, lw in (("100K", "-", 1.8), ("300K", "--", 1.4)):
    for name, L, c, mk, lab in (("tesla_classic_w1000", 12000, BLUE, "o", "valve, 12 um leads"), ("tesla_classic_w1000_L1000", 1000, VERM, "D", "valve, 1 um leads")):
        v = load(H, f"C_{name}_{s}_sym")
        if not v: continue
        f = col(v, "f_Hz") / 1e12; k = (f >= 0.099) & (f <= 1.05)
        d.plot(f[k], np.abs(col(v, "RI_A_per_W"))[k], ls, color=c, lw=lw, marker=mk, ms=4.2, label=lab if s == "100K" else None)
        fu = fun(L, s, "sym")
        d.plot(f[k], np.abs(col(fu, "RI_A_per_W"))[k], ls, color=c, lw=lw, marker=mk, ms=4.2, mfc="white", alpha=0.75,
               label=lab.replace("valve", "funnel") if s == "100K" else None)
d.plot([], [], "-", color=GREY, label="100 K"); d.plot([], [], "--", color=GREY, label="300 K")
d.set_yscale("log"); d.set_ylim(3e-7, 60); fticks(d)
d.set_ylabel("shape-only responsivity, magnitude (A/W)")
d.set_title("(d) The 1 um Tesla valve against the 1 um funnel,\nsymmetric drive", loc="left")
d.legend(loc="upper left", ncol=2, fontsize=7.8, columnspacing=1.0)

# ---------------------------------------------------------------- (e), (f) the ladders
e, f_ = axs[1, 1], axs[1, 2]
LAD = (("100K", "5e11", 5, "3e11", BLUE, "o"), ("100K", "2e11", 10, "3e11", GREEN, "s"), ("100K", "1e12", 5, "3e11", PURPLE, "^"),
       ("300K", "5e11", 5, "1e11", VERM, "D"), ("300K", "1e12", 5, "3e11", ORANGE, "v"))
for s, n, dd, f, c, mk in LAD:
    allrows = [r for r in load(H, f"B_{s}_n{n}_d{dd}_f{f}") if r["ok"]]
    rows = [r for r in allrows if r["tail"] < 0.5]          # a step whose period means had not settled is drawn hollow
    loose = [r for r in allrows if r["tail"] >= 0.5]
    ss = [r for r in load(FINE, f"D1_{s}_n{n}_d{dd}") if abs(r["f_Hz"] / float(f) - 1) < 1e-6]
    if not rows or not ss: continue
    ss = ss[0]; U0 = ss["U0_over_kT"] * KB_OVER_E * ss["T_K"]
    lab = f"{s[:-1]} K, {n} cm$^{{-2}}$, {dd} nm, {float(f) / 1e12:g} THz"
    u = col(rows, "umax_over_vF"); S = col(rows, "eta_av") / col(rows, "eta_small_bdf")
    e.plot(u, S, "-", color=c, lw=1.8, marker=mk, ms=5, label=lab)
    x = col(rows, "Va") * ss["Vunit_V"] / U0
    f_.plot(x, col(rows, "eta_av"), "-", color=c, lw=1.8, marker=mk, ms=5)
    f_.plot(x, col(rows, "eta_small_bdf"), ":", color=c, lw=1.2)
    if loose:
        e.plot(col(loose, "umax_over_vF"), col(loose, "eta_av") / col(loose, "eta_small_bdf"), mk, color=c, ms=6, mfc="white", mew=1.4)
        f_.plot(col(loose, "Va") * ss["Vunit_V"] / U0, col(loose, "eta_av"), mk, color=c, ms=6, mfc="white", mew=1.4)
    for ra, rb in zip(rows[:-1], rows[1:]):                # the time-domain run at 0.3 v_F, log-log between two steps
        if ra["umax_over_vF"] <= 0.3 <= rb["umax_over_vF"]:
            w = math.log(0.3 / ra["umax_over_vF"]) / math.log(rb["umax_over_vF"] / ra["umax_over_vF"])
            at = lambda k: math.exp(math.log(ra[k]) + w * math.log(rb[k] / ra[k]))
            f_.plot(at("Va") * ss["Vunit_V"] / U0, at("eta_av"), "o", color="white", ms=10, mec=INK, mew=1.6, zorder=5)
            f_.plot(at("Va") * ss["Vunit_V"] / U0, at("eta_av"), "o", color=c, ms=4.5, zorder=6)
    xl = ss["Va_at_0p3vF"] * ss["Vunit_V"] / U0
    el = ss["eta_per_W"] * (ss["Va_at_0p3vF"] * ss["Vunit_V"]) ** 2 / (8 * ss["Rant_ohm"])
    f_.plot(xl, el, marker="*", color=c, ms=13, mec=INK, mew=0.8, ls="none")
    if s == "100K" and n == "5e11":
        m4 = [r for r in load(H, "B_100K_n5e11_d5_f3e11_hw40") if r["ok"]]
        if m4:
            e.plot(col(m4, "umax_over_vF"), col(m4, "eta_av") / col(m4, "eta_small_bdf"), "x", color=INK, ms=8, mew=1.8, label="the first, 40 nm mesh")
            f_.plot(col(m4, "Va") * ss["Vunit_V"] / U0, col(m4, "eta_av"), "x", color=INK, ms=8, mew=1.8)
e.set_ylim(0.88, 2.75); e.plot([0.3, 0.3], [0.88, 1.6], color=GREY, lw=1.0, ls="--"); e.axhspan(0.9, 1.3, color="#f0f0f0", zorder=0)
e.text(0.305, 0.905, "drift limit", color=GREY, fontsize=8.5, va="bottom")
e.set_xlabel(r"largest drift speed over $v_F$"); e.set_ylabel("efficiency over its square law, S")
e.set_title("(e) Time domain at the thin-gate points:\ndeparture from the square law (band: the guess)", loc="left")
e.plot([], [], "o", color=GREY, mfc="white", mew=1.4, label="not settled after 12 periods")
e.legend(loc="upper left", fontsize=7.8)
f_.plot([], [], "o", color="white", mec=INK, mew=1.6, ms=9, ls="none", label="time domain at 0.3 $v_F$")
f_.plot([], [], marker="*", color="white", mec=INK, ms=11, ls="none", label="the paper's number (square law of\nthe small-signal solver at its limit)")
f_.plot([], [], ":", color=GREY, label="square law of the time-domain solver")
f_.set_yscale("log"); f_.set_xlabel(r"antenna amplitude over the gate overdrive $U_0$"); f_.set_ylabel("conversion efficiency")
f_.set_title("(f) The efficiency itself (colours as in e), against\nthe number the paper quotes at the limit", loc="left")
f_.legend(loc="lower right", fontsize=7.8)

fig.tight_layout(h_pad=1.8, w_pad=1.8)
fig.savefig(out, dpi=170)
print("wrote", out)
