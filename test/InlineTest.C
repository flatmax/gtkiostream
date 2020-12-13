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
#include "Window.H"
#include "Buttons.H"
#include "Labels.H"
#include "Box.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

int main(int argc, char *argv[]){
  gtk_init( &argc, &argv ); // init GTK


  // Do all of the GTK+ IOStream in ONE line
  Window() << (VBox() << (Labels()<<"completely inline\n\nClasses destruct but\n\nthe widgets still exist.") // add the label to the HBox
                            << (Buttons()<<BUTTONLABELSTRUCT("Quit", quit, NULL)) ).show(); // add the button and show the HBox

  gtk_main(); // run GTK+
}

