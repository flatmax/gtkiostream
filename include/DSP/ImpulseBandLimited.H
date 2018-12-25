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

#ifndef IMPULSEBANDLIMITED_H
#define IMPULSEBANDLIMITED_H

// Debug
#include "Debug.H"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
#pragma GCC diagnostic pop
#include "gtkiostream_config.h"
// #define FIR_BLOCKSIZE_MISMATCH_ERROR FIR_ERROR_OFFSET-1
// #define FIR_H_EMPTY_ERROR FIR_ERROR_OFFSET-2
// #define FIR_CHANNEL_MISMATCH_ERROR FIR_ERROR_OFFSET-3
//
// /** Debug class for the ImpulseBandLimited class
// */
// class ImpulseBandLimitedDebug :  virtual public Debug  {
// public:
//     FIRDebug(){
// #ifndef NDEBUG
// errors[FIR_BLOCKSIZE_MISMATCH_ERROR]=std::string("The input data was not of the same length you used as the variable for the method init. ");
// errors[FIR_H_EMPTY_ERROR]=std::string("The fileter h is empty, please load using loadTimeDomainCoefficients. ");
// errors[FIR_CHANNEL_MISMATCH_ERROR]=std::string("The input, output and h columns (channels) are not the same count. ");
//
// #endif // NDEBUG
//     }
// };

/** Class to implement a bandlimited impulse. The impulse is held in the inherited Eigen::Array
\example ImpulseBandLimitedTest.C
*/
template<typename FP_TYPE>
class ImpulseBandLimited : public Eigen::Array<FP_TYPE, Eigen::Dynamic, 1> {
  Eigen::FFT<FP_TYPE> fft; ///< The fast Fourier transform

  // using Eigen::Array<FP_TYPE, Eigen::Dynamic, 1>::rows;
  // using Eigen::Array<FP_TYPE, Eigen::Dynamic, 1>::cols;
  // using Eigen::Array<FP_TYPE, Eigen::Dynamic, 1>::block;

public:

  /** Generate the band limited impulse response.
  \param s The duration in seconds
  \param fs The sample rate in Hz
  \param fi The minimum frequency to keep (lower bound)
  \param fa The maximum frequency to keep (upper bound)
  \return Negative value on error and the current impulse state is left unchanged.
  */
  int generateImpulse(float s, float fs, float fi, float fa);

#ifdef HAVE_SOX
    /** Save the impulse to file
    \param fileName The name of the file to load the time domain coefficients from
    \return Negative value on error.
    */
    int saveToFile(const std::string fileName, float fs);
#endif
};
#endif // IMPULSEBANDLIMITED_H
