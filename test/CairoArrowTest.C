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
#include "Box.H"
#include "Surface.H"
#include "Box.H"

#include "CairoArrow.H"

static void quit(void *wid, gpointer data){
  gtk_main_quit();
}

static void drawArrow(void *wid, gpointer data){
    // create a cairo context from the pixmap and setup the line width/colour
#ifdef PIXMAP_ORIG_VER
    Pixmap *surface=static_cast<Pixmap *>(data);
    cairo_t *cr=gdk_cairo_create(surface->getPixmap());
#else
    Surface *surface=static_cast<Surface *>(data);
    cairo_t *cr=cairo_create(surface->getSurface());
#endif
    cairo_set_source_rgb(cr, 1.,1.,1.);
    cairo_set_line_width (cr, 3.);

    // now draw three arrows - one of each type
    CairoArrow ca;
    ca.draw(cr, 10, 150, 550, 100, .2, 20, 0, 1);
    ca.draw(cr, 250, 380, 200, 200, .2, 20, 0, 0);
    ca.draw(cr, 550, 200, 350, 350, .2, 20, 1, 0);

    // destroy the cairo context and redraw the widget
    cairo_destroy(cr);
    gtk_widget_queue_draw(surface->getWidget());
}

int main(int argc, char *argv[]){

    gtk_init( &argc, &argv );

    VBox vBox;
    Buttons buttons;

#ifdef PIXMAP_ORIG_VER
    Pixmap surface(600,400);
#else
    Surface surface(600,400);
#endif
    buttons<<BUTTONLABELSTRUCT("Quit", quit, &buttons)<<BUTTONLABELSTRUCT("draw arrow", drawArrow, &surface);

    Window topWindow(640,480);
    topWindow<<(VBox()<<surface.getWidget()<<buttons).show();
    gtk_main();
}
