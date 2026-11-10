"""Shared loaders for the Stage B2 CSVs in results/bthz."""
import csv, math, os
import numpy as np

FIELDS = {"uniform": ("uniform", None), "local175": ("local", 175.0), "local500": ("local", 500.0), "local1600": ("local", 1600.0)}
RANT = 75.0


def load(path):
    if not os.path.exists(path): return []
    rows = list(csv.DictReader(open(path)))
    out = []
    for r in rows:
        d = {}
        for k, v in r.items():
            if k in ("geo", "field"): d[k] = v
            else:
                try: d[k] = float(v)
                except ValueError: d[k] = float("nan")
        out.append(d)
    return out


def field_key(r):
    if r["field"] == "uniform": return "uniform"
    return f"local{int(round(r['ellF_nm']))}"


def lam_key(r):
    return "inf" if math.isinf(r["lambda_nm"]) else str(int(round(r["lambda_nm"])))


class StaticCurve:
    """Static I(V) from B1 for one (geo, field, lambda): even/odd polynomial fits
    E(V) = I2 V^2 + I4 V^4, O(V) = G1 V + I3 V^3 (two-sided estimator), with
    the quasi-static dc current for a cosine drive and its error."""

    def __init__(self, rows):
        by_v = {}
        for r in rows: by_v[round(r["V_V"], 6)] = r
        vs = sorted(v for v in by_v if v > 0 and -v in by_v)
        self.V = np.array(vs)
        Ip = np.array([by_v[v]["Idc_A"] for v in vs]); Im = np.array([by_v[-v]["Idc_A"] for v in vs])
        eIp = np.array([by_v[v]["Idc_err_A"] for v in vs]); eIm = np.array([by_v[-v]["Idc_err_A"] for v in vs])
        self.E = 0.5 * (Ip + Im); self.eE = 0.5 * np.hypot(eIp, eIm)
        self.O = 0.5 * (Ip - Im); self.eO = self.eE
        self.Ip, self.Im = Ip, Im
        self.Ibw_p = np.array([by_v[v]["Ibw_A"] for v in vs]); self.Ibw_m = np.array([by_v[-v]["Ibw_A"] for v in vs])
        self.rows = rows

    def _fit_even(self, vmax):
        m = self.V <= vmax * 1.0001
        if m.sum() < 2: m = np.ones_like(self.V, bool)
        A = np.stack([self.V[m] ** 2, self.V[m] ** 4], 1)
        W = 1.0 / self.eE[m]
        coef, *_ = np.linalg.lstsq(A * W[:, None], self.E[m] * W, rcond=None)
        cov = np.linalg.inv((A * W[:, None]).T @ (A * W[:, None]))
        return coef, cov

    def _fit_odd(self, vmax):
        m = self.V <= vmax * 1.0001
        if m.sum() < 2: m = np.ones_like(self.V, bool)
        A = np.stack([self.V[m], self.V[m] ** 3], 1)
        W = 1.0 / self.eO[m]
        coef, *_ = np.linalg.lstsq(A * W[:, None], self.O[m] * W, rcond=None)
        cov = np.linalg.inv((A * W[:, None]).T @ (A * W[:, None]))
        return coef, cov

    def qs(self, Va):
        """quasi-static <I(Va cos phi)> and its error"""
        (I2, I4), cov = self._fit_even(max(Va, 0.1))
        g = np.array([0.5 * Va ** 2, 0.375 * Va ** 4])
        return float(g @ np.array([I2, I4])), float(math.sqrt(g @ cov @ g)), I2, I4

    def G_qs(self, Va):
        (G1, I3), cov = self._fit_odd(max(Va, 0.1))
        g = np.array([1.0, 0.75 * Va ** 2])
        return float(g @ np.array([G1, I3])), float(math.sqrt(g @ cov @ g)), G1, I3

    def RI_qs(self, Va, Rant=RANT):
        I, eI, _, _ = self.qs(Va)
        G, _, _, _ = self.G_qs(Va)
        Vant = Va * abs(1 + Rant * G)
        Pav = Vant ** 2 / (8 * Rant)
        return I / Pav, eI / Pav, Pav


def static_curves(b1rows):
    groups = {}
    for r in b1rows: groups.setdefault((r["geo"], field_key(r), lam_key(r)), []).append(r)
    return {k: StaticCurve(v) for k, v in groups.items()}


def fmt_f(f):
    return f"{f/1e9:g} GHz" if f < 1e12 else f"{f/1e12:g} THz"
