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

#include "DSP/BandLimiter.H"

#ifdef HAVE_SOX
#include <Sox.H>
#ifndef HAVE_EMSCRIPTEN
template<typename FP_TYPE>
int BandLimiter<FP_TYPE>::saveToFile(const std::string fileName, float fs){
  Sox<FP_TYPE> sox; // use sox to write to file
  int ret;
  if (ret=sox.openWrite(fileName, fs, this->cols(), this->array().abs().maxCoeff())<0) // try to open the file
      return SoxDebug().evaluateError(ret, fileName);
  else
    return sox.write(*this);
  return ret;
}
#endif
#endif

template<typename FP_TYPE>
int BandLimiter<FP_TYPE>::HPBandlimit(float fi, float fs){
  if (this->rows()==0)
    return Debug().evaluateError(0,"BandLimiter::HPBandlimit no samples provided, returning.\n");
  int N = (int)floor(fi/(fs/(float)this->rows()));
  printf("N=%d\n",N);
  if (N<0)
    return Debug().evaluateError(0,"BandLimiter::HPBandlimit the frequency was so low, nothing was band limited.\n");
  Eigen::Matrix<typename Eigen::FFT<FP_TYPE>::Complex, Eigen::Dynamic, 1> H(this->rows(), 1);
  for (int m=0; m<this->cols(); m++){
    // fft.fwd(H.data(), this->col(m).data(), this->rows()); // find the DFT of the data
    fft.fwd(H, this->col(m)); // find the DFT of the data
     // cout<<H.transpose()<<endl;
    // cout<<this->col(m).transpose()<<endl;
    H.topRows(N).setZero();
    if (N>1)
      H.bottomRows(N-1).setZero();

    // cout<<'\n'<<H.transpose()<<endl;
    fft.inv(this->col(m).data(), H.data(), H.rows()); // find the iDFT of H
  }
  return 0;
}


template class BandLimiter<short int>;
template class BandLimiter<int>;
// template class BandLimiter<unsigned int>;
template class BandLimiter<float>;
template class BandLimiter<double>;
