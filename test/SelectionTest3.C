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
#include "Selection.H"

#include "xpm/play.xpm"

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

static void selectionCallback(void *wid, gpointer data){
    char text[128];
    Selection *s=(Selection *)data;
    s->getSelection(text);
    std::cout<<"you selected : "<<text<<std::endl;
}

int main(int argc, char *argv[]) {
    gtk_init( &argc, &argv );

    Window topWindow;
    HBox hBox;

    Selection selection; // construct a Selection class
    SelectionColumn sc("text column", 0); // create the columns class with a new column which handles text
    sc<<pair<string*, GtkCellRendererText*>(new string("text"), GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new())); // create a column for text
    //SelectionColumn scp("pixmap col", 1); // create the pixmap column
    sc<<pair<string*, GtkCellRendererPixbuf*>(new string("pixbuf"), GTK_CELL_RENDERER_PIXBUF(gtk_cell_renderer_pixbuf_new())); // create a second column which handles pixbufs
    selection<<&sc; // setup the Selection with the columns
    //selection<<&scp; // setup the Selection with the columns

    selection.setSelectionCallback(G_CALLBACK(selectionCallback)); // add the callback

    GdkPixbuf *testPixBuf = gdk_pixbuf_new_from_xpm_data(static_cast<const char**>(playXPM)); // get the pixbuf

//    selection.grab(1); // get the first column
    selection<<"test col 1"; // add some text to the column
  //  selection.grab(2); // get the second column - the pixbuf column
    selection<<testPixBuf; // add the pix buf to the column

    // add a third row with both text and a pixbuf
    //selection.grab(1); // get the text column
    selection<<"test row 2";
    //selection.grab(2); // get the second pixbuf
    selection.add(testPixBuf, true); // add to the same row

    // or add the pixbuf first and then the text
    //selection.grab(2); // get the second column - the pixbuf column
    selection.add(testPixBuf); // add to the same row
    //selection.grab(1); // get the second column - the pixbuf column
    selection.add("test row 3", true);

    selection.show();

    selection.grab(1); // ensure callbacks are referencing the first column


    hBox<<BoxIS(TRUE, TRUE)<<selection.getWidget(); // the 'getWidget' method is present in most classses - in future, Selection will find a way to get rid of the 'getWidget' requirement.
    hBox.show();
    topWindow<< hBox;

    gtk_main();
}
