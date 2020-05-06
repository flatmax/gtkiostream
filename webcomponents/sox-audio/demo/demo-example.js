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
      <sox-audio>
        SoxAudio loading ...
      </sox-audio>
    `;
  }

  setURL(){
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    soxAudio.url="11.Neutral.44k.wav";
  }

  dumpAudio(){
    var soxAudio = this.shadowRoot.querySelector('sox-audio');
    console.log(soxAudio.audio)
  }
}
customElements.define('demo-example', DemoExample);
