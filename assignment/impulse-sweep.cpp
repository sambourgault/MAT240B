#include "everything.h"
using namespace diy;

/* OBSERVATIONS

Using Adobe Audition, I observes that the first tenth of second contains only one frequency and therefore 
the signal simply looks like a sine wave in the ttime-domain and one peak in the frequency-domain. This is
expected since the midi value 127 is around 12500 Hz which means that the first-order partial falls above 
the Nyquist frequency and therefore not considerd in the signal value. As the time passes, the fundamental
frequency becomes smaller, which allows for an increasing number of partials that don't fall above the 
Nyquist. I notices that the signal becomes gradually an impulse train and that the number of peaks in 
the frequency-domain increases as well. The spectrogram shows also the increasing number of present 
frequencies. One thing I don't quite understand is that the spectrogram shows in a very shaded tone the 
presence of other non-partial frequencies. I wonder what it can be. When the signal impulse reaches a 
fundamental frequency closer to midi 0 (8.18 Hz). The frequency-domain graph does not show well define 
peaks, which might be because the fundamental and its partials are to close to each other to see the
definition of each peak.

*/

int main(int argc, char* argv[]) {
  double phase = 0;
  int index = 0;
  float wave = 0;

  for (float note = 127; note > 0; note -= 0.001) {
    float frequency = mtof(note);

    // adding up as much partial possible without going over the Nyquist frequency
    while(index*frequency < SAMPLE_RATE/2.0){
      // fourier series for an impulse wave
      wave += sin(phase*index);
      index++;
    }

    // making sure each partial is equally weighted
    wave /= (index-1);

    // output a float
    say(wave * 0.1f);
    
    // update the phase
    phase += 2 * pi * frequency / SAMPLE_RATE;
      if (phase > 2 * pi) phase -= 2 * pi;

    index = 0;
    wave = 0;
  }
}
