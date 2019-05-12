import {html, PolymerElement} from '@polymer/polymer/polymer-element.js';

/**
 * `gtkiostream-`
 * gtkiostream WASM loader
 *
 * @customElement
 * @polymer
 */
export class LibgtkIOStream extends PolymerElement {

//   static get properties() {
//     return {
//       moduleName: { // the module name once loaded
//         type: String,
//         value: 'libgtkIOStream'
//       }
//     };
// }
  /** When ready define the moduleName based on the module file name
  */
  connectedCallback(){
    super.connectedCallback();
    this.moduleName = 'libgtkIOStream';
    let script = document.createElement('script');
    script.src = 'libgtkIOStream.js';
    document.head.appendChild(script);
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
}

window.customElements.define('libgtkiostream-', LibgtkIOStream);
