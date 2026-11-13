"""Shared analysis for Stage B8 (results/bpic): the production runs P1 to P4 of
plans/ballistic_selfconsistent.md, the pooled decision point, and the Stage B7
imposed-field numbers they are compared with (results/bshort)."""
import csv, math, os, sys
import numpy as np

sys.path.insert(0, os.path.dirname(__file__))
import short_funnel_common as b7

R = "results/bpic"
RANT = 75.0
VA = 0.1
GATES = (300, 30)
L, XF, ELL = 546.41, 446.41, 100.0          # device length, neck position, B7 neck-drop length (nm)
TEXT = ("label", "geo", "mode")


def rows(name):
    p = os.path.join(R, name)
    if not os.path.exists(p): return []
    out = []
    for r in csv.DictReader(open(p)):
        d = {k: (v if k in TEXT else float(v)) for k, v in r.items()}
        d["d"] = int(round(d["d_nm"])); d["f"] = d["f_Hz"] / 1e12
        d["I"] = d["Idc_A"] * 1e6; d["e"] = d["Idc_err_A"] * 1e6          # uA
        d["P"] = d["Pabs_W"] * 1e6                                          # uW
        d["Y"] = complex(d["ReY1_S"], d["ImY1_S"])
        d["split"] = abs(d["IdcS_A"] - d["IdcD_A"]) * 1e6                   # the two plane estimators, uA
        d["src"] = name
        out.append(d)
    return out


def is_base(r):   # the numerical settings of the checkpoint; only N, seed, window differ
    return (abs(r["h_nm"] - 5) < 0.1 and abs(r["dt_fs"] - 2.5) < 0.05 and r["filter"] == 1.5
            and r["clen_nm"] == 200 and int(r["cblock"]) == 2 and r["interaction"] == 1 and r["imposed"] == 0)


def pool(rs):
    """Weighted mean of I_dc (uA); error is the larger of the weighted error and the scatter of the mean."""
    w = np.array([1 / q["e"] ** 2 for q in rs]); v = np.array([q["I"] for q in rs])
    m = float((w * v).sum() / w.sum()); e = float(1 / math.sqrt(w.sum()))
    if len(v) > 1: e = max(e, float(np.std(v, ddof=1) / math.sqrt(len(v))))
    return m, e


def decision():
    """All base runs at the decision point with 4 million particles or more: {gate: dict}."""
    allr = rows("T30.csv") + rows("T30N.csv") + rows("maps.csv") + rows("P2.csv")
    out = {}
    for d in GATES:
        rs = [r for r in allr if r["d"] == d and r["geo"] == "funnel" and r["mode"] == "sym" and is_base(r)
              and abs(r["f"] - 1) < 1e-6 and abs(r["V_V"] - VA) < 1e-9 and r["N"] >= 4e6]
        m, e = pool(rs)
        P = float(np.mean([r["P"] for r in rs])); Y = complex(np.mean([r["Y"] for r in rs]))
        Pav = (VA * abs(1 + RANT * Y)) ** 2 / (8 * RANT) * 1e6
        chi2 = float(sum(((r["I"] - m) / r["e"]) ** 2 for r in rs))
        out[d] = dict(I=m, e=e, P=P, Y=Y, n=len(rs), R=m / P, eR=e / P, R1=m / Pav, eR1=e / Pav, chi2=chi2, runs=rs)
    return out


def static_fits():
    """{gate: fit} from P1, in the form of short_funnel_common.static_fit (currents in A)."""
    out = {}
    for d in GATES:
        st = {round(r["V_V"], 4): r for r in rows("P1.csv") if r["d"] == d}
        out[d] = b7.static_fit(st, VA) if len(st) == 4 else None
    return out


def b7_g3():
    """Stage B7, G3 at 700 nm: {field: (fit, rows)}."""
    st, ac = b7.load_all()
    return {fm: (b7.static_fit(st.get(("G3", fm, "700")), VA), ac.get(("G3", fm, "700"), [])) for fm in b7.FIELDS}


def series(geo, mode, d, extra=()):
    """Production rows for one device, drive and gate, sorted by frequency; the 1 THz shape-only funnel
    point is replaced by the pooled decision value."""
    rs = [r for n in ("P2.csv", "P3.csv", "P4.csv") + tuple(extra) for r in rows(n) if r["geo"] == geo and r["mode"] == mode and r["d"] == d]
    return sorted(rs, key=lambda r: r["f"])


def r_static(r, fit):
    """Stage B7 headline definition: per watt absorbed at the static conductance."""
    g = fit["Gqs"]
    return 2 * r["Idc_A"] / (VA ** 2 * g), 2 * r["Idc_err_A"] / (VA ** 2 * g)


def load_map(path):
    a = np.loadtxt(path)
    x = np.unique(a[:, 0]); y = np.unique(a[:, 1])
    assert len(x) * len(y) == len(a) and a[1, 0] != a[0, 0]
    return x, y, {k: a[:, i].reshape(len(y), len(x)) for k, i in (("mean", 2), ("cos", 3), ("sin", 4), ("rho", 5))}


def axis(x, y, field, half=26.0):
    return field[np.abs(y) < half].mean(0)


# ---- extension P5, P6 (tools/pic_ext.sh) ------------------------------------------
# cases of the extension: key -> (label, file, mean free path in nm, specularity, B7 key)
CASES = (("inf", "no scattering", "P5.csv", math.inf, 1.0),
         ("700", "700 nm", "P1.csv", 700.0, 1.0),
         ("100", "100 nm", "P5.csv", 100.0, 1.0),
         ("p05", "700 nm, half-diffuse walls", "P6.csv", 700.0, 0.5))


def _match(r, lam, p):
    return (r["lambda_nm"] == lam) and abs(r["p"] - p) < 1e-9 and r["geo"] == "funnel" and r["mode"] == "sym"


def ext_static(key, d):
    """Static pair at +-0.1 V: currents (uA), asymmetry, conductance from the odd part (S), and the
    quasi-static rectified current of a 0.1 V cosine from the even part taken as V^2 (uA)."""
    _, _, name, lam, p = [q for q in CASES if q[0] == key][0]
    st = {round(r["V_V"], 4): r for r in rows(name) if r["f"] == 0 and r["d"] == d and _match(r, lam, p)}
    if 0.1 not in st or -0.1 not in st: return None
    a, b = st[0.1], st[-0.1]
    return dict(Ip=a["I"], Im=b["I"], eIp=a["e"], eIm=b["e"], A=a["I"] / abs(b["I"]),
                eA=a["I"] / abs(b["I"]) * math.hypot(a["e"] / a["I"], b["e"] / b["I"]),
                G=(a["I"] - b["I"]) / (2 * VA) * 1e-6, Iqs=(a["I"] + b["I"]) / 4, eIqs=math.hypot(a["e"], b["e"]) / 4)


def ext_ac(key, d, V=VA):
    """1 THz, shape-only drive: dict with I, e (uA), P (uW), Y, R_abs, R_1. The 700 nm, 0.1 V case is the pooled decision
    point; other cases are single runs, pooled with a second seed where P6c has one."""
    _, _, name, lam, p = [q for q in CASES if q[0] == key][0]
    if key == "700" and abs(V - VA) < 1e-9:
        q = decision()[d]; out = dict(I=q["I"], e=q["e"], P=q["P"], Y=q["Y"], n=q["n"], split=float("nan"))
    else:
        src = "P6.csv" if key == "700" else name
        rs = [r for n in (src, "P6c.csv") for r in rows(n) if abs(r["f"] - 1) < 1e-6 and r["d"] == d and _match(r, lam, p) and abs(r["V_V"] - V) < 1e-9]
        if not rs: return None
        m, e = pool(rs)      # the half-diffuse case has a second seed (P6c)
        out = dict(I=m, e=e, P=float(np.mean([r["P"] for r in rs])), Y=complex(np.mean([r["Y"] for r in rs])), n=len(rs),
                   split=max(r["split"] for r in rs), row=rs[0], runs=rs)
    Pav = (V * abs(1 + RANT * out["Y"])) ** 2 / (8 * RANT) * 1e6
    out.update(R=out["I"] / out["P"], eR=out["e"] / out["P"], R1=out["I"] / Pav, eR1=out["e"] / Pav, V=V)
    return out


def b7_case(key):
    """Stage B7 imposed-field values for the same case: {field: dict(A, G, Iqs, I, e, Rs)} at 1 THz, 0.1 V
    (currents in uA; Rs is per watt absorbed at the static conductance)."""
    out = {}
    if key == "p05":
        s5 = [r for r in b7._rows("results/bshort/S5.csv") if r["geo"] == "G3p05"]
        for fm in b7.FIELDS:
            q = [r for r in s5 if r["field"] == fm]
            st = {round(r["V_V"], 4): r for r in q if r["f_Hz"] == 0}
            ac = [r for r in q if abs(r["f_Hz"] - 1e12) < 1][0]
            G = (st[0.1]["Idc_A"] - st[-0.1]["Idc_A"]) / (2 * VA)
            out[fm] = dict(A=st[0.1]["Idc_A"] / abs(st[-0.1]["Idc_A"]), G=G, Iqs=(st[0.1]["Idc_A"] + st[-0.1]["Idc_A"]) / 4 * 1e6,
                           I=ac["Idc_A"] * 1e6, e=ac["Idc_err_A"] * 1e6, Rs=2 * ac["Idc_A"] / (VA ** 2 * G))
        return out
    st, ac = b7.load_all()
    for fm in b7.FIELDS:
        s = st[("G3", fm, key)]; a = [r for r in ac[("G3", fm, key)] if abs(r["f_Hz"] - 1e12) < 1][0]
        G = (s[0.1]["Idc_A"] - s[-0.1]["Idc_A"]) / (2 * VA)
        out[fm] = dict(A=s[0.1]["Idc_A"] / abs(s[-0.1]["Idc_A"]), G=G, Iqs=(s[0.1]["Idc_A"] + s[-0.1]["Idc_A"]) / 4 * 1e6,
                       I=a["Idc_A"] * 1e6, e=a["Idc_err_A"] * 1e6, Rs=2 * a["Idc_A"] / (VA ** 2 * G))
    return out


def b7_amp():
    """Stage B7 amplitude ladder at 1 THz, 700 nm: {field: [(V, I_uA, e_uA)]} including 0.1 V."""
    out = {}
    s5 = [r for r in b7._rows("results/bshort/S5.csv") if r["geo"] == "G3amp"]
    _, ac = b7.load_all()
    for fm in b7.FIELDS:
        a = [r for r in ac[("G3", fm, "700")] if abs(r["f_Hz"] - 1e12) < 1][0]
        pts = [(r["V_V"], r["Idc_A"] * 1e6, r["Idc_err_A"] * 1e6) for r in s5 if r["field"] == fm] + [(0.1, a["Idc_A"] * 1e6, a["Idc_err_A"] * 1e6)]
        out[fm] = sorted(pts)
    return out
