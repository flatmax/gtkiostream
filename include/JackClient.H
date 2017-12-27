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
#ifndef JACKCLIENT_H_
#define JACKCLIENT_H_

#include "JackBase.H"

/** Class to connect to a jack server as a client, see : http://jackaudio.org/

This client auto-connects the output and input ports to the first available ports.

See JackClientTest.C for an example, however usage is like so :
\code
/// This test client inherits from the JackClient and implements the processAudio callback to get input, put output and process
class TestJackClient : public JackClient {
    int processAudio(jack_nframes_t nframes) { ///< The Jack client callback
        jack_default_audio_sample_t *out = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( outputPorts[0], nframes );
        // load the output audio samples here

        // do something with the audio samples here

        jack_default_audio_sample_t *in = ( jack_default_audio_sample_t* ) jack_port_get_buffer ( inputPorts[0], nframes );
        // do something with the input audio samples here
        return 0;
    }
};

TestJackClient jackClient; // init the jack client

// at each step check that res isn't indicating an error...
int res=jackClient.connect("jack test client"); // connect the client to the Jack system indicating the callback

res=jackClient.createPorts("in ", 1, "out ", 1); // create the ports for the jack patching system.

res=jackClient.startClient(1, 1, true); // Start the client running with one input and one output port, which is auto-connected to the system ports

// at this point, the processAudio is called each time audio samples are available by the jack system.
// don't exit the program until you have finished processing all the audio you want to !
\endcode

*/
class JackClient : virtual public JackBase {
    /** This is the process audio callback which is called each time audio is acquired and required by the audio system for input and output.
    Callback to pass to the jack server using JackClient::connect.
    You must overload processAudio as that is where the processing is done in your class.
    \sa processAudio
    \param nframes The number of frames to process
    \param arg the user data
    */
    static int processAudioStatic(jack_nframes_t nframes, void *arg) { ///< The Jack client callback
        return reinterpret_cast<JackClient*>(arg)->processAudio(nframes);
    }

    /** This is the callback triggered when the buffer size changes.
    Callback to pass to the jack server using JackClient::connect.
    You must overload bufferSizeChange as that is where the processing is done in your class.
    \sa bufferSizeChange
    \param nframes The number of frames to process
    \param arg the user data
    */
    static int bufferSizeChangeStatic(jack_nframes_t nframes, void *arg) { ///< The Jack client callback
        return reinterpret_cast<JackClient*>(arg)->bufferSizeChange(nframes);
    }

protected:
    /** The Jack client callback - to be implemented by your inheriting class
    \param nframes The number of frames to process.
    \return 0 to keep processing, a different number on error.
    */
    virtual int processAudio(jack_nframes_t nframes)=0;

    /** The Jack client callback - to be implemented by your inheriting class if you need to do things when the buffer size changes
    \param nframes The number of frames to process when processAudio is called.
    \return 0 on success, a different number on error.
    */
    virtual int bufferSizeChange(jack_nframes_t nframes){
        cout<<"JackClient::bufferSizeChangeStatic : New buffer size = "<<nframes<<endl;
        return 0;
    }

public:
    /** Constructor.
    */
    JackClient(void) : JackBase() {}

    /** Constructor. Connecting the client to the default server.
    \param clientName_ The client name, which will initiate a server connection.
    */
    JackClient(string clientName_) : JackBase(clientName_) {}

    /// Destructor
    virtual ~JackClient() {
        disconnect(); // if the client is running, then stop the client and disconnect from the server
    }

    /** Connect to the server
    This starts the server and sets up the process callback/arg to use.
    \param clientName_ The name of the client to use
    \return NO_ERROR on success
    */
    virtual int connect(string clientName_) {
        return JackBase::connect(clientName_);
    }

    /** Connect to the server
        This starts the server and sets up the process callback/arg to use.
        \param clientName_ The name of the client to use
        \param serverName The name of the server to use
        \return NO_ERROR on success
    */
    virtual int connect(const string &clientName_, const string &serverName) {
        JackBase::connect(clientName_, serverName); // get the jack client connected

        if (0 != jack_set_process_callback(client, processAudioStatic, this))
            return JackDebug().evaluateError(JACK_SET_CALLBACK_ERROR);

        if (0 != jack_set_buffer_size_callback(client, bufferSizeChangeStatic, this))
            return JackDebug().evaluateError(JACK_SET_BUFFSIZE_CALLBACK_ERROR);

        return NO_ERROR;
    }

    /** Get the server buffer size (block size)
    \return the current buffer size
    */
    virtual int getBlockSize() {
        return jack_get_buffer_size (client);
    }


    /** Set the server buffer size (block size)
    \return 0 on success
    */
    int setBlockSize(int size) {
        int res=jack_set_buffer_size(client,size);
        if (res!=0)
            return JACK_SET_BUFFER_SIZE_ERROR;
        return NO_ERROR;
    }

    /** Start the client running.
    Activate jack but don't connect any ports.
    \return 0 if ok or error on failure
    */
    virtual int startClient() {
        /* Tell the JACK server that we are ready to roll.  Our
        * process() callback will start running now. */

        return jack_activate(client);
    }

    /** Start the client running.
    If the doConnect is true, then an attempt is made to auto connect the ports to the system ports. If auto connection fails, the client is still running.
    \param inCnt The number of input ports to connect to
    \param outCnt The number of output ports to connect to
    \param doConnect Auto-connect inputs and outputs to the system ports.
    \return 0 if ok or, JACK_HAS_NO_CAPTURE_PORTS_ERROR, JACK_HAS_NO_PLAYBACK_PORTS_ERROR
    */
    virtual int startClient(int inCnt, int outCnt, bool doConnect=true) {

        if (startClient())
            return JackDebug().evaluateError(JACK_ACTIVATE_ERROR);


        if (doConnect) {
            vector<jack_port_t *> inPorts, outPorts; // find the physical ports and auto connect
            int inCntLocal=getPortListAndCount((JackPortFlags)(JackPortIsPhysical|JackPortIsOutput), &inPorts, NULL, NULL);
            if (inCntLocal==0)
                return JackDebug().evaluateError(JACK_HAS_NO_CAPTURE_PORTS_ERROR);
            if (inCnt<inCntLocal)
                inCntLocal=inCnt;
            for (int i=0; i<inCntLocal; i++)
                if (jack_connect(client, jack_port_name(inPorts[i]), jack_port_name(inputPorts[i])))
                    cerr<<"cannot connect input port "<<i<<endl;

            int outCntLocal=getPortListAndCount((JackPortFlags)(JackPortIsPhysical|JackPortIsInput), &outPorts, NULL, NULL);
            if (outCntLocal==0)
                return JackDebug().evaluateError(JACK_HAS_NO_PLAYBACK_PORTS_ERROR);
            if (outCnt<outCntLocal)
                outCntLocal=outCnt;
            for (int i=0; i<outCntLocal; i++)
                if (jack_connect(client, jack_port_name(outputPorts[i]), jack_port_name(outPorts[i])))
                    cerr<<"cannot connect output port "<<i<<endl;
        }
        return 0;
    }
};
#endif // JACKCLIENT_H_
