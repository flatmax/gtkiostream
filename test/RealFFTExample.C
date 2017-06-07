/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
   This file is part of GTK+ IOStream class set

   GTK+ IOStream is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GTK+ IOStream is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with GTK+ IOStream
*/

#include <fstream>
#include <iostream>
using namespace std;

#include <fft/RealFFT.H>
#include <Octave.H>
#include <stdlib.h>

int main (void){

    string tempPath="/tmp"; //note: if you don't have /tmp on your system, then you will ahve to change the tempmorary dir path name

    // start octave - adding the tempPath to the octave path
    vector<string> args(3); args[0]=string("--silent"); args[1]=string("--path"); args[2]=tempPath;
    Octave octave(args);

    int count=11;

    ofstream out((tempPath+"/RealFFTExampleTest.m").c_str());
    out<<"function [INr, INc, pwrSpec]=RealFFTExampleTest(in)"<<endl;
    out<<"IN=fft(in);"<<endl;
    out<<"INr=real(IN);"<<endl;
    out<<"INc=imag(IN);"<<endl;
    out<<"pwrSpec=abs(IN);"<<endl;
    out<<"end"<<endl;
    out.close();

    vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > input(1), output(2);
    // set some global variables
    input[0]=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(count, 1); // generate some random data;
    octave.runM("RealFFTExampleTest", input, output);

    RealFFTData fftData(count);
    RealFFT rfft(&fftData);

    // read data into data and rdata :
  for (int i=0; i<count; i++)
        fftData.in[i]=input[0](i,0);

  // forward transform :
  rfft.fwdTransform();
  double error=0.0, maxError=0.0;
  for (int i=0; i<(int)floor(count/2.)+1; i++){
//    cout<<fftData.out[i]<<'\t'<<output[0](i,0)<<endl;
    error+=abs(fftData.out[i]-output[0](i,0));
    if (abs(fftData.out[i]-output[0](i,0)) > maxError)
        maxError=abs(fftData.out[i]-output[0](i,0));
  }
  for (int i=(int)floor(count/2.)+1; i<count; i++){
//    cout<<fftData.out[i]<<'\t'<<output[1](i,0)<<endl;
    error+=abs(fftData.out[i]+output[1](i,0)); // NOTE that the complex data is negated in the upper frequency range
    if (abs(fftData.out[i]-output[1](i,0)) > maxError)
        maxError=abs(fftData.out[i]+output[1](i,0));
  }
  cout<<'\n'<<"The total fwdTransform error = "<<error<<endl;
  cout<<'\n'<<"The maximum fwdTransform error = "<<maxError<<endl;


  // Find the power spectrum ...
  fftData.compPowerSpec();
  fftData.sqrtPowerSpec();
  error=0.0, maxError=0.0;
  for (int i=0; i<(int)ceil((float)count/2.); i++){
//    cout<<fftData.power_spectrum[i]<<'\t'<<output[2](i,0)<<endl;
    error+=abs(fftData.power_spectrum[i]-output[2](i,0));
    if (abs(fftData.power_spectrum[i]-output[2](i,0)) > maxError)
        maxError=abs(fftData.power_spectrum[i]-output[2](i,0));
  }
  cout<<'\n'<<"The total power spectrum computation error = "<<error<<endl;
  cout<<'\n'<<"The maximum power spectrum computation error = "<<maxError<<endl;

  for (int i=0; i<count; i++)
    fftData.in[i]=0.0;

  // inverse transform to check what happens (have to rescale too):
  rfft.invTransform();

  error=0.0, maxError=0.0;
  for (int i=0; i<count; i++){
    //cout<<fftData.in[i]<<'\t'<<input[0](i,0)<<endl;
    error+=abs(fftData.in[i]/(float)count-input[0](i,0));
    if (abs(fftData.in[i]/(float)count-input[0](i,0)) > maxError)
        maxError=abs(fftData.in[i]/(float)count-input[0](i,0));
  }
  cout<<'\n'<<"The total invTransform error = "<<error<<endl;
  cout<<'\n'<<"The maximum invTransform error = "<<maxError<<endl;

  return 0;
}
