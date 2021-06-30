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

#include "DSP/BandLimiter.H"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]){

  float fs=480.;
  float fi=100.;
  int N=(int)fs;
  int M=3;

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> impulse(N, M);
  impulse.topRows(1).setOnes();
  BandLimiter<double> bl;
  bl.HPBandlimit(impulse, fi, fs);
  #ifdef HAVE_SOX
    bl.saveToFile("/tmp/test.wav", fs);
  #endif
  return 0;
}
