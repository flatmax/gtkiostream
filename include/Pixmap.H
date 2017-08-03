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
#ifndef PIXMAP_H_
#define PIXMAP_H_

#include <gtk/gtk.h>

#define PIXMAP_ORIG_VER ///< This to return to the original GdkPixmap approach, which is to be superceeded

#if GTK_MAJOR_VERSION>2 // if using GTK3 or more recent, then don't use GdkPixmaps, use cairo surfaces
#error "GTK verions is > 2, Pixmap has been superceeded by Surface"
#endif

#ifndef _MSC_VER
#warning "Pixmap has been superceeded by Surface and is not present in GTK version 3 and greater."
#else
#pragma message("Pixmap has been superceeded by Surface and is not present in GTK version 3 and greater.")
#endif

#include "Surface.H"

/** This class should not be used in new code, unless it uses old versions of GTK.
Switch to using the Surface class.
*/
class Pixmap : public Surface {
public:
        /** Default constructor of size 200 by 200.
        This has been replaced by Surface.
        */
        Pixmap() : Surface(){
            init(200, 200);
        }

        /** Construct a pixmap.
        This has been replaced by Surface.
        \param width the width of the pixmap
        \param height the height of the pixmap
        */
        Pixmap(int width, int height) : Surface(width, height){
            init(width, height);
        }
};

#endif  //PIXMAP_H_
