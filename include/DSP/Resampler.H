#ifndef RESAMPLER_H
#define RESAMPLER_H
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

#include "FIR.H" // for FIRDebug and Eigen includes
class ResamplerDebug : public FIRDebug {
public:
  ResamplerDebug(){
#ifndef NDEBUG
    errors[FIR_CHANNEL_MISMATCH_ERROR]=std::string("The input and output columns (channels) are not the same count. ");
#endif // NDEBUG
  }
};

/** Class which implements exact integer resapmling.
The resampled data is of type FRAME_TYPE. The original data is of any type
*/
template<typename FRAME_TYPE>
class Resampler {
  Eigen::FFT<FRAME_TYPE> fft;
  Eigen::Matrix<typename  Eigen::FFT<FRAME_TYPE>::Complex, Eigen::Dynamic, 1> X; ///< x in the DFT domain
  Eigen::Matrix<typename  Eigen::FFT<FRAME_TYPE>::Complex, Eigen::Dynamic, 1> Y; ///< y in the DFT domain
  Eigen::Matrix<FRAME_TYPE, Eigen::Dynamic, 1> xTemp; ///< x temporary storage
  Eigen::Matrix<FRAME_TYPE, Eigen::Dynamic, Eigen::Dynamic> yTemp; ///< y temporary storage
public:
    //Resampler(){} ///<Constructor
    virtual ~Resampler(){} ///< Destructor

    /** Resample x to y.
    Assumes that y is of FRAME_TYPE data. x can be any other type as it is cast to FRAME_TYPE
    */
    template<typename Derived, typename DerivedOther>
    int resample(const Eigen::DenseBase<Derived> &x, Eigen::DenseBase<DerivedOther> const &y){
      if (x.cols()!=y.cols())
        return ResamplerDebug().evaluateError(FIR_CHANNEL_MISMATCH_ERROR);
      if (X.rows()!=x.rows())
        X.resize(x.rows(),1);
      if (Y.rows()!=y.rows())
        Y.resize(y.rows(),1);
      if (yTemp.rows()!=y.rows())
        yTemp.resize(y.rows(),1);
      if (yTemp.cols()!=y.cols())
        yTemp.resize(y.rows(),y.cols());

      if (x.rows()==y.rows()){
        for (int c=0; c<x.cols(); c++)
          yTemp.col(c)=x.col(c). template cast<FRAME_TYPE>();
        // for (int c=0; c<x.cols(); c++){
        //   yTemp=x.column(c);
        //
        // }
      } else
        for (int c=0; c<x.cols(); c++){
          xTemp=x.col(c). template cast<FRAME_TYPE>();
          fft.fwd(X.data(), xTemp.data(), x.rows()); // find the DFT of x : X=dft(x)
          unsigned int N=floor((float)y.rows()/2.)+1;
          if (y.rows()>x.rows()){ // y is larger then x
            Y.setZero(); // this can be made more efficient by limiting the zeroing to only the samplres which require zeroing.
            N=floor((float)x.rows()/2.)+1;
          }
          Y.topRows(N)=X.topRows(N);
          Y.bottomRows(N-1)=X.bottomRows(N-1);
          // Y(N,0)*=2.; // is this necessary for the decimation case ?
          fft.inv(yTemp.col(c).data(), Y.data(), Y.rows()); // take back to the time domain
        }
      const_cast< Eigen::DenseBase<DerivedOther>& >(y)=yTemp;

      return 0;
    }
};
#endif // RESAMPLER_H
