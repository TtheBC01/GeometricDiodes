"""Stage H5 tables (results/dcgate/tables_H5.md) from D1 to D4 and ref.csv.
usage: .venv/bin/python tools/dc_gate_tables.py"""
import sys, os, math
sys.path.insert(0, os.path.dirname(__file__))
from dc_gate_common import rows, ref, ref_of, NAME, R

refd = ref()
out = ["# Stage H5 tables: steady gated flow under the two reversal protocols", "",
       "Columns: target bias V / U_0 (U_0 = 1.43 V); current; voltage of run a; Reynolds number I / (n e nu); peak drift speed over the four",
       "solves; swap-contacts asymmetry with the gate at the downstream and at the upstream contact; the incompressible Stage H value at the same",
       "Re, damping and walls; polarity-flip asymmetry; its one-dimensional closed form without inertia. Rows with a drift speed above 0.3 v_F are",
       "outside the fluid model's range and are marked with *.", ""]
for fn, title in (("D1", "D1: controls"), ("D2", "D2: funnels"), ("D3", "D3: classical Tesla valve")):
    rs = rows(fn)
    if not rs: continue
    out += [f"## {title}", ""]
    keys = []
    for r in rs:
        k = (r["geo"], r["set"], r["slip"])
        if k not in keys: keys.append(k)
    for k in keys:
        sub = [r for r in rs if (r["geo"], r["set"], r["slip"]) == k]
        wall = "slip walls" if math.isinf(k[2]) else "no-slip walls"
        out += [f"**{NAME[k[0]]}, {k[1]} set, {wall}** (R = {sub[0]['R_ohm']:.0f} ohm, {int(sub[0]['nodes'])} nodes)", "",
                "| V / U_0 | I (uA) | V (mV) | Re | u / v_F | swap, gate downstream | swap, gate upstream | incompressible | flip | 1-D flip |",
                "|---|---|---|---|---|---|---|---|---|---|"]
        for r in sub:
            Dinc, ok = ref_of(refd, r)
            star = "*" if r["umax_vF"] > 0.3 else ""
            nc = "" if r["ok"] else " (not converged)"
            inc = "n/a" if math.isnan(Dinc) else f"{Dinc - 1:+.2e}"
            out.append(f"| {r['v_target']:g}{star} | {r['I_A'] * 1e6:.2f} | {r['V_a_V'] * 1e3:.2f} | {r['Re']:.3f} | {r['umax_vF']:.3f} | "
                       f"{r['D_swap_dn'] - 1:+.2e} | {r['D_swap_up'] - 1:+.2e} | {inc} | {r['D_flip0'] - 1:+.3e} | {r['D_1d_flip'] - 1:+.3e} |{nc}")
        out.append("")
rs = rows("D4")
if rs:
    out += ["## D4: mesh convergence", "",
            "| device | wall mesh (nm) | nodes | V / U_0 | swap, gate downstream | swap, gate upstream | flip |", "|---|---|---|---|---|---|---|"]
    for r in rs:
        out.append(f"| {NAME[r['geo']]} | {r['hw']:g} | {int(r['nodes'])} | {r['v_target']:g} | {r['D_swap_dn'] - 1:+.3e} | {r['D_swap_up'] - 1:+.3e} | {r['D_flip0'] - 1:+.4e} |")
    out.append("")
open(f"{R}/tables_H5.md", "w").write("\n".join(out))
print("\n".join(out))
