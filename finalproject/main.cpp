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

typedef mlpack::neighbor::NeighborSearch<  //
    mlpack::neighbor::NearestNeighborSort, //
    mlpack::metric::EuclideanDistance,
    arma::mat,              //
    mlpack::tree::BallTree> //
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
const int frameSize = 1024;
const int hopSize = frameSize / 4;

static Gist<float> gist(frameSize, sampleRate);

struct Appp : App
{
  Parameter p1{"/p1", "", 0.5, "", 0.0, 1.0};
  Parameter p2{"/p2", "", 0.5, "", 0.0, 1.0};
  Parameter p3{"/p3", "", 0.5, "", 0.0, 1.0};
  Parameter radius{"/radius", "", 0.5, "", 0.0, 1.0};
  ParameterBool mic{"mic", "", 1.0};
  ControlGUI gui;

  vector<float> sample;          // contain all sample data
  vector<FeatureVector> feature; //
  vector<int> neighbors;
  arma::mat dataset;

  HashSpace space;
  Mesh mesh;
  Mesh line;

  Appp(int argc, char *argv[])
  {
    //  arma::mat dataset(3, 1000, arma::fill::randu);

    std::string filename = argv[1];
    filename += ".meta.csv";
    mlpack::data::Load(filename, dataset, true);
    cout << "dataset size: " << dataset.size() << endl;

    //myknn(dataset);

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
    gui << p1 << p2 << p3 << radius << mic;
    gui.init();
    navControl().useMouse(false);

    for (int n = 0; n + frameSize < sample.size(); n += frameSize)
    {
      gist.processAudioFrame(&sample[n], frameSize);
      FeatureVector v(f1(), f2(), f3(), f4(), f5());
      // Vec3f v(f1(), f2(), f3());
      /*if (v.x > maximum.x) maximum.x = v.x;
      if (v.y > maximum.y) maximum.y = v.y;
      if (v.z > maximum.z) maximum.z = v.z;
      if (v.x < minimum.x) minimum.x = v.x;
      if (v.y < minimum.y) minimum.y = v.y;
      if (v.z < minimum.z) minimum.z = v.z;*/
      feature.push_back(v);
    }

    line.primitive(Mesh::LINES);
    line.vertex(0, 0, 0);
    line.vertex(1, 1, 1);

    //space = HashSpace(6, feature.size());

    //float dim = space.dim();

    mesh.primitive(Mesh::POINTS);

    /*for (int n = 0; n < feature.size(); n++) {
      feature[n].x = (feature[n].x - minimum.x) / (maximum.x - minimum.x);
      feature[n].y = (feature[n].y - minimum.y) / (maximum.y - minimum.y);
      feature[n].z = (feature[n].z - minimum.z) / (maximum.z - minimum.z);
      mesh.vertex(feature[n]);
      feature[n] *= dim;
      space.move(n, feature[n].x, feature[n].y, feature[n].z);
    }*/
  }

  // doesn't start until App::start() is called
  //
  void onSound(AudioIOData &io) override
  {
    FeatureVector v;
    if (mic)
    {
      gist.processAudioFrame(io.inBuffer(0), frameSize);
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
      myKNN myknn(dataset);
      myknn.Search(query.t(), 1, neighbors, distances);
      //cout << neighbors[0] << endl;

      float *frame = &sample[neighbors[0] * frameSize];
      for (int i = 0; i < frameSize; i++) //
        io.outBuffer(0)[i] = frame[i];
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