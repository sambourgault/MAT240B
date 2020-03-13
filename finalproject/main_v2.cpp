#include "al/app/al_App.hpp"
#include "al/sound/al_SoundFile.hpp"
#include "al/spatial/al_HashSpace.hpp"
#include "al/ui/al_ControlGUI.hpp" // gui.draw(g)
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
  Parameter p1{"/p1", "", 0.5, "", 0.0, 1.0};
  Parameter p2{"/p2", "", 0.5, "", 0.0, 1.0};
  Parameter p3{"/p3", "", 0.5, "", 0.0, 1.0};
  Parameter minEnergyPeak{"/minEnergyPeak", "", 0.05, "", 0.0, 0.3};
  Parameter radius{"/radius", "", 0.5, "", 0.0, 1.0};
  ParameterBool mic{"/mic", "", 1.0};
  ControlGUI gui;

  myKNN myknn;
  vector<float> sample; // contain all sample data
  vector<float> soundQuery;
  vector<float> soundQueryFrame;
  int soundQueryIndex = 0;
  vector<float> input;
  vector<FeatureVector> feature; //
  vector<int> neighbors;
  vector<int> previousNeighbors;
  arma::mat dataset;

  //buffer1.resize(1024);
  vector<float> sample1;
  vector<float> sample2;
  vector<float> hann;
  vector<float> hann2;
  float maxDist = 200.0f;
  int readIndex = 0;
  int bufferIndex = 0;
  int sampleIndex = 0;

  deque<int> neighborPos;
  deque<int> energyPeaks;

  //HashSpace space;
  Mesh mesh;
  Mesh line;

  std::ofstream test;

  Appp(int argc, char *argv[])
  {
    test.open("test.csv");

    // loading metadata of the corpus
    std::string filename = argv[1];
    filename += ".meta.csv";
    mlpack::data::Load(filename, dataset, true);
    //cout << "dataset size: " << dataset.size() << endl;

    // putting soundfile (corpus) in sample vector
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

    if (sample.size() < frameSize) //
      exit(1);

    for (int i = 3; i < 4; i++)
    {
      SoundFile soundFile;
      if (!soundFile.open(argv[i])) //
        exit(1);
      if (soundFile.channels > 1) //
        exit(1);
      for (int i = 0; i < soundFile.data.size(); i++) //
        soundQuery.push_back(soundFile.data[i]);
    }

    if (soundQuery.size() < frameSize) //
      exit(1);
  }

  // choose between features here:
  //
  float rms() { return gist.rootMeanSquare(); } // yes
  float peakEnergy() { return gist.peakEnergy(); }
  float zcr() { return gist.zeroCrossingRate(); }
  float energyDifference() { return gist.energyDifference(); }
  float spectralCentroid() { return gist.spectralCentroid(); }
  float spectralDifference() { return gist.spectralDifference(); } // yes
  float cSpectralDifference() { return gist.complexSpectralDifference(); }
  float highFrequencyContent() { return gist.highFrequencyContent(); } // yes
  float pitch() { return gist.pitch(); }
  float highPeak()
  {
    const std::vector<float> &magSpec = gist.getMagnitudeSpectrum();
    return findHighestPeak(magSpec);
  } // yes

  Vec3f minimum{1e30f}, maximum{-1e30f};

  void onCreate() override
  {

    input.resize(frameSize, 0.0f);
    sample1.resize(frameSize, 0.0f);
    sample2.resize(frameSize, 0.0f);
    hann.resize(frameSize, 0.0f);
    hann2.resize(2 * frameSize, 0.0f);
    soundQueryFrame.resize(frameSize, 0.0f);

    for (int i = 0; i < hann.size(); i++)
    {
      hann[i] = 0.5f * (1 - cos(2 * M_PI * i / frameSize));
    }

    for (int i = 0; i < hann2.size(); i++)
    {
      hann2[i] = 0.5f * (1 - cos(2 * M_PI * i / (2 * frameSize)));
    }

    gui << p1 << p2 << p3 << minEnergyPeak << radius << mic;
    gui.init();
    navControl().useMouse(true);

    // link knn to dataset
    myknn.Train(dataset);

    /*for (int n = 0; n + frameSize < sample.size(); n += frameSize)
    {
      gist.processAudioFrame(&sample[n], frameSize);
      FeatureVector v(rms(), peakEnergy() , zcr(),energyDifference(), spectralDifference(),highFrequencyContent(), highPeak());
      Vec3f vv(rms(), spectralDifference(), highFrequencyContent());
      if (vv.x > maximum.x)
        maximum.x = vv.x;
      if (vv.y > maximum.y)
        maximum.y = vv.y;
      if (vv.z > maximum.z)
        maximum.z = vv.z;
      if (vv.x < minimum.x)
        minimum.x = vv.x;
      if (vv.y < minimum.y)
        minimum.y = vv.y;
      if (vv.z < minimum.z)
        minimum.z = vv.z;
      feature.push_back(v);
    }

    line.primitive(Mesh::LINES);
    line.vertex(0, 0, 0);
    line.vertex(1, 1, 1);

    //space = HashSpace(6, feature.size());
    //float dim = space.dim();

    mesh.primitive(Mesh::POINTS);

    for (int n = 0; n < feature.size(); n++)
    {
      feature[n].f1 = (feature[n].f1 - minimum.x) / (maximum.x - minimum.x);
      feature[n].f2 = (feature[n].f2 - minimum.y) / (maximum.y - minimum.y);
      feature[n].f3 = (feature[n].f3 - minimum.z) / (maximum.z - minimum.z);
      Vec3f f(feature[n].f1, feature[n].f2, feature[n].f3);
      mesh.vertex(f);
      //feature[n] *= dim;
      //space.move(n, feature[n].x, feature[n].y, feature[n].z);
    }*/
  }

  // doesn't start until App::start() is called
  //
  void onSound(AudioIOData &io) override
  {

    //return;

    if (mic)
    {
      for (int i = 0; i < frameSize; i++)
      {
        //cout << io.in(0) << endl;
        //input[i] = io.in(0);
        input[i] = io.inBuffer(0)[i];
        //cout << input[i] << endl;
        test << input[i] << endl;
      }
      //return;

      // get input buffer every frameSize
      /*for (int i = 0; i < frameSize; i++)
      {
        input[i] = io.inBuffer(0)[i];
      }*/
      //cout << io.inBuffer(0)[0] << endl;

      /*for (int i = 0; i < frameSize; i++){
        io.outBuffer(0)[i] = input[i];
        io.outBuffer(1)[i] = input[i];
      }
      return;*/

      // gist for input buffer
      gist.processAudioFrame(input);

      arma::mat query = {rms(), peakEnergy(), zcr(), energyDifference(), spectralDifference(), highFrequencyContent(), highPeak()};
      arma::mat distances;
      arma::Mat<size_t> neighbors;

      // execute the search
      //
      //myKNN myknn(dataset);
      myknn.Search(query.t(), 1, neighbors, distances);
      //cout << neighbors[0] << endl;

      //float *frame = &sample[neighbors[0] * hopSize];
      neighborPos.push_back(neighbors[0] * hopSize);
      energyPeaks.push_back(query[1]);
      if (neighborPos.size() > 2)
      {
        neighborPos.pop_front();
        energyPeaks.pop_front();
      }

      for (int i = 0; i < frameSize; i++)
      {
        float value = 0.0f;

        for (int j = 0; j < neighborPos.size(); j++)
        {
          if (energyPeaks[j] > minEnergyPeak)
          {
            value += hann2[i + (j * frameSize)] * sample[neighborPos[j] + i + (j * frameSize)];
          }
          else
          {
            value += 0.0f;
          }
        }
        test << (1.0f / neighborPos.size()) * value << endl;
        //io.outBuffer(0)[i] = (1.0f / neighborPos.size()) * value;
        //if(query[1] > minEnergyPeak){
        io.outBuffer(0)[i] = value;
        io.outBuffer(1)[i] = value;
        //} else {
        // io.outBuffer(0)[i] = 0.0f;
        //}
      }

      return;

      //io.outBuffer(0)[readIndex] = buffer1[readIndex];
      /*for (int i = 0; i < frameSize; i++)
      {
        //Hann function
        //float w_i = 0.5f * (1 - cos(2 * M_PI * i / frameSize));

        if (sampleIndex == 0)
        {
          //if (distances[0] < maxDist)
          //{
            sample1[i] = frame[i];
          //}
          //else
          //{
          //  sample1[i] = 0.0f;
          //}
          sampleIndex = 1;
        }
        else
        {
          //if (distances[0] < maxDist)
          //{
            sample2[i] = frame[i];
          //}
          //else
          //{
          //  sample2[i] = 0.0f;
          //}
          sampleIndex = 0;
        }

        if (!sample1.empty() && !sample2.empty())
        {
          for (int i = 0; i < frameSize; i++)
          { //
            //io.outBuffer(0)[i] = sample1[i];
            float value = 0;
            if (sampleIndex == 0)
            {
              if (i < hopSize)
              {
                io.outBuffer(0)[i] = hann[i + hopSize] * sample1[i + hopSize] + hann[i] * (sample1[i] + sample2[i]) / 2;
                value = hann[i + hopSize] * sample1[i + hopSize] + hann[i] * (sample1[i] + sample2[i]) / 2;
              }
              else
              {
                io.outBuffer(0)[i] = hann[i] * (sample1[i] + sample2[i]) / 2 + hann[i - hopSize] * sample2[i];
                value = hann[i] * (sample1[i] + sample2[i]) / 2 + hann[i - hopSize] * sample2[i];
              }
            }
            else if (sampleIndex == 1)
            {
              if (i < hopSize)
              {
                io.outBuffer(0)[i] = sample2[i + hopSize] + (sample1[i] + sample2[i]) / 2;
                value = sample2[i + hopSize] + (sample1[i] + sample2[i]) / 2;
              }
              else
              {
                io.outBuffer(0)[i] = (sample1[i] + sample2[i]) / 2 + sample1[i];
                value = (sample1[i] + sample2[i]) / 2 + sample1[i];
              }
            }

            //printf("%f\n", io.outBuffer(0)[i]);
            //test << value << endl;
          }
        }
      }*/

      return;
    }
    else
    {
      // gist for input buffer
      // get input buffer every frameSize
      for (int i = 0; i < frameSize; i++)
      {
        soundQueryFrame[i] = soundQuery[soundQueryIndex];
        soundQueryIndex++;
        //test << soundQuery[soundQueryIndex] << endl;
        if (soundQueryIndex > soundQuery.size())
        {
          exit(1);
          soundQueryIndex = 0;
        }
      }

      gist.processAudioFrame(soundQueryFrame);

      arma::mat query = {rms(), peakEnergy(), zcr(), energyDifference(), spectralDifference(), highFrequencyContent(), highPeak()};
      arma::mat distances;
      arma::Mat<size_t> neighbors;

      // execute the search
      //
      //myKNN myknn(dataset);
      myknn.Search(query.t(), 1, neighbors, distances);
      //cout << neighbors[0] << endl;

      float *frame = &sample[neighbors[0] * hopSize];

      neighborPos.push_back(neighbors[0] * hopSize);
      energyPeaks.push_back(query[1]);
      if (neighborPos.size() > 2)
      {
        neighborPos.pop_front();
        energyPeaks.pop_front();
      }

      for (int i = 0; i < frameSize; i++)
      {
        float value = 0.0f;

        for (int j = 0; j < neighborPos.size(); j++)
        {
          if (energyPeaks[j] > minEnergyPeak)
          {
            value += hann2[i + (j * frameSize)] * sample[neighborPos[j] + i + (j * frameSize)];
          }
          else
          {
            value += 0.0f;
          }
        }

        test << value << endl;
        //io.outBuffer(0)[i] = (1.0f / neighborPos.size()) * value;
        //if(query[1] > minEnergyPeak){
        io.outBuffer(0)[i] = value;
        io.outBuffer(1)[i] = value;
        //} else {
        // io.outBuffer(0)[i] = 0.0f;
        //}
      }

      return;

      /*for (int i = 0; i < frameSize; i++)
      {
        
        if (distances[0] < maxDist){
        io.outBuffer(0)[i] = hann[i]*frame[i];
        test << frame[i] << endl;
        } else {
          io.outBuffer(0)[i] = 0.0f;
          test << 0.0f << endl;
        }
      }

      return;*/

      for (int i = 0; i < frameSize; i++)
      {
        //Hann function
        //float w_i = 0.5f * (1 - cos(2 * M_PI * i / frameSize));

        if (sampleIndex == 0)
        {
          if (distances[0] < maxDist)
          {
            sample1[i] = frame[i];
          }
          else
          {
            sample1[i] = 0.0f;
          }
          sampleIndex = 1;
        }
        else
        {
          if (distances[0] < maxDist)
          {
            sample2[i] = frame[i];
          }
          else
          {
            sample2[i] = 0.0f;
          }
          sampleIndex = 0;
        }

        /*//test << frame[i] << ',' << distances[0] << endl;
        if (sampleIndex == 0){
         // test << sample2[i] << endl;
          io.outBuffer(0)[i] = sample2[i];
        } else {
         // test << sample1[i] << endl;
          io.outBuffer(0)[i] = sample1[i];
        }
        //io.outBuffer(1)[readIndex] = hann[i]*frame[i];*/
      }

      //return;

      if (!sample1.empty() && !sample2.empty())
      {
        for (int i = 0; i < frameSize; i++)
        { //
          //io.outBuffer(0)[i] = sample1[i];
          float value = 0;
          if (sampleIndex == 0)
          {
            if (i < hopSize)
            {
              io.outBuffer(0)[i] = hann[i + hopSize] * sample1[i + hopSize] + hann[i] * (sample1[i] + sample2[i]) / 2;
              value = hann[i + hopSize] * sample1[i + hopSize] + hann[i] * (sample1[i] + sample2[i]) / 2;
            }
            else
            {
              io.outBuffer(0)[i] = hann[i] * (sample1[i] + sample2[i]) / 2 + hann[i - hopSize] * sample2[i];
              value = hann[i] * (sample1[i] + sample2[i]) / 2 + hann[i - hopSize] * sample2[i];
            }
          }
          else if (sampleIndex == 1)
          {
            if (i < hopSize)
            {
              io.outBuffer(0)[i] = sample2[i + hopSize] + (sample1[i] + sample2[i]) / 2;
              value = sample2[i + hopSize] + (sample1[i] + sample2[i]) / 2;
            }
            else
            {
              io.outBuffer(0)[i] = (sample1[i] + sample2[i]) / 2 + sample1[i];
              value = (sample1[i] + sample2[i]) / 2 + sample1[i];
            }
          }

          //printf("%f\n", io.outBuffer(0)[i]);
          //test << value << endl;
        }
      }

      return;
    }

    // empty; filled in by the search
    //

    /*if (line.vertices().size())  //
      line.vertices()[0] = v;

    v *= space.dim();

    HashSpace::Query query(1);
    if (query(space, v, radius * space.maxRadius())) {
      float* frame = &sample[query[0]->id * frameSize];
      for (int i = 0; i < frameSize; i++)  //
        io.outBuffer(0)[i] = frame[i];
      return;
    }*/

    /*int ratio = frameSize / hopSize;

    for (int j = 0; j < neighbors.size(); j++)
    {

      for (int i = 0; i < hopSize; i++)
      {
        float add = 0.0f;
        for (int k = 0; k < ratio; k++)
        {
          if (j >= k && (neighbors[j - k] * hopSize + i + hopSize) < sample.size())
          {
            // Hamming window
            float w_i = 0.54f - 0.46f * cos(TWO_PI * ((k * hopSize) + i) / frameSize);
            add += w_i * sample[neighbors[j - k] * hopSize + (k * hopSize) + i];
          }
        }

        printf("%f\n", add);
      }
    }*/

    while (io())
    {
      float f = 0;
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onDraw(Graphics &g) override
  {

    g.clear(Color(0.21));
    /* g.draw(mesh);
    g.draw(line);*/
    gui.draw(g);
  }
};

int main(int argc, char *argv[])
{
  // Default devices
  AudioDevice dev = AudioDevice::defaultOutput();
  dev.print();

  AudioDevice dev_in = AudioDevice::defaultInput();
  dev_in.print();

  // You can also specify a device by name:
  AudioDevice dev_name = AudioDevice("Device Name");
  // This is probably an invalid device. This will be printed.
  dev_name.print();

  // You can get a list of available devices with their names
  for (int i = 0; i < AudioDevice::numDevices(); i++)
  {
    printf(" --- [%2d] ", i);
    AudioDevice dev(i);
    dev.print();
  }

  // sample rate, buffer size, nb out, nb in;
  Appp app(argc, argv); // blocks until contructor is complete
  app.audioDomain()->configure(48000, frameSize, 2, 2);
  app.audioDomain()->audioIO().print();
  app.start(); // blocks; hand over control to the framework
}
