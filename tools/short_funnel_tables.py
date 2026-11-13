"""Stage B7 tables (results/bshort/tables_B7.md).
usage: .venv/bin/python tools/short_funnel_tables.py"""
import sys, os, math
sys.path.insert(0, os.path.dirname(__file__))
from short_funnel_common import *

static, ac = load_all()
out = ["# Stage B7 tables: short funnels under THz drive", "",
       "Definitions (results/BALLISTIC_SHORT.md): A = I(+0.1 V) / |I(-0.1 V)|; G = static conductance at the drive amplitude; I_qs = quasi-static",
       "rectified current of a 0.1 V cosine drive from the static curve; N = 1 / (75 ohm x G), the number of devices in parallel that brings the",
       "static resistance to 75 ohm; S = I_dc / I_qs; R_1 = responsivity of one device against the available power of a 75 ohm antenna (with the",
       "device's own first-harmonic admittance Y_1); R_abs = I_dc per watt absorbed at the static conductance, i.e. the array of N matched to",
       "75 ohm with its impedance taken as frequency independent (the headline figure); R_cm = I_dc per watt absorbed by the conduction current at",
       "the drive frequency, 2 I_dc / (V_a^2 Re Y_1), quoted only where Re Y_1 > 0.3 G; NEP = Johnson noise of 75 ohm at 300 K over R_abs.", ""]
out += ["## Static", "", "| device | field | mean free path (nm) | lambda / L | A(0.1 V) | A(0.05 V) | G (mS) | I_qs (uA) | I_qs per absorbed watt (A/W) | N |",
        "|---|---|---|---|---|---|---|---|---|---|"]
for g in DEV:
    for fm in FIELDS:
        for lam in LAMS:
            fit = static_fit(static.get((g, fm, lam)))
            if not fit: continue
            ll = "inf" if lam == "inf" else f"{float(lam) / LEN[g]:.2f}"
            out.append(f"| {g} | {fm} | {lam} | {ll} | {fit['A']:.3f} | {fit['A05']:.3f} | {fit['Gqs'] * 1e3:.3f} | {fit['Iqs'] * 1e6:.2f} +- {fit['Iqs_err'] * 1e6:.2f} | "
                       f"{2 * fit['Iqs'] / (0.01 * fit['Gqs']):.2f} | {1 / (RANT * fit['Gqs']):.1f} |")
out.append("")
for g in DEV:
    out += [f"## {g}: {NAME[g]}", ""]
    for fm in FIELDS:
        for lam in LAMS:
            rows = ac.get((g, fm, lam))
            if not rows: continue
            fit = static_fit(static.get((g, fm, lam)))
            out += [f"**{fm} field, mean free path {lam} nm**", "",
                    "| f (THz) | f L / v_F | I_dc (uA) | S | Y_1 (mS) | R_1 (A/W) | R_abs (A/W) | R_cm (A/W) | NEP (pW/Hz^1/2) |", "|---|---|---|---|---|---|---|---|---|"]
            for r in rows:
                o = figures(r, fit)
                S = f"{o['S']:.2f} +- {o['eS']:.2f}" if fit and o["Ssig"] > 5 else "n/a"
                rn = f"{o['R_abs']:.3f} +- {o['eR_abs']:.3f}" if fit else "n/a"
                ra = f"{o['R_cm']:.2f}" if fit and not math.isnan(o["R_cm"]) else "n/a"
                nep = f"{o['NEP'] * 1e12:.0f}" if fit and abs(o["R_abs"]) > 3 * o["eR_abs"] else "n/a"
                out.append(f"| {r['f_Hz'] / 1e12:g} | {r['f_Hz'] * LEN[g] * 1e-9 / VF:.3f} | {o['I'] * 1e6:.3f} +- {o['eI'] * 1e6:.3f} | {S} | "
                           f"{o['Y'].real * 1e3:.3f}{o['Y'].imag * 1e3:+.3f}i | {o['R_single']:.3f} +- {o['eR_single']:.3f} | {rn} | {ra} | {nep} |")
            out.append("")
open(f"{R}/tables_B7.md", "w").write("\n".join(out))
print("\n".join(out[:60]))
