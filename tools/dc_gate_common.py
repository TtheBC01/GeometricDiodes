"""Loaders for Stage H5 (results/dcgate)."""
import csv, os, math
R = "results/dcgate"
NAME = {"chan": "straight channel", "symcon": "symmetric constriction", "funnel": "funnel, 175 nm neck", "funnel_w1000": "funnel, 1 um neck",
        "tesla_classic": "Tesla valve, 175 nm", "tesla_classic_w1000": "Tesla valve, 1 um"}


def rows(fn):
    p = f"{R}/{fn}.csv"
    if not os.path.exists(p): return []
    out = []
    for r in csv.DictReader(open(p)):
        d = {k: (v if k in ("geo", "set", "closure") else float(v)) for k, v in r.items()}
        out.append(d)
    return out


def ref():
    d = {}
    for r in rows("ref"):
        d[(r["geo"], r["set"], r["slip"], r["hw"], round(r["Re"], 6))] = (r["D_inc"], r["ok"])
    return d


def ref_of(refd, r):
    return refd.get((r["geo"], r["set"], r["slip"], r["hw"], round(r["Re"], 6)), (float("nan"), 0))
