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
#ifndef COMPLEXFFT_H_
#define COMPLEXFFT_H_

#include "fft/FFTCommon.H"
#include "fft/ComplexFFTData.H"

//class ComplexFFTData;

///class ComplexFFT controls fftw plans and executes fwd/inv transforms
class ComplexFFT {
  /// The fwd/inv plans
  fftw_plan fwdPlan, invPlan;
  /// Method to create the plans
  void createPlan(void){
  if (data){
    //fftw3
    fwdPlan = fftw_plan_dft_1d(data->getSize(), data->in, data->out, FFTW_FORWARD, PLANTYPE);
    invPlan = fftw_plan_dft_1d(data->getSize(), data->out, data->in, FFTW_BACKWARD, PLANTYPE);
  }
}

  /// Method to destroy the plans
  void destroyPlan(void){
  if (data){
    fftw_destroy_plan(fwdPlan);
    fftw_destroy_plan(invPlan);
  }
}

protected:
  //  int size;
  /// The pointer to the relevant data
  ComplexFFTData *data;
public:

  /// fft init ... data pointed to by 'd'
  ComplexFFT(ComplexFFTData *d){
  //  std::cout <<"ComplexFFT init:"<<this<<std::endl;
  data=d;
  createPlan();
}

  /// fft deconstructor
  virtual ~ComplexFFT(){
  destroyPlan();
}


  /// Use this to change associated fft data (for fft'ing)
  void switchData(ComplexFFTData *d){
  //fftw_cleanup();
  destroyPlan();
  data=d;
  createPlan();
}


  /// Forward transform the data (in to out)
  void fwdTransform(){
  if (!data)
    std::cerr<<"ComplexFFT::fwdTransform : data not present, please switch data"<<std::endl;
  else
    fftw_execute(fwdPlan);
  /*fftw_execute_dft(
          fwdPlan,
          data->in, data->out);

	  }*/
}


  /// Inverse transform the data (out to in)
  void invTransform(){
  if (!data)
    std::cerr<<"ComplexFFT::invTransform : data not present, please switch data"<<std::endl;
  else
    fftw_execute(invPlan);
  /*fftw_execute_dft(
          invPlan,
          data->in, data->out);

	  }*/
}

};
/** \example ComplexFFTExample.C
 * This is an example of how to use the class.
 */
#endif // COMPLEXFFT_H_
