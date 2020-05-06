import { LitElement, html } from 'lit-element';
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
    };
  }

  setURL(){
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    soxAudio.url="11.Neutral.44k.wav";
  }

  dumpAudio(){
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    console.log(soxAudio.audio)
  }

  getFormats(){
    console.log('demo-example getFormats');
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    this.formats=soxAudio.formats;
  }
}
customElements.define('demo-example', DemoExample);
