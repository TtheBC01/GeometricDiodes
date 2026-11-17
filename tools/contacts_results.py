"""Stage B10, Part A (plans/ballistic_contacts.md): tables and figure for the
0.55 um funnel between contacts that reflect part of the electrons.
Reads results/bpic/P10c.csv, P10t.csv, P10k.csv, P10l.csv, P10n.csv, P10s.csv,
P10r.csv and the maps P10s_T*_V*.txt (tools/pic_ext.sh P10c P10t P10k P10s
P10l P10n P10r);
writes results/bpic/tables_B10.md and results/bpic/B10_contacts.png.
usage: .venv/bin/python tools/contacts_results.py"""
import math, os, sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
sys.path.insert(0, os.path.dirname(__file__))
import pic_common as c

KF = math.sqrt(math.pi * 1.4e12 * 1e-14)       # per nm
HV = 0.6582119569                                # hbar v_F, eV nm
EF, KT = HV * KF, 8.617333e-5 * 300.0
G0 = 2 * 7.748091729e-5                          # 4 e^2 / h, S
G_LEAD = G0 * 500 / (math.pi * HV) * KT * math.log(1 + math.exp(EF / KT))   # Sharvin conductance of the 500 nm lead at 300 K, S
RSH_UM = 1e-3 / (G0 * KF / math.pi)              # ballistic resistance of a wide strip, ohm um
FLOOR = 0.05                                     # uA
TC = (0.75, 0.5, 0.25)
MATCH = {7.0: 0.5, 38.0: 0.25}                   # junction width (nm) -> the transparency with the same strip conductance
BLUE, VERM, GREEN, GREY, INK, PURP = "#0072B2", "#D55E00", "#009E73", "#6e6e6e", "#222222", "#5e2a84"


def tlm(T):
    """Contact resistance per contact as a transfer-length measurement reports it (ohm um)."""
    return 0.5 * RSH_UM * (2 - T) / T


def base_static(d): return c.ext_static("700", d)
def base_ac(d): return c.ext_ac("700", d)


def series(T, d):
    """Guess S: the share of the applied voltage left on the device when each contact adds (1 - T) / T of the
    lead's ballistic resistance in series with the device as measured between ideal contacts."""
    return 1.0 / (1.0 + 2 * (base_static(d)["G"] / G_LEAD) * (1 - T) / T)


def junction_avg(dj, kind):
    """Averages of the junction transmission exp(-pi k d s^2) over the sine s of the angle and the wave number:
    'arrive' (weight k f, the share of arrivals of a gas at rest that is absorbed) or 'strip' (weight k (-df/dE),
    T / (2 - T), the conductance of a clean strip over the ideal value)."""
    E = np.linspace(1e-4, EF + 30 * KT, 3001); f = 1 / (1 + np.exp((E - EF) / KT)); k = E / HV
    s = np.linspace(0, 1, 2001)
    T = np.exp(-math.pi * dj * k[:, None] * s[None, :] ** 2)
    w = k * f if kind == "arrive" else k * f * (1 - f)
    g = T if kind == "arrive" else T / (2 - T)
    return float(np.trapezoid(w * np.trapezoid(g, s, axis=1), E) / np.trapezoid(w, E))


def point(r):
    """One 1 THz run as a dict in the form of pic_common.ext_ac, with the heating and the absorbed share of arrivals."""
    Pav = (r["V_V"] * abs(1 + c.RANT * r["Y"])) ** 2 / (8 * c.RANT) * 1e6 if r["V_V"] else float("nan")
    q = dict(I=r["I"], e=r["e"], P=r["P"], Y=r["Y"], split=r["split"], N=r["N"], sec=r["seconds"],
             R=r["I"] / r["P"] if r["P"] else float("nan"), eR=r["e"] / r["P"] if r["P"] else float("nan"),
             R1=r["I"] / Pav if r["V_V"] else float("nan"), eR1=r["e"] / Pav if r["V_V"] else float("nan"),
             heat=r["Ek_last_eV"] / r["Ek0_eV"] - 1, hfirst=r["Ek_first_eV"] / r["Ek0_eV"] - 1,
             aS=r["absorb_S"] / r["arrive_S"] if r["arrive_S"] else 1.0, aD=r["absorb_D"] / r["arrive_D"] if r["arrive_D"] else 1.0)
    return q


def scan(name, key, d):
    """{value of column `key`: point} for the 1 THz funnel runs of one file at gate d."""
    return {r[key]: point(r) for r in c.rows(name) if r["d"] == d and r["f"] > 0}


def static_pairs():
    """{T: dict} from P10s (gate 300 nm), in the form of pic_common.ext_static."""
    out = {}
    for T in sorted({r["contact_T"] for r in c.rows("P10s.csv")}, reverse=True):
        st = {round(r["V_V"], 4): r for r in c.rows("P10s.csv") if r["contact_T"] == T}
        if 0.1 not in st or -0.1 not in st: continue
        a, b = st[0.1], st[-0.1]
        out[T] = dict(Ip=a["I"], Im=b["I"], eIp=a["e"], eIm=b["e"], A=a["I"] / abs(b["I"]),
                      eA=a["I"] / abs(b["I"]) * math.hypot(a["e"] / a["I"], b["e"] / b["I"]),
                      G=(a["I"] - b["I"]) / (2 * c.VA) * 1e-6, Iqs=(a["I"] + b["I"]) / 4, eIqs=math.hypot(a["e"], b["e"]) / 4)
    return out


def shares(path, V):
    """Share of the bias along the axis from a potential map: within 15 nm of the source plane, lead and taper to
    x = 350 nm, neck region 350 to 531 nm, within 15 nm of the drain plane (the split of Stage B8)."""
    x, y, f = c.load_map(path)
    u = c.axis(x, y, f["mean"])
    at = lambda xx: float(np.interp(xx, x, u))
    cuts = (0.0, 15.0, 350.0, c.L - 15.0, c.L)
    return [(at(cuts[i]) - at(cuts[i + 1])) / V for i in range(4)], (x, u / V)


def temperature(heat):
    """The temperature (K) of a gas at rest of the same density whose mean kinetic energy is (1 + heat) times that at
    300 K. The driven gas is not thermal and part of the energy is ordered motion: a measure of the heating, no more."""
    E = np.linspace(1e-5, 8 * EF, 40001)
    def mean(T):
        kt = 8.617333e-5 * T
        lo, hi = 0.0, 2 * EF                      # chemical potential at fixed density
        for _ in range(60):
            mu = 0.5 * (lo + hi); f = 1 / (1 + np.exp(np.clip((E - mu) / kt, -700, 700)))
            if np.trapezoid(E * f, E) > 0.5 * EF * EF: hi = mu
            else: lo = mu
        return np.trapezoid(E * E * f, E) / np.trapezoid(E * f, E)
    target = (1 + heat) * mean(300.0); lo, hi = 250.0, 900.0
    for _ in range(40):
        T = 0.5 * (lo + hi)
        if mean(T) > target: hi = T
        else: lo = T
    return T


def contact_points():
    """For the paper figure: ({gate: {transparency: point pooled over every run}}, {gate: {junction width in nm: point}})."""
    tr = {d: scan("P10t.csv", "contact_T", d) for d in c.GATES}
    rr = {d: scan("P10r.csv", "contact_T", d) for d in c.GATES}
    nn = {(r["d"], r["contact_T"]): point(r) for r in c.rows("P10n.csv")}
    pooled = {d: {T: pool([tr[d][T]] + ([rr[d][T]] if T in rr[d] else []) + ([nn[(d, T)]] if (d, T) in nn else [])) for T in tr[d]} for d in c.GATES}
    return pooled, {d: scan("P10k.csv", "junction_nm", d) for d in c.GATES}


def pool(ps):
    """Inverse-variance mean of independent runs of one configuration (I, e, R, eR, P)."""
    w = np.array([1 / q["e"] ** 2 for q in ps]); I = float(np.sum(w * [q["I"] for q in ps]) / w.sum()); e = float(1 / math.sqrt(w.sum()))
    P = float(np.mean([q["P"] for q in ps]))
    return dict(I=I, e=e, P=P, R=I / P, eR=e / P, n=len(ps))


def main():
    L = []
    w = L.append
    w("# Stage B10, Part A: the short funnel between contacts that reflect\n")
    w("Generated by `tools/contacts_results.py` from `results/bpic/P10*.csv`. The 0.55 um funnel, mirror walls, mean free path "
      "700 nm, shape-only wiring; 1 THz and 0.1 V unless stated. Currents in uA, powers in uW, responses in A/W. "
      "\"Ideal\" is the contact of Stages B8 and B9 (pooled runs). With f the share of the applied voltage that a series contact "
      "resistance leaves on the device, \"series\" is what a series resistor gives (I_dc as f cubed, R_abs as f squared; plan, "
      "section 11) and \"registered\" is guess S as the plan's section 7 registered it before the runs (f squared and f), which "
      "left out that the rectified current also flows through the contacts.\n")

    # ---- 1 controls
    w("## 1. Controls\n")
    w("| run | contact | gate (nm) | I_dc | kinetic energy change |")
    w("|---|---|---|---|---|")
    for r in c.rows("P10c.csv"):
        p = point(r)
        con = f"junction {r['junction_nm']:g} nm" if r["junction_nm"] > 0 else f"transparency {r['contact_T']:g}"
        what = "mirror-symmetric funnel, driven" if r["geo"] == "symmetric" else "funnel, no drive"
        w(f"| {what} | {con} | {r['d']} | {p['I']:+.3f} +- {p['e']:.3f} | {100 * p['heat']:+.2f} % |")
    w("")

    # ---- 2 transparency
    w("## 2. Contact transparency\n")
    w("| transparency | contact resistance (ohm um) | gate (nm) | I_dc | series | ratio to series | registered | ratio to registered | P_abs | series | R_abs | series | registered | R_1 | kinetic energy change |")
    w("|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|")
    tr = {d: scan("P10t.csv", "contact_T", d) for d in c.GATES}
    for d in c.GATES:
        b = base_ac(d)
        w(f"| 1 (ideal) | {tlm(1):.0f} | {d} | {b['I']:+.3f} +- {b['e']:.3f} | | | | | {b['P']:.3f} | | {b['R']:.3f} +- {b['eR']:.3f} | | | {b['R1']:.4f} | |")
        for T in TC:
            if T not in tr[d]: continue
            p = tr[d][T]; f = series(T, d)
            w(f"| {T:g} | {tlm(T):.0f} | {d} | {p['I']:+.3f} +- {p['e']:.3f} | {b['I'] * f ** 3:.3f} | {p['I'] / (b['I'] * f ** 3):.2f} +- {p['e'] / (b['I'] * f ** 3):.2f} | "
              f"{b['I'] * f * f:.3f} | {p['I'] / (b['I'] * f * f):.2f} +- {p['e'] / (b['I'] * f * f):.2f} | "
              f"{p['P']:.3f} | {b['P'] * f:.3f} | {p['R']:.3f} +- {p['eR']:.3f} | {b['R'] * f * f:.3f} | {b['R'] * f:.3f} | {p['R1']:.4f} | {100 * p['heat']:+.2f} % ({temperature(p['heat']):.0f} K) |")
    w("")
    hb = np.mean([r["Ek_last_eV"] / r["Ek0_eV"] - 1 for r in c.rows("P8.csv") + c.rows("P9.csv") if r["f"] > 0])
    w(f"Kinetic energy change: mean kinetic energy per electron in the last period over its value before the drive; in brackets the temperature of a gas "
      f"at rest with that energy. Between ideal contacts (the wall runs of Stages B8 and B9) the same drive gives {100 * hb:+.2f} % ({temperature(hb):.0f} K), steady after the settling time. "
      f"The model has no loss of energy to the lattice: heat leaves only through the contacts.\n")

    # ---- 2b second seed
    rr = {d: scan("P10r.csv", "contact_T", d) for d in c.GATES} if os.path.exists(os.path.join(c.R, "P10r.csv")) else {d: {} for d in c.GATES}
    nn = {(r["d"], r["contact_T"]): point(r) for r in c.rows("P10n.csv")}
    pooled = {d: {} for d in c.GATES}
    for d in c.GATES:
        for T in tr[d]:
            pooled[d][T] = pool([tr[d][T]] + ([rr[d][T]] if T in rr[d] else []) + ([nn[(d, T)]] if (d, T) in nn else []))
    if any(rr[d] for d in c.GATES):
        w("Second seed (11) where the current came out above the series value by more than a quarter (plan, section 7, outcome 4); settling time 10 ps for both. "
          "The pooled value takes every run of the configuration, the 16 million particle run of section 4 included.\n")
        w("| transparency | gate (nm) | first seed | second seed | difference (s.e.) | kinetic energy change, second seed | runs pooled | pooled I_dc | ratio to series | ratio to registered | pooled R_abs | series |")
        w("|---|---|---|---|---|---|---|---|---|---|---|---|")
        for d in c.GATES:
            b = base_ac(d)
            for T in TC:
                if T not in rr[d]: continue
                a, q, m = tr[d][T], rr[d][T], pooled[d][T]; f = series(T, d)
                w(f"| {T:g} | {d} | {a['I']:+.3f} +- {a['e']:.3f} | {q['I']:+.3f} +- {q['e']:.3f} | {(q['I'] - a['I']) / math.hypot(a['e'], q['e']):+.1f} | {100 * q['heat']:+.2f} % | {m['n']} | "
                  f"{m['I']:+.3f} +- {m['e']:.3f} | {m['I'] / (b['I'] * f ** 3):.2f} +- {m['e'] / (b['I'] * f ** 3):.2f} | {m['I'] / (b['I'] * f * f):.2f} +- {m['e'] / (b['I'] * f * f):.2f} | "
                  f"{m['R']:.3f} +- {m['eR']:.3f} | {b['R'] * f * f:.3f} |")
        w("")

    # ---- 3 junctions
    w("## 3. Collimating contacts\n")
    w("The junction widths were chosen so that a clean strip conducts as between contacts of transparency 0.5 and 0.25 "
      "(at the Fermi wave vector; with the thermal spread the strip value is given below).\n")
    w("| junction (nm) | matched transparency | strip conductance over ideal (formula) | share of arrivals absorbed, source / drain (formula for a gas at rest) | gate (nm) | I_dc | "
      "matched transparency run | difference (s.e.) | ratio to series | P_abs | R_abs | R_1 | kinetic energy change |")
    w("|---|---|---|---|---|---|---|---|---|---|---|---|---|")
    jn = {d: scan("P10k.csv", "junction_nm", d) for d in c.GATES}
    for d in c.GATES:
        b = base_ac(d)
        for dj in sorted(jn[d]):
            p = jn[d][dj]; T = MATCH.get(dj); m = tr[d].get(T); f = series(T, d)
            diff = (p["I"] - m["I"]) / math.hypot(p["e"], m["e"]) if m else float("nan")
            w(f"| {dj:g} | {T:g} | {junction_avg(dj, 'strip'):.3f} ({T / (2 - T):.3f}) | {p['aS']:.3f} / {p['aD']:.3f} ({junction_avg(dj, 'arrive'):.3f}) | {d} | {p['I']:+.3f} +- {p['e']:.3f} | "
              f"{m['I']:+.3f} +- {m['e']:.3f} | {diff:+.1f} | {p['I'] / (b['I'] * f ** 3):.2f} +- {p['e'] / (b['I'] * f ** 3):.2f} | {p['P']:.3f} | {p['R']:.3f} +- {p['eR']:.3f} | {p['R1']:.4f} | {100 * p['heat']:+.2f} % ({temperature(p['heat']):.0f} K); {100 * (p['hfirst']):+.2f} % in the first period |")
    w("")

    # ---- 4 long leads, 5 particle number
    w("## 4. Ideal contacts moved out (leads of 300 nm) and the particle-number check\n")
    w("| run | gate (nm) | particles | I_dc | reference | ratio | P_abs | R_abs | reference R_abs | Re Y, Im Y (mS) |")
    w("|---|---|---|---|---|---|---|---|---|---|")
    for r in c.rows("P10l.csv"):
        p = point(r); b = base_ac(r["d"])
        w(f"| leads 300 nm, ideal contacts | {r['d']} | {r['N'] / 1e6:.0f} million | {p['I']:+.3f} +- {p['e']:.3f} | {b['I']:.3f} +- {b['e']:.3f} (leads 100 nm) | {p['I'] / b['I']:.2f} +- {p['e'] / b['I']:.2f} | "
          f"{p['P']:.3f} | {p['R']:.3f} +- {p['eR']:.3f} | {b['R']:.3f} | {p['Y'].real * 1e3:.3f}, {p['Y'].imag * 1e3:+.3f} (leads 100 nm: {b['Y'].real * 1e3:.3f}, {b['Y'].imag * 1e3:+.3f}) |")
    for r in c.rows("P10n.csv"):
        p = point(r); m = tr[r["d"]].get(r["contact_T"])
        w(f"| transparency {r['contact_T']:g} | {r['d']} | {r['N'] / 1e6:.0f} million | {p['I']:+.3f} +- {p['e']:.3f} | {m['I']:+.3f} +- {m['e']:.3f} (8 million) | {p['I'] / m['I']:.2f} | "
          f"{p['P']:.3f} | {p['R']:.3f} +- {p['eR']:.3f} | {m['R']:.3f} | {p['Y'].real * 1e3:.3f}, {p['Y'].imag * 1e3:+.3f} |")
    w("")

    # ---- 6 static
    w("## 5. Static curve behind reflecting contacts (gate 300 nm)\n")
    w("| transparency | I(+0.1 V) | I(-0.1 V) | asymmetry | G (mS) | G over ideal | series | quasi-static I_dc | series |")
    w("|---|---|---|---|---|---|---|---|---|")
    b = base_static(300); sp = static_pairs()
    w(f"| 1 (ideal) | {b['Ip']:.2f} | {b['Im']:.2f} | {b['A']:.3f} +- {b['eA']:.3f} | {b['G'] * 1e3:.3f} | 1 | 1 | {b['Iqs']:.2f} +- {b['eIqs']:.2f} | |")
    for T, s in sp.items():
        f = series(T, 300)
        w(f"| {T:g} | {s['Ip']:.2f} | {s['Im']:.2f} | {s['A']:.3f} +- {s['eA']:.3f} | {s['G'] * 1e3:.3f} | {s['G'] / b['G']:.3f} | {f:.3f} | {s['Iqs']:.2f} +- {s['eIqs']:.2f} | {b['Iqs'] * f ** 3:.2f} |")
    w("")
    w("Share of the bias along the axis (|y| < 26 nm):\n")
    w("| transparency, bias | within 15 nm of the source plane | lead and taper, to x = 350 nm | neck region, 350 to 531 nm | within 15 nm of the drain plane | both planes | series, both planes |")
    w("|---|---|---|---|---|---|---|")
    prof = {}
    for T, tag in ((1.0, "P1_d300_V{V}.txt"), (0.5, "P10s_T0.5_V{V}.txt"), (0.25, "P10s_T0.25_V{V}.txt")):
        for V in (0.1, -0.1):
            path = os.path.join(c.R, tag.format(V=V))
            if not os.path.exists(path): continue
            sh, pr = shares(path, V); prof[(T, V)] = pr
            f = series(T, 300); b0 = prof.get("base")
            if T == 1.0 and V == 0.1: prof["base"] = sh[0] + sh[3]
            guess = "" if T == 1.0 else f"{1 - f * (1 - prof['base']):.3f}"
            w(f"| {T:g}, {V:+.1f} V | {sh[0]:.3f} | {sh[1]:.3f} | {sh[2]:.3f} | {sh[3]:.3f} | {sh[0] + sh[3]:.3f} | {guess} |")
    w("")

    # ---- 7 bookkeeping
    w("## 6. Bookkeeping\n")
    allr = [r for n in ("P10c.csv", "P10t.csv", "P10k.csv", "P10l.csv", "P10n.csv", "P10s.csv", "P10r.csv") if os.path.exists(os.path.join(c.R, n)) for r in c.rows(n)]
    if allr:
        w(f"{len(allr)} runs, {sum(r['seconds'] for r in allr) / 3600:.2f} hours of wall time; {min(r['seconds'] for r in allr) / 60:.1f} to "
          f"{max(r['seconds'] for r in allr) / 60:.1f} minutes each. Largest difference between the source-plane and drain-plane estimators of I_dc: "
          f"{max(r['split'] for r in allr):.3f} uA. Particles lost through a wall: at most {int(max(r['leaked'] for r in allr))} in a run. "
          f"Largest charge error: {max(r['charge_err'] for r in allr):.1e} electrons.\n")
    open(os.path.join(c.R, "tables_B10.md"), "w").write("\n".join(L))
    print("\n".join(L))

    # ================================================================ figure
    plt.rcParams.update({"font.size": 9.5, "axes.titlesize": 10.5, "axes.spines.top": False, "axes.spines.right": False})
    fig = plt.figure(figsize=(14.5, 10.4))
    gs = fig.add_gridspec(1, 2, left=0.06, right=0.985, top=0.895, bottom=0.57, wspace=0.2)
    gs2 = fig.add_gridspec(1, 2, left=0.19, right=0.985, top=0.44, bottom=0.065, wspace=0.26)
    Tg = np.linspace(0.2, 1.0, 300)
    sec_f = (lambda t: 0.5 * RSH_UM * (2 - np.clip(t, 1e-3, None)) / np.clip(t, 1e-3, None), lambda r: RSH_UM / (np.clip(r, 1e-3, None) + 0.5 * RSH_UM))
    for col, (key, ek, ylab, title) in enumerate((("I", "e", "rectified current at 1 THz, 0.1 V (uA)", "(a)  Rectified current against the contact"),
                                                    ("R", "eR", "response per absorbed watt (A/W)", "(b)  Response per absorbed watt"))):
        ax = fig.add_subplot(gs[0, col]); ax.set_title(title, loc="left", pad=26)
        if key == "I": ax.axhspan(-FLOOR, FLOOR, color="#dddddd", lw=0)
        for d, ls, mfc in ((300, "-", None), (30, (0, (5, 2)), "white")):
            b = base_ac(d); f = np.array([series(t, d) for t in Tg])
            ax.plot(Tg, b[key] * (f ** 3 if key == "I" else f * f), color=GREY, lw=1.6, ls=ls)
            if d == 300: ax.plot(Tg, b[key] * (f * f if key == "I" else f), color=GREY, lw=1.0, ls=":")
            xs = [1.0] + [T for T in TC if T in tr[d]]
            ys = [b[key]] + [pooled[d][T][key] for T in xs[1:]]; es = [b[ek]] + [pooled[d][T][ek] for T in xs[1:]]
            ax.errorbar([t + (0.006 if d == 300 else -0.006) for t in xs], ys, yerr=es, fmt="o", color=BLUE, mfc=BLUE if mfc is None else mfc, ms=7, capsize=3, lw=1.2,
                        label=f"one transparency for all angles, gate {d} nm")
            xj = [MATCH[dj] * 0.94 for dj in sorted(jn[d])]
            ax.errorbar(xj, [jn[d][dj][key] for dj in sorted(jn[d])], yerr=[jn[d][dj][ek] for dj in sorted(jn[d])], fmt="D", color=VERM,
                        mfc=VERM if mfc is None else mfc, ms=6.5, capsize=3, lw=1.2, label=f"collimating contact, gate {d} nm")
        ax.plot([], [], color=GREY, lw=1.6, label="series resistor; dashed: gate 30 nm")
        ax.plot([], [], color=GREY, lw=1.0, ls=":", label="guess S as registered (gate 300 nm)")
        ax.set_xlim(1.04, 0.19); ax.set_xticks([1.0, 0.75, 0.5, 0.25]); ax.set_xlabel("transparency of each contact"); ax.set_ylabel(ylab)
        sec = ax.secondary_xaxis("top", functions=sec_f); sec.set_xticks([50, 100, 150, 250, 350]); sec.set_xlabel("contact resistance of each contact (ohm um)")
        ax.spines["top"].set_visible(False)
        if key == "I": ax.text(0.99, 0.0, "numerical floor", ha="left", va="center", fontsize=8.4, color=GREY)
        else: ax.set_ylim(0, None); ax.legend(frameon=False, fontsize=8.6, loc="lower left")

    # (c) ratio to the guess, every run
    ax = fig.add_subplot(gs2[0, 0]); ax.set_title("(c)  Every contact against a series resistor", loc="left", pad=16)
    ax.axvspan(0.75, 1.25, color=BLUE, alpha=0.10, lw=0); ax.axvline(1.0, color=GREY, lw=1.0)
    labels, y = [], 0
    rows_ = [(f"transparency {T:g}", "t", T) for T in TC] + [(f"collimating, junction {dj:g} nm\n(conducts as transparency {MATCH[dj]:g})", "j", dj) for dj in sorted(MATCH)] + [("ideal contacts, leads of 300 nm\n(over the value with leads of 100 nm)", "l", None)]
    ll = {r["d"]: point(r) for r in c.rows("P10l.csv")}
    for lab, kind, v in rows_:
        for d, mfc, off in ((300, None, 0.16), (30, "white", -0.16)):
            b = base_ac(d)
            if kind == "t" and v in tr[d]: p, g = pooled[d][v], b["I"] * series(v, d) ** 3
            elif kind == "j" and v in jn[d]: p, g = jn[d][v], b["I"] * series(MATCH[v], d) ** 3
            elif kind == "l" and d in ll: p, g = ll[d], b["I"]
            else: continue
            col = BLUE if kind == "t" else VERM if kind == "j" else GREEN
            ax.errorbar([p["I"] / g], [y + off], xerr=[p["e"] / g], fmt="o" if kind != "j" else "D", color=col, mfc=col if mfc is None else mfc, ms=7, capsize=3)
        labels.append(lab); y -= 1
    ax.set_yticks(range(0, y, -1)); ax.set_yticklabels(labels); ax.set_ylim(y + 0.5, 0.6)
    ax.set_xlabel("rectified current over the series-resistor value (full: gate 300 nm, open: 30 nm)")
    ax.text(1.0, 0.62, "within a quarter", ha="center", va="bottom", fontsize=8.6, color=BLUE)
    ax.spines["left"].set_visible(False); ax.tick_params(axis="y", length=0)

    # (d) potential along the axis
    ax = fig.add_subplot(gs2[0, 1]); ax.set_title("(d)  Where the bias drops: static, +0.1 V, gate 300 nm", loc="left")
    for T, col, lab in ((1.0, INK, "ideal contacts"), (0.5, BLUE, "transparency 0.5"), (0.25, VERM, "transparency 0.25")):
        if (T, 0.1) in prof:
            x, u = prof[(T, 0.1)]; m = (x >= 0) & (x <= c.L)
            ax.plot(x[m], u[m], color=col, lw=2.0, label=lab)
    ax.axvline(c.XF, color=GREY, lw=0.8, ls=":"); ax.text(c.XF - 6, 0.44, "neck", ha="right", va="center", fontsize=8.6, color=GREY)
    ax.set_xlabel("position along the axis (nm)"); ax.set_ylabel("electron potential energy over the bias"); ax.set_xlim(0, c.L); ax.legend(frameon=False, fontsize=8.8, loc="lower left")
    fig.savefig(os.path.join(c.R, "B10_contacts.png"), dpi=170)
    print("wrote", os.path.join(c.R, "B10_contacts.png"))


if __name__ == "__main__":
    main()
