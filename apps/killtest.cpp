// Kill test: funnel versus Tesla-inspired channel at equal minimum feature,
// equal leads, under the same uniform-field + momentum-relaxation + edge
// specularity model. Writes results/killtest.csv and prints a summary.
#include "generators.hpp"
#include "transport.hpp"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <map>
#include <string>
#include <vector>

using namespace gd;

struct Geo { std::string name; Boundary b; double L; };

int main(int argc, char** argv) {
  long N = argc > 1 ? std::atol(argv[1]) : 400000;
  const double n_cm2 = 1.4e12;  // Nguyen 2021 Device D at V_g = +18 V
  const double EF = EF_from_density(n_cm2);
  std::vector<double> specs = {1.0, 0.5, 0.0};
  std::vector<double> lambdas = {std::numeric_limits<double>::infinity(), 700, 200};
  std::vector<double> volts = {0.05, 0.1};

  std::vector<Geo> geos;
  { FunnelParams f; Boundary b = make_funnel(f); geos.push_back({"funnel_nguyenD", b, b.bbox_max().x}); }
  { Boundary b = make_stepped_channel(2000, 1000, 175, 5490, 2000, 1000); geos.push_back({"straight_control", b, b.bbox_max().x}); }
  { TeslaParams t; t.n_stages = 1; Boundary b = make_tesla(t); geos.push_back({"tesla_1stage", b, b.bbox_max().x}); }
  { TeslaParams t; t.n_stages = 3; Boundary b = make_tesla(t); geos.push_back({"tesla_3stage", b, b.bbox_max().x}); }
  { TeslaParams t; t.n_stages = 3; t.beta_entry_deg = 20; t.beta_exit_deg = 60; t.r = 300; t.a = 2400;
    Boundary b = make_tesla(t); geos.push_back({"tesla_3stage_v2", b, b.bbox_max().x}); }

  std::ofstream csv("results/killtest.csv");
  csv << "geometry,L_nm,W_S_nm,specularity,lambda_nm,V,I_A,I_err_A,T,T_err,f_source,f_trapped,f_dirac,mean_hits,mean_scatters,mean_time_fs,mean_time_tx_fs\n";
  std::printf("Kill test: n=%.2e cm^-2 (E_F=%.3f eV, lambda_F=%.1f nm), T=300 K, N=%ld per point\n\n",
              n_cm2, EF, fermi_wavelength(n_cm2), N);
  std::printf("%-18s %5s %7s %6s | %10s %10s | %8s %8s | %7s %7s\n", "geometry", "p", "lambda", "V", "I(+V) uA",
              "I(-V) uA", "A", "+-", "R+ kOhm", "dirac");
  struct Row { double Ip, Im, eIp, eIm; };
  for (auto& g : geos) {
    std::string err = g.b.validate(175);
    if (!err.empty()) std::printf("WARNING %s: %s\n", g.name.c_str(), err.c_str());
    for (double spec : specs)
      for (double lam : lambdas)
        for (double V : volts) {
          Row r{};
          for (int sgn : {+1, -1}) {
            TransportParams p; p.EF = EF; p.V = sgn * V; p.specularity = spec; p.lambda_mfp = lam;
            Transport tr(g.b, p);
            auto iv = tr.current(N, 100 + sgn);
            const auto& s = iv.stats;
            csv << g.name << "," << g.L << "," << g.b.contact_width(0) << "," << spec << "," << lam << "," << p.V << ","
                << iv.I << "," << iv.I_err << "," << iv.T << "," << iv.T_err << "," << s.fraction(Fate::Source) << ","
                << s.fraction(Fate::Trapped) + s.fraction(Fate::Leaked) << "," << s.fraction(Fate::Dirac) << ","
                << s.sum_hits / s.n << "," << s.sum_scatters / s.n << "," << s.sum_time / s.n << ","
                << (s.fate_count[(int)Fate::Drain] > 0 ? s.sum_time_tx / s.fate_count[(int)Fate::Drain] : 0) << "\n";
            csv.flush();
            if (sgn > 0) { r.Ip = iv.I; r.eIp = iv.I_err; }
            else { r.Im = std::fabs(iv.I); r.eIm = iv.I_err; }
            if (sgn < 0) {
              double A = r.Ip / r.Im;
              double eA = A * std::sqrt(std::pow(r.eIp / r.Ip, 2) + std::pow(r.eIm / r.Im, 2));
              std::printf("%-18s %5.2f %7.0f %6.2f | %10.3f %10.3f | %8.4f %8.4f | %7.3f %7.1e\n", g.name.c_str(), spec,
                          lam, V, r.Ip * 1e6, r.Im * 1e6, A, eA, V / r.Ip * 1e-3, s.fraction(Fate::Dirac));
            }
          }
        }
    std::printf("\n");
  }
  std::printf("CSV written to results/killtest.csv\n");
  return 0;
}
