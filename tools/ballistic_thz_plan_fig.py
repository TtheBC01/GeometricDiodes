"""Plan figure for Stage B2 (plans/ballistic_thz.png): the two field models on
the Nguyen Device D funnel, the transit-time prediction for the rectified
current against the hydrodynamic reference, and the two-sided time-dependent
estimator. Numbers come from results/killtest.csv and results/THZ.md S4.
usage: .venv/bin/python tools/ballistic_thz_plan_fig.py plans/ballistic_thz.png
"""
import sys, math
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

BLUE, VERM, GREEN, GREY = "#0072B2", "#D55E00", "#009E73", "#6e6e6e"
BLUES = ["#9ecae1", "#4292c6", "#08306b"]
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False,
                     "axes.spines.right": False, "axes.grid": True, "grid.color": "#e6e6e6",
                     "grid.linewidth": 0.6})

# ---- funnel geometry (FunnelParams defaults: Nguyen 2021 Device D) ----------
Wc, Lc, alpha, Wn, Wd, Ld = 2000.0, 2000.0, 30.0, 175.0, 2000.0, 2420.0
Lt = (Wc - Wn) / 2 / math.tan(math.radians(alpha))      # taper length 1580.5 nm
L = Lc + Lt + Ld                                          # 6000.5 nm
x_neck = Lc + Lt
vF = 1.0  # nm/fs

# ---- static reference from results/killtest.csv (p = 1, 300 K, n 1.4e12) ---
I_p, I_m = 189.457, 116.69          # uA at +-0.1 V, lambda = inf
I2 = (I_p - I_m) / 2 / 0.1**2       # uA/V^2  (3640)
Idc_qs = 0.5 * I2 * 0.1**2          # uA at Va 0.1 V (18.2)
R_dev = 0.1 / ((I_p + I_m) / 2e6)   # ohm, chord resistance at 0.1 V (~650)
R_ant = 75.0
Va_ant = 0.1 * (R_ant + R_dev) / R_dev
P_av = Va_ant**2 / (8 * R_ant)      # W
RI_qs = Idc_qs * 1e-6 / P_av        # A/W (~0.9)
I_p7, I_m7 = 108.413, 99.8479       # lambda = 700 nm
RI_qs7 = RI_qs * ((I_p7 - I_m7) / (I_p - I_m))

fig = plt.figure(figsize=(15, 9.2))
gs = fig.add_gridspec(2, 2, height_ratios=[1, 1.15], hspace=0.42, wspace=0.22)

# ---- (a) geometry and the field models --------------------------------------
ax = fig.add_subplot(gs[0, 0])
xs = [0, Lc, x_neck, x_neck, L, L, x_neck, x_neck, Lc, 0, 0]
ys = [Wc / 2, Wc / 2, Wn / 2, Wd / 2, Wd / 2, -Wd / 2, -Wd / 2, -Wn / 2, -Wc / 2, -Wc / 2, Wc / 2]
ax.fill(xs, ys, color="#eef3f8", ec="#333", lw=1.0, zorder=1)
ax.plot([0, 0], [-Wc / 2, Wc / 2], color=BLUE, lw=3)
ax.plot([L, L], [-Wd / 2, Wd / 2], color=VERM, lw=3)
ax.text(60, Wc / 2 + 80, "source (2 um)", color=BLUE, fontsize=8.5)
ax.text(L - 60, Wd / 2 + 80, "drain (2 um)", color=VERM, fontsize=8.5, ha="right")
ax.text(x_neck, -Wn / 2 - 260, "neck 175 nm", ha="center", fontsize=8.5)
ax.annotate("", xy=(x_neck, -1250), xytext=(0, -1250), arrowprops=dict(arrowstyle="<->", color=GREY, lw=0.9))
ax.text(x_neck / 2, -1480, "source to neck: 3.58 um, 3.6 ps at v_F", ha="center", fontsize=8.5, color=GREY)
# potential profiles, drawn on a second scale inside the leads (nondimensional)
xg = np.linspace(0, L, 400)
scale = 850.0
def g_loc(x, ell):
    return np.clip((x - (x_neck - ell / 2)) / ell, 0, 1)
ax.plot(xg, Wc / 2 + 350 + scale * (1 - xg / L), color=GREEN, lw=2, label="uniform field: U = -eV x/L (Stage 1)")
for ell, c, ls in [(1600, BLUES[0], "-"), (500, BLUES[1], "-"), (175, BLUES[2], "-")]:
    ax.plot(xg, Wc / 2 + 350 + scale * (1 - g_loc(xg, ell)), color=c, lw=1.6, ls=ls,
            label=f"localised field: drop over l_F = {ell} nm at the neck")
ax.text(80, Wc / 2 + 350 + scale + 60, "U(x)/eV, 1 -> 0", fontsize=8, color=GREY)
ax.set_xlim(-150, L + 150); ax.set_ylim(-1650, Wc / 2 + 350 + scale + 300)
ax.set_aspect("equal"); ax.grid(False)
ax.set_xlabel("x (nm)"); ax.set_yticks([])
ax.spines["left"].set_visible(False)
ax.legend(loc="upper center", fontsize=8, frameon=False, bbox_to_anchor=(0.5, -0.16), ncol=2)
ax.set_title("(a) Device D funnel and the two field models of Stage B2", loc="left")

# ---- (b) estimator schematic -------------------------------------------------
ax = fig.add_subplot(gs[0, 1])
T = 1.0
t = np.linspace(0, 2.6, 600)
ax.plot(t, np.cos(2 * np.pi * t), color="#333", lw=1.4)
ax.axhline(0, color=GREY, lw=0.6)
ax.text(2.62, 0.95, "V(t) = Va cos(2 pi f t)", fontsize=8.5, va="center")
rng = np.random.default_rng(3)
y0 = -1.5
rows = [(BLUE, "S", 0.95), (BLUE, "S", 0.9), (VERM, "D", 0.9), (BLUE, "S", 0.6), (VERM, "D", 0.75), (BLUE, "S", 0.5), (VERM, "D", 0.55)]
for i, (c, lab, dur) in enumerate(rows):
    t0 = rng.uniform(0, 1.0)
    y = y0 - 0.32 * i
    ax.plot([t0, t0 + dur], [y, y], color=c, lw=2.2, solid_capstyle="butt")
    ax.plot(t0, y, marker="o", color=c, ms=4); ax.plot(t0 + dur, y, marker=">", color=c, ms=5)
    ax.text(-0.05, y, lab, color=c, fontsize=8, ha="right", va="center")
ax.annotate("", xy=(1.0, y0 + 0.22), xytext=(0.0, y0 + 0.22), arrowprops=dict(arrowstyle="<->", color=GREY, lw=0.8))
ax.text(0.5, y0 + 0.3, "injection phase uniform over one period", ha="center", fontsize=8, color=GREY)
ax.text(1.75, y0 - 0.1, "flights 0.5 to 10 ps, energy not conserved:\n"
        "dk/dt = -grad U(x, t), chords <= v_F / (40 f)", fontsize=8, va="top")
ax.text(1.75, y0 - 1.15, "I_dc = (g e/h) Phi [ <T_SD> - <T_DS> ]\n"
        "I_1 from the arrival phases at the drain\n"
        "Phi = int N(E) f(E) dE = 1.43 mA (W = 2 um)", fontsize=8.5, va="top",
        bbox=dict(boxstyle="round,pad=0.4", fc="#f7f7f7", ec="#cccccc"))
ax.set_xlim(-0.15, 3.3); ax.set_ylim(y0 - 2.2, 1.3)
ax.set_xticks([0, 1, 2]); ax.set_xticklabels(["0", "1/f", "2/f"]); ax.set_yticks([])
ax.spines["left"].set_visible(False); ax.grid(False)
ax.set_xlabel("time")
ax.set_title("(b) Two-sided time-dependent estimator (equilibrium injection from both contacts)", loc="left")

# ---- (c) transit-time prediction vs the hydrodynamic reference ----------------
ax = fig.add_subplot(gs[1, :])
f = np.logspace(9, 12.7, 500)
def sinc_ratio(tau_fs):
    x = np.pi * f * tau_fs * 1e-15
    main = np.where(x < np.pi, np.abs(np.sin(x) / x), np.nan)
    env = np.where(x >= np.pi, 1.0 / x, np.nan)
    return main, env
curves = [("uniform field, tau = 3.6 ps (source to neck)", 3580, GREEN, "-"),
          ("localised, l_F = 1600 nm (taper), tau = 1.6 ps", 1600, BLUES[0], "-"),
          ("localised, l_F = 500 nm, tau = 0.5 ps", 500, BLUES[1], "-"),
          ("localised, l_F = 175 nm (neck), tau = 0.18 ps", 175, BLUES[2], "-")]
for lab, tau, c, ls in curves:
    main, env = sinc_ratio(tau)
    ax.plot(f, RI_qs * main, color=c, lw=2, ls=ls, label=lab)
    ax.plot(f, RI_qs * env, color=c, lw=1.2, ls=":")
ax.plot([], [], color=GREY, lw=1.2, ls=":", label="dotted: 1 / (pi f tau) envelope beyond the first zero")
ax.axhline(RI_qs, color=GREY, lw=0.8, ls=":")
ax.text(1.1e9, RI_qs * 0.66, f"quasi-static ballistic funnel, lambda -> inf: {RI_qs:.2f} A/W (18 uA at Va 0.1 V)", fontsize=8.5, color=GREY)
ax.axhline(RI_qs7, color=GREY, lw=0.8, ls="--")
ax.text(1.1e9, RI_qs7 * 0.68, f"same at lambda = 700 nm (300 K hBN): {RI_qs7:.2f} A/W", fontsize=8.5, color=GREY)
# hydrodynamic S4 reference (DS termination, 75 ohm, ideal contacts)
hyd = [(0.9e12, 0.34, "1 um funnel, 100 K", "o"), (0.45e12, 0.09, "1 um funnel, 300 K", "s"),
       (1.87e12, 0.45, "175 nm funnel, 100 K", "^"), (1.87e12, 0.10, "175 nm funnel, 300 K", "v")]
for fx, ri, lab, m in hyd:
    ax.plot(fx, ri, marker=m, color=VERM, ms=8, ls="none", mec="white", mew=0.8)
ax.plot([], [], marker="o", color=VERM, ls="none", label="hydrodynamic (Stage H2, S4): best R_I of the DS-terminated funnels")
ax.text(0.42e12, 0.062, "1 um, 300 K", fontsize=7.5, color=VERM, ha="right")
ax.text(0.78e12, 0.40, "1 um, 100 K", fontsize=7.5, color=VERM, ha="right")
ax.text(2.0e12, 0.50, "175 nm, 100 K", fontsize=7.5, color=VERM)
ax.text(2.0e12, 0.085, "175 nm, 300 K", fontsize=7.5, color=VERM)
# decision bands at 1 THz
ax.axvspan(0.3e12, 1.0e12, color="#f2f2f2", zorder=0)
ax.text(0.55e12, 1.6e-3, "decision window\n0.3 to 1 THz", ha="center", fontsize=8.5, color=GREY)
ax.axhline(1.0, color=GREEN, lw=1.0, ls="--"); ax.text(1.1e9, 1.25, "alive: R_I(1 THz) >= 1 A/W", color=GREEN, fontsize=8.5)
ax.axhline(0.3, color=VERM, lw=1.0, ls="--"); ax.text(1.1e9, 0.235, "closed: R_I(1 THz) <= 0.3 A/W", color=VERM, fontsize=8.5)
for fx in [3e9, 1e10, 3e10, 1e11, 3e11, 1e12]:
    ax.plot(fx, 2.4, marker="|", color="#333", ms=9)
ax.text(1.0e11, 2.6, "B2 frequencies", fontsize=8, ha="center", va="bottom")
ax.set_xscale("log"); ax.set_yscale("log")
ax.set_xlim(1e9, 5e12); ax.set_ylim(1e-3, 4)
ax.set_xlabel("frequency (Hz)")
ax.set_ylabel("current responsivity R_I = I_dc / P_av (A/W, 75 ohm antenna)")
ax.legend(loc="lower left", fontsize=8, frameon=False, ncol=1)
ax.set_title("(c) What B2 measures: the transfer of the ballistic rectified current to THz. "
             "Curves are the plan's sinc(pi f tau) guess with tau the time to cross the field region; the runs decide.",
             loc="left")

fig.suptitle("Stage B2 plan: does the ballistic funnel asymmetry survive at THz drive?", x=0.01, ha="left", fontsize=12, y=0.97)
out = sys.argv[1] if len(sys.argv) > 1 else "plans/ballistic_thz.png"
fig.savefig(out, dpi=150, bbox_inches="tight")
print("wrote", out, f"RI_qs={RI_qs:.3f} A/W, RI_qs(700nm)={RI_qs7:.3f}, P_av={P_av*1e6:.1f} uW, R_dev={R_dev:.0f} ohm")
