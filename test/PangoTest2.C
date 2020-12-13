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
/* Compile with ...
g++ -ggdb `gtk-config --cflags` PangoTest.C -o PangoTest `gtk-config --libs`
*/
#include "Window.H"
#include "Buttons.H"
#include "Labels.H"
#include "Pango.H"
#include "ColourLineSpec.H"
#include "Box.H"

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv ); // init GTK

    Window topWindow; // Create the top box

    Buttons buttons; // create a quit button
    Labels labs; // create a labels list
    Pango pango; // Create a Pango class for marking up the labels
    HBox hBox; // create an HBox
    VBox vBox; // create a VBox
    vBox.start=true; // ensure packing is from the start


    // try a pango type with only text, no other markup information
    pango<<"text test"; // same label with different colours/sizes

    labs<<pango; // load a formatted label

    vBox << labs; // load the labels into a vertical box
    hBox << vBox // load the vertically stacked labels into the horizontal box
         << (buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL)); // create and add the button to the HBox

    vBox.show(); // show the VBox
    hBox.show(); // show the HBox
    topWindow << hBox; // Load the HBox into the top window

    gtk_main(); // run GTK+
}
