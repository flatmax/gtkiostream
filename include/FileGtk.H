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
#ifndef FILEGTK_H_
#define FILEGTK_H_

#include <iostream>
#include <gtk/gtk.h>

#define TITLE "Select audio file"

class FileGtk {
  GtkWidget* fileGtk;

  static void store_filename(GtkFileSelection *selector, gpointer data) {
    FileGtk* f=(FileGtk*)data;
    if (f->fileName) delete f->fileName;
    //    f->fileName = gtk_file_selection_get_filename(GTK_FILE_SELECTION(f->fileGtk));
    f->fileName = (gchar*)gtk_file_selection_get_filename(GTK_FILE_SELECTION(f->fileGtk));
    std::cout<<"FileGtk::store_filename: File is now : "<<f->fileName<<std::endl;
  }

  void internalInit(void){
    fileGtk=gtk_file_selection_new(TITLE);
    //gtk_widget_ref(fileGtk);
    //gtk_file_selection_complete(GTK_FILE_SELECTION(fileGtk), fileName);

    g_signal_connect(G_OBJECT (GTK_FILE_SELECTION(fileGtk)->ok_button), "clicked", reinterpret_cast<GCallback> (store_filename), this);
    g_signal_connect(G_OBJECT (GTK_FILE_SELECTION(fileGtk)->ok_button), "clicked", reinterpret_cast<GCallback> (gtk_widget_hide), GTK_OBJECT(fileGtk));
    g_signal_connect(G_OBJECT (GTK_FILE_SELECTION(fileGtk)->cancel_button), "clicked", reinterpret_cast<GCallback> (gtk_widget_hide),GTK_OBJECT(fileGtk));
  }
protected:
  gchar* fileName;
public:
  FileGtk(void){
#ifdef DEBUG
    std::cout<<"FileGtk::FileGtk()"<<std::endl;
#endif
    fileName=NULL;
    internalInit();
  }
  FileGtk(const char* name){
#ifdef DEBUG
    std::cout<<"FileGtk::FileGtk(name)"<<std::endl;
#endif
    fileName=g_strdup(name);
    internalInit();
  }

  ~FileGtk(void){
#ifdef DEBUG
    std::cout<<"FileGtk::~FileGtk()"<<std::endl;
#endif
    if (fileName) delete fileName;
    //gtk_widget_unref(fileGtk);
    //    gtk_widget_destroy(fileGtk);
    gtk_widget_hide(fileGtk);
  }

  void getFileName(void){gtk_widget_show(fileGtk);}
  void setFileName(const char* fn){
    if (fileName) delete fileName;
    fileName=g_strdup(fn);
  }

  void addOKCallback(GtkSignalFunc callB, gpointer data){
    g_signal_connect_after(G_OBJECT(GTK_FILE_SELECTION(fileGtk)->ok_button), "clicked", callB, data);
  }
  void addCancelCallback(GtkSignalFunc callB, gpointer data){
    g_signal_connect_after(G_OBJECT(GTK_FILE_SELECTION(fileGtk)->cancel_button), "clicked", callB, data);
  }
};
#endif //FILEGTK_H_

