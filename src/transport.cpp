#include "transport.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <stdexcept>
#include <thread>

namespace gd {

const char* fate_name(Fate f) {
  switch (f) {
    case Fate::Source: return "source";
    case Fate::Drain: return "drain";
    case Fate::Trapped: return "trapped";
    case Fate::Dirac: return "dirac";
    case Fate::Leaked: return "leaked";
    default: return "?";
  }
}

void RunStats::merge(const RunStats& o) {
  n += o.n;
  for (int i = 0; i < (int)Fate::Count; ++i) fate_count[i] += o.fate_count[i];
  sum_path += o.sum_path; sum_time += o.sum_time; sum_hits += o.sum_hits; sum_scatters += o.sum_scatters;
  sum_path_tx += o.sum_path_tx; sum_time_tx += o.sum_time_tx;
  if (time_hist.counts.empty()) { time_hist = o.time_hist; path_hist = o.path_hist; hits_hist = o.hits_hist; }
  else { time_hist.merge(o.time_hist); path_hist.merge(o.path_hist); hits_hist.merge(o.hits_hist); }
}

double InjectionTable::sample(Rng& rng) const {
  double u = rng.uniform();
  auto it = std::lower_bound(cdf.begin(), cdf.end(), u);
  size_t i = std::min<size_t>(std::max<size_t>(1, it - cdf.begin()), cdf.size() - 1);
  double f = (u - cdf[i - 1]) / std::max(1e-300, cdf[i] - cdf[i - 1]);
  return E[i - 1] + f * (E[i] - E[i - 1]);
}

Transport::Transport(const Boundary& b, TransportParams p) : b_(b), p_(p) {
  L_ = b_.bbox_max().x - b_.bbox_min().x;
  force_ = (p_.uniform_field && p_.V != 0) ? Vec2{p_.V / L_, 0} : Vec2{0, 0};
  force_mag_ = force_.norm();
  build_table();
}

double Transport::potential(Vec2 x) const {
  if (!p_.uniform_field) return 0.0;
  return -p_.V * (x.x - b_.bbox_min().x) / L_;
}

void Transport::build_table() {
  // weight(E) = N(E_kin,source) |f_S(E) - f_D(E)|, with N(E) = E W/(pi hbar vF)
  // and E_kin at the source equal to the total energy (U_source = 0).
  const double kT = kBoltzmann * p_.T;
  const double muS = p_.EF, muD = p_.EF - p_.V;
  double lo = std::max(0.0, std::min(muS, muD) - 12 * kT), hi = std::max(muS, muD) + 12 * kT;
  const int nE = 4000;
  table_.E.resize(nE + 1); table_.cdf.resize(nE + 1);
  const double W = b_.contact_width(0);
  double acc = 0;
  std::vector<double> w(nE + 1);
  for (int i = 0; i <= nE; ++i) {
    double E = lo + (hi - lo) * i / nE;
    table_.E[i] = E;
    double df = fermi(E, muS, kT) - fermi(E, muD, kT);
    w[i] = modes(E, W) * std::fabs(df);
  }
  table_.cdf[0] = 0;
  for (int i = 1; i <= nE; ++i) {
    acc += 0.5 * (w[i] + w[i - 1]) * (table_.E[i] - table_.E[i - 1]);
    table_.cdf[i] = acc;
  }
  table_.Z = acc;
  table_.sign = p_.V >= 0 ? 1 : -1;
  if (acc > 0) for (auto& c : table_.cdf) c /= acc;
}

Fate Transport::trace(Rng& rng, double& path, double& time, long& hits, long& scatters,
                      std::vector<Vec2>* rec) const {
  // ---- injection -------------------------------------------------------
  const int cid = p_.inject_contact;
  const auto& csegs = b_.contact_segments(cid);
  // pick a contact segment with probability proportional to its length
  double Wc = b_.contact_width(cid), u = rng.uniform() * Wc;
  int sidx = csegs.back();
  for (int si : csegs) { double l = b_.segments()[si].len; if (u <= l) { sidx = si; break; } u -= l; }
  const Segment& cs = b_.segments()[sidx];
  Vec2 pos = cs.a + cs.d * (rng.uniform() * cs.len);
  double th = p_.cosine_injection ? rng.cosine_angle() : rng.uniform(-kPi / 2, kPi / 2);
  Vec2 dir = cs.n * std::cos(th) + cs.d * std::sin(th);
  double Etot = p_.E_fixed >= 0 ? p_.E_fixed : table_.sample(rng);
  double Ek = Etot - potential(pos);
  if (Ek <= p_.Ek_min) return Fate::Dirac;
  Vec2 k = dir * (Ek / kHbarVF);
  int last_seg = sidx;
  pos += cs.n * 1e-7;
  double to_scatter = std::isinf(p_.lambda_mfp) ? std::numeric_limits<double>::infinity()
                                                 : rng.exponential(p_.lambda_mfp);
  path = 0; time = 0; hits = 0; scatters = 0;
  if (rec) rec->push_back(pos);
  const double inv_hv = 1.0 / kHbarVF;

  auto reflect = [&](const Segment& s) {
    if (rng.uniform() < p_.specularity) {
      k = k - s.n * (2.0 * dot(k, s.n));
    } else {
      double a = rng.cosine_angle();
      Vec2 nd = s.n * std::cos(a) + s.d * std::sin(a);
      k = nd * k.norm();
    }
  };

  // ---- flight loop -----------------------------------------------------
  while (true) {
    if (time > p_.max_time || hits + scatters > p_.max_events)
      return b_.contains(pos) ? Fate::Trapped : Fate::Leaked;
    double knorm = k.norm();
    Vec2 d = k / knorm;
    double step;          // chord length attempted
    Vec2 cdir = d;        // chord direction
    if (force_mag_ == 0) {
      step = std::min(to_scatter, (p_.max_time - time) * kVF + 1.0);
    } else {
      double R = kHbarVF * knorm / force_mag_;  // local radius of curvature (nm)
      step = std::min({p_.max_chord, std::sqrt(8.0 * R * p_.chord_tol), to_scatter});
      step = std::max(step, 1e-3);
      Vec2 kmid = k + force_ * (0.5 * step * inv_hv);  // dt = step / vF
      cdir = kmid.unit();
    }
    Hit h;
    bool hit = b_.nearest_hit(pos, cdir, step, last_seg, h);
    double adv = hit ? h.t : step;
    pos = hit ? h.p : pos + cdir * adv;
    path += adv; time += adv / kVF;
    if (force_mag_ != 0) {
      k += force_ * (adv * inv_hv);
      // enforce energy conservation exactly: |k| from E_tot - U(pos)
      double Ekin = Etot - potential(pos);
      if (Ekin <= p_.Ek_min) { if (rec) rec->push_back(pos); return Fate::Dirac; }
      k = k.unit() * (Ekin * inv_hv);
    }
    if (!std::isinf(to_scatter)) to_scatter -= adv;
    if (rec && (hit || force_mag_ != 0)) rec->push_back(pos);
    if (hit) {
      const Segment& s = b_.segments()[h.seg];
      if (s.kind == SegKind::Contact) return s.contact == 0 ? Fate::Source : Fate::Drain;
      ++hits;
      reflect(s);
      last_seg = h.seg;
      pos += s.n * 1e-7;
      // a reflected direction that still points into the wall (possible after
      // a diffuse draw at a corner) is harmless: the next query skips this
      // segment and the neighbour will catch it.
    } else if (to_scatter <= 0) {
      ++scatters;
      double a = rng.uniform(0, 2 * kPi);
      k = from_angle(a) * k.norm();
      to_scatter = rng.exponential(p_.lambda_mfp);
      last_seg = -1;
    } else if (force_mag_ == 0) {
      // ran out of time budget without any event
      return b_.contains(pos) ? Fate::Trapped : Fate::Leaked;
    } else {
      last_seg = -1;  // moved away from the last wall along a curved chord
    }
  }
}

RunStats Transport::run(long n, uint64_t seed, int nbatch, int threads,
                        std::vector<Trajectory>* record, int n_record) const {
  if (threads <= 0) threads = std::max(1u, std::thread::hardware_concurrency());
  nbatch = std::max(1, nbatch);
  std::vector<RunStats> bs(nbatch);
  std::vector<std::vector<Trajectory>> recs(nbatch);
  std::atomic<int> next{0};
  auto worker = [&]() {
    for (;;) {
      int bi = next.fetch_add(1);
      if (bi >= nbatch) return;
      long nb = n / nbatch + (bi < n % nbatch ? 1 : 0);
      Rng rng(seed * 0x9E3779B97F4A7C15ULL + 1000003ULL * (bi + 1));
      RunStats& st = bs[bi];
      st.inject = p_.inject_contact;
      st.time_hist.init(1.0, 1e6, 60, true);
      st.path_hist.init(1.0, 1e6, 60, true);
      st.hits_hist.init(0.0, 400.0, 80, false);
      for (long i = 0; i < nb; ++i) {
        double path, time; long hits, scat;
        std::vector<Vec2>* rp = nullptr;
        if (record && bi == 0 && (int)recs[bi].size() < n_record) {
          recs[bi].push_back({});
          rp = &recs[bi].back().pts;
        }
        Fate f = trace(rng, path, time, hits, scat, rp);
        if (rp) recs[bi].back().fate = f;
        st.n++;
        st.fate_count[(int)f] += 1;
        st.sum_path += path; st.sum_time += time; st.sum_hits += hits; st.sum_scatters += scat;
        if ((int)f == (p_.inject_contact == 0 ? (int)Fate::Drain : (int)Fate::Source)) { st.sum_path_tx += path; st.sum_time_tx += time; }
        st.time_hist.add(std::max(time, 1.0)); st.path_hist.add(std::max(path, 1.0)); st.hits_hist.add((double)hits);
      }
    }
  };
  std::vector<std::thread> pool;
  for (int t = 0; t < threads; ++t) pool.emplace_back(worker);
  for (auto& t : pool) t.join();
  RunStats total;
  total.inject = p_.inject_contact;
  for (auto& st : bs) {
    total.merge(st);
    if (st.n) total.batch_T.add(st.transmission());
  }
  if (record) *record = std::move(recs[0]);
  total.inject = p_.inject_contact;
  return total;
}

Transport::IV Transport::current(long n, uint64_t seed, int nbatch, int threads) const {
  IV iv;
  iv.stats = run(n, seed, nbatch, threads);
  iv.T = iv.stats.transmission();
  iv.T_err = iv.stats.transmission_err();
  iv.Z = table_.Z;
  double pref = kValleySpin * kE2OverH * table_.Z * table_.sign;  // amperes per unit T
  iv.I = pref * iv.T;
  iv.I_err = std::fabs(pref) * iv.T_err;
  return iv;
}

}  // namespace gd
