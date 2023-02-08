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

#include "DSP/ImpulsePink.H"

using namespace Eigen;

#include <iostream>

template<typename FP_TYPE>
void ImpulsePink<FP_TYPE>::setMag(Array<typename FFT<double>::Complex, Dynamic, 1> const &X, float fs, float fi, float fa){
  ImpulseBandLimited<FP_TYPE>::setMag(X, fs, fi, fa); // zero out necessary DFT bins
  // aooky the ramp down in PSD
  int N=X.rows(); // the number of samples
  int No2 = (int)ceil((float)N/2.);
  Array<double, Dynamic, 1> sf=Array<double, Dynamic, 1>::LinSpaced(No2,1.,(double)No2);
  std::cout<<"XOrig=["<<X<<"];"<<std::endl;
  sf=sf.sqrt();
  const_cast< Array<typename FFT<double>::Complex, Dynamic, 1>& >(X).topRows(No2)/=sf;
  No2 = (int)floor((float)N/2.);
  const_cast< Array<typename FFT<double>::Complex, Dynamic, 1>& >(X).bottomRows(No2)*=sf.topRows(No2).colwise().reverse();
  std::cout<<"XFinal=["<<X<<"];"<<std::endl;
}

template class ImpulsePink<short int>;
template class ImpulsePink<int>;
template class ImpulsePink<float>;
template class ImpulsePink<double>;
