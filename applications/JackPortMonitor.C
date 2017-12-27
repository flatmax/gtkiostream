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

/*
Author: Matt Flax <flatmax@flatmax.org>
Date: 2013.08.05
*/

#include <unistd.h>
#include "JackPortMonitor.H"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    bool monitorPorts=true, autoConnectNetClients=true;
    JackPortMonitor jackPM("port monitor", monitorPorts, autoConnectNetClients); // init the jack port manager, use the non gui client type.

    cout<<"Jack : sample rate set to : "<<jackPM.getSampleRate()<<" Hz"<<endl;

    jackPM.print(cout);

    while (1)
        sleep(360);

    return 0;
}
