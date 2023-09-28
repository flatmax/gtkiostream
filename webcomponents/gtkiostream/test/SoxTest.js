#! /usr/bin/env node
/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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
  //////////////////////////
  // Audio file read test
  let data = fs.readFileSync('11.Neutral.44k.wav');
  // let data = fs.readFileSync('11.Neutral.44k.flac');
  let Nmem = libgtkIOStream.mallocHEAP(data.length, 1, 'audio'); // resize the heap
  libgtkIOStream.libgtkIOStream.HEAPU8.set(data, libgtkIOStream.audio);
  let sox = new libgtkIOStream.libgtkIOStream.Sox;
  let formats=sox.printFormats();
  let ret=sox.openRead(libgtkIOStream.audio, libgtkIOStream.audioSize);
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
  let audio=[];
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
  //

  //////////////////////////
  // Audio file write test

  // prepare the audio data for writing out
  audio=new Float32Array(N*ch);
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
  Nmem = libgtkIOStream.mallocHEAP(ch*N*4, 1, 'audioOut'); // resize the heap
  libgtkIOStream.libgtkIOStream.HEAPF32.set(audio, libgtkIOStream.audioOut>>2);
  ret=sox.write(libgtkIOStream.audioOut, N);
  if (ret!=(ch*N)){
    console.log('didn\'t write the correct amount of data when calling sox.write')
    return;
  }

  // write the audio file to disk
  let memPtr=sox.getMemFilePtr();
  let audioF = libgtkIOStream.libgtkIOStream.HEAPU8.subarray(memPtr, memPtr+sox.getBufferSize());
  fs.writeFileSync('/tmp/sox.js.wav', audioF, 'binary');
});
