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

// structure for Peak values
struct Peak
{
  float db, hz;
};

// function for peak sorting
bool comparePeak(const Peak &a, const Peak &b)
{
  return a.db > b.db;
}

// amplitude to dB converting function
float atodb(float a) { return 20.0f * log10f(a / 1.0f); }

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
  const int windowSize = 2048; // L
  const int overlapSize = 1024;
  const int dataSize = sample.size();
  cout << "DATA : " << dataSize << endl;
  //vector<float> magnitudes;
  vector<Peak> peak;
  float threshold = 30.0f;

  int startIndex = 0;
  // go over the sample a window at a time
  while (startIndex < dataSize - windowSize)
  {
    kiss_fft_cfg cfg = kiss_fft_alloc(windowSize, 0, 0, 0);
    kiss_fft_cpx *cx_in = new kiss_fft_cpx[windowSize];
    kiss_fft_cpx *cx_out = new kiss_fft_cpx[windowSize / 2 + 1];

    // time-domain audio parameter
    float zcr = 0.0f;
    float previousMag = 0.0f;

    // defining the window values: cx_in
    for (int i = 0; i < windowSize; i++)
    {
      // Hamming window
      // float w_i = 0.54 - 0.46 * cos(TWO_PI * i / windowSize);
      // cx_in[i].r = sample.at(startIndex + i) * w_i;

      // Rectangle window
      cx_in[i].r = sample.at(startIndex + i);
      cx_in[i].i = 0.0;

      // if the multiplication of magnitude and previous magnitude is negative, increment zero crossing rate.
      if (i != 0 && (cx_in[i].r * previousMag) < 0)
      {
        zcr++;
      }
      previousMag = cx_in[i].r;
    }
    // final evaluation of the zero crossing rate
    zcr /= windowSize;

    // fft on window
    kiss_fft(cfg, cx_in, cx_out);

    // frequency-domain audio parameters
    float centroid_num = 0.0f;
    float centroid_denom = 0.0f;
    float centroid = 0.0f;

    float rms = 0.0f;

    int M = windowSize / 2 + 1;

    float max = -100;
    float maxFreq =-1;

    for (int i = 0; i < M; i++)
    {
      float freq = i * SAMPLE_RATE / (2 * M);
      float magnitudeSquare = pow(cx_out[i].r, 2) + pow(cx_out[i].i, 2);
      float magnitude = sqrt(magnitudeSquare);
      if (magnitude > max){
        max = magnitude;
        maxFreq = freq;
      }

      // check if its a max and add to peak vector
      if (i > 0 && i < M - 1 && atodb(magnitude) > threshold)
      {
        // this part sets "considered" to false as soon as the point considered is not a real max.
        bool considered = true;
        for (int j = 1; j < 2; j++)
        {
          if (sqrt(pow(cx_out[i - j].r, 2) + pow(cx_out[i - j].i, 2)) > magnitude)
          {
            considered = false;
            break;
          }
          if (sqrt(pow(cx_out[i + j].r, 2) + pow(cx_out[i + j].i, 2)) > magnitude)
          {
            considered = false;
            break;
          }
        }

        // if "considered" as reamained true, the point is considered as a peak and added to the peak vector.
        if (considered)
        {
          peak.push_back(Peak());
          peak[peak.size() - 1].hz = freq;
          peak[peak.size() - 1].db = atodb(magnitude);
        }
      }
      // compute the numerator and denominator of the spectral centroid
      centroid_num += freq * magnitude;
      centroid_denom += magnitude;

      // add magnitude square to rms
      rms += magnitudeSquare;
    }

    cout << "maxFreq: " << maxFreq <<  " MAX: " << max << " dB: " << atodb(max) << endl;


    centroid = centroid_num / centroid_denom;
    rms = sqrt(rms / M);

    // int samplenumber, float spectralCentroid, float RMS, float ZCR,
    cout << "samplenumber: " << startIndex << ", spectral centroid: " << centroid << ", rms: " << rms << ", zcr: " << zcr << endl;

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

    // make an error when I add the following lines
    // kiss_fft_free(cfg);
    // delete[] cx_in;
    // delete[] cx_out;
    peak.clear();
    startIndex += overlapSize;
  }
}
