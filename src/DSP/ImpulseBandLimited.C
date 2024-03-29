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

#include "DSP/ImpulseBandLimited.H"
#include <limits>

using namespace Eigen;

#ifdef HAVE_SOX
#include <Sox.H>

#ifndef HAVE_EMSCRIPTEN
template<typename FP_TYPE>
int ImpulseBandLimited<FP_TYPE>::saveToFile(const std::string fileName, float fs){
  Sox<FP_TYPE> sox; // use sox to write to file
  int ret;
  if (ret=sox.openWrite(fileName, fs, this->cols(), this->abs().maxCoeff())<0) // try to open the file
      return SoxDebug().evaluateError(ret, fileName);
  else
    return sox.write(*this);
  return ret;
}
#endif
#endif

template<typename FP_TYPE>
int ImpulseBandLimited<FP_TYPE>::generateImpulseShift(float s, float fs, float fi, float fa){
  int ret=generateImpulse(s, fs, fi, fa);
  if (ret<0)
    return ret;
  // circularly shift the impulse response
  Array<FP_TYPE, Dynamic, 1> shifted(this->rows(), this->cols());
  int Nb = (int)ceil((double)this->rows()/2.);
  int Nt = (int)floor((double)this->rows()/2.);
  shifted.topRows(Nt) = this->bottomRows(Nt);
  shifted.bottomRows(Nb) = this->topRows(Nb);
  *static_cast<Array<FP_TYPE, Dynamic, 1>*>(this) = shifted;
  return 0;
}

template<typename FP_TYPE>
void ImpulseBandLimited<FP_TYPE>::setMag(Array<typename FFT<double>::Complex, Dynamic, 1> const &X, float fs, float fi, float fa){
  // zero out of band data
  int N=X.rows(); // the number of samples
  int cnt=(int)(fi/(fs/(float)N));
  if (cnt>0)
    const_cast< Array<typename FFT<double>::Complex, Dynamic, 1>& >(X).block(0,0,cnt,1).setZero(); // low freq
  int start=(N-(int)(fi/(fs/(float)N))+1);
  cnt=(N-1)-start+1;
  if (cnt>0)
    const_cast< Array<typename FFT<double>::Complex, Dynamic, 1>& >(X).block(start,0,cnt,1).setZero(); // high freq
  start=(int)(fa/(fs/(float)N))+1;
  cnt=(int)((fs-fa)/(fs/(float)N))-1-start+1;
  if (cnt>0)
    const_cast< Array<typename FFT<double>::Complex, Dynamic, 1>& >(X).block(start,0,cnt,1).setZero(); // mid freq
}

template<typename FP_TYPE>
int ImpulseBandLimited<FP_TYPE>::generateImpulse(float s, float fs, float fi, float fa){
  if (s<=0. || fs<=0. || fi<0. || fa>fs/2.)
    return Debug().evaluateError(EINVAL, "Duration is incorrect, ensure s>0");
  if (fs<=0.)
    return Debug().evaluateError(EINVAL, "Sample rate is incorrect, ensure fs>0");
  if (fi>fs/2 || fi<0)
    return Debug().evaluateError(EINVAL, "Minimum frequency is incorrect, ensure fs/2>=fi>=0");
  if (fa>fs/2. || fa<0)
    return Debug().evaluateError(EINVAL, "Maximum frequency is incorrect, ensure fs/2>=fa>=0");
  int N=round(s*fs); // the number of samples'
  Array<double, Dynamic, 1> x(N,1);
  x.setZero(); // initialise the impulse
  x(0,0)=1.;
  Array<typename FFT<double>::Complex, Dynamic, 1> X(x.rows(), 1); // The DFT of the x
  FFT<double> fft; // The fast Fourier transform
  fft.fwd(X.data(), x.data(), x.rows()); // find the DFT of x

  setMag(X, fs, fi, fa); // adjust the magnitude

  fft.inv(x.data(), X.data(), X.rows()); // find the iDFT of X
  if (0. == (double)((FP_TYPE)0.1)) // scale if necessary (i.e. FP_TYPE is not a floating point type)
    x*=std::numeric_limits<FP_TYPE>::max();
  *(Array<FP_TYPE, Dynamic, 1>*)this=x.cast<FP_TYPE>(); // copy over
  return 0;
}

template class ImpulseBandLimited<short int>;
template class ImpulseBandLimited<int>;
// template class ImpulseBandLimited<unsigned int>;
template class ImpulseBandLimited<float>;
template class ImpulseBandLimited<double>;
