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
#include "Window.H"
#include "SelectionArea.H"
#include "Buttons.H"
#include "Box.H"

// Quit button callback
static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

// Center the selection area callback
static void center(void *wid, gpointer data) {
    static_cast<SelectionArea*>(data)->centerStaticBox();
}

// Center the selection area callback
static void halveWidths(void *wid, gpointer data) {
	SelectionArea *sa=static_cast<SelectionArea*>(data);
	int width = sa->getStaticBox().getWidth();
    sa->setBoxWidths(width/2);
}

// Center the selection area callback
static void halveHeights(void *wid, gpointer data) {
	SelectionArea *sa=static_cast<SelectionArea*>(data);
	int height = sa->getStaticBox().getHeight();
    sa->setBoxHeights(height/2);
}

// Center the selection area callback
static void halveWidthsHeights(void *wid, gpointer data) {
	SelectionArea *sa=static_cast<SelectionArea*>(data);
	int width = sa->getStaticBox().getWidth();
	int height = sa->getStaticBox().getHeight();
    sa->setBoxsWidthHeight(width/2, height/2);
}

// Keep the aspect ratio
static void keepAspectRatio(void *wid, gpointer data) {
	SelectionArea *sa=static_cast<SelectionArea*>(data);
	sa->keepAspectRatio(true);
}

// Float the aspect ratio
static void keepAspectRatioFalse(void *wid, gpointer data) {
	SelectionArea *sa=static_cast<SelectionArea*>(data);
	sa->keepAspectRatio(false);
}

// Double the surface size
static void doubleSize(void *wid, gpointer data) {
	SelectionArea *sa=static_cast<SelectionArea*>(data);
	int width, height;
	sa->getSize(width, height);
	sa->setSize(width*2, height*2);
}

// Halve the surface size
static void halveSize(void *wid, gpointer data) {
	SelectionArea *sa=static_cast<SelectionArea*>(data);
	int width, height;
	sa->getSize(width, height);
	sa->setSize(width/2, height/2);
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv );

    Window topWindow;

//    gtk_widget_set_size_request (topWindow.win, 1000, 500);

    VBox vBox;

    SelectionArea selectionArea; // The drawing area object

    vBox<<BoxIS(FALSE,FALSE, FALSE)<<(Buttons()<<BUTTONLABELSTRUCT((char*)"Quit", quit, NULL)<<BUTTONLABELSTRUCT((char*)"center", center, &selectionArea)
											   <<BUTTONLABELSTRUCT((char*)"halve widths", halveWidths, &selectionArea)<<BUTTONLABELSTRUCT((char*)"halve heights", halveHeights, &selectionArea)
											   <<BUTTONLABELSTRUCT((char*)"halve widths and heights", halveWidthsHeights, &selectionArea)
											   <<BUTTONLABELSTRUCT((char*)"keep aspect ratio", keepAspectRatio, &selectionArea)
											   <<BUTTONLABELSTRUCT((char*)"don't keep aspect ratio", keepAspectRatioFalse, &selectionArea)
											   <<BUTTONLABELSTRUCT((char*)"double size", doubleSize, &selectionArea)
											   <<BUTTONLABELSTRUCT((char*)"halve size", halveSize, &selectionArea)
									 ); // show a quit button

    vBox << BoxIS(TRUE, TRUE)<<selectionArea.getWidget();  // show the figure
    vBox.show(); // show the vertical box
    topWindow<< vBox; // add it to the top window

    gtk_main();
}
