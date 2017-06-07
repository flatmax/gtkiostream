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
  buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL);

  Labels labs; // create a labels list
  labs<<" this is 1"<<" this is 2"; // Load some labels

  HBox hBox; // create an HBox
  hBox << buttons << labs; // load the button and label into the HBox
  hBox.show(); // show the HBox
  topWindow << hBox; // Load the HBox into the top window

  gtk_main(); // run GTK+
}
