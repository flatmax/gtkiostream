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

#include <fft/Real2DFFT.H>
#include <Octave.H>
#include <stdlib.h>

int main(void) {
    string tempPath="/tmp"; //note: if you don't have /tmp on your system, then you will ahve to change the tempmorary dir path name

    // start octave - adding the tempPath to the octave path
    vector<string> args(3);
    args[0]=string("--silent");
    args[1]=string("--path");
    args[2]=tempPath;
    Octave octave(args);

    int count=7;

    ofstream out((tempPath+"/Real2DFFTExampleTest.m").c_str());
    out<<"function [INr, INc, pwrSpec]=RealFFTExampleTest(in)"<<endl;
    out<<"IN=fft2(in);"<<endl;
    out<<"INr=real(IN);"<<endl;
    out<<"INc=imag(IN);"<<endl;
    out<<"pwrSpec=abs(IN);"<<endl;
    out<<"end"<<endl;
    out.close();

    vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > input(1), output(3);
    // set some global variables
    input[0]=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(count, count); // generate some random data;
    octave.runM("Real2DFFTExampleTest", input, output);

    Real2DFFTData *fftData = new Real2DFFTData(count, count);
    Real2DFFT *fft= new Real2DFFT(fftData);

    // clear the data
    fftData->clearInput();
    fftData->clearOutput();

//    int temp=x/2, temp2=y/2;
//    for (int j=0; j<x; j++)
//        fftData->in[temp2+j*x]=10000.0;
//    for (int j=0; j<y; j++)
//        fftData->in[temp*y+j]=10000.0;
//    //  fftData->in[temp*y+(y-1)/2]=20000.0;

    for (int i=0; i<fftData->getXSize(); i++)
        for (int j=0; j<fftData->getYSize(); j++)
            fftData->in[i*count+j]=input[0](i,j);

    fft->fwdTransform();

//    for (int i=0; i<round(fftData->getXSize()/2.); i++) {
    double error=0., maxError=0.;
    for (int i=0; i<fftData->getXSize(); i++) {
        for (int j=0; j<round(fftData->getYSize()/2.); j++){
//            cout<<i<<' '<<j<<'\t'<<c_re(fftData->out[i*count+j])<<'\t'<<output[0](i,j)<<'\t'<<c_re(fftData->out[i*count+j])-output[0](i,j)<<endl;
//            cout<<'\t'<<c_re(fftData->out[i*(int)round(fftData->getXSize()/2.)+j])<<'\t';
            double err=abs(c_re(fftData->out[i*(int)round(fftData->getXSize()/2.)+j])-output[0](i,j))+abs(c_im(fftData->out[i*(int)round(fftData->getXSize()/2.)+j])-output[1](i,j));
            error+=err;
            if (err>maxError)
                maxError=err;
        }
//        cout<<endl;
    }
//    cout<<'\n'<<endl;
//    cout<<output[0].block(0,0,fftData->getXSize(), round(fftData->getYSize()/2.))<<endl;
cout<<"the total error for the fwd transform is = "<<error<<endl;
cout<<"the max error for the fwd transform is = "<<maxError<<endl;


//    fftData->reScale();
    fftData->compPowerSpec();
    fftData->sqrtPowerSpec();

    error=0.; maxError=0.;
    for (int i=0; i<fftData->getXSize(); i++) {
        for (int j=0; j<round(fftData->getYSize()/2.); j++){
//            cout<<'\t'<<fftData->power[i*(int)round(fftData->getXSize()/2.)+j]<<'\t';
            double err=abs(fftData->power[i*(int)round(fftData->getXSize()/2.)+j]-output[2](i,j));
            error+=err;
            if (err>maxError)
                maxError=err;
        }
//        cout<<endl;
    }

//cout<<output[2]<<endl;

cout<<"the total error for the power spectrum is = "<<error<<endl;
cout<<"the max error for the power spectrum is = "<<maxError<<endl;

//    fft->invTransform();

//    for (int i=0; i<fftData->getXSize(); i++) {
//        for (int j=0; j<fftData->getYSize(); j++)
//            cout<<i<<' '<<j<<'\t'<<c_im(fftData->out[i*count+j])<<'\t'<<output[1](i,j)<<'\t'<<c_im(fftData->out[i*count+j])-output[1](i,j)<<endl;
//        cout<<endl;
//    }
//    cout<<'\n'<<endl;
//    /*  for (int i=0;i<fftData.getXSize();i++){
//      for (int j=0;j<fftData.getYHalfSize();j++)
//        cout<<fftData.out[i][j].im<<'\t';
//      cout<<endl;
//      }*/
////    for (int i=0; i<x; i++) {
////        for (int j=0; j<y/2+1; j++)
////            cout<<fftData->power[i*(y/2+1)+j]<<'\t';
////        cout<<endl;
////    }
    delete fftData;
    delete fft;
}
