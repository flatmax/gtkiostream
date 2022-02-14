import { LitElement, html } from 'lit';
import '../sox-audio';

/** Example demo element for testing element loading
*/
class DemoExample extends LitElement {
  render(){
    return html`
      <style>
        :host { display: block; }
        div[hidden] {
          display: none;
        }
        sox-audio {
          padding: 10px;
          margin: 10px 0;
          border: 1px solid #bcd
        }
      </style>

      <input type="button" value="load audio" @click="${this.setURL}"/>
      <input type="button" value="dump audio" @click="${this.dumpAudio}" />
      <input type="button" value="show formats" @click="${this.getFormats}" />
      <input type="button" value="save audio" @click="${this.memWrite}" />
      <input type="button" value="load blob audio" @click="${this.loadBlob}" />
      <sox-audio @sox-audio-ready=${this.getFormats}>
        SoxAudio loading ...
      </sox-audio>
      <div>
        available formats :
        <div>
          ${this.formats}
        </div>
      </div>
    `;
  }

  static get properties() {
    return {
      formats: {type: String}, // url for fetching the audio
      ext: {type: String}, // audio write file extension
    };
  }

  /** This opens and reads an audio file
  */
  setURL(){
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    soxAudio.url="11.Neutral.44k.wav";
  }

  /** dump the audio data to console.
  */
  dumpAudio(){
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    console.log(soxAudio.audio)
  }

  /** Get the available audio formats
  */
  getFormats(){
    console.log('demo-example getFormats');
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    this.formats=soxAudio.formats;
  }

  /** This method demonstrates a full write and extraction of audio for the user.
  Once the openWrite has been called, many write calls may follow - growing the audio file.
  */
  memWrite(){
    this.ext='wav';
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    if (!soxAudio.audio){
      console.log('load audio first')
      return;
    }
    let ch=soxAudio.audio.length;
    // find the max value of the audio data
    let maxVal = -1;
    for (let c=0; c<ch; c++){
      let maxV=Math.max.apply(null, soxAudio.audio[c].map(Math.abs));
      if (maxV>maxVal)
        maxVal=maxV;
    }
    soxAudio.openWrite(soxAudio.fsIn, ch, maxVal, this.ext);
    soxAudio.write(soxAudio.audio); // write the previously loaded audio to memory disk
    this.giveAudioToUser(soxAudio);
  }

  /* create a URL using the memory file in sox
  */
  createURL(){
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    if (!soxAudio.audio){
      console.log('load audio first')
      return;
    }
    let blob = soxAudio.getBlob();
    let url = window.URL.createObjectURL(blob);
    return url;
  }

  /** Give the audio file which was written to the user
  */
  giveAudioToUser(){
    let a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";
    a.href = this.createURL();
    a.download = 'audio.'+this.ext;
    a.click();
    window.URL.revokeObjectURL(a.htrf);
  }

  /* test loading a blob
  */
  loadBlob(){
    this.ext='wav';
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    if (!soxAudio.audio){
      console.log('load audio first')
      return;
    }
    let ch=soxAudio.audio.length;
    // find the max value of the audio data
    let maxVal = -1;
    for (let c=0; c<ch; c++){
      let maxV=Math.max.apply(null, soxAudio.audio[c].map(Math.abs));
      if (maxV>maxVal)
        maxVal=maxV;
    }
    soxAudio.openWrite(soxAudio.fsIn, ch, maxVal, this.ext);
    soxAudio.write(soxAudio.audio); // write the previously loaded audio to memory disk
    console.log(this.createURL())
    soxAudio.url = this.createURL();
  }
}
customElements.define('demo-example', DemoExample);
