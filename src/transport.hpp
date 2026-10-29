// Event-driven semiclassical transport of Dirac electrons in a 2D polygonal
// device: analytic straight flights (field-free) or chord-stepped flights in a
// uniform in-plane field, specular/diffuse wall reflection, isotropic
// momentum-relaxing bulk scattering, absorbing reservoir contacts, and a
// Landauer bias-window current estimator.
#pragma once
#include "core.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace gd {

enum class Fate { Source = 0, Drain = 1, Trapped = 2, Dirac = 3, Leaked = 4, Count = 5 };
const char* fate_name(Fate f);

struct TransportParams {
  double EF = EF_from_density(1.4e12);  // eV, local Fermi energy at the contacts
  double T = 300;                       // K
  double V = 0;                         // volts; drain electrostatic potential minus source
  bool uniform_field = true;            // apply F = (V/L, 0) eV/nm over the whole domain
  double lambda_mfp = std::numeric_limits<double>::infinity();  // nm; isotropic momentum relaxation
  double specularity = 1.0;             // probability of specular reflection at walls
  double max_time = 5e6;                // fs; residence cutoff -> Trapped (5 ns)
  long max_events = 2000000;            // wall hits + scatterings cutoff -> Trapped
  double chord_tol = 0.05;              // nm sagitta tolerance of field flights
  double max_chord = 40;                // nm
  double Ek_min = 2e-3;                 // eV; kinetic energy below this -> Dirac fate
  int inject_contact = 0;               // 0 = source, 1 = drain
  // Energy sampling. When E_fixed >= 0 the injection is monoenergetic at that
  // total energy (used by field-free tests); otherwise energies are drawn
  // from N(E)|f_S(E) - f_D(E)| (bias-window estimator).
  double E_fixed = -1;
  // Angular sampling at the contact: cosine law (physical) or isotropic.
  bool cosine_injection = true;
};

struct Histogram {
  double lo = 0, hi = 1;
  bool log_bins = false;
  std::vector<double> counts;
  void init(double lo_, double hi_, int nb, bool log_) {
    lo = lo_; hi = hi_; log_bins = log_; counts.assign(nb + 2, 0.0);
  }
  void add(double v, double w = 1) {
    int nb = (int)counts.size() - 2;
    double f = log_bins ? (std::log(v) - std::log(lo)) / (std::log(hi) - std::log(lo)) : (v - lo) / (hi - lo);
    int b = f < 0 ? 0 : (f >= 1 ? nb + 1 : 1 + (int)(f * nb));
    counts[b] += w;
  }
  void merge(const Histogram& o) { for (size_t i = 0; i < counts.size(); ++i) counts[i] += o.counts[i]; }
};

struct RunStats {
  long n = 0;
  int inject = 0;  // contact the particles were injected from
  double fate_count[(int)Fate::Count] = {0, 0, 0, 0, 0};
  double sum_path = 0, sum_time = 0, sum_hits = 0, sum_scatters = 0;
  double sum_path_tx = 0, sum_time_tx = 0;  // transmitted particles only
  Histogram time_hist, path_hist, hits_hist;
  // batch statistics of the transmission probability
  RunningStat batch_T;
  // probability of reaching the contact opposite to the injecting one
  double transmission() const { return n ? fate_count[inject == 0 ? (int)Fate::Drain : (int)Fate::Source] / n : 0.0; }
  double reflection() const { return n ? fate_count[inject == 0 ? (int)Fate::Source : (int)Fate::Drain] / n : 0.0; }
  double transmission_err() const { return batch_T.stderr_mean(); }
  double fraction(Fate f) const { return n ? fate_count[(int)f] / n : 0.0; }
  void merge(const RunStats& o);
};

struct Trajectory {
  std::vector<Vec2> pts;
  Fate fate = Fate::Trapped;
};

struct InjectionTable {
  // CDF over total energy for the bias-window estimator; Z is the integral
  // of N(E)|f_S - f_D| dE in eV (dimensionless modes x eV), sign = sign(V).
  std::vector<double> E, cdf;
  double Z = 0;
  int sign = 1;
  double sample(Rng& rng) const;
};

class Transport {
 public:
  Transport(const Boundary& b, TransportParams p);
  const TransportParams& params() const { return p_; }
  // Electron potential energy U(x) in eV (0 at source plane, -V at drain plane).
  double potential(Vec2 x) const;
  // Run n particles over nbatch independent batches; optionally record the
  // first n_record trajectories of batch 0.
  RunStats run(long n, uint64_t seed, int nbatch = 64, int threads = 0,
               std::vector<Trajectory>* record = nullptr, int n_record = 0) const;
  // Landauer current in amperes: I = g (e^2/h) (W/(pi hbar vF)) sign(V) Z <T>.
  // Positive I means net electron flow source -> drain.
  struct IV { double I = 0, I_err = 0, T = 0, T_err = 0, Z = 0; RunStats stats; };
  IV current(long n, uint64_t seed, int nbatch = 64, int threads = 0) const;
  const InjectionTable& injection_table() const { return table_; }

 private:
  Fate trace(Rng& rng, double& path, double& time, long& hits, long& scatters,
             std::vector<Vec2>* rec) const;
  void build_table();
  const Boundary& b_;
  TransportParams p_;
  InjectionTable table_;
  Vec2 force_;     // eV/nm
  double force_mag_ = 0;
  double L_ = 0;   // device length (x extent)
};

}  // namespace gd
