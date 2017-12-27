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
#ifndef REALFFT_H_
#define REALFFT_H_

#include "fft/FFTCommon.H"
#include "fft/RealFFTData.H"

///class RealFFT controls fftw plans and executes fwd/inv transforms
class RealFFT {
    /// The fwd/inv plans
    fftw_plan fwdPlan, invPlan;

    /// Method to create the plans
    void createPlan(void);

    /// Method to destroy the plans
    void destroyPlan(void);

protected:
    /// The pointer to the relevant data
    RealFFTData *data;
public:
    /// fft init ... don't forget to associate data using switchData
    RealFFT(void);

    /** fft init ... data pointed to by 'd'
    \param d The data to use.,
    */
    RealFFT(RealFFTData *d);

    /// fft deconstructor
    virtual ~RealFFT(void);

    /// Use this to change associated fft data (for fft'ing)
    void switchData(RealFFTData *d);

    /// Use this to change associated fft data (for fft'ing)
    void switchData(RealFFTData &d);

    /// Forward transform the data (in to out)
    void fwdTransform();

    /// Inverse transform the data (out to in)
    void invTransform();
};
/** \example RealFFTExample.C
 * This is an example of how to use the class.
 */
#endif // REALFFT_H_
