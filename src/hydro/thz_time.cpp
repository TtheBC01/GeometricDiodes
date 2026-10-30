#include "hydro/thz_time.hpp"
#include "hydro/fem.hpp"
#include <cmath>
#include <cstdio>

namespace gd::hydro {

ThzTimeSolver::ThzTimeSolver(const Mesh& m, const ThzParams& p, int density_order)
    : m_(m), p_(p), g_(m, p, density_order) {
  x_ = Eigen::VectorXd::Zero(g_.ndof);
  for (int a = 0; a < g_.nd; ++a) x_[g_.ndof_n(a)] = 1.0;   // rest: n = 1, u = 0, V = 0
}

void ThzTimeSolver::contact_h(double V, double& hc, double& dh_dV) const {
  const ClosureFn& c = p_.closure;
  if (c.kind == Closure::ShallowWater) { hc = V; dh_dV = 1.0; return; }
  // Phi(n) = (1-q)(n-1) + 2q(sqrt n - 1) = V, m = sqrt n:
  double q = c.q, A = 1 - q;
  double m = (-2 * q + std::sqrt(4 * q * q + 4 * A * (1 + q + V))) / (2 * A);
  double n = m * m;
  hc = c.h(n);
  dh_dV = c.hp(n) / c.Phip(n);
}

double ThzTimeSolver::port_drive(int cidx, double t, double Omega, bool steady) const {
  const Port& P = p_.ports[cidx];
  if (steady) return P.dc == Port::Kind::Current ? P.Idc : 0.0;   // dc Voltage ports: Va = 0 at dc
  if (P.ac == Port::Kind::Current) return P.I0 * std::cos(Omega * t);
  return std::real(P.Va * std::exp(cd(0.0, Omega * t)));
}

void ThzTimeSolver::assemble(double c0, const Eigen::VectorXd& hist, double t, double Omega, bool steady,
                             Eigen::VectorXd& F, std::vector<Eigen::Triplet<double>>& trip) const {
  const ThzGrid& g = g_;
  const ClosureFn& cl = p_.closure;
  const int ndl = g.density_order == 2 ? 6 : 3;
  const double Gamma = p_.Gamma, nu = p_.nu;
  F.setZero(g.ndof);
  trip.clear();
  trip.reserve(m_.tri.size() * 500 + m_.bedges.size() * 80 + g.ndof);
  const auto& Q = tri_quad7();
  std::array<double, 6> N; std::array<Vec2, 6> dN;
  double Nd[6]; Vec2 dNd[6];
  auto node_u = [&](int a) -> Vec2 {
    if (g.type[a] == ThzNode::Wall) return g.t[a] * x_[2 * a] + g.n[a] * x_[2 * a + 1];
    return {x_[2 * a], x_[2 * a + 1]};
  };
  auto scatter_uu = [&](int ga, int gb, const double K[2][2]) {
    double Ra[2][2], Rb[2][2]; g.frame(ga, Ra); g.frame(gb, Rb);
    for (int i = 0; i < 2; ++i) {
      if (g.dir[2 * ga + i]) continue;
      for (int j = 0; j < 2; ++j) {
        if (g.dir[2 * gb + j]) continue;
        double val = 0;
        for (int k = 0; k < 2; ++k) for (int l = 0; l < 2; ++l) val += Ra[i][k] * K[k][l] * Rb[j][l];
        if (val != 0) trip.emplace_back(2 * ga + i, 2 * gb + j, val);
      }
    }
  };
  double Kuu[6][2][6][2], Kun[6][2][6], Knu[6][6][2], Knn[6][6], ru[6][2], rn[6];
  for (const auto& T : m_.tri) {
    TriGeom gm = tri_geom(m_.X[T[0]], m_.X[T[1]], m_.X[T[2]]);
    Vec2 ue[6]; double ne[6];
    for (int a = 0; a < 6; ++a) ue[a] = node_u(T[a]);
    for (int v = 0; v < ndl; ++v) ne[v] = x_[g.ndof_n(T[v])];
    std::fill(&Kuu[0][0][0][0], &Kuu[0][0][0][0] + 144, 0.0);
    std::fill(&Kun[0][0][0], &Kun[0][0][0] + 72, 0.0);
    std::fill(&Knu[0][0][0], &Knu[0][0][0] + 72, 0.0);
    std::fill(&Knn[0][0], &Knn[0][0] + 36, 0.0);
    std::fill(&ru[0][0], &ru[0][0] + 12, 0.0);
    std::fill(rn, rn + 6, 0.0);
    for (const auto& q : Q) {
      double L[3] = {q.L0, q.L1, q.L2};
      p2_basis(gm, q.L0, q.L1, q.L2, N, dN);
      if (ndl == 6) { for (int i = 0; i < 6; ++i) { Nd[i] = N[i]; dNd[i] = dN[i]; } }
      else { for (int i = 0; i < 3; ++i) { Nd[i] = L[i]; dNd[i] = gm.gL[i]; } }
      double w = q.w * gm.area;
      Vec2 u{0, 0}, udot{0, 0}; double gu[2][2] = {{0, 0}, {0, 0}};
      double n = 0, ndot = 0; Vec2 gn{0, 0};
      for (int a = 0; a < 6; ++a) {
        u += ue[a] * N[a];
        gu[0][0] += ue[a].x * dN[a].x; gu[0][1] += ue[a].x * dN[a].y;
        gu[1][0] += ue[a].y * dN[a].x; gu[1][1] += ue[a].y * dN[a].y;
        if (c0 != 0) {   // time derivative: c0 u - hist (Cartesian history stored per node)
          Vec2 hcart{hist[2 * T[a]], hist[2 * T[a] + 1]};
          udot += (ue[a] * c0 - hcart) * N[a];
        }
      }
      for (int v = 0; v < ndl; ++v) {
        n += ne[v] * Nd[v]; gn += dNd[v] * ne[v];
        if (c0 != 0) ndot += (c0 * ne[v] - hist[g.ndof_n(T[v])]) * Nd[v];
      }
      double hp = cl.hp(n), hpp = cl.hpp(n), hval = cl.h(n);
      double divu = gu[0][0] + gu[1][1];
      double conv[2] = {u.x * gu[0][0] + u.y * gu[0][1], u.x * gu[1][0] + u.y * gu[1][1]};
      double uk[2] = {u.x, u.y}, udk[2] = {udot.x, udot.y};
      for (int a = 0; a < 6; ++a) {
        double dNa[2] = {dN[a].x, dN[a].y};
        for (int k = 0; k < 2; ++k) {
          ru[a][k] += w * (N[a] * (udk[k] + conv[k] + Gamma * uk[k]) - hval * dNa[k] +
                           nu * (gu[k][0] * dNa[0] + gu[k][1] * dNa[1]));
          for (int v = 0; v < ndl; ++v) Kun[a][k][v] += -w * hp * Nd[v] * dNa[k];
        }
        for (int b = 0; b < 6; ++b) {
          double dNb[2] = {dN[b].x, dN[b].y};
          double ugradNb = u.x * dNb[0] + u.y * dNb[1];
          double lap = dNa[0] * dNb[0] + dNa[1] * dNb[1];
          for (int k = 0; k < 2; ++k)
            for (int l = 0; l < 2; ++l) {
              double val = N[b] * gu[k][l] * N[a];
              if (k == l) val += (c0 + Gamma) * N[a] * N[b] + ugradNb * N[a] + nu * lap;
              Kuu[a][k][b][l] += w * val;
            }
        }
      }
      (void)hpp;
      for (int v = 0; v < ndl; ++v) {
        rn[v] += w * Nd[v] * (ndot + dot(gn, u) + n * divu);
        for (int b = 0; b < 6; ++b) {
          Knu[v][b][0] += w * Nd[v] * (gn.x * N[b] + n * dN[b].x);
          Knu[v][b][1] += w * Nd[v] * (gn.y * N[b] + n * dN[b].y);
        }
        for (int wv = 0; wv < ndl; ++wv)
          Knn[v][wv] += w * Nd[v] * (c0 * Nd[wv] + dot(dNd[wv], u) + Nd[wv] * divu);
      }
    }
    for (int a = 0; a < 6; ++a) {
      int ga = T[a]; double r[2]; g.rot(ga, ru[a], r);
      for (int i = 0; i < 2; ++i) if (!g.dir[2 * ga + i]) F[2 * ga + i] += r[i];
      for (int b = 0; b < 6; ++b) {
        double K[2][2] = {{Kuu[a][0][b][0], Kuu[a][0][b][1]}, {Kuu[a][1][b][0], Kuu[a][1][b][1]}};
        scatter_uu(ga, T[b], K);
      }
      for (int v = 0; v < ndl; ++v) {
        double c[2] = {Kun[a][0][v], Kun[a][1][v]}, cr[2]; g.rot(ga, c, cr);
        for (int i = 0; i < 2; ++i) if (!g.dir[2 * ga + i]) trip.emplace_back(2 * ga + i, g.ndof_n(T[v]), cr[i]);
      }
    }
    for (int v = 0; v < ndl; ++v) {
      int row = g.ndof_n(T[v]);
      F[row] += rn[v];
      for (int b = 0; b < 6; ++b) {
        double c[2] = {Knu[v][b][0], Knu[v][b][1]}, cr[2]; g.rot(T[b], c, cr);
        for (int j = 0; j < 2; ++j) if (!g.dir[2 * T[b] + j]) trip.emplace_back(row, 2 * T[b] + j, cr[j]);
      }
      for (int wv = 0; wv < ndl; ++wv) trip.emplace_back(row, g.ndof_n(T[wv]), Knn[v][wv]);
    }
  }
  // Navier slip.
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
        double r[2]; g.rot(nodes[i], re[i], r);
        for (int k = 0; k < 2; ++k) if (!g.dir[2 * nodes[i] + k]) F[2 * nodes[i] + k] += r[k];
        for (int j = 0; j < 3; ++j) {
          double K[2][2] = {{Ke[i][j] * e.t.x * e.t.x, Ke[i][j] * e.t.x * e.t.y},
                            {Ke[i][j] * e.t.y * e.t.x, Ke[i][j] * e.t.y * e.t.y}};
          scatter_uu(nodes[i], nodes[j], K);
        }
      }
    }
  }
  // Ports: contact traction -h_c(V_c) * coef on momentum rows (residual and
  // Jacobian wrt V_c), and the port equation with the nonlinear flux int n u.n ds.
  for (int c = 0; c < g.np; ++c) {
    const Port& P = p_.ports[c];
    int prow = g.ndof_port(c);
    double V = x_[prow], hc, dh; contact_h(V, hc, dh);
    double I = 0;
    for (int ei : g.port_edges[c]) {
      const auto& e = m_.bedges[ei];
      int nodes[3] = {e.a, e.b, e.m};
      // traction (P2 edge basis, exact edge weights)
      for (int i = 0; i < 3; ++i) {
        double w = ThzGrid::edge_w[i] * e.len;
        double v[2] = {w * e.n.x, w * e.n.y}, r[2]; g.rot(nodes[i], v, r);
        for (int k = 0; k < 2; ++k)
          if (!g.dir[2 * nodes[i] + k]) { F[2 * nodes[i] + k] += -hc * r[k]; trip.emplace_back(2 * nodes[i] + k, prow, -dh * r[k]); }
      }
      // flux int n u.n ds with quadrature (n P1 or P2 along the edge)
      Vec2 un[3] = {node_u(e.a), node_u(e.b), node_u(e.m)};
      double nn_[3];
      if (g.density_order == 2) { nn_[0] = x_[g.ndof_n(e.a)]; nn_[1] = x_[g.ndof_n(e.b)]; nn_[2] = x_[g.ndof_n(e.m)]; }
      else { nn_[0] = x_[g.ndof_n(e.a)]; nn_[1] = x_[g.ndof_n(e.b)]; nn_[2] = 0.5 * (nn_[0] + nn_[1]); }
      for (const auto& q : edge_quad3()) {
        auto phi = p2_edge_basis(q.s);
        double nq = 0, unq = 0;
        for (int i = 0; i < 3; ++i) { nq += phi[i] * nn_[i]; unq += phi[i] * dot(un[i], e.n); }
        double w = q.w * e.len;
        I += w * nq * unq;
        // Jacobian of I wrt u dofs and n dofs
        for (int i = 0; i < 3; ++i) {
          double v[2] = {w * nq * phi[i] * e.n.x, w * nq * phi[i] * e.n.y}, r[2]; g.rot(nodes[i], v, r);
          for (int k = 0; k < 2; ++k) if (!g.dir[2 * nodes[i] + k]) trip.emplace_back(prow, 2 * nodes[i] + k, r[k]);
        }
        if (g.density_order == 2) {
          for (int i = 0; i < 3; ++i) trip.emplace_back(prow, g.ndof_n(nodes[i]), w * unq * phi[i]);
        } else {
          double dn[2] = {phi[0] + 0.5 * phi[2], phi[1] + 0.5 * phi[2]};
          trip.emplace_back(prow, g.ndof_n(e.a), w * unq * dn[0]);
          trip.emplace_back(prow, g.ndof_n(e.b), w * unq * dn[1]);
        }
      }
    }
    Port::Kind kind = steady ? P.dc : P.ac;
    double R = steady ? P.Rdc : P.Rac;
    double drive = port_drive(c, t, Omega, steady);
    // The flux Jacobian entries above are for "I"; scale rows for the Voltage kind.
    switch (kind) {
      case Port::Kind::Voltage:   // R I + V - drive = 0
        F[prow] += R * I + V - drive;
        trip.emplace_back(prow, prow, 1.0);
        break;
      case Port::Kind::Current:   // I - drive = 0
        F[prow] += I - drive;
        break;
      case Port::Kind::Open:
        F[prow] += I;
        break;
    }
    if (kind == Port::Kind::Voltage && R != 1.0) {
      // rescale the flux entries just added to this row by R (they were appended last for this port)
      for (auto it = trip.rbegin(); it != trip.rend(); ++it) {
        if (it->row() != prow) continue;
        if (it->col() == prow) continue;
        *it = Eigen::Triplet<double>(it->row(), it->col(), it->value() * R);
      }
    }
  }
  for (int i = 0; i < 2 * g.nn; ++i) if (g.dir[i]) trip.emplace_back(i, i, 1.0);
}

bool ThzTimeSolver::newton(double c0, const Eigen::VectorXd& hist, double t, double Omega, bool steady, double tol,
                           int maxit) {
  Eigen::VectorXd F, d;
  std::vector<Eigen::Triplet<double>> trip;
  Eigen::SparseMatrix<double> J(g_.ndof, g_.ndof);
  if (!lu_) lu_ = std::make_unique<Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>>();
  double r0 = -1;
  for (int it = 0; it < maxit; ++it) {
    assemble(c0, hist, t, Omega, steady, F, trip);
    double r = F.norm();
    if (r0 < 0) r0 = r;
    if (p_.verbose) std::printf("    newton it=%d |F|=%.3e\n", it, r);
    if (!std::isfinite(r)) return false;
    if (r < tol) return true;
    J.setFromTriplets(trip.begin(), trip.end());
    if (!analyzed_) { lu_->analyzePattern(J); analyzed_ = true; }
    lu_->factorize(J);
    if (lu_->info() != Eigen::Success) return false;
    d = lu_->solve(F);
    x_ -= d;
  }
  assemble(c0, hist, t, Omega, steady, F, trip);
  return F.norm() < tol;
}

ThzState ThzTimeSolver::state() const {
  ThzState s;
  s.n.assign(x_.data() + 2 * g_.nn, x_.data() + 2 * g_.nn + g_.nd);
  s.u.resize(g_.nn);
  for (int a = 0; a < g_.nn; ++a)
    s.u[a] = g_.type[a] == ThzNode::Wall ? g_.t[a] * x_[2 * a] + g_.n[a] * x_[2 * a + 1] : Vec2{x_[2 * a], x_[2 * a + 1]};
  s.V.resize(g_.np); s.I.resize(g_.np);
  for (int c = 0; c < g_.np; ++c) {
    s.V[c] = x_[g_.ndof_port(c)];
    double I = 0;
    for (int ei : g_.port_edges[c]) {
      const auto& e = m_.bedges[ei];
      int nodes[3] = {e.a, e.b, e.m};
      double nn_[3];
      if (g_.density_order == 2) for (int i = 0; i < 3; ++i) nn_[i] = x_[g_.ndof_n(nodes[i])];
      else { nn_[0] = x_[g_.ndof_n(e.a)]; nn_[1] = x_[g_.ndof_n(e.b)]; nn_[2] = 0.5 * (nn_[0] + nn_[1]); }
      for (const auto& q : edge_quad3()) {
        auto phi = p2_edge_basis(q.s);
        double nq = 0, unq = 0;
        for (int i = 0; i < 3; ++i) { nq += phi[i] * nn_[i]; unq += phi[i] * dot(s.u[nodes[i]], e.n); }
        I += q.w * e.len * nq * unq;
      }
    }
    s.I[c] = I;
  }
  return s;
}

ThzState ThzTimeSolver::solve_steady(double tol, int max_newton) {
  Eigen::VectorXd hist = Eigen::VectorXd::Zero(g_.ndof);
  // continuation in the drive amplitude: scale all dc sources by lambda
  ThzParams full = p_;
  double lambda = 1.0, done = 0.0;
  Eigen::VectorXd saved = x_;
  int tries = 0;
  while (done < 1.0 - 1e-12 && tries < 40) {
    double target = std::min(1.0, done + lambda);
    for (size_t c = 0; c < p_.ports.size(); ++c) { p_.ports[c].Idc = full.ports[c].Idc * target; }
    if (newton(0.0, hist, 0.0, 0.0, true, tol, max_newton)) { done = target; saved = x_; lambda *= 1.5; }
    else { x_ = saved; lambda *= 0.5; ++tries; if (lambda < 1e-4) break; }
  }
  p_ = full;
  ThzState s = state();
  if (done < 1.0 - 1e-12) s.t = -1;   // flag: not converged to the full drive
  return s;
}

TimeTrace ThzTimeSolver::run(double Omega, int periods, int steps, double tol) {
  TimeTrace tr;
  tr.V.assign(g_.np, {}); tr.I.assign(g_.np, {});
  double Tp = 2 * M_PI / Omega, dt = Tp / steps;
  Eigen::VectorXd xm1 = x_, xm2 = x_, hist(g_.ndof);
  int ntot = periods * steps;
  for (int k = 1; k <= ntot; ++k) {
    double t = k * dt, c0;
    if (k == 1) { c0 = 1.0 / dt; hist = xm1 / dt; }                       // BDF1 start
    else { c0 = 1.5 / dt; hist = (2.0 * xm1 - 0.5 * xm2) / dt; }           // BDF2: (3x - 4x1 + x2)/(2dt)
    // history for velocity must be Cartesian per node: convert the rotated dofs
    for (int a = 0; a < g_.nn; ++a) if (g_.type[a] == ThzNode::Wall) {
      auto cart = [&](const Eigen::VectorXd& v) { return g_.t[a] * v[2 * a] + g_.n[a] * v[2 * a + 1]; };
      Vec2 h = (k == 1) ? cart(xm1) * (1.0 / dt) : (cart(xm1) * 2.0 - cart(xm2) * 0.5) * (1.0 / dt);
      hist[2 * a] = h.x; hist[2 * a + 1] = h.y;
    }
    if (!newton(c0, hist, t, Omega, false, tol, 25)) { tr.ok = false; break; }
    xm2 = xm1; xm1 = x_;
    ThzState s = state();
    tr.t.push_back(t);
    for (int c = 0; c < g_.np; ++c) { tr.V[c].push_back(s.V[c]); tr.I[c].push_back(s.I[c]); }
  }
  tr.V_dc.assign(g_.np, 0.0); tr.I_dc.assign(g_.np, 0.0);
  if (tr.ok && (int)tr.t.size() >= steps) {
    int n0 = (int)tr.t.size() - steps;
    for (int c = 0; c < g_.np; ++c) {
      for (int k = n0; k < (int)tr.t.size(); ++k) { tr.V_dc[c] += tr.V[c][k]; tr.I_dc[c] += tr.I[c][k]; }
      tr.V_dc[c] /= steps; tr.I_dc[c] /= steps;
    }
  }
  return tr;
}

}  // namespace gd::hydro
