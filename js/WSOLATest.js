#! /usr/bin/env node
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

libgtkIOStream = require('../src/.libs/libgtkIOStreamNode');

chCnt=2;
outSizePow2_=true; // force power of two output buffer sizing
WSOLA = new libgtkIOStream.WSOLA(chCnt, outSizePow2_);

// The system sample rate - get it from the audio system
fs=48000;
WSOLA.setFS(fs);

timeScale=1.2; // This value specifies whether to speed up or slow down the audio

// The number of audio sames to load initially
N=WSOLA.getSamplesRequired();

// Create the output data matrix
outputAudio= [];
for(m=0; m<chCnt; m++)
    outputAudio[m] = new Array(WSOLA.getOutputSize());

for (i=0; i<10; i++){
	console.log('We need '+chCnt+' channels of '+N+' audio samples each to be loaded');
	// We aren't using a real audio device, so for now we will use random data
	for (m=0; m<chCnt; m++)
		for (n=0; n<N; n++)
			WSOLA.loadInput(m, n, Math.random());

	console.log('processing');
	// We use process to execute WSOLA and produce the output audio data
	// timeScale can change with each iteration, allowing dynamic audio speed change
	// N tells us how much audio data to load in during the next iteration
	N=WSOLA.process(timeScale);

	// unload the audio data here
	console.log('We need '+chCnt+' channels of '+WSOLA.getOutputSize()+' audio samples each to be unloaded');
	for (m=0; m<chCnt; m++)
	 	for (n=0; n<WSOLA.getOutputSize(); n++)
	 		outputAudio[m][n]=WSOLA.unloadOutput(m, n);
}
