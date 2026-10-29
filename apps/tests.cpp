// Stage 0 / Stage 1 acceptance ladder. Every test prints the measured number,
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
  const double EF12 = EF_from_density(1e12);  // 0.1167 eV
  std::printf("GeometricDiodes acceptance ladder, N=%ld particles per run\n", N);
  std::printf("E_F(1e12 cm^-2)=%.4f eV, lambda_F=%.1f nm, kT(300K)=%.4f eV\n\n", EF12,
              fermi_wavelength(1e12), kBoltzmann * 300);

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

  std::printf("\n%d passed, %d failed\n", n_pass, n_fail);
  return n_fail ? 1 : 0;
}
