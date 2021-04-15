import { html } from 'lit-element';
import { SoxAudio } from '../sox-audio';

class SoxItem extends SoxAudio {
  // It never called
  WASMReady() {
    super.WASMReady();
    console.log("Sox ready on sox-item element")
  }

  firstUpdated() {
    // we replace WASMReady with this approach
    // we use window.libgtkIOStream to resolve the issue sox element defined after libgtkIOStream loaded
    if (window.libgtkIOStream) {
      if (!this.sox)
        this.sox = new (window.libgtkIOStream.Sox);
      this.getFormats();
      console.log(this.sox, " sox-item with window.libgtkIOStream")
    }
  }
}
customElements.define('sox-item', SoxItem);
