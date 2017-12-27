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
#include "DSP/IIRCascade.H"
#include <Octave.H>
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){

    int N=100;
    int sectionCnt=10;

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> B, A;
    B=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N,sectionCnt);
    A=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N-2,sectionCnt);
    B/=100.;
    A/=100.;
    A.row(0)=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Ones(1,sectionCnt);

    IIRCascade iir;
    iir.reset(B, A);
    N=1200;
    Eigen::Matrix<double, Eigen::Dynamic, 1> x, y;
    Eigen::Matrix<float, Eigen::Dynamic, 1> xf, yf;
    xf=Eigen::Matrix<float, Eigen::Dynamic, 1>::Random(N,1);
    x=xf.cast<double>();
    y.resize(x.rows(), x.cols());
    yf.resize(x.rows(), x.cols());
    iir.process(x, y);
    iir.reset();
    iir.process(xf, yf);
    // cout<<"yOut2=["<<y<<"];"<<endl;

//    cout<<"B=["<<B<<"];\n"<<endl;
 //   cout<<"A=["<<A<<"];\n"<<endl;
  //  cout<<"x=["<<x<<"];\n"<<endl;

    Octave octave; /// The octave instance
    vector<string> args(1); args[0]=string("");
    octave.startOctave(args);

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> yHat, tempOct;
    yHat.resize(x.rows(), x.cols());
    for (int i=0; i<sectionCnt; i++){
        vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > input, output; // The octave input and output
        input.push_back(B.col(i));
        input.push_back(A.col(i));
        input.push_back(x);
        octave.runM("filter", input, output);
        x=output[0];
    }
    yHat=x;
    cout<<(y-yHat).array().abs().sum()/y.rows()/y.cols()<<endl;
    cout<<(yf-y.cast<float>()).array().abs().sum()/y.rows()/y.cols()<<endl;

    // cout<<"A=["<<A<<"];"<<endl;
    // cout<<"B=["<<B<<"];"<<endl;
    // cout<<"xIn=["<<x<<"];"<<endl;
    // cout<<"y=["<<y<<"];"<<endl;
    // cout<<"yf=["<<yf<<"];"<<endl;
    // cout<<"x=["<<x<<"];"<<endl;
    // cout<<"xf=["<<xf<<"];"<<endl;
    return 0;
}
