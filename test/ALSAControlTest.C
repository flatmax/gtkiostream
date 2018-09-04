
/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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
#include <iostream>
using namespace std;

using namespace ALSA;

int main(int argc, char *argv[]) {
	int err=0;
	Control control;
	if ((err=control.attach("hw:0"))<0)
		return err;

	control.run();
	cout<<"Control thread started"<<endl;
	// if ((err=control.openCard("hw:0"))<0)
	// 	return err;

	// control.scanMixer();
	// //	cout<<mixer<<endl;
	// string state=mixer.getState();
	// cout<<state<<endl;
	//
	// if ((err=mixer.setPlaybackVol("Master", 50))<0)
	// 	return err;
	// double dB=-10.;
	// cout<<"setting playback vol to "<<dB<<" dB"<<endl;
	// if ((err=mixer.setPlaybackVolDB("Master", dB))<0)
	// 	return err;
	// cout<<"vol set to "<<dB<<" dB"<<endl;
	//
	// long val=10;
	// cout<<"setting PCM channel 1 vol to "<<val<<endl;
	// if ((err=mixer.setPlaybackChVol("PCM", (snd_mixer_selem_channel_id_t)1, val))<0)
	// 	return err;
	// cout<<"PCM channel 1 vol is now "<<val<<endl;
	// dB=-10.;
	// cout<<"setting PCM ch 0 playback vol to "<<dB<<" dB"<<endl;
	// if ((err=mixer.setPlaybackChVolDB("PCM", (snd_mixer_selem_channel_id_t)0, dB))<0)
	// 	return err;
	// cout<<"vol set to "<<dB<<" dB"<<endl;
	// cout<<"\n mixer PCM state is now : "<<endl;
	// cout << mixer.getElementState("PCM")<<endl;
	//
	// dB=-5.;
	// cout<<"setting playback vol to "<<dB<<" dB"<<endl;
	// string json=mixer.setPlaybackVolDBJSON("Master", dB);
	// cout<<json<<endl;
	//
	// dB=-5.1;
	// cout<<"setting PCM playback ch 1 vol to "<<dB<<" dB"<<endl;
	// json=mixer.setPlaybackChVolDBJSON("PCM", (snd_mixer_selem_channel_id_t)1, dB);
	// cout<<json<<endl;
	//
	// dB=0.;
	// cout<<"setting capture vol to "<<dB<<" dB"<<endl;
	// json=mixer.setCaptureVolDBJSON("Capture", dB);
	// cout<<json<<endl;
	//
	// int sw;
	// if ((sw=mixer.setPlaybackSwitch("Master", 0))<0)
	// 	return sw;
	// cout<<"Master switch = "<<sw<<endl;
	// sw=1;
	// json=mixer.setPlaybackSwitchJSON("Master", sw);
	// cout<<json<<endl;
	//
	// if ((sw=mixer.setCaptureSwitch("Capture", 0))<0)
	// 	return sw;
	// cout<<"Capture switch = "<<sw<<endl;
	// sw=1;
	// json=mixer.setCaptureSwitchJSON("Capture", sw);
	// cout<<json<<endl;

	while(1)
		sleep(1);

	if ((err=control.close())<0)
		return err;
	cout<<"exiting"<<endl;
	return err;
}
