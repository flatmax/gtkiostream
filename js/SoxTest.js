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

var fs = require('fs');
var libgtkIOStream = require('../src/.libs/libgtkIOStreamNode');

/** Taken from : https://github.com/flatmax/WASMAudio
malloc a WASM heap based on an audio matrix size. If the audio buffer
channel count or frame count is changed, then free and malloc again.
We remember size here to check if the heap frame count is different.
\param byteLength The number of bytes in each channel
\param chCnt The number of channels
\param heapName For example 'inBufs'
*/
function mallocHEAP(byteLength, chCnt, heapName){
  let Nb=byteLength; // number of bytes
  let M=chCnt; // number of channels
  let N=M*Nb; // total byte count
  // resize memory if required
  if (this[heapName]==null || this[heapName+'Size']!=N){
    if (this[heapName]!=null)
      libgtkIOStream._free(this[heapName]);
    this[heapName] = libgtkIOStream._malloc(N);
    this[heapName+'Size']=N;
  }
  return Nb;
}

let data = fs.readFileSync('../test/testVectors/11.Neutral.44k.wav');
let N = mallocHEAP(data.length, 1, 'audio'); // resize the heap
libgtkIOStream.HEAPU8.set(data, audio);
let Sox = new libgtkIOStream.Sox;
// Sox.printFormats();
let ret=Sox.openRead(audio, audioSize);
