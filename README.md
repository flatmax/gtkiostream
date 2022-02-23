# gtkIOStream

[Support this project by purchasing our hardware.](http://www.audioinjector.net/)

Trivialised Threading, GTK GUIs, Signal Processing, Neural Networks, Audio Processing, Octave interfacing ... and more !

## Description

Elements of DSP. Neural Network computations.

```
FIR<double> fir;
fir.loadTimeDomainCoefficients(h);
for (int m=0; m<M; i++){ // apply the filter
  // get a new input x, then filter it
  fir.filter(x, y);
  // output or save y
}
```

A complete ALSA stack. 

```
const string deviceName="default";
ALSA::Playback playBack(deviceName.c_str());
Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> buffer;
while (1){
    creatAudioData(buffer);
    playBack<<buffer; // play the audio data
}

```

Simplistic threading and locking classes.

```
ThreadedMethodClass threadedMethodClass;
res=threadedMethodClass.run();
cout<<"Waiting for the second thread to exit"<<endl;
cout<<threadedMethodClass.meetThread()<<endl;
```

Other features like creating GTK+ interfaces using IOStream style code.
Its simple and quick!

You can create GTK+ GUIs in a few lines of code.

Labels<<"Thanks for reading";
(HBox<<Labels).show();

or even one line :
(HBox() << (Labels()<<"Thanks for reading")).show();

Inline code destructs the classes, but leaves the widgets/callbacks operating. This is great for embedded memory saving code.

You can also plot like so :
Plot figure; figure.plot(x, y, x.size());

## Some only depend on headers

For example if you only want to use the ALSA or GTK GUI stuff, then no need to build the project, simply include the header files in your project.

If you want to make and install, then like so :

install some prerequisites : g++ make automake libtool pkg-config

For Debian based systems,

    sudo apt install libeigen3-dev libfftw3-dev libsox-dev libasound2-dev #libjack-dev liboctave-dev

For Fedora Linux, you might want to do, in addition:

    sudo dnf install eigen3-devel sox-devel jack-audio-connection-kit-devel gl2ps-devel suitesparse-devel arpack-devel qrupdate-devel

Then:

./configure
make
make install

If you wish to start, try this :
g++ `pkg-config --cflags gtk+-2.0` LabelsTest.C -o LabelsTest `pkg-config --libs gtk+-2.0`
./LabelsTest

For Unix/Linux etc. Users :

Run the auto conf script :
./tools/autotools.sh

Run configure  : ./configure --prefix=/usr/local
Run make : make
Run make install : make install

That should install it in a suitable directory.

## The obsolete repo (before git) lives on sf

https://sourceforge.net/projects/gtkiostream/

Matt R. Flax <flatmax@>
