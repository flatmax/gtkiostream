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
#ifndef SEPARATOR_H_
#define SEPARATOR_H_

#include <iomanip>
#include <gtk/gtk.h>
#include <mffm/LinkList.H>
#include <Widget.H>

class Separator : public LinkList<GtkWidget *> {
public:
  ~Separator(void){
    GtkWidget *temp;
    while (getCount()){
      temp=remove();
      Widget(temp).unref();
    }
  }
};

class VSeparator : public Separator {
public:
  VSeparator(void){
    vSepAlloc(1);
  }

  VSeparator(int count){
    vSepAlloc(count);
  }

  void vSepAlloc(int count){
    for (int i=0;i<count;i++){
      add(gtk_vseparator_new());
      Widget w(current());
      w.ref();
      w.show();
    }
  }
};

class HSeparator : public Separator {
public:
  HSeparator(void){
    hSepAlloc(1);
  }

  HSeparator(int count){
    hSepAlloc(count);
  }

  void hSepAlloc(int count){
    for (int i=0;i<count;i++){
      add(gtk_hseparator_new());
      Widget w(current());
      w.ref();
      w.show();
    }
  }
};
#endif //SEPARATOR_H_
