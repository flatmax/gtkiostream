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
#include "DSP/IIR.H"
#include <Octave.H>
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){

    int N=3;
    int chCnt=10;

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> B, A;
    B=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Zero(N,chCnt);
    A=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Zero(N,chCnt);
    A.row(0)=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Ones(1,chCnt);

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> BStep=B;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> AStep=A;
    double bFact=.11, aFact=.22;
    BStep*=0.; BStep.array()+=bFact;
    AStep*=0.; AStep.array()+=aFact;

    IIR iir;
    iir.reset(B, A);

    N=10;
    cout<<"Final B should be = "<<bFact*N<<endl;
    cout<<"Final A (first row) should be = "<<1.+aFact*N<<endl;
    cout<<"Final A should be = "<<aFact*N<<endl;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> x, y;
    x=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N,chCnt);
    y.resize(x.rows(), x.cols());
    iir.process(x, y, BStep, AStep);

    cout<<"B "<<iir.getB()<<endl;
    cout<<"A "<<iir.getA()<<endl;

    cout<<"IIR mem "<<iir.getMem()<<endl;
    IIR iir2;
    iir2.reset(B, A); // set the memory to zero
    cout<<"IIR2 mem "<<iir2.getMem()<<endl;
    cout<<"setMem"<<endl;
    iir2.setMem(iir);
    cout<<"IIR2 mem "<<iir2.getMem()<<endl;

    return 0;
}
