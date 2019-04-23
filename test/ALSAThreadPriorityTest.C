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

#include "ALSA/ALSA.H"
#include <iostream>
using namespace std;
using namespace ALSA;

int main(int argc, char *argv[]) {
  const string deviceName="hw:0,0";
  Playback playBack(deviceName.c_str());
  cout<<"opened the device "<<playBack.getDeviceName()<<endl;

  int res=playBack.changeThreadPriority(-1);
  if (res<0)
    printf("Error changing the thread priority\n");
  return 0;
}
