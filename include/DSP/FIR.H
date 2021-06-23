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

#ifndef FIR_H
#define FIR_H

#include "gtkiostream_config.h"
#include "Debug.H"
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>

#define FIR_BLOCKSIZE_MISMATCH_ERROR FIR_ERROR_OFFSET-1
#define FIR_H_EMPTY_ERROR FIR_ERROR_OFFSET-2
#define FIR_CHANNEL_MISMATCH_ERROR FIR_ERROR_OFFSET-3

/** Debug class for the FIR class
*/
class FIRDebug :  virtual public Debug  {
public:
    FIRDebug(){
#ifndef NDEBUG
errors[FIR_BLOCKSIZE_MISMATCH_ERROR]=std::string("The input data was not of the same length you used as the variable for the method init. ");
errors[FIR_H_EMPTY_ERROR]=std::string("The fileter h is empty, please load using loadTimeDomainCoefficients. ");
errors[FIR_CHANNEL_MISMATCH_ERROR]=std::string("The input, output and h columns (channels) are not the same count. ");

#endif // NDEBUG
    }
};

/** An FIR filter implemented using the overlap add algorithm.

You need to load the time domain coefficients using (loadTimeDomainCoefficients).
You need to define the window size N for the algorithm using init.
This assumes that all input audio data (which is filtered) will be of the same
window size (block size) as the time domain coefficients.
Call the filter method with input to convolve with h to produce the output.
\example FIRTest.C
*/
template<typename FP_TYPE>
class FIR {
  Eigen::FFT<FP_TYPE> fft; ///< The fast Fourier transform
  Eigen::Array<typename Eigen::FFT<FP_TYPE>::Complex, Eigen::Dynamic, Eigen::Dynamic> H; ///< The DFT of the FIR coefficients
  Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> h; ///< the time domain representation of the filter
  Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> x; ///< the time domain signal for filtering
  Eigen::Matrix<FP_TYPE, Eigen::Dynamic, 1> yTemp; ///< the time domain signal for filtering
  Eigen::Array<typename Eigen::FFT<FP_TYPE>::Complex, Eigen::Dynamic, 1> Y; ///< the time domain filter output and also the DFT of one col of x

  /** Resets the H matrix once N or h is changed.
  */
  void resetDFT();
protected:
  unsigned int N; ///< Block size of the audio subsystem
  Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> y; ///< the time domain output signal
public:
    FIR(){N=0;} ///< Constructor

    /** Initialise the input audio frame count (window size or block size)
    \param blockSize The block size.
    */
    void init(unsigned int blockSize);

#ifdef HAVE_SOX
#ifndef HAVE_EMSCRIPTEN
    /** Method to read time domain coefficients from file, convert to the Fourier domain and Construct the necessary data types.
    You may pass any audio file which can be read by the class Sox - it can read a very large number of audio file formats.
    \param fileName The name of the file to load the time domain coefficients from
    \param whichCh The channel to use, if we load 10 channels and whichCh=0, then only store the first channel, if whichCh=2 then only store the third channel.
    \return Negative value on error.
    */
    int loadTimeDomainCoefficients(const std::string fileName, int whichCh);

    /** Method to read time domain coefficients from file, convert to the Fourier domain and Construct the necessary data types.
    You may pass any audio file which can be read by the class Sox - it can read a very large number of audio file formats.
    \param fileName The name of the file to load the time domain coefficients from
    \return Negative value on error.
    */
    int loadTimeDomainCoefficients(const std::string fileName);
#endif
#endif

    /** Method to load time domain coefficients from Matrix, convert to the Fourier domain and Construct the necessary data types.
    \param h The Matrix with time domain coefficients. Each column is a different channel
    \return Negative value on error.
    */
    void loadTimeDomainCoefficients(const Eigen::Matrix<FP_TYPE, Eigen::Dynamic, Eigen::Dynamic> hIn);

    /** Convolve the input with h producing the output.
    Each column is a channel and then number of input, output and h channels must match.
    \param input The input signal of block size N where N is defined by calling init, each column is a different channel
    \param output  The output signal of block size N where N is defined by calling init, each column is a different channel
    */
    template<typename Derived, typename DerivedOther>
    void filter(const Eigen::MatrixBase<Derived> &input, Eigen::DenseBase<DerivedOther> const &output) {
      if (input.rows()!=N){
        FIRDebug().evaluateError(FIR_BLOCKSIZE_MISMATCH_ERROR);
        return;
      }
      if (input.cols()!=h.cols() || output.cols() != h.cols()){
        printf("input.cols() %d output.cols() %d h.cols() %d\n",input.cols(), output.cols(), h.cols());
        FIRDebug().evaluateError(FIR_CHANNEL_MISMATCH_ERROR);
        return;
      }
      if (h.rows()==0) {
        FIRDebug().evaluateError(FIR_H_EMPTY_ERROR);
        return;
      }

      if (x.cols() != input.cols()){ // resize if necessary
        x.setZero(h.rows(), input.cols());
        y.setZero(h.rows(), input.cols());
      }
      y.topRows(y.rows()-N)=y.bottomRows(y.rows()-N); // keep the residual
      y.bottomRows(N).setZero();

      x.topRows(N)=input;

      for (int i=0; i<x.cols(); i++){ // perform the filter on each column
        fft.fwd(Y.data(), x.col(i).data(), x.rows()); // find the DFT of X=Z=dft(x) (store in Y)
        Y*=H.col(i); // convolve X (which is Y) with H
        fft.inv(yTemp.data(), Y.data(), Y.rows()); // take back to the time domain
        y.col(i)+=yTemp; // add to the residual
      }
      const_cast< Eigen::DenseBase<DerivedOther>& >(output)=y.topRows(N);
    }

    /** Get the number of channels in h
    \return The number of channels (columns) in h.
    */
    int getChannelCnt(){return h.cols();}

    /** Get the sample count of the filters
    \return the number of samples in a channel's filter.
    */
    int getN(){return h.rows();}
};
#endif // FIR_H
