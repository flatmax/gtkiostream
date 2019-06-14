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
#ifndef REALFFTDATA_H_
#define REALFFTDATA_H_

#include "fft/FFTCommon.H"
#include <complex>

/// class RealFFTData controls and manipulates fft data
class RealFFTData {
  /// Var used to specify if the memory was allocated by the RealFFTData class
  int deleteInOutMemory;
public:
  /// Specifies the size of the data array
  int size;
  /// Specifies the minimum and maximum power bins as used in the methods findMaxMinPowerBins and compPowerSpec
  int minPowerBin, maxPowerBin;
  /// the input, output and power_spectrum arrays
  fftw_real *in, *out, *power_spectrum; //, *powerDeriv; power deriv. removed for now
  /// The total power (summed) of the power spectrum as used in the method compPowerSpec
  double totalPower;

  /// All memory to be allocated internally
  RealFFTData(int sz);
  /// input and output data arrays are to be allocated by another process
  RealFFTData(int sz, fftw_real*inp, fftw_real*outp);
  /// Deconstructor
  ~RealFFTData(void);

  /// Limits the maximum to 'lim' and returns the last fft bin with max
  int limitHalfPowerSpec(double lim);

  /// Returns the number of elements in the input and output arrays
  int getSize(void){return size;}
  /// Returns the number of elements in the power spectrum array
  int getHalfSize(void){ if (!(size%2)) return size/2; else return size/2+1;}

  /// Returns the maximum input variable
  fftw_real findMaxIn(void);
  /// Fills the max and min power spectrum bins
  void findMaxMinPowerBins(void);

  /// This function computes the power spectrum and returns the max bin
  int compPowerSpec();
  /// This function computes the square root of the power spectrum and returns the max bin
  int sqrtPowerSpec();
  /// This is the power spectrum in dB
  void powerInDB();
  //  int powerSpecDeriv(); // Find the derivative of the power spectrum

  /// This function zeros the output data array (out)
  void zeroFFTData(void);

  /** Get the complex representation of the coefficient at index k
  \param k The index to get a complex coefficient representation of.
  \return The complex coefficient at index k (k<=getSize())
  */
  std::complex<fftw_real> getComplexCoeff(const unsigned int k);

  /** load the input data.
  \param i The index to load into.
  \param val The value to load into in[i]
  */
  void load(const unsigned int i, const fftw_real val);
  /** unload the output data.
  \param i The index to load into.
  \return The value to load into out[i]
  */
  fftw_real unload(const unsigned int i);
  /** unload the power_spectrum data.
  \param i The index to load into.
  \return The value to load into power_spectrum[i]
  */
  fftw_real unloadPS(const unsigned int i);

  /// For debugging purposes, dump the in array to stdout
  void dumpIn();
  /// For debugging purposes, dump the out array to stdout
  void dumpOut();
};
#endif // REALFFTDATA_H_
