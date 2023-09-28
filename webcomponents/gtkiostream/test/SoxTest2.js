#! /usr/bin/env node
/* Copyright 2000-2019 Matt Flax <flatmax@flatmax.org>
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

 // NOTE : This is for testing the wasm system, in the browser you should use sox-audio for sox audio read/write

import fs from 'fs';
import LibgtkIOStream  from '../LibgtkIOStream.js';

let libgtkIOStream = new LibgtkIOStream();

libgtkIOStream.wasmReady.then(()=>{

  let data = fs.readFileSync('clippedAudio.wav');
  let Nmem = libgtkIOStream.mallocHEAP(data.length, 1, 'audio'); // resize the heap
  libgtkIOStream.libgtkIOStream.HEAPU8.set(data, libgtkIOStream.audio);
  let Sox = new libgtkIOStream.libgtkIOStream.Sox;
  let formats=Sox.printFormats();
  let ret=Sox.openRead(libgtkIOStream.audio, libgtkIOStream.audioSize);
  if (ret!=-40026) {
      throw Error('error in opening');
  }
  Sox.setMaxVal(1);
  ret=Sox.read(0); // read all of the audio
  if (ret!=0) {
      throw Error('error reading');
  }

  console.log('');
  if (Sox.getReadClips())
    console.log('The audio file loaded with clipping. It clipped '+Sox.getReadClips()+' times.'+'\nPlease reduce the magnitude of the recorded audio file and load again.')
  else
    console.log('The audio file loaded cleanly (with no clipping).');
});
