// Stage B8: self-consistent particle simulation of the ballistic device.
//
// All electrons of the sheet are represented by sample particles that move
// together. At every step their charge is assigned to a grid, the potential
// is computed from it, and the particles are advanced in that potential.
// Units as in core.hpp: nm, fs, eV, charge e. "U" is the electron potential
// energy in eV (U = -e phi).
//
// Electrostatics: a sheet in a uniform dielectric eps_r at distance d_gate
// above a grounded plane,
//     U(r) = kappa sum_r' dN(r') [ 1/|r - r'| - 1/sqrt(|r - r'|^2 + 4 d^2) ],
// kappa = e^2 / (4 pi eps0 eps_r), dN = excess electrons at a node. The two
// contacts are metal strips in the plane of the sheet; the charge on them is
// solved for at every step so that their potential is the prescribed one.
#pragma once
#include "core.hpp"
#include "fft2d.hpp"
#include "geometry.hpp"
#include "material.hpp"
#include "transport.hpp"
#include <complex>
#include <limits>
#include <memory>
#include <vector>

namespace gd {

constexpr double kCoulomb = 1.439964548;        // e^2 / (4 pi eps0) in eV nm
constexpr double kElectronPerFsToAmp = 1.602176634e-4;
constexpr double kEvPerFsToWatt = 1.602176634e-4;

// ---------------------------------------------------------------------------
// Potential of charges on an nx x ny grid of nodes with spacing h.
// ---------------------------------------------------------------------------
class Electro {
 public:
  Electro();
  ~Electro();
  // filter_cells: width (in cells) of a Gaussian low-pass applied to the
  // kernel in k space; 0 switches it off. d_gate <= 0 means no gate.
  void init(int nx, int ny, double h, double eps_r, double d_gate, double filter_cells);
  // A metal body made of blocks of nodes; each block carries one unknown
  // charge spread evenly over its nodes. Returns the body index.
  int add_metal(const std::vector<std::vector<int>>& blocks);
  void finalize();   // builds and factorises the block potential matrix
  // U = kernel * rho (eV), both arrays of size nx * ny.
  void convolve(const double* rho, double* U) const;
  // rho: excess electrons at the nodes (zero on metal nodes). Uc[b]: electron
  // potential energy of metal body b. Outputs the total U at every node and
  // the electron number induced on each body.
  void solve(const std::vector<double>& rho, const std::vector<double>& Uc,
             std::vector<double>& U, std::vector<double>& q);
  double kernel(int di, int dj) const;   // eV per electron at node offset (di, dj), filter included
  double kappa() const { return kappa_; }
  int nx() const { return nx_; }
  int ny() const { return ny_; }
  int num_blocks() const { return (int)blocks_.size(); }
  // Mean of U over the blocks of body b, minus the prescribed value (max abs
  // over blocks) for the last solve: zero to round-off by construction.
  double last_metal_residual() const { return resid_; }

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
  int nx_ = 0, ny_ = 0, Px_ = 0, Py_ = 0;
  double h_ = 1, kappa_ = 1, d_ = 0;
  std::unique_ptr<FFT2D> fft_;
  std::vector<double> Kre_, Kim_, keff_;
  mutable std::vector<double> wre_, wim_;
  struct Block { std::vector<int> nodes; int body; };
  std::vector<Block> blocks_;
  int nbodies_ = 0;
  std::vector<double> rho_m_, Um_;
  double resid_ = 0;
};

// ---------------------------------------------------------------------------
enum class DriveMode { Sym = 0, GateDrain = 1, GateSource = 2 };

struct PicParams {
  double EF = EF_from_density(1.4e12);   // eV, as in TransportParams
  double T = 300;                        // K
  double lambda_mfp = std::numeric_limits<double>::infinity();   // nm
  double specularity = 1.0;
  // discretisation
  double h = 5;            // nm, grid spacing
  long N = 1000000;        // sample particles in the equilibrium fill
  double dt = 0;           // fs; 0 -> 0.5 h / v_F
  // electrostatics
  bool interaction = true;
  double eps_r = 3.8;
  double d_gate = 30;      // nm; <= 0: no gate
  double contact_len = 200;   // nm, length of the metal strips
  int contact_block = 2;   // nodes per block edge
  double filter = 1.5;     // Gaussian low-pass width in cells
  bool contacts = true;    // false: closed box, contact edges reflect
  // drive: electron potential energies of the contacts, with V(t) in volts,
  //   Sym:        U_S = +V/2, U_D = -V/2      (mirror-symmetric circuit)
  //   GateDrain:  U_S = +V,   U_D = 0         (drain tied to the gate)
  //   GateSource: U_S = 0,    U_D = -V        (source tied to the gate)
  // so that U_S - U_D = V in every mode: positive V drives electrons from
  // source to drain, as in Transport.
  DriveMode mode = DriveMode::Sym;
  double V = 0;            // static drive
  double f_Hz = 0;         // > 0: V(t) = Va cos(2 pi f t)
  double Va = 0;
  double ramp_fs = 500;    // the drive is switched on smoothly over this time
  // imposed potential of Stages B2 to B7, U = -V(t) g(x) (used with the
  // interaction off to reproduce those stages): 0 none, 1 uniform, 2 a ramp
  // of length ell_F centred on x_F
  int imposed = 0;
  double ell_F = 100;
  double x_F = std::numeric_limits<double>::quiet_NaN();
  // tests: a static external potential U1 cos(q (x - x_min)), and an initial
  // density modulation n_eq (1 + amp cos(m pi (x - x_min) / L))
  double ext_U1 = 0, ext_q = 0;
  double init_amp = 0;
  int init_m = 1;
  int threads = 0;
  uint64_t seed = 1;
};

struct PicStep {
  double t = 0;            // fs
  double V = 0;            // drive at t (volts)
  double US = 0, UD = 0;   // contact potential energies at t (eV)
  double absS = 0, absD = 0;   // net electrons taken up by source / drain in the step ending at t
  double qS = 0, qD = 0;   // electrons induced on the source / drain metal at t
  double Qg = 0;           // excess electrons in the sheet at t
  double jvol = 0;         // electron flow source -> drain, volume estimator (electrons / fs)
  double Ek = 0;           // mean kinetic energy per particle (eV)
  long np = 0;
};

struct PicResult {
  int nper = 0;
  double Idc = 0, Idc_err = 0;   // A; mean of the two plane estimators, batch error over periods
  double IdcS = 0, IdcD = 0;     // A; source-plane and drain-plane estimators
  double Idc_vol = 0;            // A; volume estimator
  std::complex<double> I1{0, 0}; // A; first harmonic of the drive-port current, I(t) = Re(I1 e^{i w t})
  std::complex<double> Y1{0, 0}; // S
  double Pabs = 0;               // W; time average of V(t) I_drive(t)
  double Ek_first = 0, Ek_last = 0;   // eV; mean kinetic energy over the first and the last period
  double np_mean = 0, Qg_mean = 0;
  double charge_error = 0;       // max |Qg change + absorbed| over the window (electrons)
};

class Pic {
 public:
  Pic(const Boundary& b, PicParams p);
  ~Pic();
  void step();
  double time() const { return t_; }
  double dt() const { return dt_; }
  double drive(double t_fs) const;
  const PicParams& params() const { return p_; }
  const std::vector<PicStep>& history() const { return hist_; }
  // analysis of the window [t0, t0 + nper * period]
  PicResult analyse(double t0, double period, int nper) const;

  // grid and fields
  int nx() const { return nx_; }
  int ny() const { return ny_; }
  double h() const { return p_.h; }
  Vec2 node(int i, int j) const { return {gx0_ + i * p_.h, gy0_ + j * p_.h}; }
  const std::vector<double>& U() const { return U_; }       // eV
  const std::vector<double>& rho() const { return rho_; }   // excess electrons per node
  const std::vector<double>& background() const { return bg_; }
  // field averages over a window; harmonics with respect to the drive
  void start_average();
  int averaged_steps() const { return navg_; }
  const std::vector<double>& U_mean() const { return Uavg_; }
  const std::vector<double>& U_cos() const { return Ucos_; }
  const std::vector<double>& U_sin() const { return Usin_; }
  const std::vector<double>& rho_mean() const { return ravg_; }

  double weight() const { return w_; }          // electrons per sample particle
  double n_eq() const { return neq_; }          // nm^-2
  double area() const { return area_; }         // nm^2
  double rate(int c) const { return rate_[c]; } // one-way electron rate of contact c (electrons / fs)
  double length() const { return L_; }
  long num_particles() const;
  long leaked() const { return leaked_; }
  // wall-clock seconds spent in the particle push and in the field solve
  double seconds_push() const { return tpush_; }
  double seconds_field() const { return tfield_; }
  Electro& electro() { return es_; }

 private:
  struct P { double x, y, kx, ky; };
  struct Worker;
  void fields();
  int move(P& p, double rem, Rng& rng) const;
  void deposit(std::vector<double>& a, double x, double y, double w) const;
  Vec2 imposed_force(double x, double t) const;

  const Boundary& b_;
  PicParams p_;
  Electro es_;
  double dt_ = 0, t_ = 0, omega_ = 0;
  double L_ = 0, xmin_ = 0, xmax_ = 0, x_lo_ = 0, x_hi_ = 0;
  int nx_ = 0, ny_ = 0;
  double gx0_ = 0, gy0_ = 0;
  std::vector<int> fold_;
  std::vector<char> safe_, out_;
  std::vector<double> bg_, rho_, U_;
  std::vector<double> Uavg_, Ucos_, Usin_, ravg_;
  bool averaging_ = false;
  int navg_ = 0;
  double w_ = 0, neq_ = 0, area_ = 0;
  double rate_[2] = {0, 0};
  InjectionTable table_[2];
  std::vector<std::unique_ptr<Worker>> workers_;
  std::vector<PicStep> hist_;
  std::vector<double> q_;
  long leaked_ = 0;
  double tpush_ = 0, tfield_ = 0;
};

}  // namespace gd
