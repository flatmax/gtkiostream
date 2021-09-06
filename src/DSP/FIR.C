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

#include "DSP/FIR.H"

#ifdef HAVE_SOX
#ifndef HAVE_EMSCRIPTEN

#include <Sox.H>

template<typename FP_TYPE>
int FIR<FP_TYPE>::loadTimeDomainCoefficients(const std::string fileName, int whichCh){
  int ret=loadTimeDomainCoefficients(fileName); // load all channels and then select only the one requested
  if (ret<0)
    return ret;
  if (whichCh>(h.cols()-1))
    return SoxDebug().evaluateError(SOX_COL_BOUNDS_ERROR, " FIR: whichCh is > then the number of channels available in the loaded filter\n");
  loadTimeDomainCoefficients(h.col(whichCh));
  return 0;
}

template<typename FP_TYPE>
int FIR<FP_TYPE>::loadTimeDomainCoefficients(const std::string fileName){
  int ret=NO_ERROR;
  Sox<FP_TYPE> sox; // use sox to try to read the filter from file
  if ((ret=sox.openRead(string(fileName)))<0 && ret!=SOX_READ_MAXSCALE_ERROR) // try to open the file
      SoxDebug().evaluateError(ret, fileName);
  else {
      Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> hNew; // the time domain representation of the filter
      if (ret=sox.read(hNew)<0) // Try to read the entire file into the coefficient Matrix B.
          SoxDebug().evaluateError(ret, fileName);
      // else {
      //   if (chCnt>0 && hNew.cols() != chCnt)
      //     if (hNew.cols() > chCnt) // we need to shed rows from the matrix
      //       hNew=hNew.leftCols(chCnt);
      //     else { // the matrix needs expansion
      //       int colsO=hNew.cols();
      //       hNew.conservativeResize(hNew.rows(), chCnt);
      //       hNew.rightCols(chCnt-colsO).setZero();
      //       hNew.rightCols(chCnt-colsO).topRows(1).setOnes();
      //     }
      // cout<<hNew.topRows(4)<<endl;
      else
        loadTimeDomainCoefficients(hNew);
      // }
  }
  return ret;
}

template<typename FP_TYPE>
int FIR<FP_TYPE>::saveTimeDomainCoefficients(const std::string fileName, double fs){
  int ret=NO_ERROR;
  Sox<FP_TYPE> sox; // use sox to try to read the filter from file
  if (ret=sox.openWrite(fileName, fs, h.cols(), h.array().abs().maxCoeff())<0) // try to open the file
    return SoxDebug().evaluateError(ret, fileName);
  ret=sox.write(h);
  return ret;
}
#endif
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

// #include "gtkiostream_config.h"
#ifdef HAVE_EMSCRIPTEN
template<typename FP_TYPE>
void FIR<FP_TYPE>::loadTimeDomainCoefficients(intptr_t signal, size_t samples, size_t channels){
  Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Unaligned >
                                        h_signal((float*)signal, samples, channels);
  loadTimeDomainCoefficients(h_signal.cast<FP_TYPE>());
}

template<typename FP_TYPE>
void FIR<FP_TYPE>::filter_js(intptr_t input, intptr_t output, size_t samples, size_t channels) {
  Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Unaligned >
                                        in((float*)input, samples, channels);
  Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>, Eigen::Unaligned >
                                        out((float*)output, samples, channels);
  Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> outTemp(samples, channels);
  filter(in.cast<FP_TYPE>(), outTemp);
  out = outTemp.template cast<float>();
}

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(FIR_ex) {
emscripten::class_<FIR<double>>("FIR")
    .constructor() // empty constructor - requires switchData to be called
    .function("init", &FIR<double>::init)
    .function("getChannelCnt", &FIR<double>::getChannelCnt)
    .function("getN", &FIR<double>::getN)
    .function("loadTimeDomainCoefficients", emscripten::select_overload<void(intptr_t, size_t, size_t)>(&FIR<double>::loadTimeDomainCoefficients), emscripten::allow_raw_pointers())
    .function("filter", &FIR<double>::filter_js, emscripten::allow_raw_pointers())
    ;
}
#endif
