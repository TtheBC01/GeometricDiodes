"""Markdown tables for results/THZ.md from the gd_thz_sweep CSVs in results/thz.
usage: thz_tables.py results/thz
Prints: S1 table with the Dyakonov-Shur shallow-water formula at the same
(Omega, Gamma); S2/S3 tables of responsivity at reference frequencies and the
peak responsivity with its frequency; impedance at the same frequencies.
"""
import csv, glob, math, os, re, sys


def ds_sw(Om, G, L):
    wt = Om / G; r = math.sqrt(1 + 1 / wt ** 2)
    kp = Om * math.sqrt((r + 1) / 2); kpp = Om * math.sqrt((r - 1) / 2)
    beta = 2 * wt / math.sqrt(1 + wt ** 2); sh = math.sinh(kpp * L); c = math.cos(kp * L)
    return (1 + beta - (1 + beta * math.cos(2 * kp * L)) / (sh * sh + c * c)) / 4


def load(path):
    rows = list(csv.DictReader(open(path)))
    if not rows:
        return None, float("nan"), float("nan")
    for r in rows:
        for k in r:
            if k not in ("geo", "set", "term"): r[k] = float(r[k])
    txt = open(path[:-4] + ".txt").read()
    m = re.search(r"Gamma ([\d.e+-]+), 1/Re_s", txt)
    gamma = float(m.group(1)) if m else float("nan")
    m = re.search(r"dc resistance [\d.]+ R_unit = ([\d.]+) ohm", txt)
    rdc = float(m.group(1)) if m else float("nan")
    return rows, gamma, rdc


def nearest(rows, f):
    return min(rows, key=lambda r: abs(math.log(r["f_Hz"] / f)))


REF = [1e10, 3e10, 1e11, 3e11, 1e12, 3e12]


def fmt_f(f):
    return f"{f/1e9:.0f} GHz" if f < 1e12 else f"{f/1e12:g} THz"


def main():
    d = sys.argv[1]
    # S1
    print("### S1: uniform 1 um channel, DS detector (ideal source, open drain), V_dc / Ua^2 in units of V_unit\n")
    s1 = sorted(glob.glob(os.path.join(d, "S1_*.csv")))
    if s1:
        hdr = "| f | Omega | " + " | ".join(os.path.basename(p)[8:-7] for p in s1) + " | DS shallow water (100 K Gamma) | DS (300 K Gamma) |"
        print(hdr); print("|" + "---|" * (hdr.count("|") - 1))
        data = [d_ for d_ in (load(p) for p in s1) if d_[0]]
        g100 = [g for (_, g, _) in data if abs(g - 0.056) < 0.01]; g300 = [g for (_, g, _) in data if abs(g - 0.28) < 0.05]
        for f in REF:
            cells = []
            for rows, g, _ in data:
                r = nearest(rows, f); cells.append(f"{r['Voc_per_Va2']:+.3f}")
            r0 = nearest(data[0][0], f)
            Om = r0["Omega"]
            cells.append(f"{ds_sw(Om, g100[0], 10):.3f}" if g100 else "-")
            cells.append(f"{ds_sw(Om, g300[0], 10):.3f}" if g300 else "-")
            print(f"| {fmt_f(r0['f_Hz'])} | {Om:.3f} | " + " | ".join(cells) + " |")
        print()
    # S2/S3: per device, rows = (set, term)
    for tag, title in (("S2_funnel_", "S2: funnel, 175 nm neck, 2 um leads 6 um long"),
                       ("S2_funnel_short_", "S2 control: the same funnel with 1 um leads"),
                       ("S2_funnel_w1000_", "S2: funnel scaled to a 1 um neck, 4 um leads"),
                       ("S3_tesla_classic_", "S3: classical Tesla valve, 175 nm")):
        files = sorted(p for p in glob.glob(os.path.join(d, tag + "*.csv"))
                       if "_fine" not in p and (tag != "S2_funnel_" or ("w1000" not in p and "short" not in p)))
        if not files: continue
        print(f"### {title}\n")
        print("Current responsivity R_I (A/W) at 75 ohm; the last two columns give the peak |R_I| and its frequency; R_dc in ohm.\n")
        print("| set | term | " + " | ".join(fmt_f(f) for f in REF) + " | peak |R_I| | at | R_dc |")
        print("|" + "---|" * (len(REF) + 5))
        for p in files:
            rows, g, rdc = load(p)
            if not rows: continue
            name = os.path.basename(p)[len(tag):-4]
            st, term = name.rsplit("_", 1)
            cells = [f"{nearest(rows, f)['RI_A_per_W']:+.2e}" for f in REF]
            pk = max(rows, key=lambda r: abs(r["RI_A_per_W"]))
            print(f"| {st} | {term} | " + " | ".join(cells) + f" | {abs(pk['RI_A_per_W']):.2e} | {fmt_f(pk['f_Hz'])} | {rdc:.0f} |")
        print()
        print("Noise-equivalent power (W/Hz^1/2), Johnson noise of R_dc at the set temperature: current readout "
              "sqrt(4 k T / R_dc) / R_I and voltage readout sqrt(4 k T R_dc) / R_V (identical when R_V = R_I R_dc):\n")
        print("| set | term | " + " | ".join(fmt_f(f) for f in REF) + " | best | at |")
        print("|" + "---|" * (len(REF) + 4))
        kB = 1.380649e-23
        for p in files:
            rows, g, rdc = load(p)
            if not rows: continue
            name = os.path.basename(p)[len(tag):-4]
            st, term = name.rsplit("_", 1)
            T = 100.0 if st.startswith("100") else 300.0
            nep = lambda r: math.sqrt(4 * kB * T / rdc) / abs(r["RI_A_per_W"]) if r["RI_A_per_W"] != 0 else float("inf")
            cells = [f"{nep(nearest(rows, f)):.1e}" for f in REF]
            best = min(rows, key=nep)
            print(f"| {st} | {term} | " + " | ".join(cells) + f" | {nep(best):.1e} | {fmt_f(best['f_Hz'])} |")
        print()
        print("Voltage responsivity R_V (V/W), open circuit:\n")
        print("| set | term | " + " | ".join(fmt_f(f) for f in REF) + " | peak |R_V| | at |")
        print("|" + "---|" * (len(REF) + 4))
        for p in files:
            rows, g, rdc = load(p)
            if not rows: continue
            name = os.path.basename(p)[len(tag):-4]
            st, term = name.rsplit("_", 1)
            cells = [f"{nearest(rows, f)['RV_V_per_W']:+.2e}" for f in REF]
            pk = max(rows, key=lambda r: abs(r["RV_V_per_W"]))
            print(f"| {st} | {term} | " + " | ".join(cells) + f" | {abs(pk['RV_V_per_W']):.2e} | {fmt_f(pk['f_Hz'])} |")
        print()
        print("Device impedance Re Z / Im Z (ohm) seen by the antenna and the fraction of the available power absorbed (sym termination, 100K / 300K):\n")
        print("| set | " + " | ".join(fmt_f(f) for f in REF) + " |")
        print("|" + "---|" * (len(REF) + 1))
        for p in files:
            if not p.endswith("_sym.csv"): continue
            rows, g, rdc = load(p)
            if not rows: continue
            st = os.path.basename(p)[len(tag):-8]
            cells = [f"{nearest(rows, f)['ReZ_ohm']:.0f} / {nearest(rows, f)['ImZ_ohm']:.0f} ({nearest(rows, f)['Pin_over_Pav']:.2f})" for f in REF]
            print(f"| {st} | " + " | ".join(cells) + " |")
        print()
    fine_tables(d)


def fine_tables(d):
    for p in sorted(glob.glob(os.path.join(d, "*_fine.csv"))):
        rows, g, rdc = load(p)
        if not rows: continue
        print(f"### Fine scan: {os.path.basename(p)[:-4]}\n")
        print("| f (THz) | Re Z | Im Z | P_in/P_av | R_I (A/W) | R_V (V/W) |")
        print("|---|---|---|---|---|---|")
        for r in rows:
            print(f"| {r['f_Hz']/1e12:.3f} | {r['ReZ_ohm']:.0f} | {r['ImZ_ohm']:.0f} | {r['Pin_over_Pav']:.2f} | {r['RI_A_per_W']:+.3e} | {r['RV_V_per_W']:+.3e} |")
        print()


if __name__ == "__main__":
    main()
