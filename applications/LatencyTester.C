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

#include <iostream>
#include <algorithm>
using namespace std;
#include "OptionParser.H"

#include "DSP/ImpulseBandLimited.H"
#include "ALSA/ALSA.H"

using namespace Eigen;

#ifdef HAVE_SOX
#include <Sox.H>
#endif

#define DEFAULT_BUFFER_US 3000. ///< The default buffer durection in us
#define F_TYPE short int

/** Class to play band limited impulse responses and record them back.
Lays the foundation for automated latency testing.
*/
class LatencyTester : public ImpulseBandLimited<F_TYPE>, public ALSA::FullDuplex<F_TYPE> {
  unsigned int N; ///< The number of samples per call matching the period size
  unsigned int n; ///< The current sample in the impulse we are indexing
  unsigned int ch; ///< The number of channels we are processing
  unsigned int M; ///< The number of loops we will process
  int m; ///< The loop number we are processing
  Eigen::Array<F_TYPE, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> recordedAudio;

  int process(){
    // cout<<"m "<<m<<" recordedAudio.rows() "<<recordedAudio.rows()<<endl;
    if (m>=recordedAudio.rows()) // work out whether we should finish
      return recordedAudio.rows();

    if (inputAudio.rows()!=N || inputAudio.cols()!=ch){ // should only happen on the first dummy processing loop
      if (m!=-1){
        cout<<"Buffer resetting should only happen on the first processing dummy loop"<<endl;
        return -2;
      }
      m++;
      inputAudio.resize(N, ch);
      inputAudio.setZero();
      outputAudio.resize(N, ch);
      recordedAudio.resize(rows()*M, ch); // reset the recorded audio
      recordedAudio.setZero();
      return 0;
    }

    int nn=0; // copy the impulse to the output
    // cout<<"while enter "<<endl;
    while (nn+1<N){
      int cnt=std::min(N-nn, (int)rows()-n); // don't sample past the end of the output buffer nor the impulse buffer
      // cout<<"0: n "<<n<<" nn "<<nn<<" cnt "<<cnt<<endl;
      for (int c=0; c<ch; c++)
        outputAudio.block(0, c, cnt, 1)=ImpulseBandLimited<F_TYPE>::block(nn, 0, cnt, 1); // copy the audio data over
      nn+=cnt;
      if (nn>N){ // remove this test when working
        cout<<"youv'e gone past the end with nn error"<<endl;
        return -2;
      }
      n+=cnt;
      // cout<<"1: n "<<n<<" nn "<<nn<<" cnt "<<cnt<<endl;
      if (n==this->rows())
        n=0;
      if (n>this->rows()){ // remove this test when working
        cout<<"youv'e gone past the end with n error"<<endl;
        return -1;
      }
      // cout<<"2: n "<<n<<" nn "<<nn<<" cnt "<<cnt<<endl;
    }
    // cout<<"while exited "<<endl;

    int cnt=std::min((unsigned int)recordedAudio.rows()-m, N);
    // cout<<"0: m "<<m<<" cnt "<<cnt<<" ch "<<ch<<endl;
    recordedAudio.block(m, 0, cnt, ch)=inputAudio.topRows(cnt); // store the input data
    m+=cnt;
    // cout<<"1: m "<<m<<" cnt "<<cnt<<" ch "<<ch<<endl;

    // cout<<"m "<<m<<" recordedAudio.rows() "<<recordedAudio.rows()<<endl;

    return 0; // return 0 to continue
  }

public:
  LatencyTester(string dev, unsigned int c, unsigned int MIn) : ALSA::FullDuplex<F_TYPE>(dev.c_str()){
    ch=c;
    M=MIn;
  }

  ~LatencyTester(){}

  int go(){
    inputAudio.resize(0,0); // force resize on the the iteration
    outputAudio.resize(0,0);
    n=0; // reset the sample index
    m=-1; // We are on loop -1 the dummy process first for setting up buffers
    snd_pcm_uframes_t p;
    int ret=Capture::getPeriodSize(&p);
    if (ret!=0)
      return ret;
    N=p;
    cout<<"LatencyTester::go : starting with N = "<<N<<endl;
    return ALSA::FullDuplex<F_TYPE>::go();
  }

#ifdef HAVE_SOX
  int saveRecordingToFile(string name, float fs){
    Sox<F_TYPE> sox; // use sox to write to file
    int ret;
    if (ret=sox.openWrite(name, fs, recordedAudio.cols(), recordedAudio.abs().maxCoeff())<0) // try to open the file
        return SoxDebug().evaluateError(ret, name);
    else
      return sox.write(recordedAudio);
    return ret;
  }
#endif
};

int printUsage(string name) {
    cout<<"\nUseage: \n"<<endl;
    cout<<name<<" [-t duration] [-o num] [-i num] [-O num] [-g num] "<<endl;
    cout<<name<<" -D str : ALSA Device"<<endl;
    cout<<name<<" -r num : Sample rate in Hz"<<endl;
    cout<<name<<" -c num : Channels"<<endl;
    cout<<name<<" -b num : Buffer duration in us"<<endl;
    cout<<name<<" -i num : Minimum frequency in Hz"<<endl;
    cout<<name<<" -a num : Maximum frequency in Hz"<<endl;
    cout<<name<<" -l num : Loop count"<<endl;
    return 0;
}

int main(int argc, char *argv[]) {
    OptionParser op;

    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        return printUsage(argv[0]);

    string device("hw:0"); // ALSA IO device
    int test=op.getArg<string>("D", argc, argv, device, i=0);
    cout<<"Using ALSA device : "<<device<<endl;

    float fs=48000.; // sample rate
    op.getArg<float>("r", argc, argv, fs, i=0);
    cout<<"Sample rate : "<<fs<<" Hz"<<endl;

    int ch=2; // the number of channels
    op.getArg<int>("c", argc, argv, ch, i=0);
    cout<<"Channels : "<<ch<<endl;

    float b=DEFAULT_BUFFER_US;
    op.getArg<float>("b", argc, argv, b, i=0);
    b=::round(b*fs*1.e-6);
    cout<<"Buffer length : "<<b<<" samples, "<<b/fs*1.e6<<" us"<<endl;

    float fi=100.; // the minimum frequency
    op.getArg<float>("i", argc, argv, fi, i=0);
    cout<<"Minimum frequency : "<<fi<<" Hz"<<endl;

    float fa=10000.; // the minimum frequency
    op.getArg<float>("a", argc, argv, fi, i=0);
    cout<<"Maximum frequency : "<<fa<<" Hz"<<endl;

    unsigned int l=5; // The number of loops for testing
    op.getArg<unsigned int>("l", argc, argv, l, i=0);
    cout<<"Loop count : "<<l<<endl;

    float s=1.; // Duration of a loop in s
    cout<<"Impulse duration : "<<s<<" seconds"<<endl;

    LatencyTester latencyTester(device, ch, l);

    int res=latencyTester.resetParams(); // we don't want defaults so reset and refil the params ...
    if (res<0)
      return res;

    if ((res=latencyTester.setBufSize((int)b))<0)
      return ALSA::ALSADebug().evaluateError(res);
    if ((res=latencyTester.setChannels(ch))<0)
      return ALSA::ALSADebug().evaluateError(res);
    if ((res=latencyTester.setSampleRate(fs))<0)
      return ALSA::ALSADebug().evaluateError(res);

    snd_pcm_format_t format=SND_PCM_FORMAT_S16_LE;
    if ((res=latencyTester.setFormat(format))<0)
      return ALSA::ALSADebug().evaluateError(res);
    if ((res=latencyTester.setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0)
      return ALSA::ALSADebug().evaluateError(res);

    // generate the band limited impulse
    if ((res=latencyTester.generateImpulse(s, fs, fi, fa))<0)
      return res;

    if ((res=latencyTester.go())<0) // start the full duplex read/write/process going.
      return res;

#ifdef HAVE_SOX
    latencyTester.saveToFile("/tmp/impulse.wav", fs); // save the impulse to file
    latencyTester.saveRecordingToFile("/tmp/recordedImpulses.wav", fs); // save the impulse recordings to file
#endif

  	return 0;
}
