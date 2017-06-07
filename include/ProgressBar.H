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
#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_

#include <iomanip>
#include <gtk/gtk.h>
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "Widget.H"

/** \brief Class to show progress
This class shows a progress bar which may be initialised at construction or later.
The progress bar ranges between 0.0 and 1.0, negative values cause it to 'pulse'
*/
class ProgressBar : public Widget {
    GtkWidget* widget; ///< The progress bar
    float last;
public:

    /// Constructor
    ProgressBar(void) {
        widget=NULL;
#if DEBUG_PB >1
        cout<<"ProgressBar::ProgressBar()"<<endl;
#endif
        widget=gtk_progress_bar_new();
        //gtk_progress_set_format_string (GTK_PROGRESS (widget), "%v from [%l-%u] (=%p%%)");
        show();
        last=0.;
    }

    /// Destructor
    ~ProgressBar(void) {
#if DEBUG_PB >1
        cout<<"ProgressBar::~ProgressBar()"<<endl;
#endif
        if (widget)
            g_object_unref(widget);
        widget=NULL;
    }

    /** Update the progress bar.
    numbers < 0.0 cause a 'pulsing' progress bar
    numbers >= 0.0 and <= 1.0 set the fractional fill of the progress bar.
    \param what the fractional fill or pulse indicator
    */
    ProgressBar& operator =(float what) {
        if (what>1.0)
            what=1.0;
        if (fabs(what-last)>0.05) {
            if (what<0.0)
                gtk_progress_bar_pulse (GTK_PROGRESS_BAR(widget));
            else
                gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(widget), what);
        last=what;
        }
        return *this;
    }
};
#endif //PROGRESSBAR_H_
