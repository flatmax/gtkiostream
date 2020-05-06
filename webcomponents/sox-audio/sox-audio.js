import {LibgtkIOStream} from 'gtkiostream/libgtkiostream.js';

/**
 * `sox-audio`
 * Webcomponent using gtkiostream and sox for audio decoding
 *
 * @customElement
 * @polymer
 * @demo demo/index.html
 */
export class SoxAudio extends LibgtkIOStream {
  static get properties() {
    return {
      url: {type: String}, // url for fetching the audio
    };
  }

  /** Observed properties are handled here.
  This is a litelement builtin
  */
  updated(changedProperties) {
    if (changedProperties.has('url')) // when the url property changes, handle it
      this.decodeURL();
  }

  /** Fetch the url and attempt to decode the audio from the binary data using Sox
  */
  decodeURL(){
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
      let _fs=this.sox.getFSIn();
      console.log(''+ch+' channels')
      console.log(''+N+' frames')
      console.log('fs='+_fs+' Hz')
      let Nb = this.mallocHEAP(N*4, ch, 'audioOut'); // resize the heap ch*N 32 bit words
      this.sox.getAudio(this.audioOut, ch, N);
      this.audio=[]; // load the audio into the audio array
      for (var c=0; c<ch; c++) // retrieve the audio channel data
        this.audio.push(new Float32Array(eval(this.moduleName).HEAPF32.subarray((this.audioOut+c*Nb)>>2, (this.audioOut+c*Nb+Nb)>>2)));
      this.freeHEAP('audioOut');
      console.log(this.url+' loaded');
      this.decodeSuccess();
    }).catch((err) => {
      console.log(err)
      this.decodeError(err);
    })
  }

  /** Overload this method on the client once successfully decode
  */
  decodeSuccess() {}

  /** Overload this method on the client once get decode error
  */
  decodeError(e) {}
}

window.customElements.define('sox-audio', SoxAudio);
