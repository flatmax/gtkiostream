/*
 libaudiomask - hybrid simultaneous audio masking threshold evaluation library
    Copyright (C) 2000-2018  Dr Matthew Raphael Flax

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
*
* This example shows how to use Dr M.R. Flax's (2000) hybrid
* simultaneous audio masking class to find the masking threshold of a time domain signal.
*
* The compilation of this file is demonstrated in Makefile.
* Run this file : ./AudioMaskerExample
* View the results of this file using www.octave.org by running the script view.m
* - simply type view once octave has started and you are in the suitable directory.
*
* The input audio should be stored in the file INPUTFILENAME in text format - each sample seperated by a * white space.
*
* ========================= HOWTO ===============================
* \code
*     // First find the masking threshold
*     AudioMasker masker(sampleFreq, count); // Create the audio masker class using fs=sampleFreq and count filters
*     masker.excite(input, sampleCount); // find the mask for the array of input data which has sampleCount time samples.
*
*     // Now do something with the masking threshold ...
*
*     // The frequency domain mask is now located here
*     for (int j=0; j<count;j++)
*         masker.mask[j]; // This is the mask at each of the count frequencies of interest
*
*     // A more sophisticated example - find the threshold for each Fourier bin
*     double fact=(double)sampleFreq/((double)sampleCount-1.0); // convert from an index to the equivalent * Fourier bin frequency
*     for (int j=0; j<halfSampleCount;j++){
*         cout<<"finding for freq "<<j*fact<<'\t'; // The frequency we are inspecting
*         double threshold=masker.findThreshold(j*fact); // The masking threshold
*         20*log10(threshold); // The threshold in decibels (dB)
*     }
*
* \endcode
*   // The following example calculates the spectral power and the masking threshold for each Fourier bin of interest ...
*/

#include <math.h>
#include "Octave.H"
#include "AudioMask/AudioMasker.H"

#define INPUTFILENAME "test/testVectors/audio.44100.txt" // input file - text written samples seperated by white spaces
#define POWFILENAME "/tmp/fa.pow" // The power spectrum of the input signal
#define THRESHFILENAME "/tmp/thresh.dat" // The masking threshold at each frequency of the input signal
#define TMASKFILENAME "/tmp/fa.t.mask" // The masking threshold for each filter CF
#define EXCITEFILENAME "/tmp/fa.excite" // The excitation
#include <fstream>

int main(int argc, char *argv[]) {
cout<<argc<<endl;
    if (argc<2){
        cout<<"Useage "<<argv[0]<<" fileName"<<endl;
        cout<<"e.g.   "<<argv[0]<<" INPUTFILENAME"<<endl;
        cout<<"\twhere fileName is an audio file text written samples seperated by white spaces, look for the file "<<INPUTFILENAME<<" which shipped with gtkIOStream for an example."<<endl;
        exit(-1);
    }

    // Setup many variables
    // The number of time domain samples
    int sampleCount=1024, halfSampleCount=(int)rint((double)sampleCount/2.0);
    //int sampleCount=9000, halfSampleCount=(int)rint((double)sampleCount/2.0);
    // The filter bank count and sample frequency
    int count=50, sampleFreq=44100;
    // The number of time domain samples to skip before the sample of interest.
    int skip=8192-sampleCount-1;
    // The input array to hold the input time data
    double input[sampleCount];

    // open the input file
    ifstream inputF(argv[1]);
    if (!inputF){
        cerr<<"couldn't open the file "<<argv[1]<<endl;
        exit(-1);
    }

    // Skip the first 2*'skip' samples.
    int temp;
    for (int i=0; i<skip; i++)
        inputF >> temp >> input[0];

    // load sampleCount samples as the input to the algorithm
    for (int i=0; i<sampleCount; i++)
        inputF >> input[i];
    inputF.close();

//    ofstream outputCF("cf.dat"); // central freq. output file
    ofstream outputT(TMASKFILENAME);
    ofstream outputP(POWFILENAME); // Input data Fourier power file

    // Get our masking function (class) ...
    AudioMasker masker(sampleFreq, count);
    //AudioMasker masker; // Can also be called like so with default filter banks and sampleFrequency

    masker.excite(input, sampleCount); // find the mask

    for (int j=0; j<count; j++) { // Output the central freq to file
  //      outputCF <<masker.pfb->cf[j]*((double)sampleCount/(double)sampleFreq)<<'\t';
        outputT  << 20*log10(masker.mask[j])<<'\t'; // output the mask for each filter CF to file
    }
    //outputCF<<endl;
    outputT<<endl;

    RealFFTData fftData(sampleCount); // Find the fourier transform for the output of the power to file
    RealFFT fft(&fftData); // init the Fourier transform
    for (int j=0; j<sampleCount; j++) // load the time domain input to the Fourier transform
        fftData.in[j]=input[j];
    fft.fwdTransform(); // Perform the transformation of the time domain to the frequency domain.
    fftData.compPowerSpec(); // Find the power spectrum
    for (int j=0; j<sampleCount/2; j++) // Output the power spectrum to file
        outputP<<20*log10(sqrt(fftData.power_spectrum[j]))<<'\t';
    outputP<<endl;

    //outputCF.close();
    outputT.close();
    outputP.close();

    ofstream outputF(THRESHFILENAME); // Find and output the masking threshold for each Fourier bin in the power spectrum
    double fact=(double)sampleFreq/((double)sampleCount-1.0); // convert from an index to the equivalent Fourier bin frequency
    for (int j=0; j<halfSampleCount; j++) {
        //    cout<<"finding for freq "<<j*fact<<'\t';
        outputF<<20*log10(masker.findThreshold(j*fact))<<'\t';
    }
    outputF<<endl;
    outputF.close();

    ofstream octFile("/tmp/view.m");
    octFile<<"% run this in octave : install octave"<<endl;
    octFile<<"load -force "<<POWFILENAME<<endl;
    octFile<<"load -force "<<THRESHFILENAME<<endl;
    octFile<<"whos"<<endl;
    octFile<<"semilogx(1:length(fa), fa, 1:length(thresh), thresh)"<<endl;
    octFile<<"legend('power spec.','masking threshold')"<<endl;
    octFile<<"xlabel('Fourier Bin')"<<endl;
    octFile<<"ylabel('Energy')"<<endl;
    octFile<<"title('Typical masking example - vary the threshold up and down to change audio quality')"<<endl;
    octFile<<"pause"<<endl;
    octFile.close();

    string tempPath="/tmp"; //note: if you don't have /tmp on your system, then you will ahve to change the tempmorary dir path name

    // start octave - adding the tempPath to the octave path
    vector<string> args(3); args[0]=string("--silent"); args[1]=string("--path"); args[2]=tempPath;
    Octave octave(args);

    octave.runM("view");
}
