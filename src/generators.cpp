#include "generators.hpp"
#include <cmath>
#include <stdexcept>
#include <string>

namespace gd {

ContactTagger end_plane_tagger(double L, double tol) {
  return [L, tol](Vec2 a, Vec2 b) -> int {
    if (std::fabs(a.x) < tol && std::fabs(b.x) < tol) return 0;
    if (std::fabs(a.x - L) < tol && std::fabs(b.x - L) < tol) return 1;
    return -1;
  };
}

Boundary make_straight_channel(double L, double W) {
  Polygon P;
  P.pts = {{0, -W / 2}, {L, -W / 2}, {L, W / 2}, {0, W / 2}};
  return Boundary({P}, end_plane_tagger(L));
}

Boundary make_stepped_channel(double Wc, double Lc, double w, double Lm, double Wd, double Ld) {
  double L = Lc + Lm + Ld;
  Polygon P;
  P.pts = {{0, -Wc / 2}, {Lc, -Wc / 2}, {Lc, -w / 2}, {Lc + Lm, -w / 2}, {Lc + Lm, -Wd / 2},
           {L, -Wd / 2}, {L, Wd / 2}, {Lc + Lm, Wd / 2}, {Lc + Lm, w / 2}, {Lc, w / 2},
           {Lc, Wc / 2}, {0, Wc / 2}};
  return Boundary({P}, end_plane_tagger(L));
}

Boundary make_funnel(const FunnelParams& p) {
  if (p.Wn >= p.Wc || p.Wn > p.Wd) throw std::runtime_error("funnel: neck must be narrower than leads");
  double Lt = 0.5 * (p.Wc - p.Wn) / std::tan(deg2rad(p.alpha_deg));
  double Lo = p.beta_deg >= 89.999 ? 0.0 : 0.5 * (p.Wd - p.Wn) / std::tan(deg2rad(p.beta_deg));
  double x1 = p.Lc, x2 = x1 + Lt, x3 = x2 + p.Ln, x4 = x3 + Lo, L = x4 + p.Ld;
  std::vector<Vec2> lower = {{0, -p.Wc / 2}, {x1, -p.Wc / 2}, {x2, -p.Wn / 2}};
  if (p.Ln > 0) lower.push_back({x3, -p.Wn / 2});
  lower.push_back({x4, -p.Wd / 2});
  lower.push_back({L, -p.Wd / 2});
  Polygon P;
  P.pts = lower;
  for (auto it = lower.rbegin(); it != lower.rend(); ++it) P.pts.push_back({it->x, -it->y});
  return Boundary({P}, end_plane_tagger(L));
}

Boundary make_symmetric_constriction(double Wc, double Lc, double alpha_deg, double Wn, double Ln) {
  FunnelParams p;
  p.Wc = Wc; p.Lc = Lc; p.alpha_deg = alpha_deg; p.Wn = Wn; p.Ln = Ln;
  p.beta_deg = alpha_deg; p.Wd = Wc; p.Ld = Lc;
  return make_funnel(p);
}

namespace {
// Offset walls of one Tesla stage in path order (entry junction -> exit
// junction). Coordinates relative to the stage anchor x_s on the top wall
// y = w/2 of the main channel; y measured from the channel centerline.
struct StageWalls {
  std::vector<Vec2> outer;  // right-hand wall of the path (far from island)
  std::vector<Vec2> inner;  // left-hand wall of the path (island side)
  double x_entry_outer = 0, x_entry_inner = 0, x_exit_outer = 0, x_exit_inner = 0;
  double xmin = 0, xmax = 0;
};

StageWalls build_stage(const TeslaParams& p) {
  const double w = p.w, hw = 0.5 * w;
  const double be = deg2rad(p.beta_entry_deg), bx = deg2rad(p.beta_exit_deg);
  if (p.r <= hw) throw std::runtime_error("tesla: r must exceed w/2");
  Vec2 d0{-std::cos(be), std::sin(be)};   // entry heading (up-left)
  Vec2 d1{std::cos(bx), -std::sin(bx)};   // exit heading (down-right)
  // The path turns counter-clockwise from heading d0 to heading d1.
  double th0 = std::atan2(d0.y, d0.x), th1 = std::atan2(d1.y, d1.x);
  while (th1 <= th0) th1 += 2 * kPi;
  double turn = th1 - th0;  // ccw turn angle
  Vec2 S0{0, hw};                       // centerline start on the top wall
  Vec2 A = S0 + d0 * p.a;               // arc start
  Vec2 C = A + d0.left() * p.r;         // center on the left of the heading
  // arc end
  double phi0 = std::atan2(A.y - C.y, A.x - C.x);
  Vec2 B = C + from_angle(phi0 + turn) * p.r;
  double height = B.y - hw;
  if (height <= 0.5 * w)
    throw std::runtime_error("tesla: branch does not clear the channel (arc end height " + std::to_string(height) +
                             " nm above the wall, need > " + std::to_string(0.5 * w) + "); increase a");
  double b = height / std::sin(bx);
  Vec2 E = B + d1 * b;  // centerline end on the top wall (E.y == hw)
  StageWalls sw;
  // Entry straight: walls offset +-hw. Right (outer) wall starts on the wall
  // line at x = +hw/sin(be), left (inner) at x = -hw/sin(be).
  sw.x_entry_outer = hw / std::sin(be);
  sw.x_entry_inner = -hw / std::sin(be);
  sw.outer.push_back({sw.x_entry_outer, hw});
  sw.inner.push_back({sw.x_entry_inner, hw});
  Vec2 nl = d0.left(), nr = d0.right();
  sw.outer.push_back(A + nr * hw);
  sw.inner.push_back(A + nl * hw);
  // Arc: outer radius r + hw (right of a ccw turn is the outside), inner r - hw.
  int nseg = std::max(4, (int)std::ceil(turn / deg2rad(p.arc_step_deg)));
  for (int i = 1; i <= nseg; ++i) {
    double phi = phi0 + turn * i / nseg;
    sw.outer.push_back(C + from_angle(phi) * (p.r + hw));
    sw.inner.push_back(C + from_angle(phi) * (p.r - hw));
  }
  // Exit straight down to the wall line.
  Vec2 nl1 = d1.left(), nr1 = d1.right();
  Vec2 bo = B + nr1 * hw, bi = B + nl1 * hw;
  sw.outer.push_back(bo); sw.inner.push_back(bi);
  // walls reach y = hw after moving along d1
  double so = (bo.y - hw) / std::sin(bx), si = (bi.y - hw) / std::sin(bx);
  Vec2 eo = bo + d1 * so, ei = bi + d1 * si;
  eo.y = hw; ei.y = hw;
  sw.outer.push_back(eo); sw.inner.push_back(ei);
  sw.x_exit_outer = eo.x; sw.x_exit_inner = ei.x;
  if (!(sw.x_exit_inner < sw.x_entry_inner))
    throw std::runtime_error("tesla: exit junction is not upstream of the entry junction; increase a or r");
  if (sw.x_exit_outer >= sw.x_exit_inner || sw.x_entry_inner >= sw.x_entry_outer)
    throw std::runtime_error("tesla: junction ordering inconsistent");
  sw.xmin = 1e300; sw.xmax = -1e300;
  for (auto& q : sw.outer) { sw.xmin = std::min(sw.xmin, q.x); sw.xmax = std::max(sw.xmax, q.x); }
  (void)E;
  return sw;
}
}  // namespace

Boundary make_tesla(const TeslaParams& p, TeslaInfo* info) {
  StageWalls sw = build_stage(p);
  const double hw = 0.5 * p.w;
  double footprint = sw.xmax - sw.xmin;          // x-extent of one stage's outer wall
  double pitch = footprint + p.gap;
  // anchors: stage k's outer wall must start after lead_in; stage anchor is
  // at x_s = x0 + (-sw.xmin) so the leftmost outer point sits at x0.
  double Lm = p.lead_in + p.n_stages * footprint + (p.n_stages - 1) * p.gap + p.lead_out;
  double x_ch0 = p.Lc, x_ch1 = p.Lc + Lm, L = x_ch1 + p.Ld;
  std::vector<double> anchors;
  for (int k = 0; k < p.n_stages; ++k) anchors.push_back(x_ch0 + p.lead_in + k * pitch - sw.xmin);
  // outer loop (CCW): bottom wall left->right, drain, top wall right->left.
  Polygon outer;
  outer.pts = {{0, -p.Wc / 2}, {x_ch0, -p.Wc / 2}, {x_ch0, -hw}, {x_ch1, -hw}, {x_ch1, -p.Wd / 2},
               {L, -p.Wd / 2}, {L, p.Wd / 2}, {x_ch1, p.Wd / 2}, {x_ch1, hw}};
  for (int k = p.n_stages - 1; k >= 0; --k) {
    double xs = anchors[k];
    for (Vec2 q : sw.outer) outer.pts.push_back({xs + q.x, q.y});
  }
  outer.pts.push_back({x_ch0, hw});
  outer.pts.push_back({x_ch0, p.Wc / 2});
  outer.pts.push_back({0, p.Wc / 2});
  std::vector<Polygon> loops{outer};
  double island_area = 0;
  for (int k = 0; k < p.n_stages; ++k) {
    Polygon isl;
    double xs = anchors[k];
    for (Vec2 q : sw.inner) isl.pts.push_back({xs + q.x, q.y});
    // close along the top wall from exit junction back to entry junction
    isl.make_cw();
    island_area += std::fabs(isl.signed_area());
    loops.push_back(isl);
  }
  if (info) {
    info->main_length = Lm; info->total_length = L;
    info->stage_footprint = footprint; info->island_area = island_area;
  }
  return Boundary(loops, end_plane_tagger(L));
}

}  // namespace gd
