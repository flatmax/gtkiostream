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

#include "Sox.H"

#define USE_INTERLEAVED

int main(int argc, char *argv[]) {
  if (argc<2){
    cout<<"Usage:\n"<<argv[0]<<" audioFileName"<<endl;
    return -1;
  }
  const string deviceName="hw:0,0";
  Playback playBack(deviceName.c_str());
  cout<<"opened the device "<<playBack.getDeviceName()<<endl;

  int res;
  // we don't want defaults so reset and refil the params ...
  playBack.resetParams();

  if ((res=playBack.setFormat(SND_PCM_FORMAT_S32_LE))<0)
    return ALSADebug().evaluateError(res);

#ifndef USE_INTERLEAVED
  cout<<"\n\nsetting non interleaved"<<endl;
  if ((res=playBack.setAccess(SND_PCM_ACCESS_RW_NONINTERLEAVED))<0)
    return ALSADebug().evaluateError(res);
#else
  cout<<"\n\nsetting interleaved"<<endl;
  if ((res=playBack.setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
    return ALSADebug().evaluateError(res);
#endif

  Sox<short int> sox;
  res=sox.openRead(argv[1]);
  if (res<0 && res!=SOX_READ_MAXSCALE_ERROR)
  return SoxDebug().evaluateError(res);

  unsigned int fs;
  if (sox.getFSIn()!=playBack.getSampleRate()){
    cout<<"sample rate mismatch, file = "<<sox.getFSIn()<<" Hz and ALSA = "<<playBack.getSampleRate()<<endl;
    if ((res=playBack.setSampleRate(sox.getFSIn()))<0)
      return ALSADebug().evaluateError(res);
    fs=playBack.getSampleRate();
  }
  cout<<"rates are now, file = "<<sox.getFSIn()<<" Hz and ALSA = "<<fs<<endl;

  int latency=2048;
  if ((res=playBack.setBufSize(latency))<0)
    return ALSADebug().evaluateError(res);

  cout<<"latency = "<<(float)latency/(float)fs<<" s"<<endl;

  if (playBack.getChannels()!=sox.getChCntIn())
    playBack.setChannels(sox.getChCntIn());

  playBack.setParams();
  snd_pcm_format_t format;
  playBack.getFormat(format);
  cout<<"format "<<playBack.getFormatName(format)<<endl;
  cout<<"channels "<<playBack.getChannels()<<endl;
  snd_pcm_uframes_t pSize;
  playBack.getPeriodSize(&pSize);
  cout<<"period size "<<pSize<<endl;

  if (!playBack.prepared()){
    cout<<"should be prepared, but isn't"<<endl;
    return -1;
  }

  // playBack.enableLog();
  //   cout<<"status"<<endl;
  // playBack.dumpStatus();
  //  cout<<"PCM"<<endl;
  //  playBack.dumpPCM();
  //  cout<<"setup"<<endl;
  //  playBack.dumpSetup();
  //  cout<<"\n\nHW setup"<<endl;
  //  playBack.dumpHWSetup();
  //  cout<<"\nHW params"<<endl;
  //  playBack.dumpHWParams();
  //  cout<<"\n\nSW setup"<<endl;
  //  playBack.dumpSWSetup();
  //  cout<<"\nSW params"<<endl;
  //  playBack.dumpSWParams();

  Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> buffer;
  size_t totalWritten=0;
  while (sox.read(buffer, pSize)>=0){
    if (buffer.rows()==0) // end of the file.
      break;
#ifndef USE_INTERLEAVED
    playBack.writeBufN(buffer);
#else
    playBack<<buffer; // play the audio data
    //playBack.writeBuf(buffer); // play the audio data
#endif
    totalWritten+=buffer.rows();
  }
  return 0;
}
