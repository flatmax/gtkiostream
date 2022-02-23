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

#include <limits>

int printUsage(string name, string devOut, string devIn, int latency) {
    cout<<name<<" : An application to playback an audio file and capture to file."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options] audioFileName"<<endl;
    cout<<"\n\t\t The captured audio will be written to audioFileName.capture.wav"<<endl;
    cout<<"\t e.g. "<<name<<" [options] /tmp/out.wav"<<endl;
    cout<<"\t -D : The name of the output device : (-D "<<devOut<<")"<<endl;
    cout<<"\t -C : The name of the input  device : (-C "<<devIn<<")"<<endl;
    Sox<float> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;
    return 0;
}

class FullDuplexFile : public FullDuplex<int> {
	int N; ///< The number of frames
	int ch; ///< The number of channels

  Sox<int> sox; ///< The output audio file and the capture file
  Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> buffer; // audio buffer for sox

	/** Your class must inherit this class and implement the process method.
	The inputAudio and outputAudio variables should be resized to the number of channels
	and frames you want to process. Note that the number of frames must be the same for
	both the inputAudio and outputAudio variables.
	\return <0 on error, 0 to continue and >0 to stop.
	*/
	int process(){
    int res=0;
		if (inputAudio.rows()!=N || inputAudio.cols()!=ch || outputAudio.rows()!=N || outputAudio.cols()!=ch){
      cout<<"FulleDuplexTest::process : resetting block sizes to "<<N<<", "<<ch<<endl;
			inputAudio.resize(N, ch);
			outputAudio.resize(N, ch);
			inputAudio.setZero();
		}

    // cout<<"\rinput PCM maxima "<<inputAudio.maxCoeff()<<'\n'; // print the input audio max coeff
    if ((res=sox.write(inputAudio))!=inputAudio.rows()*inputAudio.cols())
        return SoxDebug().evaluateError(res);

    // printf("process : inputAudio.rows = %ld, outputAudio.rows = %ld\n", inputAudio.rows(), outputAudio.rows());
		if ((res=sox.read(outputAudio, N))<0)
      return SoxDebug().evaluateError(res);
	  if (outputAudio.rows()!=N) // end of the file. output audio end will be truncated
	    return 1; // indicate end of file exit

		return 0; // return 0 to continue
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
public:
  /** Specify the device name and the desired latency
  \param devName The device name, e.g. hw:0 or plughw:0
  \param latency The number of frames to buffer. For example 4096
  */
	FullDuplexFile(const char*devName, int latency) : FullDuplex(devName){
    init(latency);
  }

  /** Specify the device names and the desired latency
  \param devNameOut The device name, e.g. hw:0 or plughw:0
  \param devNameIn The device name, e.g. hw:0 or plughw:0
  \param latency The number of frames to buffer. For example 4096
  */
	FullDuplexFile(const char*devNameOut, const char*devNameIn, int latency) : FullDuplex(devNameOut, devNameIn){
    init(latency);
  }

  /** initialise
  \param latency The number of frames to buffer. For example 4096
  */
  void init(int latency){
		ch=-1; // use this static number of input and output channels.
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
    if ((res=setChannels(ch))<0)
      return ALSADebug().evaluateError(res);

    res=sox.openWrite(name+".capture.wav", sox.getFSIn(), ch, std::numeric_limits<int>::max());
    return SoxDebug().evaluateError(res);
  }

  int closeFile(){
    int res=sox.closeWrite();
    return SoxDebug().evaluateError(res);
  }
};

int main(int argc, char *argv[]) {
  string deviceNameOut="default";
  string deviceNameIn="default";
  int latency=8192;

  OptionParser op;
  int i=0, ret;
  string help;
  if (op.getArg<string>("D", argc, argv, deviceNameOut, i=0)!=0)
      ;
  if (op.getArg<string>("C", argc, argv, deviceNameIn, i=0)!=0)
      ;

  if (argc<2 || op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0], deviceNameOut, deviceNameIn, latency);
  if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
    return printUsage(argv[0], deviceNameOut, deviceNameIn, latency);

  FullDuplexFile fullDuplex(deviceNameOut.c_str(), deviceNameIn.c_str(), latency);
  cout<<"opened the output device "<<fullDuplex.Playback::getDeviceName()<<endl;
  cout<<"opened the input  device "<<fullDuplex.Capture::getDeviceName()<<endl;
  cout<<"channels max "<<fullDuplex.Playback::getMaxChannels()<<endl;

  int res;
  // we don't want defaults so reset and refil the params ...
  fullDuplex.resetParams();

  if ((res=fullDuplex.setFormat(SND_PCM_FORMAT_S32_LE))<0)
    return ALSADebug().evaluateError(res);

  if ((res=fullDuplex.setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
    return ALSADebug().evaluateError(res);

  snd_pcm_uframes_t pSize=latency;
  if ((res=fullDuplex.Playback::setPeriodSize(&pSize))<0)
    return ALSADebug().evaluateError(res);
  if ((res=fullDuplex.openFile(argv[argc-1]))<0)
    return ALSADebug().evaluateError(res);

  cout<<"latency = "<<(float)latency/(float)fullDuplex.Playback::getSampleRate()<<" s"<<endl;


  snd_pcm_format_t format;
  fullDuplex.Playback::getFormat(format);
  cout<<"Playback format "<<fullDuplex.Playback::getFormatName(format)<<endl;
  fullDuplex.Capture::getFormat(format);
  cout<<"Capture format "<<fullDuplex.Capture::getFormatName(format)<<endl;
  cout<<"Playback channels "<<fullDuplex.Playback::getChannels()<<endl;
  cout<<"Capture  channels "<<fullDuplex.Capture::getChannels()<<endl;
  cout<<"Playback sample rate"<<fullDuplex.Playback::getSampleRate()<<endl;
  cout<<"Capture sample rate"<<fullDuplex.Capture::getSampleRate()<<endl;

  cout<<"calling go"<<endl;
  res=fullDuplex.go(); // start the full duplex read/write/process going.

  fullDuplex.closeFile();
	return ALSADebug().evaluateError(res);
}
