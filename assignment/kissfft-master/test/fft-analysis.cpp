#include "kiss_fft.h"
#include <cmath>
#include <complex>
#include <vector>
#include <iostream>

using namespace std;

/* Write a C++ program that uses the STFT to analyze a sound clip, reporting statistics for each 
time-domain window. Choose tentative values for window size, "hop" size, and FFT size (because of zero 
paddings). For each window, compute the [Spectral centroid][] and collect the top 8 spectral peaks 
(frequencies and magnitudes). Report these values as lines on the standard out so they are human-readable, 
but easy to parse with a regex. Also, calculate the RMS and zero-crossing rate for each window. 
For instance:

> int samplenumber, float spectralCentroid, float RMS, float ZCR, 
hz:db hz:db hz:db hz:db hz:db hz:db hz:db hz:db
*/

int main()
{
  const int N = 1000;
  const int M = N / 2 + 1;

  /*double *in = fftw_alloc_real(N);
  for (int i = 0; i < N; i++)
  {
    in[i] = 0.707 * sin(M_PI * 2 * i / N);
  }

  fftw_complex *out = fftw_alloc_complex(M);
  fftw_plan plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
  fftw_execute(plan);

  for (int k = 0; k < M; k++)
  {
    double r = out[k][0];
    double i = out[k][1];
    double m = log(sqrt(r * r + i * i));
    printf("%lf ", m);
  }
  printf("\n");

  fftw_free(in);
  fftw_free(out);
  fftw_destroy_plan(plan);*/

  kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, 0, 0);
  // kiss_fft_cpx cx_in[N];
  //kiss_fft_cpx cx_out[M];

  kiss_fft_scalar *cx_in = new kiss_fft_scalar[N];
  kiss_fft_cpx *cx_out = new kiss_fft_cpx[M];

  for (int k = 0; k < N; k++)
  {
    //... // put kth sample in cx_in[k].r and cx_in[k].i
    cx_in[k].r = 0.707 * sin(M_PI * 2 * k / N);
    cx_in[k].i = 0;
  }

  kiss_fftr(cfg, cx_in, cx_out);

  for (int i = 0; i < M; i++)
  {
    //... // transformed. DC is in cx_out[0].r and cx_out[0].i
    cout << "out " << i << " " << cx_out[i].r << "+i" << cx_out[i].i << endl;
  }

  kiss_fft_free(cfg);

  /*std::vector<double> in;
  in.resize(1000);

  for (int i = 0; i < in.size(); i++)
  {
    in[i] = 0.707 * sin(M_PI * 2 * i / in.size());
  }
  std::vector<std::complex<double>> out;
  out.resize(in.size() / 2 + 1);

  //fftw_plan plan = fftw_plan_dft_r2c_1d(
   //   in.size(), in.data(), reinterpret_cast<fftw_complex *>(out.data()),
    //  FFTW_ESTIMATE);

  fftw_execute(plan);

  //kiss_fft()

  for (int k = 0; k < out.size(); k++)
  {
    double m = log(abs(out[k]));
    printf("%lf ", m);
  }
  printf("\n");*/
}