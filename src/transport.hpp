// Event-driven semiclassical transport of Dirac electrons in a 2D polygonal
// device: analytic straight flights (field-free) or chord-stepped flights in an
// imposed in-plane field, specular/diffuse wall reflection, isotropic
// momentum-relaxing bulk scattering, absorbing reservoir contacts, and a
// Landauer bias-window current estimator.
//
// Stage B2 additions: the imposed potential may be localised at the neck
// instead of a uniform ramp, it may oscillate in time (V(t) = Va cos 2 pi f t,
// flights then integrate dk/dt = F(x, t) without energy re-imposition), and a
// two-sided equilibrium-injection estimator gives the dc current and the
// first harmonic of the drain current in a time-varying landscape.
#pragma once
#include "core.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include <complex>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace gd {

enum class Fate { Source = 0, Drain = 1, Trapped = 2, Dirac = 3, Leaked = 4, Count = 5 };
const char* fate_name(Fate f);

enum class FieldModel { Uniform = 0, Localised = 1 };

struct TransportParams {
  double EF = EF_from_density(1.4e12);  // eV, local Fermi energy at the contacts
  double T = 300;                       // K
  double V = 0;                         // volts; drain electrostatic potential minus source (static drive)
  bool uniform_field = true;            // apply the imposed potential at all (false: field free)
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
  // from N(E)|f_S(E) - f_D(E)| (bias-window estimator) or, with
  // equilibrium_injection, from N(E) f(E - E_F) in kinetic energy at the
  // injecting contact (two-sided estimator).
  double E_fixed = -1;
  bool equilibrium_injection = false;
  // Angular sampling at the contact: cosine law (physical) or isotropic.
  bool cosine_injection = true;
  // ---- Stage B2: shape of the imposed potential U(x, t) = -e V(t) g(x) ------
  FieldModel field = FieldModel::Uniform;  // g = x/L, or a ramp of length ell_F centred on x_F
  double ell_F = 500;                      // nm
  double x_F = std::numeric_limits<double>::quiet_NaN();  // nm; NaN -> device midpoint
  // ---- Stage B2: ac drive. f_Hz > 0 replaces V by Va cos(2 pi f t). --------
  double f_Hz = 0;
  double Va = 0;                           // volts
  int chords_per_period = 40;              // chord cap v_F / (chords_per_period f)
  // Re-impose |k| from E_tot - U(x) after every chord (exact in a static
  // landscape; forced off when f_Hz > 0). Off: dk/dt = F integrated directly.
  bool reimpose_energy = true;
  // Time-dependent flights only: let an electron whose kinetic energy falls
  // below Ek_min turn around classically (k passes through zero) instead of
  // taking the Dirac fate. Used to bound the bias of the Dirac cut.
  bool dirac_reflect = false;
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
  Histogram time_tx_hist;   // flight time of transmitted particles only
  // batch statistics of the transmission probability
  RunningStat batch_T;
  // ---- Stage B2 diagnostics ------------------------------------------------
  // Per-particle contribution to the drain terminal current at the drive
  // frequency: +exp(-i w t) for an electron arriving at the drain, -exp(-i w t)
  // for one leaving it. sum_h is the total; batch_h_* the batch statistics of
  // the per-batch mean (for the error bar of the first harmonic).
  std::complex<double> sum_h{0, 0};
  RunningStat batch_h_re, batch_h_im;
  double sum_heated = 0;    // particles leaving with Ek_exit > Ek_inj + 2 e |V|max
  double Ek_exit_max = 0;   // eV
  Histogram Ek_exit_hist;   // kinetic energy at the absorbing contact (eV, log bins)
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
  // CDF over energy. Bias window: total energy, Z = int N(E)|f_S - f_D| dE in
  // eV (modes x eV), sign = sign(V). Equilibrium: kinetic energy at the
  // injecting contact, Z = Phi = int N(E) f(E - E_F) dE, sign = +1.
  std::vector<double> E, cdf;
  double Z = 0;
  int sign = 1;
  double sample(Rng& rng) const;
};

class Transport {
 public:
  Transport(const Boundary& b, TransportParams p);
  const TransportParams& params() const { return p_; }
  // Electron potential energy U(x, t) in eV (0 at the source plane, -V(t) at
  // the drain plane); V(t) is the static V or Va cos(2 pi f t).
  double potential(Vec2 x, double t_fs = 0) const;
  double drive(double t_fs) const;            // V(t) in volts
  Vec2 force(Vec2 x, double t_fs) const;      // eV/nm
  double g_profile(double x) const;           // 0 at the source plane, 1 at the drain plane
  // Run n particles over nbatch independent batches; optionally record the
  // first n_record trajectories of batch 0.
  RunStats run(long n, uint64_t seed, int nbatch = 64, int threads = 0,
               std::vector<Trajectory>* record = nullptr, int n_record = 0) const;
  // Landauer current in amperes: I = g (e^2/h) (W/(pi hbar vF)) sign(V) Z <T>.
  // Positive I means net electron flow source -> drain.
  struct IV { double I = 0, I_err = 0, T = 0, T_err = 0, Z = 0; RunStats stats; };
  IV current(long n, uint64_t seed, int nbatch = 64, int threads = 0) const;
  // Two-sided equilibrium estimator (Stage B2): n particles from each contact
  // with the kinetic Fermi distribution, injection phase uniform over one
  // period when f_Hz > 0. I_dc = g e^2/h [Phi_S <T_SD> - Phi_D <T_DS>]; I_1 is
  // the first harmonic of the drain terminal current (I(t) = I_dc + Re(I_1
  // e^{i w t})), Y_1 = I_1 / Va. In a static landscape (f_Hz = 0) I_dc must
  // agree with current().
  struct AC {
    double Idc = 0, Idc_err = 0;
    std::complex<double> I1{0, 0}, Y1{0, 0};
    double I1_err = 0;  // combined standard error of Re and Im
    double T_SD = 0, T_DS = 0, Phi_S = 0, Phi_D = 0;
    RunStats S, D;
  };
  AC ac_current(long n_per_side, uint64_t seed, int nbatch = 64, int threads = 0) const;
  const InjectionTable& injection_table() const { return table_; }
  double length() const { return L_; }

 private:
  Fate trace(Rng& rng, double t_inj, double& path, double& time, long& hits, long& scatters,
             double& Ek_inj, double& Ek_exit, std::vector<Vec2>* rec) const;
  void build_table();
  const Boundary& b_;
  TransportParams p_;
  InjectionTable table_;
  bool has_field_ = false;
  bool reimpose_ = true;
  double omega_ = 0;        // rad/fs
  double period_ = 0;       // fs (0 when static)
  double chord_time_cap_ = std::numeric_limits<double>::infinity();  // nm
  double x_lo_ = 0, x_hi_ = 0;  // planes bounding the localised field region
  double L_ = 0;   // device length (x extent)
  double Vmax_ = 0;  // |V| or Va, for the heating flag
};

}  // namespace gd
