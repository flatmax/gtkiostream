/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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

#include <math.h>
#include <unistd.h> // for sleep, find something different on Microsoft

/** This test jack client will play a sine tone.
*/
class TestJackClient : public JackClient {
    float phase; ///< The tone phase

    int processAudio(jack_nframes_t nframes) { ///< The Jack client callback
        float w=2.*M_PI*1.e3/(float)getSampleRate(); //*(float)(i+1); // the frequency

        //	put output data into the buffers
        for (uint i=0; i<outputPorts.size(); i++) {
            jack_default_audio_sample_t *out = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( outputPorts[i], nframes );
            for (uint j=0; j<nframes; j++)
                out[j]=sin(w*(float)j+phase);
        }

        float rms=0.;
        //	print input data rms power to std out per channel
        for (uint i=0; i<inputPorts.size(); i++) {
            jack_default_audio_sample_t *in = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( inputPorts[i], nframes );
            for (uint j=0; j<nframes; j++)
                rms+=in[j]*in[j];
            cout<<"input ch "<<i<<" rms = "<<sqrt(rms/nframes)<<'\t';
        }
        cout<<'\n';

        phase=fmod(phase+w*(float)(nframes), 2.*M_PI); // wrap the phase
        return 0;
    }
public:

    ///Constructor
    TestJackClient() {
        phase=0.;
    }

//    /** Callback to pass to the jack server using JackClient::connect
//    */
//    static int processAudio(jack_nframes_t nframes, void *arg) { ///< The Jack client callback
//        return reinterpret_cast<TestJackClient*>(arg)->processAudio(nframes);
//    }
};

int main(int argc, char *argv[]) {
    TestJackClient jackClient; // init the jack client

    // connect to the jack server
    int res=jackClient.connect("jack test client");
    if (res!=0)
        return JackDebug().evaluateError(res);

    cout<<"Jack : sample rate set to : "<<jackClient.getSampleRate()<<" Hz"<<endl;
    cout<<"Jack : block size set to : "<<jackClient.getBlockSize()<<" samples"<<endl;

    res=jackClient.createPorts("in ", 1, "out ", 2);
    if (res!=0)
        return JackDebug().evaluateError(res);

    // start the client
    res=jackClient.startClient(1, 2, true);
    if (res!=0)
        return JackDebug().evaluateError(res);

    sleep(10); // sleep for 10 seconds ... Microsoft users may have to use a different sleep function
    return 0;
}
