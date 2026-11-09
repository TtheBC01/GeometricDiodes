"""Figures for Stage H4 (plans/hydro_density.md) from the gd_thz_sweep and
gd_thz_power CSVs in results/thz:
  D1_maps.png     (n, d) maps of eta/P_av, eta_peak and eta at 10 uW per set and frequency,
                  invalid operating points hatched
  D1_lines.png    the dimensionless response r and eta/P_av against n for each gate, with the
                  matched-antenna (D1m) and constant-mobility (D1t) variants; eta_peak against P_knee
  D2_contacts.png contact resistance: eta/P_av and NEP against R_c
  D3_ladder.png   the time-domain ladder at (5e11, 10 nm) against the H3 ladder at (1e12, 30 nm)
usage: thz_density_plot.py results/thz
"""
import csv, glob, math, os, sys
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
from matplotlib.patches import Rectangle

NS = [1e12, 5e11, 2e11, 1e11, 5e10]
DS = [30e-9, 10e-9, 5e-9]
FS = [1e11, 3e11, 1e12]
SETS = ["100K", "300K"]
# categorical (fixed order, CVD-checked): frequency 0.1 / 0.3 / 1 THz
C_F = {1e11: "#0072B2", 3e11: "#D55E00", 1e12: "#009E73"}
M_F = {1e11: "o", 3e11: "s", 1e12: "^"}
# sequential (one hue, light to dark): gate 30 / 10 / 5 nm
C_D = {30e-9: "#6baed6", 10e-9: "#2171b5", 5e-9: "#08306b"}
M_D = {30e-9: "o", 10e-9: "s", 5e-9: "^"}
C_SET = {"100K": "#0072B2", "300K": "#D55E00"}


def load(path):
    rows = list(csv.DictReader(open(path)))
    for r in rows:
        for k in r:
            if k not in ("geo", "set", "term"): r[k] = float(r[k])
    return rows


def load_grid(d, prefix):
    """{(set, f, n, d): row} from prefix_*.csv (D1, D1m, D1t, D2, D2m)."""
    g = {}
    for p in glob.glob(os.path.join(d, prefix + "_*.csv")):
        for r in load(p):
            key = (r["set"], near(r["f_Hz"], FS), near(r["n_cm2"], NS), near(r["d_m"], DS))
            if prefix.startswith("D2"): key = key + (r["Rc_ohm"],)
            g[key] = r
    return g


def near(x, vals):
    return min(vals, key=lambda v: abs(math.log(x / v)))


def valid(r):
    return r["valid_FL"] > 0.5 and r["valid_gate"] > 0.5


def fmt_f(f):
    return f"{f/1e9:.0f} GHz" if f < 1e12 else f"{f/1e12:g} THz"


def fmt_n(n):
    return f"{n:.0e}".replace("e+", "e")


def sig(x, s=2):
    if x == 0 or not math.isfinite(x): return "0"
    e = math.floor(math.log10(abs(x)))
    if -2 <= e <= 3: return f"{x:.{max(0, s - 1 - e)}f}"
    return f"{x:.{s-1}e}".replace("e-0", "e-").replace("e+0", "e")


def maps(d, g, out):
    quantities = [("eta_per_W", r"$\eta / P_\mathrm{av}$ (1/W)"), ("eta_peak", r"$\eta_\mathrm{peak}$ (predicted)"),
                  ("eta_10uW", r"$\eta$ at 10 $\mu$W")]
    cols = [(s, f) for s in SETS for f in FS]
    fig, axes = plt.subplots(len(quantities), len(cols), figsize=(15, 9.5))
    for i, (q, label) in enumerate(quantities):
        vals = [abs(r[q]) for k, r in g.items() if r[q] > 0]
        if not vals: continue
        norm = LogNorm(vmin=min(vals), vmax=max(vals))
        for j, (s, f) in enumerate(cols):
            ax = axes[i][j]
            Z = [[g[(s, f, n, dd)][q] if (s, f, n, dd) in g else float("nan") for dd in DS] for n in NS]
            im = ax.imshow(Z, cmap="Blues", norm=norm, aspect="auto")
            for a, n in enumerate(NS):
                for b, dd in enumerate(DS):
                    r = g.get((s, f, n, dd))
                    if r is None: continue
                    ok = valid(r)
                    if not ok:
                        ax.add_patch(Rectangle((b - 0.5, a - 0.5), 1, 1, fill=True, facecolor="white", alpha=0.6, hatch="////",
                                               edgecolor="#999999", lw=0))
                    dark = norm(abs(r[q])) > 0.6 and ok
                    ax.text(b, a, sig(r[q]), ha="center", va="center", fontsize=7.5,
                            color="white" if dark else ("#777777" if not ok else "#222222"))
                    if r["flag_q"] > 0.5: ax.text(b + 0.42, a - 0.42, "q", ha="right", va="top", fontsize=6, color="#555555")
            ax.set_xticks(range(len(DS))); ax.set_xticklabels([f"{dd*1e9:g}" for dd in DS], fontsize=8)
            ax.set_yticks(range(len(NS))); ax.set_yticklabels([fmt_n(n) if j == 0 else "" for n in NS], fontsize=8)
            if i == 0: ax.set_title(f"{s}, {fmt_f(f)}", fontsize=9)
            if i == len(quantities) - 1: ax.set_xlabel("gate $d$ (nm)", fontsize=8)
            if j == 0: ax.set_ylabel("$n$ (cm$^{-2}$)", fontsize=8)
            ax.tick_params(length=0)
            for sp in ax.spines.values(): sp.set_visible(False)
        cb = fig.colorbar(im, ax=axes[i].tolist(), fraction=0.012, pad=0.01)
        cb.set_label(label, fontsize=9); cb.ax.tick_params(labelsize=7)
    fig.suptitle("Stage H4, D1: 1 um funnel, Dyakonov-Shur termination, 75 ohm antenna. Hatched: outside the Fermi-liquid "
                 "($E_F \\geq 3kT$) or gating ($U_0 \\geq 4kT$) limits; q: quantum capacitance dominant ($q > 0.3$)", fontsize=9)
    fig.savefig(out, dpi=140, bbox_inches="tight"); print("wrote", out)


def lines(d, g, gm, gt, out):
    f0 = 3e11
    fig, axes = plt.subplots(2, 3, figsize=(14, 8))
    for i, s in enumerate(SETS):
        for j, (q, label, ylog) in enumerate([("r", r"$r = |V_\mathrm{oc}| / V_a^2$ (dimensionless)", False),
                                               ("eta_per_W", r"$\eta / P_\mathrm{av}$ (1/W)", True)]):
            ax = axes[i][j]
            for dd in DS:
                for grid, ls, tag in [(g, "-", "75 ohm"), (gm, "--", "matched"), (gt, ":", r"$\tau \propto \sqrt{n}$")]:
                    if grid is gt and s != "100K": continue
                    pts = [(n, grid[(s, f0, n, dd)]) for n in NS if (s, f0, n, dd) in grid]
                    if not pts: continue
                    ys = [abs(r["Voc_per_Va2"]) if q == "r" else r[q] for n, r in pts]
                    xs = [n for n, r in pts]
                    ax.plot(xs, ys, ls, color=C_D[dd], lw=1.6, marker=M_D[dd], ms=5,
                            markerfacecolor=[C_D[dd] if valid(r) else "white" for n, r in pts][0] if False else None)
                    for (n, r), y in zip(pts, ys):
                        if not valid(r): ax.plot([n], [y], marker=M_D[dd], ms=5, color=C_D[dd], markerfacecolor="white", ls="none")
                    if grid is g: ax.annotate(f"{dd*1e9:g} nm", (xs[-1], ys[-1]), textcoords="offset points", xytext=(-6, 4 if dd != 5e-9 else -10),
                                              fontsize=7.5, color=C_D[dd], ha="right")
            ax.set_xscale("log"); ax.invert_xaxis()
            if ylog: ax.set_yscale("log")
            ax.set_ylabel(label, fontsize=9); ax.grid(True, which="both", alpha=0.25)
            ax.set_title(f"{s}, {fmt_f(f0)}: solid 75 ohm, dashed matched antenna" + (r", dotted $\tau \propto \sqrt{n}$" if s == "100K" else ""),
                         fontsize=8.5)
            if i == 1: ax.set_xlabel("$n$ (cm$^{-2}$)", fontsize=9)
        # eta_peak against P_knee: the hypothesis picture (ceiling flat, knee moves with n d)
        ax = axes[i][2]
        for f in FS:
            for dd in DS:
                pts = [g[(s, f, n, dd)] for n in NS if (s, f, n, dd) in g]
                if not pts: continue
                ax.plot([r["Pknee_W"] for r in pts], [r["eta_peak"] for r in pts], "-", color=C_F[f], lw=0.8 if f != f0 else 1.6, alpha=0.9)
                for r in pts:
                    ax.plot([r["Pknee_W"]], [r["eta_peak"]], M_D[dd], color=C_F[f], ms=5 if f == f0 else 3.5,
                            markerfacecolor=C_F[f] if valid(r) else "white")
        ax.set_xscale("log"); ax.set_yscale("log"); ax.grid(True, which="both", alpha=0.25)
        ax.set_ylabel(r"predicted $\eta_\mathrm{peak}$", fontsize=9)
        if i == 1: ax.set_xlabel(r"predicted knee $P_\mathrm{knee} = (V_\mathrm{unit}/2)^2 / 8 R_\mathrm{ant}$ (W)", fontsize=9)
        ax.set_title(f"{s}: predicted peak efficiency against the knee power", fontsize=8.5)
        from matplotlib.lines import Line2D
        h = [Line2D([], [], color=C_F[f], lw=1.6, label=fmt_f(f)) for f in FS]
        h += [Line2D([], [], color="#444444", marker=M_D[dd], ls="none", label=f"{dd*1e9:g} nm") for dd in DS]
        h += [Line2D([], [], color="#444444", marker="o", ls="none", markerfacecolor="white", label="outside validity")]
        ax.legend(handles=h, fontsize=7, ncol=2, loc="lower right")
    fig.tight_layout(); fig.savefig(out, dpi=140); print("wrote", out)


def contacts(d, g, gm, g2, g2m, out):
    pts = sorted({(k[2], k[3]) for k in g2})
    if not pts: return
    RCS = [0, 100, 300, 1000]
    fig, axes = plt.subplots(2, 3, figsize=(14, 7.5), sharex=True)
    from matplotlib.lines import Line2D
    for j, f in enumerate(FS):
        for i, (q, label) in enumerate([("eta_per_W", r"$\eta / P_\mathrm{av}$ (1/W)"), ("NEP_W_per_rtHz", r"NEP (W/$\sqrt{\mathrm{Hz}}$)")]):
            ax = axes[i][j]
            for s in SETS:
                for (n, dd), mk in zip(pts, ["o", "s", "^", "D"]):
                    for base, grid, ls in [(g, g2, "-"), (gm, g2m, "--")]:
                        xs, ys = [], []
                        for Rc in RCS:
                            r = base.get((s, f, n, dd)) if Rc == 0 else grid.get((s, f, n, dd, Rc / 4.0))
                            if r is None: continue
                            xs.append(Rc); ys.append(r[q])
                        if xs: ax.plot(xs, ys, ls, color=C_SET[s], marker=mk, ms=5, lw=1.5)
            ax.set_yscale("log"); ax.set_xscale("symlog", linthresh=100); ax.set_xticks(RCS); ax.set_xticklabels([str(x) for x in RCS])
            ax.grid(True, which="both", alpha=0.25)
            if j == 0: ax.set_ylabel(label, fontsize=9)
            if i == 0: ax.set_title(fmt_f(f), fontsize=9)
            if i == 1: ax.set_xlabel(r"contact resistance $R_c$ (ohm um)", fontsize=9)
    h = [Line2D([], [], color=C_SET[s], lw=1.5, label=s) for s in SETS]
    h += [Line2D([], [], color="#444444", marker=mk, ls="none", label=f"n {fmt_n(n)}, d {dd*1e9:g} nm") for (n, dd), mk in zip(pts, ["o", "s", "^", "D"])]
    h += [Line2D([], [], color="#444444", ls="-", label="75 ohm antenna"), Line2D([], [], color="#444444", ls="--", label="matched antenna")]
    axes[0][0].legend(handles=h, fontsize=7, loc="lower left")
    fig.suptitle("Stage H4, D2: contact resistance in series with each contact (R_c over the 4 um contact; dc Thevenin resistance $R_{dc} + 2R_c$)", fontsize=9)
    fig.tight_layout(); fig.savefig(out, dpi=140); print("wrote", out)


def ladder(d, out):
    runs = []
    for s in SETS:
        p3 = os.path.join(d, f"D3_funnel_w1000_{s}_dss_n5e11_d10_f3e11.csv")
        ph = os.path.join(d, f"P{1 if s == '100K' else 2}_funnel_w1000_{s}_dss_f3e11.csv")
        if os.path.exists(p3) and os.path.getsize(p3) > 100: runs.append((s, "D3 (5e11, 10 nm)", load(p3), "-"))
        if os.path.exists(ph): runs.append((s, "H3 (1e12, 30 nm)", load(ph), "--"))
    if not runs: return
    fig, axes = plt.subplots(1, 2, figsize=(12, 4.8))
    for s, lab, rows, ls in runs:
        rows = [r for r in rows if r["ok"] > 0.5]
        c = C_SET[s]
        axes[0].plot([r["Pav_W"] for r in rows], [r["eta_av"] for r in rows], ls, color=c, marker="o" if ls == "-" else "s", ms=5, label=f"{s}, {lab}")
        axes[0].plot([r["Pav_W"] for r in rows], [r["eta_small_bdf"] for r in rows], ":", color=c, lw=0.9)
        axes[1].plot([r["Va"] for r in rows], [r["eta_av"] / r["eta_small_bdf"] for r in rows], ls, color=c, marker="o" if ls == "-" else "s", ms=5)
    axes[0].set_xscale("log"); axes[0].set_yscale("log"); axes[0].grid(True, which="both", alpha=0.25)
    axes[0].set_xlabel(r"available antenna power $P_\mathrm{av}$ (W)"); axes[0].set_ylabel(r"$\eta = P_\mathrm{dc}/P_\mathrm{av}$")
    axes[0].legend(fontsize=8, title="dotted: quadratic law (same time step)", title_fontsize=8)
    axes[1].axhline(1, color="k", lw=0.5); axes[1].grid(True, which="both", alpha=0.25)
    axes[1].set_xlabel(r"drive amplitude $V_a / V_\mathrm{unit}$"); axes[1].set_ylabel(r"$\eta$ / quadratic law (saturation factor)")
    axes[1].set_title("the saturation factor transfers between operating points if the curves coincide", fontsize=9)
    axes[0].set_title("0.3 THz, 1 um funnel, DS termination, matched dc load: knee moves with $V_\\mathrm{unit}^2$", fontsize=9)
    fig.tight_layout(); fig.savefig(out, dpi=140); print("wrote", out)


def main():
    d = sys.argv[1]
    g, gm, gt = load_grid(d, "D1"), load_grid(d, "D1m"), load_grid(d, "D1t")
    g2, g2m = load_grid(d, "D2"), load_grid(d, "D2m")
    if g:
        maps(d, g, os.path.join(d, "D1_maps.png"))
        lines(d, g, gm, gt, os.path.join(d, "D1_lines.png"))
    if g2: contacts(d, g, gm, g2, g2m, os.path.join(d, "D2_contacts.png"))
    ladder(d, os.path.join(d, "D3_ladder.png"))


if __name__ == "__main__":
    main()
