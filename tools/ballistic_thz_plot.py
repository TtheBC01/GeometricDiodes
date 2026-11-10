"""Stage B2 figures from results/bthz: the transfer S_B(f) and the responsivity
R_I(f) against the hydrodynamic reference (B2_transfer.png), the field-length
dependence, the control and the flight-time tail (B2_ellF_control.png), the
amplitude ladder (B4_amplitude.png).
usage: .venv/bin/python tools/ballistic_thz_plot.py results/bthz
"""
import sys, os, math, csv
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
sys.path.insert(0, os.path.dirname(__file__))
from ballistic_thz_common import *

BLUE, VERM, GREEN, GREY, PURPLE = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#CC79A7"
BLUES = ["#9ecae1", "#4292c6", "#08306b"]
plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False,
                     "axes.grid": True, "grid.color": "#e6e6e6", "grid.linewidth": 0.6})
HYD = [(0.9e12, 0.34, "1 um funnel, 100 K", "o"), (0.45e12, 0.09, "1 um funnel, 300 K", "s"),
       (1.87e12, 0.45, "175 nm funnel, 100 K", "^"), (1.87e12, 0.10, "175 nm funnel, 300 K", "v")]


def series(rows, Va=0.1):
    rs = sorted([r for r in rows if abs(r["V_V"] - Va) < 1e-9], key=lambda r: r["f_Hz"])
    return np.array([r["f_Hz"] for r in rs]), rs


def sinc(f, tau_fs):
    x = np.pi * f * tau_fs * 1e-15
    return np.sin(x) / x


def main(d):
    b1 = load(f"{d}/B1.csv"); sc = static_curves(b1)
    b2 = {"uniform": load(f"{d}/B2u.csv"), "local500": load(f"{d}/B2l.csv")}
    b3 = load(f"{d}/B3.csv"); b0 = load(f"{d}/B0.csv"); b4 = load(f"{d}/B4.csv")
    geo = "funnel_nguyenD"

    # ---------------- figure 1: transfer and responsivity --------------------
    fig, axes = plt.subplots(1, 2, figsize=(14.5, 5.6))
    ax = axes[0]
    styles = {("uniform", "inf"): (GREEN, "-", "o", "uniform field, lambda -> inf"),
              ("uniform", "700"): (GREEN, "--", "s", "uniform field, lambda = 700 nm"),
              ("local500", "inf"): (BLUES[1], "-", "o", "localised field, l_F = 500 nm, lambda -> inf"),
              ("local500", "700"): (BLUES[1], "--", "s", "localised field, l_F = 500 nm, lambda = 700 nm")}
    for fk, rows in b2.items():
        for lk in ["inf", "700"]:
            rs = [r for r in rows if lam_key(r) == lk]
            if not rs or (geo, fk, lk) not in sc: continue
            qs, eqs, _, _ = sc[(geo, fk, lk)].qs(0.1)
            if qs < 3 * eqs: continue  # no usable quasi-static reference (localised field at 700 nm: A < 1, 3 sigma from zero)
            f, rs = series(rs)
            S = np.array([r["Idc_A"] for r in rs]) / qs
            eS = np.array([math.hypot(r["Idc_err_A"] / qs, r["Idc_A"] * eqs / qs ** 2) for r in rs])
            c, ls, m, lab = styles[(fk, lk)]
            ax.errorbar(f, S, eS, color=c, ls=ls, marker=m, ms=5, lw=1.8, capsize=2, label=lab, mec="white", mew=0.6)
    ff = np.logspace(8.3, 12.1, 300)
    ax.plot(ff, sinc(ff, 3580), color=GREEN, lw=1, ls=":", label="sinc guess, tau = 3.6 ps (uniform)")
    ax.plot(ff, sinc(ff, 500), color=BLUES[1], lw=1, ls=":", label="sinc guess, tau = 0.5 ps (l_F = 500 nm)")
    ax.axhline(0, color=GREY, lw=0.8); ax.axhline(1, color=GREY, lw=0.8, ls="--")
    ax.axvspan(3e11, 1e12, color="#f2f2f2", zorder=0)
    ax.set_xscale("log"); ax.set_xlim(2e8, 1.5e12); ax.set_ylim(-0.4, 1.3)
    ax.text(2.3e8, 1.2, "localised field at lambda = 700 nm omitted: its static asymmetry is below 1 (no reference); see (b)", fontsize=7.5, color=GREY)
    ax.set_xlabel("frequency (Hz)"); ax.set_ylabel("S_B = I_dc(f) / quasi-static I_dc  (Va = 0.1 V)")
    ax.legend(fontsize=8, frameon=False, loc="lower left")
    ax.set_title("(a) Transfer of the rectified current to THz (Device D funnel, p = 1, 300 K)", loc="left")

    ax = axes[1]
    for fk, rows in b2.items():
        for lk in ["inf", "700"]:
            rs = [r for r in rows if lam_key(r) == lk]
            if not rs: continue
            f, rs = series(rs)
            RI = np.array([r["RI_A_per_W"] for r in rs]); eRI = np.array([r["Idc_err_A"] / r["Pav_W"] for r in rs])
            c, ls, m, lab = styles[(fk, lk)]
            pos = RI > 0
            ax.errorbar(f[pos], RI[pos], eRI[pos], color=c, ls=ls, marker=m, ms=5, lw=1.8, capsize=2, label=lab, mec="white", mew=0.6)
            if (~pos).any():
                ax.plot(f[~pos], -RI[~pos], color=c, ls="none", marker=m, ms=6, mfc="white", mew=1.4)
    ax.plot([], [], color=GREY, ls="none", marker="o", mfc="white", mew=1.4, label="open marker: negative (sign-reversed) I_dc, |R_I| shown")
    for fx, ri, lab, m in HYD:
        ax.plot(fx, ri, marker=m, color=VERM, ms=8, ls="none", mec="white", mew=0.8)
    ax.plot([], [], marker="o", color=VERM, ls="none", label="hydrodynamic reference (THZ.md S4), DS termination, 75 ohm")
    ax.text(0.5e12, 0.075, "1 um, 300 K", fontsize=7.5, color=VERM, ha="left")
    ax.text(0.75e12, 0.38, "1 um, 100 K", fontsize=7.5, color=VERM, ha="right")
    ax.text(2.0e12, 0.48, "175 nm, 100 K", fontsize=7.5, color=VERM)
    ax.text(2.0e12, 0.107, "175 nm, 300 K", fontsize=7.5, color=VERM)
    ax.axhline(1.0, color=GREEN, lw=1.0, ls="--"); ax.text(2.3e8, 1.15, "alive: R_I(1 THz) >= 1 A/W", color=GREEN, fontsize=8.5)
    ax.axhline(0.3, color=VERM, lw=1.0, ls="--"); ax.text(2.3e8, 0.235, "closed: R_I(1 THz) <= 0.3 A/W", color=VERM, fontsize=8.5)
    ax.axvspan(3e11, 1e12, color="#f2f2f2", zorder=0)
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(2e8, 5e12); ax.set_ylim(2e-3, 3)
    ax.set_xlabel("frequency (Hz)"); ax.set_ylabel("R_I = I_dc / P_av (A/W, 75 ohm antenna, own mismatch)")
    ax.legend(fontsize=7.6, frameon=False, loc="lower left")
    ax.set_title("(b) Responsivity against the hydrodynamic funnels and the decision lines", loc="left")
    fig.tight_layout()
    fig.savefig(f"{d}/B2_transfer.png", dpi=150, bbox_inches="tight"); print("wrote", f"{d}/B2_transfer.png")

    # ---------------- figure 2: ell_F, control, flight-time tail --------------
    fig, axes = plt.subplots(1, 3, figsize=(15.5, 4.9))
    ax = axes[0]
    for f0, c, m in [(3e11, BLUE, "o"), (1e12, VERM, "s")]:
        xs, ys, es = [], [], []
        for ell in [175, 500, 1600]:
            rs = [r for r in b3 if abs(r["f_Hz"] - f0) < 1 and abs(r["ellF_nm"] - ell) < 1 and lam_key(r) == "inf"]
            if not rs and ell == 500:
                rs = [r for r in b2["local500"] if abs(r["f_Hz"] - f0) < 1 and lam_key(r) == "inf" and abs(r["V_V"] - 0.1) < 1e-9]
            key = (geo, f"local{ell}", "inf")
            if not rs or key not in sc: continue
            qs, eqs, _, _ = sc[key].qs(0.1)
            xs.append(ell); ys.append(rs[0]["Idc_A"] / qs); es.append(math.hypot(rs[0]["Idc_err_A"] / qs, rs[0]["Idc_A"] * eqs / qs ** 2))
        ax.errorbar(xs, ys, es, color=c, marker=m, ms=6, lw=1.8, capsize=2, label=f"{fmt_f(f0)}", mec="white")
        rs = [r for r in b2["uniform"] if abs(r["f_Hz"] - f0) < 1 and lam_key(r) == "inf" and abs(r["V_V"] - 0.1) < 1e-9]
        if rs and (geo, "uniform", "inf") in sc:
            qs, eqs, _, _ = sc[(geo, "uniform", "inf")].qs(0.1)
            ax.errorbar([6000], [rs[0]["Idc_A"] / qs], [rs[0]["Idc_err_A"] / qs], color=c, marker=m, ms=6, mfc="white", mew=1.4, ls="none", capsize=2)
        ell = np.logspace(2.1, 3.9, 200)
        ax.plot(ell, sinc(f0, ell), color=c, lw=1, ls=":")
    ax.plot([], [], color=GREY, ls=":", label="sinc(pi f l_F / v_F) guess")
    ax.plot([], [], color=GREY, marker="o", mfc="white", mew=1.4, ls="none", label="open: uniform field (drop over the 6 um device)")
    ax.axhline(0, color=GREY, lw=0.8); ax.axhline(1, color=GREY, lw=0.8, ls="--")
    ax.set_xscale("log"); ax.set_xlim(120, 9000); ax.set_ylim(-0.4, 1.3)
    ax.set_xticks([175, 500, 1600, 6000]); ax.set_xticklabels(["175\n(neck)", "500", "1600\n(taper)", "uniform\n(6000)"])
    ax.xaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
    ax.set_xlabel("length of the potential drop l_F (nm)"); ax.set_ylabel("S_B at Va = 0.1 V")
    ax.legend(fontsize=8, frameon=False, loc="lower left")
    ax.set_title("(a) Where the drop lives decides the roll-off (B3)", loc="left")

    ax = axes[1]
    for fk, c, m, lab in [("uniform", GREEN, "o", "uniform field"), ("local", BLUES[1], "s", "localised, l_F = 500 nm")]:
        rs = sorted([r for r in b0 if r["field"] == fk], key=lambda r: r["f_Hz"])
        if not rs: continue
        f = np.array([r["f_Hz"] for r in rs])
        ax.errorbar(f, [r["Idc_A"] * 1e6 for r in rs], [r["Idc_err_A"] * 1e6 for r in rs], color=c, marker=m, ms=5, lw=1.5, capsize=2, label=lab, mec="white")
    ax.axhline(0, color=GREY, lw=0.8)
    qs_u = sc[(geo, "uniform", "inf")].qs(0.1)[0] * 1e6 if (geo, "uniform", "inf") in sc else 18
    ax.axhline(qs_u, color=GREY, lw=0.8, ls="--"); ax.text(2.3e8, qs_u * 1.05, "funnel quasi-static I_dc", fontsize=8, color=GREY)
    ax.set_xscale("log"); ax.set_xlim(2e8, 1.5e12)
    ax.set_xlabel("frequency (Hz)"); ax.set_ylabel("I_dc of the straight control (uA)")
    ax.legend(fontsize=8, frameon=False)
    ax.set_title("(b) Straight 175 nm control: zero at every frequency (B0)", loc="left")

    ax = axes[2]
    th = f"{d}/B1_thist_uniform_laminf.csv"
    if os.path.exists(th):
        rows = list(csv.DictReader(open(th)))
        lo = np.array([float(r["t_lo_fs"]) for r in rows]); hi = np.array([float(r["t_hi_fs"]) for r in rows])
        for k, c, lab in [("all_S", BLUE, "all source-injected"), ("tx_S", BLUE, "source-injected, transmitted"),
                          ("all_D", VERM, "all drain-injected"), ("tx_D", VERM, "drain-injected, transmitted")]:
            cnt = np.array([float(r[k]) for r in rows]); tot = cnt.sum()
            tail = np.cumsum(cnt[::-1])[::-1] / tot  # fraction with t >= lo
            m = np.isfinite(lo) & (lo > 0)
            ax.plot(lo[m] * 1e-3, tail[m], color=c, lw=1.8, ls="-" if k.startswith("all") else "--", label=lab)
        for f0 in [3e8, 3e9, 3e10]:
            ax.axvline(1 / f0 * 1e12 / (2 * np.pi), color=GREY, lw=0.7, ls=":")
            ax.text(1 / f0 * 1e12 / (2 * np.pi) * 1.1, 2.5e-5, f"1/(2 pi f), {fmt_f(f0)}", rotation=90, fontsize=7.5, color=GREY, va="bottom")
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(1, 2000); ax.set_ylim(1e-5, 1.2)
    ax.set_xlabel("flight time t (ps)"); ax.set_ylabel("fraction of electrons with flight time > t")
    ax.legend(fontsize=8, frameon=False, loc="lower left")
    ax.set_title("(c) The slow-drift tail behind the GHz decline (static, V = 0.1 V)", loc="left")
    fig.tight_layout()
    fig.savefig(f"{d}/B2_ellF_control.png", dpi=150, bbox_inches="tight"); print("wrote", f"{d}/B2_ellF_control.png")

    # ---------------- figure 3: amplitude ladder ------------------------------
    if b4:
        fig, axes = plt.subplots(1, 2, figsize=(12, 4.8))
        rs = sorted(b4, key=lambda r: r["V_V"])
        fk = field_key(rs[0]); f0 = rs[0]["f_Hz"]
        key = (geo, fk, "inf")
        Va = np.array([r["V_V"] for r in rs]); I = np.array([r["Idc_A"] for r in rs]); eI = np.array([r["Idc_err_A"] for r in rs])
        ax = axes[0]
        ax.errorbar(Va, I * 1e6, eI * 1e6, color=BLUE, marker="o", ms=6, lw=1.8, capsize=2, label=f"I_dc at {fmt_f(f0)}, {fk}", mec="white")
        if key in sc:
            vv = np.linspace(0.03, 0.32, 100)
            (I2a, I4a), _ = sc[key]._fit_even(0.2)  # one quartic fit to all four static points
            qs = 0.5 * I2a * vv ** 2 + 0.375 * I4a * vv ** 4
            ax.plot(vv, qs * 1e6, color=GREY, ls="--", lw=1.2, label="quasi-static <I(Va cos phi)>, quartic fit to B1")
            I2 = sc[key].qs(0.1)[2]
            ax.plot(vv, 0.5 * I2 * vv ** 2 * 1e6, color=GREY, ls=":", lw=1.2, label="pure square law I2 Va^2 / 2")
        ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.04, 0.35)
        ax.set_xlabel("drive amplitude Va across the device (V)"); ax.set_ylabel("rectified current (uA)")
        ax.legend(fontsize=8, frameon=False, loc="upper left")
        ax.set_title("(a) Amplitude ladder (B4)", loc="left")
        ax = axes[1]
        cols = [BLUES[0], BLUES[1], BLUES[2], "#000000"]
        for r, c in zip(rs, cols):
            hp = f"{d}/B4_Ehist_V{r['V_V']:g}.csv"
            if not os.path.exists(hp): continue
            hr = list(csv.DictReader(open(hp)))
            lo = np.array([float(x["Ek_lo_eV"]) for x in hr]); hi = np.array([float(x["Ek_hi_eV"]) for x in hr])
            cnt = np.array([float(x["count_S"]) + float(x["count_D"]) for x in hr])
            m = np.isfinite(hi) & (lo > 0)
            dens = cnt[m] / cnt.sum() / np.log(hi[m] / lo[m])
            ax.step(lo[m], dens, where="post", color=c, lw=1.6, label=f"Va = {r['V_V']:g} V, heated {r['f_heated']:.1e}")
        ax.axvline(0.138, color=GREY, lw=0.8, ls="--"); ax.text(0.142, 0.9, "E_F", fontsize=8, color=GREY, transform=ax.get_xaxis_transform())
        ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xlim(0.01, 2); ax.set_ylim(1e-5, 10)
        ax.set_xlabel("kinetic energy at the absorbing contact (eV)"); ax.set_ylabel("density per ln E")
        ax.legend(fontsize=8, frameon=False, loc="upper right")
        ax.set_title("(b) Exit energies: how much the ac field pumps the electrons", loc="left")
        fig.tight_layout()
        fig.savefig(f"{d}/B4_amplitude.png", dpi=150, bbox_inches="tight"); print("wrote", f"{d}/B4_amplitude.png")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "results/bthz")
