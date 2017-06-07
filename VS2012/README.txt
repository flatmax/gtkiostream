
For now you have to run the applications shipped with the gtkIOStream precompiled binary from the command line. These include (WSOLA, WSOLAJackGTK and audioMasker).

Once installed, you can speed up and slow down audio using the following command at the command prompt :

WSOLA audioFile speed

For example to speed up test.wav by 20% :

WSOLA test.wav 1.2

Note WSOLA.exe may report and error at the end of processing, but the output file will be good.

To use the WSOLAJackGTK interface, you will need jack audio installed. This plays the audio you are speeding up and down so you can hear it.

Make sure you edit the system environment PATH variable to include the install path of the WSOLA.exe executable - if it isn't already present.


All the best.
www.flatmaxstudios.com
