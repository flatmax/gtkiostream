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

#include <iostream>
#include <fstream>
using namespace std;

#include "DSP/Resampler.H"
int main(int argc, char *argv[]){

  int Nx=100;
  int Ny=100;
  int chCnt=2;

  // Generate some data
  Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> x;
  x=Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>::Random(Nx,chCnt);

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> y(Ny, chCnt);

  Resampler<double> resampler;
  resampler.resample(x, y);

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> yp9(Ny+9, chCnt);
  resampler.resample(x, yp9);
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> yp8(Ny+8, chCnt);
  resampler.resample(x, yp8);

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> ym9(Ny-9, chCnt);
  resampler.resample(x, ym9);
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> ym8(Ny-8, chCnt);
  resampler.resample(x, ym8);

  ofstream output("/tmp/x.txt");
  output<<x<<endl;
  output.close();

  output.open("/tmp/y.txt");
  output<<y<<endl;
  output.close();

  output.open("/tmp/yp9.txt");
  output<<yp9<<endl;
  output.close();
  output.open("/tmp/yp8.txt");
  output<<yp8<<endl;
  output.close();

  output.open("/tmp/ym9.txt");
  output<<ym9<<endl;
  output.close();
  output.open("/tmp/ym8.txt");
  output<<ym8<<endl;
  output.close();
  return 0;
}
