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
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){

    // 64 bit coefficients stored in 8 bit words
    unsigned char Bc[]={0,0,0,0,0,0,240,63,92,97,61,172,152,117,7,64,22,244,144,38,31,67,17,64,255,126,223,67,126,167,17,64,160,218,193,5,251,28,16,64,153,13,24,206,145,210,14,64,119,168,207,171,54,33,10,64,100,196,229,226,163,78,0,64,153,121,5,252,107,184,235,63,140,199,171,16,201,132,205,63,78,146,42,109,32,71,159,63};
    unsigned char Ac[]={0,0,0,0,0,0,240,63,108,235,52,52,65,245,228,63,82,36,95,165,154,15,249,63,46,106,165,75,35,121,240,63,96,109,179,103,200,180,239,63,54,127,198,95,102,78,223,63,245,94,194,170,119,173,211,63,30,178,239,110,37,155,182,63,111,166,239,44,27,62,163,63,84,95,29,19,39,154,130,63,30,58,119,188,41,70,113,63};

    // Define sizes
    const int Na=sizeof(Ac)/sizeof(double);
    const int Nb=sizeof(Bc)/sizeof(double);
    const int chCnt=1;

    double *Ad = reinterpret_cast<double *>(Ac);
    double *Bd = reinterpret_cast<double *>(Bc);

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> B, A;
    A=Eigen::Map<Eigen::Matrix<double,Na,chCnt>, Eigen::Unaligned > (Ad);
    B=Eigen::Map<Eigen::Matrix<double,Na,chCnt>, Eigen::Unaligned > (Bd);

    // cout<<A<<endl; // print the coefficients
    // cout<<B<<endl;

    // Process some data with the IIR filter
    IIR iir;
    iir.reset(B, A);

    int N=1000;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> x(N, chCnt), y(N, chCnt);
    x.setZero();
    x(0,0)=1.0; // construct an impulse as the input
    iir.process(x, y); // process the IIR filter to find the response
    cout<<y<<endl; // output the response
    return 0;
}
