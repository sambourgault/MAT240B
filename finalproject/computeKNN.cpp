#include <cmath>
#include <iostream>
#include <fstream>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

#include "Gist.h"

// midi to frequency function
float mtof(float m) { return 8.175799f * powf(2.0f, m / 12.0f); }

// amplitude to dB converting function
float atodb(float a) { return 20.0f * log10f(a / 1.0f); }

// Karl Yerkes / 2020-02-20
//
// Example of using mlpack to do NN search of random data
//
// build like this:
//
//   c++ -std=c++11 use-mlpack.cpp -lmlpack -larmadillo
//

using namespace mlpack::neighbor;

typedef mlpack::neighbor::NeighborSearch<   //
    mlpack::neighbor::NearestNeighborSort,  //
    mlpack::metric::EuclideanDistance,
    arma::mat,                              //
    mlpack::tree::BallTree>                 //
    MyKNN;

// tree types:
// mlpack::tree::BallTree
// mlpack::tree::KDTree
// mlpack::tree::RStarTree
// mlpack::tree::RTree
// mlpack::tree::StandardCoverTree

int main() {
  // create and open the .csv file
  std::ofstream neighborsInfo;
  neighborsInfo.open("neighbors.meta.csv");


  // from: https://www.mlpack.org/doc/mlpack-3.2.2/doxygen/sample.html


  // make up some random data
  //
  //arma::mat dataset(3, 1000, arma::fill::randu);
  arma::mat dataset;
  mlpack::data::Load("data.meta.csv", dataset, true);
  cout << "dataset size: "  << dataset.size() << endl;

  // make up some data not in the corpus above
  //
  //arma::mat query(3, 50, arma::fill::randu);
  arma::mat query;
  mlpack::data::Load("query.meta.csv", query, true);
  cout << "query size: "  << query.size() << endl;


  // empty; filled in by the search
  //
  arma::mat distances;
  arma::Mat<size_t> neighbors;

  // execute the search
  //
  MyKNN myknn(dataset);
  myknn.Search(query, 1, neighbors, distances);

  cout << "size: " << neighbors.n_elem << endl;
  for (size_t i = 0; i < neighbors.n_elem; ++i) {
    std::cout << neighbors[i] << " " << distances[i] << std::endl;
    neighborsInfo << neighbors[i] << endl;
  }


}
