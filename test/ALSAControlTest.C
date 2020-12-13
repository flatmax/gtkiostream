
/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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
	if ((err=control.openCard("hw:0"))<0)
		return err;

	control.scanMixer();

	if ((err=control.setPlaybackVol("Master", 50))<0)
		return err;
	sleep(1);

	double dB=-5.;
	string json=control.setPlaybackVolDBJSON("Master", dB);

	sleep(1);
	if ((err=control.close())<0)
		return err;
	cout<<"exiting"<<endl;
	return err;
}
