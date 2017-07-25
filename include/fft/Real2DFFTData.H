/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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
#ifndef REAL2DFFTDATA_H_
#define REAL2DFFTDATA_H_

#include "fft/FFTCommon.H"
//#include "fft/Real2DFFT.H"

/// class Real2DFFTData controls and manipulates real 2D fft data
class Real2DFFTData {
  /// x=row y=column
  int x, y;
  /// The total memory used by this class
  fftw_real *mem;
  /// Free the memory
  void memDeInit(void);
public:
  /// The input data and power spectrum
  fftw_real *in, *power;
  /// The output data
  fftw_complex *out;
  /// Arrays which sum across rows (x) and columns (y)
  fftw_real *xSum, *ySum;
  /// A sum across the input time signal
  fftw_real *timeXSum;
  /// Power spectral sums across rows (x) and columns (y)
  fftw_real *realXSum, *imagXSum;

  /// The total power in the power spectrum, the maximum and minimum powers too
  double totalPower, maxPower, minPower;
  /// The minimum/maximum row (x) and column (y) sums
  double xSumMin, xSumMax, ySumMin, ySumMax;
  /// Row (x) and Column (y) max sum indexes
  int maxXSumIndex, maxYSumIndex;

  /// Constructor with all memory to be allocated internally
  Real2DFFTData(int r, int c);
  /// Deconstructor
  ~Real2DFFTData();

  /// The row count
  int getXSize(){return x;}
  /// The column count
  int getYSize(){return y;}
  /// The half row count
  int getXHalfSize(){ if (!(x%2)) return x/2; else return x/2+1;}
  /// The half column count
  int getYHalfSize(){ if (!(y%2)) return y/2; else return y/2+1;}

  /// Scales the output down by the number of elements
  void reScale(void);
  /// This function computes the power spectrum and updates the totalPower, maxPower and minPower
  void compPowerSpec(); // Find the power spectrum
  /// This function computes the square root of the power spectrum and returns the max bin
  int sqrtPowerSpec();
  /// Finds 10*log10(power spectrum) and updates the totalPower, maxPower and minPower
  void compLogPowerSpec(); // Find the log power spectrum

  /// Updates timeXSum
  void timeSpecAverage();
  /// Updates realXSum and imagXSum
  void complexSpecAverage();
  /// Finds the power Spectrum averages and
  /// updates the xSumMin, xSumMax, ySumMin, ySumMax, xSum, ySum, maxXSumIndex, maxYSumIndex
  void powerSpecAverage();
  /// Finds the y-sum between columns start and stop
  void findYSum(int start, int stop);
  /// Finds the y-max for the ySum array, updates ySumMin, ySumMax, maxYSumIndex
  void findYMax(void);

  /// Zeros the in array
  void clearInput(void);
  /// Zeros the out awway
  void clearOutput(void);
};
#endif // REAL2DFFTDATA_H_
