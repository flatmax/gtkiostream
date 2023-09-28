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

 import fs from 'fs';
 import LibgtkIOStream  from '../LibgtkIOStream.js';

 let libgtkIOStream = new LibgtkIOStream();

 libgtkIOStream.wasmReady.then(()=>{
  //////////////////////////
  // Audio file read test
  let fn = '11.Neutral.44k.wav';
  let audio = libgtkIOStream.readAudio(fn);
  let ch=libgtkIOStream.sox.getCols();
  let N=libgtkIOStream.sox.getRows();
  let _fs=libgtkIOStream.sox.getFSIn();

  console.log(''+ch+' channels')
  console.log(''+N+' frames')
  console.log('fs='+_fs+' Hz')
  // let audio=[];
  // for (let c=0; c<ch; c++){
  //   audio.push(new Float32Array(N));
  //   for (let n=0; n<N; n++)
  //     audio[c][n]=sox.getSample(n, c);
  // }

  // print out the audio
  for (let n=0; n<10; n++){
    let str='';
    for (let c=0; c<ch; c++)
      str+=audio[c][n]+' ';
    console.log(str)
  }


  // open the audio memory file for writing
  let ret = libgtkIOStream.writeAudio(audio, _fs, '/tmp/sox3.js', 'wav');
  console.log('returns '+ret);
});
