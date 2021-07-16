# \<fir->

WASM Webcomponent fir- for digital signal processing FIR convolution with floating point double (64 bit) precision.

This webcomponent follows the [open-wc](https://github.com/open-wc/open-wc) recommendation.

## Installation
```bash
npm i fir-dsp
```

## Usage
```html
<script type="module">
  import 'fir-/fir-.js';
</script>

<fir-></fir->
```

## Local Demo with `web-dev-server`
```bash
npm start
```
To run a local development server that serves the basic demo located in `demo/index.html`
look at the console log for output

## Demoing with Storybook
To run a local instance of Storybook for your component, run
```bash
npm run storybook
```

To build a production version of Storybook, run
```bash
npm run storybook:build
```


## Tooling configs

For most of the tools, the configuration is in the `package.json` to reduce the amount of files in your project.

If you customize the configuration a lot, you can consider moving them to individual files.
