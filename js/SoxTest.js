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
var libgtkIOStream = require('./libgtkIOStreamNode');

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

//////////////////////////
// Audio file read test
let data = fs.readFileSync('11.Neutral.44k.wav');
let Nmem = mallocHEAP(data.length, 1, 'audio'); // resize the heap
libgtkIOStream.HEAPU8.set(data, audio);
let sox = new libgtkIOStream.Sox;
let formats=sox.printFormats();
let ret=sox.openRead(audio, audioSize);
if (ret!=-40026) {
    throw Error('error in opening');
}
sox.setMaxVal(1);
ret=sox.read(0); // read all of the audio
if (ret!=0) {
    throw Error('error reading');
}

let ch=sox.getCols();
let N=sox.getRows();
let _fs=sox.getFSIn();
console.log(''+ch+' channels')
console.log(''+N+' frames')
console.log('fs='+_fs+' Hz')
audio=[];
for (let c=0; c<ch; c++){
  audio.push(new Float32Array(N));
  for (let n=0; n<N; n++)
    audio[c][n]=sox.getSample(n, c);
}

// print out the audio
// for (let n=0; n<N; n++){
//   let str='';
//   for (let c=0; c<ch; c++)
//     str+=audio[c][n]+' ';
//   console.log(str)
// }

//////////////////////////
// Audio file write test

// prepare the audio data for writing out
audio=new Float64Array(N*ch);
  for (let n=0; n<N; n++){
    for (let c=0; c<ch; c++){
    audio[c*N+n]=sox.getSample(n, c);
  }
  if (n<10)
    console.log(audio[n]+' '+audio[N+n])
}

// open the audio memory file for writing
ret=sox.openWrite(_fs, ch, 1.0, "wav");
if (ret)
    throw Error('error in opening write');

// write the audio data to a memory file
Nmem = mallocHEAP(ch*N*8, 1, 'audioOut'); // resize the heap
libgtkIOStream.HEAPF64.set(audio, audioOut>>3);
ret=sox.write(audioOut, N);
if (ret!=(ch*N)){
  console.log('didn\'t write the correct amount of data when calling sox.write')
  return;
}

// write the audio file to disk
let memPtr=sox.getMemFilePtr();
let audioF = libgtkIOStream.HEAPU8.subarray(memPtr, memPtr+sox.getBufferSize());
fs.writeFileSync('/tmp/sox.js.wav', audioF, 'binary');
