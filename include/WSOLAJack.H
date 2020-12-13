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
#ifndef WSOLAJACK_H_
#define WSOLAJACK_H_

#include <JackClient.H>

typedef float FP_TYPE;

/** Connects WSOLA to the audio system using JackClient.
*/
class WSOLAJack : public WSOLA, public JackClient {
    FP_TYPE timeScale; ///< The time scale to use for speed scaling the audio

    Sox<FP_TYPE> sox; ///< Audio file reading class

    Matrix<FP_TYPE, Dynamic, Dynamic> audioData; ///< The audio data which has been read from the sox file

    int N; ///< The number of audio samples required by WSOLA from the audio file

    jack_default_audio_sample_t **outs; ///< The number of output port created for this audio stream

    /** The Jack client callback.
    Initialises by setting up the output buffers.
    Steps through all nframes in chunks of N/2 output samples.
    Outputs the last N/2 chunk of samples, processes a new chunk,
    Reads in a chunk.
    Exits. Returns non-zero when new samples can't be retrieved from file.
    */
    int processAudio(jack_nframes_t nframes) {
        if (nframes%getOutputSize()) { // if we are asked to process a number of frames which isn't divisible by our output block size, print the error and exit.
            ostringstream oss;
            oss<<"jack wants "<<nframes<<" to process which doesn't divide by the number of frames which WSOLA can process in one step "<<getOutputSize()<<'\t';
            return JackDebug().evaluateError(WSOLA_NFRAMES_JACK_ERROR, oss.str());
        }

        int ret=0;
        for (uint i=0; i<outputPorts.size(); i++)
            outs[i] = ( jack_default_audio_sample_t* ) jack_port_get_buffer (outputPorts[i] , nframes);

        int processed=0;
        while (processed!=nframes) {
            //cout<<processed<<'\t'<<outputPorts.size()<<'\t'<<output.rows()<<'\t'<<output.cols()<<endl;
            // output the audio data
            for (uint i=0; i<outputPorts.size(); i++)
                for (int j=0; j<getOutputSize(); j++)
                    outs[i][processed+j]=output(i, j);

            N=process(timeScale, audioData);

            // read more audio data
            ret=readAudio(N);
            if (ret!=N) {
                cerr<<"couldn't read audio, wanted "<<N<<" got "<<ret<<" rolling out"<<endl;
                if (noMoreAudio()<=0)
                    return N; // returns a non zero number to stop
            } else
                ret=NO_ERROR;

//            cout<<"read audo returns "<<audioData.rows()<< '\t'<<audioData.cols()<<endl;

            processed+=getOutputSize();
        }
        return ret;
    }

    int readAudio(int sampleCount){
        Matrix<FP_TYPE, Dynamic, Dynamic> output;
        int ret=sox.read(audioData, sampleCount); // sox read
        audioData.transposeInPlace();
        if (audioData.cols()!=sampleCount){
            cout<<"couldn't read "<<sampleCount<<" samples only read "<<ret<<endl;
            cout<<"readAudio "<<audioData.rows()<<'\t'<<audioData.cols()<<endl;
            // TODO: Zero audioData which wasn't read here.
            //Matrix<FP_TYPE, Dynamic, Dynamic>::Zeros(audioData.rows(),sampleCount-audioData.cols());
        }
        return audioData.cols();
    }

public:

    /** Constructor
    Using the filename, open the audio file, read the first block of data, connect to and configure Jack.
    Also process the first block to init any memory in the first pass of WSOLA.
    \param fileName The name of the audio file to open.
    */
    WSOLAJack(string fileName) {
        outs=NULL;
        timeScale=1.;

        int ret;
        if ((ret=sox.openRead(fileName))<0  && ret!=SOX_READ_MAXSCALE_ERROR)
            exit(WSOLADebug().evaluateError(ret, fileName));
        sox.setMaxVal(1.0);

        ret=connect("WSOLA");
        if (ret!=NO_ERROR)
            exit(JackDebug().evaluateError(ret));

        reset(sox.getChCntIn()); // set wsola to use the correct channel count.
        cout<<getSampleRate()<<endl;
        setFS(getSampleRate()); // set the WSOLA sample rate
        N=getSamplesRequired();
        ret=readAudio(N);
        if (ret!=N) {
            cerr<<"couldn't read audio, wanted "<<N<<" got "<<ret<<endl;
            exit(ret);
        }

        cout<<"Jack : sample rate set to : "<<getSampleRate()<<" Hz"<<endl;
        cout<<"Jack : block size set to : "<<getBlockSize()<<" samples"<<endl;

        outs = new jack_default_audio_sample_t*[sox.getChCntIn()];

        if ((ret=createPorts("in ", 0, "out ", sox.getChCntIn()))!=NO_ERROR)
            exit(JackDebug().evaluateError(ret));

        // process the first frame of audio data - the rest will happen in the processAudio method
        N=process(timeScale, audioData);

        // read more audio data
        ret=readAudio(N);
        if (ret!=N) {
            cerr<<"couldn't read audio, wanted "<<N<<" got "<<ret<<endl;
            exit(ret);
        }

        // ensure that the block size is as required.
        if (getBlockSize()%getOutputSize())
            ret=setBlockSize(getOutputSize());
        if (ret<0)
            exit(JackDebug().evaluateError(ret));

        if ((ret=startClient(0, sox.getChCntIn(), true))!=NO_ERROR)
            exit(JackDebug().evaluateError(ret));
    }

    /// Destructor
    ~WSOLAJack(void) {
        sox.closeRead();
        if (outs)
            delete [] outs;
    }

    void setTimeScale(FP_TYPE ts) {
        timeScale=ts;
    }
};

#endif // WSOLAJACK_H_
