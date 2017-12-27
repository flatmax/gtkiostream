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
#ifndef FRAME_H_
#define FRAME_H_

#include "Container.H"

/** Frame

Simple Frame widget.

*/
class Frame : public Container {
public:
    // Empty constructor
    Frame(void){
        widget=gtk_frame_new("No label"); // create the frame with the label
    }

    /** Constructor
    */
    Frame(const char *label){
        widget=gtk_frame_new(label); // create the frame with the label
    }

    /** Set the label of the frame post creation.
    \param label The label to set.
    */
    void setLabel(string label){
        gtk_frame_set_label(GTK_FRAME(widget), label.c_str());
    }

    /** Get the label of the frame.
    \return The label
    */
    string getLabel(void){
        return gtk_frame_get_label(GTK_FRAME(widget));
    }
};
#endif // FRAME_H_
