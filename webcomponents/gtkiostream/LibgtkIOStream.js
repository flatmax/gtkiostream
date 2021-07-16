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

"use strict";

const fs = require('fs');

var libgtkIOStream = require('./libgtkIOStreamNode');
libgtkIOStream().then((mod)=>{
  console.log('wasm compiled')
  libgtkIOStream = mod;
});

class LibgtkIOStream {

  constructor(){
    this.sox = new libgtkIOStream.Sox;
  }

  /** Taken from : https://github.com/flatmax/WASMAudio
  malloc a WASM heap based on an audio matrix size. If the audio buffer
  channel count or frame count is changed, then free and malloc again.
  We remember size here to check if the heap frame count is different.
  \param byteLength The number of bytes in each channel
  \param chCnt The number of channels
  \param heapName For example 'inBufs'
  */
  mallocHEAP(byteLength, chCnt, heapName){
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

  /** Print the formats and return them
  @returns the formats
  */
  printFormats(){
    return this.sox.printFormats();
  }

  /** Read an audio file to memory
  @param fn The file name to read
  @returns the audio
  */
  readAudio(fn){
    let data = fs.readFileSync(fn);
    let Nmem = this.mallocHEAP(data.length, 1, 'audio'); // resize the heap
    libgtkIOStream.HEAPU8.set(data, this.audio);
    this.sox = new libgtkIOStream.Sox;
    let ret=this.sox.openRead(this.audio, this.audioSize);
    if (ret!=-40026) {
        throw Error('error in opening');
    }
    this.sox.setMaxVal(1);
    let frameCnt=Nmem*4;
    ret=this.sox.read(frameCnt); // read all of the audio
    if (ret!=0) {
        throw Error('error reading');
    }

    let ch=this.sox.getCols();
    let N=this.sox.getRows();
    let _fs=this.sox.getFSIn();
    console.log(''+ch+' channels')
    console.log(''+N+' frames')
    console.log('fs='+_fs+' Hz')
    let audio=[];
    for (let c=0; c<ch; c++){
      audio.push(new Float32Array(N));
      for (let n=0; n<N; n++)
        audio[c][n]=this.sox.getSample(n, c);
    }
    return audio;
  }

  /** wreite audio data to file
  @param data is an array of Float32Array
  @param _fs is the sample rate
  @param name is the file name without the extension
  @param ext is the extension (file type) e.g. 'wav', 'flac', etc.
  */
  writeAudio(data, _fs, name, ext){
    let ch = data.length;
    let N = data[0].length;
    // prepare the audio data for writing out
    let type = Object.prototype.toString.call(data);
    console.log(type)
    let audio;
    switch (type){
      case '[object Array]':
        audio=new Float32Array(N*ch);
        for (let n=0; n<N; n++)
          for (let c=0; c<ch; c++)
            audio[c*N+n]=data[c][n];
        break;
      case '[object Float32Array]':
        audio = data;
      default:
        console.log('writeAudio only can handle Float32Array or Array[Float32Array, ...] inputs for data.');
        return -1;
    }

    // open the audio memory file for writing
    let ret=this.sox.openWrite(_fs, ch, 1.0, ext);
    if (ret)
        return ret;

    // write the audio data to a memory file
    let Nmem = this.mallocHEAP(ch*N*4, 1, 'audioOut'); // resize the heap
    libgtkIOStream.HEAPF32.set(audio, this.audioOut>>2);
    ret=this.sox.write(this.audioOut, N);
    if (ret!=(ch*N)){
      console.log('didn\'t write the correct amount of data when calling sox.write')
      return -1;
    }

    // write the audio file to disk
    let memPtr=this.sox.getMemFilePtr();
    let audioF = libgtkIOStream.HEAPU8.subarray(memPtr, memPtr+this.sox.getBufferSize());
    fs.writeFileSync(name+'.'+ext, audioF, 'binary');
    return 0;
  }
}

module.exports = {
  LibgtkIOStream
}
