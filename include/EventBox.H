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
#ifndef EVENTBOX_H_
#define EVENTBOX_H_

#include <gtk/gtk.h>
#include "Box.H"

class EventBox : public Box {
    /** The function called to pack GTK widgets into the event box.
        Overloads Box::pack
      \param obj The GtkWidget to add to the container
    */
    virtual void pack(GtkWidget *obj) {
        gtk_container_add(GTK_CONTAINER(widget), obj);
    }

public:
    /** Constructor
    defaults to children below with a visible window
    */
    EventBox(void) {
        widget=gtk_event_box_new();
        setAboveChild(false);
        setVisibleWindow(true);
    }

    /** set the background colour of the top window
    \param colour The GdkColour to change the background to
    \param state The colour for a particular state. Defaults to GTK_STATE_NORMAL
    */
    void setBackgroundColour(GdkColor *colour, GtkStateType state=GTK_STATE_NORMAL) {
        gtk_widget_modify_bg(widget, state, colour);
    }

    /** set the background colour of the top window
    \param colourName The name of the colour to use. e.g. "white"
    \param state The colour for a particular state. Defaults to GTK_STATE_NORMAL
    */
    void setBackgroundColour(const char* colourName, GtkStateType state=GTK_STATE_NORMAL) {
        GdkColor colour;
        gdk_color_parse (colourName, &colour);
        setBackgroundColour(&colour, state);
    }

    /** Whether to set the event box above it's children
    \param aboveChild if true, then set above the children, false will set it below.
    */
    void setAboveChild(bool aboveChild){
        gtk_event_box_set_above_child(GTK_EVENT_BOX(widget),aboveChild);
    }

    /** Whether the event box is visible (can change background and so on)
    \param visible true to show, false to hide
    */
    void setVisibleWindow(bool visible){
        gtk_event_box_set_visible_window(GTK_EVENT_BOX(widget),visible);
    }
};
#endif
