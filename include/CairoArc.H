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
#ifndef CAIROARC_H_
#define CAIROARC_H_

#include <cairo/cairo.h>
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#define CAIROARC_DEFAULT_RADIUS 20. ///< The default radius
#define CAIROARC_DEFAULT_FILLED false ///< The default fill value.
#define CAIROARC_DEFAULT_THETASTART 0. ///< The default start angle
#define CAIROARC_DEFAULT_THETAEND M_PI/2. ///< The default ending angle (half circle)

/** Given a Cairo context, draw an arc.
The arc is specified by a central point (x, y) the radius and a starting, ending angle and whether it is filled or not.
Assumes that the Cairo context is already configured for line width and colour.
\image html CairoArc.test.png
*/
class CairoArc {
protected:
    double radius; ///< The radius of the circle.
    double thetaStart, thetaEnd; ///< The angles from start to end
    bool filled; ///< Whether to fill the box
public:
    /// Empty constructor
    CairoArc() {
        radius=CAIROARC_DEFAULT_RADIUS;
        thetaStart=CAIROARC_DEFAULT_THETASTART;
        thetaEnd=CAIROARC_DEFAULT_THETAEND;
        filled=CAIROARC_DEFAULT_FILLED;
    }

    /** Constructor which draws an arc at a particular location.
    This constructor also stores the various member variables for radius, angle and fill state.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    \param angleStart The starting angle
    \param angleEnd The ending angle
    \param bool fillIn Whether the fill or not.
    */
    CairoArc(cairo_t  *cr, double x, double y, double radiusIn, double angleStart, double angleEnd, bool fillIn){
        draw(cr, x, y, radiusIn, angleStart, angleEnd, fillIn);
    }

    virtual ~CairoArc() {}

    /** Change the drawing settings.
    \param radiusIn The radius to draw the arc to.
    \param angleStart The starting angle
    \param angleEnd The ending angle
    \param bool fillIn Whether the fill or not.
    */
    void settings(double radiusIn, double angleStart, double angleEnd, bool fillIn){
        radius=radiusIn;
        thetaStart=angleStart; thetaEnd=angleEnd;
        filled=fillIn;
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param bool fillIn Whether the fill or not.
    */
    void draw(cairo_t  *cr, double x, double y, bool fillIn){
        settings(radius, thetaStart, thetaEnd, fillIn);
        draw(cr, x, y);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    */
    void draw(cairo_t  *cr, double x, double y, double radiusIn){
        settings(radiusIn, thetaStart, thetaEnd, filled);
        draw(cr, x, y);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    \param bool fillIn Whether the fill or not.
    */
    void draw(cairo_t  *cr, double x, double y, double radiusIn, bool fillIn){
        settings(radiusIn, thetaStart, thetaEnd, fillIn);
        draw(cr, x, y);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param angleStart The starting angle
    \param angleEnd The ending angle
    \param bool fillIn Whether the fill or not.
    */
    void draw(cairo_t  *cr, double x, double y, double angleStart, double angleEnd, bool fillIn){
        settings(radius, angleStart, angleEnd, fillIn);
        draw(cr, x, y);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    \param angleStart The starting angle
    \param angleEnd The ending angle
    */
    void draw(cairo_t  *cr, double x, double y, double radiusIn, double angleStart, double angleEnd){
        settings(radiusIn, angleStart, angleEnd, filled);
        draw(cr, x, y);
    }

    /** Draw the arc at a particular location.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    \param radiusIn The radius to draw the arc to.
    \param angleStart The starting angle
    \param angleEnd The ending angle
    \param bool fillIn Whether the fill or not.
    */
    void draw(cairo_t  *cr, double x, double y, double radiusIn, double angleStart, double angleEnd, bool fillIn){
        settings(radiusIn, angleStart, angleEnd, fillIn);
        draw(cr, x, y);
    }

    /** Draw the arc at a particular location.
    \param cr The cairo context to draw to.
    \param x The x location to draw use as the center.
    \param y The y location to draw use as the center.
    */
    void draw(cairo_t  *cr, double x, double y) {
        cairo_move_to(cr, x, y); // start with the center at x, y
        if (!filled)
            cairo_move_to(cr, x+cos(thetaStart)*radius, y+sin(thetaStart)*radius);
        cairo_arc(cr, x, y, radius, thetaStart, thetaEnd); // draw the arc
        if (!filled)
            cairo_stroke (cr);
        else
            cairo_fill (cr);
    }

    /** Find the radius
    \return The radius currently used to draw with.
    */
    double getRadius(){
        return radius;
    }

    /** Find the starting angle
    \return The angle to start drawing from.
    */
    double getAngleStart(){
        return thetaStart;
    }

    /** Find the ending angle
    \return The angle to stop drawing at.
    */
    double getAngleEnd(){
        return thetaEnd;
    }
};

#endif // CAIROARC_H_
