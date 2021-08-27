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

import { LitElement } from 'lit-element';
import modProm from './libgtkIOStream.js';

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
    modProm().then((mod)=>{
      this.libgtkIOStream = mod; // for rendered wasm that delay
      this.WASMReady();
    })
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
  }
}

window.customElements.define('libgtkiostream-', LibgtkIOStream);
