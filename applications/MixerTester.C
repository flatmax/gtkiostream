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

/*
Author: Matt Flax <flatmax@flatmax.org>
Date: 2013.08.05
*/

#include "JackClient.H"
#include <iostream>
#include <fstream>
using namespace std;
#include "OptionParser.H"
//#include "Sox.H"
#include "DSP/MixerTestAudio.H"
#include "Octave.H"
#include <unistd.h>

int printUsage(string name) {
    cout<<"\nUseage: \n"<<endl;
    cout<<name<<" [-t duration] [-o num] [-i num] [-O num] [-g num] "<<endl;
    cout<<name<<" -t num : duration in seconds"<<endl;
    cout<<name<<" -o num : number of output channels to open at the same time on the audio device"<<endl;
    cout<<name<<" -O num : total number of test output channels to record"<<endl;
    cout<<name<<" -i num : number of input channels to open at the same time on the audio device"<<endl;
    cout<<name<<" -g num : the output gain"<<endl;
    return 0;
}

int main(int argc, char *argv[]) {
    OptionParser op;

    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        return printUsage(argv[0]);

    MixerTestAudio mixerAudio; // init the crossover tester

    float t=10.0;
    op.getArg<float>("t", argc, argv, t, i=0);
    cout<<"running for "<<t<<" seconds"<<endl;
    mixerAudio.setDuration(t);

    float g=1.0;
    op.getArg<float>("g", argc, argv, g, i=0);
    cout<<"using a gain="<<g<<endl;
    mixerAudio.setGain(g);

    int inChCnt=2;
    op.getArg<int>("i", argc, argv, inChCnt, i=0);
    cout<<"recording  "<<inChCnt<<" channels of physical input audio to record each time"<<endl;

    int outChCnt=1;
    op.getArg<int>("o", argc, argv, outChCnt, i=0);
    cout<<"playing  "<<outChCnt<<" channels of output random noise"<<endl;

    int outTestChCnt=2;
    op.getArg<int>("O", argc, argv, outTestChCnt, i=0);
    cout<<"recording  "<<outTestChCnt<<" channels of output audio (mixer inputs) total, maximum of "<<outChCnt<<" channels each run."<<endl;

    mixerAudio.setChannels(outChCnt, inChCnt, outTestChCnt);

    cout<<"Jack : sample rate set to : "<<mixerAudio.getSampleRate()<<" Hz"<<endl;
    cout<<"Jack : block size set to : "<<mixerAudio.getBlockSize()<<" samples"<<endl;

    ofstream mFileOut("/tmp/MixerTester.m");
    mFileOut<<"function MixerTester(data)"<<endl;
    mFileOut<<"sampleRate="<<mixerAudio.getSampleRate()<<';'<<endl;
    mFileOut<<"[N,M]=size(data); figure(1); f=linspace(0,sampleRate,N);"<<endl;
    mFileOut<<"for i=2:M"<<endl;
    mFileOut<<"    subplot(M-1,1,i-1);  loglog(f, abs(fft(data(:,i))));"<<endl;
    mFileOut<<"end"<<endl;
    mFileOut<<"xlabel('f (Hz)')"<<endl;
    mFileOut<<"figure(2); t=(0:N-1)/sampleRate;"<<endl;
    mFileOut<<"for i=2:M"<<endl;
    mFileOut<<"    subplot(M-1,1,i-1); plot(t,data(:,i));"<<endl;
    mFileOut<<"    set(gca,'ylim',[-1.5 1.5]);"<<endl;
    mFileOut<<"end"<<endl;
    mFileOut<<"xlabel('t (s)')"<<endl;
    mFileOut<<"disp('paused'); pause \nend"<<endl;
    mFileOut.close();

    vector<string> args(3); args[0]=string("--silent"); args[1]=string("--path"); args[2]="/tmp";
    Octave octave(args);

    while (1) {
        mixerAudio.reset();
        while (mixerAudio.getNumberOfRecordedChannels()<outTestChCnt){
    //        cout<<"\r"<<mixerAudio.getNumberOfRecordedChannels()<<" channels already recorded, recroding the "<<mixerAudio.getNumberOfRecordedChannels()+1<<" channel, device under test input channel."<<endl;
            cout<<mixerAudio.getNumberOfRecordedChannels()<<" channels already recorded, recroding the "<<mixerAudio.getNumberOfRecordedChannels()+1<<" channel, device under test input channel."<<endl;

    //        string userInput;
    //        getline(cin, userInput);
            int ret=mixerAudio.recordNextChannelSet();
            if (ret!=NO_ERROR)
                exit(ret);
            sleep(t+1); // sleep for duration seconds
            while (mixerAudio.isRecording()) // whilst still recording, sleep for half a second, allowing time to finish.
                sleep(1);
        }

    //    cout<<"\rDone, now going to write to the file "<<argv[argc-1]<<endl;

    //    // open sox
    //    Sox sox;
    //    int ret=sox.openWrite(argv[argc-1], mixerAudio.getSampleRate(), inTestChCnt+outChCnt+1, 1.0);
    //    if (ret!=NO_ERROR)
    //        return SoxDebug().evaluateError(ret);
    //
    //    int written=sox.write(mixerAudio.audio);
    //    if (written!=mixerAudio.audio.rows()*mixerAudio.audio.cols()) {
    //        cout<<SoxDebug().evaluateError(written)<<endl;
    //        cout<<"written "<<written<<endl;
    //        cout<<"Output matrix size (rows, cols) = ("<<mixerAudio.audio.rows()<<", "<<mixerAudio.audio.cols()<<")"<<endl;
    //        cout<<"Matrix is a total of "<<mixerAudio.audio.rows()*mixerAudio.audio.cols()<<" samples"<<endl;
    //        cout<<"Error writing, exiting."<<endl;
    //    }
    //
    //    sox.closeWrite();

        cout<<"\r launching octave to view results "<<endl;

        vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> > input(1), output;
        input[0]=mixerAudio.audio;
        octave.runM("MixerTester",input, output);
    }
    return 0;
}

