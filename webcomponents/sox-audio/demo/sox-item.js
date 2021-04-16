import { html } from 'lit-element';
import { SoxAudio } from '../sox-audio';

class SoxItem extends SoxAudio {
  WASMReady() {
    super.WASMReady();
    console.log("Sox ready on sox-item element")
  }
}
customElements.define('sox-item', SoxItem);
