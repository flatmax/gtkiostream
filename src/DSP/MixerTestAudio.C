/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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
Date: 2015.03.12
*/

#include "DSP/MixerTestAudio.H"

MixerTestAudio::MixerTestAudio()
{
    testFrequencies.push_back(100.);
    testFrequencies.push_back(1000.);
    testFrequencies.push_back(10000.);
}

MixerTestAudio::~MixerTestAudio()
{
    //dtor
}

int MixerTestAudio::processAudio(jack_nframes_t nframes) { // The Jack client callback
    int maxIdx=(audio.rows()>=nframes+samplesProcessed)?nframes:audio.rows()-samplesProcessed;
    //	put output data into the buffers, only one output vector at column 0
    int outCh=outputPorts.size();
    if (currentOutputChannel<outCh){
        jack_default_audio_sample_t *out = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( outputPorts[currentOutputChannel], nframes );
        for (uint j=0; j<maxIdx; j++)
            out[j]=audio(samplesProcessed+j,0);
    }

    // all input data indexed after column 0
    int numIn=((audio.cols()-1-currentInputChannel)+1 < numIn)?audio.cols()-1-currentInputChannel+1:inputPorts.size();
    for (uint i=0; i<numIn; i++) {
        jack_default_audio_sample_t *in = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( inputPorts[i], nframes );
        for (uint j=0; j<maxIdx; j++)
            audio(samplesProcessed+j, 1+currentOutputChannel+i)=in[j];
    }

    samplesProcessed+=nframes;
    samplesToProcess-=nframes;

    if (samplesToProcess<=0) {
        currentOutputChannel+=1;
            recordLock.unLock(); // this shouldn't cause a problem because unlocking shouldn't cause waiting issues.
            return -1;
    }
    return 0;
}


int MixerTestAudio::setChannels(int outCnt, int inCnt, int testOutCnt) {
    int ret=NO_ERROR;
    if ((ret=destroyPorts())!=NO_ERROR)
        return ret;
    if ((ret=recordLock.tryLock())==NO_ERROR) {
        createPorts("in ", inCnt, "out ", outCnt);
        audio.resize(audio.rows(), 1+testOutCnt); // the extra one is because the output channel is first
        recordLock.unLock();
    }
    return ret;
}

int MixerTestAudio::reset() {
    int ret=CrossoverAudio::reset();
    // overwrite CrossoverAudio random data with sinusoidal data
    int N=samplesToProcess-zeroSampleCnt;
    Eigen::Array<float, Eigen::Dynamic, 1> sinusoidPhase(N, 1), sinusoid(N, 1);
    sinusoidPhase=Eigen::Array<float, Eigen::Dynamic, 1>::LinSpaced(N,0.,(double)N/(double)getSampleRate())*2.*M_PI;

    sinusoid=Eigen::Matrix<float,Eigen::Dynamic, 1>::Zero(N,1);
    for (int i=0; i<testFrequencies.size(); i++)
        sinusoid+=(sinusoidPhase*testFrequencies[i]).sin();
    sinusoid*=gain/(float)testFrequencies.size();

    audio.col(0).block(0,0,N,1)=sinusoid;

    currentOutputChannel=0;
    return ret;
}
