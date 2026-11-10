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
  sum_h += o.sum_h; sum_heated += o.sum_heated; Ek_exit_max = std::max(Ek_exit_max, o.Ek_exit_max);
  if (time_hist.counts.empty()) { time_hist = o.time_hist; path_hist = o.path_hist; hits_hist = o.hits_hist; Ek_exit_hist = o.Ek_exit_hist; time_tx_hist = o.time_tx_hist; }
  else { time_hist.merge(o.time_hist); path_hist.merge(o.path_hist); hits_hist.merge(o.hits_hist); Ek_exit_hist.merge(o.Ek_exit_hist); time_tx_hist.merge(o.time_tx_hist); }
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
  if (p_.f_Hz > 0) {
    omega_ = 2 * kPi * p_.f_Hz * 1e-15;  // rad/fs
    period_ = 1.0 / (p_.f_Hz * 1e-15);
    chord_time_cap_ = kVF * period_ / std::max(1, p_.chords_per_period);
    reimpose_ = false;
    Vmax_ = std::fabs(p_.Va);
    has_field_ = p_.uniform_field && p_.Va != 0;
  } else {
    reimpose_ = p_.reimpose_energy;
    Vmax_ = std::fabs(p_.V);
    has_field_ = p_.uniform_field && p_.V != 0;
  }
  double xF = std::isnan(p_.x_F) ? b_.bbox_min().x + 0.5 * L_ : p_.x_F;
  x_lo_ = xF - 0.5 * p_.ell_F; x_hi_ = xF + 0.5 * p_.ell_F;
  build_table();
}

double Transport::drive(double t_fs) const {
  return p_.f_Hz > 0 ? p_.Va * std::cos(omega_ * t_fs) : p_.V;
}

double Transport::g_profile(double x) const {
  if (p_.field == FieldModel::Uniform) return (x - b_.bbox_min().x) / L_;
  return std::clamp((x - x_lo_) / p_.ell_F, 0.0, 1.0);
}

double Transport::potential(Vec2 x, double t_fs) const {
  if (!p_.uniform_field) return 0.0;
  return -drive(t_fs) * g_profile(x.x);
}

Vec2 Transport::force(Vec2 x, double t_fs) const {
  if (!has_field_) return {0, 0};
  double gp;
  if (p_.field == FieldModel::Uniform) gp = 1.0 / L_;
  else gp = (x.x >= x_lo_ && x.x < x_hi_) ? 1.0 / p_.ell_F : 0.0;
  return {drive(t_fs) * gp, 0};
}

void Transport::build_table() {
  const double kT = kBoltzmann * p_.T;
  const int nE = 4000;
  table_.E.resize(nE + 1); table_.cdf.resize(nE + 1);
  const double W = b_.contact_width(p_.equilibrium_injection ? p_.inject_contact : 0);
  std::vector<double> w(nE + 1);
  double lo, hi;
  if (p_.equilibrium_injection) {
    // weight(Ek) = N(Ek) f(Ek - E_F): kinetic energy at the injecting contact,
    // whose electrochemical potential sits E_F above its own band bottom.
    lo = 0.0; hi = p_.EF + 30 * kT;
    for (int i = 0; i <= nE; ++i) {
      double E = lo + (hi - lo) * i / nE;
      table_.E[i] = E;
      w[i] = modes(E, W) * fermi(E, p_.EF, kT);
    }
    table_.sign = 1;
  } else {
    // weight(E) = N(E_kin,source) |f_S(E) - f_D(E)|, with N(E) = E W/(pi hbar vF)
    // and E_kin at the source equal to the total energy (U_source = 0).
    const double muS = p_.EF, muD = p_.EF - p_.V;
    lo = std::max(0.0, std::min(muS, muD) - 12 * kT); hi = std::max(muS, muD) + 12 * kT;
    for (int i = 0; i <= nE; ++i) {
      double E = lo + (hi - lo) * i / nE;
      table_.E[i] = E;
      double df = fermi(E, muS, kT) - fermi(E, muD, kT);
      w[i] = modes(E, W) * std::fabs(df);
    }
    table_.sign = p_.V >= 0 ? 1 : -1;
  }
  double acc = 0;
  table_.cdf[0] = 0;
  for (int i = 1; i <= nE; ++i) {
    acc += 0.5 * (w[i] + w[i - 1]) * (table_.E[i] - table_.E[i - 1]);
    table_.cdf[i] = acc;
  }
  table_.Z = acc;
  if (acc > 0) for (auto& c : table_.cdf) c /= acc;
}

Fate Transport::trace(Rng& rng, double t_inj, double& path, double& time, long& hits, long& scatters,
                      double& Ek_inj, double& Ek_exit, std::vector<Vec2>* rec) const {
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
  double Etot, Ek;
  if (p_.E_fixed >= 0) { Etot = p_.E_fixed; Ek = Etot - potential(pos, t_inj); }
  else if (p_.equilibrium_injection) { Ek = table_.sample(rng); Etot = Ek + potential(pos, t_inj); }
  else { Etot = table_.sample(rng); Ek = Etot - potential(pos, t_inj); }
  Ek_inj = Ek; Ek_exit = Ek;
  if (Ek <= p_.Ek_min) return Fate::Dirac;
  Vec2 k = dir * (Ek / kHbarVF);
  int last_seg = sidx;
  pos += cs.n * 1e-7;
  double to_scatter = std::isinf(p_.lambda_mfp) ? std::numeric_limits<double>::infinity()
                                                 : rng.exponential(p_.lambda_mfp);
  path = 0; time = 0; hits = 0; scatters = 0;
  if (rec) rec->push_back(pos);
  const double inv_hv = 1.0 / kHbarVF;
  const bool localised = p_.field == FieldModel::Localised;

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
    Vec2 d = knorm > 0 ? k / knorm : Vec2{1, 0};
    double step;          // chord length attempted
    Vec2 cdir = d;        // chord direction
    Vec2 F0{0, 0};        // force used for the chord geometry
    if (!has_field_) {
      step = std::min(to_scatter, (p_.max_time - time) * kVF + 1.0);
    } else {
      // force half a nanometre ahead, so a particle sitting on a field-region
      // plane sees the side it is moving into
      F0 = force(localised ? pos + d * 0.5 : pos, t_inj + time);
      double Fm = F0.norm();
      step = std::min({p_.max_chord, to_scatter, chord_time_cap_});
      if (Fm > 0) {
        double R = kHbarVF * knorm / Fm;  // local radius of curvature (nm)
        step = std::min(step, std::sqrt(8.0 * R * p_.chord_tol));
        if (localised) step = std::min(step, p_.ell_F / 20);
      }
      if (localised && d.x != 0) {
        // end the chord on the next field-region plane ahead
        for (double xp : {x_lo_, x_hi_}) {
          double t = (xp - pos.x) / d.x;
          if (t > 1e-6 && t < step) step = t;
        }
      }
      step = std::max(step, 1e-3);
      Vec2 kmid = k + F0 * (0.5 * step * inv_hv);  // dt = step / vF
      cdir = kmid.unit();
    }
    Hit h;
    bool hit = b_.nearest_hit(pos, cdir, step, last_seg, h);
    double adv = hit ? h.t : step;
    Vec2 pos0 = pos;
    pos = hit ? h.p : pos + cdir * adv;
    path += adv; time += adv / kVF;
    if (has_field_) {
      if (reimpose_) {
        k += F0 * (adv * inv_hv);
        // enforce energy conservation exactly: |k| from E_tot - U(pos)
        double Ekin = Etot - potential(pos, t_inj + time);
        if (Ekin <= p_.Ek_min) { if (rec) rec->push_back(pos); Ek_exit = Ekin; return Fate::Dirac; }
        k = k.unit() * (Ekin * inv_hv);
      } else {
        // non-conservative chord: midpoint force in space and time
        Vec2 Fm = force((pos0 + pos) * 0.5, t_inj + time - 0.5 * adv / kVF);
        k += Fm * (adv * inv_hv);
        double Ekin = kHbarVF * k.norm();
        if (Ekin <= p_.Ek_min && !p_.dirac_reflect) { if (rec) rec->push_back(pos); Ek_exit = Ekin; return Fate::Dirac; }
      }
    }
    if (!std::isinf(to_scatter)) to_scatter -= adv;
    if (rec && (hit || has_field_)) rec->push_back(pos);
    if (hit) {
      const Segment& s = b_.segments()[h.seg];
      if (s.kind == SegKind::Contact) {
        Ek_exit = kHbarVF * k.norm();
        return s.contact == 0 ? Fate::Source : Fate::Drain;
      }
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
    } else if (!has_field_) {
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
  const bool ac = p_.f_Hz > 0;
  const int cid = p_.inject_contact;
  auto worker = [&]() {
    for (;;) {
      int bi = next.fetch_add(1);
      if (bi >= nbatch) return;
      long nb = n / nbatch + (bi < n % nbatch ? 1 : 0);
      Rng rng(seed * 0x9E3779B97F4A7C15ULL + 1000003ULL * (bi + 1));
      RunStats& st = bs[bi];
      st.inject = cid;
      st.time_hist.init(1.0, 1e6, 60, true);
      st.path_hist.init(1.0, 1e6, 60, true);
      st.hits_hist.init(0.0, 400.0, 80, false);
      st.Ek_exit_hist.init(1e-3, 10.0, 80, true);
      st.time_tx_hist.init(1.0, 1e6, 60, true);
      std::complex<double> hsum{0, 0};
      for (long i = 0; i < nb; ++i) {
        double path, time, Ek_inj, Ek_exit; long hits, scat;
        // injection phase: stratified over one period within the batch
        double t_inj = ac ? period_ * (i + rng.uniform()) / nb : 0.0;
        std::vector<Vec2>* rp = nullptr;
        if (record && bi == 0 && (int)recs[bi].size() < n_record) {
          recs[bi].push_back({});
          rp = &recs[bi].back().pts;
        }
        Fate f = trace(rng, t_inj, path, time, hits, scat, Ek_inj, Ek_exit, rp);
        if (rp) recs[bi].back().fate = f;
        st.n++;
        st.fate_count[(int)f] += 1;
        st.sum_path += path; st.sum_time += time; st.sum_hits += hits; st.sum_scatters += scat;
        bool tx = (int)f == (cid == 0 ? (int)Fate::Drain : (int)Fate::Source);
        if (tx) { st.sum_path_tx += path; st.sum_time_tx += time; st.time_tx_hist.add(std::max(time, 1.0)); }
        st.time_hist.add(std::max(time, 1.0)); st.path_hist.add(std::max(path, 1.0)); st.hits_hist.add((double)hits);
        if (f == Fate::Source || f == Fate::Drain) {
          st.Ek_exit_hist.add(std::max(Ek_exit, 1e-3));
          st.Ek_exit_max = std::max(st.Ek_exit_max, Ek_exit);
          if (Ek_exit > Ek_inj + 2 * Vmax_) st.sum_heated += 1;
        }
        if (ac) {
          // drain terminal: +1 for an arrival at the drain, -1 for a departure
          // from it. A drain-injected electron that ends in the Dirac cut (or
          // trapped) is booked as returned at that time: classically it would
          // have turned around there, and the dc estimator already treats it
          // as not transmitted, so the two estimators stay consistent.
          if (cid == 1) hsum -= std::polar(1.0, -omega_ * t_inj);
          if (f == Fate::Drain || (cid == 1 && f != Fate::Source)) hsum += std::polar(1.0, -omega_ * (t_inj + time));
        }
      }
      st.sum_h = hsum;
      if (nb) { st.batch_h_re.add(hsum.real() / nb); st.batch_h_im.add(hsum.imag() / nb); }
    }
  };
  std::vector<std::thread> pool;
  for (int t = 0; t < threads; ++t) pool.emplace_back(worker);
  for (auto& t : pool) t.join();
  RunStats total;
  total.inject = cid;
  for (auto& st : bs) {
    total.merge(st);
    if (st.n) {
      total.batch_T.add(st.transmission());
      total.batch_h_re.add(st.sum_h.real() / st.n);
      total.batch_h_im.add(st.sum_h.imag() / st.n);
    }
  }
  if (record) *record = std::move(recs[0]);
  total.inject = cid;
  return total;
}

Transport::IV Transport::current(long n, uint64_t seed, int nbatch, int threads) const {
  if (p_.equilibrium_injection) throw std::runtime_error("current(): bias-window estimator needs equilibrium_injection = false");
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

Transport::AC Transport::ac_current(long n_per_side, uint64_t seed, int nbatch, int threads) const {
  AC r;
  TransportParams ps = p_, pd = p_;
  ps.equilibrium_injection = pd.equilibrium_injection = true;
  ps.inject_contact = 0; pd.inject_contact = 1;
  Transport ts(b_, ps), td(b_, pd);
  r.S = ts.run(n_per_side, seed, nbatch, threads);
  r.D = td.run(n_per_side, seed + 7919, nbatch, threads);
  r.Phi_S = ts.table_.Z; r.Phi_D = td.table_.Z;
  r.T_SD = r.S.transmission(); r.T_DS = r.D.transmission();
  const double pref = kValleySpin * kE2OverH;  // amperes per (eV x modes)
  r.Idc = pref * (r.Phi_S * r.T_SD - r.Phi_D * r.T_DS);
  r.Idc_err = pref * std::hypot(r.Phi_S * r.S.transmission_err(), r.Phi_D * r.D.transmission_err());
  if (p_.f_Hz > 0 && r.S.n && r.D.n) {
    // I(t) = sum_i q_i delta(t - t_i) over one period, q = pref Phi T_p / N per
    // particle; I_1 = (2 / T_p) sum q_i e^{-i w t_i}
    r.I1 = 2.0 * pref * (r.Phi_S * r.S.sum_h / (double)r.S.n + r.Phi_D * r.D.sum_h / (double)r.D.n);
    double eS = std::hypot(r.S.batch_h_re.stderr_mean(), r.S.batch_h_im.stderr_mean());
    double eD = std::hypot(r.D.batch_h_re.stderr_mean(), r.D.batch_h_im.stderr_mean());
    r.I1_err = 2.0 * pref * std::hypot(r.Phi_S * eS, r.Phi_D * eD);
    if (p_.Va != 0) r.Y1 = r.I1 / p_.Va;
  }
  return r;
}

}  // namespace gd
