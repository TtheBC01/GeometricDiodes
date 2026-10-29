// Core types, units and RNG for the GeometricDiodes kill test.
//
// Units used throughout:  length nm, time fs, energy eV, charge e.
// With v_F = 1e6 m/s = 1 nm/fs the Dirac kinematics read
//     E_kin = hbar v_F |k|,   dx/dt = v_F k/|k|,   hbar dk/dt = F,
// where F is the force in eV/nm (F = -e grad(phi) for electrons).
#pragma once
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace gd {

constexpr double kPi = 3.14159265358979323846;
constexpr double kHbar = 0.6582119569;            // eV fs
constexpr double kVF = 1.0;                        // nm/fs (1e6 m/s)
constexpr double kHbarVF = kHbar * kVF;            // eV nm
constexpr double kBoltzmann = 8.617333262e-5;      // eV/K
constexpr double kE2OverH = 38.7404585e-6;         // siemens (e^2/h)
constexpr int kValleySpin = 4;                     // g_s g_v for graphene

struct Vec2 {
  double x = 0, y = 0;
  Vec2() = default;
  Vec2(double x_, double y_) : x(x_), y(y_) {}
  Vec2 operator+(Vec2 o) const { return {x + o.x, y + o.y}; }
  Vec2 operator-(Vec2 o) const { return {x - o.x, y - o.y}; }
  Vec2 operator*(double s) const { return {x * s, y * s}; }
  Vec2 operator/(double s) const { return {x / s, y / s}; }
  Vec2 operator-() const { return {-x, -y}; }
  Vec2& operator+=(Vec2 o) { x += o.x; y += o.y; return *this; }
  Vec2& operator-=(Vec2 o) { x -= o.x; y -= o.y; return *this; }
  double norm() const { return std::sqrt(x * x + y * y); }
  double norm2() const { return x * x + y * y; }
  Vec2 unit() const { double n = norm(); return n > 0 ? Vec2{x / n, y / n} : Vec2{1, 0}; }
  Vec2 left() const { return {-y, x}; }   // 90 deg counter-clockwise
  Vec2 right() const { return {y, -x}; }  // 90 deg clockwise
};
inline Vec2 operator*(double s, Vec2 v) { return v * s; }
inline double dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
inline double cross(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }
inline Vec2 from_angle(double th) { return {std::cos(th), std::sin(th)}; }
inline double deg2rad(double d) { return d * kPi / 180.0; }

// xoshiro256** -- small, fast, reproducible; seeded via splitmix64.
class Rng {
 public:
  explicit Rng(uint64_t seed = 1) { reseed(seed); }
  void reseed(uint64_t seed) {
    uint64_t z = seed;
    for (auto& w : s_) {
      z += 0x9e3779b97f4a7c15ULL;
      uint64_t t = z;
      t = (t ^ (t >> 30)) * 0xbf58476d1ce4e5b9ULL;
      t = (t ^ (t >> 27)) * 0x94d049bb133111ebULL;
      w = t ^ (t >> 31);
    }
  }
  uint64_t next() {
    const uint64_t result = rotl(s_[1] * 5, 7) * 9;
    const uint64_t t = s_[1] << 17;
    s_[2] ^= s_[0]; s_[3] ^= s_[1]; s_[1] ^= s_[2]; s_[0] ^= s_[3];
    s_[2] ^= t; s_[3] = rotl(s_[3], 45);
    return result;
  }
  // Uniform in (0,1): never returns exactly 0 so that -log(u) is finite.
  double uniform() { return ((next() >> 11) + 0.5) * (1.0 / 9007199254740992.0); }
  double uniform(double a, double b) { return a + (b - a) * uniform(); }
  // Angle with probability density (1/2) cos(theta) on (-pi/2, pi/2):
  // the flux-weighted (Lambertian) law for carriers crossing a line.
  double cosine_angle() { return std::asin(2.0 * uniform() - 1.0); }
  double exponential(double mean) { return -mean * std::log(uniform()); }

 private:
  static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
  uint64_t s_[4]{};
};

// Welford running mean/variance for batch statistics.
struct RunningStat {
  long n = 0;
  double mean = 0, m2 = 0;
  void add(double v) {
    ++n;
    double d = v - mean;
    mean += d / n;
    m2 += d * (v - mean);
  }
  double variance() const { return n > 1 ? m2 / (n - 1) : 0.0; }
  double stderr_mean() const { return n > 1 ? std::sqrt(variance() / n) : 0.0; }
};

}  // namespace gd
