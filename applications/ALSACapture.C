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
#include <strstream>
using namespace std;
using namespace ALSA;

#include "Sox.H"
#include "OptionParser.H"

int printUsage(string name, string dev, int chCnt, float T, int fs) {
    cout<<name<<" : An application to capture input to various audio file formats."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options] outFileName"<<endl;
    cout<<"\t e.g. "<<name<<" [options] /tmp/out.wav"<<endl;
    cout<<"\t -D : The name of the device : (-D "<<dev<<")"<<endl;
    cout<<"\t -c : The number of channels to open, if the available number is less, then it is reduced to the available : (-c "<<chCnt<<")"<<endl;
    cout<<"\t -t : The duration to sample for : (-t "<<T<<")"<<endl;
    cout<<"\t -r : The sample rate to use in Hz : (-r "<<fs<<")"<<endl;
    Sox<float> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;
    return 0;
}

int main(int argc, char *argv[]) {
  int chCnt=2; // The number of channels to record
  int fs=48000; // The sample rate
  float duration=2.1; // The number of seconds to record for
  string deviceName="hw:0";

  OptionParser op;
  int i=0, ret;
  string help;
  if (argc<2 || op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0], deviceName, chCnt, duration, fs);
  if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
    return printUsage(argv[0], deviceName, chCnt, duration, fs);

  if (op.getArg<int>("c", argc, argv, chCnt, i=0)!=0)
      ;

  if (op.getArg<string>("D", argc, argv, deviceName, i=0)!=0)
      ;

  if (op.getArg<float>("t", argc, argv, duration, i=0)!=0)
      ;

  if (op.getArg<int>("r", argc, argv, fs, i=0)!=0)
      ;

  // struct sched_param param;
  // param.sched_priority = 96;
  // if (sched_setscheduler(0, SCHED_FIFO, & param) == -1) {
  //     perror("sched_setscheduler");
  //     return -1;
  // }

  Capture capture(deviceName.c_str());
  cout<<"opened the device "<<capture.getDeviceName()<<endl;

  int res;
  // we don't want defaults so reset and refil the params ...
  capture.resetParams();

  snd_pcm_format_t format=SND_PCM_FORMAT_S32_LE;
  if ((res=capture.setFormat(format))<0)
    return ALSADebug().evaluateError(res);

  if ((res=capture.setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
    return ALSADebug().evaluateError(res);

  if ((res=capture.setChannels(chCnt))<0)
    return ALSADebug().evaluateError(res);

  Sox<int> sox;
  res=sox.openWrite(argv[argc-1], fs, capture.getChannels(), pow(2.,(double)snd_pcm_format_width(format)));
  if (res<0)
    return SoxDebug().evaluateError(res,string("when opening the file ")+argv[argc-1]);
  cout<<"opened the file : "<<argv[argc-1]<<" for writing"<<endl;

  if ((res=capture.setSampleRate(fs))<0)
    return ALSADebug().evaluateError(res);
  cout<<"rates are now, file = "<<sox.getFSIn()<<" Hz and ALSA = "<<capture.getSampleRate()<<endl;

  int latency=2048;
  if ((res=capture.setBufSize(latency))<0)
    return ALSADebug().evaluateError(res);

  cout<<"latency = "<<(float)latency/(float)fs<<" s"<<endl;

  capture.setParams();
  capture.getFormat(format);
  snd_pcm_uframes_t pSize;
  capture.getPeriodSize(&pSize);

  if (!capture.prepared()){
    cout<<"should be prepared, but isn't"<<endl;
    return -1;
  }

  Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> buffer(latency,chCnt);
  if ((res=capture.start())<0) // start the device capturing
    ALSADebug().evaluateError(res);

    cout<<"format "<<capture.getFormatName(format)<<endl;
    cout<<"channels "<<capture.getChannels()<<endl;
    cout<<"period size "<<pSize<<endl;

  int N=(int)floor(duration*(float)fs);
  while (N>0){
    if (latency>N) // make sure we don't read too much on the last read.
      latency=N;
    if (buffer.rows()==0) // end of the file.
      break;
    // write the audio data to file.
    int toWrite=(latency<=buffer.rows())?latency:N;
    capture>>buffer.block(0, 0, toWrite, chCnt);
    if ((ret=sox.write(buffer.block(0, 0, toWrite, chCnt)))!=toWrite*chCnt)
      return SoxDebug().evaluateError(ret);
    N-=latency;
  }
  sox.closeWrite();
  return 0;
}
