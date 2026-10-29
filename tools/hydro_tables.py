#!/usr/bin/env python3
"""Print markdown tables of D-1 from gd_hydro_sweep stdout logs.
usage: hydro_tables.py log.txt [log2.txt ...]   (rows: geometry, Da, S; columns: Re)"""
import re, sys
from collections import OrderedDict
pat = re.compile(r"^(\S+)\s+Da\s+([\d.]+)\s+S\s+(\S+)\s+Re\s+([\d.]+)\s+\|\s+Pi_f\s+([-\d.e+]+)\s+Pi_r\s+([-\d.e+]+)\s+\|\s+D-1\s+([-+\d.e]+)\s+\|.*?umax\s+([\d.]+)\s+([\d.]+)\s*(NOT CONVERGED)?")
for path in sys.argv[1:]:
    table = OrderedDict(); Res = set()
    for line in open(path):
        m = pat.match(line)
        if not m: continue
        g, Da, S, Re, Pf, Pr, Dm1, uf, ur, nc = m.groups()
        Re = float(Re); Res.add(Re)
        table.setdefault((g, float(Da), S), {})[Re] = (float(Dm1), nc is None, float(Pf))
    Res = sorted(Res)
    print(f"### {path}\n")
    print("| geometry | Da | S | Pi_f (lowest Re) | " + " | ".join(f"Re {Re:g}" for Re in Res) + " |")
    print("|---|---|---|---|" + "---|" * len(Res))
    for (g, Da, S), d in table.items():
        cells = [(f"{d[Re][0]:+.1e}" + ("" if d[Re][1] else " (nc)")) if Re in d else "-" for Re in Res]
        print(f"| {g} | {Da:g} | {S} | {d[min(d)][2]:.0f} | " + " | ".join(cells) + " |")
    print()
