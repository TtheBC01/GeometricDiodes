"""All figures of the paper, drawn from the result files of this repository.
usage: .venv/bin/python paper/make_figures.py [1 2 ... overview]     (no argument: all)
Outputs paper/figs/figN_*.pdf and .png. Style and colour conventions: paper/figstyle.py.
The file numbers are the order in which the figures were made. In the paper fig9 (steady
gated flow) is Fig. 4, fig10 (length scaling of the ballistic response) is Fig. 9, fig11 (the
short funnel with the field computed) is Fig. 10 and fig8 (synthesis) is Fig. 11.
"""
import sys, os, math, csv
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "tools"))
from figstyle import *
import matplotlib.tri as mtri
from matplotlib.lines import Line2D
from matplotlib.ticker import NullFormatter, LogLocator


# =============================================================================
# Figure 1: devices and characteristic frequencies
# =============================================================================
def fig1():
    fig = plt.figure(figsize=(COL2, 3.9))
    gs = fig.add_gridspec(2, 3, height_ratios=[0.62, 1.0], hspace=0.16, wspace=0.06)

    def scalebar(ax, x0, y0, length, label):
        ax.plot([x0, x0 + length], [y0, y0], color=BLACK, lw=1.2, solid_capstyle="butt")
        ax.text(x0 + length / 2, y0 - 0.06 * (ax.get_ylim()[1] - ax.get_ylim()[0]), label, ha="center", va="top", fontsize=6.8)

    # (a) funnel, Device D of Nguyen et al.
    ax = fig.add_subplot(gs[0, 0])
    loops, xn, L = funnel_loops()
    draw_domain(ax, loops)
    ax.plot([0, 0], [-1000, 1000], color=BLUE, lw=2.2, zorder=3); ax.plot([L, L], [-1000, 1000], color=BLUE, lw=2.2, zorder=3)
    ax.annotate("neck 175 nm", xy=(xn, -60), xytext=(xn + 900, -1650), fontsize=6.8, ha="center",
                arrowprops=dict(arrowstyle="-", lw=0.5, color=GREY))
    ax.text(1000, 0, "S", color=BLUE, fontsize=8, ha="center", va="center"); ax.text(L - 1100, 0, "D", color=BLUE, fontsize=8, ha="center", va="center")
    ax.annotate("", xy=(xn - 200, 1300), xytext=(700, 1300), arrowprops=dict(arrowstyle="->", lw=0.7, color=BLACK))
    ax.text(1900, 1380, "forward", fontsize=6.8, ha="center", va="bottom")
    ax.set_xlim(-300, 8200); ax.set_ylim(-2300, 1900); ax.set_aspect("equal"); ax.axis("off")
    scalebar(ax, 700, -1500, 1000, r"1 $\mu$m")
    ax.text(0.0, 1.0, "(a)  funnel diode", transform=ax.transAxes, fontsize=8, fontweight="bold", va="bottom")

    # (b) classical Tesla valve, same minimum feature
    ax = fig.add_subplot(gs[0, 1])
    tl = load_poly(os.path.join(ROOT, "meshes", "tesla_classic.poly"))
    draw_domain(ax, tl, fc="#fbe3d4")
    xs = np.concatenate([l[:, 0] for l in tl]); ys = np.concatenate([l[:, 1] for l in tl])
    ax.plot([xs.min()] * 2, [-1000, 1000], color=VERM, lw=2.2, zorder=3); ax.plot([xs.max()] * 2, [-1000, 1000], color=VERM, lw=2.2, zorder=3)
    ax.annotate("", xy=(xs.max() - 700, 1300), xytext=(700, 1300), arrowprops=dict(arrowstyle="->", lw=0.7, color=BLACK))
    ax.text(0.5 * xs.max(), 1380, "forward (unimpeded)", fontsize=6.8, ha="center", va="bottom")
    ax.set_xlim(-300, 8200); ax.set_ylim(-2300, 1900); ax.set_aspect("equal"); ax.axis("off")
    scalebar(ax, 3300, -1500, 1000, r"1 $\mu$m")
    ax.text(0.0, 1.0, "(b)  classical Tesla valve", transform=ax.transAxes, fontsize=8, fontweight="bold", va="bottom")

    # (c) gated 1 um funnel with its antenna port
    ax = fig.add_subplot(gs[0, 2])
    fl = load_poly(os.path.join(ROOT, "meshes", "h_funnel_w1000.poly"))
    xs = np.concatenate([l[:, 0] for l in fl])
    k = 8000.0 / (xs.max() - xs.min())
    draw_domain(ax, fl, scale=k)
    ax.plot([0, 0], [-2000 * k, 2000 * k], color=BLUE, lw=2.2, zorder=3); ax.plot([xs.max() * k] * 2, [-2000 * k, 2000 * k], color=BLUE, lw=2.2, zorder=3)
    ax.text(4000, -1250, r"gate at distance $d$ above", fontsize=6.5, ha="center", va="top")
    ax.text(0, 1150, r"$V_a\cos\omega t$ through $R_{\rm ant}$", fontsize=6.5, ha="left", va="bottom", color=BLUE)
    ax.text(xs.max() * k, 1150, "drain to gate", fontsize=6.5, ha="right", va="bottom", color=BLUE)
    ax.set_xlim(-300, 8300); ax.set_ylim(-2300, 1900); ax.set_aspect("equal"); ax.axis("off")
    scalebar(ax, 3000, -1900, 4000 * k, r"4 $\mu$m")
    ax.text(0.0, 1.0, r"(c)  gated 1 $\mu$m funnel", transform=ax.transAxes, fontsize=8, fontweight="bold", va="bottom")

    # (d) characteristic frequencies
    ax = fig.add_subplot(gs[1, :])
    lanes = ["plasmons", "collisions", "viscous flow", "ballistic flight"]
    ax.axvspan(3e11, 1e12, color="#e3f4ee", zorder=0)
    ax.text(5.5e11, 3.78, "band of interest", fontsize=6.5, ha="center", va="top", color=GREEN)
    for i, name in enumerate(lanes):
        ax.axhline(i, color="#d5d5d5", lw=0.6, zorder=1)
        ax.text(1.75e8, i, name, fontsize=7.5, va="center", ha="right", fontweight="bold")

    def tick(f, lane, text, color, dy=0.17, ha="center"):
        ax.plot([f], [lane], marker="|", color=color, ms=11, mew=1.6, ls="none", zorder=3)
        ax.text(f, lane + dy, text, fontsize=6.3, ha=ha, va="bottom" if dy > 0 else "top", color=color, linespacing=1.05)

    def band(f0, f1, lane, color, text, dy=0.17):
        ax.plot([f0, f1], [lane, lane], color=color, lw=5, alpha=0.4, solid_capstyle="butt", zorder=2)
        ax.text(math.sqrt(f0 * f1), lane + dy, text, fontsize=6.3, ha="center", va="bottom" if dy > 0 else "top", color=color, linespacing=1.05)

    band(1e9, 1e10, 3, PURPLE, "slow-drift electrons (flights of 20 to 100 ps)")
    tick(1 / 3.58e-12, 3, r"transit zero $1/\tau_{\rm tr}$, $\tau_{\rm tr}=3.6$ ps", PURPLE, ha="right")
    tick(1 / 0.5e-12, 3, r"$v_F/\ell_F$, 500 nm drop", PURPLE, dy=-0.17)
    band(1e10, 1e11, 2, VERM, r"viscous and convective rates of a 1 $\mu$m channel: steady-flow diodicity below")
    tick(1 / (2 * math.pi * 5e-12), 1, r"$1/2\pi\tau_{\rm mr}$, 100 K", GREY, ha="right")
    tick(1 / (2 * math.pi * 1e-12), 1, r"$1/2\pi\tau_{\rm mr}$ (300 K), $1/2\pi\tau_{ee}$ (100 K)", GREY, dy=-0.17)
    tick(1 / (2 * math.pi * 0.1e-12), 1, r"$1/2\pi\tau_{ee}$, 300 K", GREY)
    tick(1.42e11, 0, r"$s_0/2\pi W$, 1 $\mu$m funnel", BLUE, ha="right")
    tick(1.87e12, 0, "taper resonance,\n175 nm funnel", BLUE)
    ax.set_xscale("log"); ax.set_xlim(2e8, 5e12); ax.set_ylim(-0.45, 3.8)
    ax.set_yticks([]); ax.set_xlabel("frequency (Hz)")
    for sp in ["left", "right", "top"]: ax.spines[sp].set_visible(False)
    ax.tick_params(top=False, right=False, which="both")
    ax.text(-0.12, 1.0, "(d)  characteristic frequencies", transform=ax.transAxes, fontsize=8, fontweight="bold", va="bottom")
    save(fig, "fig1_devices")


# =============================================================================
# Figure 2: ballistic dc
# =============================================================================
def fig2():
    rows = load_csv(os.path.join(RES, "killtest.csv")) + [r for r in load_csv(os.path.join(RES, "killtest_classic.csv")) if r["geometry"] == "tesla_classic"]
    d = {}
    for r in rows: d[(r["geometry"], r["specularity"], r["lambda_nm"], r["V"])] = r
    combos = [(p, lam) for p in (1.0, 0.5, 0.0) for lam in (math.inf, 700.0, 200.0)]
    series = [("funnel_nguyenD", "funnel", BLUE, "o", 5),
              ("tesla_classic", "classical Tesla valve", VERM, "D", 4.5),
              ("tesla_1stage", "side-loop valve, 1 stage", ORANGES[0], "^", 4.5),
              ("tesla_3stage", "side-loop valve, 3 stages", ORANGES[0], "s", 4),
              ("tesla_3stage_v2", "side-loop valve, 3 stages (sharper)", ORANGES[0], "v", 4.5),
              ("straight_control", "straight control", GREY, "x", 4.5)]
    fig, axes = plt.subplots(1, 2, figsize=(COL2, 2.95), gridspec_kw=dict(wspace=0.24))
    x = np.arange(len(combos)); off = np.linspace(-0.3, 0.3, len(series))
    for (g, lab, c, m, ms), o in zip(series, off):
        A, eA, R = [], [], []
        for p, lam in combos:
            a, b = d.get((g, p, lam, 0.1)), d.get((g, p, lam, -0.1))
            if a is None or b is None: A.append(np.nan); eA.append(np.nan); R.append(np.nan); continue
            v = a["I_A"] / abs(b["I_A"]); A.append(v)
            eA.append(v * math.hypot(a["I_err_A"] / a["I_A"], b["I_err_A"] / b["I_A"])); R.append(0.1 / a["I_A"] * 1e-3)
        axes[0].errorbar(x + o, A, eA, color=c, marker=m, ms=ms, ls="none", lw=0.8, label=lab, mec="white" if m != "x" else c, mew=0.5 if m != "x" else 1.0, zorder=3 if g == "funnel_nguyenD" else 2)
        axes[1].plot(x + o, R, color=c, marker=m, ms=ms, ls="none", mec="white" if m != "x" else c, mew=0.5 if m != "x" else 1.0)
    lab = [("%g" % p, r"$\infty$" if math.isinf(lam) else "%g" % lam) for p, lam in combos]
    for ax in axes:
        ax.set_xticks(x); ax.set_xticklabels([f"{b}" for a, b in lab])
        for k in (2.5, 5.5): ax.axvline(k, color="#cccccc", lw=0.6)
        ax.tick_params(axis="x", which="both", top=False, length=0)
        ax.set_xlim(-0.6, 8.6)
        ax.set_xlabel(r"mean free path $\lambda$ (nm), by edge specularity $p$", labelpad=13)
        for k, p in zip((1, 4, 7), ("specular, $p=1$", "$p=0.5$", "diffuse, $p=0$")):
            ax.text(k, -0.095, p, transform=ax.get_xaxis_transform(), ha="center", va="top", fontsize=7)
    axes[0].axhline(1, color=GREY, lw=0.7, ls="--", zorder=1)
    axes[0].set_ylabel(r"asymmetry $A=I(+V)/|I(-V)|$ at 0.1 V"); axes[0].set_ylim(0.82, 1.72)
    axes[0].plot([-0.6, 0.3], [1.66, 1.66], color=GREY, lw=0.7, ls=":")
    axes[0].text(0.4, 1.66, "measured 1.66 (Nguyen et al., device D)", fontsize=6.5, va="center", color=GREY)
    hh, ll = axes[0].get_legend_handles_labels()
    fig.legend(hh, ll, loc="upper center", ncol=3, fontsize=6.8, bbox_to_anchor=(0.5, 1.03), handletextpad=0.2, columnspacing=1.4)
    axes[1].set_yscale("log"); axes[1].set_ylabel(r"forward resistance $V/I(+V)$ (k$\Omega$)"); axes[1].set_ylim(0.3, 40)
    panel(axes[0], "a", x=-0.16, y=1.02); panel(axes[1], "b", x=-0.16, y=1.02)
    save(fig, "fig2_ballistic_dc")


# =============================================================================
# Figure 3: viscous dc
# =============================================================================
def load_fld(path):
    with open(path) as f:
        nv, nt, scale = f.readline().split(); nv, nt = int(nv), int(nt)
        nodes = np.array([list(map(float, f.readline().split())) for _ in range(nv)])
        tris = np.array([list(map(int, f.readline().split())) for _ in range(nt)])
    return nodes, tris, float(scale)


def fig3():
    fig = plt.figure(figsize=(COL2, 5.0))
    gs = fig.add_gridspec(2, 2, height_ratios=[1.25, 1.0], hspace=0.30, wspace=0.26)
    Sls = {math.inf: "-", 0.5: "--", 0.0: ":", 0.1: "-."}
    Slab = {math.inf: r"perfect slip", 0.5: r"$S=0.5$", 0.0: "no slip", 0.1: r"$S=0.1$"}

    def curve(ax, rows, color, S, marker, label=None, lw=1.3):
        rows = sorted(rows, key=lambda r: r["Re"])
        Re = np.array([r["Re"] for r in rows]); D1 = np.array([r["D"] - 1 for r in rows])
        ax.plot(Re, np.abs(D1), color=color, ls=Sls[S], lw=lw, label=label)
        pos = D1 > 0
        ax.plot(Re[pos], np.abs(D1[pos]), color=color, marker=marker, ls="none", ms=4, mec="white", mew=0.4)
        ax.plot(Re[~pos], np.abs(D1[~pos]), color=color, marker=marker, ls="none", ms=4, mfc="white", mew=0.8)

    # (a) 175 nm devices, Da = 0
    ax = fig.add_subplot(gs[0, 0])
    cl = load_csv(os.path.join(RES, "hydro_S2_classic.csv")); s2 = load_csv(os.path.join(RES, "hydro_S2.csv"))
    for S in (math.inf, 0.5, 0.0):
        curve(ax, [r for r in cl if r["geometry"] == "tesla_classic" and r["Da"] == 0 and r["S"] == S], VERM, S, "D")
        curve(ax, [r for r in s2 if r["geometry"] == "funnel" and r["Da"] == 0 and r["S"] == S], BLUE, S, "o")
    ax.axhspan(1e-7, 3e-5, color=SHADE, zorder=0); ax.text(0.034, 5e-7, "discretisation floor", fontsize=6.3, color=GREY, va="center")
    ax.plot([0.04, 0.2], [0.05, 0.05], color=BLACK, lw=2.5, solid_capstyle="butt")
    ax.text(0.038, 0.075, "measured (Geurs et al.),\nRe estimated here", fontsize=6.3, va="bottom", ha="left")
    ax.axvspan(2.6, 60, color="#fff3e6", zorder=0)
    ax.text(2.9, 5e-7, r"$u>0.3\,v_F$ (175 nm)", fontsize=6.3, color=ORANGES[1], va="center")
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.03, 45); ax.set_ylim(3e-7, 4)
    ax.set_xlabel(r"Reynolds number ${\rm Re}=I/(ne\nu)$"); ax.set_ylabel(r"diodicity $|D-1|$")
    h = [Line2D([], [], color=VERM, marker="D", ms=4, label="classical Tesla valve (reverse harder)"),
         Line2D([], [], color=BLUE, marker="o", ms=4, mfc="white", label="funnel (forward harder)"),
         Line2D([], [], color=GREY, ls="-", label="perfect slip"), Line2D([], [], color=GREY, ls="--", label=r"slip length $0.5\,W$"),
         Line2D([], [], color=GREY, ls="-.", label=r"$0.1\,W$"), Line2D([], [], color=GREY, ls=":", label="no slip")]
    fig.legend(handles=h, loc="upper center", ncol=3, fontsize=6.6, bbox_to_anchor=(0.5, 0.985), columnspacing=1.6, handletextpad=0.4)
    panel(ax, "a")

    # (b) the valve scaled to a 1 um channel with damping and finite slip
    ax = fig.add_subplot(gs[0, 1])
    s4 = load_csv(os.path.join(RES, "hydro_S4_classic_w1000.csv"))
    for Da, c in zip((0, 8, 32), ORANGES):
        for S in (math.inf, 0.5, 0.1):
            curve(ax, [r for r in s4 if r["Da"] == Da and r["S"] == S], c, S, "D")
    ax.axhspan(1e-7, 3e-5, color=SHADE, zorder=0)
    ax.axvspan(3, 15, color="#fff3e6", zorder=0, alpha=0.6); ax.axvspan(15, 60, color="#fff3e6", zorder=0)
    ax.text(3.15, 5e-7, r"$u>0.3\,v_F$: $\nu=0.1$", fontsize=6.3, color=ORANGES[1], va="center")
    ax.text(15.6, 5e-7, r"$\nu=0.02$ m$^2$/s", fontsize=6.3, color=ORANGES[1], va="center")
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.7, 45); ax.set_ylim(3e-7, 4)
    ax.set_xlabel(r"Reynolds number"); ax.set_ylabel(r"diodicity $|D-1|$, 1 $\mu$m valve")
    h = [Line2D([], [], color=c, marker="D", ms=4, label=f"Da = {Da}") for Da, c in zip((0, 8, 32), ORANGES)]
    ax.legend(handles=h, loc="upper left", fontsize=6.3, title="damping", title_fontsize=6.3)
    panel(ax, "b")

    # (c), (d) speed fields of the 175 nm valve at Re 30, perfect slip
    smax = None; ims = []
    for j, (d, lab) in enumerate((("f", "forward"), ("r", "reverse"))):
        ax = fig.add_subplot(gs[1, j])
        nodes, tris, scale = load_fld(os.path.join(RES, f"hydro_tesla_classic_Re30_Sinf_{d}.fld"))
        x, y = nodes[:, 0] * scale / 1000, nodes[:, 1] * scale / 1000   # um
        sp = np.hypot(nodes[:, 2], nodes[:, 3])
        if smax is None:
            n2, _, _ = load_fld(os.path.join(RES, "hydro_tesla_classic_Re30_Sinf_r.fld")); smax = max(sp.max(), np.hypot(n2[:, 2], n2[:, 3]).max())
        tr = mtri.Triangulation(x, y, tris[:, :3])
        im = ax.tripcolor(tr, sp, shading="gouraud", cmap="cividis", vmin=0, vmax=smax, rasterized=True); ims.append(im)
        x0, x1 = 5.2, x.max() - 5.2
        ax.set_xlim(x0, x1); ax.set_ylim(-1.25, 1.25); ax.set_aspect("equal")
        ax.set_xticks([]); ax.set_yticks([])
        for s in ax.spines.values(): s.set_visible(False)
        ax.plot([x0 + 1.3, x0 + 2.3], [-1.05, -1.05], color=BLACK, lw=1.2); ax.text(x0 + 2.4, -1.05, r"1 $\mu$m", fontsize=6.5, ha="left", va="center")
        arrow = dict(arrowstyle="->", lw=0.8, color=BLACK)
        if d == "f": ax.annotate("", xy=(x0 + 2.0, 1.05), xytext=(x0 + 0.6, 1.05), arrowprops=arrow)
        else: ax.annotate("", xy=(x1 - 2.0, 1.05), xytext=(x1 - 0.6, 1.05), arrowprops=arrow)
        ax.text(0.5, 1.0, f"{lab} flow, Re = 30", transform=ax.transAxes, fontsize=7.5, ha="center", va="bottom")
        panel(ax, "cd"[j], x=-0.02, y=0.98)
    cax = fig.add_axes([0.36, 0.13, 0.3, 0.014])
    cb = fig.colorbar(ims[0], cax=cax, orientation="horizontal"); cb.set_label("speed / mean inlet speed", fontsize=7); cb.ax.tick_params(labelsize=6.5, top=False)
    save(fig, "fig3_viscous_dc")


# =============================================================================
# Figure 4: plasmonic small signal
# =============================================================================
def ds_closed_form(Om, Gam, L):
    wt = Om / Gam; r = np.sqrt(1 + 1 / wt ** 2)
    kp = Om * np.sqrt((r + 1) / 2); kpp = Om * np.sqrt((r - 1) / 2)
    beta = 2 * wt / np.sqrt(1 + wt ** 2)
    f = 1 + beta - (1 + beta * np.cos(2 * kp * L)) / (np.sinh(kpp * L) ** 2 + np.cos(kp * L) ** 2)
    return f / 4


def signed_loglog(ax, x, y, color, ls, marker="o", label=None, ms=3.2, lw=1.2, zorder=2):
    x = np.asarray(x); y = np.asarray(y)
    ax.plot(x, np.abs(y), color=color, ls=ls, lw=lw, label=label, zorder=zorder)
    pos = y > 0
    ax.plot(x[pos], np.abs(y[pos]), color=color, marker=marker, ls="none", ms=ms, mec="white", mew=0.3, zorder=zorder)
    ax.plot(x[~pos], np.abs(y[~pos]), color=color, marker=marker, ls="none", ms=ms, mfc="white", mew=0.7, zorder=zorder)


def fig4():
    T = os.path.join(RES, "thz")
    fig, axes = plt.subplots(2, 2, figsize=(COL2, 4.9), gridspec_kw=dict(hspace=0.30, wspace=0.26))
    # (a) validation on the uniform channel
    ax = axes[0, 0]
    sw = load_csv(f"{T}/S1_chan_100K_sw_ds.csv"); g1 = load_csv(f"{T}/S1_chan_100K_ds.csv"); g3 = load_csv(f"{T}/S1_chan_300K_ds.csv")
    f = np.array([r["f_Hz"] for r in sw]); Om = np.array([r["Omega"] for r in sw])
    ff = np.logspace(10, math.log10(1.3e12), 900); OO = ff * Om[0] / f[0]
    ax.plot(ff, ds_closed_form(OO, 0.056, 10.0), color=BLACK, lw=0.9, label="Dyakonov-Shur closed form")
    m = f <= 1.3e12
    ax.plot(f[m], np.array([abs(r["Voc_per_Va2"]) for r in sw])[m], color=GREY, marker="s", ms=3.2, ls="none", mec="white", mew=0.3, label="solver, shallow-water closure")
    ax.plot(f[m], np.array([abs(r["Voc_per_Va2"]) for r in g1])[m], color=BLUE, marker="o", ms=3.2, lw=1.0, mec="white", mew=0.3, label="solver, Dirac closure, 100 K")
    ax.plot(f[m], np.array([abs(r["Voc_per_Va2"]) for r in g3])[m], color=BLUE, marker="o", ms=3.2, lw=1.0, ls="--", mec="white", mew=0.3, label="solver, Dirac closure, 300 K")
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(8e9, 1.3e12); ax.set_ylim(5e-3, 300)
    ax.set_xlabel("frequency (Hz)"); ax.set_ylabel(r"dc drain voltage $\Delta U\,U_0/U_a^{2}$")
    ax.legend(loc="upper left", fontsize=6.3, borderaxespad=0.6)
    panel(ax, "a")

    def ri_panel(ax, stem, sets, entries, title, ylim=(3e-6, 4)):
        for term, color, lab in entries:
            for st, ls in sets:
                rows = load_csv(f"{T}/{stem}_{st}_{term}.csv")
                if not rows: continue
                signed_loglog(ax, [r["f_Hz"] for r in rows], [r["RI_A_per_W"] for r in rows], color, ls)
        ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(8e9, 3.6e12); ax.set_ylim(*ylim)
        ax.set_xlabel("frequency (Hz)"); ax.set_ylabel(r"responsivity $|R_I|$ (A/W)")
        ax.axvspan(3e11, 1e12, color="#e3f4ee", zorder=0)
        ax.text(0.03, 0.95, title, transform=ax.transAxes, fontsize=6.5, va="top")

    sets = [("100K", "-"), ("300K", "--")]
    ri_panel(axes[0, 1], "S2_funnel_w1000", sets, [("dss", BLUE, ""), ("sym", GREEN, "")], r"1 $\mu$m funnel, $R_{\rm dc}=168\ \Omega$ (100 K)")
    panel(axes[0, 1], "b")
    ri_panel(axes[1, 0], "S2_funnel", sets, [("dss", BLUE, ""), ("sym", GREEN, "")], r"175 nm funnel, $R_{\rm dc}=1.9$ k$\Omega$ (100 K)")
    axes[1, 0].annotate("taper\nresonance", xy=(1.87e12, 0.3), xytext=(6e11, 2.2e-4), fontsize=6.3, ha="center",
                        arrowprops=dict(arrowstyle="->", lw=0.5, color=GREY), color=GREY)
    panel(axes[1, 0], "c")
    ri_panel(axes[1, 1], "S3_tesla_classic", sets, [("dss", ORANGE, ""), ("sym", VERM, "")], r"classical Tesla valve, $R_{\rm dc}=10.9$ k$\Omega$ (100 K)")
    panel(axes[1, 1], "d")
    h = [Line2D([], [], color=BLUE, marker="o", ms=3.2, label="funnel, source through antenna, drain to gate (circuit asymmetry)"),
         Line2D([], [], color=GREEN, marker="o", ms=3.2, label="funnel, symmetric drive (shape only)"),
         Line2D([], [], color=ORANGE, marker="o", ms=3.2, label="Tesla valve, drain to gate"),
         Line2D([], [], color=VERM, marker="o", ms=3.2, label="Tesla valve, symmetric drive (shape only)"),
         Line2D([], [], color=GREY, ls="-", label="100 K"), Line2D([], [], color=GREY, ls="--", label="300 K"),
         Line2D([], [], color=GREY, marker="o", mfc="white", ls="none", ms=3.2, label="open symbol: negative sign")]
    fig.legend(handles=h, loc="upper center", ncol=2, fontsize=6.5, bbox_to_anchor=(0.5, 1.005), columnspacing=1.5, handletextpad=0.4)
    fig.subplots_adjust(top=0.865)
    save(fig, "fig4_plasmonic_small_signal")


# =============================================================================
# Figure 5: large signal
# =============================================================================
def fig5():
    T = os.path.join(RES, "thz"); VU = 1.487
    fig, axes = plt.subplots(1, 3, figsize=(COL2, 2.75), gridspec_kw=dict(wspace=0.46))
    fs = [("1e11", BLUES[0], "0.1 THz"), ("3e11", BLUES[1], "0.3 THz"), ("1e12", BLUES[2], "1 THz")]
    ax = axes[0]
    for st, run, ls in (("100K", "P1", "-"), ("300K", "P2", "--")):
        for f, c, lab in fs:
            rows = sorted(load_csv(f"{T}/{run}_funnel_w1000_{st}_dss_f{f}.csv"), key=lambda r: r["Va"])
            if not rows: continue
            P = np.array([r["Pav_W"] for r in rows]); eta = np.array([r["eta_av"] for r in rows])
            ax.plot(P, eta, color=c, ls=ls, marker="o", ms=3, mec="white", mew=0.3)
    P = np.logspace(-7.5, -2.3, 10)
    for k in (1.0, 2.4): ax.plot(P, k * P, color=GREY, lw=0.6, ls=":")
    ax.text(0.96, 0.05, "dotted: square laws\n" r"$\eta=1$ and $2.4\ {\rm W^{-1}}\times P_{\rm av}$", transform=ax.transAxes, fontsize=6.3, color=GREY, ha="right", va="bottom")
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(2e-7, 4e-3); ax.set_ylim(2e-7, 1e-2)
    ax.set_xlabel(r"available power $P_{\rm av}$ (W)"); ax.set_ylabel(r"conversion efficiency $\eta=P_{\rm dc}/P_{\rm av}$")
    panel(ax, "a", x=-0.22)
    ax = axes[1]
    for st, run, ls in (("100K", "P1", "-"), ("300K", "P2", "--")):
        for f, c, lab in fs:
            rows = sorted(load_csv(f"{T}/{run}_funnel_w1000_{st}_dss_f{f}.csv"), key=lambda r: r["Va"])
            if not rows: continue
            ax.plot([r["Va"] * VU for r in rows], [r["eta_av"] / r["eta_small_bdf"] for r in rows], color=c, ls=ls, marker="o", ms=3, mec="white", mew=0.3)
    rows = sorted(load_csv(f"{T}/P4_funnel_w1000_100K_sym_f3e11.csv"), key=lambda r: r["Va"])
    ax.plot([r["Va"] * VU for r in rows], [r["eta_av"] / r["eta_small_bdf"] for r in rows], color=GREEN, ls="-", marker="o", ms=3, mec="white", mew=0.3)
    ax.axhline(1, color=GREY, lw=0.6, ls=":")
    ax.text(0.2, 0.52, "shape only,\n0.3 THz", fontsize=6.3, color=GREEN, ha="right", va="center")
    ax.set_xscale("log"); ax.set_xlim(0.012, 1.3); ax.set_ylim(0.35, 1.2)
    ax.set_xlabel(r"drive amplitude $V_a$ (V)"); ax.set_ylabel(r"$\eta$ / square law")
    panel(ax, "b", x=-0.22)
    ax = axes[2]
    for f, c, lab in fs:
        rows = sorted(load_csv(f"{T}/P1_funnel_w1000_100K_dss_f{f}.csv"), key=lambda r: r["Va"])
        V = [r["Va"] * VU for r in rows]
        ax.plot(V, [r["n_max"] for r in rows], color=c, marker="o", ms=3, mec="white", mew=0.3)
        ax.plot(V, [r["n_min"] for r in rows], color=c, marker="o", ms=3, mec="white", mew=0.3)
    ax.axhline(1, color=GREY, lw=0.6, ls=":"); ax.axvline(1.43, color=GREY, lw=0.6, ls="--")
    ax.text(1.55, 0.06, "gate overdrive", fontsize=6.3, color=GREY, rotation=90, ha="left", va="bottom")
    ax.set_xscale("log"); ax.set_xlim(0.012, 2.4); ax.set_ylim(0, 1.8)
    ax.set_xlabel(r"drive amplitude $V_a$ (V)"); ax.set_ylabel(r"$n_{\min}/n_0$ and $n_{\max}/n_0$ (100 K)")
    panel(ax, "c", x=-0.22)
    h = [Line2D([], [], color=c, marker="o", ms=3, label=lab) for f, c, lab in fs] + [Line2D([], [], color=GREY, ls="-", label="100 K"), Line2D([], [], color=GREY, ls="--", label="300 K")]
    fig.legend(handles=h, loc="upper center", ncol=5, fontsize=6.6, bbox_to_anchor=(0.5, 1.04), columnspacing=1.6)
    save(fig, "fig5_large_signal")


# =============================================================================
# Figure 6: operating point and contacts
# =============================================================================
NS = [1e12, 5e11, 2e11, 1e11, 5e10]; DNM = [30, 10, 5]


def d1_row(prefix, st, n, d, f):
    tag = f"{n:.0e}".replace("e+", "e")
    rows = load_csv(os.path.join(RES, "thz", f"{prefix}_{st}_n{tag}_d{d}.csv"))
    rows = [r for r in rows if abs(math.log(r["f_Hz"] / f)) < 0.05]
    return rows[0] if rows else None


def fig6():
    fig, axes = plt.subplots(2, 2, figsize=(COL2, 4.9), gridspec_kw=dict(hspace=0.32, wspace=0.28))
    mk = {30: "o", 10: "s", 5: "^"}; col = dict(zip(DNM, BLUES[::-1]))
    valid = lambda r: r["valid_FL"] > 0.5 and r["valid_gate"] > 0.5
    # (a) saturation power against n d
    ax = axes[0, 0]
    for d in DNM:
        for n in NS:
            r = d1_row("D1", "100K", n, d, 3e11)
            if r is None: continue
            ok = valid(r)
            ax.plot(n * d * 1e-7, r["Pknee_W"], marker=mk[d], color=col[d], ms=4.5, ls="none", mfc=col[d] if ok else "white", mew=0.8)
    xnd = np.logspace(4.25, 6.6, 10)
    r0 = d1_row("D1", "100K", 1e12, 30, 3e11)
    ax.plot(xnd, r0["Pknee_W"] * (xnd / 3e6) ** 2, color=GREY, lw=0.6, ls=":", zorder=1)
    ax.set_xlim(1.5e4, 5e6); ax.set_ylim(5e-8, 3e-3)
    ax.set_xscale("log"); ax.set_yscale("log")
    ax.set_xlabel(r"$n\,d$ (cm$^{-1}$)"); ax.set_ylabel(r"saturation power $P_{\rm knee}$ (W)")
    ax.text(0.05, 0.92, r"$P_{\rm knee}\propto V_{\rm unit}^2\sim(nd)^2$", transform=ax.transAxes, fontsize=6.8, va="top")
    panel(ax, "a")
    # (b) peak efficiency against density
    ax = axes[0, 1]
    for st, ls in (("100K", "-"), ("300K", "--")):
        for d in DNM:
            xs, ys, oks = [], [], []
            for n in NS:
                r = d1_row("D1", st, n, d, 3e11)
                if r is None: continue
                xs.append(n); ys.append(r["eta_peak"]); oks.append(valid(r))
            xs, ys, oks = np.array(xs), np.array(ys), np.array(oks)
            ax.plot(xs, ys, color=col[d], ls=ls, lw=1.0)
            ax.plot(xs[oks], ys[oks], color=col[d], marker=mk[d], ms=4.5, ls="none", mec="white", mew=0.3)
            ax.plot(xs[~oks], ys[~oks], color=col[d], marker=mk[d], ms=4.5, ls="none", mfc="white", mew=0.8)
    ax.axhline(0.01, color=GREY, lw=0.6, ls=":"); ax.text(1.05e12, 0.0108, "1 %", fontsize=6.3, color=GREY, ha="right", va="bottom")
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_ylim(1e-4, 3e-2); ax.set_xlim(3.5e10, 1.5e12)
    ax.set_xlabel(r"carrier density $n$ (cm$^{-2}$)"); ax.set_ylabel(r"peak efficiency $\eta_{\rm peak}$, 0.3 THz")
    panel(ax, "b")
    # (c) contact resistance at the reference point
    ax = axes[1, 0]
    for st, ls in (("100K", "-"), ("300K", "--")):
        for f, c, lab in (("1e11", BLUES[0], ""), ("3e11", BLUES[1], ""), ("1e12", BLUES[2], "")):
            base = d1_row("D1", st, 1e12, 30, float(f))
            xs, ys = [0], [1.0]
            for Rc in (100, 300, 1000):
                rows = [r for r in load_csv(os.path.join(RES, "thz", f"D2_{st}_n1e12_d30_Rc{Rc}.csv")) if abs(math.log(r["f_Hz"] / float(f))) < 0.05]
                if rows: xs.append(Rc); ys.append(rows[0]["eta_per_W"] / base["eta_per_W"])
            ax.plot(xs, ys, color=c, ls=ls, marker="o", ms=3.5, mec="white", mew=0.3)
    xx = np.linspace(0, 1000, 100)
    ax.set_xscale("symlog", linthresh=100); ax.set_yscale("log"); ax.set_xlim(-8, 1300); ax.set_ylim(5e-3, 1.6)
    ax.set_xticks([0, 100, 300, 1000]); ax.set_xticklabels(["0", "100", "300", "1000"]); ax.xaxis.set_minor_formatter(NullFormatter())
    ax.set_xlabel(r"contact resistance $R_c$ ($\Omega\,\mu$m)"); ax.set_ylabel(r"$\eta/P_{\rm av}$ relative to ideal contacts")
    hf = [Line2D([], [], color=c, marker="o", ms=3.5, label=l) for c, l in zip(BLUES, ("0.1 THz", "0.3 THz", "1 THz"))]
    ax.legend(handles=hf, loc="lower left", fontsize=6.3)
    panel(ax, "c")
    # (d) detector NEP against density
    ax = axes[1, 1]
    for st, ls in (("100K", "-"), ("300K", "--")):
        for d in DNM:
            xs, ys, oks = [], [], []
            for n in NS:
                r = d1_row("D1", st, n, d, 3e11)
                if r is None: continue
                xs.append(n); ys.append(r["NEP_W_per_rtHz"]); oks.append(valid(r))
            xs, ys, oks = np.array(xs), np.array(ys), np.array(oks)
            ax.plot(xs, ys, color=col[d], ls=ls, lw=1.0)
            ax.plot(xs[oks], ys[oks], color=col[d], marker=mk[d], ms=4.5, ls="none", mec="white", mew=0.3)
            ax.plot(xs[~oks], ys[~oks], color=col[d], marker=mk[d], ms=4.5, ls="none", mfc="white", mew=0.8)
    xs, ys = [], []
    for n in NS:
        r = d1_row("D1t", "100K", n, 30, 3e11)
        if r: xs.append(n); ys.append(r["NEP_W_per_rtHz"])
    ax.plot(xs, ys, color=GREY, ls="-.", lw=1.0, marker="o", ms=3.5, mec="white", mew=0.3)
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(3.5e10, 1.5e12)
    ax.set_xlabel(r"carrier density $n$ (cm$^{-2}$)"); ax.set_ylabel(r"NEP at 0.3 THz (W/Hz$^{1/2}$)")
    panel(ax, "d")
    h = [Line2D([], [], color=col[d], marker=mk[d], ms=4.5, label=f"gate distance {d} nm") for d in DNM] + \
        [Line2D([], [], color=GREY, ls="-", label="100 K"), Line2D([], [], color=GREY, ls="--", label="300 K"),
         Line2D([], [], color=GREY, marker="o", mfc="white", ls="none", ms=4.5, label="open: outside the Fermi-liquid or gating limit"),
         Line2D([], [], color=GREY, ls="-.", marker="o", ms=3.5, label="constant mobility, 30 nm, 100 K (panel d)")]
    fig.legend(handles=h, loc="upper center", ncol=3, fontsize=6.6, bbox_to_anchor=(0.5, 1.02), columnspacing=1.6)
    fig.subplots_adjust(top=0.9)
    save(fig, "fig6_operating_point")


# =============================================================================
# Figure 7: ballistic funnel at THz
# =============================================================================
DPURPLE = "#5e2a84"


def fig7():
    import ballistic_thz_common as bc
    B = os.path.join(RES, "bthz")
    sc = bc.static_curves(bc.load(f"{B}/B1.csv")); geo = "funnel_nguyenD"
    b2 = {"uniform": bc.load(f"{B}/B2u.csv"), "local500": bc.load(f"{B}/B2l.csv")}; b3 = bc.load(f"{B}/B3.csv")
    fig, axes = plt.subplots(1, 3, figsize=(COL2, 2.85), gridspec_kw=dict(wspace=0.46))
    ax = axes[0]
    sty = {("uniform", "inf"): (PURPLE, "-", "o", "uniform, clean"), ("uniform", "700"): (PURPLE, "--", "o", r"uniform, $\lambda=700$ nm"),
           ("local500", "inf"): (DPURPLE, "-", "s", "500 nm drop, clean")}
    for (fk, lk), (c, ls, m, lab) in sty.items():
        rs = sorted([r for r in b2[fk] if bc.lam_key(r) == lk and abs(r["V_V"] - 0.1) < 1e-9], key=lambda r: r["f_Hz"])
        qs, eqs, _, _ = sc[(geo, fk, lk)].qs(0.1)
        S = np.array([r["Idc_A"] for r in rs]) / qs; e = np.array([math.hypot(r["Idc_err_A"] / qs, r["Idc_A"] * eqs / qs ** 2) for r in rs])
        ax.errorbar([r["f_Hz"] for r in rs], S, e, color=c, ls=ls, marker=m, ms=3.5, lw=1.2, mec="white", mew=0.3, label=lab)
    ff = np.logspace(8.3, 12.1, 300); x = np.pi * ff * 3.58e-12
    ax.plot(ff, np.sin(x) / x, color=GREY, lw=0.7, ls=":", label="transit factor")
    ax.axhline(0, color=GREY, lw=0.5); ax.axvspan(3e11, 1e12, color="#e3f4ee", zorder=0)
    ax.set_xscale("log"); ax.set_xlim(2e8, 1.5e12); ax.set_ylim(-0.95, 1.2)
    ax.set_xlabel("frequency (Hz)"); ax.set_ylabel(r"$I_{\rm dc}(f)$ / quasi-static value")
    ax.legend(loc="lower left", fontsize=6.0, handlelength=1.8, borderaxespad=0.3, labelspacing=0.3)
    panel(ax, "a", x=-0.22)
    ax = axes[1]
    for f0, m, c in ((3e11, "o", GREY), (1e12, "s", BLACK)):
        xs, ys, es = [], [], []
        for ell in (175, 500, 1600):
            rs = [r for r in b3 if abs(r["f_Hz"] - f0) < 1 and abs(r["ellF_nm"] - ell) < 1]
            if not rs: continue
            qs, eqs, _, _ = sc[(geo, f"local{ell}", "inf")].qs(0.1)
            xs.append(ell); ys.append(rs[0]["Idc_A"] / qs); es.append(math.hypot(rs[0]["Idc_err_A"] / qs, rs[0]["Idc_A"] * eqs / qs ** 2))
        ax.errorbar(xs, ys, es, color=c, marker=m, ms=4, lw=1.2, mec="white", mew=0.3, label=fmt_f(f0))
        ru = [r for r in b2["uniform"] if abs(r["f_Hz"] - f0) < 1 and bc.lam_key(r) == "inf" and abs(r["V_V"] - 0.1) < 1e-9]
        qs = sc[(geo, "uniform", "inf")].qs(0.1)[0]
        ax.errorbar([6000], [ru[0]["Idc_A"] / qs], [ru[0]["Idc_err_A"] / qs], color=c, marker=m, ms=4, mfc="white", mew=0.8, ls="none")
    ax.axhline(0, color=GREY, lw=0.5)
    ax.set_xscale("log"); ax.set_xlim(120, 9000); ax.set_ylim(-0.3, 1.2)
    ax.set_xticks([175, 500, 1600, 6000]); ax.set_xticklabels(["175", "500", "1600", "uniform"]); ax.xaxis.set_minor_formatter(NullFormatter())
    ax.tick_params(axis="x", which="minor", bottom=False, top=False)
    ax.set_xlabel(r"length of the potential drop $\ell_F$ (nm)"); ax.set_ylabel(r"$I_{\rm dc}$ / quasi-static value")
    ax.legend(loc="upper right", fontsize=6.3)
    panel(ax, "b", x=-0.22)
    ax = axes[2]
    rows = list(csv.DictReader(open(f"{B}/B1_thist_uniform_laminf.csv")))
    lo = np.array([float(r["t_lo_fs"]) for r in rows])
    for k, c, ls, lab in (("all_S", PURPLE, "-", "source-injected, all"), ("tx_S", PURPLE, "--", "source-injected, transmitted"),
                          ("all_D", GREY, "-", "drain-injected, all"), ("tx_D", GREY, "--", "drain-injected, transmitted")):
        cnt = np.array([float(r[k]) for r in rows]); tail = np.cumsum(cnt[::-1])[::-1] / cnt.sum()
        m = lo > 0
        ax.plot(lo[m] * 1e-3, tail[m], color=c, ls=ls, lw=1.2, label=lab)
    ax.axvline(3.58, color=GREY, lw=0.5, ls=":"); ax.text(4.3, 1.6e-5, "direct flight\nto the neck", fontsize=6.0, color=GREY, ha="left", va="bottom")
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(1, 1500); ax.set_ylim(1e-5, 300)
    ax.set_yticks([1e-5, 1e-4, 1e-3, 1e-2, 1e-1, 1])
    ax.set_xlabel("flight time $t$ (ps)"); ax.set_ylabel("fraction with flight time $>t$")
    ax.legend(loc="upper right", fontsize=5.8, handlelength=1.8, borderaxespad=0.3)
    panel(ax, "c", x=-0.22)
    save(fig, "fig7_ballistic_thz")


# =============================================================================
# Stage B8 data: the 0.55 um funnel with the computed field (tools/pic_common.py)
# =============================================================================
def pic():
    """tools/pic_common.py with the working directory at the repository root (its paths are relative)."""
    os.chdir(ROOT)
    import pic_common
    return pic_common


def pic_g3(d, mode="sym"):
    """Funnel G3, computed field, gate at d nm: one dict per frequency (THz) with I, e (uA), P (uW), Y, R (per
    absorbed watt), R1 (one device on 75 ohm) and NEP of the conjugately matched array (W/Hz^1/2, Johnson noise
    of the static conductance). The 1 THz shape-only point is the pooled decision value."""
    c = pic(); dec = c.decision()[d]; G = c.static_fits()[d]["Gqs"]; out = []
    for r in c.series("funnel", mode, d):
        q = dict(f=r["f"], I=r["I"], e=r["e"], P=r["P"], Y=r["Y"])
        if mode == "sym" and abs(r["f"] - 1) < 1e-6: q.update(I=dec["I"], e=dec["e"], P=dec["P"], Y=dec["Y"])
        Pav = (c.VA * abs(1 + c.RANT * q["Y"])) ** 2 / (8 * c.RANT) * 1e6
        q["R"] = q["I"] / q["P"]; q["eR"] = q["e"] / q["P"]; q["R1"] = q["I"] / Pav
        q["NEP"] = math.sqrt(4 * kB * 300.0 * G / (c.RANT * q["Y"].real)) / abs(q["R"]) if q["Y"].real > 0 else float("nan")
        out.append(q)
    return out


# =============================================================================
# Figure 8: synthesis
# =============================================================================
def fig8():
    import ballistic_thz_common as bc
    T = os.path.join(RES, "thz"); B = os.path.join(RES, "bthz")
    fig, axes = plt.subplots(1, 2, figsize=(COL2, 4.25), gridspec_kw=dict(wspace=0.27))
    fig.subplots_adjust(bottom=0.36)
    ax = axes[0]
    for st, ls in (("100K", "-"), ("300K", "--")):
        for term, c in (("dss", BLUE), ("sym", GREEN)):
            rows = load_csv(f"{T}/S2_funnel_w1000_{st}_{term}.csv")
            signed_loglog(ax, [r["f_Hz"] for r in rows], [r["RI_A_per_W"] for r in rows], c, ls, ms=2.4, lw=1.1)
        rows = load_csv(f"{T}/S3_tesla_classic_{st}_sym.csv")
        signed_loglog(ax, [r["f_Hz"] for r in rows], [r["RI_A_per_W"] for r in rows], VERM, ls, ms=2.4, lw=1.1)
    b2 = {"uniform": bc.load(f"{B}/B2u.csv"), "local500": bc.load(f"{B}/B2l.csv")}
    for fk, c, m in (("uniform", PURPLE, "o"), ("local500", DPURPLE, "s")):
        for lk, ls in (("inf", "-"), ("700", "--")):
            rs = sorted([r for r in b2[fk] if bc.lam_key(r) == lk and abs(r["V_V"] - 0.1) < 1e-9], key=lambda r: r["f_Hz"])
            f = np.array([r["f_Hz"] for r in rs]); RI = np.array([r["RI_A_per_W"] for r in rs]); e = np.array([r["Idc_err_A"] / r["Pav_W"] for r in rs])
            sig = np.abs(RI) > 2 * e          # points consistent with zero are not drawn
            signed_loglog(ax, f[sig], RI[sig], c, ls, marker=m, ms=3.2, lw=1.1)
    g3 = pic_g3(300)
    o = [q for q in g3 if abs(q["I"]) > 2 * q["e"]]      # the 0.55 um funnel with the computed field (Stage B8)
    signed_loglog(ax, [q["f"] * 1e12 for q in o], [q["R"] for q in o], BLACK, "--", marker="D", ms=3.4, lw=1.5, zorder=3)
    signed_loglog(ax, [q["f"] * 1e12 for q in o], [q["R1"] for q in o], BLACK, "--", marker="D", ms=2.2, lw=0.7, zorder=3)
    ax.axvspan(3e11, 1e12, color="#e3f4ee", zorder=0)
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(2e8, 3.6e12); ax.set_ylim(3e-6, 6)
    ax.set_xlabel("frequency (Hz)"); ax.set_ylabel(r"responsivity $|R_I|$ (A/W), 75 $\Omega$ source")
    h = [Line2D([], [], color=BLUE, marker="o", ms=3, label="plasmonic funnel, drain to gate"),
         Line2D([], [], color=GREEN, marker="o", ms=3, label="plasmonic funnel, shape only"),
         Line2D([], [], color=VERM, marker="o", ms=3, label="plasmonic Tesla valve, shape only"),
         Line2D([], [], color=PURPLE, marker="o", ms=3, label=r"ballistic funnel, 6 $\mu$m, uniform field"),
         Line2D([], [], color=DPURPLE, marker="s", ms=3, label=r"ballistic funnel, 6 $\mu$m, 500 nm drop at neck"),
         Line2D([], [], color=BLACK, ls="--", marker="D", ms=3.4, lw=1.5, label=r"ballistic funnel, 0.55 $\mu$m: per absorbed watt"),
         Line2D([], [], color=BLACK, ls="--", marker="D", ms=2.2, lw=0.7, label=r"ballistic funnel, 0.55 $\mu$m: one device"),
         Line2D([], [], color=GREY, ls="-", label="100 K, or clean"), Line2D([], [], color=GREY, ls="--", label=r"300 K, or $\lambda=700$ nm")]
    ax.legend(handles=h, loc="upper center", fontsize=6.2, ncol=1, handlelength=2.2, labelspacing=0.3, bbox_to_anchor=(0.5, -0.17))
    panel(ax, "a", x=-0.17)

    # (b) NEP: simulated (Johnson limited, ideal coupling) against measured detectors
    ax = axes[1]
    for st, Tk, ls in (("100K", 100.0, "-"), ("300K", 300.0, "--")):
        rows = load_csv(f"{T}/S2_funnel_w1000_{st}_dss.csv")
        f = np.array([r["f_Hz"] for r in rows]); nep = np.array([math.sqrt(4 * kB * Tk / r["Rdc_ohm"]) / abs(r["RI_A_per_W"]) for r in rows])
        m = (f >= 5e10) & (f <= 2.4e12)
        ax.plot(f[m], nep[m], color=BLUE, ls=ls, lw=1.2)
    xs, ys, ys2 = [], [], []
    for f0 in (1e11, 3e11, 1e12):
        r = d1_row("D1", "100K", 5e10, 30, f0)
        if r: xs.append(f0); ys.append(r["NEP_W_per_rtHz"])
    ax.plot(xs, ys, color=BLUE, ls=":", lw=1.2, marker="o", ms=3, mec="white", mew=0.3)
    xs, ys = [], []
    for st, ls2 in (("300K", "--"),):
        rows = load_csv(os.path.join(T, f"D2_{st}_n1e12_d30_Rc300.csv"))
        ax.plot([r["f_Hz"] for r in rows], [r["NEP_W_per_rtHz"] for r in rows], color=SKY, ls=ls2, lw=1.2, marker="o", ms=3, mec="white", mew=0.3)
    o = [q for q in g3 if q["I"] > 3 * q["e"]]
    ax.plot([q["f"] * 1e12 for q in o], [q["NEP"] for q in o], color=BLACK, ls="--", lw=1.2, marker="D", ms=3, mec="white", mew=0.3)
    lit = load_csv(os.path.join(ROOT, "paper", "literature_detectors.csv"), text=("label", "class", "note", "key"))
    cls = {"fet": ("D", BLACK, "measured: graphene FET / plasmonic"), "pte": ("^", ORANGE, "measured: photothermoelectric"),
           "ballistic": ("s", PURPLE, "measured: ballistic rectifier / geometric diode"), "incumbent": ("x", GREY, "commercial room-temperature detectors")}
    seen = set()
    for r in lit:
        m, c, lab = cls.get(r["class"], ("o", GREY, r["class"]))
        cold = r["T_K"] < 200
        ax.plot(r["f_Hz"], r["NEP_W_rtHz"], marker=m, color=c, ms=4.6, ls="none", mfc="white" if cold else c, mew=0.9, zorder=4)
        seen.add(r["class"])
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(5e10, 6e12)
    ax.set_ylim(1e-13, max(3e-9, 3 * max([r["NEP_W_rtHz"] for r in lit] + [1e-9])))
    ax.set_xlabel("frequency (Hz)"); ax.set_ylabel(r"NEP (W/Hz$^{1/2}$)")
    h = [Line2D([], [], color=BLUE, ls="-", label="simulated plasmonic funnel, 100 K"), Line2D([], [], color=BLUE, ls="--", label="simulated plasmonic funnel, 300 K"),
         Line2D([], [], color=SKY, ls="--", marker="o", ms=3, label=r"300 K with 300 $\Omega\,\mu$m contacts"),
         Line2D([], [], color=BLUE, ls=":", marker="o", ms=3, label=r"100 K, $5\times10^{10}$ cm$^{-2}$"),
         Line2D([], [], color=BLACK, ls="--", marker="D", ms=3, label=r"simulated 0.55 $\mu$m ballistic funnel array, 300 K")]
    h += [Line2D([], [], color=c, marker=m, ms=4.6, ls="none", label=lab) for k, (m, c, lab) in cls.items() if k in seen]
    if lit: h += [Line2D([], [], color=GREY, marker="D", mfc="white", ms=4.6, ls="none", label="open symbol: measured below 200 K")]
    ax.legend(handles=h, loc="upper center", fontsize=6.2, handlelength=2.2, labelspacing=0.3, bbox_to_anchor=(0.5, -0.17))
    panel(ax, "b", x=-0.17)
    save(fig, "fig8_synthesis")


# =============================================================================
# Figure 9 (Fig. 4 of the paper): steady flow with the gate
# =============================================================================
def fig9():
    D = os.path.join(RES, "dcgate")
    def rows(fn):
        out = []
        for r in csv.DictReader(open(os.path.join(D, fn + ".csv"))):
            out.append({k: (v if k in ("geo", "set", "closure") else float(v)) for k, v in r.items()})
        return out
    allr = rows("D1") + rows("D2") + rows("D3"); d4 = rows("D4")
    refd = {(r["geo"], r["set"], r["slip"], r["hw"], round(r["Re"], 6)): r["D_inc"] for r in rows("ref")}
    ref_of = lambda r: refd.get((r["geo"], r["set"], r["slip"], r["hw"], round(r["Re"], 6)), float("nan"))
    MK = {"chan": "x", "symcon": "+", "funnel": "o", "funnel_w1000": "s", "tesla_classic": "^", "tesla_classic_w1000": "v"}
    NAME = {"chan": "straight channel", "symcon": "symmetric constriction", "funnel": r"funnel, 175 nm", "funnel_w1000": r"funnel, 1 $\mu$m",
            "tesla_classic": "Tesla valve, 175 nm", "tesla_classic_w1000": r"Tesla valve, 1 $\mu$m"}
    CF = {"chan": GREY, "symcon": GREY, "funnel": BLUE, "funnel_w1000": BLUE, "tesla_classic": VERM, "tesla_classic_w1000": VERM}   # flip: circuit
    CS = {"symcon": GREY, "funnel": GREEN, "funnel_w1000": GREEN, "tesla_classic": VERM, "tesla_classic_w1000": VERM}              # swap: shape
    fig, axes = plt.subplots(1, 3, figsize=(COL2, 3.0), gridspec_kw=dict(wspace=0.50))
    fig.subplots_adjust(bottom=0.385)

    def pts(ax, x, y, inside, marker, color, filled, ms=3.6):
        line = marker in "x+"
        for sel, a in ((inside, 1.0), (~inside, 0.28)):
            if not sel.any(): continue
            ax.plot(x[sel], y[sel], ls="none", marker=marker, ms=ms + (1.0 if line else 0), color=color, mfc=color if (filled and not line) else "white",
                    mew=0.8, alpha=a, zorder=4)

    ax = axes[0]
    sel = [r for r in allr if r["set"] == "300K" and math.isinf(r["slip"])]
    one = sorted([r for r in sel if r["geo"] == "chan"], key=lambda r: r["v_target"])
    ax.plot([r["v_target"] for r in one], [r["D_1d_flip"] - 1 for r in one], color=BLACK, lw=0.9, ls=(0, (4, 2)), zorder=2)
    for geo in MK:
        sub = sorted([r for r in sel if r["geo"] == geo], key=lambda r: r["v_target"])
        x = np.array([r["v_target"] for r in sub]); inside = np.array([r["umax_vF"] <= 0.3 for r in sub])
        pts(ax, x, np.array([abs(r["D_flip0"] - 1) for r in sub]), inside, MK[geo], CF[geo], True)
        if geo == "chan": continue
        ys = np.array([abs(r["D_swap_dn"] - 1) for r in sub])
        ax.plot(x, ys, color=CS[geo], lw=0.6, alpha=0.6, zorder=3)
        pts(ax, x, ys, inside, MK[geo], CS[geo], False)
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(7e-4, 0.42); ax.set_ylim(5e-9, 1.5)
    ax.set_xlabel(r"bias $V/U_{0}$"); ax.set_ylabel(r"asymmetry $|D-1|$")
    ax.text(0.00085, 1.0e-2, "polarity reversed", fontsize=6.0, color=BLUE, va="bottom", ha="left")
    ax.text(0.38, 1.2e-8, "contacts exchanged", fontsize=6.0, color=GREEN, va="bottom", ha="right")
    ax.set_yticks([1e-8, 1e-6, 1e-4, 1e-2, 1])
    panel(ax, "a", x=-0.27)

    ax = axes[1]
    lo, hi = 1e-8, 1
    ax.fill_between([lo, hi], [lo / 2, hi / 2], [lo * 2, hi * 2], color=SHADE, lw=0, zorder=0)
    ax.plot([lo, hi], [lo, hi], color=GREY, lw=0.6, zorder=1)
    for geo in ("funnel", "funnel_w1000", "tesla_classic", "tesla_classic_w1000"):
        for slip_inf in (True, False):
            sub = [r for r in allr if r["geo"] == geo and math.isinf(r["slip"]) == slip_inf and not math.isnan(ref_of(r))]
            if not sub: continue
            X = np.array([abs(ref_of(r) - 1) for r in sub]); Y = np.array([abs(r["D_swap_dn"] - 1) for r in sub])
            pts(ax, X, Y, np.array([r["umax_vF"] <= 0.3 for r in sub]), MK[geo], CS[geo], slip_inf, ms=3.4)
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(lo, hi); ax.set_ylim(lo, hi)
    ax.set_xticks([1e-8, 1e-6, 1e-4, 1e-2, 1]); ax.set_yticks([1e-8, 1e-6, 1e-4, 1e-2, 1])
    ax.set_xlabel(r"$|D-1|$ without the gate"); ax.set_ylabel(r"$|D-1|$ with the gate")
    panel(ax, "b", x=-0.27)

    ax = axes[2]
    for geo, c in (("funnel", GREEN), ("tesla_classic", VERM)):
        for vv, ls, fill in ((0.01, "-", True), (0.03, "--", False)):
            sub = sorted([r for r in d4 if r["geo"] == geo and abs(r["v_target"] - vv) < 1e-9], key=lambda r: r["hw"])
            if not sub: continue
            ax.plot([r["hw"] for r in sub], [abs(r["D_swap_dn"] - 1) for r in sub], color=c, ls=ls, lw=1.0, marker=MK[geo], ms=3.8,
                    mfc=c if fill else "white", mew=0.8)
    sub = sorted([r for r in d4 if r["geo"] == "funnel" and abs(r["v_target"] - 0.03) < 1e-9], key=lambda r: r["hw"])
    ax.plot([r["hw"] for r in sub], [abs(r["D_flip0"] - r["D_1d_flip"]) for r in sub], color=BLUE, ls="--", lw=1.0, marker="o", ms=3.8, mfc="white", mew=0.8)
    h = np.array([9.0, 30.0]); ax.plot(h, 1.2e-7 * h ** 2, color=GREY, lw=0.7, ls=":")
    ax.axhline(0.0528 * 0.03 ** 2, color=GREEN, lw=0.6, ls=(0, (1, 2)))
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(8.5, 33); ax.set_ylim(5e-8, 6e-4)
    ax.set_xticks([10, 14, 20, 28]); ax.set_xticklabels(["10", "14", "20", "28"]); ax.xaxis.set_minor_formatter(NullFormatter())
    ax.tick_params(axis="x", which="minor", bottom=False, top=False)
    ax.set_xlabel("wall mesh size (nm)"); ax.set_ylabel(r"$|D-1|$")
    panel(ax, "c", x=-0.27)

    h = [Line2D([], [], color=GREY, marker=MK[g], ls="none", ms=4.6 if MK[g] in "x+" else 3.8, mfc="white" if MK[g] in "x+" else GREY, mew=0.8, label=NAME[g]) for g in MK]
    h += [Line2D([], [], color=BLACK, lw=0.9, ls=(0, (4, 2)), label="one-dimensional closed form"),
          Line2D([], [], color=BLUE, marker="o", ls="none", ms=3.8, label="polarity reversed (circuit): funnel"),
          Line2D([], [], color=GREEN, marker="o", ls="none", ms=3.8, mfc="white", mew=0.8, label="contacts exchanged (shape): funnel"),
          Line2D([], [], color=VERM, marker="^", ls="none", ms=3.8, label="Tesla valve: filled, polarity; open, contacts"),
          Line2D([], [], color=GREY, lw=1.0, ls="-", label=r"(c) $V/U_{0}=0.01$"), Line2D([], [], color=GREY, lw=1.0, ls="--", label=r"(c) $V/U_{0}=0.03$"),
          Line2D([], [], color=BLUE, lw=1.0, ls="--", marker="o", ms=3.8, mfc="white", mew=0.8, label="(c) funnel: polarity term minus closed form"),
          Line2D([], [], color=GREY, lw=0.7, ls=":", label=r"(c) slope $h^{2}$"),
          Line2D([], [], color=GREEN, lw=0.6, ls=(0, (1, 2)), label="(c) inertial term of the funnel at 0.03")]
    fig.legend(handles=h, loc="lower center", ncol=3, fontsize=6.2, bbox_to_anchor=(0.5, -0.02), columnspacing=1.4, handlelength=2.2, labelspacing=0.35)
    save(fig, "fig9_gated_dc")


# =============================================================================
# Figure 10 (Fig. 9 of the paper): length scaling of the ballistic response
# =============================================================================
def fig10():
    import short_funnel_common as sf
    cwd = os.getcwd(); os.chdir(ROOT)
    try: static, ac = sf.load_all()
    finally: os.chdir(cwd)
    RAMP = {"G0": "#d2b5e3", "G1": "#b48ad0", "G2": "#9461b8", "G3": "#3d1560", "G4": "#6f3b9a"}
    MK = {"G0": "o", "G1": "v", "G2": "D", "G3": "s", "G4": "^"}
    LAB = {"G0": r"G0, 6.0 $\mu$m", "G1": r"G1, 3.0 $\mu$m", "G2": r"G2, 2.1 $\mu$m", "G3": r"G3, 0.55 $\mu$m", "G4": r"G4, 0.50 $\mu$m"}
    SHAPE = {"G0": dict(), "G1": dict(Wc=1000, Lc=1000, Wn=87.5, Wd=1000, Ld=1210), "G2": dict(Lc=250, Ld=250),
             "G3": dict(Wc=500, Lc=100, Wn=100, Wd=500, Ld=100), "G4": dict(Wc=400, Lc=169, alpha=45, Wn=75, Wd=400, Ld=169)}
    fig = plt.figure(figsize=(COL2, 5.0))
    gs = fig.add_gridspec(2, 2, wspace=0.27, hspace=0.42, left=0.085, right=0.985, top=0.965, bottom=0.085)

    ax = fig.add_subplot(gs[0, 0])
    NECK = {"G0": 175, "G1": 88, "G2": 175, "G3": 100, "G4": 75}
    x0 = 0.0; xs = {}
    for g in sf.DEV:
        loops, xn, L = funnel_loops(**SHAPE[g])
        draw_domain(ax, loops, fc=RAMP[g], ec=BLACK, lw=0.5, dx=x0)
        W = loops[0][:, 1].max() * 2; xs[g] = (x0, L)
        ax.text(x0 + L / 2, -W / 2 - 200, g, fontsize=7, ha="center", va="top")
        x0 += L + 560
    xmax = x0 - 560
    ax.plot([0, 1000], [1500, 1500], color=BLACK, lw=1.2, solid_capstyle="butt"); ax.text(1150, 1500, r"1 $\mu$m", fontsize=7, va="center")
    ax.annotate("", xy=(5500, 1500), xytext=(3700, 1500), arrowprops=dict(arrowstyle="->", lw=0.7, color=GREY))
    ax.text(5650, 1500, "forward", fontsize=6.8, va="center", color=GREY)
    # second row: the two short devices enlarged five times
    k = 5.0; yc = -3600.0; x1 = 3500.0
    for g in ("G3", "G4"):
        loops, xn, L = funnel_loops(**SHAPE[g])
        draw_domain(ax, loops, fc=RAMP[g], ec=BLACK, lw=0.5, scale=k, dx=x1, dy=yc)
        W = loops[0][:, 1].max() * 2
        ax.text(x1 + k * L / 2, yc - k * W / 2 - 200, f"{g}: neck {NECK[g]} nm", fontsize=6.6, ha="center", va="top")
        x1 += k * L + 3000
    ax.plot([0, 1250], [yc + 1000, yc + 1000], color=BLACK, lw=1.2, solid_capstyle="butt"); ax.text(0, yc + 780, "250 nm", fontsize=7, va="top")
    ax.text(0, yc - 100, "enlarged\nfive times", fontsize=6.6, va="top", color=GREY)
    ax.set_xlim(-250, xmax + 250); ax.set_ylim(yc - 2100, 1900); ax.set_aspect("equal"); ax.axis("off")
    panel(ax, "a", x=-0.02, y=0.97)

    ax = fig.add_subplot(gs[0, 1])
    ax.axvspan(0.4, 1.7, color=SHADE, zorder=0)
    for g in sf.DEV:
        rows = ac.get((g, "uniform", "inf")); fit = sf.static_fit(static.get((g, "uniform", "inf")))
        o = [sf.figures(r, fit) for r in rows]; x = [r["f_Hz"] * sf.LEN[g] * 1e-9 / sf.VF for r in rows]
        ax.errorbar(x, [q["S"] for q in o], [q["eS"] for q in o], color=RAMP[g], marker=MK[g], ms=3.6, lw=1.1, mec="white", mew=0.3, label=LAB[g])
    ax.axhline(0, color=GREY, lw=0.5); ax.set_xscale("log"); ax.set_xlim(1.5e-3, 40); ax.set_ylim(-0.25, 1.15)
    ax.set_xlabel(r"reduced frequency $fL/v_{F}$"); ax.set_ylabel(r"$I_{\rm dc}(f)$ / quasi-static value")
    ax.legend(loc="lower left", fontsize=6.3, handlelength=1.8, labelspacing=0.3)
    panel(ax, "b", x=-0.15)

    ax = fig.add_subplot(gs[1, 0])
    ax.axhspan(0.07, 0.34, color="#dcecf7", zorder=0); ax.text(0.0092, 0.205, "plasmonic funnel", fontsize=6.0, color=BLUE, va="bottom")
    ax.axvspan(0.3, 1.0, color="#e3f4ee", zorder=0, alpha=0.6)
    for fm, c, m in (("uniform", PURPLE, "o"), ("local", DPURPLE, "s")):
        rows = ac[("G3", fm, "700")]; fit = sf.static_fit(static[("G3", fm, "700")])
        o = [sf.figures(r, fit) for r in rows]; f = [r["f_Hz"] / 1e12 for r in rows]
        ax.errorbar(f, [q["R_abs"] for q in o], [q["eR_abs"] for q in o], color=c, ls="--", marker=m, ms=3.8, lw=1.3, mec="white", mew=0.3)
        ax.plot(f, [q["R_single"] for q in o], color=c, ls="--", lw=0.7, marker=m, ms=2.4, mfc="white", mew=0.5)
    ax.axhline(0, color=GREY, lw=0.5); ax.set_xscale("log"); ax.set_xlim(0.008, 40); ax.set_ylim(-0.15, 2.45)
    ax.set_xlabel("frequency (THz)"); ax.set_ylabel("responsivity (A/W)")
    ax.text(0.0092, 1.25, r"G3, 0.55 $\mu$m" + "\n" + r"$\lambda=700$ nm", fontsize=6.6, va="top")
    h = [Line2D([], [], color=PURPLE, ls="--", marker="o", ms=3.8, lw=1.3, label="uniform field, per absorbed watt"),
         Line2D([], [], color=DPURPLE, ls="--", marker="s", ms=3.8, lw=1.3, label="drop at the neck, per absorbed watt"),
         Line2D([], [], color=GREY, ls="--", marker="o", ms=2.4, lw=0.7, mfc="white", mew=0.5, label=r"thin: one device on 75 $\Omega$")]
    ax.legend(handles=h, loc="upper right", fontsize=6.2, handlelength=2.6, labelspacing=0.3)
    panel(ax, "c", x=-0.15)

    ax = fig.add_subplot(gs[1, 1])
    ax.axhspan(0.07, 0.34, color="#dcecf7", zorder=0)
    for fm, c, m in (("uniform", PURPLE, "o"), ("local", DPURPLE, "s")):
        for lam, ls in (("inf", "-"), ("700", "--")):
            P = []
            for g in sf.DEV:
                rows = [r for r in ac.get((g, fm, lam), []) if abs(r["f_Hz"] - 1e12) < 1]; fit = sf.static_fit(static.get((g, fm, lam)))
                if rows and fit:
                    q = sf.figures(rows[0], fit); P.append((sf.LEN[g] / 1e3, q["R_abs"], q["eR_abs"]))
            P.sort()
            ax.errorbar([a[0] for a in P], [a[1] for a in P], [a[2] for a in P], color=c, ls=ls, marker=m, ms=3.8, lw=1.2, mec="white", mew=0.3)
    ax.axhline(0, color=GREY, lw=0.5); ax.set_xscale("log"); ax.set_xlim(0.42, 7.5); ax.set_ylim(-0.15, 1.55)
    ax.set_xticks([0.5, 1, 2, 3, 6]); ax.set_xticklabels(["0.5", "1", "2", "3", "6"]); ax.xaxis.set_minor_formatter(NullFormatter())
    ax.tick_params(axis="x", which="minor", bottom=False, top=False)
    ax.set_xlabel(r"device length $L$ ($\mu$m)"); ax.set_ylabel("responsivity at 1 THz (A/W)")
    ax.text(7.1, 0.205, "plasmonic funnel", fontsize=6.0, color=BLUE, va="center", ha="right")
    h = [Line2D([], [], color=PURPLE, marker="o", ms=3.8, lw=1.2, label="uniform field"), Line2D([], [], color=DPURPLE, marker="s", ms=3.8, lw=1.2, label="drop at the neck"),
         Line2D([], [], color=GREY, ls="-", label="no scattering"), Line2D([], [], color=GREY, ls="--", label=r"$\lambda=700$ nm")]
    ax.legend(handles=h, loc="upper right", fontsize=6.2, handlelength=2.4, labelspacing=0.3, ncol=2, columnspacing=1.2)
    panel(ax, "d", x=-0.15)
    save(fig, "fig10_ballistic_length")


# =============================================================================
# Figure 11 (Fig. 10 of the paper): the short funnel with the field computed
# =============================================================================
def fig11():
    from matplotlib.colors import LinearSegmentedColormap
    c = pic(); sf = c.b7
    static, ac = sf.load_all(); dec = c.decision()
    L, XF, ELL = c.L, c.XF, c.ELL
    fig = plt.figure(figsize=(COL2, 4.75))
    gs = fig.add_gridspec(2, 3, wspace=0.50, hspace=0.50, left=0.075, right=0.985, top=0.955, bottom=0.115)
    FT = [0.1, 0.3, 1, 3, 6]

    def faxis(ax):
        ax.set_xscale("log"); ax.set_xlim(0.07, 9); ax.set_xticks(FT); ax.set_xticklabels([f"{v:g}" for v in FT])
        ax.xaxis.set_minor_formatter(NullFormatter()); ax.tick_params(axis="x", which="minor", bottom=False, top=False)
        ax.set_xlabel("frequency (THz)")

    # (a) static potential, gate at 300 nm
    ax = fig.add_subplot(gs[0, 0])
    x, y, m = c.load_map(f"{c.R}/P1_d300_V0.1.txt"); u = m["mean"] / 0.1
    cmap = LinearSegmentedColormap.from_list("pg", ["#1b7837", "#a6dba0", "#eeeeee", "#c2a5cf", "#762a83"])
    pm = ax.pcolormesh(x, y, u, cmap=cmap, vmin=-0.5, vmax=0.5, shading="nearest", rasterized=True)
    ax.contour(x, y, u, levels=np.arange(-0.4, 0.41, 0.1), colors=BLACK, linewidths=0.3, alpha=0.6)
    ax.plot([0, 100, XF, XF, L, L, XF, XF, 100, 0, 0], [250, 250, 50, 250, 250, -250, -250, -50, -250, -250, 250], color=BLACK, lw=0.8)
    for x0 in (-200, L): ax.plot([x0, x0 + 200, x0 + 200, x0, x0], [-250, -250, 250, 250, -250], color=BLACK, lw=0.5, ls=":")
    ax.text(-100, 0, "source", ha="center", va="center", fontsize=6.0, color="white", rotation=90)
    ax.text(L + 100, 0, "drain", ha="center", va="center", fontsize=6.0, color="white", rotation=90)
    ax.set_xlim(-210, L + 210); ax.set_ylim(-265, 265); ax.set_aspect("equal")
    ax.set_xlabel("$x$ (nm)"); ax.set_ylabel("$y$ (nm)"); ax.set_xticks([0, 250, 500]); ax.set_yticks([-250, 0, 250])
    cb = fig.colorbar(pm, ax=ax, orientation="horizontal", location="top", fraction=0.08, pad=0.04, ticks=[-0.5, 0, 0.5])
    cb.set_label("potential energy / $eV$", fontsize=6.6, labelpad=2); cb.ax.tick_params(labelsize=6.2, length=2); cb.outline.set_linewidth(0.4)
    panel(ax, "a", x=-0.36, y=1.32)

    # (b) potential along the axis against the imposed profiles
    ax = fig.add_subplot(gs[0, 1])
    ax.axvspan(-200, 0, color=SHADE, lw=0); ax.axvspan(L, L + 200, color=SHADE, lw=0)
    xr = np.linspace(-200, L + 200, 900)
    ax.plot(xr, np.clip(0.5 - xr / L, -0.5, 0.5), color=PURPLE, lw=1.0, label="imposed: uniform")
    ax.plot(xr, np.clip(0.5 - (xr - (XF - ELL / 2)) / ELL, -0.5, 0.5), color=DPURPLE, lw=1.0, label="imposed: at the neck")
    for d, ls, lw in ((300, "-", 1.5), (30, "--", 1.0)):
        x, y, m = c.load_map(f"{c.R}/P1_d{d}_V0.1.txt")
        ax.plot(x, c.axis(x, y, m["mean"]) / 0.1, color=BLACK, ls=ls, lw=lw, label=f"computed, gate {d} nm")
    ax.axvline(XF, color=GREY, lw=0.5, ls=":"); ax.text(XF + 12, 0.30, "neck", fontsize=6.2, color=GREY, ha="left")
    ax.set_xlim(-200, L + 200); ax.set_ylim(-0.6, 0.6); ax.set_xticks([0, 250, 500]); ax.set_yticks([-0.5, 0, 0.5])
    ax.set_xlabel("$x$ on the axis (nm)"); ax.set_ylabel("potential energy / $eV$")
    panel(ax, "b", x=-0.30)

    # (c) rectified current against frequency, shape-only drive
    ax = fig.add_subplot(gs[0, 2])
    for fm, col, mk in (("uniform", PURPLE, "o"), ("local", DPURPLE, "s")):
        rs = [r for r in ac[("G3", fm, "700")] if abs(r["Idc_A"]) > 2 * r["Idc_err_A"] and r["f_Hz"] > 0.6e11]
        ax.errorbar([r["f_Hz"] / 1e12 for r in rs], [r["Idc_A"] * 1e6 for r in rs], [r["Idc_err_A"] * 1e6 for r in rs], color=col, marker=mk, ms=3.2, lw=1.0, mec="white", mew=0.3)
    for d, ls, mfc in ((300, "-", BLACK), (30, "--", "white")):
        rs = [q for q in pic_g3(d) if abs(q["I"]) > 2 * q["e"]]
        ax.errorbar([q["f"] for q in rs], [q["I"] for q in rs], [q["e"] for q in rs], color=BLACK, ls=ls, marker="D", ms=3.2, lw=1.2, mfc=mfc, mec=BLACK, mew=0.6)
    faxis(ax); ax.set_yscale("log"); ax.set_ylim(0.08, 150); ax.set_ylabel(r"rectified current ($\mu$A)")
    h = [Line2D([], [], color=PURPLE, marker="o", ms=3.2, lw=1.0, label="imposed: uniform"), Line2D([], [], color=DPURPLE, marker="s", ms=3.2, lw=1.0, label="imposed: at the neck"),
         Line2D([], [], color=BLACK, marker="D", ms=3.2, lw=1.2, label="computed, gate 300 nm"),
         Line2D([], [], color=BLACK, ls="--", marker="D", mfc="white", ms=3.2, lw=1.2, label="computed, gate 30 nm")]
    ax.legend(handles=h, loc="upper right", fontsize=5.6, handlelength=2.2, labelspacing=0.25, borderaxespad=0.2)
    panel(ax, "c", x=-0.30)

    # (d), (e) shape against circuit
    SER = (("funnel", "sym", BLACK, "D", "-", "funnel, shape only"), ("strip", "gated", BLUE, "o", "-", "strip, drain to gate"),
           ("funnel", "gated", ORANGE, "s", "-", "funnel, drain to gate"), ("symmetric", "sym", GREY, "x", "none", "mirror-symmetric control"))
    for k, d in enumerate(c.GATES):
        ax = fig.add_subplot(gs[1, k]); ax.axhline(0, color=GREY, lw=0.5)
        for geo, mode, col, mk, ls, lab in SER:
            if geo == "funnel": rs = pic_g3(d, mode)
            else: rs = [dict(f=r["f"], I=r["I"], e=r["e"]) for r in c.series(geo, mode, d)]
            if geo == "funnel" and mode == "sym": rs = [q for q in rs if 0.25 < q["f"] < 3.5]
            ax.errorbar([q["f"] for q in rs], [q["I"] for q in rs], [q["e"] for q in rs], color=col, marker=mk, ms=3.6, lw=1.1, ls=ls,
                        mec=col if mk == "x" else "white", mew=0.9 if mk == "x" else 0.3, label=lab)
        ax.set_xscale("log"); ax.set_xlim(0.2, 4.5); ax.set_xticks([0.3, 1, 3]); ax.set_xticklabels(["0.3", "1", "3"])
        ax.xaxis.set_minor_formatter(NullFormatter()); ax.tick_params(axis="x", which="minor", bottom=False, top=False)
        ax.set_xlabel("frequency (THz)"); ax.set_ylim(-2.1, 1.75); ax.set_ylabel(r"rectified current ($\mu$A)")
        ax.text(0.04, 0.95, f"gate at {d} nm", transform=ax.transAxes, fontsize=6.6, va="top")
        if k == 0: ax.legend(loc="lower left", fontsize=5.6, handlelength=1.8, labelspacing=0.25, borderaxespad=0.3)
        panel(ax, "de"[k], x=-0.30)

    # (f) mean free path and walls at 1 THz
    ax = fig.add_subplot(gs[1, 2])
    keys = [k for k, *_ in c.CASES]; xs = np.arange(len(keys))
    ax.axhspan(0.07, 0.34, color="#dcecf7", zorder=0); ax.axhline(0, color=GREY, lw=0.5)
    for fm, col, mk, off in (("uniform", PURPLE, "o", -0.24), ("local", DPURPLE, "s", 0.24)):
        ax.plot(xs + off, [c.b7_case(k)[fm]["Rs"] for k in keys], mk, color=col, ms=3.6, mec="white", mew=0.3, ls="none")
    for d, mfc, off in ((300, BLACK, -0.08), (30, "white", 0.08)):
        v = [c.ext_ac(k, d) for k in keys]
        ax.errorbar(xs + off, [q["R"] for q in v], [q["eR"] for q in v], fmt="D", color=BLACK, ms=3.4, mfc=mfc, mec=BLACK, mew=0.6, lw=0.8)
    ax.set_xticks(xs); ax.set_xticklabels(["clean", "700", "100", "700,\n$p=0.5$"]); ax.set_xlim(-0.55, len(keys) - 0.45)
    ax.tick_params(axis="x", which="both", top=False)
    ax.set_xlabel("mean free path (nm)"); ax.set_ylabel("responsivity at 1 THz (A/W)"); ax.set_ylim(-0.12, 1.08)
    ax.text(-0.5, 0.275, "plasmonic funnel", fontsize=5.8, color=BLUE, va="center", ha="left")
    panel(ax, "f", x=-0.30)
    save(fig, "fig11_selfconsistent")


# =============================================================================
# Overview figure for OVERVIEW.md: the two shapes, plain labels
# =============================================================================
def fig_overview():
    fig, axes = plt.subplots(1, 2, figsize=(COL2, 1.95), gridspec_kw=dict(wspace=0.05))
    def arrow(ax, x0, x1, y, text, color):
        ax.annotate("", xy=(x1, y), xytext=(x0, y), arrowprops=dict(arrowstyle="->", lw=0.9, color=color))
        ax.text(0.5 * (x0 + x1), y + (130 if y > 0 else -130), text, fontsize=7.5, ha="center", va="bottom" if y > 0 else "top", color=color)
    ax = axes[0]
    loops, xn, L = funnel_loops(); draw_domain(ax, loops)
    for x in (0, L): ax.plot([x, x], [-1000, 1000], color=BLUE, lw=2.4, zorder=3)
    arrow(ax, 900, L - 900, 1300, "easy direction (into the funnel)", BLACK)
    arrow(ax, L - 900, 900, -1300, "harder direction", GREY)
    ax.annotate("neck:\n175 nm", xy=(xn + 40, 0), xytext=(xn + 1250, 0), fontsize=7, ha="center", va="center", color=GREY,
                arrowprops=dict(arrowstyle="-", lw=0.5, color=GREY))
    ax.set_title("Funnel (\"geometric diode\")", fontsize=9, fontweight="bold")
    ax = axes[1]
    tl = load_poly(os.path.join(ROOT, "meshes", "tesla_classic.poly")); draw_domain(ax, tl, fc="#fbe3d4")
    xs = np.concatenate([l[:, 0] for l in tl])
    for x in (xs.min(), xs.max()): ax.plot([x, x], [-1000, 1000], color=VERM, lw=2.4, zorder=3)
    arrow(ax, 900, xs.max() - 900, 1300, "easy direction (straight through)", BLACK)
    arrow(ax, xs.max() - 900, 900, -1300, "harder direction (the loops turn the flow back)", GREY)
    ax.set_title("Tesla valve", fontsize=9, fontweight="bold")
    for ax in axes:
        ax.set_xlim(-400, 8300); ax.set_ylim(-2350, 2000); ax.set_aspect("equal"); ax.axis("off")
        ax.plot([200, 1200], [-2200, -2200], color=BLACK, lw=1.2, solid_capstyle="butt")
        ax.text(1350, -2200, "1 micrometre", fontsize=7, va="center")
    save(fig, "overview_devices")


FIGS = {1: fig1, 2: fig2, 3: fig3, 4: fig4, 5: fig5, 6: fig6, 7: fig7, 8: fig8, 9: fig9, 10: fig10, 11: fig11}

if __name__ == "__main__":
    args = sys.argv[1:]
    if "overview" in args or not args: fig_overview()
    which = [int(a) for a in args if a != "overview"] or ([] if args else sorted(FIGS))
    for k in which: FIGS[k]()
