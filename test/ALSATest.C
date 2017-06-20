/* Copyright 2000-2015 Matt Flax <flatmax@flatmax.org>
   This file is part of GTK+ IOStream class set

   GTK+ IOStream is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GTK+ IOStream is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with GTK+ IOStream
*/

#include "ALSA/ALSA.H"
using namespace std;

using namespace ALSA;

int main(int argc, char *argv[]) {
    Playback playBack;
    Capture capture;

    unsigned int fs=playBack.getSampleRate();
    if (fs<0)
        return fs;

    cout<<"fs = "<<fs<<endl;

    int latency=2048;
    cout<<"latency = "<<(float)latency/(float)fs<<" s"<<endl;

    snd_pcm_format_t format;
    capture.getFormat(format);
    cout<<"format "<<capture.getFormatName(format)<<endl;

    capture.link(playBack);

    char *buffer=new char[latency*capture.getFormatBits()/8*2];
    if (!buffer)
      return -1;

    capture.setSilence(buffer, latency*2);

    cout<<capture.wait()<<endl;

    delete [] buffer;
    return 0;
}
