
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

#include "OptionParser.H"

using namespace ALSA;

int printUsage(string name) {
    cout<<"\nUseage: \n"<<endl;
    cout<<name<<" [-D device] "<<endl;
    cout<<name<<" -D str : ALSA Device"<<endl;
    return 0;
}


int main(int argc, char *argv[]) {

  OptionParser op;

  int i=0;
  string help;
  if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0]);

  string device("hw:0"); // ALSA IO device
  int test=op.getArg<string>("D", argc, argv, device, i=0);
  cout<<"Using ALSA device : "<<device<<endl;

	int err=0;
	Control control;
	if ((err=control.openCard(device))<0)
		return err;

	control.scanMixer();

  while (1)
    sleep(1);

	if ((err=control.close())<0)
		return err;
	cout<<"exiting"<<endl;
	return err;
}
