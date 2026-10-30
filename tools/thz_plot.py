"""Plot Stage H2 sweep CSVs: responsivity and impedance against frequency.
usage: thz_plot.py out.png label=file.csv [label=file.csv ...] [--title T] [--quantity RI|RV|Z]
"""
import csv, sys
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def load(path):
    rows = list(csv.DictReader(open(path)))
    f = [float(r["f_Hz"]) for r in rows]
    return f, rows


def main():
    out = sys.argv[1]
    series = []; title = ""; quantity = "RI"
    args = sys.argv[2:]
    i = 0
    while i < len(args):
        a = args[i]
        if a == "--title": title = args[i + 1]; i += 2; continue
        if a == "--quantity": quantity = args[i + 1]; i += 2; continue
        lab, path = a.split("=", 1); series.append((lab, path)); i += 1
    fig, axes = plt.subplots(2, 1, figsize=(8, 8), sharex=True)
    for lab, path in series:
        f, rows = load(path)
        if quantity == "Z":
            axes[0].plot(f, [float(r["ReZ_ohm"]) for r in rows], label=lab)
            axes[1].plot(f, [float(r["ImZ_ohm"]) for r in rows], label=lab)
            axes[0].set_ylabel("Re Z (ohm)"); axes[1].set_ylabel("Im Z (ohm)")
        else:
            ri = [float(r["RI_A_per_W"]) for r in rows]; rv = [float(r["RV_V_per_W"]) for r in rows]
            axes[0].plot(f, [abs(v) for v in ri], label=lab)
            axes[1].plot(f, [abs(v) for v in rv], label=lab)
            axes[0].set_ylabel("|R_I| (A/W)"); axes[1].set_ylabel("|R_V| (V/W)")
            axes[0].set_yscale("log"); axes[1].set_yscale("log")
    for ax in axes:
        ax.set_xscale("log"); ax.grid(True, which="both", alpha=0.3); ax.legend(fontsize=8)
    axes[1].set_xlabel("frequency (Hz)")
    if title: axes[0].set_title(title)
    fig.tight_layout(); fig.savefig(out, dpi=130)
    print("wrote", out)


if __name__ == "__main__":
    main()
