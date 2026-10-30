// Shared pieces of the Stage H2 (gated-compressible, time-harmonic) solvers:
// the closure functions of plans/hydro_thz_notes.md, the port description,
// and the node classification / wall frames / dof layout built from a mesh.
//
// Dof layout (nn P2 nodes, nd density dofs = nv for P1 density or nn for P2,
// np ports):
//   velocity of node a: 2a, 2a+1 (Cartesian, or tangential/normal at Wall nodes)
//   density of node a:  2 nn + a   (a < nd)
//   port potential c:   2 nn + nd + c
// Both problems use P1 density (Taylor-Hood) by default: with the density
// gradient integrated by parts the dispersion error is second order in h,
// and the pair stays inf-sup stable in the quasi-static limit, where equal
// order P2/P2 loses 15-20 % (ladder T5). P2 density remains an option.
#pragma once
#include "hydro/mesh.hpp"
#include <cmath>
#include <complex>
#include <limits>
#include <stdexcept>
#include <vector>

namespace gd::hydro {

using cd = std::complex<double>;

enum class Closure { ShallowWater, Graphene };

// Nondimensional closure: h(n) with h(1) = 0, h'(1) = 1 (notes, section 2).
struct ClosureFn {
  Closure kind = Closure::Graphene;
  double q = 0;  // quantum-capacitance fraction of s_0^2 (graphene only)
  double hp(double n) const {   // h'(n) = s^2(n) / n
    return kind == Closure::ShallowWater ? 1.0 : (1 - q) / std::sqrt(n) + q / n;
  }
  double hpp(double n) const {  // h''(n)
    return kind == Closure::ShallowWater ? 0.0 : -0.5 * (1 - q) * std::pow(n, -1.5) - q / (n * n);
  }
  double h(double n) const {
    return kind == Closure::ShallowWater ? n - 1 : 2 * (1 - q) * (std::sqrt(n) - 1) + q * std::log(n);
  }
  // Contact (electrochemical) potential in units of m_0* s_0^2 / e, Phi(1) = 0.
  double Phi(double n) const {
    return kind == Closure::ShallowWater ? n - 1 : (1 - q) * (n - 1) + 2 * q * (std::sqrt(n) - 1);
  }
  double Phip(double n) const { return kind == Closure::ShallowWater ? 1.0 : (1 - q) + q / std::sqrt(n); }
  double kappa() const { return hpp(1.0); }
  // dc voltage coefficient c_V of |n_1|^2 (notes, section 3): -kappa/4 + Phi''(1)/4.
  double cV() const { return kind == Closure::ShallowWater ? 0.0 : -kappa() / 4 + (-q / 2) / 4; }
};

struct Port {
  enum class Kind { Voltage, Current, Open };
  int tag = 0;
  // first order (ac): Thevenin source Va with series R, or current source I0, or open
  Kind ac = Kind::Voltage;
  cd Va = 0;
  double Rac = 0;
  double I0 = 0;
  // dc: load R to ground (Voltage with Va = 0), current source, or open
  Kind dc = Kind::Voltage;
  double Rdc = 0;
  double Idc = 0;
  static Port ground(int tag) { Port p; p.tag = tag; return p; }
  static Port open(int tag) { Port p; p.tag = tag; p.ac = Kind::Open; p.dc = Kind::Open; return p; }
  static Port thevenin(int tag, cd Va, double R, double Rload) {
    Port p; p.tag = tag; p.Va = Va; p.Rac = R; p.Rdc = Rload; return p;
  }
};

struct ThzParams {
  double Gamma = 0;            // W / (s_0 tau_mr)
  double nu = 0;               // 1 / Re_s
  double tau_ee = 0;           // s_0 tau_ee / W (0 = Newtonian)
  ClosureFn closure;
  double slip = std::numeric_limits<double>::infinity();  // l_s / W; 0 = no-slip
  double corner_deg = 25;
  std::vector<Port> ports;
  bool verbose = false;
  cd nu_omega(double Omega) const { return nu / cd(1.0, Omega * tau_ee); }
};

enum class ThzNode { Interior, Contact, Wall, Pinned };

// Node classification, wall frames and port edge lists for one mesh.
struct ThzGrid {
  const Mesh& m;
  int nn = 0, nv = 0, nd = 0, np = 0, ndof = 0, density_order = 1;
  std::vector<ThzNode> type;        // velocity treatment per node
  std::vector<Vec2> t, n;           // rotated frame at Wall nodes
  std::vector<char> dir;            // Dirichlet (zero) velocity dof flags
  std::vector<int> port_of_node;    // -1 or port index for density nodes on a contact (size nd)
  std::vector<std::vector<int>> port_edges;  // boundary edge indices per port

  ThzGrid(const Mesh& mesh, const ThzParams& p, int density_order_) : m(mesh), density_order(density_order_) {
    nn = m.nn(); nv = m.nv; np = (int)p.ports.size();
    nd = density_order == 2 ? nn : nv;
    ndof = 2 * nn + nd + np;
    type.assign(nn, ThzNode::Interior);
    t.assign(nn, Vec2{1, 0}); n.assign(nn, Vec2{0, 1});
    dir.assign(ndof, 0);
    port_of_node.assign(nd, -1);
    port_edges.assign(np, {});
    std::vector<std::vector<int>> inc(nn);
    for (size_t i = 0; i < m.bedges.size(); ++i) {
      const auto& e = m.bedges[i];
      inc[e.a].push_back((int)i); inc[e.b].push_back((int)i); inc[e.m].push_back((int)i);
      if (e.tag >= 0) {
        int c = -1;
        for (int k = 0; k < np; ++k) if (p.ports[k].tag == e.tag) c = k;
        if (c < 0) throw std::runtime_error("ThzGrid: contact tag without a port");
        port_edges[c].push_back((int)i);
        port_of_node[e.a] = c; port_of_node[e.b] = c;
        if (density_order == 2) port_of_node[e.m] = c;
      }
    }
    double cos_corner = std::cos(deg2rad(p.corner_deg));
    for (int a = 0; a < nn; ++a) {
      if (inc[a].empty()) continue;
      std::vector<Vec2> wn; std::vector<double> wl;
      bool contact = false;
      for (int ei : inc[a]) {
        const auto& e = m.bedges[ei];
        if (e.tag >= 0) contact = true; else { wn.push_back(e.n); wl.push_back(e.len); }
      }
      if (wn.empty()) { type[a] = ThzNode::Contact; continue; }   // velocity free (do-nothing)
      if (p.slip == 0 || (wn.size() >= 2 && dot(wn[0], wn[1]) < cos_corner)) {
        type[a] = ThzNode::Pinned; dir[2 * a] = dir[2 * a + 1] = 1; continue;
      }
      (void)contact;  // a contact-wall corner is a wall node: u.n_wall = 0
      Vec2 nn_ = wn[0];
      if (wn.size() >= 2) nn_ = (wn[0] * wl[0] + wn[1] * wl[1]).unit();
      type[a] = ThzNode::Wall; n[a] = nn_; t[a] = nn_.right();
      dir[2 * a + 1] = 1;
    }
  }
  // Rotation of a Cartesian nodal vector into the nodal frame (identity unless Wall).
  void rot(int a, const double v[2], double out[2]) const {
    if (type[a] == ThzNode::Wall) { out[0] = t[a].x * v[0] + t[a].y * v[1]; out[1] = n[a].x * v[0] + n[a].y * v[1]; }
    else { out[0] = v[0]; out[1] = v[1]; }
  }
  void frame(int a, double R[2][2]) const {
    if (type[a] == ThzNode::Wall) { R[0][0] = t[a].x; R[0][1] = t[a].y; R[1][0] = n[a].x; R[1][1] = n[a].y; }
    else { R[0][0] = 1; R[0][1] = 0; R[1][0] = 0; R[1][1] = 1; }
  }
  int ndof_n(int a) const { return 2 * nn + a; }
  int ndof_port(int c) const { return 2 * nn + nd + c; }
  // P2 edge integral weights of the three edge nodes (a, b, m): int phi_i ds / len.
  static constexpr double edge_w[3] = {1.0 / 6, 1.0 / 6, 2.0 / 3};
};

}  // namespace gd::hydro
