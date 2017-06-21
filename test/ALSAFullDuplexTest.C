
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

class FullDuplexTest : public FullDuplex {
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
			cout<<"resize"<<endl;
			inputAudio.resize(N, ch);
			outputAudio.resize(N, ch);
		}
		outputAudio=inputAudio; // copy the input to output.
		return 0; // return 0 to continue
	}
public:
	FullDuplexTest(const char*devName, int latency) : FullDuplex(devName){
		ch=2; // use this static number of input and output channels.
		N=latency;
		inputAudio.resize(0,0); // force zero size to ensure resice on the first process.
		outputAudio.resize(0,0);
	}
};

int main(int argc, char *argv[]) {
	int latency=2048;
	int fs=44100; // The sample rate
	float duration=2.1; // The number of seconds to record for
	snd_pcm_format_t format=SND_PCM_FORMAT_S16_LE;
	cout<<"latency = "<<(float)latency/(float)fs<<" s"<<endl;

	const string deviceName="hw:0";
	FullDuplexTest fullDuplex(deviceName.c_str(), latency);
	cout<<"opened the device "<<fullDuplex.Playback::getDeviceName()<<endl;

	int res;
	// we don't want defaults so reset and refil the params ...
	fullDuplex.Playback::resetParams();
	fullDuplex.Capture::resetParams();

	if ((res=fullDuplex.Playback::setFormat(format))<0)
		return ALSADebug().evaluateError(res);
	if ((res=fullDuplex.Capture::setFormat(format))<0)
		return ALSADebug().evaluateError(res);

#ifndef USE_INTERLEAVED
	cout<<"\n\nsetting non interleaved"<<endl;
	if ((res=fullDuplex.Playback::setAccess(SND_PCM_ACCESS_RW_NONINTERLEAVED))<0)
		return ALSADebug().evaluateError(res);
	if ((res=fullDuplex.Capture::setAccess(SND_PCM_ACCESS_RW_NONINTERLEAVED))<0)
		return ALSADebug().evaluateError(res);
#else
	cout<<"\n\nsetting interleaved"<<endl;
	if ((res=fullDuplex.Playback::setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
		return ALSADebug().evaluateError(res);
	if ((res=fullDuplex.Capture::setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
		return ALSADebug().evaluateError(res);
#endif

	if ((res=fullDuplex.Playback::setSampleRate(fs))<0)
		return ALSADebug().evaluateError(res);
	if ((res=fullDuplex.Capture::setSampleRate(fs))<0)
		return ALSADebug().evaluateError(res);

	res=fullDuplex.go(); // start the full duplex read/write/process going.
	return ALSADebug().evaluateError(res);
}
