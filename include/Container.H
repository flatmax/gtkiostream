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
#ifndef CONTAINER_H_
#define CONTAINER_H_
#include <gtk/gtk.h>
#include "mffm/LinkList.H"
#include "Widget.H"

/** Container

Base class for those which inherit from the Container

*/
class Container : public Widget {
protected:

    /** Should be overloaded by inheriting classes if it has more sophisticated paching techniques, such as Box
    */
    virtual void pack(GtkWidget *obj){
         gtk_container_add(GTK_CONTAINER(widget),obj);
    }

public:
    /** Set the border size of the container class
    \param size the size of the border
    */
    void setBorder(unsigned int size){
        gtk_container_set_border_width(GTK_CONTAINER(widget), size);
    }

    /** Get the border size of the container class
    \return the size of the border
    */
    unsigned int getBorder(void){
        return gtk_container_get_border_width(GTK_CONTAINER(widget));
    }

        /*! Operator to pack a different container in this container
    \param c The container to add.
    \return Returns a Container reference for further processing.
    */
    Container& operator <<(Container& c) {
        return operator<<(c.getWidget());
    }

    /*! Operator to pack a pointed to container in this container
    \param c A pointer to the container to add.
    \return Returns a Container reference for further processing.
    */
    Container& operator <<(Container * c) {
        return operator<<(c->getWidget());
    }

    /*! Operator to pack a Widget class
    \param w The widget to add.
    \return Returns a Container reference for further processing.
    */
    Container& operator <<(Widget& w) {
        return operator<<(w.getWidget());
    }

    /*! Operator to pack a Widget pointer
    \param w A pointer to the Widget to add.
    \return Returns a Container reference for further processing.
    */
    Container& operator <<(Widget * w) {
        return operator<<(w->getWidget());
    }

    /*! Pack a GtkWidget into the container.
    \param w A pointer to the GtkWidget to pack.
    \return Returns a Container reference for further processing.
    */
    virtual Container& operator <<(GtkWidget * w) {
        pack(w);
        return *this;
    }

    /*! Pack a LinkList of GtkWidget(s) into the container.
    \param ll A LinkList of GtkWidget* for packing from the first to the last
    \return Returns a Container reference for further processing.
    */
    Container& operator <<(LinkList<GtkWidget *> &ll) {
        ll.grab(1);// load using next from the first
        for (int i=1;i<=ll.getCount();i++,ll.next())
            operator<<(ll.current());
        return *this;
    }

    /*! Unpack a widget from the container.
        Note that removal of the widget can destroy it if the only reference is the container.
    \param w The widget to be unpacked from the container.
    \return Returns a Container reference for further processing.
    */
    Container& operator >>(GtkWidget * w) {
        gtk_container_remove(GTK_CONTAINER(widget), w);
        return *this;
    }

    /*! Unpack a container pointer from the container.
        Note that removal of the widget can destroy it if the only reference is the container.
    \param c The pointer to the container to be unpacked from the container.
    \return Returns a Container reference for further processing.
    */
    Container& operator >>(Container * c) {
        gtk_container_remove(GTK_CONTAINER(widget), c->getWidget());
        return *this;
    }

    /*! Unpack a container from the container.
        Note that removal of the widget can destroy it if the only reference is the container.
    \param c The pointer to the container to be unpacked from the container.
    \return Returns a Container reference for further processing.
    */
    Container& operator >>(Container &c) {
        gtk_container_remove(GTK_CONTAINER(widget), c.getWidget());
        return *this;
    }

    /*! Unpack a LinkList of GtkWidget(s) from the container.
    \param ll A LinkList of GtkWidget* for un-packing from the first to the last
    \return Returns a Container reference for further processing.
    */
    Container& operator >>(LinkList<GtkWidget *> &ll) {
        for (int i=1; i<=ll.getCount(); i++)
            (*this)>>ll.next();
        return *this;
    }

//    Container& operator=(const Container b){
//        widget=b.getWidget();
//    }

    /** Sets the sensitivity of the containers childeren.
    \param sens The sensitivity to set.
    */
    void setChildrensSensitivity(bool sens) {
        GList *widgetList=g_list_first(gtk_container_get_children(GTK_CONTAINER(widget)));
        GtkWidget *widget;
        for (int i=0; i<=(int)g_list_length(widgetList); i++) {
            widget=static_cast<GtkWidget*>(g_list_nth_data(widgetList,i));
            gtk_widget_set_sensitive(widget,sens);
        }
    }


    /** Finds if any of the container's children are sensitive.
    \return true if any of the container's children are sensitive, or false otherwise.
    */
    bool areAnyChildrensSensitive(void) {
        GList *widgetList=g_list_first(gtk_container_get_children(GTK_CONTAINER(widget)));
        GtkWidget *widget;
        for (int i=0; i<=(int)g_list_length(widgetList); i++) {
            widget=static_cast<GtkWidget*>(g_list_nth_data(widgetList,i));
            if (gtk_widget_get_sensitive(widget))
                return true;
        }
        return false;
    }

    /** Find out how many children are contained.
    \return the number of children contained.
    */
    unsigned int size(){
        return g_list_length(gtk_container_get_children(GTK_CONTAINER(widget)));
    }
};
#endif // CONTAINER_H_
