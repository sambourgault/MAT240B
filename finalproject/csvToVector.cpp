#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// reverse a sound clip
//
// - takes lines of floats
// - gives lines of floats
//
// use it like this:
//
//   ./read.exe something.wav | ./reverse.exe | ./write.exe reversed.wav
//
//

int main(int argc, char *argv[])
{
  // read in a sequence of lines where each line is a human readable floating
  // point number.
  //
  std::string filename = argv[1];
  std::ifstream file(filename);
  // std::cout << "heyyy" << std::endl;
  std::vector<float> test;
  std::string line;
  getline(file, line);
  cout << line << endl;

  // Iterate through each line and split the content using delimeter
  while (line != "")
  {
    float f = stof(line);
    //std::cout << f << std::endl;
    getline(file, line);
    test.push_back(f);
  }
  // Close the File
  file.close();

  /*string line;
  getline(cin, line);
  vector<float> sample;
  while (line != "") {
    float f = stof(line);
    getline(cin, line);
    sample.push_back(f);
  }*/

  // write a sequence of lines where each line is a human readable floating
  // point number.
  //
  for (int i = 0; i < test.size(); i++)
  {
    printf("%f\n", test[i]);
    //test.pop_front();
  }
}