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
#ifndef COMPLEXFFTDATA_H_
#define COMPLEXFFTDATA_H_

#include "fft/FFTCommon.H"
//#include "fft/ComplexFFT.H"

/// class ComplexFFTData controls and manipulates complex fft data
class ComplexFFTData {
public:
  /// Specifies the size of the data array
  int size;
  /// the input and output arrays
  fftw_complex *in, *out;
  /// the power_spectrum array
  fftw_real *power_spectrum;
  /// The total power (summed) of the power spectrum as used in the method compPowerSpec
  double totalPower;
  /// Specifies the minimum and maximum power bins as used in the methods findMaxMinPowerBins and compPowerSpec
  int minPowerBin, maxPowerBin;

  /// Constructor with all memory to be allocated internally
  ComplexFFTData(int sz);
  /// Deconstructor
  ~ComplexFFTData(void);

  /// Use this to change associated fft data (for fft'ing)
  void switchData(ComplexFFTData *d);

  /// Limits the maximum to 'lim' and returns the last fft bin with max
  int limitHalfPowerSpec(double lim);

  /// Returns the number of elements in the input and output arrays
  int getSize(){return size;}
  //  int getHalfSize(){ if (!(size%2)) return size/2; else return size/2+1;}

  /// This function computes the power spectrum and returns the max bin
  int compPowerSpec();
  //  int powerSpecDeriv(); // Find the derivative of the power spectrum

  /// This function computes the square root of the power spectrum and returns the max bin
  int sqrtPowerSpec();
};
#endif // COMPLEXFFTDATADATA_H_
