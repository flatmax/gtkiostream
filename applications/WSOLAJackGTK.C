/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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
#include "WSOLA.H"
#ifndef _MSC_VER
#include "Sox.H"
#else
// Note : Microsoft doesn't understand the different between upper and lower case in file names.
// on microsoft, you have to manually rename Sox.H to SoxWindows.H
#include "SoxWindows.H"
#endif


#include <OptionParser.H>
#include <Window.H>
#include <Buttons.H>
#include <Scales.H>
#include <Box.H>

#include "WSOLAJack.H"

#include <sstream>

void printUsage(const char *str) {
    cerr<<"Usage: "<<str<<" -h or --help"<<endl;
    cerr<<"Usage: "<<str<<" fileName.wav"<<endl;
    cerr<<"\t the fileName can be any readable audio file format."<<endl;
    cerr<<"\n Author : Matt Flax <flatmax@>"<<endl;
    exit(0);
}

static void scaleTouch(void *widget, gpointer data) {
    WSOLAJack *wsj=reinterpret_cast<WSOLAJack *>(data);
    FP_TYPE newScale=Scales().getVal((GtkRange*)widget);
    wsj->setTimeScale(newScale);
}

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    if (argc<2)
        printUsage(argv[0]);

    OptionParser op;
    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        printUsage(argv[0]);
    if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
        printUsage(argv[0]);

    string fileName(argv[argc-1]);
    cout<<"input file = "<<fileName<<endl;

    WSOLAJack WSOLAjack(fileName);

    gtk_init( &argc, &argv );
    Window topWindow;

    Scales speed;
    speed<<VScaleFuncData(.7,1.51,.01,scaleTouch,&WSOLAjack);
    speed=1.;
    topWindow<< (VBox()
                <<BoxIS(true,true,true) <<speed // the speed scale
                <<BoxIS(false,false,true) <<(Buttons()<<BUTTONLABELSTRUCT("Quit", quit, NULL))
                ).show();

    gtk_main();

    return NO_ERROR;
}


