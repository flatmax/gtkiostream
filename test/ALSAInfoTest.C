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

#include "ALSA/ALSA.H"
#include <iostream>
using namespace std;

using namespace ALSA;

int main(int argc, char *argv[]) {
  const string deviceName="hw:0";
  PCM pcm;
  int res=pcm.open(deviceName.c_str(), SND_PCM_STREAM_PLAYBACK, 0); // open a pcm device could also be Capture or Playback classes
  if (res<0) // failure to open
    return res;
//    Playback pcm(deviceName.c_str()); // another way to open a pcm device
  cout<<"Opened the PCM device "<<deviceName<<endl;

  Info info;
  cout<<"Getting device info "<<endl;
  info.getInfo(pcm);

  cout<<"Card   Number     : "<< info.getCard()<<endl;
  cout<<"Device Number     : "<< info.getDevice()<<endl;
  cout<<"Sub-device Number : "<< info.getSubDevice()<<endl;
  cout<<"Short ID          : "<< info.getID()<<endl;
  cout<<"Name              : "<< info.getName()<<endl;
  cout<<"Sub-device name   : "<< info.getSubDeviceName()<<endl;

  return 0;
}
