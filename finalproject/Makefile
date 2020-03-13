.SUFFIXES:

CXX=
CXX += c++
CXX += -std=c++11
CXX += -Wall
CXX += -DUSE_KISS_FFT
//CXX += -DUSE_ACCELERATE_FFT 
//CXX += -DUSE_FFTW

SRC=
SRC += Gist/src/pitch/Yin.cpp
SRC += Gist/src/mfcc/MFCC.cpp
SRC += Gist/src/core/CoreFrequencyDomainFeatures.cpp
SRC += Gist/src/core/CoreTimeDomainFeatures.cpp
SRC += Gist/src/onset-detection-functions/OnsetDetectionFunction.cpp
SRC += Gist/src/Gist.cpp
SRC += Gist/src/fft/WindowFunctions.cpp
//SRC += Gist/src/fft/AccelerateFFT.cpp

INC=
INC += -IGist/src/
INC += -IGist/src/core/
INC += -IGist/src/fft/
INC += -IGist/src/mfcc/
INC += -IGist/src/onset-detection-functions/
INC += -IGist/src/pitch/
INC += -IGist/libs/kiss_fft130/ # if you want to use KissFFT

LIB=
//LIB += -framework Accelerate
//LIB += -lfftw3

OBJ = $(patsubst %.cpp,%.o,$(SRC))
OBJ += Gist/libs/kiss_fft130/kiss_fft.o # if you want to use KissFFT

%.o: %.cpp
	$(CXX) $(INC) -c -o $@ $^

%.o: %.c
	$(CXX) $(INC) -c -o $@ $^

use-gist: libgist
	$(CXX) $(LIB) $(INC) createMetafile.cpp libgist.a -o createMetafile
	$(CXX) $(LIB) $(INC) computeKNN.cpp libgist.a -lmlpack -larmadillo -o computeKNN 
	$(CXX) $(LIB) $(INC) concatenateSound.cpp -o concatenateSound 
	$(CXX) $(LIB) $(INC) csvToVector.cpp -o csvToVector 

libgist: $(OBJ)
	ar -rc libgist.a $^

clean:
	rm -f $(OBJ) libgist.a use-gist
