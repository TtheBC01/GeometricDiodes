#!/usr/bin/env python
"""Render a .fld dump from gd_hydro_fields as PNG: speed (top) and pressure (bottom).
usage: hydro_png.py in.fld out.png [x0 x1 y0 y1]  (crop window in W units)"""
import sys, math
from PIL import Image, ImageDraw

def load(path):
    with open(path) as f:
        nv, nt, scale = f.readline().split(); nv, nt = int(nv), int(nt)
        nodes = [tuple(map(float, f.readline().split())) for _ in range(nv)]
        tris = [tuple(map(int, f.readline().split())) for _ in range(nt)]
    return nodes, tris

def cmap(t):  # 0..1 -> viridis-like
    t = max(0.0, min(1.0, t))
    stops = [(68, 1, 84), (59, 82, 139), (33, 145, 140), (94, 201, 98), (253, 231, 37)]
    k = t * (len(stops) - 1); i = min(int(k), len(stops) - 2); f = k - i
    return tuple(int(stops[i][c] * (1 - f) + stops[i + 1][c] * f) for c in range(3))

def main():
    inp, out = sys.argv[1], sys.argv[2]
    nodes, tris = load(inp)
    if len(sys.argv) > 6:
        x0, x1, y0, y1 = map(float, sys.argv[3:7])
    else:
        x0 = min(n[0] for n in nodes); x1 = max(n[0] for n in nodes)
        y0 = min(n[1] for n in nodes); y1 = max(n[1] for n in nodes)
    Wpx = 1800; s = Wpx / (x1 - x0); Hpx = int((y1 - y0) * s) + 1
    speed = [math.hypot(n[2], n[3]) for n in nodes]
    pres = [n[4] for n in nodes]
    sel = [i for i, n in enumerate(nodes) if x0 <= n[0] <= x1 and y0 <= n[1] <= y1]
    smax = max(speed[i] for i in sel); pmin = min(pres[i] for i in sel); pmax = max(pres[i] for i in sel)
    img = Image.new("RGB", (Wpx + 2, 2 * Hpx + 12), "white"); d = ImageDraw.Draw(img)
    P = lambda x, y, off: ((x - x0) * s + 1, (y1 - y) * s + 1 + off)
    for a, b, c in tris:
        pts = [nodes[a], nodes[b], nodes[c]]
        if all(p[0] < x0 for p in pts) or all(p[0] > x1 for p in pts) or all(p[1] < y0 for p in pts) or all(p[1] > y1 for p in pts):
            continue
        v = (speed[a] + speed[b] + speed[c]) / 3 / smax
        d.polygon([P(p[0], p[1], 0) for p in pts], fill=cmap(v), outline=cmap(v))
        q = ((pres[a] + pres[b] + pres[c]) / 3 - pmin) / max(1e-300, pmax - pmin)
        d.polygon([P(p[0], p[1], Hpx + 10) for p in pts], fill=cmap(q), outline=cmap(q))
    d.text((6, 4), f"|u| / {smax:.2f}", fill="white"); d.text((6, Hpx + 14), f"p: {pmin:.1f} .. {pmax:.1f}", fill="white")
    img.save(out)

if __name__ == "__main__":
    main()
