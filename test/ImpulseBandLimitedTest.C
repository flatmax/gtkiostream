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

#include "DSP/ImpulseBandLimited.H"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]){

  float s=1.;
  float fs=48000.;
  float fi=100.;
  float fa=10000.;

  ImpulseBandLimited<int> ibl;
  int ret = ibl.generateImpulse(s, fs, fi, fa);
#ifdef HAVE_SOX
  ibl.saveToFile("/tmp/test.wav", fs);
#endif
  if (ret>0)
    return ret;
  ret = ibl.generateImpulseShift(s, fs, fi, fa);
#ifdef HAVE_SOX
  ibl.saveToFile("/tmp/testShifted.wav", fs);
#endif
  return ret;
}
