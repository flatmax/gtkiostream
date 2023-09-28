// Copyright (c) 2000-2021 Flatmax Pty Ltd. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of mad chops coder AU nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"use strict";

import libgtkIOStream from './libgtkIOStream_wasm.js';
import { LitElement } from 'lit';

if (typeof window === 'undefined'){ // nodejs
  await import("./DynamicImport.js");
} else {  // browser
}

/**
 * `gtkiostream-`
 * gtkiostream WASM loader
 *
 * @customElement
 * @polymer
 */
export class LibgtkIOStream extends LitElement {
  static get properties() {
    return {
      libgtkIOStream: { type: Object }
    }
  }

  constructor() {
    super();
    this.wasmReady = new Promise((resolve, reject) => {
      libgtkIOStream().then((mod)=>{
        this.libgtkIOStream = mod; // for rendered wasm that delay
        this.WASMReady();
        resolve();
      });
    });
  }

  /** malloc a WASM heap based on an audio matrix size. If the audio buffer
  channel count or frame count is changed, then free and malloc again.
  We remember size here to check if the heap frame count is different.
  \param byteLength The number of bytes in each channel
  \param chCnt The number of channelsthis.scriptName
  \param heapName For example 'inBufs'
  */
  mallocHEAP(byteLength, chCnt, heapName){
    let Nb=byteLength; // number of bytes
    let M=chCnt; // number of channels
    let N=M*Nb; // total byte count
    // resize memory if required
    if (this[heapName]==null || this[heapName+'Size']!=N){
      if (this[heapName]!=null)
        this.libgtkIOStream._free(this[heapName]);
      this[heapName] = this.libgtkIOStream._malloc(N);
      this[heapName+'Size']=N;
    }
    return Nb;
  }

  /** Given a heap variable name, free the data.
  @param heapName The name of the variable to free
  */
  freeHEAP(heapName){
    if (this[heapName])
      this.libgtkIOStream._free(this[heapName]);
    this[heapName]=null;
    if (this[heapName+'Size'])
      this[heapName+'Size']=null;
  }

  /// overload this to execute something when the WASM has finished compiling
  WASMReady(){
    console.log('wasm ready')
  }


  // The following methods are for nackwards compatability with the original NodeJS class

  /** Print the formats and return them
  @returns the formats
  */
  printFormats(){
    if (typeof window === 'undefined'){ // nodejs
      // ok
    } else {  // browser
      console.log('use the sox-audio element in the browser');
    }

    if (!this.sox)
      this.sox = new this.libgtkIOStream.Sox;
    return this.sox.printFormats();
  }

  /** Read an audio file to memory
  @param fn The file name to read
  @returns the audio
  */
  readAudio(fn){
    if (typeof window === 'undefined'){ // nodejs
      // ok
    } else {  // browser
      console.log('use the sox-audio element in the browser');
    }

    if (!this.sox)
      this.sox = new this.libgtkIOStream.Sox;
    let data = fs.readFileSync(fn);
    let Nmem = this.mallocHEAP(data.length, 1, 'audio'); // resize the heap
    this.libgtkIOStream.HEAPU8.set(data, this.audio);
    this.sox = new this.libgtkIOStream.Sox;
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
    if (typeof window === 'undefined'){ // nodejs
      // ok
    } else {  // browser
      console.log('use the sox-audio element in the browser');
    }

    if (!this.sox)
      this.sox = new this.libgtkIOStream.Sox;
    let ch = data.length;
    let N = data[0].length;
    // prepare the audio data for writing out
    let type = Object.prototype.toString.call(data);

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
    this.libgtkIOStream.HEAPF32.set(audio, this.audioOut>>2);
    ret=this.sox.write(this.audioOut, N);
    if (ret!=(ch*N)){
      console.log('didn\'t write the correct amount of data when calling sox.write')
      return -1;
    }

    // write the audio file to disk
    let memPtr=this.sox.getMemFilePtr();
    let audioF = this.libgtkIOStream.HEAPU8.subarray(memPtr, memPtr+this.sox.getBufferSize());
    fs.writeFileSync(name+'.'+ext, audioF, 'binary');
    return 0;
  }

}

export default LibgtkIOStream

if (typeof window === 'undefined'){ // nodejs
} else { // browser
  window.customElements.define('libgtkiostream-', LibgtkIOStream);
}
