// Device catalogue shared by the Stage H2/H3 drivers (gd_thz_sweep, gd_thz_power).
#pragma once
#include "generators.hpp"
#include <stdexcept>
#include <string>

namespace gd::hydro {

struct ThzGeo { Boundary b; double W_ref; std::string mesh_name; };

// chan: uniform 1 um x 10 um channel; funnel: 175 nm neck, 2 um leads 6 um long;
// funnel_short: the same with 1 um leads; funnel_w1000: 1 um neck, 4 um leads
// 12 um long; tesla_classic / tesla_classic_w1000: the classical valve (poly files).
inline ThzGeo make_thz_geo(const std::string& name) {
  if (name == "chan") return {make_straight_channel(10000, 1000), 1000, "t_chan10"};
  if (name == "funnel") { FunnelParams f; f.Lc = 6000; f.Ld = 6000; return {make_funnel(f), 2000, "h_funnel"}; }
  if (name == "funnel_short") { FunnelParams f; f.Lc = 1000; f.Ld = 1000; return {make_funnel(f), 2000, "h_funnel_short"}; }
  if (name == "funnel_w1000") {
    FunnelParams f; f.Wc = f.Wd = 4000; f.Wn = 1000; f.Lc = f.Ld = 12000;
    return {make_funnel(f), 4000, "h_funnel_w1000"};
  }
  if (name == "tesla_classic") return {load_boundary("meshes/tesla_classic_h.poly"), 2000, "h_tesla_classic"};
  if (name == "tesla_classic_w1000") return {load_boundary("meshes/tesla_classic_w1000_h.poly"), 4000, "h_tesla_classic_w1000"};
  throw std::runtime_error("unknown geometry " + name);
}

}  // namespace gd::hydro
