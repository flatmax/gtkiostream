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

#ifndef BANDLIMITER_H
#define BANDLIMITER_H

// Debug
#include "Debug.H"
#include "gtkiostream_config.h"
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>

/** Class to band limit a signal
\example BandLimiterTest.C
*/
template<typename FP_TYPE>
class BandLimiter : public Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> {
  Eigen::FFT<FP_TYPE> fft; ///< The fast Fourier transform

  /** Givent the mInimum frequency (fi), high pass filter the inherited data type from fi up.
  On return, the inherited Eigen array will contain the band limited data.
  @param fi The mInimum frequency to preserve.
  @param fs the sample rate
  @return 0 on success, error otherwise.
  */
  int HPBandlimit(float fi, float fs);

public:
#ifdef HAVE_SOX
    /** Save the impulse to file
    \param fileName The name of the file to load the time domain coefficients from
    \return Negative value on error.
    */
    int saveToFile(const std::string fileName, float fs);
#endif

  /** Given an input signa, and the mInimum frequency (fi) high pass filter the signal from fi up.
  On return, the inherited Eigen array will contain the band limited data.
  @param input the input signal to band limit
  @param fi The mInimum frequency to preserve.
  @param fs the sample rate
  @return 0 on success, error otherwise.
  */
  template<typename Derived>
  int HPBandlimit(const Eigen::MatrixBase<Derived> &input, float fi, float fs){
    *static_cast<Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic>*>(this) = input.template cast<FP_TYPE>();
    return HPBandlimit(fi, fs);
  }
};
#endif // BANDLIMITER_H
