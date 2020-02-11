#include "kiss_fft.h"
#include <cmath>
#include <complex>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

/* Write a C++ program that uses the STFT to analyze a sound clip, reporting statistics for each 
time-domain window. Choose tentative values for window size, "hop" size, and FFT size (because of zero 
paddings). For each window, compute the Spectral centroid and collect the top 8 spectral peaks 
(frequencies and magnitudes). Report these values as lines on the standard out so they are human-readable, 
but easy to parse with a regex. Also, calculate the RMS and zero-crossing rate for each window. 
For instance:

> int samplenumber, float spectralCentroid, float RMS, float ZCR, 
hz:db hz:db hz:db hz:db hz:db hz:db hz:db hz:db
*/

struct Peak
{
  float db, hz;
};

float atodb(float a) { return 20.0f * log10f(a / 1.0f); }

bool comparePeak(const Peak &a, const Peak &b)
{
  return a.db > b.db;
}

int main()
{
  // read in a sequence of lines where each line is a human readable floating
  // point number.
  // from Karl Yerkes code read.cpp.
  const float TWO_PI = 6.28318530718;
  const float SAMPLE_RATE = 44100;

  string line;
  getline(cin, line);
  vector<float> sample;

  while (line != "")
  {
    float f = stof(line);
    getline(cin, line);
    sample.push_back(f);
  }

  // STFT algorithm
  // https://www.originlab.com/doc/Origin-Help/STFT-Algorithm
  const int windowSize = 256; // L
  const int overlapSize = 128;
  const int dataSize = sample.size();
  vector<float> magnitudes;
  vector<Peak> peak;
  
  int startIndex = 0;
  // go over the sample a window at a time
  while (startIndex < dataSize - windowSize)
  {
    kiss_fft_cfg cfg = kiss_fft_alloc(windowSize, 0, 0, 0);
    kiss_fft_cpx *cx_in = new kiss_fft_cpx[windowSize];
    kiss_fft_cpx *cx_out = new kiss_fft_cpx[windowSize / 2 + 1];

    // defining the window values
    for (int i = 0; i < windowSize; i++)
    {
      // Hamming window
      float w_i = 0.54 - 0.46 * cos(TWO_PI * i / windowSize);
      cx_in[i].r = sample.at(startIndex + i) * w_i;
      cx_in[i].i = 0.0;
    }

    // fft on window
    kiss_fft(cfg, cx_in, cx_out);

    // info to find
    float centroid_num = 0.0f;
    float centroid_denom = 0.0f;
    float centroid = 0.0f;

    float rms = 0.0f;
    float zcr = 0.0f;
    float previousMag = 0.0f;

    int M = windowSize / 2 + 1;

    for (int i = 0; i < M - 1; i++)
    {
      float freq = (i + startIndex) * SAMPLE_RATE / (2 * M);
      float magnitudeSquare = pow(cx_out[i].r, 2) + pow(cx_out[i].i, 2);
      float magnitude = sqrt(magnitudeSquare);

      // check if its a max and add to peak vector
      if (i > 0 && i < M - 2 && (pow(cx_out[i - 1].r, 2) + pow(cx_out[i - 1].i, 2)) < magnitude && (pow(cx_out[i + 1].r, 2) + pow(cx_out[i + 1].i, 2)) < magnitude)
      {
        peak.push_back(Peak());
        peak[peak.size() - 1].hz = freq;
        peak[peak.size() - 1].db = atodb(magnitude);
      }

      // compute the numerator and denominator of the spectral centroid
      centroid_num += freq * magnitude;
      centroid_denom += magnitude;

      // add magnitude square to rms
      rms += magnitudeSquare;

      // if the multiplication of magnitude and previous magnitude is negative, increment zero crossing rate.
      if (i != 0 && (atodb(magnitude) * atodb(previousMag)) < 0)
      {
        zcr++;
      }

      previousMag = magnitude;
    }

    centroid = centroid_num / centroid_denom;
    rms = sqrt(rms / M);
    zcr /= M;

    // int samplenumber, float spectralCentroid, float RMS, float ZCR,
    cout << "samplenumber: " << startIndex << ", spectral centroid:" << centroid << ", rms: " << rms << ", zcr: " << zcr << endl;

    // hz:db hz:db hz:db hz:db hz:db hz:db hz:db hz:db
    sort(peak.begin(), peak.end(), comparePeak);
    int maxSize = peak.size();
    if (maxSize >= 8)
    {
      maxSize = 8;
    }
    for (int i = 0; i < maxSize; i++)
    {
      cout << peak.at(i).hz << ":" << peak.at(i).db << " ";
    }
    cout << endl;

    kiss_fft_free(cfg);
    delete[] cx_in;
    delete[] cx_out;
    peak.clear();
    startIndex += overlapSize;
  }
}
