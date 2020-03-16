# Final Project for MAT240B
## title
in translation

## description
In translation is a matter/computer/living being performance that investigates potential conversations between a performer (living being), a computational system (computer), and found objects (matter: branches, tree cones, tie wraps, nails, chains, tiny saw, button, plastic bags, etc.).

This project is seen as the first stage of a longer exploration on the theme of matter/digital/living-being sonic interaction. It is an attempt to immerse myself into a better understanding of the potential voices of everyday objects. The new materialist perspective in the humanities appears as a good framework to investigate the sonic and visual poetry hidden in the things that surround us.

## schematics
### installation schematics
The setup is represented as an alchemy experimentation. The performer interacts with the found and animated objects in order to generate sounds on a surface that we will call the *playground*. The sound is picked up by a contact microphone located on the same *playground*. This signal is interpreted as a live query for a concatenative synthesis algorithm. A synthesized version of this initial signal is then sent over a monitor.

![schematics 1](schematics/schema.png)

### digital concatenative synthesis schematics
A schematics of the system used for translating the analog signal is illustrated in the schematics bellow. The digital translator is based on a concatenative synthesis technique. Audio features of a corpus are calculated using [*Gist*](https://github.com/adamstark/Gist). 

The realtime sound target is fed to the system using a contact microphone. The same audio features are extracted from every frame and compared to the corpus in order to determine its nearest neighbor using the [*mlpack*](https://www.mlpack.org/getstarted.html) library. The system, then, replaces the incoming target signal frame by this nearest neighbor frame from the corpus and sends it over the output smonitor.

![schematics 2](schematics/digitalSchema.png)

## how to run it
### download the code and dependencies
- [Allolib playground](https://github.com/AlloSphere-Research-Group/allolib_playground)
- [this repo](https://github.com/sambourgault/MAT240B)
- [mlpack](https://www.mlpack.org/getstarted.html)
    - using `brew install mlpack` (macOS)
- [Gist](https://github.com/adamstark/Gist)

### folders setup
- place the **Gist** folder in the **final project** folder inside **MAT240B** folder on your machine. (replace the existing empty one).
- place the **final project** folder in the **allolib_playground** folder on your machine.

### run run run
- open terminal and go to the **allolib_playground** folder.
- use the command line to make the executable `./run.sh finalproject/main_v3.cpp`
- cd to `finalproject/bin` and run the code using `./main_v3 data ../onTranslationMono.wav ../query_soundfile.wav` 
    - note that `data` correspond to the name of your `.meta.csv` containing the corpus audio features values. `../onTranslationMono.wav` is the corpus file, and `../query_soundfile.wav` is an **optional** argument use to run the code on this file instead of using live input. I use `../soundExplorationMono_01.wav` as my `../query_soundfile.wav` but you can use your own!
- the live input is currently set to be an aggregate of my headphones (2 outs) and my h4n zoom recorder used as an audio interface (2 ins). your setup is probably different then mine. to make this program work, you will have to change the `main_v3.cpp` program `main` function to fit your own setup.
- once the allolib window opens up, use the **energy peak** slider to change the input considered "energetic" enough to be resynthesized.

### use a different corpus?
if you want to use your own corpus, you will have to create a new **data** meta file.
-  currently mycorpus if the file **onTranslationMono.wav** and the **data.meta.csv** contains the audio information associated with that soundfile.
- so... if you want to use your own corpus: in terminal, go to the **finalproject** folder.
- run the command `make` to make **createMetafile.cpp**.
- run the command `./read.exe onTranslationMono.wav | ./createMetafile data`
- copy your new **data.meta.csv** into the **bin** folder `cp data.meta.csv bin/data.meta.csv` 


## notes
- I'm running this code on a MacBook Pro (15 in, 2018) with macOS Mojave (10.14.4).
- I used Visual Studio Code (1.43.0) to write this code.

## license

MIT License

Copyright (c) [2020] [Sam Bourgault]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

