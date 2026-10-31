"""Markdown tables for the Stage H3 section of results/THZ.md from the
gd_thz_power CSVs in results/thz (P1_*, P2_*, P3_*, P4_*).
usage: thz_power_tables.py results/thz
For each run: a table of eta_av, eta_small, their ratio, n_min and the
Thevenin correction against the available power; then a summary with the
peak efficiency, the power at the peak, the saturation onset (eta_av 10 %
below the quadratic law) and the breakdown amplitude.
"""
import csv, glob, math, os, sys


def load(path):
    rows = list(csv.DictReader(open(path)))
    for r in rows:
        for k in r:
            if k not in ("geo", "set", "term"): r[k] = float(r[k])
    return rows


def fmt_f(f):
    return f"{f/1e9:.0f} GHz" if f < 1e12 else f"{f/1e12:g} THz"


def fmt_P(P):
    if P < 1e-6: return f"{P*1e9:.3g} nW"
    if P < 1e-3: return f"{P*1e6:.3g} uW"
    return f"{P*1e3:.3g} mW"


def main():
    d = sys.argv[1]
    files = sorted(glob.glob(os.path.join(d, "P[1-4]_*.csv")))
    summary = []
    for p in files:
        rows = load(p)
        if not rows: continue
        name = os.path.basename(p)[:-4]
        r0 = rows[0]
        print(f"### {name}: {r0['geo']} {r0['set']} {r0['term']} at {fmt_f(r0['f_Hz'])}, R_dc {r0['Rdc_ohm']:.0f} ohm, R_L {r0['RL_ohm']:.0f} ohm\n")
        print("| Va | P_av | P_in/P_av | eta_av | eta_small (BDF2) | ratio | eta_abs | n_min | n_max | tail | corr | drift | ok |")
        print("|---|---|---|---|---|---|---|---|---|---|---|---|---|")
        for r in rows:
            ratio = r["eta_av"] / r["eta_small_bdf"] if r["eta_small_bdf"] > 0 else float("nan")
            print(f"| {r['Va']:g} | {fmt_P(r['Pav_W'])} | {r['Pin_W']/r['Pav_W']:.3f} | {r['eta_av']:.3e} | {r['eta_small_bdf']:.3e} | {ratio:.3f} | "
                  f"{r['eta_abs']:.3e} | {r['n_min']:.3f} | {r['n_max']:.3f} | {r['tail']:.1e} | {r['corr']:+.1e} | {r['dc_drift']:.1e} | "
                  f"{'yes' if r['ok'] > 0.5 else 'no (breakdown)'} |")
        print()
        good = [r for r in rows if r["ok"] > 0.5]
        if not good: continue
        pk = max(good, key=lambda r: r["eta_av"])
        onset = next((r for r in good if r["eta_av"] < 0.9 * r["eta_small_bdf"]), None)
        bad = next((r for r in rows if r["ok"] < 0.5), None)
        summary.append((name, r0, pk, onset, bad, good[-1]))
    print("### Efficiency at fixed available power\n")
    print("eta_av interpolated in log-log between the computed amplitudes (quadratic law below the first point); the amplitude at 1 mW is Va 0.52, i.e. 0.78 V.\n")
    PREF = [1e-6, 1e-5, 1e-4, 1e-3]
    print("| run | set | term | f | " + " | ".join(fmt_P(P) for P in PREF) + " | quadratic law eta / P_av (1/W) |")
    print("|" + "---|" * (len(PREF) + 5))
    for name, r0, pk, onset, bad, last in summary:
        rows = [r for r in load(os.path.join(d, name + ".csv")) if r["ok"] > 0.5]
        def at(P):
            if P <= rows[0]["Pav_W"]: return rows[0]["eta_av"] * P / rows[0]["Pav_W"]
            if P > rows[-1]["Pav_W"]: return float("nan")
            for a, b in zip(rows, rows[1:]):
                if a["Pav_W"] <= P <= b["Pav_W"]:
                    t = math.log(P / a["Pav_W"]) / math.log(b["Pav_W"] / a["Pav_W"])
                    return math.exp(math.log(a["eta_av"]) + t * math.log(b["eta_av"] / a["eta_av"]))
        cells = [("-" if math.isnan(at(P)) else f"{at(P):.1e}") for P in PREF]
        print(f"| {name} | {r0['set']} | {r0['term']} | {fmt_f(r0['f_Hz'])} | " + " | ".join(cells) + f" | {rows[0]['eta_small'] / rows[0]['Pav_W']:.2f} |")
    print()
    print("### Summary\n")
    print("| run | set | term | f | peak eta_av | at P_av | eta/quadratic at peak | n_min at peak | saturation onset (eta 10 % below quadratic) | last converged Va | breakdown |")
    print("|---|---|---|---|---|---|---|---|---|---|---|")
    for name, r0, pk, onset, bad, last in summary:
        print(f"| {name} | {r0['set']} | {r0['term']} | {fmt_f(r0['f_Hz'])} | {pk['eta_av']:.2e} | {fmt_P(pk['Pav_W'])} (Va {pk['Va']:g}) | "
              f"{pk['eta_av']/pk['eta_small_bdf']:.2f} | {pk['n_min']:.2f} | "
              f"{('Va ' + format(onset['Va'], 'g') + ', ' + fmt_P(onset['Pav_W'])) if onset else 'none in range'} | {last['Va']:g} | "
              f"{('Va ' + format(bad['Va'], 'g') + (' (n <= 0)' if bad['n_min'] <= 0 else ' (Newton)')) if bad else 'none'} |")
    print()


if __name__ == "__main__":
    main()
