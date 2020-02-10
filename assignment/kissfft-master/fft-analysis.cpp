#include "kiss_fft.h"
#include <cmath>
#include <complex>
#include <vector>
#include <iostream>
//#include "Gamma-master/Gamma/SamplePlayer.h"


using namespace std;
//using namespace gam;

/* Write a C++ program that uses the STFT to analyze a sound clip, reporting statistics for each 
time-domain window. Choose tentative values for window size, "hop" size, and FFT size (because of zero 
paddings). For each window, compute the [Spectral centroid][] and collect the top 8 spectral peaks 
(frequencies and magnitudes). Report these values as lines on the standard out so they are human-readable, 
but easy to parse with a regex. Also, calculate the RMS and zero-crossing rate for each window. 
For instance:

> int samplenumber, float spectralCentroid, float RMS, float ZCR, 
hz:db hz:db hz:db hz:db hz:db hz:db hz:db hz:db
*/

int main()
{
  

/*
  if (wavFile == NULL)
  {
    printf("Cannot open wave file\n");
  }
  else
  {
    cout << "yoooooooooo" << endl;
  }

  // obtain file size:
  fseek(wavFile, 0, SEEK_END);
  fileSize = ftell(wavFile);
  rewind(wavFile);

  // allocate memory to contain the whole file:
  float* buffer = (float*)malloc(sizeof(float) * fileSize);
  if (buffer == NULL){
    cout << "buffer is null" << endl;
  }

  // copy the file into the buffer:
  fread(buffer, sizeof(float), fileSize, wavFile);

  for (int i = 0;i < fileSize/sizeof(float); i++){
    cout << buffer[i] << endl;
  }


  // terminate
  fclose(wavFile);

  cout << "size (byte) " << fileSize << endl;
  cout << "size float (byte) " << sizeof(float)<< endl;*/


    // read in a sequence of lines where each line is a human readable floating
  // point number.
  //
  string line;
  getline(cin, line);
  vector<float> sample;
  while (line != "") {
    float f = stof(line);
    getline(cin, line);
    sample.push_back(f);
    cout << f << endl;
  }

  // write a sequence of lines where each line is a human readable floating
  // point number.
  //
  /*while (sample.size() > 0) {
    printf("%f\n", sample.back());
    sample.pop_back();
  }*/



  /*const int N = fileSize/sizeof(float);
  //const int N = 1000;
  const int M = N / 2 + 1;

  kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, 0, 0);
  kiss_fft_cpx *cx_in = new kiss_fft_cpx[N];
  kiss_fft_cpx *cx_out = new kiss_fft_cpx[M];

  for (int k = 0; k < N; k++)
  {
    //... // put kth sample in cx_in[k].r and cx_in[k].i
    cx_in[k].r = buffer[k];
   // cout << cx_in[k].r << endl;
    //0.707 * sin(M_PI * 2 * k / N);
    cx_in[k].i = 0.0;
  }

  kiss_fft(cfg, cx_in, cx_out);

  for (int i = 0; i < M; i++)
  {
    //... // transformed. DC is in cx_out[0].r and cx_out[0].i
    cout << "out " << i << " " << cx_out[i].r << "+i" << cx_out[i].i << endl;
  }

  //cout << "sample number: " << N << ", spectral centroid: " << endl;

  kiss_fft_free(cfg);
  free(buffer);
  delete[] cx_in;
  delete[] cx_out;*/
}
