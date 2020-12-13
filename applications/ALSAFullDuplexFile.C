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

#include "OptionParser.H"

int printUsage(string name, string dev, int latency) {
    cout<<name<<" : An application to playback an audio file and capture to file."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options] audioFileName"<<endl;
    cout<<"\n\t\t The captured audio will be written to audioFileName.capture.wav"<<endl;
    cout<<"\t e.g. "<<name<<" [options] /tmp/out.wav"<<endl;
    cout<<"\t -D : The name of the device : (-D "<<dev<<")"<<endl;
    Sox<float> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;
    return 0;
}

class FullDuplexTest : public FullDuplex<int> {
	int N; ///< The number of frames
	int ch; ///< The number of channels

  Sox<int> sox; ///< The output audio file
  Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> buffer; // audio buffer for sox

	/** Your class must inherit this class and implement the process method.
	The inputAudio and outputAudio variables should be resized to the number of channels
	and frames you want to process. Note that the number of frames must be the same for
	both the inputAudio and outputAudio variables.
	\return <0 on error, 0 to continue and >0 to stop.
	*/
	int process(){
    int res=0;
		if (inputAudio.rows()!=N || inputAudio.cols()!=ch){
			inputAudio.resize(N, ch);
			outputAudio.resize(N, ch);
			inputAudio.setZero();
		}
		if ((res=sox.read(outputAudio, N))<0)
		  if (outputAudio.rows()==0) // end of the file.
		    return 1; // indicate end of file exit

		cout<<"\rinput PCM maxima "<<inputAudio.square().colwise().mean().sqrt()<<'\n'; // print the input audio max coeff

		return 0; // return 0 to continue
	}
public:
  /** Specify the device name and the desired latency
  \param devName The device name, e.g. hw:0 or plughw:0
  \param latency The number of frames to buffer. For example 4096
  */
	FullDuplexTest(const char*devName, int latency) : FullDuplex(devName){
		ch=2; // use this static number of input and output channels.
		N=latency;
		inputAudio.resize(0,0); // force zero size to ensure resice on the first process.
		outputAudio.resize(0,0);
	}

  /** Open an audio file for output. Setup the full duplex hw params.
  \param name The name of the audio file to open.
  \returns 0 on success, otherwise an error.
  */
  int openFile(string name){
    int res=sox.openRead(name);
    if (res<0 && res!=SOX_READ_MAXSCALE_ERROR)
    return SoxDebug().evaluateError(res);

    unsigned int fs;
    if (sox.getFSIn()!=Playback::getSampleRate()){
      cout<<"sample rate mismatch, file = "<<sox.getFSIn()<<" Hz and ALSA = "<<Playback::getSampleRate()<<endl;
      cout<<"fixing sample rate mismatch"<<endl;
      if ((res=setSampleRate(sox.getFSIn()))<0)
        return ALSADebug().evaluateError(res);
      fs=Playback::getSampleRate();
    }
    cout<<"rates are now, file = "<<sox.getFSIn()<<" Hz and ALSA = "<<fs<<endl;

    ch=sox.getChCntIn();
    cout<<"setting ALSA channels to " <<ch<<endl;
    return 0;
  }
};

int main(int argc, char *argv[]) {
  string deviceName="hw:0";
  int latency=8192;

  OptionParser op;
  int i=0, ret;
  string help;
  if (argc<2 || op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0], deviceName, latency);
  if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
    return printUsage(argv[0], deviceName, latency);

  if (op.getArg<string>("D", argc, argv, deviceName, i=0)!=0)
      ;

  FullDuplexTest fullDuplex(deviceName.c_str(), latency);
  cout<<"opened the device "<<fullDuplex.Playback::getDeviceName()<<endl;
  cout<<"channels max "<<fullDuplex.Playback::getMaxChannels()<<endl;

  int res;
  // we don't want defaults so reset and refil the params ...
  fullDuplex.resetParams();

  if ((res=fullDuplex.setFormat(SND_PCM_FORMAT_S32_LE))<0)
    return ALSADebug().evaluateError(res);

  if ((res=fullDuplex.setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
    return ALSADebug().evaluateError(res);

  if ((res=fullDuplex.openFile(argv[1]))<0)
    return ALSADebug().evaluateError(res);

  if ((res=fullDuplex.setBufSize(latency))<0)
    return ALSADebug().evaluateError(res);

  cout<<"latency = "<<(float)latency/(float)fullDuplex.Playback::getSampleRate()<<" s"<<endl;


  snd_pcm_format_t format;
  fullDuplex.Playback::getFormat(format);
  cout<<"format "<<fullDuplex.Playback::getFormatName(format)<<endl;
  cout<<"channels "<<fullDuplex.Playback::getChannels()<<endl;
  snd_pcm_uframes_t pSize;
  fullDuplex.Playback::getPeriodSize(&pSize);
  cout<<"period size "<<pSize<<endl;

  res=fullDuplex.go(); // start the full duplex read/write/process going.
	return ALSADebug().evaluateError(res);
}
