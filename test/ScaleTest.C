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
#include "Window.H"
#include "Scales.H"
#include "Buttons.H"
#include "Box.H"

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

static void scaleTouch(void *widget, gpointer data) {
    cout<<"scaleTouch "<<data<<endl;
    cout<<"set to "<<Scales().getVal((GtkRange*)widget)<<endl;;
}

int main(int argc, char *argv[]) {
    gtk_init( &argc, &argv );

    Window topWindow;
    gtk_widget_set_size_request (topWindow.getWidget(), 200, 300);

    Scales scales;
    scales<<HScaleFuncData(0.,100.,1.,scaleTouch,NULL)<<VScaleFuncData(0.,100.,10.,scaleTouch,NULL)<<VScaleFuncData(.7,1.51,.001,scaleTouch,NULL);
    scales.grab(1);
    scales=50.9;
    scales.grab(2);
    scales=50.9;

    scales.grab(3); scales.quantise(true);

    Buttons buttons;
    buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL);

    topWindow<< (HBox()<<scales.grab(2)<<BoxIS(true,true)<<
                 (VBox()<<BoxIS(true,true,true)<< scales.grab(1)<<buttons).show()
                 <<BoxIS(true,true)<<scales.grab(3)
                ).show();

    gtk_main();
}
