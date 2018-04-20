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

#include "DSP/FIR.H"

#ifdef HAVE_SOX
#include <Sox.H>

template<typename FP_TYPE>
int FIR<FP_TYPE><FP_TYPE>::loadTimeDomainCoefficients(const std::string fileName){
  int ret=NO_ERROR;
  Sox<FP_TYPE> sox; // use sox to try to read the filter from file
  if ((ret=sox.openRead(string(fileName)))<0 && ret!=SOX_READ_MAXSCALE_ERROR) // try to open the file
      SoxDebug().evaluateError(ret, fileName);
  else {
      Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> hNew; // the time domain representation of the filter
      if (ret=sox.read(hNew)<0) // Try to read the entire file into the coefficient Matrix B.
          SoxDebug().evaluateError(ret, fileName);
      else
          loadTimeDomainCoefficients(hNew);
  }
  return ret;
}
#endif

template<typename FP_TYPE>
void FIR<FP_TYPE>::loadTimeDomainCoefficients(const Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> hIn){
  h=hIn;
  resetDFT();
}

template<typename FP_TYPE>
void FIR<FP_TYPE>::resetDFT(){
  // only reset the DFT if both block size and filter h are defined.
  if (N==0 || h.rows()<=0 || h.cols() <=0)
    return;
  // Find the DFT of h and store in H
  Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> hNew(h.rows()+N, h.cols());
  x.setZero(hNew.rows(), hNew.cols()); // make the input signal the same length as H
  yTemp.setZero(hNew.rows(), 1); // make the temporary output buffer the same length as H
  y.setZero(hNew.rows(), hNew.cols()); // make the temporary output buffer the same length as H
  Y.setZero(hNew.rows(), 1); // make the DFT of the input signal the same length as H
  hNew.setZero();
  hNew.topRows(h.rows())=h;
  H.setZero(hNew.rows(), hNew.cols());
  for (int i=0; i<hNew.cols(); i++)
    fft.fwd(H.col(i).data(), hNew.col(i).data(), hNew.rows());
}

template<typename FP_TYPE>
void FIR<FP_TYPE>::init(unsigned int blockSize){
  N=blockSize;
  resetDFT();
}

template class FIR<float>;
template class FIR<double>;
