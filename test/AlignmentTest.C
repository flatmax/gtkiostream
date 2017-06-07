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
/* Compile with ...
g++ -ggdb `gtk-config --cflags` LabelsTest.C -o LabelsTest `gtk-config --libs`
*/
#include "Window.H"
#include "Labels.H"
#include "Box.H"
#include "Buttons.H"
#include "Alignment.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

int main(int argc, char *argv[]){

  gtk_init( &argc, &argv );

  Window topWindow;

  Buttons buttons;
  buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL);

  Labels labs;
  labs<<(const char*)"this label \nis centrally aligned";

  HBox hBox;

  Alignment al(.5,.5,0.,0.);
  al<<labs.grab(1);  al.show();
  hBox<<BoxIS(true,true,true)<<al<<buttons.grab(1);

  hBox.show();
  topWindow<< hBox.getWidget();

  gtk_main();
}
