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
#ifndef REALFFT_H_
#define REALFFT_H_

#include "fft/FFTCommon.H"
#include "fft/RealFFTData.H"

///class RealFFT controls fftw plans and executes fwd/inv transforms
class RealFFT {
    /// The fwd/inv plans
    fftw_plan fwdPlan, invPlan;

    /// Method to create the plans
    void createPlan(void) {
        if (data) {
            //fftw3
            fwdPlan=fftw_plan_r2r_1d(data->getSize(), data->in, data->out, FFTW_R2HC, PLANTYPE);
            invPlan=fftw_plan_r2r_1d(data->getSize(), data->out, data->in, FFTW_HC2R, PLANTYPE);
        }
    }

    /// Method to destroy the plans
    void destroyPlan(void) {
        if (data) {
            fftw_destroy_plan(fwdPlan);
            fftw_destroy_plan(invPlan);
        }
    }

protected:
    /// The pointer to the relevant data
    RealFFTData *data;
public:
    /// fft init ... don't forget to associate data using switchData
    RealFFT(void) {
        data=NULL;
        createPlan();
    }

    /** fft init ... data pointed to by 'd'
    \param d The data to use.,
    */
    RealFFT(RealFFTData *d) {
        //  cout <<"RealFFT init:"<<this<<std::endl;
        data=d;
        createPlan();
    }

    /// fft deconstructor
    virtual ~RealFFT(void) {
        destroyPlan();
    }

    /// Use this to change associated fft data (for fft'ing)
    void switchData(RealFFTData *d) {
        destroyPlan();
        data=d;
        createPlan();
    }

    /// Use this to change associated fft data (for fft'ing)
    void switchData(RealFFTData &d) {
        switchData(&d);
    }

    /// Forward transform the data (in to out)
    void fwdTransform() {
        if (!data)
            printf("RealFFT::fwdTransform : data not present, please switch data");
        else
            fftw_execute(fwdPlan);
    }

    /// Inverse transform the data (out to in)
    void invTransform() {
        if (!data)
            printf("RealFFT::invTransform : data not present, please switch data");
        else
            fftw_execute(invPlan);
    }
};
/** \example RealFFTExample.C
 * This is an example of how to use the class.
 */

 #include "config.h"
 #ifdef HAVE_EMSCRIPTEN
 #include <emscripten/bind.h>
 EMSCRIPTEN_BINDINGS(RealFFT_ex) {
   emscripten::class_<RealFFT>("RealFFT")
   .constructor() // empty constructor - requires switchData to be called
   .constructor<RealFFTData*>() // the constructor takes in a data class
   .function("switchData", emscripten::select_overload<void(RealFFTData&)>(&RealFFT::switchData))
   .function("fwdTransform", &RealFFT::fwdTransform)
   .function("invTransform", &RealFFT::invTransform);

 //    .function("getFS", &EQ::getFS)
 //    .property("x", &MyClass::getX, &MyClass::setX)
 //    .class_function("getStringFromInstance", &MyClass::getStringFromInstance)
 }
 #endif

#endif // REALFFT_H_
