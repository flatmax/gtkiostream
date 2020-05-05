
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

#include "Sox.H"

class FullDuplexTest : public FullDuplex<int> {
	#define CH_CNT 8
	int N; ///< The number of frames
	int ch; ///< The number of channels
	int ch7Offset=-3; ///< The channel 7 offset from zero
	Eigen::Array<int, Eigen::Dynamic, CH_CNT> shiftedData;

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
			shiftedData.resize(N, ch);
			inputAudio.setZero();
			return 0;
		}

		// if (ch7Offset<0) { // only do this once
			// find the zero columns (for the Audio Injector Octo these are actually -256 not zero)
			Eigen::Array<int, 1, CH_CNT> mins = inputAudio.colwise().minCoeff();
			if (mins(0)==mins(CH_CNT-1)==-256)
				ch7Offset=7;
			else
				for(int i=0; i<mins.cols()-1; ++i)
		    	if(mins(i)==-256 && mins(i+1)==-256){
						ch7Offset=i;
						break;
					}
					// cout<<mins<<'\t'<<ch7Offset<<'\n';
					// printf("ch7Offset %d \n",ch7Offset);
		// }

		// if necessary circularly shift audio channels to realign input
		if (ch7Offset!=6) // when it is 6, it is in the right location so skip that case
			if (ch7Offset!=7){ // here we need a variable amount of circular column shifting
				shiftedData.leftCols(CH_CNT-ch7Offset-2)=inputAudio.rightCols(CH_CNT-ch7Offset-2);
				shiftedData.rightCols(ch7Offset+2)=inputAudio.leftCols(ch7Offset+2);
			} else { // ch 7 is in ch 8's position so circularly shift by one
				shiftedData.leftCols(CH_CNT-1)=inputAudio.rightCols(CH_CNT-1);
				shiftedData.rightCols(1)=inputAudio.leftCols(1);
			}
		// mins = shiftedData.colwise().minCoeff();
		// cout<<mins<<"\n\n\n";
		// inputAudio=shiftedData;
		outputAudio=shiftedData; // copy the shifted data out
		return 0; // return 0 to continue
	}
public:
	FullDuplexTest(const char*devName, int latency) : FullDuplex(devName){
		ch=CH_CNT; // use this static number of input and output channels.
		N=latency;
		inputAudio.resize(0,0); // force zero size to ensure resice on the first process.
		outputAudio.resize(0,0);
	}
};

int main(int argc, char *argv[]) {
	int latency=4096;
	int fs=48000; // The sample rate
	cout<<"latency = "<<(float)latency/(float)fs<<" s"<<endl;

	const char deviceName[]="hw:0";
	FullDuplexTest fullDuplex(deviceName, latency);
	cout<<"opened the device "<<fullDuplex.Playback::getDeviceName()<<endl;
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
