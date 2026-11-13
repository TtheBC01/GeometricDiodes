"""Stage H5: incompressible reference for every row of results/dcgate/D{1,2,3}.csv.
Groups the rows by device, parameter set, wall condition and mesh and calls
gd_dc_gate_ref with the matching Reynolds numbers. Writes results/dcgate/ref.csv.
usage: .venv/bin/python tools/dc_gate_ref.py"""
import csv, os, subprocess, collections
R = "results/dcgate"
groups = collections.OrderedDict()
for fn in ("D1", "D2", "D3"):
    p = f"{R}/{fn}.csv"
    if not os.path.exists(p): continue
    for r in csv.DictReader(open(p)):
        if r["geo"] == "chan": continue
        groups.setdefault((r["geo"], r["set"], r["slip"], r["hw"], r["hm"]), []).append(r["Re"])
out = f"{R}/ref.csv"
if os.path.exists(out): os.remove(out)
for (geo, st, slip, hw, hm), res in groups.items():
    cmd = ["./build/gd_dc_gate_ref", "--geo", geo, "--set", st, "--slip", slip, "--hw", hw, "--hm", hm, "--Re", ",".join(res), "--out", out]
    print(" ".join(cmd), flush=True)
    subprocess.run(cmd, check=False)
