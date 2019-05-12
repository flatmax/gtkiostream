import {LibgtkIOStream} from 'gtkiostream/libgtkiostream-.js';

/**
 * `sox-audio`
 * Webcomponent using gtkiostream and sox for audio decoding
 *
 * @customElement
 * @polymer
 * @demo demo/index.html
 */
class SoxAudio extends LibgtkIOStream {
  static get properties() {
    return {
      url: { // url for fetching the audio
        type: String,
        value: null,
        observer: 'decodeURL'
      },
    };
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
      this.audio=[];
      for (let c=0; c<ch; c++){
        this.audio.push(new Float32Array(N));
        for (let n=0; n<N; n++)
          this.audio[c][n]=this.sox.getSample(n, c);
      }
      console.log(this.url+' loaded');
    }).catch((err) => {
      console.log(err)
    })
  }
}

window.customElements.define('sox-audio', SoxAudio);
