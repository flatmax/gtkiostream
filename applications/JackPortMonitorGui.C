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

/*
Author: Matt Flax <flatmax@flatmax.org>
Date: 2014.10.08
*/

#include "Window.H"
#include "JackPortMonitorGui.H"
using namespace std;

static void quit(void *wid, gpointer data){
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    gdk_threads_init();
    gdk_threads_enter();
    gtk_init( &argc, &argv );

    bool monitorPorts=true, autoConnectNetClients=true;
    JackPortMonitorGui jackPMG(monitorPorts, autoConnectNetClients); // init the jack port manager, use the non gui client type.

    jackPMG.reSyncPorts();

    cout<<"Jack : sample rate set to : "<<jackPMG.getSampleRate()<<" Hz"<<endl;

    Window topWindow;
    topWindow<<(VBox()<<BoxIS(false, true, false)<<(Buttons()<<BUTTONLABELSTRUCT("Quit", quit, NULL)) // add the quit button
                      <<BoxIS(true, true, true)<<jackPMG.HBox::show() // add the jack port manager gui
               ).show();

    gtk_main();
    gdk_threads_leave();
    return 0;
}
