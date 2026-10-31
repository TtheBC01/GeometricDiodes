"""Plot Stage H3 large-signal CSVs (gd_thz_power): conversion efficiency against
available antenna power with the small-signal law, and the density extrema.
usage: thz_power_plot.py out.png label=file.csv [label=file.csv ...] [--title T]
"""
import csv, sys
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def load(path):
    rows = list(csv.DictReader(open(path)))
    for r in rows:
        for k in r:
            if k not in ("geo", "set", "term"): r[k] = float(r[k])
    return rows


def main():
    out = sys.argv[1]; series = []; title = ""
    args = sys.argv[2:]; i = 0
    while i < len(args):
        if args[i] == "--title": title = args[i + 1]; i += 2; continue
        lab, path = args[i].split("=", 1); series.append((lab, path)); i += 1
    fig, axes = plt.subplots(3, 1, figsize=(8, 10.5), sharex=True, gridspec_kw=dict(height_ratios=[2, 1, 1]))
    colors = plt.rcParams["axes.prop_cycle"].by_key()["color"]
    for j, (lab, path) in enumerate(series):
        rows = load(path)
        if not rows: continue
        c = colors[j % len(colors)]
        P = [r["Pav_W"] for r in rows]
        axes[0].plot(P, [r["eta_av"] for r in rows], "o-", color=c, label=lab)
        # the quadratic law of the BDF2-discrete operator, which the time stepping converges to at small amplitude
        axes[0].plot(P, [r["eta_small_bdf"] for r in rows], "--", color=c, lw=0.9, alpha=0.7)
        bad = [r for r in rows if r["ok"] < 0.5]
        if bad: axes[0].plot([bad[0]["Pav_W"]], [max(bad[0]["eta_av"], 1e-12)], "x", color=c, ms=10)
        axes[1].plot(P, [r["eta_av"] / r["eta_small_bdf"] if r["eta_small_bdf"] > 0 else float("nan") for r in rows], "o-", color=c, ms=4)
        axes[2].plot(P, [r["n_min"] for r in rows], "v-", color=c, ms=4)
        axes[2].plot(P, [r["n_max"] for r in rows], "^-", color=c, ms=4)
    axes[0].set_xscale("log"); axes[0].set_yscale("log"); axes[0].set_ylabel(r"$\eta = P_\mathrm{dc} / P_\mathrm{av}$")
    axes[1].axhline(1, color="k", lw=0.5); axes[1].axhline(0.9, color="k", lw=0.5, ls=":")
    axes[1].set_ylabel(r"$\eta$ / quadratic law"); axes[1].grid(True, which="both", alpha=0.3)
    axes[0].grid(True, which="both", alpha=0.3); axes[0].legend(fontsize=8, title="solid: time domain; dashed: quadratic law (same time step)", title_fontsize=8)
    if title: axes[0].set_title(title, fontsize=10)
    axes[2].axhline(1, color="k", lw=0.5); axes[2].set_ylabel(r"density extrema $n_\mathrm{min}$, $n_\mathrm{max}$")
    axes[2].set_xlabel(r"available antenna power $P_\mathrm{av}$ (W)"); axes[2].grid(True, which="both", alpha=0.3)
    fig.tight_layout(); fig.savefig(out, dpi=130); print("wrote", out)


if __name__ == "__main__":
    main()
