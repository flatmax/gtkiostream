/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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
#ifndef GTKINTERFACE_H_
#define GTKINTERFACE_H_

#include "Window.H"

/** GtkInterface has been replaced by Window
*/
class GtkInterface : public Window {
public:
    /** \brief constructor specifying window size and resideable options
    \param width the width of the window
    \param height the height of the window
    \param resiseable whether the window can be resised.
    */
    GtkInterface(int width, int height, bool resiseable=true) : Window(width, height, resiseable) {  }

    /** Empty constructor.
    */
    GtkInterface() : Window() {}
};

#endif
