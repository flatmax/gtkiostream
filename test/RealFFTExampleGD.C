/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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
#include <stdlib.h>

int main (void){
    // string tempPath="/tmp";
    // ofstream out((tempPath+"/RealFFTExampleTest.m").c_str());
    // out<<"function [INr, INc, pwrSpec]=RealFFTExampleTest(in)"<<endl;
    // out<<"IN=fft(in);"<<endl;
    // out<<"INr=real(IN);"<<endl;
    // out<<"INc=imag(IN);"<<endl;
    // out<<"pwrSpec=abs(IN);"<<endl;
    // out<<"end"<<endl;
    // out.close();

    // cout<<"octave-cli --eval \"";

    unsigned int N=8;
    RealFFTData fftData(N);
    RealFFT rfft(&fftData);

    for (unsigned int i=0; i<N; i++)
      fftData.in[i]=(drand48()-.5)*10.;

      cout<<"clear all;"<<endl;
    cout<<"x=[";
    fftData.dumpIn();
    cout<<"];"<<endl;

    // forward transform :
    rfft.fwdTransform();

    // cout<<"y=["<<endl;
    // fftData.dumpOut();
    // cout<<"];"<<endl;
    cout<<"X=[";
    for (unsigned int i=0; i<N; i++){
      std::complex<fftw_real> cDat=fftData.getComplexCoeff(i);
      cout<<cDat.real()<<"+i*"<<cDat.imag()<<' ';
    }
    cout<<"];"<<endl;

    cout<<"XHAT=fft(x);"<<endl;
    cout<<"[X; XHAT]"<<endl;
    cout<<"sum(X-XHAT)/length(X)"<<endl;

    // cout<<"\""<<endl;
  return 0;

//   // Find the power spectrum ...
//   fftData.compPowerSpec();
//   fftData.sqrtPowerSpec();
//   error=0.0, maxError=0.0;
//   for (int i=0; i<(int)ceil((float)input[0].cols()/2.); i++){
// //    cout<<fftData.power_spectrum[i]<<'\t'<<output[2](0,i)<<endl;
//     error+=abs(fftData.power_spectrum[i]-output[2](0,i));
//     if (abs(fftData.power_spectrum[i]-output[2](0,i)) > maxError)
//         maxError=abs(fftData.power_spectrum[i]-output[2](0,i));
//   }
//   cout<<'\n'<<"The total power spectrum computation error = "<<error<<endl;
//   cout<<'\n'<<"The maximum power spectrum computation error = "<<maxError<<endl;
//
//   for (int i=0; i<input[0].cols(); i++)
//     fftData.in[i]=0.0;
//
//   // inverse transform to check what happens (have to rescale too):
//   rfft.invTransform();
//
//   error=0.0, maxError=0.0;
//   for (int i=0; i<input[0].cols(); i++){
//     //cout<<fftData.in[i]<<'\t'<<input[0](0,i)<<endl;
//     error+=abs(fftData.in[i]/(float)count-input[0](0,i));
//     if (abs(fftData.in[i]/(float)count-input[0](0,i)) > maxError)
//         maxError=abs(fftData.in[i]/(float)count-input[0](0,i));
//   }
//   cout<<'\n'<<"The total invTransform error = "<<error<<endl;
//   cout<<'\n'<<"The maximum invTransform error = "<<maxError<<endl;
//
//   return 0;
}
