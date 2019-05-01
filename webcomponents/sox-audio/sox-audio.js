import {html, PolymerElement} from '@polymer/polymer/polymer-element.js';

/**
 * `sox-audio`
 *
 *
 * @customElement
 * @polymer
 * @demo demo/index.html
 */
class SoxAudio extends PolymerElement {
  static get template() {
    return html`
      <style>
        :host {
          display: block;
        }
      </style>
      <h2>Hello [[prop1]]!</h2>
    `;
  }
  static get properties() {
    return {
      url: {
        type: String,
        value: null,
        observer: 'decodeURL'
      },
      scriptName: { // The name of the script to load : for example stripNAme.js
        type: String,
        value: '../libgtkIOStream.js',
        observer: 'scriptNameChange'
      },
    };
  }

  /** When ready get the WASM loaded and compiling.
  */
  connectedCallback(){
    super.connectedCallback();
    let idx = this.scriptName.indexOf(".js");
    if (idx==-1)
      return console.log("Error : the script name you provided doesn't end with .js");
    this.moduleName = this.scriptName.slice(0, idx);
    idx = this.moduleName.indexOf("/"); // strip any path from the modle name - needs fixing if the path is larger then one node
    if (idx)
      this.moduleName = this.moduleName.slice(idx+1, this.moduleName.length);
  }

  scriptNameChange(){
    console.log(this.moduleName)
    console.log('scriptNameChange : '+this.scriptName);
    let script = document.createElement('script');
    script.src = this.scriptName;
    document.head.appendChild(script);
  }

  /** malloc a WASM heap based on an audio matrix size. If the audio buffer
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
        eval(this.moduleName)._free(this[heapName]);
      this[heapName] = eval(this.moduleName)._malloc(N);
      this[heapName+'Size']=N;
    }
    return Nb;
  }

  freeHEAP(heapName){
    eval(this.moduleName)._free(this[heapName]);
    this[heapName+'Size']=null;
  }

  decodeURL(){
    console.log()
    if (this.url == null)
      return;
    fetch(this.url).then((response) => {
      return response.arrayBuffer();
    }).then((data) => {
      let Nmem = this.mallocHEAP(data.byteLength, 1, 'audio'); // resize the heap
      eval(this.moduleName).HEAPU8.set(new Uint8Array(data), this.audio);
      this.sox = new (eval(this.moduleName).Sox);
      let ret=this.sox.openRead(this.audio, this.audioSize);
      if (ret!=-40026) {
          throw Error('error in opening');
      }
      this.sox.setMaxVal(1);
      ret=this.sox.read(0); // read all of the audio
      if (ret!=0) {
          throw Error('error reading');
      }

      this.freeHEAP('audio');

      let ch=this.sox.getCols();
      let N=this.sox.getRows();
      console.log(''+ch+' channels')
      console.log(''+N+' frames')
      this.audio=[];
      for (let c=0; c<ch; c++){
        this.audio.push(new Float32Array(N));
        for (let n=0; n<N; n++)
          this.audio[c][n]=this.sox.getSample(n, c);
      }
    }).catch((err) => {
      console.log(err)
    })
  }
}

window.customElements.define('sox-audio', SoxAudio);
