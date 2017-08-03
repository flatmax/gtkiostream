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
/* Compile with ...
g++ -ggdb `gtk-config --cflags` LabelsTest.C -o LabelsTest `gtk-config --libs`
*/
#include "Window.H"
#include "Buttons.H"
#include "Labels.H"
#include "Box.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

int main(int argc, char *argv[]){

  gtk_init( &argc, &argv ); // init GTK

  Window topWindow; // Create the top box

  Buttons buttons; // create a quit button
  Labels labs; // create a labels list
  HBox hBox; // create an HBox
  VBox vBox; // create a VBox

  // create a quit buttons and add two labels to a VBox
  // At the same time, add them to the HBox
  hBox << (vBox << (labs<<" this is 2"<<" this is 1")) // create and add the labels to the VBox to the HBox
       << (buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL)); // create and add the button to the HBox

  vBox.show(); // show the VBox
  hBox.show(); // show the HBox
  topWindow << hBox; // Load the HBox into the top window

  gtk_main(); // run GTK+
}
