// Stage 0 / Stage 1 acceptance ladder (T1 to T11) and the Stage B2 ladder
// (T12 to T16: time-dependent imposed field and two-sided estimator). Every test prints the measured number,
// the target, and PASS/FAIL. Targets come from analytic results (Sharvin,
// Datta ballistic-diffusive interpolation, reciprocity, detailed balance) or
// from the independently computed 2D Clausing table in the feasibility report.
#include "generators.hpp"
#include "transport.hpp"
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace gd;

static int n_pass = 0, n_fail = 0;
static void check(const std::string& name, bool ok, const std::string& detail) {
  std::printf("[%s] %-58s %s\n", ok ? "PASS" : "FAIL", name.c_str(), detail.c_str());
  (ok ? n_pass : n_fail)++;
}
static std::string fmt(const char* f, double a, double b = 0, double c = 0, double d = 0) {
  char buf[256]; std::snprintf(buf, sizeof buf, f, a, b, c, d); return buf;
}

int main(int argc, char** argv) {
  long N = argc > 1 ? std::atol(argv[1]) : 200000;
  long NB = argc > 2 ? std::atol(argv[2]) : 1000000;  // Stage B2 tests, particles per side
  bool only_b2 = argc > 3 && std::string(argv[3]) == "b2";
  const double EF12 = EF_from_density(1e12);  // 0.1167 eV
  std::printf("GeometricDiodes acceptance ladder, N=%ld particles per run\n", N);
  std::printf("E_F(1e12 cm^-2)=%.4f eV, lambda_F=%.1f nm, kT(300K)=%.4f eV\n\n", EF12,
              fermi_wavelength(1e12), kBoltzmann * 300);

  if (!only_b2) {
  // ---- T1: specular straight channel, field free: T = 1 exactly ----------
  {
    Boundary b = make_straight_channel(4000, 200);
    TransportParams p; p.uniform_field = false; p.E_fixed = EF12; p.specularity = 1.0; p.max_time = 1e8;
    auto st = Transport(b, p).run(N, 1);
    check("T1 specular straight channel T = 1", st.transmission() == 1.0,
          fmt("T=%.6f trapped=%.2e leaked=%.2e", st.transmission(), st.fraction(Fate::Trapped), st.fraction(Fate::Leaked)));
  }

  // ---- T2: 2D Clausing factors, fully diffuse walls ------------------------
  {
    const double LW[] = {0.5, 1, 2, 4, 8, 16};
    const double ref[] = {0.804, 0.683, 0.541, 0.400, 0.274, 0.179};
    bool all = true; std::string det;
    for (int i = 0; i < 6; ++i) {
      Boundary b = make_straight_channel(LW[i] * 200, 200);
      TransportParams p; p.uniform_field = false; p.E_fixed = EF12; p.specularity = 0.0;
      auto st = Transport(b, p).run(N, 2);
      double T = st.transmission(), e = st.transmission_err();
      bool ok = std::fabs(T - ref[i]) < 0.006 + 3 * e;
      all = all && ok;
      det += fmt("%.0f:%.3f(%.3f) ", LW[i], T, ref[i]);
    }
    check("T2 diffuse-wall channel vs 2D Clausing table (L/W:T(ref))", all, det);
  }

  // ---- T3: field-free reciprocity on an asymmetric device ------------------
  {
    FunnelParams fp;  // Nguyen 2021 Device D
    Boundary b = make_funnel(fp);
    for (double spec : {1.0, 0.3}) {
      TransportParams p; p.uniform_field = false; p.E_fixed = EF12; p.specularity = spec; p.lambda_mfp = 700;
      p.inject_contact = 0; auto s0 = Transport(b, p).run(N, 3);
      p.inject_contact = 1; auto s1 = Transport(b, p).run(N, 4);
      double a = b.contact_width(0) * s0.transmission(), ea = b.contact_width(0) * s0.transmission_err();
      double c = b.contact_width(1) * s1.transmission(), ec = b.contact_width(1) * s1.transmission_err();
      double z = std::fabs(a - c) / std::sqrt(ea * ea + ec * ec);
      check(fmt("T3 field-free reciprocity W_S T_SD = W_D T_DS (funnel, p=%.1f)", spec), z < 3.5,
            fmt("T_SD=%.4f T_DS=%.4f  z=%.2f  (Wn/Wc=0.0875 bound)", s0.transmission(), s1.transmission(), z));
    }
  }

  // ---- T4: reciprocity at fixed total energy WITH the field on -------------
  {
    FunnelParams fp; Boundary b = make_funnel(fp);
    for (double spec : {1.0, 0.3}) {
      TransportParams p; p.V = 0.1; p.E_fixed = EF12 + 0.05; p.specularity = spec; p.lambda_mfp = 700;
      p.inject_contact = 0; auto s0 = Transport(b, p).run(N, 5);
      p.inject_contact = 1; auto s1 = Transport(b, p).run(N, 6);
      // unitarity: N_S(E) T_SD(E) = N_D(E) T_DS(E) with the mode number taken
      // at each contact's own kinetic energy (E at the source, E + eV at the drain)
      double NS = modes(p.E_fixed, b.contact_width(0)), ND = modes(p.E_fixed + p.V, b.contact_width(1));
      double a = NS * s0.transmission(), ea = NS * s0.transmission_err();
      double c = ND * s1.transmission(), ec = ND * s1.transmission_err();
      double z = std::fabs(a - c) / std::sqrt(ea * ea + ec * ec);
      check(fmt("T4 fixed-landscape reciprocity N_S T_SD = N_D T_DS, V=0.1 (p=%.1f)", spec), z < 3.5,
            fmt("T_SD=%.4f T_DS=%.4f  z=%.2f dirac=%.1e", s0.transmission(), s1.transmission(), z, s0.fraction(Fate::Dirac)));
    }
  }

  // ---- T5: symmetric device gives A = 1 -----------------------------------
  {
    Boundary b = make_symmetric_constriction(2000, 1500, 30, 175, 0);
    for (double spec : {1.0, 0.3}) {
      TransportParams p; p.specularity = spec; p.lambda_mfp = 700;
      p.V = 0.1;  auto ip = Transport(b, p).current(N, 7);
      p.V = -0.1; auto im = Transport(b, p).current(N, 8);
      double A = ip.I / std::fabs(im.I);
      double eA = A * std::sqrt(std::pow(ip.I_err / ip.I, 2) + std::pow(im.I_err / im.I, 2));
      check(fmt("T5 symmetric constriction A(0.1 V) = 1 (p=%.1f)", spec), std::fabs(A - 1) < 3.5 * eA + 0.003,
            fmt("A=%.4f +- %.4f  I=%.3e A", A, eA, ip.I));
    }
  }

  // ---- T6: zero bias gives zero current and T_SD = T_DS at E_F ------------
  {
    FunnelParams fp; Boundary b = make_funnel(fp);
    TransportParams p; p.V = 0; p.specularity = 0.5; p.lambda_mfp = 700;
    auto iv = Transport(b, p).current(N / 10, 9);
    check("T6 zero bias: injection weight Z = 0 and I = 0", iv.Z == 0 && iv.I == 0, fmt("Z=%.3e I=%.3e", iv.Z, iv.I));
  }

  // ---- T7: particle conservation (no trapped / leaked / dirac loss) --------
  {
    TeslaParams tp; Boundary b = make_tesla(tp);
    TransportParams p; p.V = 0.1; p.specularity = 0.0; p.lambda_mfp = 700;
    auto st = Transport(b, p).run(N, 10);
    double loss = st.fraction(Fate::Trapped) + st.fraction(Fate::Leaked) + st.fraction(Fate::Dirac);
    check("T7 conservation on Tesla-3 (diffuse walls): lost fraction < 1e-3", loss < 1e-3,
          fmt("source=%.4f drain=%.4f trapped=%.1e leaked=%.1e", st.fraction(Fate::Source),
              st.transmission(), st.fraction(Fate::Trapped), st.fraction(Fate::Leaked)) +
              fmt(" dirac=%.1e", st.fraction(Fate::Dirac)) +
              fmt(" mean_hits=%.0f mean_time=%.0f fs", st.sum_hits / st.n, st.sum_time / st.n));
  }

  // ---- T8: Sharvin conductance of a ballistic channel ---------------------
  {
    Boundary b = make_straight_channel(2000, 1000);
    TransportParams p; p.EF = EF12; p.V = 0.001; p.specularity = 1.0;
    auto iv = Transport(b, p).current(N / 4, 11);
    double G = iv.I / p.V, Gs = sharvin_conductance(EF12, 1000);
    check("T8 Sharvin conductance W=1 um, n=1e12 (target 8.74 mS)", std::fabs(G / Gs - 1) < 0.01,
          fmt("G=%.4f mS  Sharvin=%.4f mS  ratio=%.4f", G * 1e3, Gs * 1e3, G / Gs));
  }

  // ---- T9: ballistic-to-diffusive interpolation (Drude limit) --------------
  {
    const double lam = 300, L = 10000, W = 1000;
    Boundary b = make_straight_channel(L, W);
    TransportParams p; p.EF = EF12; p.V = 0.001; p.specularity = 1.0; p.lambda_mfp = lam; p.max_time = 5e7;
    auto iv = Transport(b, p).current(N / 2, 12);
    double l1 = 0.5 * kPi * lam, Tref = l1 / (L + l1);
    double sigma = 2 * kE2OverH * kF_from_density(1e12) * lam;  // Drude: (2e^2/h) k_F l
    double Gdrude = sigma * W / L;
    double G = iv.I / p.V;
    check("T9 quasi-diffusive channel: T = l1/(L+l1), l1 = pi*lambda/2", std::fabs(iv.T / Tref - 1) < 0.05,
          fmt("T=%.4f ref=%.4f  G=%.3f mS  Drude sigma*W/L=%.3f mS", iv.T, Tref, G * 1e3, Gdrude * 1e3) +
              fmt(" trapped=%.1e", iv.stats.fraction(Fate::Trapped)));
  }

  // ---- T10: field-flight integrator convergence ----------------------------
  {
    FunnelParams fp; Boundary b = make_funnel(fp);
    TransportParams p; p.V = 0.1; p.specularity = 1.0; p.lambda_mfp = 700;
    p.chord_tol = 0.05; auto a = Transport(b, p).current(N, 13);
    p.chord_tol = 0.005; p.max_chord = 10; auto c = Transport(b, p).current(N, 13);
    double z = std::fabs(a.T - c.T) / std::sqrt(a.T_err * a.T_err + c.T_err * c.T_err);
    check("T10 chord tolerance 0.05 vs 0.005 nm gives the same T", z < 3.5, fmt("T=%.4f vs %.4f  z=%.2f", a.T, c.T, z));
  }

  // ---- T11: Truccolo 2024 code-to-code (ballistic, specular) --------------
  {
    FunnelParams tf; tf.Wc = 100; tf.Lc = 100; tf.alpha_deg = 45; tf.Wn = 10; tf.beta_deg = 90; tf.Wd = 40; tf.Ld = 100;
    Boundary b = make_funnel(tf);
    TransportParams p; p.uniform_field = false; p.E_fixed = EF12; p.specularity = 1.0;
    auto s0 = Transport(b, p).run(N, 14);
    std::printf("[INFO] T11 Truccolo-like funnel, zero field: T_SD=%.4f (Wn/Wc=0.100; paper quotes T=0.1)\n", s0.transmission());
    for (double V : {0.1, 0.3}) {
      TransportParams q; q.specularity = 1.0; q.EF = 0.2;
      q.V = V;  auto ip = Transport(b, q).current(N, 15);
      q.V = -V; auto im = Transport(b, q).current(N, 16);
      std::printf("[INFO] T11 Truccolo-like funnel, uniform field, E_F=0.2 eV: A(%.1f V)=%.3f  (paper: 1.05-1.15 at low bias) dirac=%.1e\n",
                  V, ip.I / std::fabs(im.I), im.stats.fraction(Fate::Dirac));
    }
  }

  }  // !only_b2

  // =====================================================================
  // Stage B2 ladder: imposed field in time, two-sided estimator
  // =====================================================================
  std::printf("\nStage B2 ladder, %ld particles per side\n", NB);
  const double EF14 = EF_from_density(1.4e12);
  FunnelParams fpD; Boundary funnel = make_funnel(fpD);
  const double x_neck = fpD.Lc + 0.5 * (fpD.Wc - fpD.Wn) / std::tan(deg2rad(fpD.alpha_deg));
  auto within = [](double a, double b, double err, double nsig, double rel) {
    return std::fabs(a - b) < std::max(nsig * err, rel * std::fabs(b));
  };

  // ---- T12: non-conservative chord integration in a static field ----------
  {
    // kill-test reference (results/killtest.csv, funnel, p = 1, lambda = inf, 400k)
    const double Tp_ref = 0.141297, Tp_err = 0.000479, Tm_ref = 0.0413975, Tm_err = 0.000354, A_ref = 189.457 / 116.69;
    TransportParams p; p.EF = EF14; p.reimpose_energy = false;
    p.V = 0.1;  auto ip = Transport(funnel, p).current(NB, 201);
    p.V = -0.1; auto im = Transport(funnel, p).current(NB, 202);
    double zp = std::fabs(ip.T - Tp_ref) / std::hypot(ip.T_err, Tp_err), zm = std::fabs(im.T - Tm_ref) / std::hypot(im.T_err, Tm_err);
    double A = ip.I / std::fabs(im.I);
    bool ok = zp < 3.5 && zm < 3.5 && std::fabs(A / A_ref - 1) < 0.01 + 3.5 * A * std::hypot(ip.T_err / ip.T, im.T_err / im.T) / A_ref;
    check("T12 dk/dt = F integration (no energy re-imposition) reproduces the static kill test", ok,
          fmt("T+=%.5f (ref %.5f, z=%.2f)", ip.T, Tp_ref, zp) + fmt(" T-=%.5f (z=%.2f) A=%.4f (ref %.4f)", im.T, zm, A, A_ref));
  }

  // ---- T13: two-sided equilibrium estimator against the bias window ------
  {
    TransportParams p; p.EF = EF14; p.V = 0.1;
    Transport tr(funnel, p);
    auto bw = tr.current(NB, 203);
    auto ts = tr.ac_current(NB, 204);
    double z = std::fabs(ts.Idc - bw.I) / std::hypot(ts.Idc_err, bw.I_err);
    check("T13a two-sided equilibrium estimator = bias-window estimator at V = 0.1 V", z < 3.5,
          fmt("I_2s=%.3f +- %.3f uA  I_bw=%.3f +- %.3f uA", ts.Idc * 1e6, ts.Idc_err * 1e6, bw.I * 1e6, bw.I_err * 1e6) + fmt("  z=%.2f", z));
    p.V = 0; Transport t0(funnel, p);
    auto z0 = t0.ac_current(NB, 205);
    check("T13b two-sided estimator at V = 0 gives I = 0", std::fabs(z0.Idc) < 3.5 * z0.Idc_err,
          fmt("I=%.3f +- %.3f uA  T_SD=%.5f T_DS=%.5f", z0.Idc * 1e6, z0.Idc_err * 1e6, z0.T_SD, z0.T_DS));
  }

  // ---- T14: quasi-static limit at 300 MHz ----------------------------------
  // (the plan said 3 GHz; the taper feeds a slow-drift population into the
  // source lead, 5 % of its electrons live longer than 20 ps, and at 3 GHz
  // those already see the field change: the anchor sits a decade lower)
  {
    TransportParams p; p.EF = EF14;
    double Ip[2], Im[2], eIp[2], eIm[2]; const double Vs[2] = {0.05, 0.1};
    for (int i = 0; i < 2; ++i) {
      p.V = Vs[i];  auto a = Transport(funnel, p).current(NB, 210 + i);  Ip[i] = a.I; eIp[i] = a.I_err;
      p.V = -Vs[i]; auto c = Transport(funnel, p).current(NB, 220 + i);  Im[i] = c.I; eIm[i] = c.I_err;
    }
    // even and odd parts: E(V) = I2 V^2 + I4 V^4, O(V) = G1 V + I3 V^3
    double E1 = 0.5 * (Ip[0] + Im[0]), E2 = 0.5 * (Ip[1] + Im[1]);
    double O1 = 0.5 * (Ip[0] - Im[0]), O2 = 0.5 * (Ip[1] - Im[1]);
    double v1 = Vs[0], v2 = Vs[1];
    double I4 = (E2 / (v2 * v2) - E1 / (v1 * v1)) / (v2 * v2 - v1 * v1), I2 = E2 / (v2 * v2) - I4 * v2 * v2;
    double I3 = (O2 / v2 - O1 / v1) / (v2 * v2 - v1 * v1), G1 = O2 / v2 - I3 * v2 * v2;
    double eE1 = 0.5 * std::hypot(eIp[0], eIm[0]), eE2 = 0.5 * std::hypot(eIp[1], eIm[1]);
    const double Va = 0.1;
    // quasi-static dc for a cosine drive: <E(Va cos phi)> = I2 Va^2/2 + 3 I4 Va^4/8,
    // a linear combination of E1 and E2 (error propagated through it)
    auto qs_of = [&](double e1, double e2) {
      double i4 = (e2 / (v2 * v2) - e1 / (v1 * v1)) / (v2 * v2 - v1 * v1), i2 = e2 / (v2 * v2) - i4 * v2 * v2;
      return 0.5 * i2 * Va * Va + 0.375 * i4 * std::pow(Va, 4);
    };
    double Idc_qs = qs_of(E1, E2);
    double eqs = std::hypot((qs_of(E1 + eE1, E2) - Idc_qs), (qs_of(E1, E2 + eE2) - Idc_qs));
    double G_qs = G1 + 0.75 * I3 * Va * Va;
    p.V = 0; p.f_Hz = 3e8; p.Va = Va;
    auto ac = Transport(funnel, p).ac_current(NB, 230);
    bool okI = within(ac.Idc, Idc_qs, std::hypot(ac.Idc_err, eqs), 3.5, 0.05);
    check("T14a quasi-static limit: I_dc(300 MHz, Va 0.1) = I2 Va^2/2 + 3 I4 Va^4/8 from the static curve", okI,
          fmt("I_dc=%.3f +- %.3f uA  qs=%.3f +- %.3f uA", ac.Idc * 1e6, ac.Idc_err * 1e6, Idc_qs * 1e6, eqs * 1e6) +
              fmt("  I2=%.3f mA/V^2 I4=%.3f mA/V^4", I2 * 1e3, I4 * 1e3));
    // |Y1| is the static conductance; the phase is the transit lag w tau and
    // is reported against the mean flight time
    double tau_mean = (ac.S.sum_time + ac.D.sum_time) / (ac.S.n + ac.D.n);
    bool okY = within(std::abs(ac.Y1), G_qs, ac.I1_err / Va, 3.5, 0.05);
    check("T14b |Y1| at 300 MHz = static conductance G1 + 3 I3 Va^2/4", okY,
          fmt("Y1=%.4f%+.4fi mS +- %.4f  G_qs=%.4f mS", ac.Y1.real() * 1e3, ac.Y1.imag() * 1e3, ac.I1_err / Va * 1e3, G_qs * 1e3) +
              fmt("  phase=%.3f rad, w*tau_mean=%.3f", std::arg(ac.Y1), 2 * kPi * p.f_Hz * 1e-15 * tau_mean));
  }

  // ---- T15: straight control under ac drive gives zero -------------------
  {
    Boundary ctrl = make_stepped_channel(2000, 1000, 175, 5490, 2000, 1000);
    for (double f : {3e9, 1e12}) {
      TransportParams p; p.EF = EF14; p.f_Hz = f; p.Va = 0.1;
      auto ac = Transport(ctrl, p).ac_current(NB, 240 + (f > 1e11));
      // the equilibrium injection samples the low-energy tail (weight ~ E), and
      // a few 1e-3 of those are driven below the 2 meV Dirac cut by the field;
      // they would have returned to their own contact, so they carry no current
      double dirac = 0.5 * (ac.S.fraction(Fate::Dirac) + ac.D.fraction(Fate::Dirac));
      double lost = 0.5 * (ac.S.fraction(Fate::Trapped) + ac.S.fraction(Fate::Leaked) + ac.D.fraction(Fate::Trapped) + ac.D.fraction(Fate::Leaked));
      double heated = (ac.S.sum_heated + ac.D.sum_heated) / (ac.S.n + ac.D.n);
      bool ok = std::fabs(ac.Idc) < 3.5 * ac.Idc_err && lost < 1e-4 && dirac < 1e-2 && heated < 1e-3;
      check(fmt("T15 straight control at %.0f GHz: I_dc = 0, trapped+leaked < 1e-4, dirac < 1e-2, heated < 1e-3", f * 1e-9), ok,
            fmt("I_dc=%.3f +- %.3f uA  trapped+leaked=%.1e dirac=%.1e", ac.Idc * 1e6, ac.Idc_err * 1e6, lost, dirac) +
                fmt(" heated=%.1e  Y1=%.4f%+.4fi mS", heated, ac.Y1.real() * 1e3, ac.Y1.imag() * 1e3));
    }
  }

  // ---- T16: chord cap in time, localised field at 1 THz -------------------
  {
    TransportParams p; p.EF = EF14; p.f_Hz = 1e12; p.Va = 0.1; p.field = FieldModel::Localised; p.ell_F = 500; p.x_F = x_neck;
    long N4 = 4 * NB;
    p.chords_per_period = 40;  auto a = Transport(funnel, p).ac_current(N4, 250);
    p.chords_per_period = 160; auto c = Transport(funnel, p).ac_current(N4, 251);
    bool ok = within(a.Idc, c.Idc, std::hypot(a.Idc_err, c.Idc_err), 3.5, 0.02);
    check("T16 chord cap v_F/(40 f) vs v_F/(160 f), localised 500 nm field, 1 THz: same I_dc", ok,
          fmt("I_dc=%.3f +- %.3f uA vs %.3f +- %.3f uA", a.Idc * 1e6, a.Idc_err * 1e6, c.Idc * 1e6, c.Idc_err * 1e6) +
              fmt("  Y1=%.4f%+.4fi mS", a.Y1.real() * 1e3, a.Y1.imag() * 1e3));
  }

  // ---- T17: the Dirac cut does not set the ac result -----------------------
  {
    // the equilibrium injection feeds ~3e-3 of the electrons into the 2 meV
    // cut; with the classical turning point instead, I_dc must not move
    TransportParams p; p.EF = EF14; p.f_Hz = 1e12; p.Va = 0.1; p.field = FieldModel::Localised; p.ell_F = 500; p.x_F = x_neck;
    auto a = Transport(funnel, p).ac_current(2 * NB, 260);
    p.dirac_reflect = true;
    auto c = Transport(funnel, p).ac_current(2 * NB, 261);
    double z = std::fabs(a.Idc - c.Idc) / std::hypot(a.Idc_err, c.Idc_err);
    double dir = 0.5 * (a.S.fraction(Fate::Dirac) + a.D.fraction(Fate::Dirac)), dir_c = 0.5 * (c.S.fraction(Fate::Dirac) + c.D.fraction(Fate::Dirac));
    check("T17 Dirac cut vs classical turning point, localised 500 nm field, 1 THz: same I_dc", z < 3.5,
          fmt("I_dc=%.3f +- %.3f uA (dirac %.1e) vs %.3f", a.Idc * 1e6, a.Idc_err * 1e6, dir, c.Idc * 1e6) +
              fmt(" +- %.3f uA (dirac %.1e)  z=%.2f", c.Idc_err * 1e6, dir_c, z));
  }

  std::printf("\n%d passed, %d failed\n", n_pass, n_fail);
  return n_fail ? 1 : 0;
}
