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
#ifndef GTKDIALOG_H_
#define GTKDIALOG_H_

#include "Window.H"

//for gtk 2.0 transition
#define GTK_WINDOW_DIALOG GTK_WINDOW_TOPLEVEL

class Gtkdialog {
public:
  GtkWidget *win;
  Gtkdialog(void){
#ifdef DEBUG
    cout<<"Gtkdialog: init "<<this<<endl;
#endif
    //int argc=0;
    win = gtk_window_new( GTK_WINDOW_DIALOG );
    //gtk_widget_ref( win );
    g_signal_connect(G_OBJECT (win), "delete_event", reinterpret_cast<GCallback> (quit), NULL );
    gtk_container_set_border_width( GTK_CONTAINER (win), 10 );
    gtk_widget_show( win );
  }

  ~Gtkdialog(void){
#ifdef DEBUG
    cout<<"Gtkdialog::~Gtkdialog "<<endl;
#endif
    gtk_widget_hide( win );
  }

  static gint quit(GtkWidget *widget, GdkEvent *event, gpointer data){
    gtk_main_quit();
    return 0;
  }

  Gtkdialog& operator <<(Box& b) {
    gtk_container_add (GTK_CONTAINER (win), b.getWidget());
    return *this;
  }
  Gtkdialog& operator <<(Box * b) {
    gtk_container_add (GTK_CONTAINER (win), b->getWidget());
    return *this;
  }
  Gtkdialog& operator <<(void * w) {
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET(w));
    return *this;
  }
  Gtkdialog& operator >>(void * w) {
    gtk_container_remove (GTK_CONTAINER (win), GTK_WIDGET(w));
    return *this;
  }
};
#endif
