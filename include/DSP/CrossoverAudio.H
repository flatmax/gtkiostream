#ifndef CROSSOVERAUDIO_H_
#define CROSSOVERAUDIO_H_

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
Date: 2014.11.09
*/

#include "JackClient.H"
#include "Thread.H"
#include <Eigen/Dense>
//using namespace Eigen;

/** Class to play and record audio data for analysis.
Usefull for measuing frequency responses.
In general, there is only one vector of output test data, it is played on all operating output channels.
*/
class CrossoverAudio : public JackClient {

    virtual int processAudio(jack_nframes_t nframes); ///< The Jack client callback

    /** Start the client running.
    If the doConnect is true, then an attempt is made to auto connect the ports to the system ports. If auto connection fails, the client is still running.
    \param inCnt The number of input ports to connect to
    \param outCnt The number of output ports to connect to
    \param doConnect Auto-connect inputs and outputs to the system ports.
    \return 0 if ok or, JACK_HAS_NO_CAPTURE_PORTS_ERROR, JACK_HAS_NO_PLAYBACK_PORTS_ERROR
    */
    virtual int startClient(int inCnt, int outCnt, bool doConnect);
protected:
    // variables setup globally
    float gain; ///< The gain for the output
    Mutex recordLock; ///< The lock for when the audio is being played/recorded.
    unsigned int zeroSampleCnt; ///< The number of samples to train with zeros

    // variables used in each test
    int samplesToProcess; ///< The number of samples to process, matching the duration
    int samplesProcessed; ///< The number of samples already processed
    int currentInputChannel; ///< The current input channel to test.
public:
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> audio; ///< The first channel is the same data sent over each output channel, then the output channels, then the input channels

    CrossoverAudio(); ///< Constructor : starts connecting to Jack audio
    virtual ~CrossoverAudio(); ///< Destructor

    /** Set the gain for the output data.
    \param g The gain to set.
    */
    void setGain(float g) {
        gain=g;
    }

    /** Get the gain for the output data.
    \return The gain
    */
    float getGain(void) {
        return gain;
    }

    /** Set the duration to sample for.
    \param d The duration in seconds
    \return NO_ERROR on success, an error if audio is already playing/recording.
    */
    int setDuration(float d);

    /** Get the duration for the output data.
    \return The duration in seconds
    */
    float getDuration(void) {
        return (float)audio.rows()*(float)getSampleRate();
    }

    /** Set the channel counts.
    \param inCnt The number of input channels.
    \param outCnt The number of output channels.
    \param testInCnt The total number of channels to record for the entire test
    \return NO_ERROR on success, an error if audio is already playing/recording.
    */
    int setChannels(int outCnt, int inCnt, int testInCnt);

    /** Get the number of input and output channels
    \param [out] inCnt The number of input channels.
    \param [out] outCnt The number of output channels.
    */
    void getChannels(int &inCnt, int &outCnt);

    /** Record the next set of channels.
    \return NO_ERROR on success.
    */
    int recordNextChannelSet();

    /** Reset to test another crossover without re-recording the channel.
    */
    void nextCrossover();

    /** Reset the system, initialise ports and the output/input sample data;
    \return NO_ERROR on success, or when the playback thread is running, it stops it, resets and then returns, indicating that the audio thread was stopped, by returning thread lock error.
    */
    virtual int reset();

    /** Get number of recorded channels.
    \return the number of channels already recorded.
    */
    virtual int getNumberOfRecordedChannels(){return currentInputChannel;}

    /** Test if the recording thread is operational (could also possibly return locked when a 'setter' method is operating.)
    \return 0 if not running, 1 if running.
    */
    int isRecording();
};

#endif // CROSSOVERAUDIO_H_
