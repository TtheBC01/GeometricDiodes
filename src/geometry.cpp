#include "geometry.hpp"
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdexcept>

namespace gd {

double Polygon::signed_area() const {
  double a = 0;
  for (size_t i = 0, n = pts.size(); i < n; ++i) a += cross(pts[i], pts[(i + 1) % n]);
  return 0.5 * a;
}
void Polygon::make_ccw() { if (signed_area() < 0) std::reverse(pts.begin(), pts.end()); }
void Polygon::make_cw() { if (signed_area() > 0) std::reverse(pts.begin(), pts.end()); }

bool ray_segment(Vec2 p, Vec2 d, Vec2 a, Vec2 b, double& t, double& u) {
  Vec2 e = b - a;
  double den = cross(d, e);
  if (std::fabs(den) < 1e-300) return false;  // parallel
  Vec2 ap = a - p;
  t = cross(ap, e) / den;
  u = cross(ap, d) / den;
  return u >= 0.0 && u <= 1.0;
}

Boundary::Boundary(std::vector<Polygon> loops, const ContactTagger& tagger, double grid_cell)
    : loops_(std::move(loops)) {
  if (loops_.empty()) throw std::runtime_error("Boundary: no loops");
  loops_[0].make_ccw();
  for (size_t i = 1; i < loops_.size(); ++i) loops_[i].make_cw();
  bmin_ = {1e300, 1e300};
  bmax_ = {-1e300, -1e300};
  for (const auto& L : loops_) {
    for (size_t i = 0, n = L.pts.size(); i < n; ++i) {
      Segment s;
      s.a = L.pts[i];
      s.b = L.pts[(i + 1) % n];
      Vec2 e = s.b - s.a;
      s.len = e.norm();
      if (s.len < 1e-9) continue;  // drop degenerate edges
      s.d = e / s.len;
      s.n = s.d.left();  // outer CCW / holes CW => domain is on the left
      int c = tagger ? tagger(s.a, s.b) : -1;
      if (c >= 0) { s.kind = SegKind::Contact; s.contact = c; }
      segs_.push_back(s);
      bmin_.x = std::min({bmin_.x, s.a.x, s.b.x}); bmin_.y = std::min({bmin_.y, s.a.y, s.b.y});
      bmax_.x = std::max({bmax_.x, s.a.x, s.b.x}); bmax_.y = std::max({bmax_.y, s.a.y, s.b.y});
    }
  }
  int nc = 0;
  for (const auto& s : segs_) nc = std::max(nc, s.contact + 1);
  contact_width_.assign(nc, 0.0);
  contact_segs_.assign(nc, {});
  for (size_t i = 0; i < segs_.size(); ++i)
    if (segs_[i].contact >= 0) {
      contact_width_[segs_[i].contact] += segs_[i].len;
      contact_segs_[segs_[i].contact].push_back((int)i);
    }
  cell_ = grid_cell;
  build_grid();
}

void Boundary::build_grid() {
  Vec2 ext = bmax_ - bmin_;
  if (cell_ <= 0) cell_ = std::max(ext.x, ext.y) / 64.0;
  nx_ = std::max(1, (int)std::ceil(ext.x / cell_) + 1);
  ny_ = std::max(1, (int)std::ceil(ext.y / cell_) + 1);
  cells_.assign((size_t)nx_ * ny_, {});
  for (size_t i = 0; i < segs_.size(); ++i) {
    // conservative: all cells overlapping the segment's bbox, then filter by
    // distance of the cell box to the segment.
    const Segment& s = segs_[i];
    int ix0 = (int)((std::min(s.a.x, s.b.x) - bmin_.x) / cell_);
    int ix1 = (int)((std::max(s.a.x, s.b.x) - bmin_.x) / cell_);
    int iy0 = (int)((std::min(s.a.y, s.b.y) - bmin_.y) / cell_);
    int iy1 = (int)((std::max(s.a.y, s.b.y) - bmin_.y) / cell_);
    ix0 = std::clamp(ix0, 0, nx_ - 1); ix1 = std::clamp(ix1, 0, nx_ - 1);
    iy0 = std::clamp(iy0, 0, ny_ - 1); iy1 = std::clamp(iy1, 0, ny_ - 1);
    for (int iy = iy0; iy <= iy1; ++iy)
      for (int ix = ix0; ix <= ix1; ++ix) {
        // cell box vs segment: separating-axis check along the segment normal
        Vec2 c0 = bmin_ + Vec2{ix * cell_, iy * cell_};
        Vec2 c1 = c0 + Vec2{cell_, cell_};
        double dmin = 1e300, dmax = -1e300;
        for (Vec2 q : {Vec2{c0.x, c0.y}, Vec2{c1.x, c0.y}, Vec2{c0.x, c1.y}, Vec2{c1.x, c1.y}}) {
          double v = dot(q - s.a, s.n);
          dmin = std::min(dmin, v); dmax = std::max(dmax, v);
        }
        if (dmin > 1e-9 || dmax < -1e-9) continue;
        cells_[(size_t)iy * nx_ + ix].push_back((int)i);
      }
  }
}

bool Boundary::nearest_hit(Vec2 p, Vec2 d, double maxlen, int skip_seg, Hit& h) const {
  // 2D DDA through the grid along the ray; test the segments of each cell and
  // stop as soon as the entry distance of the next cell exceeds the best hit.
  const double eps = 1e-9;
  double best = maxlen;
  int best_seg = -1;
  auto test_cell = [&](int ix, int iy) {
    if (ix < 0 || iy < 0 || ix >= nx_ || iy >= ny_) return;
    for (int si : cells_[(size_t)iy * nx_ + ix]) {
      if (si == skip_seg) continue;
      const Segment& s = segs_[si];
      double t, u;
      if (!ray_segment(p, d, s.a, s.b, t, u)) continue;
      if (t <= eps || t > best) continue;
      // only accept hits that approach the wall from the domain side
      if (dot(d, s.n) >= 0) continue;
      best = t; best_seg = si;
    }
  };
  // DDA setup
  Vec2 rel = p - bmin_;
  int ix = (int)std::floor(rel.x / cell_), iy = (int)std::floor(rel.y / cell_);
  int stepx = d.x > 0 ? 1 : (d.x < 0 ? -1 : 0);
  int stepy = d.y > 0 ? 1 : (d.y < 0 ? -1 : 0);
  double tdx = d.x != 0 ? std::fabs(cell_ / d.x) : 1e300;
  double tdy = d.y != 0 ? std::fabs(cell_ / d.y) : 1e300;
  double nextx = d.x > 0 ? ((ix + 1) * cell_ - rel.x) / d.x : (d.x < 0 ? (ix * cell_ - rel.x) / d.x : 1e300);
  double nexty = d.y > 0 ? ((iy + 1) * cell_ - rel.y) / d.y : (d.y < 0 ? (iy * cell_ - rel.y) / d.y : 1e300);
  double tcur = 0;
  int guard = 0;
  while (tcur <= best && guard++ < 4 * (nx_ + ny_) + 8) {
    test_cell(ix, iy);
    if (nextx < nexty) { tcur = nextx; nextx += tdx; ix += stepx; }
    else { tcur = nexty; nexty += tdy; iy += stepy; }
    if (ix < 0 || iy < 0 || ix >= nx_ || iy >= ny_) break;
  }
  if (best_seg < 0) return false;
  h.t = best; h.seg = best_seg; h.p = p + d * best;
  return true;
}

bool Boundary::contains(Vec2 p) const {
  bool inside = false;
  for (const auto& L : loops_) {
    const auto& v = L.pts;
    for (size_t i = 0, n = v.size(), j = n - 1; i < n; j = i++) {
      if (((v[i].y > p.y) != (v[j].y > p.y)) &&
          (p.x < (v[j].x - v[i].x) * (p.y - v[i].y) / (v[j].y - v[i].y) + v[i].x))
        inside = !inside;
    }
  }
  return inside;
}

double Boundary::distance_to_boundary(Vec2 p) const {
  double best = 1e300;
  for (const auto& s : segs_) {
    double u = std::clamp(dot(p - s.a, s.d), 0.0, s.len);
    best = std::min(best, (p - (s.a + s.d * u)).norm());
  }
  return best;
}

static bool segments_cross(Vec2 a, Vec2 b, Vec2 c, Vec2 d) {
  double d1 = cross(b - a, c - a), d2 = cross(b - a, d - a);
  double d3 = cross(d - c, a - c), d4 = cross(d - c, b - c);
  return ((d1 > 0) != (d2 > 0)) && ((d3 > 0) != (d4 > 0)) &&
         std::fabs(d1) > 1e-12 && std::fabs(d2) > 1e-12 && std::fabs(d3) > 1e-12 && std::fabs(d4) > 1e-12;
}

std::string Boundary::validate(double min_feature) const {
  std::ostringstream err;
  if (loops_[0].signed_area() <= 0) err << "outer loop has non-positive area; ";
  for (size_t i = 1; i < loops_.size(); ++i)
    if (loops_[i].signed_area() >= 0) err << "hole " << i << " has non-negative area; ";
  // self intersections (O(N^2); geometries here have < 2000 segments)
  int crossings = 0;
  for (size_t i = 0; i < segs_.size(); ++i)
    for (size_t j = i + 1; j < segs_.size(); ++j)
      if (segments_cross(segs_[i].a, segs_[i].b, segs_[j].a, segs_[j].b)) ++crossings;
  if (crossings) err << crossings << " segment crossings; ";
  if (contact_width_.size() < 2) err << "fewer than two contacts; ";
  for (size_t c = 0; c < contact_width_.size(); ++c)
    if (contact_width_[c] <= 0) err << "contact " << c << " has zero width; ";
  // holes must lie inside the outer loop
  for (size_t i = 1; i < loops_.size(); ++i) {
    Polygon outer = loops_[0];
    bool ok = true;
    for (Vec2 q : loops_[i].pts) {
      Boundary tmp({outer}, nullptr, 1e300);
      if (!tmp.contains(q)) { ok = false; break; }
    }
    if (!ok) err << "hole " << i << " not inside outer loop; ";
  }
  // minimum feature: sample interior points and look for channels narrower
  // than min_feature (distance to boundary < min_feature/2 on both sides is
  // approximated by the medial-radius test).
  if (min_feature > 0) {
    Rng rng(7);
    int narrow = 0, tested = 0;
    for (int k = 0; k < 4000; ++k) {
      Vec2 p = random_interior_point(rng);
      ++tested;
      // walk to the medial axis: the local channel width is roughly twice
      // the largest inscribed radius near p; probe by moving away from the
      // nearest wall until the distance stops increasing.
      double r = distance_to_boundary(p);
      double stepf = 0.5;
      for (int it = 0; it < 60 && stepf > 0.005; ++it) {
        // numerical gradient of the distance field; climb to its local maximum
        const double hstep = 1e-3 * std::max(1.0, r);
        double gx = (distance_to_boundary(p + Vec2{hstep, 0}) - distance_to_boundary(p - Vec2{hstep, 0})) / (2 * hstep);
        double gy = (distance_to_boundary(p + Vec2{0, hstep}) - distance_to_boundary(p - Vec2{0, hstep})) / (2 * hstep);
        Vec2 g{gx, gy};
        if (g.norm() < 1e-3) break;
        Vec2 q = p + g.unit() * (stepf * r);
        double rq = contains(q) ? distance_to_boundary(q) : -1;
        if (rq > r) { p = q; r = rq; } else { stepf *= 0.5; }
      }
      if (2 * r < min_feature * 0.95) ++narrow;
    }
    if (narrow > tested / 50) err << "approximately " << (100.0 * narrow / tested)
                                 << "% of sampled interior points lie in channels narrower than "
                                 << min_feature << " nm; ";
  }
  return err.str();
}

Vec2 Boundary::random_interior_point(Rng& rng) const {
  for (int k = 0; k < 100000; ++k) {
    Vec2 p{rng.uniform(bmin_.x, bmax_.x), rng.uniform(bmin_.y, bmax_.y)};
    if (contains(p)) return p;
  }
  throw std::runtime_error("random_interior_point: domain appears empty");
}

void Boundary::write_svg(const std::string& path,
                         const std::vector<std::vector<Vec2>>& trajectories,
                         const std::vector<std::string>& traj_colors) const {
  std::ofstream f(path);
  Vec2 ext = bmax_ - bmin_;
  double pad = 0.03 * std::max(ext.x, ext.y);
  double W = 1400.0, S = W / (ext.x + 2 * pad), H = (ext.y + 2 * pad) * S;
  auto X = [&](double x) { return (x - bmin_.x + pad) * S; };
  auto Y = [&](double y) { return H - (y - bmin_.y + pad) * S; };
  f << "<svg xmlns='http://www.w3.org/2000/svg' width='" << W << "' height='" << H
    << "' viewBox='0 0 " << W << " " << H << "'>\n<rect width='100%' height='100%' fill='white'/>\n";
  // domain fill via even-odd path
  f << "<path fill-rule='evenodd' fill='#dfe8f3' stroke='none' d='";
  for (const auto& L : loops_) {
    for (size_t i = 0; i < L.pts.size(); ++i)
      f << (i ? " L" : "M") << X(L.pts[i].x) << " " << Y(L.pts[i].y);
    f << " Z ";
  }
  f << "'/>\n";
  for (const auto& s : segs_) {
    const char* col = s.kind == SegKind::Contact ? (s.contact == 0 ? "#d62728" : "#2ca02c") : "#222";
    f << "<line x1='" << X(s.a.x) << "' y1='" << Y(s.a.y) << "' x2='" << X(s.b.x) << "' y2='" << Y(s.b.y)
      << "' stroke='" << col << "' stroke-width='" << (s.kind == SegKind::Contact ? 4 : 1.5) << "'/>\n";
  }
  for (size_t k = 0; k < trajectories.size(); ++k) {
    const auto& tr = trajectories[k];
    if (tr.size() < 2) continue;
    std::string col = k < traj_colors.size() ? traj_colors[k] : "#1f77b4";
    f << "<polyline fill='none' stroke='" << col << "' stroke-width='1' stroke-opacity='0.8' points='";
    for (Vec2 p : tr) f << X(p.x) << "," << Y(p.y) << " ";
    f << "'/>\n";
  }
  f << "<text x='10' y='" << H - 8 << "' font-family='monospace' font-size='14'>bbox "
    << ext.x << " x " << ext.y << " nm; red = source contact, green = drain contact</text>\n</svg>\n";
}

}  // namespace gd
