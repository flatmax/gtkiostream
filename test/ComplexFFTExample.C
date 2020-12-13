/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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

#include "fft/ComplexFFT.H"
#include <Octave.H>
#include <fstream>
#include <iostream>
using namespace std;

int main (){


    string tempPath="/tmp"; //note: if you don't have /tmp on your system, then you will ahve to change the tempmorary dir path name

    // start octave - adding the tempPath to the octave path
    vector<string> args(3); args[0]=string("--silent"); args[1]=string("--path"); args[2]=tempPath;
    Octave octave(args);

    int count=11;

    ofstream out((tempPath+"/ComplexFFTExampleTest.m").c_str());
    out<<"function [INr, INc, pwrSpec]=ComplexFFTExampleTest(inR, inC)"<<endl;
    out<<"IN=fft(inR+i*inC);"<<endl;
    out<<"INr=real(IN);"<<endl;
    out<<"INc=imag(IN);"<<endl;
    out<<"pwrSpec=abs(IN);"<<endl;
    out<<"end"<<endl;
    out.close();


    vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > input(2), output(3);
    // set some global variables
    input[0]=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(count, 1); // generate some random data;
    input[1]=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(count, 1); // generate some random data;
    octave.runM("ComplexFFTExampleTest", input, output);

    ComplexFFTData fftData(count);
    ComplexFFT fft(&fftData);

    // read data into data and rdata :
    for (int i=0; i<count; i++){
        c_re(fftData.in[i])=input[0](i,0);
        c_im(fftData.in[i])=input[1](i,0);
    }

    // forward transform :
    fft.fwdTransform();

    cout<<"octave real :"<<endl;
    cout<<output[0].transpose()<<endl;
    cout<<"fft real :"<<endl;
    for (int i=0; i<count; i++)
        cout<<c_re(fftData.out[i])<<'\t';
    cout<<"\noctave imag :"<<endl;
    cout<<output[1].transpose()<<endl;
    cout<<"fft imag :"<<endl;
    for (int i=0; i<count; i++)
        cout<<c_im(fftData.out[i])<<'\t';
    cout<<endl;

  double error=0.0, maxError=0.0;
  for (int i=0; i<count; i++){
    double er=abs(c_re(fftData.out[i])-output[0](i,0))+abs(c_im(fftData.out[i])-output[1](i,0));
    error+=er;
    if (er > maxError)
        maxError=er;
  }
  cout<<'\n'<<"The total fwdTransform error = "<<error<<endl;
  cout<<'\n'<<"The maximum fwdTransform error = "<<maxError<<endl;

//  for (int i=0; i<count; i++)
//    c_re(fftData.in[i])=c_im(fftData.in[i])=0.;

    // inverse transform :
    fft.invTransform();

    error=0.0, maxError=0.0;
    for (int i=0; i<count; i++){
    double er=abs(c_re(fftData.in[i])/(float)count-input[0](i,0))+abs(c_im(fftData.in[i])/(float)count-input[1](i,0));
    error+=er;
    if (er > maxError)
        maxError=er;
    }
    cout<<'\n'<<"The total invTransform error = "<<error<<endl;
    cout<<'\n'<<"The maximum invTransform error = "<<maxError<<endl;

  // Find the power spectrum ...
  fftData.compPowerSpec();
  fftData.sqrtPowerSpec();
//    cout<<output[2].transpose()<<endl;
//  for (int i=0; i<count; i++)
//    cout << fftData.power_spectrum[i]<<'\t';

    error=0.0, maxError=0.0;
    for (int i=0; i<count; i++){
        double er=fftData.power_spectrum[i]-output[2](i,0);
        error+=er;
        if (er > maxError)
            maxError=er;
    }
    cout<<'\n'<<"The total power spectrum error = "<<error<<endl;
    cout<<'\n'<<"The maximum power spectrum error = "<<maxError<<endl;

}

