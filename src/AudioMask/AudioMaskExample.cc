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

#include "../fft/realFFT.H"
#include "AudioMask.H"

// Filter bank memory de-allocation routine
void FBDeMalloc(double **outputGT, int fCount){
  if (outputGT){
    for (int i=0;i<fCount;i++)
      if (outputGT[i]) delete [] outputGT[i];
    delete [] outputGT;
  }
}

// Filter bank memory allocation routine
double ** FBMalloc(int fCount, int sCount){
  // Find the output of the gammatone filters
  double **outputGT=NULL;
  if (!(outputGT=new double*[fCount])){
    cerr<<"filter bank malloc error initial"<<endl;
    exit(-1);
  } else {
    for (int i=0;i<fCount;i++)
      outputGT[i]=NULL;
    for (int i=0;i<fCount;i++)
      if (!(outputGT[i]=new double[sCount])){
        cerr<<"filter bank malloc error secondary"<<endl;
        FBDeMalloc(outputGT, fCount);
        exit(-1);
      }
  }
  return outputGT;
}

#define INPUTFILENAME "fa.dat"
#define TMASKFILENAME "fa.t.mask"
#define BMASKFILENAME "fa.b.mask"
#define POWFILENAME "fa.pow"
#define EXCITEFILENAME "fa.excite"
#include <fstream>

#define USEGCFB
#ifdef USEGCFB
#define PFB gcfb
#else
#define PFB gtfb
#endif

void main(void){
  int sampleCount=16384, halfSampleCount=(int)rint((double)sampleCount/2.0);
  int count=50; int lowFreq=100; int sampleFreq=8192;
  double input[sampleCount], **outputGT, **output;
  double **powOutput;

  //  bzero(input, sampleCount*sizeof(double));
  //input[0]=100.0;
  // Load the input
  ifstream inputF(INPUTFILENAME);
  int temp;
  for (int i=0; i<sampleCount;i++)
    inputF >> temp >> input[i];
  inputF.close();

  powOutput=FBMalloc(count, halfSampleCount);
  outputGT=FBMalloc(count, sampleCount);
  for (int i=0;i<count;i++)
      bzero(outputGT[i], sampleCount*sizeof(double));

  output=FBMalloc(count, sampleCount);
  for (int i=0;i<count;i++)
      bzero(output[i], sampleCount*sizeof(double));

#ifdef USEGCFB
  // Get our gammachirp filter bank and filter using it
  GCFB gcfb(lowFreq, sampleFreq, count);
  gcfb.filter((double*)input, outputGT, output, sampleCount);
#else
  GTFB gtfb(lowFreq, sampleFreq, count);
  for (int i=1; i<=count;i++)
    gtfb.grab(i)->filter(input, &outputGT[i-1][0], sampleCount);
#endif

  // Convert the output to the frequency domain ...
  realFFTData fftData(sampleCount);
  realFFT fft(&fftData);
  for (int i=0;i<count;i++){
    for (int j=0; j<sampleCount;j++)
      fftData.in[j]=outputGT[i][j];
    fft.fwdTransform();
    fftData.compPowerSpec();
    for (int j=0; j<halfSampleCount;j++){
      powOutput[i][j]=sqrt(fftData.power_spectrum[j]);
      //      powOutput[i][j]=fftData.power_spectrum[j];
      //cout<<powOutput[i][j]<<' ';
    }
    //    cout<<endl;
  }

  // Get our spreading function ...
  AudioMask mask(count);
  // Set up the frequencies of interest (filter bank centre freqs.)
  PFB.grab(1);
  for (int i=0; i<count;i++)
#ifdef USEGCFB
    mask.setCFreq(i, gcfb.prev()->gt->cf);
#else
    mask.setCFreq(i, gtfb.prev()->cf);
#endif
  // Find the spreading function
  mask.exciteTerhardt(powOutput, sampleCount, sampleFreq);

  PFB.grab(1);
  ofstream outputF(TMASKFILENAME);
  for (int i=0; i<count;i++)
#ifdef USEGCFB
    outputF <<gcfb.prev()->gt->cf*((double)sampleCount/(double)sampleFreq)<<'\t'<< 20*log10(mask.mask[i])<<'\n';
#else
    outputF <<gtfb.prev()->cf*((double)sampleCount/(double)sampleFreq)<<'\t'<< 20*log10(mask.mask[i])<<'\n';
#endif
    //for (int i=0; i<sampleCount;i++)
    //outputF << input[i]<<'\n';
  outputF.close();

  mask.exciteBeerends(powOutput, sampleCount, sampleFreq);
  cout<<"Done exciting"<<endl;
  PFB.grab(1);
  outputF.open(BMASKFILENAME);
  for (int i=0; i<count;i++)
#ifdef USEGCFB
    outputF <<gcfb.prev()->gt->cf*(sampleCount/sampleFreq)<<'\t'<< 10*log10(mask.mask[i])<<'\n';
#else
    outputF <<gtfb.prev()->cf*(sampleCount/sampleFreq)<<'\t'<< 10*log10(mask.mask[i])<<'\n';
#endif
    //for (int i=0; i<sampleCount;i++)
    //outputF << input[i]<<'\n';
  outputF.close();

  outputF.open(POWFILENAME);
  for (int j=0; j<sampleCount;j++)
    fftData.in[j]=input[j];
  fft.fwdTransform();
  fftData.compPowerSpec();
  for (int j=0; j<halfSampleCount;j++){
    outputF<<j+1<<'\t'<<20*log10(sqrt(fftData.power_spectrum[j]))<<endl;
    //outputF<<j+1<<'\t'<<20*log10(fftData.power_spectrum[j])<<endl;
  }
  outputF.close();


  FBDeMalloc(powOutput, count);
  FBDeMalloc(output, count);
  FBDeMalloc(outputGT, count);
}
