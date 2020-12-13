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
#include "DSP/CrossoverAudio.H"
#include <algorithm>

CrossoverAudio::CrossoverAudio() {
    gain=0.9;
    int res=connect("CrossoverAudio");
    if (res!=0)
        JackDebug().evaluateError(res);
    zeroSampleCnt=15*getBlockSize();
}

CrossoverAudio::~CrossoverAudio() {
}

int CrossoverAudio::processAudio(jack_nframes_t nframes) { // The Jack client callback
    int maxIdx=(audio.rows()>=nframes+samplesProcessed)?nframes:audio.rows()-samplesProcessed;
    //	put output data into the buffers, only one output vector at column 0
    int outCh=outputPorts.size();
    for (uint i=0; i<outCh; i++) {
        jack_default_audio_sample_t *out = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( outputPorts[i], nframes );
        for (uint j=0; j<maxIdx; j++)
            out[j]=audio(samplesProcessed+j,0);
    }

    // all input data indexed after column 0
    int numIn=((audio.cols()-1-currentInputChannel)+1 < numIn)?audio.cols()-1-currentInputChannel+1:inputPorts.size();
    for (uint i=0; i<numIn; i++) {
        jack_default_audio_sample_t *in = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( inputPorts[i], nframes );
        for (uint j=0; j<maxIdx; j++)
            audio(samplesProcessed+j, 1+currentInputChannel+i)=in[j];
    }

    samplesProcessed+=nframes;
    samplesToProcess-=nframes;

    if (samplesToProcess<=0) {
        currentInputChannel+=numIn;
            recordLock.unLock(); // this shouldn't cause a problem because unlocking shouldn't cause waiting issues.
            return -1;
    }
    return 0;
}

int CrossoverAudio::startClient(int inCnt, int outCnt, bool doConnect) {
    recordLock.lock();
    return JackClient::startClient(inCnt, outCnt, doConnect);
}

int CrossoverAudio::reset() {
    int ret=NO_ERROR;
    if ((ret=recordLock.tryLock())!=NO_ERROR) { // if recording
        samplesToProcess=0; // force the audio thread to stop and wait.
        recordLock.lock();
    }
    recordLock.unLock();
    samplesToProcess=audio.rows();
    samplesProcessed=0;
    currentInputChannel=0;
    audio.block(0, 0, audio.rows(), audio.cols())=Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic>::Zero(audio.rows(), audio.cols());
    audio.col(0).block(0,0,samplesToProcess-zeroSampleCnt,1)=Eigen::Matrix<float,Eigen::Dynamic, 1>::Random(samplesToProcess-zeroSampleCnt)*gain;
    return ret;
}

int CrossoverAudio::setDuration(float d) {
    int ret=NO_ERROR;
    if ((ret=recordLock.tryLock())==NO_ERROR){
        audio.resize((unsigned int)(d*(float)getSampleRate())+zeroSampleCnt, audio.cols());
        recordLock.unLock();
    }
    return ret;
}

int CrossoverAudio::setChannels(int outCnt, int inCnt, int testInCnt) {
    int ret=NO_ERROR;
    if ((ret=destroyPorts())!=NO_ERROR)
        return ret;
    if ((ret=recordLock.tryLock())==NO_ERROR) {
        createPorts("in ", inCnt, "out ", outCnt);
        audio.resize(audio.rows(), 1+outputPorts.size()+testInCnt); // the extra one is because the output channel is first
        recordLock.unLock();
    }
    return ret;
}

void CrossoverAudio::getChannels(int &inCnt, int &outCnt) {
    inCnt=inputPorts.size();
    outCnt=outputPorts.size();
}

int CrossoverAudio::recordNextChannelSet() {
    samplesToProcess=audio.rows();
    samplesProcessed=0;
    int res=startClient(inputPorts.size(), outputPorts.size(), true);
    if (res!=NO_ERROR)
        recordLock.unLock();
    return JackDebug().evaluateError(res);
}

void CrossoverAudio::nextCrossover(){
    currentInputChannel=1;
    audio.block(0, 1, audio.rows(), audio.cols()-1)=Eigen::Matrix<float,Eigen::Dynamic,Eigen::Dynamic>::Zero(audio.rows(), audio.cols()-1);
}

int CrossoverAudio::isRecording() {
    if (recordLock.tryLock()==NO_ERROR) {
        recordLock.unLock();
        return 0;
    }
    return 1;
}
