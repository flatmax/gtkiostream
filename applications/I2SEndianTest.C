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

// following definitions from Jack
#define NORMALIZED_FLOAT_MIN -1.0f
#define NORMALIZED_FLOAT_MAX  1.0f
#define SAMPLE_16BIT_SCALING  32767.0f
#define SAMPLE_16BIT_MAX  32767
#define SAMPLE_16BIT_MIN  -32767
#define SAMPLE_16BIT_MAX_F  32767.0f
#define SAMPLE_16BIT_MIN_F  -32767.0f

#define f_round(f) lrintf(f)

#define float_16(s, d)\
	if ((s) <= NORMALIZED_FLOAT_MIN) {\
		(d) = SAMPLE_16BIT_MIN;\
	} else if ((s) >= NORMALIZED_FLOAT_MAX) {\
		(d) = SAMPLE_16BIT_MAX;\
	} else {\
		(d) = f_round ((s) * SAMPLE_16BIT_SCALING);\
	}



/** This jack client will output 0xff on each channel for each sample.
*/
class TestJackClient : public JackClient {
    int processAudio(jack_nframes_t nframes) { ///< The Jack client callback
        short dataS[4]={(short)0xaaaa, (short)0xff, (short)0x5555, (short)0xff0};
        float dataF[4]={(float)dataS[0]/SAMPLE_16BIT_SCALING, (float)dataS[1]/SAMPLE_16BIT_SCALING, (float)dataS[2]/SAMPLE_16BIT_SCALING, (float)dataS[3]/SAMPLE_16BIT_SCALING};
        //	put output data into the buffers
        for (uint i=0; i<outputPorts.size(); i++) {
            jack_default_audio_sample_t *out = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( outputPorts[i], nframes );
            for (int j=0; j<nframes; j++){
                int index=i+(int)fmod((float)j*2.,4.);
                out[j]=dataF[index];
            }
        }

//        //	do something with the input data
//        for (uint i=0; i<inputPorts.size(); i++) {
//            jack_default_audio_sample_t *in = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( inputPorts[i], nframes );
//            for (uint j=0; j<nframes; j++)
//                rms+=in[j]*in[j];
//        }
        return 0;
    }
};

int main(int argc, char *argv[]) {

#if __BYTE_ORDER == __LITTLE_ENDIAN
    cout<<"This system is little endian."<<endl;
#elif __BYTE_ORDER == __BIG_ENDIAN
    cout<<"This system is big endian."<<endl;
#endif

    TestJackClient jackClient; // init the jack client

    // connect to the jack server
    int res=jackClient.connect("jack test client");
    if (res!=0)
        return JackDebug().evaluateError(res);

    cout<<"Jack : sample rate set to : "<<jackClient.getSampleRate()<<" Hz"<<endl;
    cout<<"Jack : block size set to : "<<jackClient.getBlockSize()<<" samples"<<endl;

    res=jackClient.createPorts("in ", 2, "out ", 2);
    if (res!=0)
        return JackDebug().evaluateError(res);

    // start the client
    res=jackClient.startClient(2, 2, true);
    if (res!=0)
        return JackDebug().evaluateError(res);

    sleep(60); // sleep for 10 seconds ... Microsoft users may have to use a different sleep function
    return 0;
}

