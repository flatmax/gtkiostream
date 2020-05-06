# \<sox-audio\>

Web component for loading and saving audio files of various formats in nodejs or the browser.

This element allows you to bypass the webaudioAPI which resamples the audio to the browser's chosen sample rate. This effects you if you simply want the binary audio data at the original sample rate.

This element also allows you to save raw audio data to an audio file, such as wav, mp3, aiff, etc.

If you don't want to do this in the browser, but want to do it in nodejs, you can look at the gtkiostream npm dependency test/SoxTest.js OR you can look at the  [same file in the original git repo](https://github.com/flatmax/gtkiostream/blob/master/js/SoxTest.js) .

## Example instantiation of this element (from demo/index.html) :

```
<sox-audio></sox-audio>
```

## Example to load the audio :

```
function setURL(){
  var soxAudio = document.querySelector('sox-audio');
  soxAudio.url="11.Neutral.44k.wav";
}
```

## Example to save audio data to a memory file :

```
var soxAudio = this.shadowRoot.querySelector('sox-audio');
soxAudio.openWrite(sampleRate, channels, maxAbsVal, 'mp3'); // open the memory file for writing in mp3 format with a maximum possble sample magnitude of maxAbsVal
while (continue){ // this is your main audio process/write loop
  // get your audio data to write to file
  soxAudio.write(audio); // write some audio to the mp3 file
}
```

## Event indicating that the WASM module is compiled and ready for use :

The 'sox-audio-ready' event is emitted once the WASM code is compiled and is ready for use.

```
<sox-audio @sox-audio-ready=${this.myFuncWhenReady}></sox-audio>
```

## Where is the read audio data stored ?

The audio data is stored in the element's variable audio like so :

```
var soxAudio = document.querySelector('sox-audio');
console.log(soxAudio.audio);
```

## How can I find the audio's sample rate ?

The sample rate of the input file can be found by querying sox :

```
var soxAudio = document.querySelector('sox-audio');
console.log(soxAudio.sox.getFSIn()+' Hz'); // once audio data has been loaded
console.log(soxAudio.sox.getFSOut()+' Hz'); // once the file is open with openWrite
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
