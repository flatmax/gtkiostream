#ifndef MIXERTESTAUDIO_H
#define MIXERTESTAUDIO_H
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
Date: 2015.03.12
*/


#include <DSP/CrossoverAudio.H>

/** Class to test all Mixer channels through less mixer outputs.
The mixer is an external device to the computer the setup is like so :
computer output ====> mixer ======> computer input
*/
class MixerTestAudio : public CrossoverAudio {
    int currentOutputChannel; ///< The current output channel to test.

    virtual int processAudio(jack_nframes_t nframes); ///< The Jack client callback

    /// Superimposed sinusoidal test frequencies
    vector<float> testFrequencies;

    public:
        MixerTestAudio(); ///<Constructor
        virtual ~MixerTestAudio(); ///< Destructor

    /** Set the channel counts.
    \param inCnt The number of input channels.
    \param outCnt The number of output channels.
    \param testOutCnt The total number of channels to record for the entire test
    \return NO_ERROR on success, an error if audio is already playing/recording.
    */
    int setChannels(int outCnt, int inCnt, int testOutCnt);

    /** Reset the system, initialise ports and the output/input sample data;
    \return NO_ERROR on success, or when the playback thread is running, it stops it, resets and then returns, indicating that the audio thread was stopped, by returning thread lock error.
    */
    virtual int reset();

    /** Get number of recorded channels.
    \return the number of channels already recorded.
    */
    virtual int getNumberOfRecordedChannels(){return currentOutputChannel;}

};

#endif // MIXERTESTAUDIO_H
