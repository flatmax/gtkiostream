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
g++ -ggdb `gtk-config --cflags` ButtonsTest.C -o ButtonsTest `gtk-config --libs`
`pkg-config --cflags --libs gtk+-2.0`
*/
#include "Window.H"
#include "Buttons.H"
#include "xpm/play.xpm"
#include "Box.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

static void toggleButton(void *widget, gpointer data){
  cout<<"ToggleButton "<<gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))<<endl;
}
static void playButton(void *widget, gpointer data){
  cout<<"playButton "<<endl;
}

int main(int argc, char *argv[]){

  gtk_init( &argc, &argv );

  Window topWindow;
  Buttons buttons;
  HBox hBox;

  buttons<<BUTTONLABELSTRUCT("Quit", quit, &buttons)<<BUTTONTOGGLESTRUCT("TOGGLE",toggleButton, 0) // normal and toggle buttons
         <<BUTTONPIXMAPSTRUCTTRANSPARENT(playXPM, (void (*)(void*, void*))reinterpret_cast<GCallback>(playButton), 0) // transparent pixmap button
         <<BUTTONPIXMAPSTRUCT(playXPM, (void (*)(void*, void*))reinterpret_cast<GCallback>(playButton), 0); // regular buttons are the same

  hBox<<buttons;   // Same as  hBox<< buttons.grab(1)<< buttons.grab(2)<< buttons.grab(3)<< buttons.grab(4);
  hBox.show();
  topWindow<< hBox;

  gtk_main();
}
