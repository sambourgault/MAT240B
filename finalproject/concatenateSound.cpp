#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

#include "Gist.h"

// parameter for fft
const int sampleRate = 44100;
const int frameSize = 4096;
const int hopSize = frameSize / 2;
const float TWO_PI = 6.28318530718;

// midi to frequency function
float mtof(float m) { return 8.175799f * powf(2.0f, m / 12.0f); }

// amplitude to dB converting function
float atodb(float a) { return 20.0f * log10f(a / 1.0f); }

int main()
{
  // File pointer
  std::fstream fin;

  // Open an existing file
  fin.open("neighbors.meta.csv", std::ios::in);

  std::string line0;
  std::vector<float> neighbors;
  getline(fin, line0);

  while (line0 != "")
  {
    float f = stof(line0);
    getline(fin, line0);
    neighbors.push_back(f);
    //cout << f << endl;
  }
  //cout << neighbors.size() << endl;

  // read in a sequence of lines where each line is a human readable floating
  // point number.
  // std::string line;
  // getline(cin, line);
  // std::vector<float> audio;

  // while (line != "")
  // {
  //   float f = stof(line);
  //   getline(cin, line);
  //   audio.push_back(f);
  // }
  // cout << audio.size() << endl;

  // write a sequence of lines where each line is a human readable floating
  // point number.
  //

  // read in a sequence of lines where each line is a human readable floating
  // point number.
  //
  string line;
  getline(cin, line);
  vector<float> sample;
  while (line != "")
  {
    float f = stof(line);
    getline(cin, line);
    sample.push_back(f);
    //cout << f << endl;
  }

  int ratio = frameSize / hopSize;

  for (int j = 0; j < neighbors.size(); j++)
  {
    //printf("%f\n", sin(neighbors.size()));
    // add 1024 samples at a time
    //cout << neighbors[j] << endl;

    for (int i = 0; i < hopSize; i++)
    {
      float add = 0.0f;
      for (int k = 0; k < ratio; k++)
      {
        if (j >= k && (neighbors[j - k] * hopSize + i + hopSize) < sample.size())
        {
          // Hamming window
          float w_i = 0.54f - 0.46f * cos(TWO_PI * ((k*hopSize)+i) / frameSize);
          //float w_i = 0.5f;
          // Hanning
          //float w_i = 0.5f * (1 - cos(TWO_PI * ((k*hopSize)+i) / frameSize));
          add += w_i*sample[neighbors[j - k] * hopSize + (k*hopSize) + i];
        }
      }

      printf("%f\n", add);

      // Hamming window
      //float w_i = 0.54f - 0.46f * cos(TWO_PI * i / frameSize);
      //printf("%f\n", sample[neighbors[j]*hopSize + i]);
      /*if ((neighbors[j] * hopSize + i) < sample.size())
      {
        printf("%f\n", (sample[neighbors[j] * hopSize + i] + add));
        //std::cout << sample[neighbors[j] * hopSize + i] << endl;
      }*/
    }
    //printf("%f\n", fin.back());
    //neighbors.pop_back();
  }

  // write a sequence of lines where each line is a human readable floating
  // point number.
  //
  /*for (int i = 0; i < sample.size(); i++) {
    printf("%f\n", sample[i]);
  }*/
}
