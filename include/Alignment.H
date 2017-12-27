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
#ifndef ALIGNMENT_H_
#define ALIGNMENT_H_

#include <gtk/gtk.h>

#include "Box.H"

/** This class handels the alignment and overloads the Box type
*/
class Alignment : public Box {
    /*! The function called to pack GTK widgets into the Alignment container.
      \param obj The GtkWidget to pack
    */
    virtual void pack(GtkWidget *obj) {
            Container::pack(obj);
    }
public:
    /** Constructor sets the alignment and coverage.
    Uses default values of 0.5 (middle) and 1. (unity scaling).
    */
    Alignment(void) {
        Container::widget=gtk_alignment_new(0.5,0.5,1.,1.);
    }

    /** Constructor sets the alignment and coverage
    \param xalign The horizontal alignment
    \param yalign The vertical alignment
    \param xscale The horizontal scaling in the box - how much the box will fill the space (1.0 for all)
    \param yscale The vertical scaling in the box - how much the box will fill the space (1.0 for all)
    */
    Alignment(gfloat xalign, gfloat yalign, gfloat xscale, gfloat yscale) {
        Container::widget=gtk_alignment_new(xalign, yalign, xscale, yscale);
    }

    /** sets the alignment and coverage
    \param xalign The horizontal alignment
    \param yalign The vertical alignment
    \param xscale The horizontal scaling in the box - how much the box will fill the space (1.0 for all)
    \param yscale The vertical scaling in the box - how much the box will fill the space (1.0 for all)
    */
    void set(gfloat xalign, gfloat yalign, gfloat xscale, gfloat yscale){
        gtk_alignment_set(GTK_ALIGNMENT(widget), xalign, yalign, xscale, yscale);
    }

    /** sets the padding around the container
    \param padding_top
    \param padding_bottom
    \param padding_left
    \param padding_right
    */
    void padding(guint padding_top, guint padding_bottom, guint padding_left, guint padding_right){
        gtk_alignment_set_padding(GTK_ALIGNMENT(widget), padding_top, padding_bottom, padding_left, padding_right);
    }
};
#endif //ALIGNMENT_H_
