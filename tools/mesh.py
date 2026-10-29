#!/usr/bin/env python
"""Triangulate a GeometricDiodes polygon export with gmsh.

usage: mesh.py in.poly out.mesh h_wall h_max [--png out.png] [--dist-max D]

Input (.poly, written by gd::hydro::write_poly, units nm):
    loops <n>
    loop <nverts>
    x y tag          # tag of the edge starting at this vertex: -1 wall, >=0 contact
Output (.mesh, 0-based indices):
    nodes N / x y ... / triangles M / a b c (CCW) ... / edges K / a b tag ...
Element size is h_wall within 2*h_wall of any wall, growing linearly to
h_max at distance dist_max (default 20*h_wall). Geometry vertices are always
honoured, so finely polygonised arcs are resolved at their own scale.
"""
import sys


def read_poly(path):
    loops = []
    with open(path) as f:
        toks = f.read().split()
    i = 0
    assert toks[i] == "loops"; nloops = int(toks[i + 1]); i += 2
    for _ in range(nloops):
        assert toks[i] == "loop"; nv = int(toks[i + 1]); i += 2
        pts = []
        for _ in range(nv):
            pts.append((float(toks[i]), float(toks[i + 1]), int(toks[i + 2]))); i += 3
        loops.append(pts)
    return loops


def main():
    args = [a for a in sys.argv[1:]]
    png = None; dist_max = None
    if "--png" in args:
        k = args.index("--png"); png = args[k + 1]; del args[k:k + 2]
    if "--dist-max" in args:
        k = args.index("--dist-max"); dist_max = float(args[k + 1]); del args[k:k + 2]
    inp, out, h_wall, h_max = args[0], args[1], float(args[2]), float(args[3])
    if dist_max is None:
        dist_max = 20 * h_wall
    loops = read_poly(inp)

    import gmsh
    gmsh.initialize()
    gmsh.option.setNumber("General.Terminal", 0)
    gmsh.option.setNumber("General.Verbosity", 1)
    gmsh.model.add("gd")
    geo = gmsh.model.geo
    lines_by_tag = {}
    curve_loops = []
    for pts in loops:
        n = len(pts)
        ptags = [geo.addPoint(x, y, 0.0) for (x, y, t) in pts]
        ltags = []
        for i in range(n):
            l = geo.addLine(ptags[i], ptags[(i + 1) % n])
            ltags.append(l)
            lines_by_tag.setdefault(pts[i][2], []).append(l)
        curve_loops.append(geo.addCurveLoop(ltags))
    surf = geo.addPlaneSurface(curve_loops)
    geo.synchronize()
    for tag, ls in lines_by_tag.items():
        gmsh.model.addPhysicalGroup(1, ls, tag + 2)  # -1 -> 1, contact c -> c + 2
    gmsh.model.addPhysicalGroup(2, [surf], 1)

    walls = lines_by_tag.get(-1, [])
    fd = gmsh.model.mesh.field.add("Distance")
    gmsh.model.mesh.field.setNumbers(fd, "CurvesList", walls)
    gmsh.model.mesh.field.setNumber(fd, "Sampling", 400)
    ft = gmsh.model.mesh.field.add("Threshold")
    gmsh.model.mesh.field.setNumber(ft, "InField", fd)
    gmsh.model.mesh.field.setNumber(ft, "SizeMin", h_wall)
    gmsh.model.mesh.field.setNumber(ft, "SizeMax", h_max)
    gmsh.model.mesh.field.setNumber(ft, "DistMin", 2 * h_wall)
    gmsh.model.mesh.field.setNumber(ft, "DistMax", dist_max)
    gmsh.model.mesh.field.setAsBackgroundMesh(ft)
    gmsh.option.setNumber("Mesh.MeshSizeExtendFromBoundary", 0)
    gmsh.option.setNumber("Mesh.MeshSizeFromPoints", 0)
    gmsh.option.setNumber("Mesh.MeshSizeFromCurvature", 0)
    gmsh.option.setNumber("Mesh.Algorithm", 6)  # Frontal-Delaunay
    gmsh.option.setNumber("Mesh.Smoothing", 5)
    gmsh.model.mesh.generate(2)

    node_tags, coords, _ = gmsh.model.mesh.getNodes()
    index = {int(t): i for i, t in enumerate(node_tags)}
    xy = [(coords[3 * i], coords[3 * i + 1]) for i in range(len(node_tags))]
    _, tri_nodes = gmsh.model.mesh.getElementsByType(2)
    tris = []
    for k in range(0, len(tri_nodes), 3):
        a, b, c = (index[int(tri_nodes[k + j])] for j in range(3))
        (xa, ya), (xb, yb), (xc, yc) = xy[a], xy[b], xy[c]
        if (xb - xa) * (yc - ya) - (xc - xa) * (yb - ya) < 0:
            b, c = c, b
        tris.append((a, b, c))
    edges = []
    for tag, ls in lines_by_tag.items():
        for l in ls:
            _, ln = gmsh.model.mesh.getElementsByType(1, l)
            for k in range(0, len(ln), 2):
                edges.append((index[int(ln[k])], index[int(ln[k + 1])], tag))
    gmsh.finalize()

    with open(out, "w") as f:
        f.write(f"nodes {len(xy)}\n")
        for x, y in xy:
            f.write(f"{x:.10g} {y:.10g}\n")
        f.write(f"triangles {len(tris)}\n")
        for a, b, c in tris:
            f.write(f"{a} {b} {c}\n")
        f.write(f"edges {len(edges)}\n")
        for a, b, t in edges:
            f.write(f"{a} {b} {t}\n")
    print(f"{out}: {len(xy)} nodes, {len(tris)} triangles, {len(edges)} boundary edges")

    if png:
        from PIL import Image, ImageDraw
        xs = [p[0] for p in xy]; ys = [p[1] for p in xy]
        x0, x1, y0, y1 = min(xs), max(xs), min(ys), max(ys)
        Wpx = 2400; s = Wpx / (x1 - x0); Hpx = int((y1 - y0) * s) + 1
        img = Image.new("RGB", (Wpx + 2, Hpx + 2), "white"); d = ImageDraw.Draw(img)
        P = lambda p: ((p[0] - x0) * s + 1, (y1 - p[1]) * s + 1)
        for a, b, c in tris:
            d.polygon([P(xy[a]), P(xy[b]), P(xy[c])], outline=(120, 120, 200))
        col = {-1: (0, 0, 0), 0: (200, 0, 0), 1: (0, 150, 0)}
        for a, b, t in edges:
            d.line([P(xy[a]), P(xy[b])], fill=col.get(t, (255, 0, 255)), width=2)
        img.save(png)


if __name__ == "__main__":
    main()
