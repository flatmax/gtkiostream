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

#include "DSP/FIR.H"
#include <Octave.H>
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){

    int N=100;
    int chCnt=2;

    // Generate a filter
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> h;
    h=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N,chCnt);

    // Generate the input data and reserve the output data space
    int Mx=10;
    int Nx=Mx*200;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> x, y;
    x=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(Nx,chCnt);
    y.setZero(x.rows(), x.cols());

    // instantiate the FIR object
    FIR<double> fir;
    fir.init(Mx);
    fir.loadTimeDomainCoefficients(h);
    for (int i=0; i<Nx/Mx; i++) // apply the filter in windows of N samples
      fir.filter(x.block(i*Mx, 0, Mx, chCnt), y.block(i*Mx, 0, Mx, chCnt));

    // Test with Octave's implementaiton.
    Octave octave; /// The octave instance
    vector<string> args(1); args[0]=string("");
    octave.startOctave(args);

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> yHat, A;
    A.setOnes(1,1);
    yHat.setZero(x.rows(), x.cols());
    for (int i=0; i<chCnt; i++){
        vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > input, output; // The octave input and output
        input.push_back(h.col(i));
        input.push_back(A);
        input.push_back(x.col(i));
        octave.runM("filter", input, output);
        yHat.col(i)=output[0];
    }

    // calculate the dB of disagreement
    double err=(y-yHat).array().abs().sum()/(double)y.rows()/(double)y.cols();
    double rms=x.array().abs().sum()/(double)x.rows()/(double)x.cols();
    cout<<"rms="<<rms<<endl;
    cout<<"err="<<err<<endl;
    cout<<"err/rms="<<err/rms<<endl;
    cout<<"error = "<<20.*log10(err/rms)<<" dB"<<endl;

    return 0;
}
