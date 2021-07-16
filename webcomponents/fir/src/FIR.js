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

import { DSP } from 'libdsp/DSP.js';

/** WASM FIR convolver, floating point double (64 bit) precision
*/
export class FIR extends DSP {
  /// When the WASM code is ready initialise the FIR object
  WASMReady(){
    console.log("FIR::WASMReady")
    super.WASMReady();
    if (!this.fir)
      this.fir = new this.libdsp.FIR;
    this.dispatchEvent(new CustomEvent('FIR-ready', { detail: {} }));
  }

  /** Given a filter (h) and a signal (s), computer the impuse response
  @param h Float32Array data which is the filter
  @param s Float32Array data which is the signal to be filtered
  @returns Float32Array of length h.length+s.length which is the convolution h*s.
  */
  getImpulseResponse(h, s){
    if (!Float32Array.prototype.isPrototypeOf(h)) // we have been given a Float32Array
      throw new Error("FIR::getImpulseResponse : h is not a Float32Array class");
    if (!Float32Array.prototype.isPrototypeOf(s)) // we have been given a Float32Array
      throw new Error("FIR::getImpulseResponse : s is not a Float32Array class");

    let Nh=h.length; // create WASM memory for the filter
    let Nhb = this.mallocHEAP(Nh*4, 1, 'h'); // resize the heap ch*N 32 bit words
    this.libdsp.HEAPF32.subarray((this.h)>>2, (this.h+Nhb)>>2).set(h);
    this.fir.loadTimeDomainCoefficients(this.h, Nh, 1);//load in the filter
    let sHat = new Float32Array(Nh+s.length); // we create a new array to ensure tail of the conv. is all zero
    sHat.set(s);
    let Ns=sHat.length;
    let Nsb = this.mallocHEAP(Ns*4, 1, 's'); // resize the heap ch*N 32 bit words
    this.libdsp.HEAPF32.subarray((this.s)>>2, (this.s+Nsb)>>2).set(sHat);

    this.fir.init(Ns);
    this.fir.filter(this.s, this.s, Ns, 1); // filter the input and return the convolution to the same heap pointer
    return this.libdsp.HEAPF32.subarray((this.s)>>2, (this.s+Nsb)>>2); // return the float32array
  }
}
