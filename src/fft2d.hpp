// Complex 2-D FFT on a 2^mx x 2^my grid (nx points per row, ny rows) with
// split real / imaginary arrays, row-major. Built from 1-D transforms of the
// rows and of the columns. On macOS the 1-D transform is vDSP's (Accelerate);
// a plain radix-2 transform is always compiled in and can be selected for a
// cross check with FFT2D::force_plain(true). Both are unnormalised.
#pragma once
#include <vector>

namespace gd {

class FFT2D {
 public:
  FFT2D(int log2nx, int log2ny);
  ~FFT2D();
  FFT2D(const FFT2D&) = delete;
  FFT2D& operator=(const FFT2D&) = delete;
  int nx() const { return nx_; }
  int ny() const { return ny_; }
  // rows_in: only the first rows_in rows of the input are non-zero (the rest
  // are skipped in the row pass). rows_out: only the first rows_out rows of
  // the result are needed. Negative: all rows.
  void forward(double* re, double* im, int rows_in = -1) const;
  void inverse(double* re, double* im, int rows_out = -1) const;   // divide by nx * ny afterwards
  static const char* backend();
  static void force_plain(bool on);

 private:
  struct Plan { int lg = 0, n = 0; std::vector<double> cs, sn; std::vector<int> rev; };
  void transform(double* re, double* im, bool inv, int rows_first, int rows_last) const;
  void fft1(const Plan& pl, double* re, double* im, bool inv) const;
  static void make_plan(Plan& pl, int lg);
  int lgx_ = 0, lgy_ = 0, nx_ = 0, ny_ = 0;
  Plan px_, py_;
  void* setup_ = nullptr;
};

}  // namespace gd
