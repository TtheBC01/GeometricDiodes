#include "hydro/thz_solver.hpp"
#include "hydro/fem.hpp"
#include <cstdio>

namespace gd::hydro {

ThzSolver::ThzSolver(const Mesh& m, const ThzParams& p, int density_order)
    : m_(m), p_(p), g1_(m, p, density_order), g0_(m, p, 1) {}

void ThzSolver::flux_row(const ThzGrid& g, int c, std::vector<std::pair<int, double>>& coef) const {
  coef.clear();
  for (int ei : g.port_edges[c]) {
    const auto& e = m_.bedges[ei];
    int nodes[3] = {e.a, e.b, e.m};
    for (int i = 0; i < 3; ++i) {
      int a = nodes[i];
      double w = ThzGrid::edge_w[i] * e.len;
      double v[2] = {w * e.n.x, w * e.n.y}, r[2];
      g.rot(a, v, r);  // row vector times R_a^T: coefficient on the nodal-frame dofs
      for (int k = 0; k < 2; ++k)
        if (!g.dir[2 * a + k] && r[k] != 0) coef.emplace_back(2 * a + k, r[k]);
    }
  }
}

void ThzSolver::assemble_operator(const ThzGrid& g, double Omega, cd nu, std::vector<Eigen::Triplet<cd>>& trip) const {
  const auto& Q = tri_quad7();
  const cd iO(0.0, Omega);
  const double Gamma = p_.Gamma;
  const int ndl = g.density_order == 2 ? 6 : 3;   // local density basis size
  std::array<double, 6> N; std::array<Vec2, 6> dN;
  double Nd[6]; Vec2 dNd[6];
  cd Kuu[6][2][6][2]; double Kun[6][2][6], Knu[6][6][2], Knn[6][6];
  auto scatter_uu = [&](int ga, int gb, const cd K[2][2]) {
    double Ra[2][2], Rb[2][2];
    g.frame(ga, Ra); g.frame(gb, Rb);
    for (int i = 0; i < 2; ++i) {
      if (g.dir[2 * ga + i]) continue;
      for (int j = 0; j < 2; ++j) {
        if (g.dir[2 * gb + j]) continue;
        cd val = 0;
        for (int k = 0; k < 2; ++k)
          for (int l = 0; l < 2; ++l) val += Ra[i][k] * K[k][l] * Rb[j][l];
        if (val != 0.0) trip.emplace_back(2 * ga + i, 2 * gb + j, val);
      }
    }
  };
  for (const auto& T : m_.tri) {
    TriGeom gm = tri_geom(m_.X[T[0]], m_.X[T[1]], m_.X[T[2]]);
    std::fill(&Kuu[0][0][0][0], &Kuu[0][0][0][0] + 144, cd(0));
    std::fill(&Kun[0][0][0], &Kun[0][0][0] + 72, 0.0);
    std::fill(&Knu[0][0][0], &Knu[0][0][0] + 72, 0.0);
    std::fill(&Knn[0][0], &Knn[0][0] + 36, 0.0);
    for (const auto& q : Q) {
      double L[3] = {q.L0, q.L1, q.L2};
      p2_basis(gm, q.L0, q.L1, q.L2, N, dN);
      if (ndl == 6) { for (int i = 0; i < 6; ++i) { Nd[i] = N[i]; dNd[i] = dN[i]; } }
      else { for (int i = 0; i < 3; ++i) { Nd[i] = L[i]; dNd[i] = gm.gL[i]; } }
      double w = q.w * gm.area;
      for (int a = 0; a < 6; ++a) {
        for (int b = 0; b < 6; ++b) {
          cd diag = w * ((iO + Gamma) * N[a] * N[b] + nu * dot(dN[a], dN[b]));
          Kuu[a][0][b][0] += diag; Kuu[a][1][b][1] += diag;
        }
        for (int v = 0; v < ndl; ++v) {          // -int Nd_v div(N_a e_k): gradient integrated by parts
          Kun[a][0][v] += -w * Nd[v] * dN[a].x;
          Kun[a][1][v] += -w * Nd[v] * dN[a].y;
        }
      }
      for (int v = 0; v < ndl; ++v) {
        for (int b = 0; b < 6; ++b) { Knu[v][b][0] += w * Nd[v] * dN[b].x; Knu[v][b][1] += w * Nd[v] * dN[b].y; }
        for (int wv = 0; wv < ndl; ++wv) Knn[v][wv] += w * Nd[v] * Nd[wv];
      }
    }
    for (int a = 0; a < 6; ++a) {
      int ga = T[a];
      for (int b = 0; b < 6; ++b) {
        cd K[2][2] = {{Kuu[a][0][b][0], Kuu[a][0][b][1]}, {Kuu[a][1][b][0], Kuu[a][1][b][1]}};
        scatter_uu(ga, T[b], K);
      }
      for (int v = 0; v < ndl; ++v) {
        double c[2] = {Kun[a][0][v], Kun[a][1][v]}, cr[2]; g.rot(ga, c, cr);
        for (int i = 0; i < 2; ++i) if (!g.dir[2 * ga + i]) trip.emplace_back(2 * ga + i, g.ndof_n(T[v]), cd(cr[i]));
      }
    }
    for (int v = 0; v < ndl; ++v) {
      int gv = T[v];
      int row = g.ndof_n(gv);
      for (int b = 0; b < 6; ++b) {
        int gb = T[b];
        double c[2] = {Knu[v][b][0], Knu[v][b][1]}, cr[2]; g.rot(gb, c, cr);
        for (int j = 0; j < 2; ++j) if (!g.dir[2 * gb + j]) trip.emplace_back(row, 2 * gb + j, cd(cr[j]));
      }
      if (Omega != 0)
        for (int wv = 0; wv < ndl; ++wv) if (Knn[v][wv] != 0) trip.emplace_back(row, g.ndof_n(T[wv]), iO * Knn[v][wv]);
    }
  }
  // Navier slip on wall edges: (1/S) int (u.t)(v.t) ds.
  if (std::isfinite(p_.slip) && p_.slip > 0) {
    const double invS = 1.0 / p_.slip;
    for (const auto& e : m_.bedges) {
      if (e.tag >= 0) continue;
      int nodes[3] = {e.a, e.b, e.m};
      double Ke[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
      for (const auto& q : edge_quad3()) {
        auto phi = p2_edge_basis(q.s);
        double w = q.w * e.len * invS;
        for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) Ke[i][j] += w * phi[i] * phi[j];
      }
      for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
          cd K[2][2] = {{cd(Ke[i][j] * e.t.x * e.t.x), cd(Ke[i][j] * e.t.x * e.t.y)},
                        {cd(Ke[i][j] * e.t.y * e.t.x), cd(Ke[i][j] * e.t.y * e.t.y)}};
          scatter_uu(nodes[i], nodes[j], K);
        }
    }
  }
  // Contact traction: the boundary term of the integrated-by-parts gradient,
  // + int_contact V_c v.n_out ds = -V_c * (flux coefficients of v).
  std::vector<std::pair<int, double>> coef;
  for (int c = 0; c < g.np; ++c) {
    flux_row(g, c, coef);
    for (auto& [k, val] : coef) trip.emplace_back(k, g.ndof_port(c), cd(-val));
  }
  for (int i = 0; i < 2 * g.nn; ++i) if (g.dir[i]) trip.emplace_back(i, i, cd(1.0));
}

void ThzSolver::assemble_ports(const ThzGrid& g, bool dc, const std::vector<double>& known_flux,
                               const std::vector<double>& node_rhs, std::vector<Eigen::Triplet<cd>>& trip,
                               Eigen::VectorXcd& rhs) const {
  (void)node_rhs;  // density is free at contacts; the potential acts through the momentum traction
  std::vector<std::pair<int, double>> coef;
  for (int c = 0; c < g.np; ++c) {
    const Port& P = p_.ports[c];
    int row = g.ndof_port(c);
    flux_row(g, c, coef);
    Port::Kind kind = dc ? P.dc : P.ac;
    double R = dc ? P.Rdc : P.Rac;
    double kf = known_flux.empty() ? 0.0 : known_flux[c];
    switch (kind) {
      case Port::Kind::Voltage:  // R (I_u + kf) + V = Va
        if (R != 0) for (auto& [k, val] : coef) trip.emplace_back(row, k, cd(R * val));
        trip.emplace_back(row, row, cd(1.0));
        rhs[row] = (dc ? cd(0.0) : P.Va) - R * kf;
        break;
      case Port::Kind::Current:  // I_u + kf = I0
        for (auto& [k, val] : coef) trip.emplace_back(row, k, cd(val));
        rhs[row] = (dc ? P.Idc : P.I0) - kf;
        break;
      case Port::Kind::Open:
        for (auto& [k, val] : coef) trip.emplace_back(row, k, cd(val));
        rhs[row] = -kf;
        break;
    }
  }
}

FirstOrder ThzSolver::solve_linear(double Omega) {
  const ThzGrid& g = g1_;
  std::vector<Eigen::Triplet<cd>> trip;
  trip.reserve(m_.tri.size() * 400 + m_.bedges.size() * 60 + g.ndof);
  Eigen::VectorXcd rhs = Eigen::VectorXcd::Zero(g.ndof);
  assemble_operator(g, Omega, p_.nu_omega(Omega), trip);
  assemble_ports(g, false, {}, {}, trip, rhs);
  Eigen::SparseMatrix<cd> A(g.ndof, g.ndof);
  A.setFromTriplets(trip.begin(), trip.end());
  if (!lu_) lu_ = std::make_unique<Eigen::SparseLU<Eigen::SparseMatrix<cd>, Eigen::COLAMDOrdering<int>>>();
  if (!analyzed_) { lu_->analyzePattern(A); analyzed_ = true; }
  lu_->factorize(A);
  if (lu_->info() != Eigen::Success) throw std::runtime_error("ThzSolver: first-order factorisation failed");
  Eigen::VectorXcd x = lu_->solve(rhs);
  FirstOrder f;
  f.Omega = Omega;
  f.n1.resize(g.nn); f.u1x.resize(g.nn); f.u1y.resize(g.nn);
  for (int a = 0; a < g.nn; ++a) {
    cd x0 = x[2 * a], x1 = x[2 * a + 1];
    if (g.type[a] == ThzNode::Wall) { f.u1x[a] = g.t[a].x * x0 + g.n[a].x * x1; f.u1y[a] = g.t[a].y * x0 + g.n[a].y * x1; }
    else { f.u1x[a] = x0; f.u1y[a] = x1; }
  }
  for (int a = 0; a < g.nd; ++a) f.n1[a] = x[g.ndof_n(a)];
  if (g.density_order == 1)   // midside values by linear interpolation
    for (const auto& T : m_.tri) {
      f.n1[T[3]] = 0.5 * (f.n1[T[0]] + f.n1[T[1]]);
      f.n1[T[4]] = 0.5 * (f.n1[T[1]] + f.n1[T[2]]);
      f.n1[T[5]] = 0.5 * (f.n1[T[2]] + f.n1[T[0]]);
    }
  f.V1.resize(g.np); f.I1.resize(g.np);
  std::vector<std::pair<int, double>> coef;
  for (int c = 0; c < g.np; ++c) {
    f.V1[c] = x[g.ndof_port(c)];
    flux_row(g, c, coef);
    cd I = 0; for (auto& [k, val] : coef) I += val * x[k];
    f.I1[c] = I;
  }
  if (p_.verbose) std::printf("  linear Omega=%g: |x|max=%.3e\n", Omega, x.cwiseAbs().maxCoeff());
  return f;
}

SecondOrder ThzSolver::solve_dc(const FirstOrder& f) {
  const ThzGrid& g = g0_;
  std::vector<Eigen::Triplet<cd>> trip;
  trip.reserve(m_.tri.size() * 260 + m_.bedges.size() * 60 + g.ndof);
  Eigen::VectorXcd rhs = Eigen::VectorXcd::Zero(g.ndof);
  assemble_operator(g, 0.0, cd(p_.nu), trip);
  // Second-order forcing: momentum f = -Re[(u1* . grad) u1]/2, continuity g = -Re[div(n1 u1*)]/2,
  // with n1 and u1 both evaluated with the P2 basis.
  const auto& Q = tri_quad7();
  std::array<double, 6> N; std::array<Vec2, 6> dN;
  for (const auto& T : m_.tri) {
    TriGeom gm = tri_geom(m_.X[T[0]], m_.X[T[1]], m_.X[T[2]]);
    cd ux[6], uy[6], ne[6];
    for (int a = 0; a < 6; ++a) { ux[a] = f.u1x[T[a]]; uy[a] = f.u1y[T[a]]; ne[a] = f.n1[T[a]]; }
    double ru[6][2] = {}, rn[3] = {};
    for (const auto& q : Q) {
      double L[3] = {q.L0, q.L1, q.L2};
      p2_basis(gm, q.L0, q.L1, q.L2, N, dN);
      double w = q.w * gm.area;
      cd u[2] = {0, 0}, gu[2][2] = {{0, 0}, {0, 0}}, nq = 0, gn[2] = {0, 0};
      for (int a = 0; a < 6; ++a) {
        u[0] += ux[a] * N[a]; u[1] += uy[a] * N[a];
        gu[0][0] += ux[a] * dN[a].x; gu[0][1] += ux[a] * dN[a].y;
        gu[1][0] += uy[a] * dN[a].x; gu[1][1] += uy[a] * dN[a].y;
        nq += ne[a] * N[a]; gn[0] += ne[a] * dN[a].x; gn[1] += ne[a] * dN[a].y;
      }
      double fk[2];
      for (int k = 0; k < 2; ++k)
        fk[k] = -0.5 * std::real(std::conj(u[0]) * gu[k][0] + std::conj(u[1]) * gu[k][1]);
      cd divu = gu[0][0] + gu[1][1];
      double gq = -0.5 * std::real(gn[0] * std::conj(u[0]) + gn[1] * std::conj(u[1]) + nq * std::conj(divu));
      for (int a = 0; a < 6; ++a) { ru[a][0] += w * N[a] * fk[0]; ru[a][1] += w * N[a] * fk[1]; }
      for (int v = 0; v < 3; ++v) rn[v] += w * L[v] * gq;
    }
    for (int a = 0; a < 6; ++a) {
      int ga = T[a]; double r[2]; g.rot(ga, ru[a], r);
      for (int k = 0; k < 2; ++k) if (!g.dir[2 * ga + k]) rhs[2 * ga + k] += r[k];
    }
    for (int v = 0; v < 3; ++v) rhs[g.ndof_n(T[v])] += rn[v];
  }
  // Known dc flux through each port from the Stokes drift Re(n1 u1*)/2, and the
  // dc contact traction P_ext = V20 - c_V |n1|^2: the known part enters the
  // momentum rows as  rhs -= c_V int |n1|^2 v.n_in ds  (notes, section 3).
  std::vector<double> drift(g.np, 0.0);
  const double cV = p_.closure.cV();
  for (int c = 0; c < g.np; ++c) {
    for (int ei : g.port_edges[c]) {
      const auto& e = m_.bedges[ei];
      int nodes[3] = {e.a, e.b, e.m};
      double tr[3] = {0, 0, 0};
      for (const auto& q : edge_quad3()) {
        auto phi = p2_edge_basis(q.s);
        cd n1 = 0, un = 0;
        for (int i = 0; i < 3; ++i) {
          n1 += phi[i] * f.n1[nodes[i]];
          un += phi[i] * (f.u1x[nodes[i]] * e.n.x + f.u1y[nodes[i]] * e.n.y);
        }
        drift[c] += q.w * e.len * 0.5 * std::real(n1 * std::conj(un));
        for (int i = 0; i < 3; ++i) tr[i] += q.w * e.len * phi[i] * std::norm(n1);
      }
      for (int i = 0; i < 3; ++i) {
        double v[2] = {-cV * tr[i] * e.n.x, -cV * tr[i] * e.n.y}, r[2];
        g.rot(nodes[i], v, r);
        for (int k = 0; k < 2; ++k) if (!g.dir[2 * nodes[i] + k]) rhs[2 * nodes[i] + k] += r[k];
      }
    }
  }
  assemble_ports(g, true, drift, {}, trip, rhs);
  // Solve as a real system.
  std::vector<Eigen::Triplet<double>> rt; rt.reserve(trip.size());
  for (auto& t : trip) rt.emplace_back(t.row(), t.col(), std::real(t.value()));
  Eigen::SparseMatrix<double> A(g.ndof, g.ndof);
  A.setFromTriplets(rt.begin(), rt.end());
  Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> lu;
  lu.analyzePattern(A); lu.factorize(A);
  if (lu.info() != Eigen::Success) throw std::runtime_error("ThzSolver: dc factorisation failed");
  Eigen::VectorXd b = rhs.real();
  Eigen::VectorXd x = lu.solve(b);
  SecondOrder s;
  s.sol.u.resize(g.nn); s.sol.p.resize(g.nv); s.sol.converged = true;
  for (int a = 0; a < g.nn; ++a) {
    double x0 = x[2 * a], x1 = x[2 * a + 1];
    s.sol.u[a] = g.type[a] == ThzNode::Wall ? g.t[a] * x0 + g.n[a] * x1 : Vec2{x0, x1};
  }
  for (int v = 0; v < g.nv; ++v) s.sol.p[v] = x[g.ndof_n(v)];
  s.V20.resize(g.np); s.I20.resize(g.np); s.I20_drift = drift;
  std::vector<std::pair<int, double>> coef;
  for (int c = 0; c < g.np; ++c) {
    s.V20[c] = x[g.ndof_port(c)];
    flux_row(g, c, coef);
    double I = drift[c]; for (auto& [k, val] : coef) I += val * x[k];
    s.I20[c] = I;
  }
  return s;
}

double ThzSolver::dc_resistance(int src, int drn) {
  ThzParams saved = p_;
  for (int c = 0; c < g0_.np; ++c) { p_.ports[c].dc = Port::Kind::Open; }
  p_.ports[src].dc = Port::Kind::Current; p_.ports[src].Idc = 1.0;
  p_.ports[drn].dc = Port::Kind::Voltage; p_.ports[drn].Rdc = 0.0;
  const ThzGrid& g = g0_;
  std::vector<Eigen::Triplet<cd>> trip;
  Eigen::VectorXcd rhs = Eigen::VectorXcd::Zero(g.ndof);
  assemble_operator(g, 0.0, cd(p_.nu), trip);
  assemble_ports(g, true, {}, {}, trip, rhs);
  std::vector<Eigen::Triplet<double>> rt; rt.reserve(trip.size());
  for (auto& t : trip) rt.emplace_back(t.row(), t.col(), std::real(t.value()));
  Eigen::SparseMatrix<double> A(g.ndof, g.ndof);
  A.setFromTriplets(rt.begin(), rt.end());
  Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> lu;
  lu.analyzePattern(A); lu.factorize(A);
  p_ = saved;
  if (lu.info() != Eigen::Success) throw std::runtime_error("ThzSolver: dc resistance factorisation failed");
  Eigen::VectorXd x = lu.solve(Eigen::VectorXd(rhs.real()));
  return x[g.ndof_port(src)];
}

}  // namespace gd::hydro
