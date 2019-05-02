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

let data = fs.readFileSync('test/testVectors/11.Neutral.44k.wav');
let Nmem = mallocHEAP(data.length, 1, 'audio'); // resize the heap
libgtkIOStream.HEAPU8.set(data, audio);
let Sox = new libgtkIOStream.Sox;
// Sox.printFormats();
let ret=Sox.openRead(audio, audioSize);
if (ret!=-40026) {
    throw Error('error in opening');
}
Sox.setMaxVal(1);
ret=Sox.read(0); // read all of the audio
if (ret!=0) {
    throw Error('error reading');
}

let ch=Sox.getCols();
let N=Sox.getRows();
let _fs=Sox.getFSIn();
console.log(''+ch+' channels')
console.log(''+N+' frames')
console.log('fs='+_fs+' Hz')
audio=[];
for (let c=0; c<ch; c++){
  audio.push(new Float32Array(N));
  for (let n=0; n<N; n++)
    audio[c][n]=Sox.getSample(n, c);
}

for (let n=0; n<N; n++){
  let str='';
  for (let c=0; c<ch; c++)
    str+=audio[c][n]+' ';
  console.log(str)
}
