"""Stage H6, check T33: compare the legacy-closure runs of tools/momentum_regress.sh with the
result files stored before the correction. Criterion 1e-8 relative on every compared number."""
import csv, math, sys

def rows(path):
    return list(csv.DictReader(open(path)))

def close(a, b):
    return abs(math.log(float(a) / float(b))) < 1e-6

worst_all = 0.0
def compare(new, old, keys, cols, label):
    global worst_all
    n, o = rows(new), rows(old)
    worst, npts = 0.0, 0
    for r in n:
        m = [q for q in o if all(close(r[k], q[k]) if k not in ("geo", "set", "slip", "term") else r[k] == q[k] for k in keys)]
        if not m:
            print(f"   {label}: no stored row for {[r[k] for k in keys]}"); worst = max(worst, 1.0); continue
        for c in cols:
            a, b = float(r[c]), float(m[0][c])
            if b == 0 and a == 0: continue
            worst = max(worst, abs(a - b) / max(abs(b), 1e-300)); npts += 1
    worst_all = max(worst_all, worst)
    print(f"{'PASS' if worst < 1e-8 else 'FAIL'}  {label}: {len(n)} rows, {npts} numbers, largest relative difference {worst:.1e}")

T, N = "results/thz", "results/thz2"
compare(f"{N}/T33_chan_100K_ds.csv", f"{T}/S1_chan_100K_ds.csv", ["f_Hz"], ["Voc_per_Va2", "ReZ_ohm", "Pin_over_Pav"], "uniform channel, 100 K, ideal source (S1)")
compare(f"{N}/T33_funnel_w1000_100K_dss.csv", f"{T}/S2_funnel_w1000_100K_dss.csv", ["f_Hz"], ["Isc_per_Va2", "Voc_per_Va2", "RI_A_per_W", "Rdc_ohm"], "1 um funnel, 100 K, DS termination (S2)")
compare(f"{N}/T33_funnel_w1000_100K_sym.csv", f"{T}/S2_funnel_w1000_100K_sym.csv", ["f_Hz"], ["Isc_per_Va2", "Voc_per_Va2", "RI_A_per_W", "Rdc_ohm"], "1 um funnel, 100 K, symmetric termination (S2)")
compare(f"{N}/T33_power_100K_dss_f3e11.csv", f"{T}/P1_funnel_w1000_100K_dss_f3e11.csv", ["Va"], ["Istar_A", "Vrect_V", "eta_av", "n_min", "n_max", "Pin_W"], "1 um funnel, time domain, 0.3 THz, Va 0.01 and 0.3 (P1)")
compare(f"{N}/T33_dcgate.csv", "results/dcgate/D2.csv", ["geo", "set", "slip", "hw", "v_target"], ["V_a", "V_b", "V_c", "V_d", "D_swap_dn", "D_flip0"], "175 nm funnel, steady gated flow, 300 K, slip (D2)")
print(f"T33 {'PASS' if worst_all < 1e-8 else 'FAIL'}: largest relative difference {worst_all:.1e} (criterion 1e-8)")
sys.exit(0 if worst_all < 1e-8 else 1)
