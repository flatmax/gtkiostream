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
#ifndef SCALE_H_
#define SCALE_H_

#include "Widget.H"
#include <mffm/LinkList.H>
#include <stdlib.h>
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <limits>

#ifdef _MSC_VER
#ifndef MS_ROUND
/// Microsoft doesn't have a round function
double round(double number) {
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}
#else
extern double round(double);
#endif
#endif

#ifndef SCALE_MAX_DISPLAY_MAX_PRECISION
#define SCALE_MAX_DISPLAY_MAX_PRECISION 37 ///< the maximum precision we want to show
#endif

/** The scale class which defines all types of sliding scales.
*/
class ScaleFuncData {
public:
    /** Define a sliding scale for both horizontal and vertical scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    \param fn The callback when changed.
    \param da The user data to pass to the callback function fn.
    */
    ScaleFuncData(double mi, double ma, double st, void (*fn)(void *, void *), gpointer da) {
        min=mi;
        max=ma;
        step=st;
        func=fn;
        data=da;
    }
    /** Define a sliding scale for both horizontal and vertical scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    \param fn The callback when changed.
    \param da The user data to pass to the callback function fn.
    */
    ScaleFuncData(double mi, double ma, double st, GCallback fn, gpointer da) {
        min=mi;
        max=ma;
        step=st;
        func=(void (*)(void*, void*))fn;
        data=da;
    }

    double min; ///< The minimum value
    double max; ///< The maximum value
    double step; ///< The stepping value
    void (*func)(void *arg1, void *arg2); ///< The callback function
    gpointer data; ///< The user data to pass to the callback function
};
#define SCALESTRUCT(mi,ma,st,fn,da) ScaleFuncData(mi,ma,st,fn,da)

/** Class defining a horizontal scale.
*/
class HScaleFuncData : public ScaleFuncData {
public:
    /** Define a sliding scale for both horizontal scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    */
    HScaleFuncData(double mi, double ma, double st) : ScaleFuncData(mi, ma, st, (GCallback)NULL, NULL) {}
    /** Define a sliding scale for both horizontal scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    \param fn The callback when changed.
    \param da The user data to pass to the callback function fn.
    */
    HScaleFuncData(double mi, double ma, double st, void (*fn)(void *, void *),gpointer da) : ScaleFuncData(mi, ma, st, fn, da) {}
    /** Define a sliding scale for both horizontal scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    \param fn The callback when changed.
    \param da The user data to pass to the callback function fn.
    */
    HScaleFuncData(double mi, double ma, double st, GCallback fn, gpointer da) : ScaleFuncData(mi, ma, st, fn, da) {}
};

/** Class defining a vertical scale.
*/
class VScaleFuncData : public ScaleFuncData {
public:
    /** Define a sliding scale for vertical scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    */
    VScaleFuncData(double mi, double ma, double st) : ScaleFuncData(mi, ma, st, (GCallback)NULL, NULL) {}
    /** Define a sliding scale for vertical scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    \param fn The callback when changed.
    \param da The user data to pass to the callback function fn.
    */
    VScaleFuncData(double mi, double ma, double st, void (*fn)(void *, void *),gpointer da) : ScaleFuncData(mi, ma, st, fn, da) {}
    /** Define a sliding scale for vertical scales
    \param mi The minimum value.
    \param ma The maximum value.
    \param st The stepping value.
    \param fn The callback when changed.
    \param da The user data to pass to the callback function fn.
    */
    VScaleFuncData(double mi, double ma, double st, GCallback fn, gpointer da) : ScaleFuncData(mi, ma, st, fn, da) {}
};

/** Base class for the HScale and VScale classes.

This class operates by using the "<<" operator to load in the VScaleFuncData and HScaleFuncData classes, like so :
\code
    Scales scales;
    scales<<HScaleFuncData(0.,100.,1.,scaleTouch,NULL)<<VScaleFuncData(0.,100.,1.,scaleTouch,NULL);
\endcode

The scales can be set by locating the desired scale widget to set and then using the "=" operator :
\code
    scales.grab(1);
    scales=50.9;
    scales.grab(2);
    scales=50.9;
\endcode

\image html ScaleTest.png
*/
class Scales : public LinkList<GtkWidget *> {

    /** Auto-set the number of decimal points to display in the slider
    \param step The slider stepping factor between movements.
    */
    void setDigits(double step) {
        step=fabs(step)-floor(fabs(step))/10.; // find the remainder mod 1.
        int digits=1; // start with no digits
        if (step!=0.) // if the remainder is < 0. then we need more digits.
            while ((step*=10.)<1.) // keep adding digits till we get above 1. and that is the number we need
                digits++;
        if (digits>SCALE_MAX_DISPLAY_MAX_PRECISION) digits=SCALE_MAX_DISPLAY_MAX_PRECISION; // ensure we don't show too much
        gtk_scale_set_digits(GTK_SCALE(current()),digits);
    }

    /** Perform the quantisation of the widget
    \param wid The widget which caused this callback.
    \param data The user data associated with that button press.
    */
    static void quantiseVal(void *wid, void *data) {
        GtkAdjustment *adj=gtk_range_get_adjustment(GTK_RANGE(wid));
#if GTK_MAJOR_VERSION!=2
            double step_size=gtk_adjustment_get_step_increment(adj);
#else
            double step_size=adj->step_increment;
#endif
        int rounded=(int)round(gtk_range_get_value(GTK_RANGE(wid))/step_size);
        gtk_range_set_value(GTK_RANGE(wid),step_size*(double)rounded);
    }

public:
    /// destructor
    virtual ~Scales(void) {
    }

    /** Get the current value of the provided scale widget.
    \param wid The widget to get the position from
    \return the value the scale is set to.
    */
    static double getVal(GtkRange * wid) {
        return gtk_range_get_value(wid);
    }

    /** Get the current value of the scale widget.
    \return the value the scale is set to. If there are no scales, then return NaN
    */
    double getVal(void) {
        if (getCount())
            return gtk_range_get_value((GtkRange*)current());
		return std::numeric_limits<double>::quiet_NaN();
    }

    /** Get the stepping value of the scale widget.
    \return the stepping value of the scale. If there are no scales, then return NaN
    */
    double getStep(void) {
        if (getCount()) {
            GtkAdjustment* gtkAdj=gtk_range_get_adjustment(GTK_RANGE(current()));
#if GTK_MAJOR_VERSION!=2
            return gtk_adjustment_get_step_increment(gtkAdj);
#else
            return gtkAdj->step_increment;
#endif
        }
		return std::numeric_limits<double>::quiet_NaN();
    }

    /** Set the stepping value of the scale widget.
    \param step the stepping value of the scale.
    */
    void setStep(double step) {
        if (getCount()) {
            GtkAdjustment* gtkAdj=gtk_range_get_adjustment(GTK_RANGE(current()));
#if GTK_MAJOR_VERSION!=2
            return gtk_adjustment_set_step_increment(gtkAdj, step);
#else
            gtkAdj->step_increment=step;
#endif
			gtk_range_set_adjustment(GTK_RANGE(current()), gtkAdj);
        }
    }

	/** Set the lower range value.
	\param lower The new lower value.
	*/
	void setLower(double lower){
        if (getCount()) {
            GtkAdjustment* gtkAdj=gtk_range_get_adjustment(GTK_RANGE(current()));
#if GTK_MAJOR_VERSION!=2
			 gtk_adjustment_set_lower(gtkAdj, lower);
#else
            gtkAdj->lower=lower;
#endif
			gtk_range_set_adjustment(GTK_RANGE(current()), gtkAdj);
        }
	}


	/** Set the upper range value.
	\param upper The new upper value.
	*/
	void setUpper(double upper){
        if (getCount()) {
            GtkAdjustment* gtkAdj=gtk_range_get_adjustment(GTK_RANGE(current()));
#if GTK_MAJOR_VERSION!=2
			 gtk_adjustment_set_upper(gtkAdj, upper);
#else
            gtkAdj->upper=upper;
#endif
			gtk_range_set_adjustment(GTK_RANGE(current()), gtkAdj);
        }
	}

	/** Get the lower range value.
	\return The current lower value.
	*/
	double getLower(void){
        if (getCount()) {
            GtkAdjustment* gtkAdj=gtk_range_get_adjustment(GTK_RANGE(current()));
#if GTK_MAJOR_VERSION!=2
			 return gtk_adjustment_get_lower(gtkAdj);
#else
            return gtkAdj->lower;
#endif
        }
	}

	/** Set the upper range value.
	\return The current upper value.
	*/
	double getUpper(void){
        if (getCount()) {
            GtkAdjustment* gtkAdj=gtk_range_get_adjustment(GTK_RANGE(current()));
#if GTK_MAJOR_VERSION!=2
			 return gtk_adjustment_get_upper(gtkAdj);
#else
            return gtkAdj->upper;
#endif
        }
	}

    /** Set the scale to a particular value.
    \param pos The value to set the scale to.
    \return A reference to this scale class.
    */
    Scales& operator =(double pos) {
        if (getCount())
            gtk_range_set_value((GtkRange*)current(),pos);
        return *this;
    }

    /** Set the scale to a particular value.
    \param pos The value to set the scale to.
    \return A reference to this scale class.
    */
    Scales& operator =(int pos) {
        return this->operator=((double)pos);
    }

    /** Draw the value above the scale widget.
    \return A reference to this scale class.
    */
    Scales &drawValue() {
        if (getCount())
            gtk_scale_set_draw_value((GtkScale*)current(), FALSE);
        return *this;
    }

    /** Create a horizontal scale widget
    \param sf The HScaleFuncData which defines the horizontal scale
    \return A reference to this scale class.
    */
    Scales &operator<<(HScaleFuncData sf) {
        LinkList<GtkWidget *>::add(gtk_hscale_new_with_range(sf.min,sf.max,sf.step));
        setDigits(sf.step);
        gtk_widget_show(current());
        if (sf.func) //set the callback
            g_signal_connect(G_OBJECT((GtkScale*)current()), "value_changed",reinterpret_cast<GCallback>(sf.func), sf.data);
        return *this;
    }

    /** Create a vertical scale widget
    \param sf The VScaleFuncData which defines the vertical scale
    \return A reference to this scale class.
    */
    Scales &operator<<(VScaleFuncData sf) {
        LinkList<GtkWidget *>::add(gtk_vscale_new_with_range(sf.min,sf.max,sf.step));
        setDigits(sf.step);
        gtk_widget_show(current());
        if (sf.func) //set the callback
            g_signal_connect(G_OBJECT((GtkScale*)current()), "value_changed",reinterpret_cast<GCallback>(sf.func), sf.data);
        return *this;
    }

    /** Find the GTK widget index (from 1) in the list of scales
    \param widget The widget to look for
    \return 0 if it isn't on the list, the index (from 1) otherwise
    */
    int findIndex(GtkWidget* widget) {
        int ret=0;
        if (getCount()) {
            GtkWidget *c=current(); // remember our current location
            grab(1);
            prev(); // we are at the last on the list
            for (int i=1; i<=getCount(); i++) {
                if (next()==widget) {
                    ret=i;
                    break;
                }
            }
            while (next()!=c) ; // return to the original button
        }
        return ret;
    }

    /** \brief connect a callback to execute after the currently connected callbacks
    \param sfd ScaleFuncData matching the callback to connect - NOTE: only the ScaleFuncData::func and ScaleFuncData::data fields are used
    */
    void signalConnectAfter(ScaleFuncData sfd) {
        if (sfd.func)
            g_signal_connect_after(G_OBJECT(current()), "value_changed",reinterpret_cast<GCallback>(sfd.func), sfd.data);
    }

    /** \brief disconnect the callback
    \param sfd ScaleFuncData matching the callback to disconnect - NOTE: only the ScaleFuncData::func and ScaleFuncData::data fields are used
    */
    void signalRemove(ScaleFuncData sfd) {
        if (sfd.func) {
            g_signal_handlers_disconnect_by_func(G_OBJECT(current()), (void*)(sfd.func), sfd.data);
        }
    }


    /** Quantise the scale to the step size.
    If the user slides it, then it is quantised to the closest step size increment.
    */
    void quantise(bool isQuantised) {
        if (getCount())
            if (isQuantised)
                g_signal_connect(G_OBJECT(current()), "value_changed",reinterpret_cast<GCallback>(quantiseVal), NULL);
            else
                g_signal_handlers_disconnect_matched(G_OBJECT((GtkButton*)current()), G_SIGNAL_MATCH_FUNC, 0, 0, NULL, reinterpret_cast<gpointer>(quantiseVal), NULL);
    }

    /** Turn the scale upside down (Vertical scales) or flip left/right (Horizontal scales)
    \param isInverted True to invert, False to return to normal transposition
    */
    void invert(bool isInverted) {
        if (getCount())
            gtk_range_set_inverted(GTK_RANGE(current()),isInverted);
    }
};

#endif //SCALE_H_
