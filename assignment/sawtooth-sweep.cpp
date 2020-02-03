#include "everything.h"
using namespace diy;

/* OBSERVATIONS

Using Adobe Audition to look at the signal generated from this Fourier series, my observations are similar 
to the impulse signal, the difference being that the signal looks more and more like a sawtooth wave as the
partials under the Nyquist frequency are added to the diminishing fundamental frequency.

*/

int main(int argc, char* argv[]) {
  double phase = 0;
  int index = 1;
  float wave = 0;

  for (float note = 127; note > 0; note -= 0.001) {
    float frequency = mtof(note);
    
    // add all the partilas that are under the Nyquist frequency
    while(index*frequency < SAMPLE_RATE/2.0){
      // fourier series for a sawtooth wave
      wave += sin(phase*index)/index;
      index++;
    }

    // output a float
    say(wave * 0.1f);
    // update the phase
    phase += 2 * pi * frequency / SAMPLE_RATE;
    if (phase > 2 * pi) phase -= 2 * pi;

    index = 1;
    wave = 0;
  }
}
