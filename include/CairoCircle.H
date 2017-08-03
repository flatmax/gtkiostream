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
#ifndef CAIROCIRCLE_H_
#define CAIROCIRCLE_H_

#include "CairoArc.H"

#define CAIROCIRCLE_DEFAULT_THETASTART 0. ///< The default start angle
#define CAIROCIRCLE_DEFAULT_THETAEND 2.*M_PI ///< The default start angle

/** Given a Cairo context, draw a circle.
The circle is specified by a central point (x, y) the radius and whether it is filled or not.
Assumes that the Cairo context is already configured for line width and colour.
\image html CairoCircle.test.png
*/
class CairoCircle : public CairoArc {
    /// Initialiser for the constructors which sets the default constant angles for an arc which forms a circle
    void init(){
        thetaStart=CAIROCIRCLE_DEFAULT_THETASTART;
        thetaEnd=CAIROCIRCLE_DEFAULT_THETAEND; // the constant circle angles
    }
public:
    /// empty constructor
    CairoCircle() {
        init();
    }

    /** Constructor which draws an arc at a particular location.
    This constructor also stores the various member variables for radius, angle and fill state.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    \param bool fillIn Whether the fill or not.
    */
    CairoCircle(cairo_t  *cr, double x, double y, double radiusIn, bool fillIn) : CairoArc(cr, x, y, radiusIn, CAIROCIRCLE_DEFAULT_THETASTART, CAIROCIRCLE_DEFAULT_THETAEND, fillIn) {
        init();
    }

    virtual ~CairoCircle() {}

    /** Change the drawing settings.
    \param radiusIn The radius to draw the arc to.
    \param bool fillIn Whether the fill or not.
    */
    void settings(double radiusIn, bool fillIn) {
        radius=radiusIn;
        filled=fillIn;
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param bool fillIn Whether the fill or not.
    */
    void draw(cairo_t  *cr, double x, double y, bool fillIn) {
        CairoArc::draw(cr, x, y, radius, thetaStart, thetaEnd, fillIn);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    */
    void draw(cairo_t  *cr, double x, double y, double radiusIn) {
        CairoArc::draw(cr, x, y, radiusIn, thetaStart, thetaEnd, filled);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    \param bool fillIn Whether the fill or not.
    */
    void draw(cairo_t  *cr, double x, double y, double radiusIn, bool fillIn) {
        CairoArc::draw(cr, x, y, radiusIn, thetaStart, thetaEnd, fillIn);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    */
    void draw(cairo_t  *cr, double x, double y) {
        CairoArc::draw(cr, x, y, radius, thetaStart, thetaEnd, filled);
    }
};

#endif // CAIROCIRCLE_H_
