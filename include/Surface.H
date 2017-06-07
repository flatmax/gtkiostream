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
#ifndef SURFACE_H_
#define SURFACE_H_

#include <gtk/gtk.h>

#if GTK_MAJOR_VERSION>2 // if using GTK3 or more recent, then don't use GdkPixmaps, use cairo surfaces
#undef PIXMAP_ORIG_VER
#endif

#ifndef PIXMAP_ORIG_VER
#include <cairo/cairo.h>
#endif

#include "Widget.H"

class Surface : public Widget {
protected:
#ifdef PIXMAP_ORIG_VER
    GdkPixmap *pixmap; ///< Backing pixmap for drawing area
#else
    cairo_surface_t *surface; ///< The backing surface for the drawing area
#endif

    /** Create a new backing surface of the appropriate size. Also call the configureEvent method of the class instance.
    \param widget The widget receiving the event.
    \param event The event which triggered the configure-event signal
    \param data The instance of Surface which is associated with the widget/event
    \return true to finish processing, false to allow other handlers to process.
    */
    static gboolean configureEventStatic(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
        //cout<<"Surface::configureEventStatic "<<endl;
        Surface *da=static_cast<Surface*>(data);
#if GTK_MAJOR_VERSION==2
        int width=widget->allocation.width, height=widget->allocation.height;
#else
        GtkAllocation widgetAlloc; // get the width and height
        gtk_widget_get_allocation(widget, &widgetAlloc);
        int width=widgetAlloc.width, height=widgetAlloc.height;
#endif
        //cout<<"Surface::configureSurface width, height "<<width<<','<<height<<endl;

#ifdef PIXMAP_ORIG_VER
        // if using a GdkPixmap, then simply paint it.
        da->setPixmap(gdk_pixmap_new(widget->window, widget->allocation.width, widget->allocation.height, -1));
        gdk_draw_rectangle(da->getPixmap(), widget->style->black_gc, TRUE, 0, 0, width, height);
#else
        // set the backing surface
        da->setSurface(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height));
        da->clear();
        // paint the widget
        cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget)); // create the cairo context and create the backing surface
        cairo_set_source_surface(cr, da->getSurface(), 0., 0.);
        cairo_paint(cr);
        cairo_destroy(cr);
#endif
        return da->configureEvent(event);
    }

    /** Overload this method to handle configure-event signals in your classes.
    \param event the event which triggered this configure-event signal.
    \return true to finish executing, false to allow other handlers to execute.
    */
    virtual gboolean configureEvent(GdkEventConfigure *event) {
        return false;
    }

    /** Redraw the screen from the backing surface. Calls an overloadable class method expose.
    \param widget The widget receiving the event.
    \param event The event which triggered the expose-event signal
    \param data The instance of Surface which is associated with the widget/event
    \return true to finish processing, false to allow other handlers to process.
    */
    static gboolean exposeEventStatic(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
        //cout<<"Surface::exposeSurfaceStatic"<<endl;
        Surface *da=static_cast<Surface*>(data);
#ifdef PIXMAP_ORIG_VER
        gdk_draw_drawable(gtk_widget_get_window(widget), widget->style->fg_gc[gtk_widget_get_state (widget)], da->getPixmap(), event->area.x, event->area.y, event->area.x, event->area.y,
                          event->area.width, event->area.height);
#else
        cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget)); // create the cairo context and paint the backing surface
        cairo_set_source_surface (cr, da->getSurface(), 0, 0);
        cairo_paint(cr);
        cairo_destroy(cr);
#endif
        return da->exposeEvent();
    }

    /** allow overloading of the expose event.
    \return true to finish processing, false to allow other handlers to process.
    */
    virtual bool exposeEvent(){
        return FALSE;
    }

#ifdef PIXMAP_ORIG_VER
    /** Method to dereference and set the pixmap to null */
    void destroyPixmap(void){
        if (pixmap){
            g_object_unref(pixmap);
            pixmap=NULL;
        }
    }
#else
    /** Method to dereference and set the surface to null */
    void destroySurface(void){
        //cout<<"Surface::destroySurface"<<endl;
        if (surface){
            cairo_surface_destroy(surface);
            surface=NULL;
        }
    }
#endif

    /** Initialise the member variables.
    \param width the width of the surface
    \param height the height of the surface
    */
    void init(int width, int height){
#ifdef PIXMAP_ORIG_VER
        pixmap=NULL;
#else
        surface=NULL;
#endif
        widget=gtk_drawing_area_new ();
        show();

        g_signal_connect(G_OBJECT(widget), "expose_event", (GCallback) exposeEventStatic, this);
        g_signal_connect(G_OBJECT(widget), "configure_event", (GCallback) configureEventStatic, this);

		setSize(width, height);
    }

    /** Connect a callback function for when the surface is changed.
    \param callback The callback to execute
    \param data The user data to pass to the callback
    */
    void connectDamagedCallback(GCallback callback, void *data){
        g_signal_connect(G_OBJECT(widget), "damage-event", callback, this);
    }

public:

    /** Default constructor of size 200 by 200
    */
    Surface() {
        init(200, 200);
    }
    /** Construct a surface
    \param width the width of the surface
    \param height the height of the surface
    */
    Surface(int width, int height) {
        init(width, height);
    }

            /** Destructor
    */
    virtual ~Surface(void) {
#ifdef PIXMAP_ORIG_VER
        destroyPixmap();
#else
        destroySurface();
#endif
    }

    /** Get the drawing area widget
    \return The drawing area widget
    */
    GtkWidget *getWidget(void){return widget;}

#ifdef PIXMAP_ORIG_VER
    /** Get the backing pixmap
    \return The backing pixmap
    */
    GdkPixmap *getPixmap(void){return pixmap;}

    /** Set the backing pixmap
    \param The new backing pixmap
    */
    void setPixmap(GdkPixmap *pixmap_){
        destroyPixmap();
        pixmap=pixmap_;
    }
#else
    /** Get the backing surface
    \return The backing suraface
    */
    cairo_surface_t *getSurface(void){return surface;}

    /** Set the backing surface
    \param surface_ The new backing surface
    */
    void setSurface(cairo_surface_t *surface_){
        //cout<<"Surface::setSurface"<<endl;
        destroySurface();
        surface=surface_;
    }

    /** Redraw the background to the surface removing all changes.
    */
    void clear(){
        cairo_t *cr = cairo_create(getSurface());
        cairo_set_source_rgb(cr, 0., 0., 0.); // set the background to black
        cairo_paint(cr);
        cairo_destroy(cr);
    }

#endif
};
#endif  //SURFACE_H_
