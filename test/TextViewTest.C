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
#include "Window.H"
#include "Buttons.H"
#include "TextView.H"
#include "Box.H"

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

static void getText(void *wid, gpointer data) {
    TextView *tv=static_cast<TextView*>(data);
    string text;
    cout<<tv->getText()<<endl;
    (*tv)>>text;
    cout<<text<<endl;
}

static void changedText(void *wid, gpointer data) {
    TextView *tv=static_cast<TextView*>(data);
    string text;
    (*tv)>>text;
    cout<<"New text is = "<<text<<endl;
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv ); // init GTK

    Window topWindow; // Create the top box

    TextView textView;
    textView<<"some text";
    textView.connectBufferChangedSignal((GCallback)changedText, &textView);

    Buttons buttons; // create a quit button
    buttons<<BUTTONLABELSTRUCT((char *)"Quit", quit, NULL)<<BUTTONLABELSTRUCT((char *)"get text from the TextView", getText, &textView);

    topWindow << (HBox()<<buttons<<textView.show()).show(); // Load the HBox into the top window

    gtk_main(); // run GTK+
}
