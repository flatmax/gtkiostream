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
#include "fft/ComplexFFTData.H"
#include <stdlib.h>

ComplexFFTData::
ComplexFFTData(int sz) {
    size=sz;
    in = out = NULL;
    power_spectrum = NULL;

    //in = new fftw_complex[size];
    //out = new fftw_complex[size];
    //power_spectrum = new fftw_real[size];
    in = (fftw_complex*)fftw_malloc(size*sizeof(fftw_complex));
    out = (fftw_complex*)fftw_malloc(size*sizeof(fftw_complex));
    power_spectrum = (fftw_real*)fftw_malloc(size*sizeof(fftw_real));
    if (!in || !out || !power_spectrum) {
        printf("Could not allocate enough mem for a ComplexFFT\n");
        //if (in) delete [] in;
        //if (out) delete [] out;
        //if (power_spectrum) delete [] power_spectrum;
        if (in) fftw_free(in);
        in=NULL;
        if (out) fftw_free(out);
        out=NULL;
        if (power_spectrum) fftw_free(power_spectrum);
        power_spectrum=NULL;
        exit(-1);
    }
    totalPower = 0.0;
}

ComplexFFTData::~ComplexFFTData() {
    //if (in) delete [] in;
    //if (out) delete [] out;
    //if (power_spectrum) delete [] power_spectrum;
    if (in) fftw_free(in);
    in=NULL;
    if (out) fftw_free(out);
    out=NULL;
    if (power_spectrum) fftw_free(power_spectrum);
    power_spectrum=NULL;
}

int ComplexFFTData::compPowerSpec() {
    int bin;
    totalPower = 0.0;
    double min=MAXDOUBLE;
    double max = power_spectrum[bin=0] = c_re(out[0])*c_re(out[0])+c_im(out[0])*c_im(out[0]);  // DC component
    for (int k = 1; k < getSize(); ++k) {
        if ((power_spectrum[k] = c_re(out[k])*c_re(out[k]) + c_im(out[k])*c_im(out[k]))>max) {
            max=power_spectrum[bin=k];
        }
        if (power_spectrum[k]<min) min=power_spectrum[minPowerBin=k];
        totalPower += power_spectrum[k];
    }
    /*  if (getSize() % 2 == 0){ // N is even
      power_spectrum[getSize()/2] = out[getSize()/2]*out[getSize()/2];  // Nyquist freq.
      if (power_spectrum[getSize()/2]>max)
        max=power_spectrum[bin=getSize()/2];
      totalPower += power_spectrum[getSize()/2];
      }*/
    return bin;
}

int ComplexFFTData::sqrtPowerSpec() {
    double min=MAXDOUBLE;
    double max=-MAXDOUBLE;
    for (int k = 0; k < getSize(); ++k) { /* (k < N/2 rounded up) */
        if ((power_spectrum[k]=sqrt(power_spectrum[k]))>max)
            max=power_spectrum[maxPowerBin=k];
        if (power_spectrum[k]<min) min=power_spectrum[minPowerBin=k];
    }
    return maxPowerBin;
}
