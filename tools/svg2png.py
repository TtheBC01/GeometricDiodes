"""Rasterize the simple SVGs written by Boundary::write_svg (paths, lines, polylines) to PNG.
Usage: python svg2png.py in.svg out.png [scale]"""
import re, sys
from PIL import Image, ImageDraw

src, dst = sys.argv[1], sys.argv[2]
scale = float(sys.argv[3]) if len(sys.argv) > 3 else 1.0
svg = open(src).read()
W = float(re.search(r"width='([\d.]+)'", svg).group(1)); H = float(re.search(r"height='([\d.]+)'", svg).group(1))
img = Image.new("RGB", (int(W*scale), int(H*scale)), "white"); dr = ImageDraw.Draw(img)
S = lambda v: float(v)*scale
for m in re.finditer(r"<path[^>]*d='([^']*)'", svg):
    for loop in re.findall(r"M([^Z]*)Z", m.group(1)):
        nums = [float(x) for x in re.findall(r"[-\d.e+]+", loop.replace("L", " "))]
        pts = [(S(nums[i]), S(nums[i+1])) for i in range(0, len(nums)-1, 2)]
        if len(pts) > 2: dr.polygon(pts, outline="#333")
for m in re.finditer(r"<line x1='([\d.e+-]+)' y1='([\d.e+-]+)' x2='([\d.e+-]+)' y2='([\d.e+-]+)' stroke='(#\w+)' stroke-width='([\d.]+)'", svg):
    x1,y1,x2,y2,col,sw = m.groups(); dr.line([(S(x1),S(y1)),(S(x2),S(y2))], fill=col, width=max(1,int(float(sw)*scale)))
for m in re.finditer(r"<polyline[^>]*stroke='(#\w+)'[^>]*points='([^']*)'", svg):
    col, pts = m.groups(); p = [tuple(S(v) for v in q.split(",")) for q in pts.split()]
    if len(p) > 1: dr.line(p, fill=col, width=1)
img.save(dst)
