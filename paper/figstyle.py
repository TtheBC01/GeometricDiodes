"""Shared style, colours and loaders for the paper figures (paper/make_figures.py).

Colour follows the entity, the same in every figure:
  funnel, circuit (Dyakonov-Shur) term ........ blue
  funnel, shape-only term ...................... green
  Tesla valve .................................. vermillion
  ballistic funnel ............................. purple
  controls, references, guides ................. grey
Temperature or mean free path is carried by line style (solid: 100 K or clean
limit; dashed: 300 K or 700 nm), never by colour.
"""
import csv, math, os
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.path import Path
from matplotlib.patches import PathPatch

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
RES = os.path.join(ROOT, "results")
OUT = os.path.join(ROOT, "paper", "figs")

BLUE, GREEN, VERM, PURPLE, GREY, BLACK = "#0072B2", "#009E73", "#D55E00", "#CC79A7", "#7a7a7a", "#222222"
ORANGE, SKY, YELLOW = "#E69F00", "#56B4E9", "#F0E442"
BLUES = ["#9ecae1", "#4292c6", "#08306b"]
ORANGES = ["#fdae6b", "#e6550d", "#7f2704"]
SHADE = "#f0f0f0"
COL1, COL2 = 3.40, 7.00   # single- and double-column widths in inches

plt.rcParams.update({
    "font.size": 8, "axes.labelsize": 8, "axes.titlesize": 8, "legend.fontsize": 6.8,
    "xtick.labelsize": 7.5, "ytick.labelsize": 7.5, "font.family": "DejaVu Sans",
    "mathtext.fontset": "dejavusans", "axes.linewidth": 0.6, "xtick.major.width": 0.6,
    "ytick.major.width": 0.6, "xtick.minor.width": 0.4, "ytick.minor.width": 0.4,
    "xtick.direction": "in", "ytick.direction": "in", "xtick.top": True, "ytick.right": True,
    "lines.linewidth": 1.3, "lines.markersize": 4, "legend.frameon": False,
    "legend.handlelength": 2.2, "axes.grid": False, "savefig.dpi": 300, "pdf.fonttype": 42,
    "errorbar.capsize": 1.5,
})


def load_csv(path, text=("geo", "geometry", "set", "term", "field")):
    if not os.path.exists(path): return []
    out = []
    with open(path) as f:
        for r in csv.DictReader(f):
            d = {}
            for k, v in r.items():
                if k is None: continue
                if k in text: d[k] = v
                else:
                    try: d[k] = float(v)
                    except (TypeError, ValueError): d[k] = float("nan")
            out.append(d)
    return out


def load_poly(path):
    """.poly interchange format: 'loops N', then per loop 'loop n' and n lines 'x y tag'."""
    loops = []
    with open(path) as f:
        toks = f.read().split("\n")
    i = 0
    while i < len(toks):
        line = toks[i].split()
        if line and line[0] == "loop":
            n = int(line[1]); pts = []
            for j in range(n):
                x, y = toks[i + 1 + j].split()[:2]; pts.append((float(x), float(y)))
            loops.append(np.array(pts)); i += n + 1
        else: i += 1
    return loops


def draw_domain(ax, loops, fc="#dfe8f1", ec=BLACK, lw=0.6, scale=1.0, dx=0.0, dy=0.0, zorder=2):
    verts, codes = [], []
    for L in loops:
        P = L * scale + np.array([dx, dy])
        verts += list(P) + [P[0]]
        codes += [Path.MOVETO] + [Path.LINETO] * (len(P) - 1) + [Path.CLOSEPOLY]
    ax.add_patch(PathPatch(Path(verts, codes), fc=fc, ec=ec, lw=lw, zorder=zorder))


def funnel_loops(Wc=2000, Lc=2000, alpha=30, Wn=175, Wd=2000, Ld=2420):
    Lt = 0.5 * (Wc - Wn) / math.tan(math.radians(alpha)); L = Lc + Lt + Ld; xn = Lc + Lt
    low = [(0, -Wc / 2), (Lc, -Wc / 2), (xn, -Wn / 2), (xn, -Wd / 2), (L, -Wd / 2)]
    return [np.array(low + [(x, -y) for x, y in reversed(low)])], xn, L


def panel(ax, label, x=-0.14, y=1.03):
    ax.text(x, y, f"({label})", transform=ax.transAxes, fontsize=8.5, fontweight="bold", va="bottom", ha="left")


def save(fig, name):
    os.makedirs(OUT, exist_ok=True)
    fig.savefig(os.path.join(OUT, name + ".pdf"), bbox_inches="tight", pad_inches=0.02)
    fig.savefig(os.path.join(OUT, name + ".png"), bbox_inches="tight", pad_inches=0.02, dpi=220)
    plt.close(fig)
    print("wrote", os.path.join("paper/figs", name + ".pdf/.png"))


def fmt_f(f):
    return f"{f/1e9:g} GHz" if f < 1e12 else f"{f/1e12:g} THz"


kB = 1.380649e-23
