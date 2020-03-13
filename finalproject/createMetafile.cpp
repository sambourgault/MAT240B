#include <cmath>
#include <iostream>
#include <fstream>


using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

#include "Gist.h"


// parameter for fft
const int sampleRate = 44100;
const int frameSize = 1024;
const int hopSize = frameSize / 2;
int ratio = frameSize/hopSize;

// midi to frequency function
float mtof(float m) { return 8.175799f * powf(2.0f, m / 12.0f); }

// amplitude to dB converting function
float atodb(float a) { return 20.0f * log10f(a / 1.0f); }

//find peaks
vector<int> findPeakIndex(const std::vector<float> &spectrum)
{
  vector<int> peakIndex;
  float threshold = 0.0f;

  // spectrum.size() is 512 (1/4 of 1024: windowsize)
  for (int i = 0; i < spectrum.size(); i++)
  {
    // check if its a max and add to peak vector
    float magnitude = spectrum[i];

    if (i > 0 && i < spectrum.size() - 1 && atodb(magnitude) > threshold)
    {
      // this part sets "considered" to false as soon as the point considered is not a real max.
      bool considered = true;

      for (int j = 1; j < 2; j++)
      {
        // if previous sample is greater, set to false
        if (spectrum[i - j] > magnitude)
        {
          considered = false;
          break;
        }
        // if following sample is greater, set to false
        if (spectrum[i + j] > magnitude)
        {
          considered = false;
          break;
        }
      }

      // if "considered" as reamained true, the point is considered as a peak and added to the peak vector.
      if (considered)
      {
        peakIndex.push_back(i);
      }
    }
  }

  return peakIndex;
}

// find highest peak
float findHighestPeak(const std::vector<float> &spectrum){
  vector<int> a = findPeakIndex(spectrum);
  float max = -1000.0f;
  int position = -1;
  for (int i = 0; i < a.size(); i++){
    if (spectrum[i] > max){
      max = spectrum[i];
      position = i;
    }
  }
  return position * sampleRate / (2.0f * spectrum.size());
}


int main(int argc, char *argv[])
{
  // create and open the .csv file
  std::ofstream metadata;
  std::string filename = argv[1];
  filename += ".meta.csv";
  metadata.open(filename);

  // read in a sequence of lines where each line is a human readable floating
  // point number.
  string line;
  getline(cin, line);
  vector<float> audio;
  int zeroCount = 0;

  while (line != "")
  {
    float f = stof(line);
    if(f == 0.0f){
      //zeroCount++;
    }
    getline(cin, line);
    audio.push_back(f);
    //std::cout << f << std::endl;
  }
  /*cout << "audio size: " << audio.size() << endl;
  cout << "zero: " << zeroCount << endl;
  cout << "ratio: " << zeroCount/audio.size() << endl;*/


  // use Gist to analyze and print a bunch of frames
  //
  Gist<float> gist(frameSize, sampleRate);
  //metadata << "n,RMS,peak,ZCR,centroid" << endl;
  for (int n = 0; n + frameSize < audio.size(); n += hopSize)
  {
    gist.processAudioFrame(&audio[n], frameSize);

    // frequency domain spectrum
    const std::vector<float> &magSpec = gist.getMagnitudeSpectrum();

    //metadata << n                               //
    metadata << gist.rootMeanSquare() 
      << ',' << gist.peakEnergy() 
      << ',' << gist.zeroCrossingRate() 
      << ',' << gist.energyDifference()
      //<< ',' << gist.pitch()
      << ',' << gist.spectralDifference()
      << ',' << gist.highFrequencyContent()
      /*<< ',' << gist.spectralCentroid()
      << ',' << gist.spectralCrest()
      << ',' << gist.spectralFlatness()*/
      << ',' << findHighestPeak(magSpec) << endl;

     /*        << ',' << gist.rootMeanSquare()    //
             << ',' << gist.peakEnergy()        //
             << ',' << gist.zeroCrossingRate()  //
             << ',' << gist.spectralCentroid(); //*/
/*      vector<int> peaks = findPeakIndex(magSpec);
      if (peaks.size()>0){
      metadata << ',' << peaks[0] * sampleRate / (2.0f * magSpec.size());
      } else {
              metadata << ',' << 0.0f;
      }
      if (peaks.size()>1){
      metadata << ',' << peaks[1] * sampleRate / (2.0f * magSpec.size());
      } else {
              metadata << ',' << 0.0f;
      }
      if (peaks.size()>2){
      metadata << ',' << peaks[2] * sampleRate / (2.0f * magSpec.size());
      } else {
              metadata << ',' << 0.0f;
      }
      metadata << endl;*/
  }
  metadata.close();
}
