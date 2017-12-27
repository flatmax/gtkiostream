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
using namespace std;
#include "OptionParser.H"
#include "Sox.H"
#include "DSP/CrossoverAudio.H"
#include <unistd.h>

typedef float FP_TYPE;

int printUsage(string name) {
    cout<<"\nUseage: \n"<<endl;
    cout<<name<<" [-t duration] [-o num] [-i num] [-I num] [-g num] outputFileName.ext : the output file name with ext replaced by a known output format extension (see below)"<<endl;
    cout<<name<<" -t num : duration in seconds"<<endl;
    cout<<name<<" -o num : number of output channels to open at the same time on the audio device"<<endl;
    cout<<name<<" -i num : number of input channels to open at the same time on the audio device"<<endl;
    cout<<name<<" -I num : total number of test input channels to record"<<endl;
    cout<<name<<" -g num : the output gain"<<endl;
    Sox<FP_TYPE> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;

    return 0;
}

int saveAudioToFile(const char *fn, const CrossoverAudio& crossAudio, const int chCnt) {
    // open sox
    Sox<FP_TYPE> sox;
    int ret=sox.openWrite(fn, crossAudio.getSampleRate(), chCnt, 1.0);
    if (ret!=NO_ERROR)
        return SoxDebug().evaluateError(ret);
    int written=sox.write(crossAudio.audio);
    if (written!=crossAudio.audio.rows()*crossAudio.audio.cols()) {
        cout<<SoxDebug().evaluateError(written)<<endl;
        cout<<"written "<<written<<endl;
        cout<<"Output matrix size (rows, cols) = ("<<crossAudio.audio.rows()<<", "<<crossAudio.audio.cols()<<")"<<endl;
        cout<<"Matrix is a total of "<<crossAudio.audio.rows()*crossAudio.audio.cols()<<" samples"<<endl;
        cout<<"Error writing, exiting."<<endl;
    }
    sox.closeWrite();
    return ret;
}


int main(int argc, char *argv[]) {
    OptionParser op;

    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        return printUsage(argv[0]);

    CrossoverAudio crossAudio; // init the crossover tester

    float t=10.0;
    op.getArg<float>("t", argc, argv, t, i=0);
    cout<<"running for "<<t<<" seconds"<<endl;
    crossAudio.setDuration(t);

    float g=1.0;
    op.getArg<float>("g", argc, argv, g, i=0);
    cout<<"using a gain="<<g<<endl;
    crossAudio.setGain(g);

    int inChCnt=2;
    op.getArg<int>("i", argc, argv, inChCnt, i=0);
    cout<<"recording  "<<inChCnt<<" channels of physical input audio to record each time"<<endl;

    int inTestChCnt=2;
    op.getArg<int>("I", argc, argv, inTestChCnt, i=0);
    cout<<"recording  "<<inTestChCnt<<" channels of input audio total, maximum of "<<inChCnt<<" channels each run."<<endl;

    int outChCnt=1;
    op.getArg<int>("o", argc, argv, outChCnt, i=0);
    cout<<"playing  "<<outChCnt<<" channels of output random noise"<<endl;

    crossAudio.setChannels(outChCnt, inChCnt, inTestChCnt);

    cout<<"Jack : sample rate set to : "<<crossAudio.getSampleRate()<<" Hz"<<endl;
    cout<<"Jack : block size set to : "<<crossAudio.getBlockSize()<<" samples"<<endl;

    crossAudio.reset();

    cout<<"Please press enter to start."<<endl;
    while (1){
        string userInput;
        while (crossAudio.getNumberOfRecordedChannels()<inTestChCnt+outChCnt){
            if (crossAudio.getNumberOfRecordedChannels()<outChCnt)
                cout<<"\r"<<crossAudio.getNumberOfRecordedChannels()<<" channels already recorded, press enter to record the "<<crossAudio.getNumberOfRecordedChannels()+1<<" channel, output calibration channel."<<endl;
            else if (crossAudio.getNumberOfRecordedChannels()==outChCnt)
                cout<<"\r"<<crossAudio.getNumberOfRecordedChannels()<<" channels already recorded, please plug in the device to test NOW !!!! press enter to record the "<<crossAudio.getNumberOfRecordedChannels()-outChCnt+1<<" channel, device under test input channel."<<endl;
            else
                cout<<"\r"<<crossAudio.getNumberOfRecordedChannels()<<" channels already recorded, press enter to record the "<<crossAudio.getNumberOfRecordedChannels()-outChCnt+1<<" channel, device under test input channel."<<endl;

            getline(cin, userInput);
            int ret=crossAudio.recordNextChannelSet();
            if (ret!=NO_ERROR)
                exit(ret);
            sleep(t+1); // sleep for duration seconds
            while (crossAudio.isRecording()) // whilst still recording, sleep for half a second, allowing time to finish.
                sleep(1);
                cout<<"done"<<endl;
        }
        cout<<"\rDone, now going to write to the file "<<argv[argc-1]<<endl;

        if (saveAudioToFile(argv[argc-1], crossAudio, inTestChCnt+outChCnt+1)!=NO_ERROR)
            break;

        cout<<"\n\nStart the analysis and press any key to record a new crossover\n"<<endl;
        crossAudio.nextCrossover(); // keep the recorded loopback channel and start again with the crossover channels.
        break;
    }

    return 0;
}
