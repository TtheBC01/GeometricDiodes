"""Shared analysis for Stage B7 (results/bshort): static fits, transfer
function, and the responsivity definitions of results/BALLISTIC_SHORT.md."""
import csv, math, os, glob
import numpy as np

R = "results/bshort"
RANT = 75.0
VF = 1e6
KB = 1.380649e-23
DEV = ["G0", "G1", "G2", "G3", "G4"]
LEN = {"G0": 6000.5, "G1": 3000.25, "G2": 2080.5, "G3": 546.41, "G4": 500.5}     # nm
NAME = {"G0": "Device D, 6.0 um", "G1": "half scale, 3.0 um", "G2": "short leads, 2.1 um", "G3": "short funnel, 0.55 um", "G4": "Zhu-like, 0.50 um"}
LAMS = ["inf", "700", "100"]
FIELDS = ["uniform", "local"]


def _rows(path):
    if not os.path.exists(path): return []
    out = []
    for r in csv.DictReader(open(path)):
        d = {}
        for k, v in r.items():
            if k in ("geo", "field", "shape"): d[k] = v
            else:
                try: d[k] = float(v)
                except (ValueError, TypeError): d[k] = float("nan")
        d["lam"] = "inf" if math.isinf(d["lambda_nm"]) else str(int(round(d["lambda_nm"])))
        out.append(d)
    return out


def load_all():
    """dict: static[(g, field, lam)] -> {V: row}; ac[(g, field, lam)] -> rows sorted by f (Va = 0.1)."""
    static, ac = {}, {}
    files = [(g, f"{R}/S1_{g}.csv", f"{R}/S2_{g}.csv") for g in DEV[1:]]
    for g, s1, s2 in files:
        for r in _rows(s1): static.setdefault((g, r["field"], r["lam"]), {})[round(r["V_V"], 4)] = r
        for r in _rows(s2):
            if abs(r["V_V"] - 0.1) < 1e-9: ac.setdefault((g, r["field"], r["lam"]), []).append(r)
    # G0 from Stage B2 (uniform and the 500 nm drop) plus the S3 extension
    for fn, fm in (("B2u", "uniform"), ("B2l", "local")):
        for r in _rows(f"results/bthz/{fn}.csv"):
            if abs(r["V_V"] - 0.1) < 1e-9 and r["p"] == 1: ac.setdefault(("G0", fm, r["lam"]), []).append(r)
    for r in _rows(f"{R}/S3.csv"): ac.setdefault(("G0", r["field"], r["lam"]), []).append(r)
    for r in _rows("results/bthz/B1.csv"):
        if r["field"] == "uniform" or abs(r["ellF_nm"] - 500) < 1:
            static.setdefault(("G0", r["field"], r["lam"]), {})[round(r["V_V"], 4)] = r
    for k in ac: ac[k].sort(key=lambda r: r["f_Hz"])
    return static, ac


def static_fit(st, Va=0.1):
    """Even/odd parts from the +-0.05 and +-0.1 V points. Returns dict with A, G_qs,
    I_qs (quasi-static dc current of a cosine drive of amplitude Va) and its error."""
    need = (0.1, -0.1, 0.05, -0.05)
    if st is None or any(v not in st for v in need): return None
    I = {v: st[v]["Idc_A"] for v in need}; e = {v: st[v]["Idc_err_A"] for v in need}
    E1, E5 = 0.5 * (I[0.1] + I[-0.1]), 0.5 * (I[0.05] + I[-0.05])
    eE1, eE5 = 0.5 * math.hypot(e[0.1], e[-0.1]), 0.5 * math.hypot(e[0.05], e[-0.05])
    O1, O5 = 0.5 * (I[0.1] - I[-0.1]), 0.5 * (I[0.05] - I[-0.05])
    # E = I2 V^2 + I4 V^4,  O = G1 V + I3 V^3
    a, b = 0.1, 0.05
    I4 = (E1 / a**2 - E5 / b**2) / (a**2 - b**2); I2 = E1 / a**2 - I4 * a**2
    I3 = (O1 / a - O5 / b) / (a**2 - b**2); G1 = O1 / a - I3 * a**2
    # I_qs = I2 Va^2/2 + 3 I4 Va^4/8 is linear in (E1, E5): coefficients by substitution
    def iqs(E1_, E5_):
        i4 = (E1_ / a**2 - E5_ / b**2) / (a**2 - b**2); i2 = E1_ / a**2 - i4 * a**2
        return i2 * Va**2 / 2 + 3 * i4 * Va**4 / 8
    c1, c5 = iqs(1, 0), iqs(0, 1)
    return dict(A=I[0.1] / abs(I[-0.1]), A05=I[0.05] / abs(I[-0.05]), Ip=I[0.1], Im=I[-0.1], I2=I2, I4=I4, G1=G1, I3=I3,
                Gqs=G1 + 0.75 * I3 * Va**2, Iqs=iqs(E1, E5), Iqs_err=math.hypot(c1 * eE1, c5 * eE5))


def figures(r, fit, Va=0.1):
    """Derived figures for one ac row r with its static fit."""
    Y = complex(r["ReY1_S"], r["ImY1_S"]); I, eI = r["Idc_A"], r["Idc_err_A"]
    out = dict(f=r["f_Hz"], I=I, eI=eI, Y=Y)
    Pav1 = (Va * abs(1 + RANT * Y))**2 / (8 * RANT)
    out["R_single"] = I / Pav1; out["eR_single"] = eI / Pav1
    if fit:
        G = fit["Gqs"]
        out["N"] = 1.0 / (RANT * G)               # parallel devices that bring the static resistance to R_ant
        # headline: per watt absorbed at the static conductance = array of N matched at its static
        # resistance, its impedance taken as frequency independent (the model has no displacement current)
        out["R_abs"] = 2 * I / (Va**2 * G); out["eR_abs"] = 2 * eI / (Va**2 * G)
        # sensitivity: per watt absorbed by the conduction current at the drive frequency (conjugate match);
        # only where Re Y_1 is at least 0.3 G, since Re Y_1 of an imposed-field model can vanish or turn negative
        ok = Y.real > 0.3 * G
        out["R_cm"] = 2 * I / (Va**2 * Y.real) if ok else float("nan"); out["eR_cm"] = 2 * eI / (Va**2 * Y.real) if ok else float("nan")
        out["S"] = I / fit["Iqs"]; out["eS"] = abs(out["S"]) * math.hypot(eI / I if I else 0, fit["Iqs_err"] / fit["Iqs"])
        out["Ssig"] = abs(fit["Iqs"]) / fit["Iqs_err"]
        out["NEP"] = math.sqrt(4 * KB * 300 / RANT) / abs(out["R_abs"]) if out["R_abs"] else float("inf")
    return out
