/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
  Capture capture(deviceName.c_str());
  cout<<"opened the device "<<capture.getDeviceName()<<endl;

  int chCnt=2; // The number of channels to record
  int fs=44100; // The sample rate
  float duration=2.1; // The number of seconds to record for

  int res;
  // we don't want defaults so reset and refil the params ...
  capture.resetParams();

  snd_pcm_format_t format=SND_PCM_FORMAT_S16_LE;
  if ((res=capture.setFormat(format))<0)
    return ALSADebug().evaluateError(res);

#ifndef USE_INTERLEAVED
  cout<<"\n\nsetting non interleaved"<<endl;
  if ((res=capture.setAccess(SND_PCM_ACCESS_RW_NONINTERLEAVED))<0)
    return ALSADebug().evaluateError(res);
#else
  cout<<"\n\nsetting interleaved"<<endl;
  if ((res=capture.setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
    return ALSADebug().evaluateError(res);
#endif

  if ((res=capture.setChannels(chCnt))<0)
    return ALSADebug().evaluateError(res);

  Sox<short int> sox;
  res=sox.openWrite(argv[1], fs, capture.getChannels(), pow(2.,(double)snd_pcm_format_width(format)));
  if (res<0)
    return SoxDebug().evaluateError(res);

  if ((res=capture.setSampleRate(fs))<0)
    return ALSADebug().evaluateError(res);
  cout<<"rates are now, file = "<<sox.getFSIn()<<" Hz and ALSA = "<<capture.getSampleRate()<<endl;

  int latency=2048;
  if ((res=capture.setBufSize(latency))<0)
    return ALSADebug().evaluateError(res);

  cout<<"latency = "<<(float)latency/(float)fs<<" s"<<endl;

  capture.setParams();
  capture.getFormat(format);
  cout<<"format "<<capture.getFormatName(format)<<endl;
  cout<<"channels "<<capture.getChannels()<<endl;
  snd_pcm_uframes_t pSize;
  capture.getPeriodSize(&pSize);
  cout<<"period size "<<pSize<<endl;

  if (!capture.prepared()){
    cout<<"should be prepared, but isn't"<<endl;
    return -1;
  }

  Eigen::Array<short int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> buffer(latency,chCnt);
  if ((res=capture.start())<0) // start the device capturing
    ALSADebug().evaluateError(res);

  int N=(int)floor(duration*(float)fs);
  while (N>0){
    if (latency>N) // make sure we don't read too much on the last read.
      latency=N;
    if (buffer.rows()==0) // end of the file.
      break;
#ifndef USE_INTERLEAVED
    capture.readBufN(buffer);
#else
    if (latency<buffer.rows())
      capture>>buffer.block(0, 0, latency, chCnt);
    else
      capture>>buffer; // capture the audio data
#endif
    // write the audio data to file.
    if (latency<buffer.rows())
      sox.write(buffer.block(0, 0, latency, chCnt));
    else
      sox.write(buffer);
    N-=latency;
  }
  sox.closeWrite();
  return 0;
}
