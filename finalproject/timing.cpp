// Karl Yerkes / 2020-02-20
//
// Example of using mlpack to do NN search of random data
//
// build like this:
//
//   c++ -std=c++11 use-mlpack.cpp -lmlpack -larmadillo
//
#include <iostream>
#include <fstream>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>
#include <time.h>
#include <chrono>

typedef mlpack::neighbor::NeighborSearch<  //
    mlpack::neighbor::NearestNeighborSort, //
    mlpack::metric::EuclideanDistance,     //
    arma::mat,                             //
    mlpack::tree::BallTree>                //
    MyKNN;

typedef mlpack::neighbor::NeighborSearch<  //
    mlpack::neighbor::NearestNeighborSort, //
    mlpack::metric::EuclideanDistance,     //
    arma::mat,                             //
    mlpack::tree::KDTree>                  //
    MyKNN2;

typedef mlpack::neighbor::NeighborSearch<  //
    mlpack::neighbor::NearestNeighborSort, //
    mlpack::metric::EuclideanDistance,     //
    arma::mat,                             //
    mlpack::tree::RStarTree>               //
    MyKNN3;

typedef mlpack::neighbor::NeighborSearch<  //
    mlpack::neighbor::NearestNeighborSort, //
    mlpack::metric::EuclideanDistance,     //
    arma::mat,                             //
    mlpack::tree::RTree>               //
    MyKNN4;

typedef mlpack::neighbor::NeighborSearch<  //
    mlpack::neighbor::NearestNeighborSort, //
    mlpack::metric::EuclideanDistance,     //
    arma::mat,                             //
    mlpack::tree::StandardCoverTree>               //
    MyKNN5;

// tree types:
// mlpack::tree::BallTree  
// mlpack::tree::KDTree
// mlpack::tree::RStarTree
// mlpack::tree::RTree
// mlpack::tree::StandardCoverTree

/* Karl's functor */
auto tic() { return std::chrono::high_resolution_clock::now(); }
double toc(std::chrono::high_resolution_clock::time_point then)
{
    return std::chrono::duration<double>(tic() - then).count();
}

int main()
{
    //std::cout << "yooooo" << std::endl;

    std::ofstream timing;
    timing.open("timing.csv");

    // repetitions
    int N = 100;
    double averageBuildIndex = 0.0f;
    double averageSearch = 0.0f;
    std::string structure;

    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < N; i++)
        {

            // make up some random data
            //
            arma::mat dataset(3, 1000, arma::fill::randu);

            // make up some data not in the corpus above
            //
            arma::mat query(3, 1, arma::fill::randu);
            //std::cout << query << std::endl;

            // empty; filled in by the search
            //
            arma::mat distances;
            arma::Mat<size_t> neighbors;

            // execute the search
            //
            if (j == 0)
            {
                structure = "ballTree";

                auto t0 = tic();
                MyKNN myknn(dataset);
                averageBuildIndex += toc(t0);

                auto t1 = tic();
                myknn.Search(query, 1, neighbors, distances);
                averageSearch += toc(t1);
            }
            else if (j == 1)
            {
                structure = "kdTree";


                auto t0 = tic();
                MyKNN2 myknn(dataset);
                averageBuildIndex += toc(t0);

                auto t1 = tic();
                myknn.Search(query, 1, neighbors, distances);
                averageSearch += toc(t1);
            }
            else if (j == 2)
            {
                structure = "rStarTree";

                auto t0 = tic();
                MyKNN3 myknn(dataset);
                averageBuildIndex += toc(t0);

                auto t1 = tic();
                myknn.Search(query, 1, neighbors, distances);
                averageSearch += toc(t1);
            } else if (j == 3)
            {
                structure = "rTree";
                auto t0 = tic();
                MyKNN4 myknn(dataset);
                averageBuildIndex += toc(t0);

                auto t1 = tic();
                myknn.Search(query, 1, neighbors, distances);
                averageSearch += toc(t1);
            }  else if (j == 4)
            {
                structure = "standardTree";
                auto t0 = tic();
                MyKNN5 myknn(dataset);
                averageBuildIndex += toc(t0);

                auto t1 = tic();
                myknn.Search(query, 1, neighbors, distances);
                averageSearch += toc(t1);
            }

            //std::cout << "return " << neighbors.n_elem << std::endl;

            //
            /* for (size_t i = 0; i < neighbors.n_elem; ++i)
        {
            std::cout << neighbors[i] << " " << distances[i] << std::endl;
        }*/
        }

        averageBuildIndex /= N;
        averageSearch /= N;

        timing << structure << ',' << averageBuildIndex << ',' << averageSearch << std::endl;
    }
}
