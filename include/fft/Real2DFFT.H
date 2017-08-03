/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
#ifndef REAL2DFFT_H_
#define REAL2DFFT_H_

#include "fft/FFTCommon.H"
#include "fft/Real2DFFTData.H"

///class Real2DFFT controls fftw plans and executes fwd/inv transforms
class Real2DFFT {
  /// The forward and inverse plans
  fftw_plan fwdPlan, invPlan;
protected:
  /// The pointer to the relevant data
  Real2DFFTData *data;
public:
  /// fft init ... data pointed to by 'd'
  Real2DFFT(Real2DFFTData *d){
  //std::cout <<"RealFFT init:"<<this<<std::endl;
  data=d;
  // std::cout <<data->getXSize() << '\t'<<data->getYSize()<<std::endl;
  fwdPlan = fftw_plan_dft_r2c_2d(data->getXSize(), data->getYSize(), data->in, data->out, PLANTYPE);
  invPlan = fftw_plan_dft_c2r_2d(data->getXSize(), data->getYSize(), data->out, data->in, PLANTYPE);
}

  /// fft deconstructor
  virtual ~Real2DFFT(){
  //  std::cout <<"RealFFT DeInit:"<<this<<std::endl;
  fftw_destroy_plan(fwdPlan);
  fftw_destroy_plan(invPlan);
  std::cout <<"RealFFT DeInit done"<<std::endl;
}


  /// Forward transform the data (in to out)
  void fwdTransform(){
  if (!data)
    std::cerr<<"Real2DFFT::fwdTransform : data not present"<<std::endl;
  else
    fftw_execute(fwdPlan);
}

  /// Inverse transform the data (out to in)
  void invTransform(){
  if (!data)
    std::cerr<<"Real2DFFT::invTransform : data not present"<<std::endl;
  else
    fftw_execute(invPlan);
}

};
/** \example Real2DFFTExample.C
 * This is an example of how to use the class.
 */
#endif // REAL2DFFT_H_
