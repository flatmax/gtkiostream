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
/* Compile with ...
g++ -ggdb `gtk-config --cflags` LabelsTest.C -o LabelsTest `gtk-config --libs`
*/
#include "Window.H"
#include "Buttons.H"
#include "Labels.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

int main(int argc, char *argv[]){

  gtk_init( &argc, &argv ); // init GTK

  Window topWindow; // Create the top box

  Buttons buttons; // create a quit button
  Labels labs; // create a labels holder
  HBox hBox; // create an HBox
  VBox vBox; // create a VBox

  // create a quit buttons and add two labels to a shown VBox
  // At the same time, add them to the HBox and show it
  // At the same time, add it to the top window
  topWindow << (hBox << (vBox << (labs<<" this is 2"<<" this is 1")).show() // show the vbox with labels
                     << (buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL))).show(); // show the hbox with labels and buttons

  gtk_main(); // run GTK+
}


