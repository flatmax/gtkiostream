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
#include "Selection.H"
#include "Box.H"

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

static void selectionCallback(void *wid, gpointer data){
    char text[128];
    Selection *s=(Selection *)data;
    s->getSelection(text);
    std::cout<<"you selected : "<<text<<std::endl;
    string textS;
    s->getSelection(textS);
    std::cout<<"you selected : "<<textS<<std::endl;
}

int main(int argc, char *argv[]) {
    gtk_init( &argc, &argv );

    Window topWindow;
    HBox hBox;

    Selection selection("test list items",G_CALLBACK(selectionCallback));
    selection<<"test"<<"this"<<"one";

    selection.show();

    hBox<<BoxIS(TRUE, TRUE)<<selection.getWidget(); // the 'getWidget' method is present in most classses - in future, Selection will find a way to get rid of the 'getWidget' requirement.
    hBox.show();
    topWindow<< hBox;

    gtk_main();
}
