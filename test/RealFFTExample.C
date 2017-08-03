/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
//    input[0]=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(1, count); // generate some random data;
input[0].resize(1,100);
input[0]<<4.5497,5.2236,6.2226,6.8095,5.1996,5.1842,4.5511,2.794,4.9386,2.3706,5.4951,3.5334,3.8609,5.2633,4.6081,5.771,5.529,4.1125,2.8273,4.4903,2.769,1.0212,0.49264,-1.8582,-0.11392,-2.3961,-1.4321,-2.962,-0.096841,-1.6438,1.6191,0.58763,0.010435,0.40752,0.9621,0.76177,0.68821,-0.81083,-0.89334,-0.40437,-2.3762,-2.163,-1.1191,-0.41497,2.9477,3.0626,1.4504,4.1391,3.1632,5.5102,3.8366,3.3688,4.0625,3.4717,3.2727,4.5003,3.6054,3.8871,2.9158,4.0317,5.5279,4.8904,5.0554,4.591,4.3782,3.3169,6.0995,5.4537,3.6365,3.5816,5.5261,2.0069,3.2094,3.2687,1.7526,2.9758,2.3571,1.4956,2.5631,2.4819,0.2467,1.231,0.55996,2.929,1.9442,1.64,-0.24525,1.1344,-0.6949,-1.2067,-2.0502,0.42126,0.5275,-1.6765,-1.6501,-1.4519,1.5032,0.74968,0.25362,1.864;
    octave.runM("RealFFTExampleTest", input, output);
    RealFFTData fftData(input[0].cols());
    RealFFT rfft(&fftData);

    // read data into data and rdata :
  for (int i=0; i<input[0].cols(); i++)
        fftData.in[i]=input[0](0,i);

  cout<<"in"<<endl;
  fftData.dumpIn();

  // forward transform :
  rfft.fwdTransform();

  cout<<"out"<<endl;
  fftData.dumpOut();
  
  double error=0.0, maxError=0.0;
  for (int i=0; i<(int)floor(input[0].cols()/2.)+1; i++){
//    cout<<fftData.out[i]<<'\t'<<output[0](0,i)<<endl;
    error+=abs(fftData.out[i]-output[0](0,i));
    if (abs(fftData.out[i]-output[0](0,i)) > maxError)
        maxError=abs(fftData.out[i]-output[0](0,i));
  }
  for (int i=(int)floor(input[0].cols()/2.)+1; i<input[0].cols(); i++){
//    cout<<fftData.out[i]<<'\t'<<output[1](0,i)<<endl;
    error+=abs(fftData.out[i]+output[1](0,i)); // NOTE that the complex data is negated in the upper frequency range
    if (abs(fftData.out[i]-output[1](0,i)) > maxError)
        maxError=abs(fftData.out[i]+output[1](0,i));
  }
  cout<<'\n'<<"The total fwdTransform error = "<<error<<endl;
  cout<<'\n'<<"The maximum fwdTransform error = "<<maxError<<endl;


  // Find the power spectrum ...
  fftData.compPowerSpec();
  fftData.sqrtPowerSpec();
  error=0.0, maxError=0.0;
  for (int i=0; i<(int)ceil((float)input[0].cols()/2.); i++){
//    cout<<fftData.power_spectrum[i]<<'\t'<<output[2](0,i)<<endl;
    error+=abs(fftData.power_spectrum[i]-output[2](0,i));
    if (abs(fftData.power_spectrum[i]-output[2](0,i)) > maxError)
        maxError=abs(fftData.power_spectrum[i]-output[2](0,i));
  }
  cout<<'\n'<<"The total power spectrum computation error = "<<error<<endl;
  cout<<'\n'<<"The maximum power spectrum computation error = "<<maxError<<endl;

  for (int i=0; i<input[0].cols(); i++)
    fftData.in[i]=0.0;

  // inverse transform to check what happens (have to rescale too):
  rfft.invTransform();

  error=0.0, maxError=0.0;
  for (int i=0; i<input[0].cols(); i++){
    //cout<<fftData.in[i]<<'\t'<<input[0](0,i)<<endl;
    error+=abs(fftData.in[i]/(float)count-input[0](0,i));
    if (abs(fftData.in[i]/(float)count-input[0](0,i)) > maxError)
        maxError=abs(fftData.in[i]/(float)count-input[0](0,i));
  }
  cout<<'\n'<<"The total invTransform error = "<<error<<endl;
  cout<<'\n'<<"The maximum invTransform error = "<<maxError<<endl;

  return 0;
}
