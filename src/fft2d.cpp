#include "fft2d.hpp"
#include <algorithm>
#include <cmath>
#include <utility>
#ifdef GD_USE_ACCELERATE
#include <Accelerate/Accelerate.h>
#endif

namespace gd {

static bool g_force_plain = false;
void FFT2D::force_plain(bool on) { g_force_plain = on; }

const char* FFT2D::backend() {
#ifdef GD_USE_ACCELERATE
  return g_force_plain ? "plain radix-2" : "vDSP";
#else
  return "plain radix-2";
#endif
}

void FFT2D::make_plan(Plan& pl, int lg) {
  const double pi = 3.14159265358979323846;
  pl.lg = lg; pl.n = 1 << lg;
  pl.cs.resize(pl.n / 2 + 1); pl.sn.resize(pl.n / 2 + 1);
  for (int i = 0; i < pl.n / 2; ++i) { pl.cs[i] = std::cos(2 * pi * i / pl.n); pl.sn[i] = std::sin(2 * pi * i / pl.n); }
  pl.rev.resize(pl.n);
  for (int i = 0; i < pl.n; ++i) {
    int r = 0;
    for (int b = 0; b < lg; ++b) if (i & (1 << b)) r |= 1 << (lg - 1 - b);
    pl.rev[i] = r;
  }
}

FFT2D::FFT2D(int log2nx, int log2ny) : lgx_(log2nx), lgy_(log2ny), nx_(1 << log2nx), ny_(1 << log2ny) {
  make_plan(px_, lgx_); make_plan(py_, lgy_);
#ifdef GD_USE_ACCELERATE
  setup_ = vDSP_create_fftsetupD((vDSP_Length)std::max(lgx_, lgy_), kFFTRadix2);
#endif
}

FFT2D::~FFT2D() {
#ifdef GD_USE_ACCELERATE
  if (setup_) vDSP_destroy_fftsetupD((FFTSetupD)setup_);
#endif
}

// In-place transform of pl.n contiguous points.
void FFT2D::fft1(const Plan& pl, double* re, double* im, bool inv) const {
#ifdef GD_USE_ACCELERATE
  if (!g_force_plain) {
    DSPDoubleSplitComplex sc{re, im};
    vDSP_fft_zipD((FFTSetupD)setup_, &sc, 1, (vDSP_Length)pl.lg, inv ? kFFTDirection_Inverse : kFFTDirection_Forward);
    return;
  }
#endif
  const int n = pl.n;
  for (int i = 0; i < n; ++i) {
    int j = pl.rev[i];
    if (j > i) { std::swap(re[i], re[j]); std::swap(im[i], im[j]); }
  }
  for (int len = 2; len <= n; len <<= 1) {
    int half = len >> 1, step = n / len;
    for (int s = 0; s < n; s += len)
      for (int k = 0; k < half; ++k) {
        double c = pl.cs[k * step], sgn = inv ? pl.sn[k * step] : -pl.sn[k * step];
        int a = s + k, b = s + k + half;
        double tr = re[b] * c - im[b] * sgn, ti = re[b] * sgn + im[b] * c;
        re[b] = re[a] - tr; im[b] = im[a] - ti;
        re[a] += tr; im[a] += ti;
      }
  }
}

// Row pass over rows [rows_first, rows_last), column pass over all columns.
// The forward transform does rows then columns, the inverse columns then rows,
// so that rows which are zero on input or not needed on output can be skipped.
void FFT2D::transform(double* re, double* im, bool inv, int rows_first, int rows_last) const {
  auto rows = [&]() {
    for (int r = rows_first; r < rows_last; ++r) fft1(px_, re + (long)r * nx_, im + (long)r * nx_, inv);
  };
  auto cols = [&]() {
    const int B = 8;   // columns gathered together, for cache locality
    std::vector<double> tr((long)B * ny_), ti((long)B * ny_);
    for (int c0 = 0; c0 < nx_; c0 += B) {
      int nb = std::min(B, nx_ - c0);
      for (int r = 0; r < ny_; ++r) {
        const double* pr = re + (long)r * nx_ + c0; const double* pi = im + (long)r * nx_ + c0;
        for (int b = 0; b < nb; ++b) { tr[(long)b * ny_ + r] = pr[b]; ti[(long)b * ny_ + r] = pi[b]; }
      }
      for (int b = 0; b < nb; ++b) fft1(py_, tr.data() + (long)b * ny_, ti.data() + (long)b * ny_, inv);
      for (int r = 0; r < ny_; ++r) {
        double* pr = re + (long)r * nx_ + c0; double* pi = im + (long)r * nx_ + c0;
        for (int b = 0; b < nb; ++b) { pr[b] = tr[(long)b * ny_ + r]; pi[b] = ti[(long)b * ny_ + r]; }
      }
    }
  };
  if (!inv) { rows(); cols(); } else { cols(); rows(); }
}

void FFT2D::forward(double* re, double* im, int rows_in) const {
  transform(re, im, false, 0, rows_in < 0 ? ny_ : std::min(rows_in, ny_));
}

void FFT2D::inverse(double* re, double* im, int rows_out) const {
  transform(re, im, true, 0, rows_out < 0 ? ny_ : std::min(rows_out, ny_));
}

}  // namespace gd
