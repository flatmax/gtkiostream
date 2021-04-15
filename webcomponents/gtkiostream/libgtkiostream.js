import { LitElement } from 'lit-element';
import modProm from './libgtkIOStream.js';
export var libgtkIOStream;
modProm().then((mod)=>{
  libgtkIOStream = mod;
  window.libgtkIOStream = mod; // for rendered wasm that delay
  window.dispatchEvent(new CustomEvent('libgtkIOStream::initialised'));
})

/**
 * `gtkiostream-`
 * gtkiostream WASM loader
 *
 * @customElement
 * @polymer
 */
export class LibgtkIOStream extends LitElement {

  /** When ready define the moduleName based on the module file name
  */
  connectedCallback(){
    super.connectedCallback();

    // add a method to setup the WASM initalised callback
    window.addEventListener('libgtkIOStream::initialised', (e)=>{
      this.WASMReady();
    })

    this.moduleName = 'libgtkIOStream';
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
        eval(this.moduleName)._free(this[heapName]);
      this[heapName] = eval(this.moduleName)._malloc(N);
      this[heapName+'Size']=N;
    }
    return Nb;
  }

  /** Given a heap variable name, free the data.
  @param heapName The name of the variable to free
  */
  freeHEAP(heapName){
    if (this[heapName])
      eval(this.moduleName)._free(this[heapName]);
    if (this[heapName+'Size'])
      this[heapName+'Size']=null;
  }

  /// overload this to execute something when the WASM has finished compiling
  WASMReady(){
  }
}

window.customElements.define('libgtkiostream-', LibgtkIOStream);
