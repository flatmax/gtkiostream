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
#ifndef GTKUTILS_H_
#define GTKUTILS_H_
#include <gtk/gtk.h>
#include <iostream>

/** \brief returns the first visible widget in a container
\param container the container who's children are checked
\return The first widget encountered which is visible.
*/
static GtkWidget *getContainersFirstVisibleWidget(GtkContainer *container) {
    GList *box=gtk_container_get_children(container); // get all of the container's contents
    GtkWidget *visible=NULL;
    std::cout<<"getContainersFirstVisibleWidget : g_list_length(box)="<<g_list_length(box)<<std::endl;
    for (int i=0; i<(int)g_list_length(box); i++) {
        visible=static_cast<GtkWidget*>(g_list_nth_data(box,i)); // get the widget
        if (gtk_widget_get_visible(visible)) {
            return visible;
        } else
            visible=NULL;
    }
    return visible;
}

/** returns the first widget in a container
\param widget the widget who's children are checked
\return The first widget encountered
*/
static GtkWidget *getContainersFirstWidget(GtkContainer *widget) {
    GList *box=gtk_container_get_children(widget); // get all of the container's contents
    if (g_list_length(box)>1) // check we don't have more then one label displayed - which we shouldn't
        std::cerr<<"More then one widget in the container - returning only the first"<<std::endl;
    if (g_list_length(box)) // check there is something displayed
        return (GtkWidget *)g_list_nth_data(box,0); // get the label
    else
        return (GtkWidget *)NULL;
}

#endif // GTKUTILS_H_
