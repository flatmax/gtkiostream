/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
#include "Pixmap.H"
#include "Buttons.H"
#include "Box.H"

// Quit button callback
static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv );

    Window topWindow;

//    gtk_widget_set_size_request (topWindow.win, 1000, 500);

    VBox vBox;

    vBox<<BoxIS(FALSE,FALSE, FALSE)<<(Buttons()<<BUTTONLABELSTRUCT((char*)"Quit", quit, NULL)); // show a quit button

    Pixmap pixmap; // The drawing area object

    vBox << BoxIS(TRUE, TRUE)<<pixmap.getWidget();  // show the figure
    vBox.show(); // show the vertical box
    topWindow<< vBox; // add it to the top window

    gtk_main();
}

