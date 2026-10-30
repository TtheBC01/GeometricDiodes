// Geometry dump: builds every kill-test device, validates it, prints a summary
// and writes SVGs (with a few sample trajectories) into results/.
#include "generators.hpp"
#include "transport.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace gd;

static void dump(const std::string& name, const Boundary& b, double min_feature) {
  std::string err = b.validate(min_feature);
  Vec2 ext = b.bbox_max() - b.bbox_min();
  std::printf("%-28s segs=%5zu loops=%zu bbox=%.0f x %.0f nm  W_S=%.0f W_D=%.0f  %s\n", name.c_str(),
              b.segments().size(), b.loops().size(), ext.x, ext.y, b.contact_width(0), b.contact_width(1),
              err.empty() ? "VALID" : ("INVALID: " + err).c_str());
  // a few ballistic trajectories, field on, diffuse-ish walls, for the picture
  TransportParams p;
  p.V = 0.1; p.specularity = 0.5; p.lambda_mfp = 1500;
  Transport tr(b, p);
  std::vector<Trajectory> rec;
  tr.run(40, 11, 1, 1, &rec, 40);
  std::vector<std::vector<Vec2>> pts; std::vector<std::string> cols;
  for (auto& t : rec) {
    pts.push_back(t.pts);
    cols.push_back(t.fate == Fate::Drain ? "#2ca02c" : (t.fate == Fate::Source ? "#d62728" : "#9467bd"));
  }
  b.write_svg("results/geom_" + name + ".svg", pts, cols);
}

int main(int argc, char** argv) {
  // gd_geom <file.poly> [min_feature]: validate and draw one file-defined geometry.
  if (argc > 1) {
    double mf = argc > 2 ? std::atof(argv[2]) : 175;
    std::string name = argv[1];
    size_t p = name.find_last_of('/'); if (p != std::string::npos) name = name.substr(p + 1);
    p = name.find_last_of('.'); if (p != std::string::npos) name = name.substr(0, p);
    dump(name, load_boundary(argv[1]), mf);
    return 0;
  }
  dump("straight_175", make_straight_channel(6000, 175), 175);
  dump("stepped_175", make_stepped_channel(2000, 1500, 175, 3000, 2000, 1500), 175);
  FunnelParams f;  // Nguyen 2021 Device D like
  dump("funnel_nguyenD", make_funnel(f), 175);
  dump("symmetric_175", make_symmetric_constriction(2000, 1500, 30, 175, 0), 175);
  TeslaParams t;
  TeslaInfo info;
  Boundary tesla3 = make_tesla(t, &info);
  std::printf("  tesla3: main_length=%.0f total=%.0f footprint/stage=%.0f island_area=%.0f nm^2\n",
              info.main_length, info.total_length, info.stage_footprint, info.island_area);
  dump("tesla3_175", tesla3, 175);
  t.n_stages = 1;
  dump("tesla1_175", make_tesla(t), 175);
  // Truccolo 2024 nano-scale funnel: Wc=100, Wn=10, Wd=40 nm, alpha_d = 85 deg
  FunnelParams tf; tf.Wc = 100; tf.Lc = 100; tf.alpha_deg = 45; tf.Wn = 10; tf.Ln = 0; tf.beta_deg = 90; tf.Wd = 40; tf.Ld = 100;
  dump("funnel_truccolo", make_funnel(tf), 10);
  return 0;
}
