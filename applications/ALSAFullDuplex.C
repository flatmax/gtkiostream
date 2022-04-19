
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

#include "OptionParser.H"

int printUsage(string name, string devOut, string devIn, int latency) {
    cout<<name<<" : An application to playback an audio file and capture to file."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options]"<<endl;
    cout<<"\n\t\t The captured audio will be written to audioFileName.capture.wav"<<endl;
    cout<<"\t e.g. "<<name<<" [options] /tmp/out.wav"<<endl;
    cout<<"\t -D : The name of the output device : (-D "<<devOut<<")"<<endl;
    cout<<"\t -C : The name of the input  device : (-C "<<devIn<<")"<<endl;
    return 0;
}


class FullDuplexTest : public FullDuplex<int> {
	int N; ///< The number of frames
	int ch; ///< The number of channels

	/** Your class must inherit this class and implement the process method.
	The inputAudio and outputAudio variables should be resized to the number of channels
	and frames you want to process. Note that the number of frames must be the same for
	both the inputAudio and outputAudio variables.
	\return <0 on error, 0 to continue and >0 to stop.
	*/
	int process(){
		if (inputAudio.rows()!=N || inputAudio.cols()!=ch){
			inputAudio.resize(N, ch);
			outputAudio.resize(N, ch);
			inputAudio.setZero();
		}
		outputAudio=inputAudio; // copy the input to output.
		return 0; // return 0 to continue
	}
public:
  /** Specify the device name and the desired latency
  \param devName The device name, e.g. hw:0 or plughw:0
  \param latency The number of frames to buffer. For example 4096
  */
	FullDuplexTest(const char*devName, int latency) : FullDuplex(devName){
		init(latency);
	}

  /** Specify the device names and the desired latency
  \param devNameOut The device name, e.g. hw:0 or plughw:0
  \param devNameIn The device name, e.g. hw:0 or plughw:0
  \param latency The number of frames to buffer. For example 4096
  */
	FullDuplexTest(const char*devNameOut, const char*devNameIn, int latency) : FullDuplex(devNameOut, devNameIn){
    init(latency);
  }

	void init(int latency){
		ch=2; // use this static number of input and output channels.
		N=latency;
		inputAudio.resize(0,0); // force zero size to ensure resice on the first process.
		outputAudio.resize(0,0);
	}

	/** Overload the link method
	*/
	int link(){
		int ret = FullDuplex<int>::link();
		if (ret<0)
			printf("Linking failed. Continuing ...\n");
		return 0;
	}

	/** Overload the link method
	*/
	int unLink(){
		int ret = FullDuplex<int>::unLink();
		if (ret<0)
			printf("Unlinking failed. Continuing ...\n");
		return 0;
	}
};

int main(int argc, char *argv[]) {
  OptionParser op;
  int i=0, ret;

  string deviceNameOut="default";
  string deviceNameIn="default";
	int latency=2048;
	int fs=48000; // The sample rate
	cout<<"latency = "<<(float)latency/(float)fs<<" s"<<endl;

  string help;
  if (op.getArg<string>("D", argc, argv, deviceNameOut, i=0)!=0)
      ;
  if (op.getArg<string>("C", argc, argv, deviceNameIn, i=0)!=0)
      ;

  if (argc<2 || op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0], deviceNameOut, deviceNameIn, latency);
  if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
    return printUsage(argv[0], deviceNameOut, deviceNameIn, latency);


  cout<<"opening the playback device "<<deviceNameOut<<endl;
  cout<<"opening the capture  device "<<deviceNameIn<<endl;
  FullDuplexTest fullDuplex(deviceNameOut.c_str(), deviceNameIn.c_str(), latency);
  cout<<"opened the playback device "<<fullDuplex.Playback::getDeviceName()<<endl;
  cout<<"opened the capture device "<<fullDuplex.Capture::getDeviceName()<<endl;
	cout<<"channels max "<<fullDuplex.Playback::getMaxChannels()<<endl;

	// we don't want defaults so reset and refil the params ...
	int res=fullDuplex.resetParams();
	if (res<0)
		return res;

	snd_pcm_format_t format=SND_PCM_FORMAT_S32_LE;
	if ((res=fullDuplex.setFormat(format))<0)
		return res;

	res=fullDuplex.setAccess(SND_PCM_ACCESS_RW_INTERLEAVED);
	if (res<0)
		return res;

	if ((res=fullDuplex.setSampleRate(fs))<0)
		return res;

	res=fullDuplex.go(); // start the full duplex read/write/process going.
	return ALSADebug().evaluateError(res);
}
