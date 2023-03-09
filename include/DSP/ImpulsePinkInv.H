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

#ifndef IMPULSEPINKINV_H
#define IMPULSEPINKINV_H

#include "ImpulseBandLimited.H"

/** Class to implement a bandlimited impulse. The impulse is held in the inherited Eigen::Array
\example ImpulsePinkInvTest.C
*/
template<typename FP_TYPE>
class ImpulsePinkInv : public ImpulseBandLimited<FP_TYPE> {

  /** Before entering setMag, the amplitude in the DFT domain is unity everywhere.
  This method zeros bands which we don't want power in and forces the ramp down in PSD
  \param X The unity DFT domain impulse response of a perfect impulse
  \param fs The sample rate in Hz
  \param fi The minimum frequency to keep (lower bound)
  \param fa The maximum frequency to keep (upper bound)
  */
  virtual void setMag(Eigen::Array<typename Eigen::FFT<double>::Complex, Eigen::Dynamic, 1> const &X, float fs, float fi, float fa);
};
#endif // IMPULSEPINKINV_H
