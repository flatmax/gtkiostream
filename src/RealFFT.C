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
#include "fft/RealFFT.H"

void RealFFT::createPlan(void) {
    if (data) {
        //fftw3
        fwdPlan=fftw_plan_r2r_1d(data->getSize(), data->in, data->out, FFTW_R2HC, PLANTYPE);
        invPlan=fftw_plan_r2r_1d(data->getSize(), data->out, data->in, FFTW_HC2R, PLANTYPE);
    }
}

void RealFFT::destroyPlan(void) {
    if (data) {
        fftw_destroy_plan(fwdPlan);
        fftw_destroy_plan(invPlan);
    }
}

RealFFT::RealFFT(void) {
    data=NULL;
    createPlan();
}

RealFFT::RealFFT(RealFFTData *d) {
    //  cout <<"RealFFT init:"<<this<<std::endl;
    data=d;
    createPlan();
}

RealFFT::~RealFFT(void) {
    destroyPlan();
}

void RealFFT::switchData(RealFFTData *d) {
    destroyPlan();
    data=d;
    createPlan();
}

void RealFFT::switchData(RealFFTData &d) {
    switchData(&d);
}

void RealFFT::fwdTransform() {
    if (!data)
        printf("RealFFT::fwdTransform : data not present, please switch data");
    else
        fftw_execute(fwdPlan);
}

void RealFFT::invTransform() {
    if (!data)
        printf("RealFFT::invTransform : data not present, please switch data");
    else
        fftw_execute(invPlan);
}

RealFFTData RealFFT::groupDelay(RealFFTData &rfd){
  RealFFTData gd(rfd.getSize()); // correctly size the group delay object
  for (int i=0; i<rfd.getSize(); i++)
    gd.in[i]=rfd.in[i]*i;
  switchData(gd); // derive the DFT coefficients
  fwdTransform();
  for (int i=0; i<rfd.getSize(); i++)
    gd.in[i]=(gd.getComplexCoeff(i)/rfd.getComplexCoeff(i)).real();
  return gd;
}

#include "gtkiostream_config.h"
#ifdef HAVE_EMSCRIPTEN
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(RealFFT_ex) {
emscripten::class_<RealFFT>("RealFFT")
    .constructor() // empty constructor - requires switchData to be called
    .constructor<RealFFTData*>() // the constructor takes in a data class
    .function("switchData", emscripten::select_overload<void(RealFFTData&)>(&RealFFT::switchData))
    .function("fwdTransform", &RealFFT::fwdTransform)
    .function("invTransform", &RealFFT::invTransform)
    .function("groupDelay", &RealFFT::groupDelay);
}
#endif
