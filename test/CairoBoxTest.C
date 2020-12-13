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
#include "Box.H"
#include "Surface.H"

#include "CairoBox.H"
#include "ColourLineSpec.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

static void drawBoxes(void *wid, gpointer data){
    // create a cairo context from the surface and setup the line width/colour
    Surface *surface=static_cast<Surface *>(data);
    cairo_t *cr=cairo_create(surface->getSurface());
    cairo_set_source_rgb(cr, 1.,1.,1.);
    cairo_set_line_width (cr, 3.);

    // now draw three box - one of each type
    CairoBox cb;
    cb.draw(cr, 0., 0., 100., 100., false);
    cb.draw(cr, 120., 120., 100., 100., true);

    // create a green box
    ColourLineSpec cls("g4");
    cls>>cr;
    cb.draw(cr, 130., 130., 10., 10., false);

    // destroy the cairo context and redraw the widget
    cairo_destroy(cr);
    gtk_widget_queue_draw(surface->getWidget());
}

int main(int argc, char *argv[]){

    gtk_init( &argc, &argv );

    VBox vBox;
    Buttons buttons;

    Surface surface(250,250);

    buttons<<BUTTONLABELSTRUCT("Quit", quit, &buttons)<<BUTTONLABELSTRUCT("draw boxes", drawBoxes, &surface);

    Window topWindow(250,350);
    topWindow<<(VBox()<<surface.getWidget()<<buttons).show();
    gtk_main();
}

