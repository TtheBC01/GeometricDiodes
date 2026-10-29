#include "hydro/ns_solver.hpp"
#include "hydro/fem.hpp"
#include <cmath>
#include <cstdio>
#include <map>
#include <stdexcept>

namespace gd::hydro {

double channel_pressure_gradient(double h, double Da, double S, double Q) {
  bool plug = !std::isfinite(S);
  if (Da < 1e-12) {
    if (plug) return 0.0;
    return Q / (2 * h * (h * h / 3 + S * h));
  }
  double k = std::sqrt(Da);
  double A = plug ? 0.0 : 1.0 / (std::cosh(k * h) + S * k * std::sinh(k * h));
  double GoverDa = Q / (2 * h * (1 - A * std::sinh(k * h) / (k * h)));
  return GoverDa * Da;
}

double channel_profile(double y, double h, double Da, double S, double Q) {
  bool plug = !std::isfinite(S);
  if (Da < 1e-12) {
    if (plug) return Q / (2 * h);
    double G = channel_pressure_gradient(h, Da, S, Q);
    return G * ((h * h - y * y) / 2 + S * h);
  }
  double k = std::sqrt(Da);
  double A = plug ? 0.0 : 1.0 / (std::cosh(k * h) + S * k * std::sinh(k * h));
  double GoverDa = channel_pressure_gradient(h, Da, S, Q) / Da;
  return GoverDa * (1 - A * std::cosh(k * y));
}

NSSolver::NSSolver(const Mesh& m, const NSParams& p) : m_(m), p_(p) {
  nn_ = m_.nn();
  ndof_ = 2 * nn_ + m_.nv;
  classify();
  x_ = Eigen::VectorXd::Zero(ndof_);
  impose_dirichlet();
}

void NSSolver::classify() {
  type_.assign(nn_, NodeType::Interior);
  t_.assign(nn_, Vec2{1, 0});
  n_.assign(nn_, Vec2{0, 1});
  std::vector<std::vector<int>> inc(nn_);
  for (size_t i = 0; i < m_.bedges.size(); ++i) {
    const auto& e = m_.bedges[i];
    inc[e.a].push_back((int)i); inc[e.b].push_back((int)i); inc[e.m].push_back((int)i);
  }
  // Inlet plane geometry.
  bool have_in = false;
  double smin = 1e300, smax = -1e300;
  Vec2 ref;
  for (const auto& e : m_.bedges) {
    if (e.tag != p_.inlet) continue;
    if (!have_in) { in_t_ = e.t; in_n_ = e.n; ref = m_.X[e.a]; have_in = true; }
    for (int v : {e.a, e.b}) {
      double s = dot(m_.X[v] - ref, in_t_);
      smin = std::min(smin, s); smax = std::max(smax, s);
    }
  }
  if (!have_in && !p_.dirichlet_all) throw std::runtime_error("NSSolver: no inlet edges with the requested tag");
  if (have_in) {
    in_h_ = 0.5 * (smax - smin);
    in_c_ = ref + in_t_ * (0.5 * (smax + smin));
  }
  double cos_corner = std::cos(deg2rad(p_.corner_deg));
  for (int a = 0; a < nn_; ++a) {
    if (inc[a].empty()) continue;
    if (p_.dirichlet_all) { type_[a] = NodeType::DirichletFn; continue; }
    bool has_in = false, all_out = true;
    std::vector<Vec2> wn;
    std::vector<double> wl;
    for (int ei : inc[a]) {
      const auto& e = m_.bedges[ei];
      if (e.tag == p_.inlet) has_in = true;
      if (e.tag != p_.outlet) all_out = false;
      if (e.tag < 0) { wn.push_back(e.n); wl.push_back(e.len); }
    }
    if (has_in) { type_[a] = NodeType::Inlet; continue; }
    if (all_out) { type_[a] = NodeType::Outlet; continue; }
    if (wn.empty()) { type_[a] = NodeType::Outlet; continue; }  // contact other than inlet/outlet: free
    if (p_.slip == 0) { type_[a] = NodeType::Pinned; continue; }
    if (wn.size() >= 2 && dot(wn[0], wn[1]) < cos_corner) { type_[a] = NodeType::Pinned; continue; }
    // Length-weighted normal (Engelman-Sani-Gresho): with u_a . n_a = 0 the
    // discrete wall flux sum_e int N_a u_h.n ds vanishes node by node, since
    // int_e N_a ds = len_e/6 for both adjacent edges of a vertex.
    Vec2 n = wn[0];
    if (wn.size() >= 2) n = (wn[0] * wl[0] + wn[1] * wl[1]).unit();
    type_[a] = NodeType::Wall;
    n_[a] = n;
    t_[a] = n.right();
  }
}

void NSSolver::impose_dirichlet() {
  dir_.assign(ndof_, 0);
  for (int a = 0; a < nn_; ++a) {
    switch (type_[a]) {
      case NodeType::Inlet: {
        double s = dot(m_.X[a] - in_c_, in_t_);
        double sp = channel_profile(s, in_h_, p_.Da, p_.slip, p_.flux);
        x_[2 * a] = sp * in_n_.x; x_[2 * a + 1] = sp * in_n_.y;
        dir_[2 * a] = dir_[2 * a + 1] = 1;
        break;
      }
      case NodeType::Pinned:
        x_[2 * a] = x_[2 * a + 1] = 0; dir_[2 * a] = dir_[2 * a + 1] = 1; break;
      case NodeType::Wall:
        x_[2 * a + 1] = 0; dir_[2 * a + 1] = 1; break;  // normal component in rotated frame
      case NodeType::DirichletFn: {
        Vec2 u = p_.dirichlet_all(m_.X[a]);
        x_[2 * a] = u.x; x_[2 * a + 1] = u.y; dir_[2 * a] = dir_[2 * a + 1] = 1;
        break;
      }
      default: break;
    }
  }
  if (p_.dirichlet_all) { x_[2 * nn_] = p_.pin_value; dir_[2 * nn_] = 1; }
}

Vec2 NSSolver::node_u(int a) const {
  if (type_[a] == NodeType::Wall) return t_[a] * x_[2 * a] + n_[a] * x_[2 * a + 1];
  return {x_[2 * a], x_[2 * a + 1]};
}

Solution NSSolver::current() const {
  Solution s;
  s.u.resize(nn_);
  for (int a = 0; a < nn_; ++a) s.u[a] = node_u(a);
  s.p.assign(x_.data() + 2 * nn_, x_.data() + ndof_);
  return s;
}

void NSSolver::set_state(const Solution& s) {
  for (int a = 0; a < nn_; ++a) {
    Vec2 u = s.u[a];
    if (type_[a] == NodeType::Wall) { x_[2 * a] = dot(u, t_[a]); x_[2 * a + 1] = dot(u, n_[a]); }
    else { x_[2 * a] = u.x; x_[2 * a + 1] = u.y; }
  }
  for (int v = 0; v < m_.nv; ++v) x_[2 * nn_ + v] = s.p[v];
  impose_dirichlet();  // re-impose exact Dirichlet values
}

void NSSolver::assemble(double Re, Eigen::VectorXd& F, std::vector<Eigen::Triplet<double>>& trip) {
  F.setZero(ndof_);
  trip.clear();
  trip.reserve(m_.tri.size() * 230 + m_.bedges.size() * 40 + ndof_);
  const auto& Q = tri_quad7();
  const double Da = p_.Da;
  std::array<double, 6> N; std::array<Vec2, 6> dN;
  // Local blocks in Cartesian frame.
  double Kuu[6][2][6][2], Kup[6][2][3], Kpu[3][6][2], ru[6][2], rp[3];
  auto rot = [&](int a, const double v[2], double out[2]) {
    if (type_[a] == NodeType::Wall) {
      out[0] = t_[a].x * v[0] + t_[a].y * v[1];
      out[1] = n_[a].x * v[0] + n_[a].y * v[1];
    } else { out[0] = v[0]; out[1] = v[1]; }
  };
  auto scatter_uu = [&](int ga, int gb, const double K[2][2]) {
    // K' = R_a K R_b^T
    double Ra[2][2] = {{1, 0}, {0, 1}}, Rb[2][2] = {{1, 0}, {0, 1}};
    if (type_[ga] == NodeType::Wall) { Ra[0][0] = t_[ga].x; Ra[0][1] = t_[ga].y; Ra[1][0] = n_[ga].x; Ra[1][1] = n_[ga].y; }
    if (type_[gb] == NodeType::Wall) { Rb[0][0] = t_[gb].x; Rb[0][1] = t_[gb].y; Rb[1][0] = n_[gb].x; Rb[1][1] = n_[gb].y; }
    for (int i = 0; i < 2; ++i) {
      if (dir_[2 * ga + i]) continue;
      for (int j = 0; j < 2; ++j) {
        double val = 0;
        for (int k = 0; k < 2; ++k)
          for (int l = 0; l < 2; ++l) val += Ra[i][k] * K[k][l] * Rb[j][l];
        if (val != 0) trip.emplace_back(2 * ga + i, 2 * gb + j, val);
      }
    }
  };
  for (const auto& T : m_.tri) {
    TriGeom g = tri_geom(m_.X[T[0]], m_.X[T[1]], m_.X[T[2]]);
    Vec2 ue[6]; double pe[3];
    for (int a = 0; a < 6; ++a) ue[a] = node_u(T[a]);
    for (int v = 0; v < 3; ++v) pe[v] = x_[2 * nn_ + T[v]];
    std::fill(&Kuu[0][0][0][0], &Kuu[0][0][0][0] + 144, 0.0);
    std::fill(&Kup[0][0][0], &Kup[0][0][0] + 36, 0.0);
    std::fill(&Kpu[0][0][0], &Kpu[0][0][0] + 36, 0.0);
    std::fill(&ru[0][0], &ru[0][0] + 12, 0.0);
    std::fill(rp, rp + 3, 0.0);
    for (const auto& q : Q) {
      double L[3] = {q.L0, q.L1, q.L2};
      p2_basis(g, q.L0, q.L1, q.L2, N, dN);
      double w = q.w * g.area;
      Vec2 u{0, 0};
      double gu[2][2] = {{0, 0}, {0, 0}};  // gu[k][l] = du_k/dx_l
      for (int a = 0; a < 6; ++a) {
        u += ue[a] * N[a];
        gu[0][0] += ue[a].x * dN[a].x; gu[0][1] += ue[a].x * dN[a].y;
        gu[1][0] += ue[a].y * dN[a].x; gu[1][1] += ue[a].y * dN[a].y;
      }
      double pq = L[0] * pe[0] + L[1] * pe[1] + L[2] * pe[2];
      double divu = gu[0][0] + gu[1][1];
      double conv[2] = {u.x * gu[0][0] + u.y * gu[0][1], u.x * gu[1][0] + u.y * gu[1][1]};
      double uk[2] = {u.x, u.y};
      for (int a = 0; a < 6; ++a) {
        double dNa[2] = {dN[a].x, dN[a].y};
        for (int k = 0; k < 2; ++k) {
          ru[a][k] += w * (Re * conv[k] * N[a] + (gu[k][0] * dNa[0] + gu[k][1] * dNa[1]) + Da * uk[k] * N[a] -
                           pq * dNa[k]);
          for (int v = 0; v < 3; ++v) Kup[a][k][v] += -w * L[v] * dNa[k];
        }
        for (int b = 0; b < 6; ++b) {
          double dNb[2] = {dN[b].x, dN[b].y};
          double ugradNb = u.x * dNb[0] + u.y * dNb[1];
          double lap = dNa[0] * dNb[0] + dNa[1] * dNb[1];
          for (int k = 0; k < 2; ++k)
            for (int l = 0; l < 2; ++l) {
              double val = Re * N[b] * gu[k][l] * N[a];
              if (k == l) val += Re * ugradNb * N[a] + lap + Da * N[b] * N[a];
              Kuu[a][k][b][l] += w * val;
            }
        }
      }
      for (int v = 0; v < 3; ++v) {
        rp[v] += -w * L[v] * divu;
        for (int b = 0; b < 6; ++b) { Kpu[v][b][0] += -w * L[v] * dN[b].x; Kpu[v][b][1] += -w * L[v] * dN[b].y; }
      }
    }
    // Scatter with nodal rotations.
    for (int a = 0; a < 6; ++a) {
      int ga = T[a];
      double r[2]; rot(ga, ru[a], r);
      for (int i = 0; i < 2; ++i) if (!dir_[2 * ga + i]) F[2 * ga + i] += r[i];
      for (int b = 0; b < 6; ++b) {
        double K[2][2] = {{Kuu[a][0][b][0], Kuu[a][0][b][1]}, {Kuu[a][1][b][0], Kuu[a][1][b][1]}};
        scatter_uu(ga, T[b], K);
      }
      for (int v = 0; v < 3; ++v) {
        double c[2] = {Kup[a][0][v], Kup[a][1][v]}, cr[2]; rot(ga, c, cr);
        for (int i = 0; i < 2; ++i) if (!dir_[2 * ga + i]) trip.emplace_back(2 * ga + i, 2 * nn_ + T[v], cr[i]);
      }
    }
    for (int v = 0; v < 3; ++v) {
      int gv = 2 * nn_ + T[v];
      if (dir_[gv]) continue;
      F[gv] += rp[v];
      for (int b = 0; b < 6; ++b) {
        int gb = T[b];
        double c[2] = {Kpu[v][b][0], Kpu[v][b][1]}, cr[2]; rot(gb, c, cr);  // row vector times R_b^T
        for (int j = 0; j < 2; ++j) trip.emplace_back(gv, 2 * gb + j, cr[j]);
      }
    }
  }
  // Navier slip on wall edges.
  if (std::isfinite(p_.slip) && p_.slip > 0) {
    const double invS = 1.0 / p_.slip;
    for (const auto& e : m_.bedges) {
      if (e.tag >= 0) continue;
      int nodes[3] = {e.a, e.b, e.m};
      Vec2 un[3] = {node_u(e.a), node_u(e.b), node_u(e.m)};
      double re[3][2] = {{0, 0}, {0, 0}, {0, 0}}, Ke[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
      for (const auto& q : edge_quad3()) {
        auto phi = p2_edge_basis(q.s);
        double ut = 0;
        for (int i = 0; i < 3; ++i) ut += phi[i] * dot(un[i], e.t);
        double w = q.w * e.len * invS;
        for (int i = 0; i < 3; ++i) {
          re[i][0] += w * ut * phi[i] * e.t.x; re[i][1] += w * ut * phi[i] * e.t.y;
          for (int j = 0; j < 3; ++j) Ke[i][j] += w * phi[i] * phi[j];
        }
      }
      for (int i = 0; i < 3; ++i) {
        int ga = nodes[i];
        double r[2]; rot(ga, re[i], r);
        for (int k = 0; k < 2; ++k) if (!dir_[2 * ga + k]) F[2 * ga + k] += r[k];
        for (int j = 0; j < 3; ++j) {
          double K[2][2] = {{Ke[i][j] * e.t.x * e.t.x, Ke[i][j] * e.t.x * e.t.y},
                            {Ke[i][j] * e.t.y * e.t.x, Ke[i][j] * e.t.y * e.t.y}};
          scatter_uu(ga, nodes[j], K);
        }
      }
    }
  }
  for (int i = 0; i < ndof_; ++i) if (dir_[i]) trip.emplace_back(i, i, 1.0);
}

bool NSSolver::newton(double Re) {
  Eigen::VectorXd F, d;
  std::vector<Eigen::Triplet<double>> trip;
  Eigen::SparseMatrix<double> J(ndof_, ndof_);
  if (!lu_) lu_ = std::make_unique<Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>>();
  double r0 = -1;
  for (int it = 0; it < p_.max_newton; ++it) {
    assemble(Re, F, trip);
    double r = F.norm();
    if (r0 < 0) r0 = r;
    if (p_.verbose) std::printf("    Newton Re=%g it=%d |F|=%.3e\n", Re, it, r);
    if (!std::isfinite(r) || r > 1e8 * std::max(1.0, r0)) return false;
    if (r < p_.tol) return true;
    J.setFromTriplets(trip.begin(), trip.end());
    if (!analyzed_) { lu_->analyzePattern(J); analyzed_ = true; }
    lu_->factorize(J);
    if (lu_->info() != Eigen::Success) { if (p_.verbose) std::printf("    factorization failed\n"); return false; }
    d = lu_->solve(F);
    x_ -= d;
  }
  assemble(Re, F, trip);
  return F.norm() < p_.tol;
}

Solution NSSolver::solve(const Solution* init) {
  double Re_cur = 0;
  if (init) { set_state(*init); Re_cur = init->Re; }
  else {
    x_.setZero(); impose_dirichlet();
    if (!newton(0)) { Solution s = current(); s.converged = false; return s; }
  }
  Solution best = current();
  best.Re = Re_cur; best.converged = true;
  // Continuation: first attempt the full jump (cheap when it works, which
  // it does for Re <~ 30 from Stokes), then halve on failure and grow by
  // 1.5 on success, with the step capped at 25 so a long path stays gentle.
  double target = p_.Re;
  double step = target - Re_cur;
  int tries = 0;
  while (Re_cur < target - 1e-12 && tries < 80) {
    double Re_try = std::min(target, Re_cur + step);
    Eigen::VectorXd saved = x_;
    if (p_.verbose) std::printf("  continuation: Re %g -> %g\n", Re_cur, Re_try);
    if (newton(Re_try)) {
      Re_cur = Re_try;
      best = current(); best.Re = Re_cur; best.converged = true;
      step = std::min(step * 1.5, 25.0);
    } else {
      x_ = saved;
      step *= 0.5;
      ++tries;
      if (step < 1e-4 * std::max(1.0, target)) break;
    }
  }
  best.converged = (std::fabs(Re_cur - target) < 1e-12);
  return best;
}

}  // namespace gd::hydro
