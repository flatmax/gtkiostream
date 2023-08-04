
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
using namespace std;

using namespace ALSA;

int main(int argc, char *argv[]) {

	if (argc<3){
    cout<<"Usage:\n"<<argv[0]<<" variable_name alsa.conf or .asoundrc or oher alsa config file path/name"<<endl;
		cout<<"     : e.g. : "<<argv[0]<<" pcm.name.variable /etc/asound.conf"<<endl;
    return -1;
  }

	int err=0;
	Config config;
	if ((err=config.load(argv[1]))<0)
		return err;

	string vals;
	string key(argv[2]);
	err=config.search<string>(key, vals);
	if (err)
		return err;
	cout<<"key "<<key<<" val="<<vals<<endl;

	return err;
}
