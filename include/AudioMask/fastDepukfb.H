/*
 libaudiomask - hybrid simultaneous audio masking threshold evaluation library
    Copyright (C) 2000-2018  Dr Matthew Raphael Flax

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FASTDEPUKFB_H_
#define FASTDEPUKFB_H_
#include <string.h>
#include "AudioMask/depukfb.H"

class FastDepUKFB : public DepUKFB {

  double n_l[2], d_l[3]; // Lower filter IIR coeff.
  double n_u[2], d_u[3]; // Upper filter IIR coeff.

  void findIIRCoeff(double fc, double pl, double pu){
    double c1, c2, c3, c4; // Numerator coefficients
    double d1, d2, d3, d4; // Denominator coefficients

    // Find the lower filter's IIR coefficients first
    c1=exp(pl/fc);
    c2=exp(pl);
    //c5=c1*c1;
    c4=c1*c2;
    c3=c1*c4;
    d_l[0]=(c2*fc); // Do this first so we can divide by it
    n_l[0]=(fc+fc*pl)/d_l[0]; // Numerator
    n_l[1]=(-c1*fc-c1*pl-c1*fc*pl)/d_l[0];
    d_l[1]=(-2*c4*fc)/d_l[0]; // Denominator
    d_l[2]=(c3*fc)/d_l[0];
    d_l[0]=1.0;

    // Find the upper filter's IIR coefficients
    d4=exp(pu/fc);
    d3=d4*d4;
    d2=exp(pu+pu/fc);
    d1=d2*d4;

    d_u[0]=(d3*fc);
    n_u[0]=(d1*fc-d1*fc*pu)/d_u[0]; // Numerator
    n_u[1]=d2*(-fc+pu+pu*fc)/d_u[0];
    d_u[1]=(-2*d4*fc)/d_u[0]; // Denominator
    d_u[2]=(fc)/d_u[0];
    d_u[0]=1.0;
  }

  void filter(double fc, double *out){
    //Second order impulse response

    // Reset the state vars and the output
    double z1=0.0, z2=0.0;
    bzero(out, (int)rint(fs/2.0)*sizeof(double));

    // Upper filter ...
    // First excite with a unit input ....
    out[0] = n_u[0] - d_u[0]*out[0] + z1;
    z1     = n_u[1] - d_u[1]*out[0] + z2;
    z2     =        - d_u[2]*out[0];

    for (int i=1;i<(int)rint(fs/2.0);i++){ // the input now equals zero
      out[i] = - d_u[0]*out[i] + z1;
      z1 =     - d_u[1]*out[i] + z2;
      z2 =     - d_u[2]*out[i];
    }

    // Reset the state vars and the lower output
    z1=z2=0.0;
    bzero(out, (int)rint(fc)*sizeof(double));

    // Lower filter ...
    // First excite with a unit input ....
    out[0] = n_l[0] - d_l[0]*out[0] + z1;
    z1     = n_l[1] - d_l[1]*out[0] + z2;
    z2     =        - d_l[2]*out[0];

    for (int i=1;i<(int)rint(fc);i++){ // the input now equals zero
      out[i] = - d_l[0]*out[i] + z1;
      z1 =     - d_l[1]*out[i] + z2;
      z2 =     - d_l[2]*out[i];
    }
  }

  void afZ(double fc, int whichFilter, double pl, double pu){
    double *filt=w[whichFilter];
    findIIRCoeff(fc, pl, pu); // Find the IIR coefficients to filter with
    filter(fc, filt); // Find the lower filter shape
  }

  /// Auditory Filter procedure
  virtual void af(double fc, int whichFilter){
    cout<<"FastDepUKFB::af"<<endl;

    // Produce the filter
    afZ(fc, whichFilter, p_l(fc), p_u(fc));
  }

public:
  FastDepUKFB(int sampleFreq, int fCnt=50) {
    init(sampleFreq, fCnt);
  }
};

#endif //FASTDEPUKFB_H_
