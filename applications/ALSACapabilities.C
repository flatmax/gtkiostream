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
#include <sstream>
using namespace std;
using namespace ALSA;

#include "OptionParser.H"

class FullDuplexCard : public FullDuplex<int> {
  /** FullDuplex::process is a virtual method. Implement a dummy method here.
  */
  int process(){
    return 0; // return 0 to continue
  }

public:
  FullDuplexCard(const char*devName) : FullDuplex(devName){
  }
};


int printUsage(string name, string dev) {
    cout<<name<<" : An application to Playback input to various audio file formats."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t -D : The name of the device : (-D "<<dev<<")"<<endl;
    return 0;
}

int main(int argc, char *argv[]) {
  string deviceName="default";

  OptionParser op;
  int i=0, ret;
  string help;
  if (argc<2 || op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0], deviceName);
  if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
    return printUsage(argv[0], deviceName);

  if (op.getArg<string>("D", argc, argv, deviceName, i=0)!=0)
      ;

  FullDuplexCard card(deviceName.c_str());
  card.resetParams();
  cout<<"opened the device "<<card.Playback::getDeviceName()<<endl;
  cout<<"\n\n\n\n\n";
  cout<<"Playback channels : ["<<card.Playback::getMinChannels()<<", "<<card.Playback::getMaxChannels()<<"]"<<endl;
  cout<<"Playback fs : ["<<card.Playback::getMinSampleRate()<<", "<<card.Playback::getMaxSampleRate()<<"] Hz"<<endl;
  cout<<"Playback buf size : ["<<card.Playback::getMinBufSize()<<", "<<card.Playback::getMaxBufSize()<<"] "<<endl;
  cout<<"Playback period size : ["<<card.Playback::getMinPeriodSize()<<", "<<card.Playback::getMaxPeriodSize()<<"] "<<endl;
  // card.Playback::printFormatsFromMask(card.Playback::getFormatMask());
  // card.Playback::printFormatsFromMask();
  card.Playback::printAvailableFormats();
  // cout<<"\n\n\n\n\n";
  // card.Playback::dumpState();
  cout<<"\n\n\n\n\n";
  return 0;
}
