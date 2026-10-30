"""Classical (Tesla 1920 / Wikipedia-principle) multi-loop Tesla valve geometry.

Topology (blocking direction described, flow arriving along a main segment):
the main channel is a zig-zag of straight segments at +-theta. At every
vertex V the arriving segment continues STRAIGHT into a loop leg, while the
main channel turns by 2 theta. The loop wraps a teardrop island (half circle
of centreline radius R) and comes back anti-parallel to its leg, merging into
the main channel a distance s = 2R / sin(2 theta) downstream of V, head-on
against the stream that took the bend. The island lies between the loop leg
and the main channel; its tip points at the oncoming blocking flow.

In the unimpeded direction the flow runs along the main channel; the loop
mouth at the merge point opens backwards (180 - 2 theta turn) and at the vertex the
loop leg needs a 180 - 2 theta turn against 2 theta to stay in the main channel.

Forward (contact 0 -> 1, +x) is the unimpeded direction, reverse (1 -> 0) is
the blocking direction, matching D = Pi_rev / Pi_fwd used elsewhere.

The domain is the union of buffered centrelines (shapely), which resolves the
junction overlaps exactly, written in the .poly format read by
gd::load_boundary and tools/mesh.py.

Usage:
  tesla_classic.py [--w 175] [--theta 25] [--R 175] [--a 600] [--Ls 1000]
                   [--n 4] [--Wc 2000] [--Lc 1000] [--Wd 2000] [--Ld 1000]
                   [--lead 600] [--out meshes/tesla_classic.poly]
                   [--png results/geom_tesla_classic.png]
Lengths in nm, angles in degrees. a = loop leg length from the vertex to the
start of the half circle.
"""
import argparse, math, sys
import numpy as np
from shapely.geometry import LineString, Polygon, box
from shapely.ops import unary_union


def dir_(deg):
    a = math.radians(deg)
    return np.array([math.cos(a), math.sin(a)])


def loop_path(V, leg_deg, a, R, sign, step_deg=2.0):
    """Centreline of one loop attached at vertex V: straight leg of length a
    along leg_deg (the arriving blocking segment's direction), then a half
    circle of radius R turning towards sign (-1 clockwise for a loop above the
    channel, +1 counter-clockwise below). Returns (points, A, C, B)."""
    u = dir_(leg_deg)
    A = V + a * u
    C = A + R * dir_(leg_deg + sign * 90)
    nseg = max(8, int(math.ceil(180 / step_deg)))
    phi0 = leg_deg - sign * 90
    pts = [V, A]
    for i in range(1, nseg + 1):
        pts.append(C + R * dir_(phi0 + sign * 180 * i / nseg))
    B = pts[-1]
    return pts, A, C, B


def build(p):
    w, th, R, a, Ls, n = p.w, p.theta, p.R, p.a, p.Ls, p.n
    if R <= w / 2:
        sys.exit("R must exceed w/2 (island round end would vanish)")
    s_merge = 2 * R / math.sin(math.radians(2 * th))        # merge distance from V
    t_ret = a - 2 * R / math.tan(math.radians(2 * th))       # return leg length
    if t_ret < 0.5 * w:
        sys.exit(f"return leg too short ({t_ret:.0f} nm): increase --a above "
                 f"{2 * R / math.tan(math.radians(2 * th)) + 0.5 * w:.0f}")
    if Ls < s_merge + 2 * w:
        sys.exit(f"Ls too short: merge point at {s_merge:.0f} nm needs Ls >= {s_merge + 2 * w:.0f}")
    # --- blocking frame: blocking flow travels +x; first vertex is a top one.
    h = 0.5 * Ls * math.sin(math.radians(th))
    Q0 = np.array([0.0, -h])
    verts = []
    x = Ls * math.cos(math.radians(th))
    for k in range(n):
        verts.append(np.array([x, h if k % 2 == 0 else -h]))
        x += Ls * math.cos(math.radians(th))
    Q1 = np.array([x, -h if n % 2 == 0 else h])
    main = [Q0 - np.array([p.lead, 0.0]), Q0] + verts + [Q1, Q1 + np.array([p.lead, 0.0])]
    paths = [main]
    loops = []
    # the return leg meets the main centreline at 180 - 4 theta... its acute
    # angle with the main line is 180 - 2*(90 - ...) = 60 deg for theta = 30;
    # extend the cap past M by less than (w/2) tan(acute/2) so it stays inside
    acute = 180.0 - 2.0 * (180.0 - 2.0 * th) if th > 45 else 2.0 * th
    ext = 0.9 * 0.5 * w * math.tan(math.radians(0.5 * acute))
    for k, V in enumerate(verts):
        top = (k % 2 == 0)
        leg = th if top else -th            # arriving blocking segment direction
        sign = -1 if top else +1            # loop above turns clockwise
        pts, A, C, B = loop_path(V, leg, a, R, sign)
        M = V + s_merge * dir_(-leg)        # on the outgoing main segment
        ret = dir_(leg + 180)
        pts.append(M + ext * ret)
        paths.append(pts)
        loops.append(dict(V=V, A=A, C=C, B=B, M=M))
    # --- union of buffered centrelines
    shapes = [LineString([tuple(q) for q in P]).buffer(w / 2, cap_style="flat", join_style="round", quad_segs=45)
              for P in paths]
    xmin = main[0][0]; xmax = main[-1][0]
    src = box(xmin - p.Lc, -p.Wc / 2, xmin + w / 2, p.Wc / 2)
    drn = box(xmax - w / 2, -p.Wd / 2, xmax + p.Ld, p.Wd / 2)
    dom = unary_union(shapes + [src, drn]).simplify(1e-3)
    if dom.geom_type != "Polygon":
        sys.exit("domain is not a single polygon: " + dom.geom_type)
    if len(dom.interiors) != n:
        sys.exit(f"expected {n} islands, got {len(dom.interiors)}: loops overlap a lead or each other; "
                 "increase --lead or --Ls")
    # --- mirror x so that the unimpeded direction is +x; source face at x = 0
    def mir(ring):
        return [(-xx, yy) for xx, yy in ring]
    ext_ring = mir(dom.exterior.coords)
    holes = [mir(r.coords) for r in dom.interiors]
    x0 = min(xx for xx, _ in ext_ring)
    ext_ring = [(xx - x0, yy) for xx, yy in ext_ring]
    holes = [[(xx - x0, yy) for xx, yy in r] for r in holes]
    poly = Polygon(ext_ring, holes)
    mp = lambda P: np.array([-P[0] - x0, P[1]])
    info = dict(L=max(xx for xx, _ in ext_ring), n=n, h=h, s_merge=s_merge, t_ret=t_ret,
                island_tip_offset=(w / 2) / math.sin(math.radians(th)),
                pitch=Ls * math.cos(math.radians(th)),
                loops=[{k: mp(v) for k, v in d.items()} for d in loops])
    return poly, info


def write_poly(poly, path):
    rings = [list(poly.exterior.coords)] + [list(r.coords) for r in poly.interiors]
    with open(path, "w") as f:
        f.write(f"loops {len(rings)}\n")
        for r in rings:
            if len(r) > 1 and abs(r[0][0] - r[-1][0]) < 1e-9 and abs(r[0][1] - r[-1][1]) < 1e-9:
                r = r[:-1]
            f.write(f"loop {len(r)}\n")
            for xx, yy in r:
                f.write(f"{xx:.6f} {yy:.6f} -1\n")


def draw(poly, info, p, path):
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    from matplotlib.patches import PathPatch, Arc
    from matplotlib.path import Path

    def patch(poly, **kw):
        v, c = [], []
        for ring in [poly.exterior] + list(poly.interiors):
            pts = list(ring.coords)
            v += pts; c += [Path.MOVETO] + [Path.LINETO] * (len(pts) - 2) + [Path.CLOSEPOLY]
        return PathPatch(Path(v, c), **kw)

    L = info["L"]; w = p.w; th = p.theta
    fig = plt.figure(figsize=(14, 10.5))
    gs = fig.add_gridspec(2, 1, height_ratios=[1, 1.5], hspace=0.2)
    # ---- full device
    ax = fig.add_subplot(gs[0])
    ax.add_patch(patch(poly, facecolor="#dfe8f3", edgecolor="#222", lw=0.8))
    ax.plot([0, 0], [-p.Wc / 2, p.Wc / 2], color="#d62728", lw=4)
    ax.plot([L, L], [-p.Wd / 2, p.Wd / 2], color="#2ca02c", lw=4)
    ax.annotate("", (L * 0.62, -p.Wc / 2 - 350), (L * 0.38, -p.Wc / 2 - 350),
                arrowprops=dict(arrowstyle="-|>", color="#1f77b4", lw=2.5))
    ax.text(L * 0.5, -p.Wc / 2 - 520, "forward = unimpeded (contact 0 -> 1)", ha="center", color="#1f77b4", fontsize=11)
    ax.annotate("", (L * 0.38, p.Wc / 2 + 350), (L * 0.62, p.Wc / 2 + 350),
                arrowprops=dict(arrowstyle="-|>", color="#d62728", lw=2.5))
    ax.text(L * 0.5, p.Wc / 2 + 480, "reverse = blocking (contact 1 -> 0)", ha="center", color="#d62728", fontsize=11)
    ax.text(-60, 0, f"source\n{p.Wc:g} nm", ha="right", va="center", color="#d62728", fontsize=9)
    ax.text(L + 60, 0, f"drain\n{p.Wd:g} nm", ha="left", va="center", color="#2ca02c", fontsize=9)
    ax.plot([L - 1500, L - 500], [-p.Wc / 2 - 900, -p.Wc / 2 - 900], color="k", lw=3)
    ax.text(L - 1000, -p.Wc / 2 - 1150, "1 um", ha="center", fontsize=10)
    ax.set_aspect("equal"); ax.set_xlim(-900, L + 900); ax.set_ylim(-p.Wc / 2 - 1300, p.Wc / 2 + 800)
    ax.axis("off")
    ax.set_title(f"Classical Tesla valve, {p.n} loops: w = {w:g} nm, theta = {th:g} deg, R = {p.R:g} nm, "
                 f"a = {p.a:g} nm, Ls = {p.Ls:g} nm, L_total = {L:.0f} nm", fontsize=12)
    # ---- zoom on the stage nearest the drain (first met by the blocking flow)
    ax = fig.add_subplot(gs[1])
    ax.add_patch(patch(poly, facecolor="#dfe8f3", edgecolor="#222", lw=1.2))
    lp = max(info["loops"], key=lambda d: d["V"][0])
    V, A, C, B, M = lp["V"], lp["A"], lp["C"], lp["B"], lp["M"]
    leg = 180 - th                       # loop leg direction from V (mirrored frame, top loop)
    out = 180 + th                       # outgoing main segment direction from V
    nxt = V + p.Ls * dir_(out)
    arc_apex = C + (p.R + w) * dir_(leg)
    pts = np.array([V, A, arc_apex, B, M, nxt, V + 0.9 * p.Ls * dir_(-th), C + (p.R + 1.5 * w) * dir_(leg + 90),
                    C + (p.R + 1.5 * w) * dir_(leg - 90), nxt + 3.5 * w * dir_(out + 90)])
    pad = 0.6 * w
    ax.set_xlim(pts[:, 0].min() - pad, pts[:, 0].max() + pad)
    ax.set_ylim(pts[:, 1].min() - pad, pts[:, 1].max() + pad)
    for P0, P1 in ((V, A), (B, M), (V, nxt)):
        ax.plot([P0[0], P1[0]], [P0[1], P1[1]], "k--", lw=0.8)
    ax.plot([C[0]], [C[1]], "k+", ms=8)
    rd = dir_(leg)
    ax.annotate("", C + p.R * rd, C, arrowprops=dict(arrowstyle="<->", lw=1))
    ax.text(*(C + 0.5 * p.R * rd + 0.45 * w * dir_(leg - 90)), f"R = {p.R:g}", fontsize=9, ha="center", va="center")
    off = 0.9 * w * dir_(leg - 90)
    ax.annotate("", A + off, V + off, arrowprops=dict(arrowstyle="<->", lw=1, color="#555"))
    ax.text(*(0.5 * (V + A) + 1.3 * w * dir_(leg - 90)), f"a = {p.a:g}", fontsize=9, ha="center", color="#555",
            rotation=leg - 180)
    off2 = 1.0 * w * dir_(out + 90)
    ax.annotate("", M + off2, V + off2, arrowprops=dict(arrowstyle="<->", lw=1, color="#555"))
    ax.text(*(0.5 * (V + M) + 1.45 * w * dir_(out + 90)), f"s = 2R/sin(2 theta) = {info['s_merge']:.0f}", fontsize=9,
            ha="center", color="#555", rotation=out - 180)
    off3 = 2.3 * w * dir_(out + 90)
    ax.annotate("", nxt + off3, V + off3, arrowprops=dict(arrowstyle="<->", lw=1, color="#555"))
    ax.text(*(0.5 * (V + nxt) + 2.75 * w * dir_(out + 90)), f"Ls = {p.Ls:g}", fontsize=9, ha="center",
            color="#555", rotation=out - 180)
    m = V + 0.55 * p.Ls * dir_(-th); nrm = dir_(-th - 90)
    ax.annotate("", m + 0.5 * w * nrm, m - 0.5 * w * nrm, arrowprops=dict(arrowstyle="<->", lw=1))
    ax.text(*(m + 0.9 * w * nrm), f"w = {w:g}", fontsize=9, ha="center")
    ax.add_patch(Arc(V, 1.6 * w, 1.6 * w, angle=0, theta1=leg, theta2=out, color="k", lw=1))
    ax.text(*(V + 2.2 * w * dir_(180)), f"2 theta = {2 * th:g} deg", fontsize=9, ha="center", va="center")
    ax.annotate("", V + 0.1 * p.Ls * dir_(-th), V + 0.8 * p.Ls * dir_(-th),
                arrowprops=dict(arrowstyle="-|>", lw=2.2, color="k"))
    ax.annotate("", A, V + 0.15 * p.a * dir_(leg), arrowprops=dict(arrowstyle="-|>", lw=2.2, color="#d62728"))
    ax.annotate("", M, B, arrowprops=dict(arrowstyle="-|>", lw=2.2, color="#d62728"))
    ax.annotate("", V + 0.85 * info["s_merge"] * dir_(out), V + 0.15 * w * dir_(out),
                arrowprops=dict(arrowstyle="-|>", lw=2.2, color="#777"))
    ax.set_aspect("equal"); ax.axis("off")
    ax.set_title(f"One stage, dashed = centrelines. Island: tip {info['island_tip_offset']:.0f} nm from V, "
                 f"apex {2 * th:g} deg, end radius {p.R - w / 2:g} nm; return leg {info['t_ret']:.0f} nm", fontsize=11)
    fig.text(0.5, 0.03,
             f"Blocking flow (black) arrives along a segment: straight on (0 deg) into the loop (red) or {2 * th:g} deg "
             f"round the bend (grey).\nThe loop returns anti-parallel and rejoins the main channel at s, meeting the "
             f"bend stream at {180 - 2 * th:g} deg. Unimpeded flow runs the other way: {2 * th:g} deg to stay in the "
             f"main channel, {180 - 2 * th:g} deg to enter either loop mouth.", ha="center", fontsize=10)
    fig.subplots_adjust(bottom=0.09)
    fig.savefig(path, dpi=130, bbox_inches="tight")


def main():
    ap = argparse.ArgumentParser()
    for k, v in dict(w=175.0, theta=25.0, R=175.0, a=600.0, Ls=1000.0, Wc=2000.0, Lc=1000.0, Wd=2000.0,
                     Ld=1000.0, lead=600.0).items():
        ap.add_argument("--" + k, type=float, default=v)
    ap.add_argument("--n", type=int, default=4)
    ap.add_argument("--out", default="meshes/tesla_classic.poly")
    ap.add_argument("--png", default="")
    p = ap.parse_args()
    poly, info = build(p)
    write_poly(poly, p.out)
    print(f"forward centreline path through the valve: {(p.n + 1) * p.Ls + 2 * p.lead:.0f} nm; "
          f"loop path per stage (vertex to merge via the loop): {p.a + math.pi * p.R + info['t_ret']:.0f} nm "
          f"vs {info['s_merge']:.0f} nm along the main channel")
    print(f"wrote {p.out}: L_total = {info['L']:.1f} nm, {len(poly.interiors)} islands, "
          f"vertex pitch {info['pitch']:.1f} nm, merge at {info['s_merge']:.1f} nm, return leg {info['t_ret']:.1f} nm, "
          f"island tip {info['island_tip_offset']:.1f} nm from the vertex, exterior {len(poly.exterior.coords)} pts")
    if p.png:
        draw(poly, info, p, p.png)
        print("wrote", p.png)


if __name__ == "__main__":
    main()
