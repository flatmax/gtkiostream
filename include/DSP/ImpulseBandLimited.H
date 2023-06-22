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

#ifndef IMPULSEBANDLIMITED_H
#define IMPULSEBANDLIMITED_H

// Debug
#include "Debug.H"
#include "gtkiostream_config.h" // inlude config.h first as it defines EIGEN_FFTW_DEFAULT
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>
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
  // using Eigen::Array<FP_TYPE, Eigen::Dynamic, 1>::rows;
  // using Eigen::Array<FP_TYPE, Eigen::Dynamic, 1>::cols;
  // using Eigen::Array<FP_TYPE, Eigen::Dynamic, 1>::block;

protected:
  /** Before entering setMag, the amplitude in the DFT domain is unity everywhere.
  This method zeros bands which we don't want power in.
  \param X The unity DFT domain impulse response of a perfect impulse
  \param fs The sample rate in Hz
  \param fi The minimum frequency to keep (lower bound)
  \param fa The maximum frequency to keep (upper bound)
  */
  virtual void setMag(const Eigen::Array<typename Eigen::FFT<double>::Complex, Eigen::Dynamic, 1> &X, float fs, float fi, float fa);

public:
  /// Empty constructor (some embedded builds fail without this present)
  ImpulseBandLimited(){}
  /// Empty destructor
  virtual ~ImpulseBandLimited(){}

  /** Generate the band limited impulse response.
  \param s The duration in seconds
  \param fs The sample rate in Hz
  \param fi The minimum frequency to keep (lower bound)
  \param fa The maximum frequency to keep (upper bound)
  \return Negative value on error and the current impulse state is left unchanged.
  */
  virtual int generateImpulse(float s, float fs, float fi, float fa);

  /** Generate the band limited impulse response and circularly shift it.
  \param s The duration in seconds
  \param fs The sample rate in Hz
  \param fi The minimum frequency to keep (lower bound)
  \param fa The maximum frequency to keep (upper bound)
  \return Negative value on error and the current impulse state is left unchanged.
  */
  virtual int generateImpulseShift(float s, float fs, float fi, float fa);

#ifdef HAVE_SOX
    /** Save the impulse to file
    \param fileName The name of the file to load the time domain coefficients from
    \return Negative value on error.
    */
    int saveToFile(const std::string fileName, float fs);
#endif
};
#endif // IMPULSEBANDLIMITED_H
