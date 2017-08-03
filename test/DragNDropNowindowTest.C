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
#include "Labels.H"
#include "Box.H"
#include "Frame.H"
#include "TextView.H"
#include "EventBox.H"

/* In this test, no data is passed, merely to demonstrate how to drag a GtkWidget without a window, such as the Frame widget.
*/

static void quit(void *wid, gpointer data) { // quit button executes this
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    // Normal GTK from here

    gtk_init( &argc, &argv );

    TextView textViewTo; // text view to have viewable text
    textViewTo<<"... To here !";

    Frame fromFrame("Try dragging me ..."); // Frame doesn't have a window
    EventBox eb; eb<<fromFrame.show(); // So we pack it into an EventBox which has a window for handling events.

    // DND related code from here

    DragNDrop dnd; // instantiate a drag and drop class
    *(Widget*)(&eb)>>dnd; // Tell the Widget it will be dragged around
    *(Widget*)(&textViewTo)<<dnd; // Tell the widget it will be the dragging destination

    // Normal GTK code from here

    VBox vBox; // vertical box to hold all of the widgets
    vBox<<BoxIS(false, true, false)<<(Buttons()<<BUTTONLABELSTRUCT("Quit", quit, NULL)); // Add the quit button to the end
    vBox<<BoxIS(true, true, true)<<eb.show()<<textViewTo.show(); // put the two DND widgets into the VBox

    Window topWindow; // the top window
    topWindow<<vBox.show();
    gtk_main();
}



