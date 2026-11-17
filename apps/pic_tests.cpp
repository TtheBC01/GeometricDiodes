// Acceptance ladder of Stage B8 (plans/ballistic_selfconsistent.md, section 4):
// the self-consistent particle solver of src/pic.*.
//   usage: gd_pic_tests [T22,T23,...] [--N n] [--h nm] [--filter cells]
// T31 (Stage B9, plans/ballistic_edges.md section 4) tests the rough-wall
// rule; it is not in the default list.
// T32 (Stage B10, plans/ballistic_contacts.md section 5) tests contacts that
// reflect; it is not in the default list either.
// T30 (convergence of the decision number) is run with gd_bpic and
// tools/pic_convergence.sh, not here.
#include "generators.hpp"
#include "pic.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace gd;

static int g_fail = 0, g_pass = 0;
static void verdict(const char* name, bool ok, const std::string& detail) {
  std::printf("%-5s %s  %s\n", name, ok ? "PASS" : "FAIL", detail.c_str());
  std::fflush(stdout);
  (ok ? g_pass : g_fail)++;
}
static std::string fmt(const char* f, double a = 0, double b = 0, double c = 0, double d = 0, double e = 0, double g = 0, double h = 0, double i = 0) {
  char buf[640]; std::snprintf(buf, sizeof buf, f, a, b, c, d, e, g, h, i); return buf;
}
static double now() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

static Boundary g3() {
  FunnelParams f; f.Wc = 500; f.Lc = 100; f.alpha_deg = 30; f.Wn = 100; f.Wd = 500; f.Ld = 100;
  return make_funnel(f);
}
static const double kG3xF = 100 + 0.5 * (500 - 100) / std::tan(deg2rad(30.0));

// complete elliptic integral K(k) by the arithmetic-geometric mean
static double ellipK(double k) {
  double a = 1, b = std::sqrt(1 - k * k);
  for (int i = 0; i < 40; ++i) { double an = 0.5 * (a + b); b = std::sqrt(a * b); a = an; }
  return kPi / (2 * a);
}

// d n / d mu of the Dirac gas at temperature T (per eV nm^2)
static double dos_T(double EF, double T) {
  double kT = kBoltzmann * T;
  return 2.0 / (kPi * kHbarVF * kHbarVF) * kT * std::log(1 + std::exp(EF / kT));
}

static PicResult run_window(Pic& pic, double settle_fs, double period, int nper) {
  while (pic.time() < settle_fs - 1e-9) pic.step();
  double t0 = pic.time();
  while (pic.time() < t0 + nper * period - 1e-9) pic.step();
  return pic.analyse(t0, period, nper);
}

int main(int argc, char** argv) {
  std::string which = "T22,T23,T24,T25,T26,T27,T28,T29";
  long N = 2000000; double h = 5, filter = 1.5;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--N" && i + 1 < argc) N = std::atol(argv[++i]);
    else if (a == "--h" && i + 1 < argc) h = std::atof(argv[++i]);
    else if (a == "--filter" && i + 1 < argc) filter = std::atof(argv[++i]);
    else which = a;
  }
  auto want = [&](const char* t) { return ("," + which + ",").find(std::string(",") + t + ",") != std::string::npos; };
  std::printf("Stage B8 ladder: N = %ld, h = %g nm, filter = %g cells, FFT backend %s\n", N, h, filter, FFT2D::backend());
  const double EF = EF_from_density(1.4e12);

  // -------------------------------------------------------------------------
  if (want("T22")) {
    // (a) the two FFT backends agree
    {
      const int lgx = 7, lgy = 5, nxx = 1 << lgx, nyy = 1 << lgy;
      std::vector<double> re(nxx * nyy), im(nxx * nyy), re2, im2, re0, im0;
      Rng rng(5);
      for (auto& v : re) v = rng.uniform(-1, 1);
      for (auto& v : im) v = rng.uniform(-1, 1);
      re2 = re; im2 = im; re0 = re; im0 = im;
      FFT2D f(lgx, lgy);
      f.forward(re.data(), im.data());
      FFT2D::force_plain(true); f.forward(re2.data(), im2.data()); FFT2D::force_plain(false);
      double e = 0, e2 = 0;
      for (int i = 0; i < nxx * nyy; ++i) e = std::max({e, std::fabs(re[i] - re2[i]), std::fabs(im[i] - im2[i])});
      f.inverse(re.data(), im.data());
      for (int i = 0; i < nxx * nyy; ++i) e2 = std::max({e2, std::fabs(re[i] / (nxx * nyy) - re0[i]), std::fabs(im[i] / (nxx * nyy) - im0[i])});
      verdict("T22a", e < 1e-9 && e2 < 1e-12, fmt("FFT backends differ by at most %.1e on a 128 x 32 random array; forward then inverse returns the input to %.1e", e, e2));
    }
    // (b) a point charge and its image
    {
      Electro es; const int nx = 96, ny = 64; const double hh = 5, d = 30;
      es.init(nx, ny, hh, 3.8, d, 0);
      std::vector<double> rho(nx * ny, 0.0), U(nx * ny);
      rho[20 * nx + 30] = 1;
      es.convolve(rho.data(), U.data());
      double e = 0;
      for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i) {
          if (i == 30 && j == 20) continue;
          double r = hh * std::hypot(i - 30, j - 20);
          double ex = es.kappa() * (1 / r - 1 / std::sqrt(r * r + 4 * d * d));
          e = std::max(e, std::fabs(U[j * nx + i] - ex) / ex);
        }
      verdict("T22b", e < 1e-8, fmt("point charge over the gate: largest relative error %.1e over the grid", e));
    }
    // (c) a uniformly charged disc over the gate, potential at its centre
    {
      Electro es; const int nx = 200, ny = 200; const double hh = 5, d = 30, R = 400;
      es.init(nx, ny, hh, 3.8, d, 0);
      std::vector<double> rho(nx * ny, 0.0), U(nx * ny);
      const double sigma = 0.01;   // electrons per nm^2
      long nin = 0;
      for (int j = 0; j < ny; ++j)
        for (int i = 0; i < nx; ++i) if (std::hypot(i - 100, j - 100) * hh <= R) { rho[j * nx + i] = sigma * hh * hh; ++nin; }
      es.convolve(rho.data(), U.data());
      double Re = std::sqrt(nin * hh * hh / kPi);   // radius of the disc with the area of the lattice disc
      double ex = es.kappa() * sigma * 2 * kPi * (Re - std::sqrt(Re * Re + 4 * d * d) + 2 * d);
      double plate = es.kappa() * sigma * 4 * kPi * d;
      double e = std::fabs(U[100 * nx + 100] / ex - 1);
      verdict("T22c", e < 1e-3, fmt("charged disc over the gate: U(0) = %.5f eV, closed form %.5f (infinite sheet %.5f), error %.1e", U[100 * nx + 100], ex, plate, e));
    }
  }

  // -------------------------------------------------------------------------
  if (want("T23")) {
    // (a) a square plate over the gate
    for (double hh : {10.0, 5.0}) {
      const double W = 400, d = 10, pad = 60;
      int n = (int)((W + 2 * pad) / hh);
      Electro es; es.init(n, n, hh, 3.8, d, 0);
      std::vector<std::vector<int>> blocks;
      int i0 = (int)(pad / hh), m = (int)(W / hh);
      for (int j = 0; j < m; ++j) for (int i = 0; i < m; ++i) blocks.push_back({(j + i0) * n + (i + i0)});
      es.add_metal(blocks); es.finalize();
      std::vector<double> rho(n * n, 0.0), U, q;
      es.solve(rho, {1.0}, U, q);
      double ideal = W * W / (4 * kPi * es.kappa() * d);
      double palmer = ideal * std::pow(1 + 2 * d / (kPi * W) * (1 + std::log(kPi * W / d)), 2);
      bool last = hh == 5.0;
      std::string s = fmt("plate 400 x 400 nm, gate at 10 nm, h = %g nm: charge %.2f e/eV; ideal %.2f; with Palmer's fringing %.2f (ratio %.3f); metal residual %.1e eV",
                          hh, q[0], ideal, palmer, q[0] / palmer, es.last_metal_residual());
      if (last) verdict("T23a", std::fabs(q[0] / palmer - 1) < 0.05, s); else std::printf("      %s\n", s.c_str());
    }
    // (b) two coplanar strips, no gate: charge per unit length from two lengths,
    // which removes the contribution of the ends
    for (double hh : {10.0, 5.0}) {
      const double w = 100, s = 100, pad = 60;
      double qlen[2] = {0, 0}, sum = 0;
      for (int c = 0; c < 2; ++c) {
        double len = c == 0 ? 800 : 1600;
        int nx = (int)((2 * w + s + 2 * pad) / hh), ny = (int)((len + 2 * pad) / hh);
        Electro es; es.init(nx, ny, hh, 3.8, 0, 0);
        std::vector<std::vector<int>> A, B;
        int i0 = (int)(pad / hh), mw = (int)(w / hh), ms = (int)(s / hh), ml = (int)(len / hh), na = hh < 10 ? 4 : 1;
        for (int j = 0; j < ml; j += na)
          for (int i = 0; i < mw; ++i) {
            std::vector<int> a, bq;
            for (int jj = j; jj < std::min(ml, j + na); ++jj) { a.push_back((jj + i0) * nx + (i0 + i)); bq.push_back((jj + i0) * nx + (i0 + mw + ms + i)); }
            A.push_back(a); B.push_back(bq);
          }
        es.add_metal(A); es.add_metal(B); es.finalize();
        std::vector<double> rho(nx * ny, 0.0), U, q;
        es.solve(rho, {0.5, -0.5}, U, q);
        qlen[c] = q[0]; sum = std::max(sum, std::fabs(q[0] + q[1]) / std::fabs(q[0]));
      }
      double k = s / (s + 2 * w), kp = std::sqrt(1 - k * k);
      double per = (qlen[1] - qlen[0]) / 800, ex = 1 / (4 * kPi * kCoulomb / 3.8) * ellipK(kp) / ellipK(k);
      bool last = hh == 5.0;
      std::string str = fmt("coplanar strips 100 nm wide, 100 nm apart, h = %g nm: %.4f e/eV per nm from lengths 800 and 1600 nm; infinite-strip formula %.4f (ratio %.3f)",
                            hh, per, ex, per / ex);
      if (last) verdict("T23b", std::fabs(per / ex - 1) < 0.05 && sum < 1e-6, str); else std::printf("      %s\n", str.c_str());
    }
  }

  // -------------------------------------------------------------------------
  if (want("T24")) {
    // Static screening of a weak sinusoidal potential U1 cos(q x), measured in
    // the central 400 x 300 nm of a closed strip 800 x 900 nm (the formulas are
    // for an infinite sheet). Three parts:
    //  (a) interaction off: the density response of the particles alone, chi = nu;
    //  (b) interaction on at weak coupling (eps_r = 40, nu G of order one), where
    //      U_tot / U_ext = 1 / (1 + nu G) is a well-conditioned test of the loop;
    //  (c) interaction on at the production coupling (eps_r = 3.8) and the
    //      production particle density. There U_tot is the small difference of
    //      U_ext and U_ind (a 14- to 30-fold cancellation), so the kernel
    //      (U_ind / dn = G) and U_ind / U_ext are tested tightly and U_tot /
    //      U_ext loosely, in an early and in a late window: collisions between
    //      sample particles slowly relax the Fermi distribution and raise chi.
    Boundary b = make_straight_channel(800, 900);
    const double nu = dos_T(EF, 300);
    struct Proj { double Uind, dn; };
    auto project = [&](const Pic& pic, double q) {
      double sU = 0, sR = 0, sc = 0;
      for (int j = 0; j < pic.ny(); ++j)
        for (int i = 0; i < pic.nx(); ++i) {
          Vec2 r = pic.node(i, j);
          if (std::fabs(r.y) > 150 || r.x < 200 || r.x >= 600) continue;   // away from the edges and the ends
          double c = std::cos(q * r.x);
          sU += pic.U_mean()[j * pic.nx() + i] / pic.averaged_steps() * c;
          sR += pic.rho_mean()[j * pic.nx() + i] / pic.averaged_steps() / (pic.h() * pic.h()) * c;
          sc += c * c;
        }
      return Proj{sU / sc, sR / sc};
    };
    auto window = [&](Pic& pic, double t0, double t1, double q) {
      while (pic.time() < t0) pic.step();
      pic.start_average();
      while (pic.time() < t1) pic.step();
      return project(pic, q);
    };
    auto Gof = [&](double q, double d, double eps, double hh) {
      double s = filter * hh;
      return 2 * kPi * kCoulomb / eps * (1 - std::exp(-2 * q * d)) / q * std::exp(-0.5 * q * q * s * s);
    };
    bool ok = true; std::string det; char buf[400];
    for (double lam : {200.0, 400.0}) {   // (a)
      PicParams p; p.EF = EF; p.h = h; p.N = 3 * N; p.filter = filter; p.contacts = false; p.interaction = false;
      p.lambda_mfp = 100; p.ext_U1 = 0.01; p.ext_q = 2 * kPi / lam; p.seed = 11;
      Pic pic(b, p);
      double t0 = now();
      Proj w = window(pic, 3000, 5000, p.ext_q);
      double chi = w.dn / (-nu * p.ext_U1);
      ok = ok && std::fabs(chi - 1) < 0.03;
      std::snprintf(buf, sizeof buf, "\n      (a) interaction off, wavelength %3.0f nm: dn / (-nu U_ext) = %.4f (target 3 %%)  [%.0f s]", lam, chi, now() - t0);
      det += buf;
    }
    for (double lam : {200.0, 400.0}) {   // (b)
      PicParams p; p.EF = EF; p.h = h; p.N = 3 * N; p.filter = filter; p.contacts = false; p.d_gate = 30; p.eps_r = 40;
      p.lambda_mfp = 100; p.ext_U1 = 0.01; p.ext_q = 2 * kPi / lam; p.seed = 11;
      Pic pic(b, p);
      double t0 = now();
      Proj w = window(pic, 3000, 5000, p.ext_q);
      double G = Gof(p.ext_q, 30, 40, pic.h()), ex = 1 / (1 + nu * G), r = (p.ext_U1 + w.Uind) / p.ext_U1, g = (w.Uind / w.dn) / G;
      ok = ok && std::fabs(r / ex - 1) < 0.03 && std::fabs(g - 1) < 0.01;
      std::snprintf(buf, sizeof buf, "\n      (b) eps_r 40, gate 30 nm, wavelength %3.0f nm (nu G = %.2f): U_tot/U_ext = %.4f, expected %.4f (ratio %.3f, target 3 %%); "
                    "U_ind / (G dn) = %.4f (target 1 %%)  [%.0f s]", lam, nu * G, r, ex, r / ex, g, now() - t0);
      det += buf;
    }
    for (double d : {30.0, 300.0})        // (c)
      for (double lam : {200.0, 400.0}) {
        PicParams p; p.EF = EF; p.h = h; p.N = 7 * N; p.filter = filter; p.d_gate = d; p.contacts = false;
        p.lambda_mfp = 100; p.ext_U1 = 0.03; p.ext_q = 2 * kPi / lam; p.seed = 11;
        Pic pic(b, p);
        double t0 = now();
        Proj e = window(pic, 1000, 2500, p.ext_q), l = window(pic, 3500, 5000, p.ext_q);
        double G = Gof(p.ext_q, d, 3.8, pic.h()), ex = 1 / (1 + nu * G);
        double re = (p.ext_U1 + e.Uind) / p.ext_U1, rl = (p.ext_U1 + l.Uind) / p.ext_U1, g = (e.Uind / e.dn) / G;
        double ind = (e.Uind / p.ext_U1) / (ex - 1);
        ok = ok && std::fabs(g - 1) < 0.01 && std::fabs(ind - 1) < 0.01 && std::fabs(re / ex - 1) < 0.10;
        std::snprintf(buf, sizeof buf, "\n      (c) gate %3.0f nm, wavelength %3.0f nm (nu G = %.1f): U_ind / (G dn) = %.4f (target 1 %%); U_ind/U_ext ratio to theory %.4f (target 1 %%);"
                      "\n          U_tot/U_ext = %.4f at 1.0-2.5 ps, %.4f at 3.5-5.0 ps, expected %.4f (ratios %.3f, %.3f; target 10 %% early)  [%.0f s]",
                      d, lam, nu * G, g, ind, re, rl, ex, re / ex, rl / ex, now() - t0);
        det += buf;
      }
    verdict("T24", ok, "static screening: particle response, kernel, and U_tot / U_ext against 1 / (1 + nu G(q))" + det);
  }

  // -------------------------------------------------------------------------
  if (want("T25")) {
    // Plasma-wave speed of the sheet: standing wave cos(m pi x / L) in a closed
    // strip 900 nm wide, measured in the central 300 nm over the first few
    // periods. (Integrated over a strip with hard edges the wave is slower: the
    // edges carry edge plasmons, ratio about 0.91 at 300 nm width.)
    Boundary b = make_straight_channel(800, 900);
    bool ok = true; std::string det;
    struct Case { double d; int m; };
    for (Case cs : {Case{30, 4}, Case{30, 8}, Case{300, 8}}) {
      PicParams p; p.EF = EF; p.h = h; p.N = 3 * N; p.filter = filter; p.d_gate = cs.d; p.contacts = false;
      p.init_amp = 0.05; p.init_m = cs.m; p.seed = 13;
      Pic pic(b, p);
      double q = cs.m * kPi / 800, hh = pic.h();
      std::vector<double> ts, as;
      auto amp = [&]() {
        double s = 0;
        for (int j = 0; j < pic.ny(); ++j)
          for (int i = 0; i < pic.nx(); ++i) {
            Vec2 r = pic.node(i, j);
            if (std::fabs(r.y) < 150) s += pic.rho()[j * pic.nx() + i] * std::cos(q * r.x);
          }
        return s;
      };
      ts.push_back(0); as.push_back(amp());
      std::vector<double> zc;
      while (pic.time() < 1500 && zc.size() < 7) {
        pic.step(); ts.push_back(pic.time()); as.push_back(amp());
        size_t k = as.size() - 1;
        if ((as[k - 1] > 0) != (as[k] > 0)) zc.push_back(ts[k - 1] + (ts[k] - ts[k - 1]) * as[k - 1] / (as[k - 1] - as[k]));
      }
      double period = zc.size() >= 3 ? 2 * (zc.back() - zc.front()) / (double)(zc.size() - 1) : 0;
      double s_meas = period > 0 ? 2 * kPi / period / q : 0;
      double nu = dos_T(EF, 300), sg = filter * hh;
      double c = nu * 2 * kPi * kCoulomb / 3.8 * (cs.d > 0 ? 1 - std::exp(-2 * q * cs.d) : 1.0) / q * std::exp(-0.5 * q * q * sg * sg);
      double s_ex = (1 + c) / std::sqrt(1 + 2 * c);
      ok = ok && std::fabs(s_meas / s_ex - 1) < 0.03;
      det += fmt("\n      gate %3.0f nm, wavelength %4.0f nm: speed %.3f v_F, collisionless theory %.3f (ratio %.3f), from %0.f zero crossings",
                 cs.d, 2 * kPi / q, s_meas, s_ex, s_meas / s_ex, (double)zc.size());
    }
    verdict("T25", ok, "plasma-wave speed against v_F (1 + c) / sqrt(1 + 2 c), target 3 %" + det);
  }

  // -------------------------------------------------------------------------
  if (want("T26")) {
    // interaction off, imposed potential: reproduce Stage B7 for G3 at 700 nm
    Boundary b = g3();
    struct Ref { int imposed; double V, f, I, e; const char* what; };
    const Ref refs[] = {{1, 0.1, 0, 103.672, 0.106, "uniform, static +0.1 V"}, {1, -0.1, 0, -66.569, 0.083, "uniform, static -0.1 V"},
                        {2, 0.1, 0, 79.846, 0.095, "neck drop, static +0.1 V"}, {2, -0.1, 0, -70.604, 0.089, "neck drop, static -0.1 V"},
                        {1, 0.1, 1e12, 3.026, 0.100, "uniform, 1 THz"}, {2, 0.1, 1e12, 2.780, 0.105, "neck drop, 1 THz"}};
    bool ok = true; std::string det;
    for (const Ref& r : refs) {
      PicParams p; p.EF = EF; p.h = h; p.N = N; p.interaction = false; p.lambda_mfp = 700;
      p.imposed = r.imposed; p.ell_F = 100; p.x_F = kG3xF; p.seed = 17;
      if (r.f > 0) { p.f_Hz = r.f; p.Va = r.V; p.ramp_fs = 0; } else { p.V = r.V; p.ramp_fs = 200; }
      Pic pic(b, p);
      double t0 = now();
      PicResult res = run_window(pic, 3000, 1000, 12);
      double I = res.Idc * 1e6, e = std::hypot(res.Idc_err * 1e6, r.e);
      // static currents: 1 % systematic allowance for the time stepping of the ensemble code
      double tol = r.f > 0 ? 2.5 * e : std::max(2.5 * e, 0.01 * std::fabs(r.I));
      ok = ok && std::fabs(I - r.I) < tol;
      char buf[400];
      std::snprintf(buf, sizeof buf, "\n      %-26s I = %8.3f +- %.3f uA (volume %8.3f), Stage B7 %8.3f +- %.3f  [%.0f s]", r.what, I, res.Idc_err * 1e6,
                    res.Idc_vol * 1e6, r.I, r.e, now() - t0);
      det += buf;
    }
    verdict("T26", ok, "interaction off reproduces Stage B7 (G3, 700 nm)" + det);
  }

  // -------------------------------------------------------------------------
  if (want("T27")) {
    // (a) A ballistic strip must have the Sharvin conductance whatever the
    // interaction does: this catches numerical scattering and any mismatch
    // between injection and potential at the contacts. The current of the strip
    // has a term in |V| (fewer modes on the decelerating side), so G is
    // extrapolated to zero bias from +-5 and +-10 mV.
    bool ok = true; std::string det;
    {
      Boundary bs = make_straight_channel(546.41, 500);
      double kT = kBoltzmann * 300;
      double Gsh = kValleySpin * kE2OverH * 500 / (kPi * kHbarVF) * kT * std::log(1 + std::exp(EF / kT));
      for (double d : {30.0, 300.0}) {
        double g[2], eg[2], t0 = now(); int k = 0;
        for (double V : {0.005, 0.01}) {
          double I[2], e[2]; int m = 0;
          for (double sgn : {1.0, -1.0}) {
            PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.d_gate = d; p.V = sgn * V; p.ramp_fs = 300; p.seed = 19;
            Pic pic(bs, p);
            PicResult res = run_window(pic, 3000, 1000, 10);
            I[m] = res.Idc; e[m] = res.Idc_err; ++m;
          }
          g[k] = (I[0] - I[1]) / (2 * V); eg[k] = std::hypot(e[0], e[1]) / (2 * V); ++k;
        }
        double G0 = 2 * g[0] - g[1], eG0 = std::hypot(2 * eg[0], eg[1]);
        ok = ok && std::fabs(G0 / Gsh - 1) < 0.01 + 2 * eG0 / Gsh;
        det += fmt("\n      strip 546 x 500 nm, gate %3.0f nm: G = %.4f (5 mV), %.4f (10 mV), extrapolated %.4f +- %.4f mS; Sharvin %.4f mS (ratio %.4f)  [%.0f s]",
                   d, g[0] * 1e3, g[1] * 1e3, G0 * 1e3, eG0 * 1e3, Gsh * 1e3, G0 / Gsh, now() - t0);
      }
    }
    // (b) G3 at 700 nm, +-10 mV: interaction on against the imposed uniform ramp
    {
      Boundary b = g3();
      double G[3] = {0, 0, 0}, eG[3] = {0, 0, 0};
      for (int c = 0; c < 3; ++c) {
        double Ip = 0, Im = 0, e2 = 0, t0 = now();
        for (double V : {0.01, -0.01}) {
          PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.lambda_mfp = 700; p.V = V; p.ramp_fs = 300; p.seed = 19;
          if (c == 0) { p.interaction = false; p.imposed = 1; }
          else { p.d_gate = c == 1 ? 30 : 300; p.mode = DriveMode::Sym; }
          Pic pic(b, p);
          PicResult res = run_window(pic, 3000, 1000, 12);
          (V > 0 ? Ip : Im) = res.Idc; e2 += res.Idc_err * res.Idc_err;
        }
        G[c] = (Ip - Im) / 0.02; eG[c] = std::sqrt(e2) / 0.02;
        det += std::string("\n      G3, ") + (c == 0 ? "interaction off, uniform ramp" : c == 1 ? "interaction on, 30 nm gate   " : "interaction on, 300 nm gate  ");
        det += fmt(": G = %.4f +- %.4f mS  [%.0f s]", G[c] * 1e3, eG[c] * 1e3, now() - t0);
      }
      ok = ok && std::fabs(G[1] / G[0] - 1) < 0.03 && std::fabs(G[2] / G[0] - 1) < 0.03;
      det += fmt("\n      G3 ratios to the interaction-free value: %.4f (30 nm), %.4f (300 nm); target 3 %% at this bias", G[1] / G[0], G[2] / G[0]);
    }
    verdict("T27", ok, "conductance with the interaction on" + det);
  }

  // -------------------------------------------------------------------------
  if (want("T28")) {
    // bookkeeping on a driven run: charge conservation, the two current
    // estimators, the two expressions for the absorbed power
    Boundary b = g3();
    PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.lambda_mfp = 700; p.d_gate = 30;
    p.mode = DriveMode::Sym; p.f_Hz = 1e12; p.Va = 0.1; p.ramp_fs = 1000; p.seed = 23;
    Pic pic(b, p);
    PicResult res = run_window(pic, 3000, 1000, 8);
    double Ph = 0.5 * p.Va * res.I1.real();
    bool ok = res.charge_error < 1e-7 && pic.electro().last_metal_residual() < 1e-9 && res.Pabs > 0 &&
              std::fabs(res.Pabs / Ph - 1) < 0.03 && std::fabs(res.Idc_vol - res.Idc) < 4 * res.Idc_err + 0.02 * std::fabs(res.Idc) && pic.leaked() == 0;
    verdict("T28", ok, fmt("charge bookkeeping error %.1e electrons; contact potential residual %.1e eV; I_dc plane %.3f +- %.3f, volume %.3f uA;", res.charge_error,
                           pic.electro().last_metal_residual(), res.Idc * 1e6, res.Idc_err * 1e6, res.Idc_vol * 1e6) +
                         fmt("\n      absorbed power %.4f uW from <V I>, %.4f uW from the first harmonic; Y1 = %.4f %+.4fi mS; leaked particles %.0f",
                             res.Pabs * 1e6, Ph * 1e6, res.Y1.real() * 1e3, res.Y1.imag() * 1e3, (double)pic.leaked()));
  }

  // -------------------------------------------------------------------------
  if (want("T29")) {
    Boundary b = g3();
    bool ok = true; std::string det;
    // (a) no drive: no current, no heating
    for (double d : {30.0, 300.0}) {
      PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.lambda_mfp = 700; p.d_gate = d; p.seed = 29;
      Pic pic(b, p);
      double Ek0 = pic.history().empty() ? 0 : 0;
      pic.step(); Ek0 = pic.history().back().Ek;
      double t0 = now();
      PicResult res = run_window(pic, 2000, 1000, 18);
      double drift = res.Ek_last / Ek0 - 1;
      ok = ok && std::fabs(res.Idc) < 2.5 * res.Idc_err && std::fabs(drift) < 0.01;
      det += fmt("\n      no drive, gate %3.0f nm: I_dc = %+.3f +- %.3f uA; mean kinetic energy %.5f -> %.5f eV (%+.2f %%) in 20 ps  [%.0f s]",
                 d, res.Idc * 1e6, res.Idc_err * 1e6, Ek0, res.Ek_last, 100 * drift, now() - t0);
    }
    // (b) mirror-symmetric device, shape-only drive at 1 THz
    {
      Boundary bs = make_symmetric_constriction(500, 100, 30, 100, 0);
      PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.lambda_mfp = 700; p.d_gate = 30;
      p.mode = DriveMode::Sym; p.f_Hz = 1e12; p.Va = 0.1; p.ramp_fs = 1000; p.seed = 31;
      Pic pic(bs, p);
      double t0 = now();
      PicResult res = run_window(pic, 3000, 1000, 12);
      ok = ok && std::fabs(res.Idc) < 2.5 * res.Idc_err;
      det += fmt("\n      mirror-symmetric device, 1 THz, shape-only drive: I_dc = %+.3f +- %.3f uA  [%.0f s]", res.Idc * 1e6, res.Idc_err * 1e6, now() - t0);
    }
    verdict("T29", ok, "null tests" + det);
  }

  // -------------------------------------------------------------------------
  // Stage B9: walls of rms roughness h_r, specularity exp(-(2 h_r k cos(theta))^2)
  if (want("T31")) {
    const double kF = EF / kHbarVF, hr = 2.4;
    const Vec2 wn{0, 1}, wd{1, 0};
    const int NB = 18;
    auto chi2 = [&](const std::vector<double>& cnt, const std::vector<double>& ex) {
      double tot = 0, s = 0;
      for (double c : cnt) tot += c;
      for (int i = 0; i < NB; ++i) { double e = ex[i] * tot; s += (cnt[i] - e) * (cnt[i] - e) / e; }
      return s;
    };
    auto bin = [&](double kx, double ky) { double a = std::atan2(kx, ky); int i = (int)std::floor((a + 0.5 * kPi) / kPi * NB); return std::min(std::max(i, 0), NB - 1); };
    const double chi_max = (NB - 1) + 3 * std::sqrt(2.0 * (NB - 1));
    // (b) the reflection law at fixed angles of incidence
    {
      bool ok = true; std::string det;
      Rng rng(3101);
      const long M = 400000;
      for (double kk : {kF, 0.6 * kF}) {
        const double q0 = 2 * hr * kk;
        std::vector<double> cnt(NB, 0.0), ex(NB, 0.0);
        double zmax = 0, dev = 0;
        std::string row;
        for (double deg : {0.0, 30.0, 60.0, 80.0}) {
          const double th = deg2rad(deg), pex = std::exp(-q0 * q0 * std::cos(th) * std::cos(th));
          long m = 0;
          for (long i = 0; i < M; ++i) {
            double kx = kk * std::sin(th), ky = -kk * std::cos(th);
            const double kx0 = kx, ky0 = ky;
            if (Pic::reflect(kx, ky, wn, wd, 1.0, hr, rng)) { ++m; dev = std::max(dev, std::fabs(kx - kx0) + std::fabs(ky + ky0)); }
            else { cnt[bin(kx, ky)] += 1; dev = std::max(dev, std::fabs(std::sqrt(kx * kx + ky * ky) - kk)); }
          }
          double z = ((double)m / M - pex) / std::sqrt(pex * (1 - pex) / M);
          zmax = std::max(zmax, std::fabs(z));
          row += fmt(" %2.0f deg %.4f (%.4f, %+.1f)", deg, (double)m / M, pex, z);
        }
        // expected distribution of the scattered angle, (1 - p(a)) cos(a), by bin
        double tot = 0;
        for (int i = 0; i < NB; ++i) {
          const int ns = 200;
          for (int j = 0; j < ns; ++j) { double a = -0.5 * kPi + kPi * (i + (j + 0.5) / ns) / NB, c = std::cos(a); ex[i] += -std::expm1(-q0 * q0 * c * c) * c; }
          tot += ex[i];
        }
        for (double& e : ex) e /= tot;
        double c2 = chi2(cnt, ex);
        ok = ok && zmax < 3 && c2 < chi_max && dev < 1e-12;
        det += fmt("\n      k = %.4f /nm: mirror-like fraction, (formula, deviation in standard errors):", kk) + row +
               fmt("\n        scattered angles against (1 - p) cos: chi2 = %.1f on %.0f bins (limit %.1f); |k| and the mirror direction kept to %.0e", c2, NB, chi_max, dev);
      }
      verdict("T31b", ok, fmt("reflection law of a wall of roughness %.1f nm", hr) + det);
    }
    // (b2) a gas arriving isotropic (cosine-law flux) leaves isotropic; with the
    // cosine law for the scattered part it does not
    {
      Rng rng(3102);
      const long M = 4000000;
      std::vector<double> ex(NB);
      for (int i = 0; i < NB; ++i) ex[i] = 0.5 * (std::sin(-0.5 * kPi + kPi * (i + 1) / NB) - std::sin(-0.5 * kPi + kPi * i / NB));
      double c2[2];
      for (int w = 0; w < 2; ++w) {
        std::vector<double> cnt(NB, 0.0);
        for (long i = 0; i < M; ++i) {
          double a = rng.cosine_angle(), kx = kF * std::sin(a), ky = -kF * std::cos(a);
          Pic::reflect(kx, ky, wn, wd, 1.0, hr, rng, w == 1);
          cnt[bin(kx, ky)] += 1;
        }
        c2[w] = chi2(cnt, ex);
      }
      verdict("T31b2", c2[0] < chi_max && c2[1] > 10 * chi_max,
              fmt("cosine-law flux in, flux out against the cosine law: chi2 = %.1f (limit %.1f); with cosine-law re-emission of the scattered part chi2 = %.0f", c2[0], chi_max, c2[1]));
    }
    // (c) the funnel as a closed box with rough walls, no field, no scattering
    // in the bulk: the gas stays uniform and isotropic
    {
      Boundary b = g3();
      const double xF = kG3xF, t30 = std::tan(deg2rad(30.0));
      std::string det; bool ok = true;
      for (int w = 0; w < 2; ++w) {
        PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.contacts = false; p.interaction = false; p.rough = hr; p.rough_lambert = w == 1; p.seed = 3103;
        Pic pic(b, p);
        double t0 = now();
        while (pic.time() < 20000 - 1e-9) pic.step();
        std::vector<double> s; pic.snapshot(s);
        const long n = (long)s.size() / 4;
        // regions: wide lead, taper, far lead, and the last 100 nm of the taper
        const double A = pic.area(), ar[4] = {100.0 * 500, 0.5 * (500 + 100) * (xF - 100), 100.0 * 500, 100 * (100 + 100 * t30)};
        double cn[4] = {0, 0, 0, 0}, c2s[4] = {0, 0, 0, 0};
        for (long i = 0; i < n; ++i) {
          double x = s[4 * i], kx = s[4 * i + 2], ky = s[4 * i + 3], k2 = kx * kx + ky * ky, c2 = k2 > 0 ? (kx * kx - ky * ky) / k2 : 0;
          int r = x < 100 ? 0 : x < xF ? 1 : 2;
          cn[r] += 1; c2s[r] += c2;
          if (x >= xF - 100 && x < xF) { cn[3] += 1; c2s[3] += c2; }
        }
        double zmax = 0; std::string row;
        const char* nm[4] = {"wide lead", "taper", "far lead", "narrow end of taper"};
        for (int r = 0; r < 4; ++r) {
          double f = ar[r] / A, zf = (cn[r] / n - f) / std::sqrt(f * (1 - f) / n), za = (c2s[r] / cn[r]) / std::sqrt(0.5 / cn[r]);
          zmax = std::max(zmax, std::max(std::fabs(zf), std::fabs(za)));
          char buf[320];
          std::snprintf(buf, sizeof buf, "\n          %-20s share of particles %.5f (area share %.5f, %+.1f s.e.); <cos 2 phi> = %+.5f (%+.1f s.e.)", nm[r], cn[r] / n, f, zf, c2s[r] / cn[r], za);
          row += buf;
        }
        long wc[6]; pic.wall_counts(wc);
        det += fmt(w == 0 ? "\n      reciprocal rule, 20 ps, %.0f particles, %.0f wall hits (%.3f mirror-like): largest deviation %.1f s.e.  [%.0f s]"
                          : "\n      cosine-law re-emission (the error the test is for), %.0f particles, %.0f wall hits (%.3f mirror-like): largest deviation %.1f s.e.  [%.0f s]",
                   (double)n, (double)(wc[0] + wc[2] + wc[4]), (double)(wc[1] + wc[3] + wc[5]) / (double)(wc[0] + wc[2] + wc[4]), zmax, now() - t0) + row;
        ok = ok && (w == 0 ? zmax < 3.5 : zmax > 10);
      }
      verdict("T31c", ok, "closed funnel with rough walls stays uniform and isotropic (8 numbers, limit 3.5 s.e.); the wrong rule is caught" + det);
    }
  }

  // -------------------------------------------------------------------------
  // Stage B10 (plans/ballistic_contacts.md section 5): contacts that reflect.
  if (want("T32") || want("T32b") || want("T32c") || want("T32d")) {
    const bool all32 = want("T32");
    const double kF = EF / kHbarVF, kT = kBoltzmann * 300;
    const double TC[3] = {0.75, 0.5, 0.25}, DJ[2] = {7.0, 38.0};
    // averages of a function g(T) of the junction transmission over the sine s
    // of the angle (cosine-law flux) and over the wave number, with weight
    // k f(E) (arrivals of the gas at rest) or k (-df/dE) (linear conductance)
    auto javg = [&](double d, bool conductance, bool series) {
      double num = 0, den = 0;
      const int nk = 4000, ns = 4000;
      for (int i = 0; i < nk; ++i) {
        double k = (i + 0.5) / nk * 3 * kF, x = (kHbarVF * k - EF) / kT;
        double f = 1 / (std::exp(x) + 1), w = k * (conductance ? f * (1 - f) : f), in = 0;
        for (int j = 0; j < ns; ++j) { double sn = (j + 0.5) / ns, T = std::exp(-kPi * k * d * sn * sn); in += series ? T / (2 - T) : T; }
        num += w * in / ns; den += w;
      }
      return num / den;
    };
    // (b) the contact law at fixed angles
    if (all32 || want("T32b")) {
      bool ok = true; std::string det;
      Rng rng(3201);
      const long M = 400000;
      const Vec2 n{1, 0};
      for (int v = 0; v < 2; ++v)
        for (double kk : {kF, 0.6 * kF}) {
          std::string row; double zmax = 0;
          for (double deg : {0.0, 10.0, 25.0, 50.0, 80.0}) {
            const double th = deg2rad(deg), kx = -kk * std::cos(th), ky = kk * std::sin(th);
            const double pex = v == 0 ? 0.5 : std::exp(-kPi * kk * DJ[1] * std::sin(th) * std::sin(th));
            long m = 0;
            for (long i = 0; i < M; ++i) if (rng.uniform() < Pic::contact_transmission(kx, ky, n, v == 0 ? 0.5 : 1.0, v == 0 ? 0.0 : DJ[1])) ++m;
            double se = std::sqrt(std::max(pex * (1 - pex), 1e-12) / M), z = ((double)m / M - pex) / se;
            if (pex > 1e-9 && pex < 1 - 1e-9) zmax = std::max(zmax, std::fabs(z)); else ok = ok && (double)m / M == (pex > 0.5 ? 1.0 : 0.0);
            row += fmt(" %2.0f deg %.4f (%.4f)", deg, (double)m / M, pex);
          }
          ok = ok && zmax < 3;
          det += fmt(v == 0 ? "\n      transparency 0.5, k = %.4f /nm: absorbed fraction (formula):" : "\n      junction 38 nm, k = %.4f /nm: absorbed fraction (formula):", kk) + row + fmt("; largest deviation %.1f s.e.", zmax);
        }
      verdict("T32b", ok, "contact law at fixed angles" + det);
    }
    // (c) the contact resistance: a clean strip with mirror walls between two
    // such contacts conducts T / (2 - T) of the ideal value (an electron let in
    // with probability T ends in the far contact with probability 1 / (2 - T))
    if (all32 || want("T32c")) {
      bool ok = true; std::string det;
      Boundary bs = make_straight_channel(546.41, 500);
      const double V = 0.005;
      double Gid = 0, eGid = 0;
      for (int c = 0; c < 6; ++c) {
        double I[2], e[2], t0 = now(); int m = 0;
        for (double sgn : {1.0, -1.0}) {
          PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.interaction = false; p.imposed = 1; p.V = sgn * V; p.ramp_fs = 300; p.seed = 3202;
          if (c >= 1 && c <= 3) p.contact_T = TC[c - 1];
          if (c >= 4) p.contact_junction = DJ[c - 4];
          Pic pic(bs, p);
          PicResult res = run_window(pic, c == 0 ? 3000 : 8000, 1000, 10);
          I[m] = res.Idc; e[m] = res.Idc_err; ++m;
        }
        double G = (I[0] - I[1]) / (2 * V), eG = std::hypot(e[0], e[1]) / (2 * V);
        if (c == 0) {
          Gid = G; eGid = eG;
          double Gsh = kValleySpin * kE2OverH * 500 / (kPi * kHbarVF) * kT * std::log(1 + std::exp(EF / kT));
          det += fmt("\n      ideal contacts: G = %.4f +- %.4f mS (Sharvin %.4f mS)  [%.0f s]", G * 1e3, eG * 1e3, Gsh * 1e3, now() - t0);
          continue;
        }
        double target = c <= 3 ? TC[c - 1] / (2 - TC[c - 1]) : javg(DJ[c - 4], true, true);
        double r = G / Gid, er = r * std::hypot(eG / G, eGid / Gid);
        bool good = std::fabs(r - target) < std::max(0.02 * target, 3 * er);
        ok = ok && good;
        det += fmt(c <= 3 ? "\n      transparency %.2f: G / G_ideal = %.4f +- %.4f, target %.4f (%+.1f s.e.)  [%.0f s]" : "\n      junction %.0f nm: G / G_ideal = %.4f +- %.4f, target %.4f (%+.1f s.e.)  [%.0f s]",
                   c <= 3 ? TC[c - 1] : DJ[c - 4], r, er, target, (r - target) / er, now() - t0);
      }
      verdict("T32c", ok, "strip conductance between reflecting contacts, +-5 mV, no interaction" + det);
    }
    // (d) equilibrium: with no drive the funnel carries no current, keeps its
    // particles and its kinetic energy, the gas in front of the source stays
    // isotropic, and the contacts absorb the share the formula gives for a gas
    // at rest. Injection that is not thinned is caught.
    if (all32 || want("T32d")) {
      bool ok = true; std::string det;
      Boundary b = g3();
      struct Case { const char* name; double T, dj; bool inter, wrong; };
      const Case cs[5] = {{"transparency 0.25, 30 nm gate", 0.25, 0, true, false}, {"junction 38 nm, 30 nm gate", 1, 38, true, false},
                          {"transparency 0.25, no interaction", 0.25, 0, false, false}, {"junction 38 nm, no interaction", 1, 38, false, false},
                          {"transparency 0.25, injection not thinned (the error the test is for)", 0.25, 0, false, true}};
      for (const Case& c : cs) {
        PicParams p; p.EF = EF; p.h = h; p.N = N; p.filter = filter; p.lambda_mfp = 700; p.contact_T = c.T; p.contact_junction = c.dj; p.contact_unthinned = c.wrong; p.seed = 3203;
        if (c.inter) p.d_gate = 30; else p.interaction = false;
        Pic pic(b, p);
        double t0 = now();
        const double tend = c.wrong ? 10000 : 20000;
        pic.step(); double Ek0 = pic.history().back().Ek;
        while (pic.time() < 3000 - 1e-9) pic.step();
        double ts = pic.time();
        pic.reset_wall_counts();
        while (pic.time() < tend - 1e-9) pic.step();
        PicResult res = pic.analyse(ts, 1000, (int)std::lround((tend - 3000) / 1000));
        long cc[4]; pic.contact_counts(cc);
        std::vector<double> s; pic.snapshot(s);
        double c2 = 0; long nl = 0;
        for (size_t i = 0; i + 3 < s.size(); i += 4) if (s[i] < 100) { double kx = s[i + 2], ky = s[i + 3], k2 = kx * kx + ky * ky; if (k2 > 0) { c2 += (kx * kx - ky * ky) / k2; ++nl; } }
        double za = (c2 / nl) / std::sqrt(0.5 / nl);
        double fex = c.dj > 0 ? javg(c.dj, false, false) : c.T;
        double fS = (double)cc[1] / cc[0], fD = (double)cc[3] / cc[2];
        double zS = (fS - fex) / std::sqrt(fex * (1 - fex) / cc[0]), zD = (fD - fex) / std::sqrt(fex * (1 - fex) / cc[2]);
        double dn = res.np_mean / (double)N - 1, dE = res.Ek_last / Ek0 - 1;
        bool good;
        if (c.wrong) good = dn > 0.10;
        else good = std::fabs(res.Idc) < 2 * res.Idc_err + 1e-12 && std::fabs(dn) < 0.003 && std::fabs(dE) < 0.01 && std::fabs(za) < 3.5 &&
                    (c.inter ? std::fabs(fS - fex) < 0.01 && std::fabs(fD - fex) < 0.01 : std::fabs(zS) < 3.5 && std::fabs(zD) < 3.5);
        ok = ok && good;
        char buf[640];
        std::snprintf(buf, sizeof buf, "\n      %s: I_dc %+.3f +- %.3f uA; particles %+.3f %%; kinetic energy %+.3f %%; <cos 2 phi> in the source lead %+.1f s.e.;"
                                       "\n          absorbed share %.4f source, %.4f drain (formula %.4f; %+.1f, %+.1f s.e.)  %s  [%.0f s]",
                      c.name, res.Idc * 1e6, res.Idc_err * 1e6, 100 * dn, 100 * dE, za, fS, fD, fex, zS, zD, good ? "ok" : "NOT OK", now() - t0);
        det += buf;
      }
      verdict("T32d", ok, "the funnel with reflecting contacts and no drive stays in equilibrium; injection that is not thinned is caught" + det);
    }
  }

  std::printf("\n%d passed, %d failed\n", g_pass, g_fail);
  return g_fail ? 1 : 0;
}
