// Acceptance ladder of Stage B8 (plans/ballistic_selfconsistent.md, section 4):
// the self-consistent particle solver of src/pic.*.
//   usage: gd_pic_tests [T22,T23,...] [--N n] [--h nm] [--filter cells]
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

  std::printf("\n%d passed, %d failed\n", g_pass, g_fail);
  return g_fail ? 1 : 0;
}
