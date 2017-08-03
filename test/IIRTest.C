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
#include "DSP/IIR.H"
#include <Octave.H>
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){

    int N=2100;
    int chCnt=10;

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> B, A;
    B=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N,chCnt);
    A=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N,chCnt);
    B/=100.;
    A/=100.;
    A.row(0)=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Ones(1,chCnt);


    IIR iir;
    iir.reset(B, A);

    N=12800;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> x, y;
    x=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N,chCnt);
    y.resize(x.rows(), x.cols());
    iir.process(x, y);

//    cout<<"B=["<<B<<"];\n"<<endl;
 //   cout<<"A=["<<A<<"];\n"<<endl;
  //  cout<<"x=["<<x<<"];\n"<<endl;

    Octave octave; /// The octave instance
    vector<string> args(1); args[0]=string("");
    octave.startOctave(args);

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> yHat;
    yHat.resize(x.rows(), x.cols());
    for (int i=0; i<chCnt; i++){
        vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > input, output; // The octave input and output
        input.push_back(B.col(i));
        input.push_back(A.col(i));
        input.push_back(x.col(i));
        octave.runM("filter", input, output);
        yHat.col(i)=output[0];
    }
    cout<<(y-yHat).array().abs().sum()/y.rows()/y.cols()<<endl;
//    cout<<y<<endl;

    return 0;
}
