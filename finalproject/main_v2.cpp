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
    mlpack::metric::EuclideanDistance,
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
  float f1, f2, f3, f4, f5;

  FeatureVector()
  {
    f1 = 0.0f;
    f2 = 0.0f;
    f3 = 0.0f;
    f4 = 0.0f;
    f5 = 0.0f;
  }

  FeatureVector(float _f1, float _f2, float _f3, float _f4, float _f5)
  {
    f1 = _f1;
    f2 = _f2;
    f3 = _f3;
    f4 = _f4;
    f5 = _f5;
  }

  void set(float _f1, float _f2, float _f3, float _f4, float _f5)
  {
    f1 = _f1;
    f2 = _f2;
    f3 = _f3;
    f4 = _f4;
    f5 = _f5;
  }
};

// UTILS functions ---------------
//
// midi to frequency function
float mtof(float m) { return 8.175799f * powf(2.0f, m / 12.0f); }

// amplitude to dB converting function
float atodb(float a) { return 20.0f * log10f(a / 1.0f); }
//--------------------------------

const int sampleRate = 44100;
const int frameSize = 4096;
const int hopSize = frameSize / 2;

static Gist<float> gist(frameSize, sampleRate);

struct Appp : App
{
  Parameter p1{"/p1", "", 0.5, "", 0.0, 1.0};
  Parameter p2{"/p2", "", 0.5, "", 0.0, 1.0};
  Parameter p3{"/p3", "", 0.5, "", 0.0, 1.0};
  Parameter radius{"/radius", "", 0.5, "", 0.0, 1.0};
  ParameterBool mic{"mic", "", 1.0};
  ControlGUI gui;

  myKNN myknn;
  vector<float> sample; // contain all sample data
  vector<float> input;
  vector<FeatureVector> feature; //
  vector<int> neighbors;
  vector<int> previousNeighbors;
  arma::mat dataset;

  //buffer1.resize(1024);
  vector<float> sample1;
  vector<float> sample2;
  int readIndex = 0;
  int bufferIndex = 0;
  int sampleIndex = 0;

  //HashSpace space;
  Mesh mesh;
  Mesh line;

  Appp(int argc, char *argv[])
  {
    // loading metadata of the corpus
    std::string filename = argv[1];
    filename += ".meta.csv";
    mlpack::data::Load(filename, dataset, true);
    cout << "dataset size: " << dataset.size() << endl;

    // putting soundfile (corpus) in sample vector
    for (int i = 2; i < argc; i++)
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
  }

  // choose between features here:
  //
  float f1() { return gist.rootMeanSquare(); }
  float f2() { return gist.peakEnergy(); }
  float f3() { return gist.zeroCrossingRate(); }
  float f4() { return gist.spectralCentroid(); }
  float f5() { return gist.pitch(); }
  float f6() { return gist.complexSpectralDifference(); }

  Vec3f minimum{1e30f}, maximum{-1e30f};

  void onCreate() override
  {
    input.resize(4096, 0.0f);
    sample1.resize(4096, 0.0f);
    sample2.resize(4096, 0.0f);

    gui << p1 << p2 << p3 << radius << mic;
    gui.init();
    navControl().useMouse(true);

    // link knn to dataset
    myknn.Train(dataset);

    for (int n = 0; n + frameSize < sample.size(); n += frameSize)
    {
      gist.processAudioFrame(&sample[n], frameSize);
      FeatureVector v(f1(), f2(), f3(), f4(), f5());
      Vec3f vv(f1(), f2(), f3());
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
    }
  }

  // doesn't start until App::start() is called
  //
  void onSound(AudioIOData &io) override
  {
    if (mic)
    {
      // get input buffer every frameSize
      for (int i = 0; i < frameSize; i++)
      {
        input[i] = io.inBuffer(0)[i];
      }

      // gist for input buffer
      gist.processAudioFrame(input);

      //gist.processAudioFrame(io.inBuffer(0), frameSize);
      arma::mat query = {f1(), f2(), f3(), f4(), f5()};
      //v.set((f1() - minimum.x) / (maximum.x - minimum.x),
      // (f2() - minimum.y) / (maximum.y - minimum.y),
      // (f3() - minimum.z) / (maximum.z - minimum.z));
      //v.set(f1(), f2(), f3(), f4(), f5());
      // XXX check if v is out of bounds? maybe re-position all points

      arma::mat distances;
      arma::Mat<size_t> neighbors;

      // execute the search
      //
      //myKNN myknn(dataset);
      myknn.Search(query.t(), 1, neighbors, distances);
      cout << neighbors[0] << endl;

      float *frame = &sample[neighbors[0] * frameSize];
      //io.outBuffer(0)[readIndex] = buffer1[readIndex];
      for (int i = 0; i < frameSize; i++)
      {
        //Hann function
        float w_i = 0.5f*(1-cos(2*M_PI*i/frameSize));

        if (sampleIndex == 0)
        {
          
          sample1[i] = w_i*frame[i];
          sampleIndex = 1;
        }
        else
        {
          sample2[i] = w_i*frame[i];
          sampleIndex = 0;
        }
      }

      for (int i = 0; i < frameSize; i++)
      { //
        if (sampleIndex == 0)
        {
          io.outBuffer(0)[i] = sample1[i+hopSize] + (sample1[i]+sample2[i])/2 + sample2[i]//frame[i];
        }
        else if (sampleIndex == 1)
        {
          io.outBuffer(0)[i] = sample2[i+hopSize] + (sample1[i]+sample2[i])/2 + sample1[i]//frame[i];
        }
      }

      return;
    }
    else
    {
      //v.set(p1, p2, p3);
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
    g.draw(mesh);
    g.draw(line);
    gui.draw(g);
  }
};

int main(int argc, char *argv[])
{

  Appp app(argc, argv); // blocks until contructor is complete
  app.audioDomain()->configure(44100, frameSize, 2, 2);
  app.start(); // blocks; hand over control to the framework
}
