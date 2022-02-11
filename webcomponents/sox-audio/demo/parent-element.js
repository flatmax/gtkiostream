import { html, LitElement } from 'lit';
import './sox-item';

class ParentElement extends LitElement {
  render(){
    return html`
      ${this.soxItems.map(item=>
        html`<sox-item></sox-item>`
      )}
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
}
customElements.define('parent-element', ParentElement);
