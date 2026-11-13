#include "pic.hpp"
#include <Eigen/Dense>
#include <chrono>
#include <stdexcept>
#include <thread>

namespace gd {

// ===========================================================================
// Electro
// ===========================================================================
struct Electro::Impl {
  Eigen::LLT<Eigen::MatrixXd> llt;
  Eigen::VectorXd rhs, sol;
};

Electro::Electro() : impl_(new Impl) {}
Electro::~Electro() = default;

void Electro::init(int nx, int ny, double h, double eps_r, double d_gate, double filter_cells) {
  nx_ = nx; ny_ = ny; h_ = h; d_ = d_gate; kappa_ = kCoulomb / eps_r;
  int lgx = 1, lgy = 1;
  while ((1 << lgx) < 2 * nx) ++lgx;
  while ((1 << lgy) < 2 * ny) ++lgy;
  Px_ = 1 << lgx; Py_ = 1 << lgy;
  fft_.reset(new FFT2D(lgx, lgy));
  const long PP = (long)Px_ * Py_;
  Kre_.assign(PP, 0.0); Kim_.assign(PP, 0.0);
  wre_.assign(PP, 0.0); wim_.assign(PP, 0.0);
  // Point kernel at node offsets. The value at zero offset is chosen so that
  // the lattice sum of a smooth charge equals the integral of 1/r to second
  // order in h: sum' h^2 / r = 2 pi R - 3.900265 h for a disc of radius R, so
  // the missing piece of the integral is 3.900265 / h. (The potential of a
  // uniformly charged square cell at its own centre, 3.525494 / h, would leave
  // a first-order error in the potential of a smooth sheet.)
  const double self = 3.900264920 / h;
  for (int dj = -(ny - 1); dj <= ny - 1; ++dj)
    for (int di = -(nx - 1); di <= nx - 1; ++di) {
      double r2 = h * h * ((double)di * di + (double)dj * dj);
      double v = (di == 0 && dj == 0) ? self : 1.0 / std::sqrt(r2);
      if (d_gate > 0) v -= 1.0 / std::sqrt(r2 + 4.0 * d_gate * d_gate);
      Kre_[(long)((dj + Py_) % Py_) * Px_ + ((di + Px_) % Px_)] = kappa_ * v;
    }
  fft_->forward(Kre_.data(), Kim_.data());
  if (filter_cells > 0) {
    const double s = filter_cells * h;
    for (int b = 0; b < Py_; ++b)
      for (int a = 0; a < Px_; ++a) {
        double kx = 2 * kPi * (a <= Px_ / 2 ? a : a - Px_) / (Px_ * h);
        double ky = 2 * kPi * (b <= Py_ / 2 ? b : b - Py_) / (Py_ * h);
        double f = std::exp(-0.5 * (kx * kx + ky * ky) * s * s);
        Kre_[(long)b * Px_ + a] *= f; Kim_[(long)b * Px_ + a] *= f;
      }
  }
  // effective real-space kernel (what a unit charge at a node produces)
  std::vector<double> re = Kre_, im = Kim_;
  fft_->inverse(re.data(), im.data());
  keff_.assign((long)(2 * nx - 1) * (2 * ny - 1), 0.0);
  for (int dj = -(ny - 1); dj <= ny - 1; ++dj)
    for (int di = -(nx - 1); di <= nx - 1; ++di)
      keff_[(long)(dj + ny - 1) * (2 * nx - 1) + (di + nx - 1)] = re[(long)((dj + Py_) % Py_) * Px_ + ((di + Px_) % Px_)] / (double)PP;
  blocks_.clear(); nbodies_ = 0;
  rho_m_.assign((long)nx * ny, 0.0); Um_.assign((long)nx * ny, 0.0);
}

double Electro::kernel(int di, int dj) const {
  return keff_[(long)(dj + ny_ - 1) * (2 * nx_ - 1) + (di + nx_ - 1)];
}

int Electro::add_metal(const std::vector<std::vector<int>>& blocks) {
  for (const auto& b : blocks) if (!b.empty()) blocks_.push_back({b, nbodies_});
  return nbodies_++;
}

void Electro::finalize() {
  const int nb = (int)blocks_.size();
  if (nb == 0) return;
  Eigen::MatrixXd M(nb, nb);
  int threads = std::max(1u, std::thread::hardware_concurrency());
  auto work = [&](int t) {
    for (int I = t; I < nb; I += threads)
      for (int J = I; J < nb; ++J) {
        double s = 0;
        for (int a : blocks_[I].nodes) {
          int ai = a % nx_, aj = a / nx_;
          for (int b : blocks_[J].nodes) s += kernel(ai - b % nx_, aj - b / nx_);
        }
        s /= (double)blocks_[I].nodes.size() * (double)blocks_[J].nodes.size();
        M(I, J) = s; M(J, I) = s;
      }
  };
  std::vector<std::thread> pool;
  for (int t = 0; t < threads; ++t) pool.emplace_back(work, t);
  for (auto& t : pool) t.join();
  // Blocks finer than the low-pass filter are nearly indistinguishable and make
  // the matrix singular to round-off; a ridge far below the physical scale
  // removes that without changing the solution on resolved scales.
  double ridge = 0;
  for (int tries = 0; tries < 8; ++tries) {
    Eigen::MatrixXd A = M;
    if (ridge > 0) A.diagonal().array() += ridge;
    impl_->llt.compute(A);
    if (impl_->llt.info() == Eigen::Success) break;
    ridge = ridge > 0 ? 100 * ridge : 1e-12 * M.diagonal().mean();
  }
  if (impl_->llt.info() != Eigen::Success) throw std::runtime_error("Electro: block potential matrix is not positive definite");
  impl_->rhs.resize(nb); impl_->sol.resize(nb);
}

void Electro::convolve(const double* rho, double* U) const {
  const long PP = (long)Px_ * Py_;
  std::fill(wre_.begin(), wre_.begin() + (long)ny_ * Px_, 0.0);
  std::fill(wim_.begin(), wim_.begin() + (long)ny_ * Px_, 0.0);
  for (int j = 0; j < ny_; ++j) std::copy(rho + (long)j * nx_, rho + (long)(j + 1) * nx_, wre_.begin() + (long)j * Px_);
  // rows beyond ny_ are zero on input: clear them (they hold the previous result) only where the
  // column pass reads them, i.e. everywhere, but skip their row transform
  std::fill(wre_.begin() + (long)ny_ * Px_, wre_.end(), 0.0);
  std::fill(wim_.begin() + (long)ny_ * Px_, wim_.end(), 0.0);
  fft_->forward(wre_.data(), wim_.data(), ny_);
  for (long i = 0; i < PP; ++i) {
    double a = wre_[i], b = wim_[i];
    wre_[i] = a * Kre_[i] - b * Kim_[i];
    wim_[i] = a * Kim_[i] + b * Kre_[i];
  }
  fft_->inverse(wre_.data(), wim_.data(), ny_);
  const double sc = 1.0 / (double)PP;
  for (int j = 0; j < ny_; ++j)
    for (int i = 0; i < nx_; ++i) U[(long)j * nx_ + i] = wre_[(long)j * Px_ + i] * sc;
}

void Electro::solve(const std::vector<double>& rho, const std::vector<double>& Uc,
                    std::vector<double>& U, std::vector<double>& q) {
  U.resize((long)nx_ * ny_);
  convolve(rho.data(), U.data());
  q.assign(nbodies_, 0.0);
  const int nb = (int)blocks_.size();
  if (nb == 0) return;
  for (int I = 0; I < nb; ++I) {
    double s = 0;
    for (int a : blocks_[I].nodes) s += U[a];
    impl_->rhs[I] = Uc[blocks_[I].body] - s / (double)blocks_[I].nodes.size();
  }
  impl_->sol = impl_->llt.solve(impl_->rhs);
  std::fill(rho_m_.begin(), rho_m_.end(), 0.0);
  for (int I = 0; I < nb; ++I) {
    double per = impl_->sol[I] / (double)blocks_[I].nodes.size();
    for (int a : blocks_[I].nodes) rho_m_[a] = per;
    q[blocks_[I].body] += impl_->sol[I];
  }
  convolve(rho_m_.data(), Um_.data());
  for (size_t i = 0; i < U.size(); ++i) U[i] += Um_[i];
  resid_ = 0;
  for (int I = 0; I < nb; ++I) {
    double s = 0;
    for (int a : blocks_[I].nodes) s += U[a];
    resid_ = std::max(resid_, std::fabs(s / (double)blocks_[I].nodes.size() - Uc[blocks_[I].body]));
  }
}

// ===========================================================================
// Pic
// ===========================================================================
struct Pic::Worker {
  std::vector<P> parts;
  Rng rng;
  std::vector<double> rho;
  double acc[2] = {0, 0};
  // per-step tallies
  double absorbed[2] = {0, 0}, injected[2] = {0, 0}, sumvx = 0, sumE = 0;
  long leaked = 0;
};

Pic::~Pic() = default;

double Pic::drive(double t) const {
  double env = 1.0;
  if (p_.ramp_fs > 0 && t < p_.ramp_fs) { double s = std::sin(0.5 * kPi * t / p_.ramp_fs); env = s * s; }
  return env * (p_.f_Hz > 0 ? p_.Va * std::cos(omega_ * t) : p_.V);
}

Vec2 Pic::imposed_force(double x, double t) const {
  double fx = 0;
  if (p_.imposed == 1) fx = drive(t) / L_;
  else if (p_.imposed == 2) fx = (x >= x_lo_ && x < x_hi_) ? drive(t) / p_.ell_F : 0.0;
  if (p_.ext_U1 != 0) fx += p_.ext_U1 * p_.ext_q * std::sin(p_.ext_q * (x - xmin_));
  return {fx, 0};
}

void Pic::deposit(std::vector<double>& a, double x, double y, double w) const {
  double fi = (x - gx0_) / p_.h, fj = (y - gy0_) / p_.h;
  int i0 = (int)std::floor(fi), j0 = (int)std::floor(fj);
  if (i0 < 0 || j0 < 0 || i0 + 1 >= nx_ || j0 + 1 >= ny_) return;
  double fx = fi - i0, fy = fj - j0;
  long n00 = (long)j0 * nx_ + i0;
  a[fold_[n00]] += w * (1 - fx) * (1 - fy);
  a[fold_[n00 + 1]] += w * fx * (1 - fy);
  a[fold_[n00 + nx_]] += w * (1 - fx) * fy;
  a[fold_[n00 + nx_ + 1]] += w * fx * fy;
}

Pic::Pic(const Boundary& b, PicParams p) : b_(b), p_(p) {
  if (b_.num_contacts() != 2) throw std::runtime_error("Pic: the device must have two contacts");
  xmin_ = b_.bbox_min().x; xmax_ = b_.bbox_max().x; L_ = xmax_ - xmin_;
  // The grid spacing is adjusted so that both contact planes lie on node
  // columns: the metal then ends exactly where the electrons are injected, and
  // the potential an injected electron starts from is the contact's.
  const int nL = std::max(2, (int)std::floor(L_ / p_.h + 0.5));
  p_.h = L_ / nL;
  const double h = p_.h;
  dt_ = p_.dt > 0 ? p_.dt : 0.5 * h / kVF;
  omega_ = 2 * kPi * p_.f_Hz * 1e-15;
  double xF = std::isnan(p_.x_F) ? 0.5 * (xmin_ + xmax_) : p_.x_F;
  x_lo_ = xF - 0.5 * p_.ell_F; x_hi_ = xF + 0.5 * p_.ell_F;
  const bool metal = p_.interaction && p_.contacts;

  int mpad = metal ? (int)std::ceil(p_.contact_len / h) + 2 : 3;
  gx0_ = xmin_ - mpad * h;
  nx_ = nL + 2 * mpad + 1;
  gy0_ = b_.bbox_min().y - 2.5 * h;
  ny_ = (int)std::ceil((b_.bbox_max().y - gy0_) / h) + 4;
  const long NN = (long)nx_ * ny_;
  fold_.resize(NN);
  for (long i = 0; i < NN; ++i) fold_[i] = (int)i;

  // ---- metal strips, blocks and the folding of charge at the contacts ----
  // Node columns iS (source plane) and iD (drain plane) and everything behind
  // them are metal. The plane column has one unknown per node, so that the
  // potential on the plane is the contact's at every node; behind it the
  // unknowns are blocks of contact_block x contact_block nodes.
  std::vector<std::vector<int>> blocksS, blocksD;
  if (metal) {
    const int iS = mpad, iD = mpad + nL;
    for (int c = 0; c < 2; ++c) {
      double ylo = 1e300, yhi = -1e300;
      for (int si : b_.contact_segments(c)) {
        const Segment& sg = b_.segments()[si];
        ylo = std::min({ylo, sg.a.y, sg.b.y}); yhi = std::max({yhi, sg.a.y, sg.b.y});
      }
      int jlo = (int)std::ceil((ylo - gy0_) / h - 0.5), jhi = (int)std::floor((yhi - gy0_) / h + 0.5);
      jlo = std::max(jlo, 0); jhi = std::min(jhi, ny_ - 1);
      int ncol = std::min((int)std::floor(p_.contact_len / h + 0.5), mpad);   // columns behind the plane
      int nb = std::max({1, p_.contact_block, (int)p_.filter});
      int pb = std::max(1, (int)p_.filter);   // nodes per unknown along the plane column
      auto& blocks = c == 0 ? blocksS : blocksD;
      for (int j = jlo; j <= jhi; j += pb) {
        std::vector<int> blk;
        for (int jj = j; jj <= std::min(jhi, j + pb - 1); ++jj) blk.push_back(jj * nx_ + (c == 0 ? iS : iD));
        blocks.push_back(blk);
      }
      for (int cb = 0; cb * nb < ncol; ++cb)
        for (int j = jlo; j <= jhi; j += nb) {
          std::vector<int> blk;
          for (int cc = cb * nb; cc < std::min(ncol, (cb + 1) * nb); ++cc) {
            int i = c == 0 ? iS - 1 - cc : iD + 1 + cc;
            for (int jj = j; jj <= std::min(jhi, j + nb - 1); ++jj) blk.push_back(jj * nx_ + i);
          }
          blocks.push_back(blk);
        }
      // charge assigned to a metal node is put on the nearest sheet node of the row
      for (int j = jlo; j <= jhi; ++j) {
        if (c == 0) for (int i = 0; i <= iS; ++i) fold_[(long)j * nx_ + i] = j * nx_ + iS + 1;
        else for (int i = iD; i < nx_; ++i) fold_[(long)j * nx_ + i] = j * nx_ + iD - 1;
      }
    }
  }
  if (p_.interaction) {
    es_.init(nx_, ny_, h, p_.eps_r, p_.d_gate, p_.filter);
    if (metal) { es_.add_metal(blocksS); es_.add_metal(blocksD); es_.finalize(); }
  }

  // ---- safe cells, background charge, area --------------------------------
  safe_.assign(NN, 0); out_.assign(NN, 0);
  for (int j = 0; j < ny_; ++j)
    for (int i = 0; i < nx_; ++i) {
      Vec2 q = node(i, j);
      bool in = b_.contains(q);
      double dist = b_.distance_to_boundary(q);
      safe_[(long)j * nx_ + i] = in && dist > 0.7072 * h + kVF * dt_ + 1e-3;
      out_[(long)j * nx_ + i] = !in && dist > 1.5 * h;
    }
  for (int c = 0; c < 2; ++c) {
    TransportParams tp;
    tp.EF = p_.EF; tp.T = p_.T; tp.equilibrium_injection = true; tp.inject_contact = c;
    Transport tr(b_, tp);
    table_[c] = tr.injection_table();
    rate_[c] = kValleySpin * table_[c].Z / (2 * kPi * kHbar);
  }
  neq_ = kPi * rate_[0] / (kVF * b_.contact_width(0));
  bg_.assign(NN, 0.0);
  {
    const int sub = 16;
    const double s = h / sub;
    long count = 0;
    Vec2 lo = b_.bbox_min(), hi = b_.bbox_max();
    int na = (int)std::ceil((hi.x - lo.x) / s), nbb = (int)std::ceil((hi.y - lo.y) / s);
    for (int bb = 0; bb < nbb; ++bb)
      for (int a = 0; a < na; ++a) {
        Vec2 q{lo.x + (a + 0.5) * s, lo.y + (bb + 0.5) * s};
        if (!b_.contains(q)) continue;
        ++count;
        deposit(bg_, q.x, q.y, neq_ * s * s);
      }
    area_ = count * s * s;
  }
  w_ = neq_ * area_ / (double)p_.N;

  // ---- workers and the equilibrium fill -----------------------------------
  int T = p_.threads > 0 ? p_.threads : (int)std::max(1u, std::thread::hardware_concurrency());
  workers_.resize(T);
  for (int t = 0; t < T; ++t) {
    workers_[t].reset(new Worker);
    Worker& wk = *workers_[t];
    wk.rng.reseed(p_.seed * 0x9E3779B97F4A7C15ULL + 7919ULL * (t + 1));
    wk.rho.assign(NN, 0.0);
    long n = p_.N / T + (t < p_.N % T ? 1 : 0);
    wk.parts.reserve(n + n / 8 + 1024);
    wk.acc[0] = wk.rng.uniform(); wk.acc[1] = wk.rng.uniform();
    while ((long)wk.parts.size() < n) {
      Vec2 q = b_.random_interior_point(wk.rng);
      if (p_.init_amp != 0) {
        double f = (1 + p_.init_amp * std::cos(p_.init_m * kPi * (q.x - xmin_) / L_)) / (1 + std::fabs(p_.init_amp));
        if (wk.rng.uniform() > f) continue;
      }
      double k = table_[0].sample(wk.rng) / kHbarVF, th = wk.rng.uniform(0, 2 * kPi);
      wk.parts.push_back({q.x, q.y, k * std::cos(th), k * std::sin(th)});
    }
  }
  rho_.assign(NN, 0.0); U_.assign(NN, 0.0);
  q_.assign(2, 0.0);
  // initial charge and field
  std::vector<std::thread> pool;
  for (int t = 0; t < T; ++t)
    pool.emplace_back([this, t]() {
      Worker& wk = *workers_[t];
      std::fill(wk.rho.begin(), wk.rho.end(), 0.0);
      for (const P& q : wk.parts) deposit(wk.rho, q.x, q.y, w_);
    });
  for (auto& t : pool) t.join();
  fields();
  PicStep s0;
  s0.t = 0; s0.V = drive(0); s0.qS = q_[0]; s0.qD = q_[1]; s0.np = num_particles();
  for (long i = 0; i < NN; ++i) s0.Qg += rho_[i];
  hist_.push_back(s0);
}

long Pic::num_particles() const {
  long n = 0;
  for (const auto& w : workers_) n += (long)w->parts.size();
  return n;
}

// Sum the workers' charge, subtract the background, and compute the potential
// and the force at the current time.
void Pic::fields() {
  const long NN = (long)nx_ * ny_;
  std::fill(rho_.begin(), rho_.end(), 0.0);
  for (const auto& w : workers_)
    for (long i = 0; i < NN; ++i) rho_[i] += w->rho[i];
  for (long i = 0; i < NN; ++i) rho_[i] -= bg_[i];
  if (!p_.interaction) return;
  double V = drive(t_);
  std::vector<double> Uc(2);
  switch (p_.mode) {
    case DriveMode::Sym: Uc[0] = 0.5 * V; Uc[1] = -0.5 * V; break;
    case DriveMode::GateDrain: Uc[0] = V; Uc[1] = 0; break;
    case DriveMode::GateSource: Uc[0] = 0; Uc[1] = -V; break;
  }
  es_.solve(rho_, Uc, U_, q_);
  if (q_.size() < 2) q_.resize(2, 0.0);
}

// Free flight over the path length rem with wall reflections. Returns the
// contact that absorbed the particle, -1 if it is still inside.
int Pic::move(P& p, double rem, Rng& rng) const {
  int last = -1;
  for (int guard = 0; guard < 200 && rem > 0; ++guard) {
    double kn = std::sqrt(p.kx * p.kx + p.ky * p.ky);
    Vec2 d = kn > 0 ? Vec2{p.kx / kn, p.ky / kn} : Vec2{1, 0};
    if (last < 0) {
      int i = (int)std::floor((p.x - gx0_) / p_.h + 0.5), j = (int)std::floor((p.y - gy0_) / p_.h + 0.5);
      if (i >= 0 && j >= 0 && i < nx_ && j < ny_ && safe_[(long)j * nx_ + i]) { p.x += d.x * rem; p.y += d.y * rem; return -1; }
    }
    Hit hit;
    if (!b_.nearest_hit({p.x, p.y}, d, rem, last, hit)) { p.x += d.x * rem; p.y += d.y * rem; return -1; }
    rem -= hit.t;
    const Segment& s = b_.segments()[hit.seg];
    if (s.kind == SegKind::Contact && p_.contacts) return s.contact;
    if (s.kind == SegKind::Contact || rng.uniform() < p_.specularity) {
      double kd = 2.0 * (p.kx * s.n.x + p.ky * s.n.y);
      p.kx -= kd * s.n.x; p.ky -= kd * s.n.y;
    } else {
      double a = rng.cosine_angle();
      Vec2 nd = s.n * std::cos(a) + s.d * std::sin(a);
      p.kx = nd.x * kn; p.ky = nd.y * kn;
    }
    p.x = hit.p.x + s.n.x * 1e-7; p.y = hit.p.y + s.n.y * 1e-7;
    last = hit.seg;
  }
  return -1;
}

void Pic::step() {
  const int T = (int)workers_.size();
  const double t0 = t_, dt = dt_, h = p_.h;
  const double pscat = std::isinf(p_.lambda_mfp) ? 0.0 : 1.0 - std::exp(-kVF * dt / p_.lambda_mfp);
  const bool grid_force = p_.interaction;
  const bool ext_force = p_.imposed != 0 || p_.ext_U1 != 0;
  const double kick = dt / kHbar, invh = 1.0 / h;
  auto work = [&](int ti) {
    Worker& wk = *workers_[ti];
    std::fill(wk.rho.begin(), wk.rho.end(), 0.0);
    wk.absorbed[0] = wk.absorbed[1] = wk.injected[0] = wk.injected[1] = 0;
    wk.sumvx = wk.sumE = 0; wk.leaked = 0;
    auto& ps = wk.parts;
    for (size_t n = 0; n < ps.size();) {
      P& q = ps[n];
      double fx = 0, fy = 0;
      double fi = (q.x - gx0_) / h, fj = (q.y - gy0_) / h;
      int i0 = (int)std::floor(fi), j0 = (int)std::floor(fj);
      if (i0 < 0 || j0 < 0 || i0 + 1 >= nx_ || j0 + 1 >= ny_ || out_[(long)(j0 + (fj - j0 > 0.5)) * nx_ + i0 + (fi - i0 > 0.5)]) {
        ++wk.leaked; ps[n] = ps.back(); ps.pop_back(); continue;
      }
      if (grid_force) {
        // minus the gradient of the bilinearly interpolated potential: the work
        // done on a particle between two points is then exactly the difference
        // of the interpolated potential, in particular from a contact plane,
        // where it equals the contact potential
        double ax = fi - i0, ay = fj - j0;
        long n00 = (long)j0 * nx_ + i0;
        double u00 = U_[n00], u10 = U_[n00 + 1], u01 = U_[n00 + nx_], u11 = U_[n00 + nx_ + 1];
        fx = -((1 - ay) * (u10 - u00) + ay * (u11 - u01)) * invh;
        fy = -((1 - ax) * (u01 - u00) + ax * (u11 - u10)) * invh;
      }
      if (ext_force) { Vec2 f = imposed_force(q.x, t0); fx += f.x; fy += f.y; }
      q.kx += fx * kick; q.ky += fy * kick;
      if (pscat > 0 && wk.rng.uniform() < pscat) {
        double kn = std::sqrt(q.kx * q.kx + q.ky * q.ky), a = wk.rng.uniform(0, 2 * kPi);
        q.kx = kn * std::cos(a); q.ky = kn * std::sin(a);
      }
      int c = move(q, kVF * dt, wk.rng);
      if (c >= 0) { wk.absorbed[c] += 1; ps[n] = ps.back(); ps.pop_back(); continue; }
      deposit(wk.rho, q.x, q.y, w_);
      double kn = std::sqrt(q.kx * q.kx + q.ky * q.ky);
      if (kn > 0) wk.sumvx += q.kx / kn;
      wk.sumE += kHbarVF * kn;
      ++n;
    }
    if (p_.contacts) {
      for (int c = 0; c < 2; ++c) {
        wk.acc[c] += rate_[c] * dt / w_ / T;
        const auto& csegs = b_.contact_segments(c);
        while (wk.acc[c] >= 1.0) {
          wk.acc[c] -= 1.0;
          double Wc = b_.contact_width(c), u = wk.rng.uniform() * Wc;
          int sidx = csegs.back();
          for (int si : csegs) { double l = b_.segments()[si].len; if (u <= l) { sidx = si; break; } u -= l; }
          const Segment& cs = b_.segments()[sidx];
          Vec2 pos = cs.a + cs.d * (wk.rng.uniform() * cs.len) + cs.n * 1e-7;
          double th = wk.rng.cosine_angle();
          Vec2 dir = cs.n * std::cos(th) + cs.d * std::sin(th);
          double k = table_[c].sample(wk.rng) / kHbarVF;
          P q{pos.x, pos.y, dir.x * k, dir.y * k};
          wk.injected[c] += 1;
          int cc = move(q, wk.rng.uniform() * kVF * dt, wk.rng);
          if (cc >= 0) { wk.absorbed[cc] += 1; continue; }
          deposit(wk.rho, q.x, q.y, w_);
          wk.sumvx += dir.x; wk.sumE += kHbarVF * k;
          ps.push_back(q);
        }
      }
    }
  };
  auto c0 = std::chrono::steady_clock::now();
  std::vector<std::thread> pool;
  for (int t = 1; t < T; ++t) pool.emplace_back(work, t);
  work(0);
  for (auto& t : pool) t.join();
  auto c1 = std::chrono::steady_clock::now();
  t_ += dt;
  fields();
  auto c2 = std::chrono::steady_clock::now();
  tpush_ += std::chrono::duration<double>(c1 - c0).count(); tfield_ += std::chrono::duration<double>(c2 - c1).count();
  PicStep s;
  s.t = t_; s.V = drive(t_);
  switch (p_.mode) {
    case DriveMode::Sym: s.US = 0.5 * s.V; s.UD = -0.5 * s.V; break;
    case DriveMode::GateDrain: s.US = s.V; s.UD = 0; break;
    case DriveMode::GateSource: s.US = 0; s.UD = -s.V; break;
  }
  double sumvx = 0, sumE = 0;
  for (const auto& wp : workers_) {
    s.absS += (wp->absorbed[0] - wp->injected[0]) * w_;
    s.absD += (wp->absorbed[1] - wp->injected[1]) * w_;
    sumvx += wp->sumvx; sumE += wp->sumE; leaked_ += wp->leaked;
  }
  s.np = num_particles();
  s.jvol = w_ * kVF * sumvx / L_;
  s.Ek = s.np ? sumE / (double)s.np : 0;
  s.qS = q_[0]; s.qD = q_[1];
  const long NN = (long)nx_ * ny_;
  for (long i = 0; i < NN; ++i) s.Qg += rho_[i];
  hist_.push_back(s);
  if (averaging_) {
    double c = std::cos(omega_ * t_), sn = std::sin(omega_ * t_);
    for (long i = 0; i < NN; ++i) { Uavg_[i] += U_[i]; Ucos_[i] += U_[i] * c; Usin_[i] += U_[i] * sn; ravg_[i] += rho_[i]; }
    ++navg_;
  }
}

void Pic::start_average() {
  const long NN = (long)nx_ * ny_;
  Uavg_.assign(NN, 0.0); Ucos_.assign(NN, 0.0); Usin_.assign(NN, 0.0); ravg_.assign(NN, 0.0);
  navg_ = 0; averaging_ = true;
}

PicResult Pic::analyse(double t0, double period, int nper) const {
  PicResult r;
  r.nper = nper;
  // records k with t0 < t_k <= t0 + nper * period; record k closes the step (k-1 -> k)
  size_t k0 = 1;
  while (k0 < hist_.size() && hist_[k0].t <= t0 + 1e-9) ++k0;
  size_t k1 = k0;
  while (k1 < hist_.size() && hist_[k1].t <= t0 + nper * period + 1e-9) ++k1;
  if (k1 <= k0) return r;
  const double span = hist_[k1 - 1].t - hist_[k0 - 1].t;
  double sS = 0, sD = 0, sv = 0, sP = 0, sn = 0, sQ = 0;
  std::complex<double> h1{0, 0};
  RunningStat per;
  double accP = 0; int curp = 0; double accT = 0;
  double Ekf = 0, Ekl = 0; int nf = 0, nl = 0;
  for (size_t k = k0; k < k1; ++k) {
    const PicStep& a = hist_[k - 1]; const PicStep& b = hist_[k];
    double dtk = b.t - a.t, tm = 0.5 * (a.t + b.t);
    double JS = (b.qS - a.qS) - b.absS;      // electrons entering the source metal from the circuit
    double JD = b.absD - (b.qD - a.qD);      // electrons leaving the drain metal to the circuit
    double Jdrive = p_.mode == DriveMode::Sym ? 0.5 * (JS + JD) : (p_.mode == DriveMode::GateDrain ? JS : JD);
    sS += -b.absS; sD += b.absD; sv += b.jvol * dtk;
    sP += 0.5 * (a.V + b.V) * Jdrive;
    if (omega_ > 0) h1 += Jdrive * std::polar(1.0, -omega_ * tm);
    sn += (double)b.np; sQ += b.Qg;
    r.charge_error = std::max(r.charge_error, std::fabs((b.Qg - a.Qg) + b.absS + b.absD));
    int pi = std::min(nper - 1, (int)((tm - t0) / period));
    if (pi != curp) { per.add(accP / accT); accP = 0; accT = 0; curp = pi; }
    accP += 0.5 * (b.absD - b.absS); accT += dtk;
    if (pi == 0) { Ekf += b.Ek; ++nf; }
    if (pi == nper - 1) { Ekl += b.Ek; ++nl; }
  }
  if (accT > 0) per.add(accP / accT);
  r.IdcS = sS / span * kElectronPerFsToAmp;
  r.IdcD = sD / span * kElectronPerFsToAmp;
  r.Idc = 0.5 * (r.IdcS + r.IdcD);
  r.Idc_err = per.stderr_mean() * kElectronPerFsToAmp;
  r.Idc_vol = sv / span * kElectronPerFsToAmp;
  r.Pabs = sP / span * kEvPerFsToWatt;
  if (omega_ > 0) {
    r.I1 = 2.0 * h1 / span * kElectronPerFsToAmp;
    if (p_.Va != 0) r.Y1 = r.I1 / p_.Va;
  }
  r.np_mean = sn / (double)(k1 - k0); r.Qg_mean = sQ / (double)(k1 - k0);
  r.Ek_first = nf ? Ekf / nf : 0; r.Ek_last = nl ? Ekl / nl : 0;
  return r;
}

}  // namespace gd
