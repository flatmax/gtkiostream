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

#ifndef SCROLLING_H_
#define SCROLLING_H_

#include <gtk/gtk.h>
#include "Selection.H"
#include "Container.H"

/** \brief Scrolling container
*/
class Scrolling : public Container {
    /** Should be overloaded by inheriting classes if it has more sophisticated paching techniques, such as Box
    */
    virtual void pack(GtkWidget *obj){
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(widget), obj);
    }
public:

    /** Construct a Scrolling widget
    */
    Scrolling(void) {
        widget = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widget), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    }
};

/** \brief Scrolling container for use with Selection classes
*/
class ScrollingSelection : public Scrolling {
public:
    /** \brief add a selection object to the scrolling window
    \param selection the selection widget to scroll
    \return This Scrolling object
    */
    ScrollingSelection & operator<<(Selection *selection) {
        gtk_container_add(GTK_CONTAINER(widget), selection->getWidget());
        /* Set vertical adjustment for the tree view */
        GtkAdjustment *vscroll = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(widget));
        gtk_tree_view_set_vadjustment(GTK_TREE_VIEW (selection->getWidget()), vscroll);
        /* Set vertical adjustment for the tree view */
        GtkAdjustment *hscroll = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(widget));
        gtk_tree_view_set_hadjustment(GTK_TREE_VIEW (selection->getWidget()), hscroll);
        return *this;
    }
};

#endif //SCROLLING_H_
