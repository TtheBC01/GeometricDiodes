// Polygonal boundary representation with contacts, inward normals, a uniform
// grid for ray/segment queries, validation and SVG export.
#pragma once
#include "core.hpp"
#include <functional>
#include <string>
#include <vector>

namespace gd {

enum class SegKind { Wall, Contact };

struct Segment {
  Vec2 a, b;
  Vec2 n;            // unit normal pointing INTO the conducting domain
  Vec2 d;            // unit tangent a->b
  double len = 0;
  SegKind kind = SegKind::Wall;
  int contact = -1;  // contact id when kind == Contact
};

struct Polygon {
  std::vector<Vec2> pts;  // closed implicitly (last connects to first)
  double signed_area() const;
  void make_ccw();
  void make_cw();
};

struct Hit {
  double t = 0;   // distance along the ray
  int seg = -1;   // segment index
  Vec2 p;         // hit point
};

// Called for each boundary edge; returns a contact id >= 0 or -1 for a wall.
using ContactTagger = std::function<int(Vec2, Vec2)>;

class Boundary {
 public:
  // loops[0] is the outer boundary; the rest are islands (holes). Orientation
  // is fixed internally so the inward normal is always the left normal.
  Boundary() = default;
  Boundary(std::vector<Polygon> loops, const ContactTagger& tagger, double grid_cell = 0);

  const std::vector<Segment>& segments() const { return segs_; }
  const std::vector<Polygon>& loops() const { return loops_; }
  int num_contacts() const { return (int)contact_width_.size(); }
  double contact_width(int c) const { return contact_width_[c]; }
  const std::vector<int>& contact_segments(int c) const { return contact_segs_[c]; }
  Vec2 bbox_min() const { return bmin_; }
  Vec2 bbox_max() const { return bmax_; }

  // Nearest intersection of the ray p + t d (0 < t <= maxlen) with any
  // segment other than skip_seg. Returns false when there is none.
  bool nearest_hit(Vec2 p, Vec2 d, double maxlen, int skip_seg, Hit& h) const;

  // Even-odd point-in-domain test over all loops.
  bool contains(Vec2 p) const;

  // Minimum unsigned distance from p to any segment (slow, O(N); used only by
  // validation and diagnostics).
  double distance_to_boundary(Vec2 p) const;

  // Returns an empty string when the geometry passes: no self-intersections,
  // correct orientation, contacts present, min feature above min_feature.
  std::string validate(double min_feature = 0) const;

  // Sample a uniformly distributed interior point (rejection on the bbox).
  Vec2 random_interior_point(Rng& rng) const;

  void write_svg(const std::string& path,
                 const std::vector<std::vector<Vec2>>& trajectories = {},
                 const std::vector<std::string>& traj_colors = {}) const;

 private:
  void build_grid();
  std::vector<Polygon> loops_;
  std::vector<Segment> segs_;
  std::vector<double> contact_width_;
  std::vector<std::vector<int>> contact_segs_;
  Vec2 bmin_, bmax_;
  // uniform grid
  int nx_ = 0, ny_ = 0;
  double cell_ = 0;
  std::vector<std::vector<int>> cells_;
};

// Ray/segment intersection helper. Returns true and t (ray parameter) and u
// (segment parameter in [0,1]) when they intersect.
bool ray_segment(Vec2 p, Vec2 d, Vec2 a, Vec2 b, double& t, double& u);

}  // namespace gd
