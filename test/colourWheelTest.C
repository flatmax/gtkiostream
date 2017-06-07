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
#include "colourWheel.H"

static void quit(GtkWidget *widget, gpointer data){
  gtk_main_quit();
}

int main(void){
  int argc=0;
  gtk_init( &argc, NULL );

  Window topWindow;

  Buttons buttons; buttons<<BUTTONLABELSTRUCT("Quit", (void (*)(void*, void*))quit, NULL);
  topWindow<<buttons.grab(1);

  ColourWheel wheel;

  gtk_main();
}
