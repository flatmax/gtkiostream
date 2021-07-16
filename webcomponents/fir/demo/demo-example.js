import { html } from 'lit-element';
import { FIR } from '../fir-';

/** Example demo element for testing element loading
*/
class DemoExample extends FIR {
  render() {
    return html`
      <style>
        :host { display: block; }
        div[hidden] {
          display: none;
        }
      </style>

      <fir-></fir->
    `;
  }

  WASMReady(){
    console.log('DemoExample::WASMReady')
    super.WASMReady();
    this.getImpulseIR();
  }

  /** Method to get an impulse response
  */
  getImpulseIR(){
    let Nh = 5;
    let Ns = 10;
    let h = new Float32Array(Nh);
    h[1]=1; // An impulse as a filter
    let s = new Float32Array(Ns);

    for (let n of s.keys())
      if (n % 2)
        s[n]=n+1;
      else
        s[n]=-(n+1);
    let hs = this.getImpulseResponse(h,s);
    console.log('h '+h);
    console.log('s '+s);
    console.log('hs '+hs);
    console.log('\nVisual Check :')
    console.log(s);
    console.log(hs.slice(h.lastIndexOf(1),s.length+1))
  }
}
customElements.define('demo-example', DemoExample);
