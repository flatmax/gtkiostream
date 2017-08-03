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
#include "DragNDrop.H"
#include "Buttons.H"
#include "Box.H"
#include "TextView.H"

/* This is a very simple example to show how to simply setup the source and destination for GTK drag and drop.
*/

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    // Normal GTK from here

    gtk_init( &argc, &argv );

    TextView textViewFrom, textViewTo; // text view to have viewable text
    textViewFrom<<"Try dragging me ...";
    textViewTo<<"... To here !";

    // DND related code from here

    DragNDrop dnd; // instantiate a drag and drop class
    *(Widget*)(&textViewFrom)>>dnd; // Tell the Widget it will be dragged around
    *(Widget*)(&textViewTo)<<dnd; // Tell the other widget it will be the dragging destination

    // Normal GTK code from here

    VBox vBox; // vertical box to hold all of the widgets
    vBox<<BoxIS(false, true, false)<<(Buttons()<<BUTTONLABELSTRUCT("Quit", quit, NULL)); // Add the quit button to the end
    vBox<<BoxIS(true, true, true)<<textViewFrom.show()<<textViewTo.show(); // put the two DND widgets into the VBox

    Window topWindow; // the top window
    topWindow<<vBox.show();
    gtk_main();
}


