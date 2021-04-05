import { libgtkIOStream, LibgtkIOStream } from 'gtkiostream/libgtkiostream.js';

/**
 * `sox-audio`
 * Webcomponent using gtkiostream and sox for audio decoding and audio file encoding
 *
 * To load and decode audio, set the url property.
 * To encode (save) audio to a memory file, first openWrite then write the audio.
 *
 * @customElement
 * @polymer
 * @demo demo/index.html
 */
export class SoxAudio extends LibgtkIOStream {
  static get properties() {
    return {
      url: {type: String}, // url for fetching the audio
      formats: {type: String}, // the supported audio file formats
      fsIn: {type: Number}, // The input sample rate
      fsOut: {type: Number}, // The input sample rate
      mimeType: {type: String}, // The mime type for the output audio format
    };
  }

  /** Observed properties are handled here.
  This is a litelement builtin
  */
  updated(changedProperties) {
    if (changedProperties.has('url')) // when the url property changes, handle it
      this.decodeURL();
  }

  /// When the WASM code is ready initialise the Sox object
  WASMReady(){
    if (!this.sox)
      this.sox = new (eval(this.moduleName).Sox);
    this.getFormats();
    this.dispatchEvent(new CustomEvent('sox-audio-ready', { detail: {} }));
  }

  /** Fetch the url and attempt to decode the audio from the binary data using Sox
  */
  decodeURL(){
    if (this.url == null)
      return;
    if (!this.sox){
      console.log('waiting for WASM to initialise, call again later');
      return;
    }
    fetch(this.url).then((response) => {
      return response.arrayBuffer();
    }).then(data => {
      this.decodeData(data);
      console.log(this.url+' loaded');
    }).catch((err) => {
      console.log(err)
      this.decodeError(err);
    })
  }

  async readBlob(blob) {
    let data = await new Response(blob).arrayBuffer();
    this.decodeData(data);
  }

  /**
   * Decode the audio buffer
   * @param data should be arraybuffer or blob
   */
  decodeData(data) {
    if (data instanceof Blob)
      return this.readBlob(data);

    this.mallocHEAP(data.byteLength, 1, 'audio'); // resize the heap
    eval(this.moduleName).HEAPU8.set(new Uint8Array(data), this.audio);
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
    this.fsIn=this.sox.getFSIn();
    console.log(''+ch+' channels')
    console.log(''+N+' frames')
    console.log('fs='+this.fsIn+' Hz')
    let Nb = this.mallocHEAP(N*4, ch, 'audioOut'); // resize the heap ch*N 32 bit words
    this.sox.getAudio(this.audioOut, ch, N);
    this.audio=[]; // load the audio into the audio array
    for (var c=0; c<ch; c++) // retrieve the audio channel data
      this.audio.push(new Float32Array(eval(this.moduleName).HEAPF32.subarray((this.audioOut+c*Nb)>>2, (this.audioOut+c*Nb+Nb)>>2)));
    this.freeHEAP('audioOut');
    this.decodeSuccess();
  }

  /** Overload this method on the client once successfully decode
  */
  decodeSuccess() {}

  /** Overload this method on the client once get decode error
  */
  decodeError(e) {}

  /** Method to prepare the system for writing audio to a memory file
  @param Fs The sample rate in Hz
  @param ch The number of channels
  @param maxVal The abs. maximum value of the output audio data
  @param ext The audio file type, for example 'wav', 'mp3', flac
  @return 0 on success, otherwise error. On exit this.fsOut=Fs whether there was success or not
  */
  openWrite(Fs, ch, maxVal, ext){
    if (!this.sox){
      console.log('waiting for WASM to initialise, call again later');
      return;
    }
    this.fsOut=Fs
    this._setMimeType(ext);
    return this.sox.openWrite(Fs, ch, 1.0, ext);
  }

  /** Set a suitable mime type from the audio file extension
  */
  _setMimeType(ext){
    this.mimeType='audio/'+ext;
    if (ext=='mp3')
      this.mimeType='audio/mpeg';
    if (ext=='aiff')
      this.mimeType='audio/x-aiff';
  }

  /** Method to encode audio to an audio file.
  @param audio The audio data to write to memory file
  @return 0 on success
  */
  write(audio){
    if (Float32Array.prototype.isPrototypeOf(audio)){ // we have been given a Float32Array
      // check that the number of samples are divisible by our channel count
      if (audio.length % this.sox.getChCntOut()){
        console.error('SoxAudio::write : error : the number of samples is not divisible by the number of channels');
        return -1;
      }
      let N=audio.length;
      let Nb = this.mallocHEAP(N*4, 1, 'audioOut'); // resize the heap ch*N 32 bit words
      eval(this.moduleName).HEAPF32.subarray((this.audioOut)>>2, (this.audioOut+Nb)>>2).set(audio);
      let ret=this.sox.write(this.audioOut, N/this.sox.getChCntOut());
      if (ret!=N){
        console.error('SoxAudio::write : error : didn\'t write the correct amount of data when calling sox.write, wrote '+ret+' instead of the desired '+(N*ch)+' smaples.')
        return -3;
      }
      return 0;
    }
    if (Array.prototype.isPrototypeOf(audio)){ // we need to convert the audio from arrays of audio to arrays of Float32Array
      let ch=audio.length;
      if (ch!=this.sox.getChCntOut()){
        console.error("SoxAudio::write : error : channel count mismatch you are trying to write "+ch+" channels, but you initialised openWrite with "+this.sox.getChCntOut()+" channels");
        return -1;
      }
      let N=audio[0].length;
      let Nb = this.mallocHEAP(N*4, ch, 'audioOut'); // resize the heap ch*N 32 bit words
      let n=0;
      for (let c=0; c<ch; c++, n+=N)
        eval(this.moduleName).HEAPF32.subarray((this.audioOut+c*Nb)>>2, (this.audioOut+c*Nb+Nb)>>2).set(audio[c]);
      let ret=this.sox.write(this.audioOut, N);
      if (ret!=(ch*N)){
        console.error('SoxAudio::write : error : didn\'t write the correct amount of data when calling sox.write, wrote '+ret+' instead of the desired '+(N*ch)+' smaples.')
        return -2;
      }
      return 0;
    }
    console.error('SoxAudio::write : error : Unknwon audio type.');
    return -4;
  }

  /** Give the file back to the user as a blob with the correct mime type
  */
  getBlob(){
    let memPtr=this.sox.getMemFilePtr();
    let audioF = libgtkIOStream.HEAPU8.subarray(memPtr, memPtr+this.sox.getBufferSize());
    return new Blob([audioF], {type: this.mimeType});
  }

  /** Get the available audio file types which can be handled
  @return a string of the available formats
  */
  getFormats(){
    if (!this.sox){
      console.log('waiting for WASM to initialise, call again later');
      return;
    }
    this.formats=this.sox.printFormats();
    return this.formats;
  }
}

window.customElements.define('sox-audio', SoxAudio);
