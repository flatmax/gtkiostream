# \<sox-audio\>

Web component for loading (and saving to come) audio files of various formats in the browser.

This element allows you to bypass the webaudioAPI which reasmples the audio to the browser's chosen sample rate. This effects you if you simply want the binary audio data at the original sample rate.

If you don't want to do this in the browser, but want to do it in nodejs, you can look at the [example](https://github.com/flatmax/gtkiostream/blob/master/js/SoxTest.js) in the gtkiostream pacakge.

## Example instantiation of this element (from demo/index.html) :

```
<sox-audio></sox-audio>
```

## Example to get the element to load the audio :

```
function setURL(){
  var soxAudio = document.querySelector('sox-audio');
  soxAudio.url="11.Neutral.44k.wav";
}
```

## Event indicating that the WASM module is compiled and ready for use :

```
window.addEventListener('libgtkIOStream::initialised', (e)=>{
  console.log('sox-audio is ready for use');
  setURL();
})
```
## Where is the audio data stored ?

The audio data is stored in the element's variable audio like so :

```
var soxAudio = document.querySelector('sox-audio');
console.log(soxAudio.audio);
```

## How can I find the audio's sample rate ?

The sample rate of the input file can be found by querying sox :

```
var soxAudio = document.querySelector('sox-audio');
console.log(soxAudio.sox.getFSIn()+' Hz');
```

## To run the Demo

### Install modules

```
npm i
```

### Viewing Your Element

```
$ polymer serve
```

### Running Tests

```
$ polymer test
```
