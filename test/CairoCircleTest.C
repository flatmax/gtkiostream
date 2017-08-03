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
#include "Buttons.H"
#include "Box.H"
#include "Surface.H"

#include "CairoCircle.H"
#include "ColourLineSpec.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

static void drawCircles(void *wid, gpointer data){
    // create a cairo context from the surface and setup the line width/colour
    Surface *surface=static_cast<Surface *>(data);
    cairo_t *cr=cairo_create(surface->getSurface());
    cairo_set_source_rgb(cr, 1.,1.,1.);
    cairo_set_line_width (cr, 3.);

    // now draw three circle
    CairoCircle cc;
    cc.draw(cr, 10., 10., 10., false);

    // create a green circle
    ColourLineSpec cls("g4");
    cls>>cr;
    cc.draw(cr, 120., 120., 40., true);

    cls="r3";
    cls>>cr;
    cc.draw(cr, 120., 120., 10., false);

    // destroy the cairo context and redraw the widget
    cairo_destroy(cr);
    gtk_widget_queue_draw(surface->getWidget());
}

static void drawArcs(void *wid, gpointer data){
    // create a cairo context from the surface and setup the line width/colour
    Surface *surface=static_cast<Surface *>(data);
    cairo_t *cr=cairo_create(surface->getSurface());
    cairo_set_source_rgb(cr, 1.,1.,1.);
    cairo_set_line_width (cr, 3.);

    // now draw three circle
    CairoArc cc;
    cc.draw(cr, 120., 120., 100., 10./306.*2.*M_PI, 200./360.*2.*M_PI, false);

    // create a green circle
    ColourLineSpec cls("g4");
    cls>>cr;
    cc.draw(cr, 120., 120., 60., 220./306.*2.*M_PI, 360./360.*2.*M_PI, false);

    // destroy the cairo context and redraw the widget
    cairo_destroy(cr);
    gtk_widget_queue_draw(surface->getWidget());
}

int main(int argc, char *argv[]){

    gtk_init( &argc, &argv );

    VBox vBox;
    Buttons buttons;

    Surface surface(250,250);

    buttons<<BUTTONLABELSTRUCT("Quit", quit, &buttons)<<BUTTONLABELSTRUCT("draw circles", drawCircles, &surface)
                                                      <<BUTTONLABELSTRUCT("draw arcs", drawArcs, &surface);

    Window topWindow(250,350);
    topWindow<<(VBox()<<surface.getWidget()<<buttons).show();
    gtk_main();
}


