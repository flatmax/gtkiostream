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
#ifndef DRAWINGAREA_H_
#define DRAWINGAREA_H_

#include <gtk/gtk.h>
#include "Surface.H"

/** DrawingArea

This class allows the user to draw within a window by moving the mouse.
It uses a backing pixmap to store drawing strokes.
\image html DrawingAreaTest.jpg
*/
class DrawingArea : public Surface {

    /** Handler for when the button is pressed
        \param widget Which widget the button is pressed in.
        \param event The event to handle
        \param data The user data which is a pointer to this class.
        \return TRUE to continue processing the event
    */
    static gint buttonPressEvent(GtkWidget *widget, GdkEventButton *event, gpointer data) {
        DrawingArea *da=static_cast<DrawingArea*>(data);
        if (event->button == 1 && da->getSurface() != NULL)
            da->draw(widget, event->x, event->y);
        if (event->button == 3 && da->getSurface() != NULL)
            da->erase(widget, event->x, event->y);
        return FALSE;
    }

    /**
    */
    static gint motionNotifyEvent(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
        DrawingArea *da=static_cast<DrawingArea*>(data);
        int x, y;
        GdkModifierType state;

        if (event->is_hint)
            gdk_window_get_pointer (event->window, &x, &y, &state);
        else {
            x = event->x;
            y = event->y;
            state = (GdkModifierType)event->state;
        }

        if (state & GDK_BUTTON1_MASK && da->getSurface() != NULL)
            da->draw (widget, x, y);
        else if (state & GDK_BUTTON3_MASK && da->getSurface() != NULL)
            da->erase (widget, x, y);
        else if (da->getSurface() != NULL)
            da->move (widget, x, y);
        return TRUE;
    }

    /** Notifies when the mouse has left the widget.
    */
    static gint leaveEventStatic(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
        DrawingArea *da=static_cast<DrawingArea*>(data);
        da->leaveEvent();
        return TRUE;
    }


    /** Create a valid drawing area of particular size
    */
    void init(void){
        connect("motion_notify_event", G_CALLBACK(motionNotifyEvent), this);
        connectDrawEvent(G_CALLBACK(buttonPressEvent), this);
        connect("leave_notify_event", G_CALLBACK(leaveEventStatic), this);
        gtk_widget_set_events (widget, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);
    }

    /** Move the brush.
    Moves the brush without drawing.
    \param widget The widget to move the brush on.
    \param x The ordinate to draw to.
    \param y The coordinate to draw to.
    */
    virtual void move(GtkWidget *widget, gdouble x, gdouble y) {
        // does nothing.
    }

    /** The mouse has left.
    */
    virtual void leaveEvent() {
		//cout<<"DrawingArea::leaveEvent"<<endl;
        // does nothing.
    }

    /** Draw a rectangle on the screen
    \param widget The widget to draw on.
    \param x The ordinate to draw to.
    \param y The coordinate to draw to.
    */
    virtual void draw(GtkWidget *widget, gdouble x, gdouble y) {
        GdkRectangle update_rect;

        update_rect.x = x - 5;
        update_rect.y = y - 5;
        update_rect.width = 10;
        update_rect.height = 10;
        cairo_t *cr=cairo_create(surface);
        cairo_set_source_rgb(cr, 1., 1., 1.); // set the rectangle to white
        cairo_rectangle(cr, update_rect.x, update_rect.y, update_rect.width, update_rect.height); // draw the rectangle on the surface
        cairo_fill(cr);
        cairo_destroy(cr);
        gtk_widget_queue_draw_area (widget, update_rect.x, update_rect.y, update_rect.width, update_rect.height);
//        cout<<"cairo_surface_status(surface) " <<cairo_surface_status(surface)<<endl;
//        cout<<"width = "<<cairo_image_surface_get_width(surface)<<endl;
//        cout<<"height = "<<cairo_image_surface_get_height(surface)<<endl;
    }

    /** Erase a rectangle on the screen
    \param widget The widget to draw on.
    \param x The ordinate to draw to.
    \param y The coordinate to draw to.
    */
    virtual void erase(GtkWidget *widget, gdouble x, gdouble y) {
        GdkRectangle update_rect;

        update_rect.x = x - 5;
        update_rect.y = y - 5;
        update_rect.width = 10;
        update_rect.height = 10;
        cairo_t *cr=cairo_create(surface);
        cairo_set_source_rgb(cr, 0., 0., 0.); // set the rectangle to white
        cairo_rectangle(cr, update_rect.x, update_rect.y, update_rect.width, update_rect.height); // draw the rectangle on the surface
        cairo_fill(cr);
        cairo_destroy(cr);
        gtk_widget_queue_draw_area (widget, update_rect.x, update_rect.y, update_rect.width, update_rect.height);
    }
public:

    /** Constructor
    Create a drawing of default size (200 by 200)
    */
    DrawingArea(void) : Surface() {
        init();
    }

    /** Constructor
    Create a drawing area of particular size
    */
    DrawingArea(int width, int height) : Surface(width, height) {
        init();
    }

    /** Destructor
    */
    virtual ~DrawingArea(void) {
    }

    void connectDrawEvent(GCallback callBack, gpointer data){
        connectAfter("button_press_event", G_CALLBACK(callBack), data);
    }
};
#endif // DRAWINGAREA_H_
