/*
 libaudiomask - hybrid simultaneous audio masking threshold evaluation library
    Copyright (C) 2000-2010  Dr Matthew Raphael Flax

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

#include <math.h>

#include "../fft/realFFT.H"
#include "MooreSpread.H"

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

void main(void){
  int sampleCount=4096, halfSampleCount=(int)rint((double)sampleCount/2.0);
  int count=75; int lowFreq=100; int sampleFreq=8192;
  double input[sampleCount], **outputGT, **output;
  double **powOutput;
  powOutput=FBMalloc(count, halfSampleCount);
  bzero(input, sampleCount*sizeof(double));
  input[0]=1.0;
  outputGT=FBMalloc(count, sampleCount);
  for (int i=0;i<count;i++)
      bzero(outputGT[i], sampleCount*sizeof(double));

  output=FBMalloc(count, sampleCount);
  for (int i=0;i<count;i++)
      bzero(output[i], sampleCount*sizeof(double));

  // Get our gammachirp filter bank and filter using it
  //GCFB gcfb(lowFreq, sampleFreq, count);
  //gcfb.filter((double*)input, outputGT, output, sampleCount);
  GTFB gtfb(lowFreq, sampleFreq, count);
  for (int i=1; i<=count;i++)
    gtfb.grab(i)->filter(input, &outputGT[i-1][0], sampleCount);

  // Convert the output to the frequency domain ...
  realFFTData fftData(sampleCount);
  realFFT fft(&fftData);
  for (int i=0;i<count;i++){
    for (int j=0; j<sampleCount;j++)
      fftData.in[j]=outputGT[i][j];
    fft.fwdTransform();
    fftData.compPowerSpec();
    for (int j=0; j<halfSampleCount;j++){
      powOutput[i][j]=fftData.power_spectrum[j];
      //cout<<powOutput[i][j]<<' ';
    }
    //    cout<<endl;
  }

  // Get our spreading function ...
  MooreSpread spreadFn(count);
  // Set up the frequencies of interest (filter bank centre freqs.)
  //gcfb.grab(1);
  gtfb.grab(1);
  for (int i=0; i<count;i++)
    spreadFn.setCFreq(i, gtfb.prev()->cf);
  //spreadFn.setCFreq(i, gcfb.prev()->gt->cf);
  // Find the spreading function
  spreadFn.excite(powOutput, sampleCount, sampleFreq);

  FBDeMalloc(powOutput, count);
  FBDeMalloc(output, count);
  FBDeMalloc(outputGT, count);
}
