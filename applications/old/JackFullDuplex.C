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
Date: 2017.04.03
*/

#include "JackClient.H"
#include <iostream>
using namespace std;
#include "OptionParser.H"
#include "Sox.H"

#include <math.h>
#include <unistd.h> // for sleep, find something different on Microsoft

int printUsage(string name) {
    cout<<"\nUseage: \n"<<endl;
    cout<<name<<" [-t duration] outputFileName.ext : the output file name with ext replaced by a known output format extension (see below)"<<endl;
    cout<<name<<" -t num : duration in seconds"<<endl;
    Sox<jack_default_audio_sample_t> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;

    return 0;
}


/** This jack client will play a sine tone and record to file
*/
class FullDuplexTones : public JackClient, public Sox<jack_default_audio_sample_t> {
    float phase; ///< The tone phase
	vector<vector<jack_default_audio_sample_t> > audioData;

    int processAudio(jack_nframes_t nframes) { ///< The Jack client callback
        float w=2.*M_PI*1.e3/(float)getSampleRate(); //*(float)(i+1); // the frequency

        //	put output data into the buffers
        for (uint i=0; i<outputPorts.size(); i++) {
            jack_default_audio_sample_t *out = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( outputPorts[i], nframes );
            for (uint j=0; j<nframes; j++)
				audioData[i][j]=out[j]=sin((i+1)*w*(float)j+phase);
        }

        float rms=0.;
        //	print input data rms power to std out per channel
        for (uint i=0; i<inputPorts.size(); i++) {
            jack_default_audio_sample_t *in = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( inputPorts[i], nframes );
            for (uint j=0; j<nframes; j++)
            	audioData[outputPorts.size()+i][j]=in[j];
        }
        phase=fmod(phase+w*(float)(nframes), 2.*M_PI); // wrap the phase

		write(audioData); // write to file
		cout<<"nframes "<<nframes<<'\n';
        return 0;
    }
public:

    ///Constructor
    FullDuplexTones() {
        phase=0.;
    }

    virtual int openWrite(const string &fileName){
		int inChCnt, outChCnt;
		getPhysicalPortCount(inChCnt, outChCnt);
		int chCnt=inChCnt+outChCnt;
		int ret=Sox<jack_default_audio_sample_t>::openWrite(fileName, getSampleRate(), chCnt, 1.0);
		if (ret<0)
			return SoxDebug().evaluateError(ret);

		audioData.resize(chCnt); // set the number of channels
		for (int i=0; i<chCnt; i++) // set the vector length
			audioData[i].resize(getBlockSize());
	}
};

int main(int argc, char *argv[]) {

    OptionParser op;

    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0 || argc < 2)
        return printUsage(argv[0]);

    FullDuplexTones jackClient; // init the jack client

    // connect to the jack server
    int res=jackClient.connect("jack test client");
    if (res!=0)
        return JackDebug().evaluateError(res);

    cout<<"Jack : sample rate set to : "<<jackClient.getSampleRate()<<" Hz"<<endl;
    cout<<"Jack : block size set to : "<<jackClient.getBlockSize()<<" samples"<<endl;

	// open the audio file
	jackClient.openWrite(argv[argc-1]);

	int inChCnt, outChCnt;
	jackClient.getPhysicalPortCount(inChCnt, outChCnt);
	cout<<"Jack : "<<inChCnt<<" physical input channels"<<endl;
	cout<<"Jack : "<<outChCnt<<" physical output channels"<<endl;
    res=jackClient.createPorts("in ", inChCnt, "out ", outChCnt);
    if (res!=0)
        return JackDebug().evaluateError(res);

    // start the client
    res=jackClient.startClient(inChCnt, outChCnt, true);
    if (res!=0)
        return JackDebug().evaluateError(res);

    float t=2.0;
    op.getArg<float>("t", argc, argv, t, i=0);
    cout<<"running for "<<t<<" seconds"<<endl;
    sleep(t); // sleep for 10 seconds ... Microsoft users may have to use a different sleep function
    return 0;
}

