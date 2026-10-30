// Parametric device generators. All devices have the source contact on the
// plane x = 0 (contact 0) and the drain contact on the plane x = L (contact 1).
// Lengths in nm, angles in degrees.
#pragma once
#include "geometry.hpp"

namespace gd {

// Straight channel of width W and length L (contacts span the full width).
Boundary make_straight_channel(double L, double W);

// Wide-lead channel: source lead (Wc x Lc) -> abrupt step to channel (w x Lm)
// -> abrupt step to drain lead (Wd x Ld). Control geometry for the Tesla device.
Boundary make_stepped_channel(double Wc, double Lc, double w, double Lm, double Wd, double Ld);

struct FunnelParams {
  double Wc = 2000;      // source-side channel width
  double Lc = 2000;      // source-side straight length before the taper
  double alpha_deg = 30; // taper angle between wall and axis (Nguyen 2021: 30 deg)
  double Wn = 175;       // neck width
  double Ln = 0;         // neck straight length
  double beta_deg = 90;  // drain-side opening angle (90 = abrupt shoulder)
  double Wd = 2000;      // drain-side channel width
  double Ld = 2420;      // drain-side straight length after the opening
};
Boundary make_funnel(const FunnelParams& p);

// Mirror-symmetric constriction: taper in and out with the same angle.
Boundary make_symmetric_constriction(double Wc, double Lc, double alpha_deg, double Wn, double Ln);

// Tesla-inspired multi-stage channel built from offset centerlines.
// Reverse flow (toward -x) at a stage peels off into a branch leaving the top
// wall at angle beta_entry (measured from -x), travels a straight length a,
// turns counter-clockwise on a centerline radius r and re-enters the main
// channel at angle beta_exit (measured from +x), pointing against the reverse
// flow. The island between branch and channel is a hole in the domain.
struct TeslaParams {
  double w = 175;            // channel (and branch) width == minimum feature
  int n_stages = 3;
  double a = 1100;           // straight branch length along the centerline
  double r = 210;            // centerline turn radius (must exceed w/2)
  double beta_entry_deg = 30;
  double beta_exit_deg = 45;
  double gap = 150;          // straight channel between stage footprints
  double Wc = 2000, Lc = 1000;  // source lead
  double Wd = 2000, Ld = 1000;  // drain lead
  double lead_in = 300;      // straight channel before the first stage
  double lead_out = 300;     // straight channel after the last stage
  double arc_step_deg = 2;   // arc discretisation
};
struct TeslaInfo {
  double main_length = 0;    // length of the narrow channel incl. stages
  double total_length = 0;
  double stage_footprint = 0;
  double island_area = 0;
};
Boundary make_tesla(const TeslaParams& p, TeslaInfo* info = nullptr);

// Load a boundary written in the .poly interchange format used by
// hydro/mesh.cpp ("loops N", then per loop "loop n" and n lines "x y tag").
// Contacts are re-tagged from the end planes x == 0 and x == max x, so the
// file's own tags are ignored. Used for geometries generated in Python
// (tools/tesla_classic.py).
Boundary load_boundary(const std::string& path);

// Tagger shared by all generators: contact 0 at x==0, contact 1 at x==L.
ContactTagger end_plane_tagger(double L, double tol = 1e-6);

}  // namespace gd
