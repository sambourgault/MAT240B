#include "al/app/al_App.hpp"
#include "al/sound/al_SoundFile.hpp"
#include "al/spatial/al_HashSpace.hpp"
#include "al/ui/al_ControlGUI.hpp" // gui.draw(g)
#include "al/types/al_SingleRWRingBuffer.hpp"

using namespace al;

#include "Gist.h"

#include <vector>
#include <mlpack/core.hpp>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>

using namespace std;

using namespace mlpack;
using namespace mlpack::neighbor; // NeighborSearch and NearestNeighborSort
using namespace mlpack::metric;   // ManhattanDistance

#define _USE_MATH_DEFINES

typedef mlpack::neighbor::NeighborSearch<  //
    mlpack::neighbor::NearestNeighborSort, //
    mlpack::metric::ManhattanDistance,
    arma::mat,            //
    mlpack::tree::KDTree> //
    myKNN;

// tree types:
// mlpack::tree::BallTree
// mlpack::tree::KDTree
// mlpack::tree::RStarTree
// mlpack::tree::RTree
// mlpack::tree::StandardCoverTree

struct FeatureVector
{
  float f1, f2, f3, f4, f5, f6, f7;

  FeatureVector()
  {
    f1 = 0.0f;
    f2 = 0.0f;
    f3 = 0.0f;
    f4 = 0.0f;
    f5 = 0.0f;
    f6 = 0.0f;
    f7 = 0.0f;
  }

  FeatureVector(float _f1, float _f2, float _f3, float _f4, float _f5, float _f6, float _f7)
  {
    f1 = _f1;
    f2 = _f2;
    f3 = _f3;
    f4 = _f4;
    f5 = _f5;
    f6 = _f6;
    f7 = _f7;
  }

  void set(float _f1, float _f2, float _f3, float _f4, float _f5, float _f6, float _f7)
  {
    f1 = _f1;
    f2 = _f2;
    f3 = _f3;
    f4 = _f4;
    f5 = _f5;
    f6 = _f6;
    f7 = _f7;
  }
};

const int sampleRate = 44100;
const int frameSize = 2048;
const int hopSize = frameSize / 2;

// UTILS functions ---------------
//
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
float findHighestPeak(const std::vector<float> &spectrum)
{
  vector<int> a = findPeakIndex(spectrum);
  float max = -1000.0f;
  int position = -1;
  for (int i = 0; i < a.size(); i++)
  {
    if (spectrum[i] > max)
    {
      max = spectrum[i];
      position = i;
    }
  }
  if (position == -1)
  {
    return 0;
  }
  return position * sampleRate / (2.0f * spectrum.size());
}
//--------------------------------

static Gist<float> gist(frameSize, sampleRate);

struct Appp : App
{
  // csv file for test purpose
  std::ofstream test;

  // gui
  Parameter minEnergyPeak{"minEnergyPeak", "", 0.01, "", 0.0, 0.2};
  Parameter amplifier{"amplifier", "", 1.0, "", 0.0, 20.0};
  ParameterBool mic{"mic", "", 0.0};
  ControlGUI gui;

  //graphics
  const size_t bufferSize = 8192;
  float bufferData[8192];
  double phase = 0;
  // Create ring buffer with size 2048
  SingleRWRingBuffer ringBuffer{bufferSize * sizeof(float)};
  Mesh curve;
  Mesh line;
  deque<float> vertexBuffer;

  // Nearest neighbors search variables
  myKNN myknn;
  vector<float> sample;     // contain all sample data
  vector<float> soundQuery; // contain all sound query data
  vector<float> soundQueryFrame;
  int soundQueryIndex = 0;
  vector<float> input;
  vector<FeatureVector> feature;
  vector<int> neighbors;
  vector<int> previousNeighbors;
  arma::mat dataset;

  //buffer1.resize(1024);
  //vector<float> sample1;
  //vector<float> sample2;
  vector<float> hann;
  vector<float> hann2;
  float maxDist = 200.0f;
  int readIndex = 0;
  int bufferIndex = 0;
  int sampleIndex = 0;

  deque<int> neighborPos;
  deque<float> energyPeaks;

  Appp(int argc, char *argv[])
  {
    cout << "hello" << endl;
    cout << argc << endl;

    // open test.csv file to save output data
    // to be able to look at the signal generated afterward
    // thi is use to debug only
    test.open("test.csv");

    // load the data (corpus) metafile
    std::string filename = argv[1];
    filename += ".meta.csv";
    mlpack::data::Load(filename, dataset, true);

    // put soundfile (corpus) in sample vector
    for (int i = 2; i < 3; i++)
    {
      SoundFile soundFile;
      if (!soundFile.open(argv[i])) //
        exit(1);
      if (soundFile.channels > 1) //
        exit(1);
      for (int i = 0; i < soundFile.data.size(); i++) //
        sample.push_back(soundFile.data[i]);
    }

    // exit the program if the sample size is smaller than a framesize
    if (sample.size() < frameSize) //
      exit(1);

    // put soundfile (query) in a soundQuery vector
    // this is used when the mic is off for testing purposes mainly
    if (argc > 2){
    for (int i = 3; i < 4; i++)
    {
      SoundFile soundFile;
      if (!soundFile.open(argv[i])) //
        exit(1);
      if (soundFile.channels > 1) //
        exit(1);
      for (int i = 0; i < soundFile.data.size(); i++){
        soundQuery.push_back(soundFile.data[i]);
        //test << soundQuery[i] << endl;
      }
    }

    if (soundQuery.size() < frameSize) //
      exit(1);
  }
  }


  // functions that calculate features
  //
  float rms() { return gist.rootMeanSquare(); }
  float peakEnergy() { return gist.peakEnergy(); }
  float zcr() { return gist.zeroCrossingRate(); }
  float energyDifference() { return gist.energyDifference(); }
  float spectralCentroid() { return gist.spectralCentroid(); }
  float spectralDifference() { return gist.spectralDifference(); }
  float cSpectralDifference() { return gist.complexSpectralDifference(); }
  float highFrequencyContent() { return gist.highFrequencyContent(); }
  float pitch() { return gist.pitch(); }
  float highPeak()
  {
    const std::vector<float> &magSpec = gist.getMagnitudeSpectrum();
    return findHighestPeak(magSpec);
  }

  void onCreate() override
  {
    line.primitive(Mesh::LINE_STRIP);
    line.vertex(0, 0, 0);
    line.vertex(1, 1, 0);
    // resize vectors to framesize
    //input.resize(frameSize, 0.0f);
    hann.resize(frameSize, 0.0f);
    hann2.resize(2 * frameSize, 0.0f); // hann function - twice as big as framesize
    soundQueryFrame.resize(frameSize, 0.0f);

    // fillup hann function vectors
    for (int i = 0; i < hann.size(); i++)
    {
      hann[i] = 0.5f * (1 - cos(2 * M_PI * i / frameSize));
    }

    for (int i = 0; i < hann2.size(); i++)
    {
      hann2[i] = 0.5f * (1 - cos(2 * M_PI * i / (2 * frameSize)));
    }

    // initialize gui
    gui << minEnergyPeak << amplifier << mic;
    gui.init();
    //navControl().useMouse(true);
    nav().pos(0, 0, 1);

    // link knn to dataset
    myknn.Train(dataset);
  }

  void onSound(AudioIOData &io) override
  {
    float sum = 0;
    input.clear();

    // add input from mic to input vector
    while (io())
    {
      float f = io.in(0) + io.in(1);
      sum += f > 0 ? f : -f;
      input.push_back(f / 2);
      //io.out(0) = io.out(1) = f / 2;
    }

    if (mic)
    {
      // gist for input buffer
      gist.processAudioFrame(input);

      arma::mat query = {rms(), peakEnergy(), zcr(), energyDifference(), spectralDifference(), highFrequencyContent(), highPeak()};
      arma::mat distances;
      arma::Mat<size_t> neighbors;

      // execute the search
      //
      myknn.Search(query.t(), 1, neighbors, distances);
      //float *frame = &sample[neighbors[0] * hopSize];

      // keep track of previous neighbors and peak energy values
      //
      neighborPos.push_back(neighbors[0] * hopSize);
      energyPeaks.push_back(query[1]);

      //cout << query[1] << endl;

      if (neighborPos.size() > 2)
      {
        neighborPos.pop_front();
        energyPeaks.pop_front();
      }

      // smooth value with the value of the previous sample
      //
      float out[2];
      for (int i = 0; i < frameSize; i++)
      {
        float value = 0.0f;

        for (int j = 0; j < neighborPos.size(); j++)
        {
          //cout << energyPeaks[j] << endl;
          if (energyPeaks[j] > minEnergyPeak)
          {
            value += hann2[i + (j * frameSize)] * sample[neighborPos[j] + i + (j * frameSize)];
            //cout << value << endl;
          }
          else
          {

            value += 0.0f;
          }
        }

        // values to be ploted
        out[0] = i;
        out[1] = value;

        io.outBuffer(0)[i] = value;
        io.outBuffer(1)[i] = input[i];

        //cout << out[0] <<' ' << out[1] << endl;
        // Write the waveforms to the ring buffer. (from Putnam audiotoGraphics.cpp)
        ringBuffer.write((const char *)out, 2 * sizeof(float));
      }
    }
    else
    {
      // if mic is off, use sound file argv[1] saved in soundQuery vector
      //
      /*for (int i = 0; i < frameSize; i++)
      {
        io.outBuffer(0)[i] = 0.0f;
        io.outBuffer(1)[i] = 0.0f;
      }*/

      // get soundfile buffer every frameSize
      for (int i = 0; i < frameSize; i++)
      {
        soundQueryFrame[i] = soundQuery[soundQueryIndex];
        soundQueryIndex++;

        if (soundQueryIndex > soundQuery.size())
        {
          // exit when the soundfile has been processed entirely
          exit(1);
          soundQueryIndex = 0;
        }
      }

      // gist for soundfile buffer
      gist.processAudioFrame(soundQueryFrame);
     
      arma::mat query = {rms(), peakEnergy(), zcr(), energyDifference(), spectralDifference(), highFrequencyContent(), highPeak()};
      arma::mat distances;
      arma::Mat<size_t> neighbors;

      // execute the search
      //
      myknn.Search(query.t(), 1, neighbors, distances);

      // the rest is the same as live input
      //

      // keep track of previous neighbors and peak energy values
      //
      neighborPos.push_back(neighbors[0] * hopSize);
      energyPeaks.push_back(query[1]);

      //cout << query[1] << endl;

      if (neighborPos.size() > 2)
      {
        neighborPos.pop_front();
        energyPeaks.pop_front();
      }

      // smooth value with the value of the previous sample
      //
      float out[2];
      for (int i = 0; i < frameSize; i++)
      {
        float value = 0.0f;

        for (int j = 0; j < neighborPos.size(); j++)
        {
          //cout << energyPeaks[j] << endl;
          if (energyPeaks[j] > minEnergyPeak)
          {
            value += hann2[i + (j * frameSize)] * sample[neighborPos[j] + i + (j * frameSize)];
          }
          else
          {
            value += 0.0f;
          }
        }

        // values to be ploted
        out[0] = i;
        out[1] = value;
        //test << value << endl;
        
        //test << soundQuery[i] << endl;

        io.outBuffer(0)[i] = value;
        io.outBuffer(1)[i] = soundQuery[(soundQueryIndex - frameSize) + i];

        // Write the waveforms to the ring buffer. (from Putnam audiotoGraphics.cpp)
        ringBuffer.write((const char *)out, 2 * sizeof(float));
      }
    }
  }

  // from Lance Putnam audiotoGraphics.cpp
  //
  void onAnimate(double dt) override
  {
    curve.primitive(Mesh::LINE_STRIP);
    curve.reset();

    size_t samplesRead = ringBuffer.read((char *)bufferData, bufferSize * sizeof(float));

    // Now we read samples from the buffer into the meash to be displayed
    for (size_t i = 0; i < samplesRead / sizeof(float); i = i + 2)
    {
      vertexBuffer.push_back(bufferData[i + 1]);
      //cout << bufferData[i]/frameSize << ' ' <<  100*bufferData[i+1] << endl;
      //curve.vertex((bufferData[i]/frameSize)-(0.5f), bufferData[i+1]);
      // The redder the lines, the closer we are to a full ring buffer
      //curve.color(HSV(0.5 *float(bufferSize)/(bufferSize - i)));
      //curve.color(HSV(1.0f));
      //cout << vertexBuffer.size() << endl;
      if (vertexBuffer.size() > (32 * frameSize))
      {
        vertexBuffer.pop_front();
      }
    }

    for (int i = 0; i < vertexBuffer.size(); i++)
    {
      curve.vertex((float)i / (0.5f * vertexBuffer.size()) - 1.0f, vertexBuffer[i]);
      curve.color(RGB(1.0));
    }
  }

  void onDraw(Graphics &g) override
  {
    g.clear(Color(0.21));
    g.meshColor();
    g.draw(curve);
    //g.draw(line);
    gui.draw(g);
  }
};

int main(int argc, char *argv[])
{

  /*for (int i = 0; i < AudioDevice::numDevices(); i++)
  {
    printf(" --- [%2d] ", i);
    AudioDevice dev(i);
    dev.print();
  }*/

  // Create an "Aggregate Audio Device"
  //
  // - Start the "Audio MIDI Setup" app
  // - Click + in the bottom left corner and choose "Create Aggregate Device"
  // - Choose the in/out devices in the area on the right
  // - Rename the new aggregate device something meaningful
  // - Run this program to see *exactly* what that name is
  // - Use that whole name in the AudioDevice constructor like we see below
  //
  AudioDevice aggregate = AudioDevice("Apple Inc.: H4Headphones");
  //aggregate.print();

  // sample rate, buffer size, nb out, nb in;
  Appp app(argc, argv); // blocks until contructor is complete
  app.audioDomain()->configure(aggregate, 44100, frameSize, 2, 2);
  //app.configureAudio(AudioDevice("Apple Inc.: H4Headphones"), 44100, frameSize, 2, 2);
  //app.audioDomain()->audioIO().print();
  app.start(); // blocks; hand over control to the framework
  return 0;
}
