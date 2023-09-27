import { html } from '../lit';
import {SoxAudio } from '../sox-audio';
import './parent-element';

class MultipleSox extends SoxAudio {
  render(){
    return html`
      <parent-element .soxItems=${this.soxItems}></parent-element>
    `;
  }

  static get properties() {
    return {
      soxItems: {type: Array}
    };
  }

  constructor() {
    super();
    this.soxItems = []
  }

  WASMReady() {
    super.WASMReady();
    console.log("Sox ready on multiple-sox element")
    // now generating the sox-item after libgtkiostream loaded
    this.soxItems = [1,2,3]
  }
}
customElements.define('multiple-sox', MultipleSox);
