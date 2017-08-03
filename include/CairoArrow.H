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

#ifndef CAIROARROW_H_
#define CAIROARROW_H_

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <cairo/cairo.h>

/** Given a Cairo context, draw an arrow.
Arrows can be of three types, line arrows, closed head arrows and filled head arrows. The arrows are specified by a base (x,y) and point (x,y).
The arrow head is specified by a fraction of the shaft size. The arrow head width is specified in pixels.
\image html CairoArrowTest.jpg
*/
class CairoArrow {
    float xSide1, xSide2; ///< The arrow head side 1
    float ySide1, ySide2; ///< The arrow head side 2
    float vecX, vecY; ///< The shaft vector transposed to the origin

    /** Given an arrow shaft, the width and ratio of the head, find the arrow head points.
    The ratio is how much as a fraction of the shaft size should be consumed by the arrow head.
    The arrow head side points are derived using the orthogonal unit vector to the shaft vector.
    \param xBase The x arrow shaft base ordinate
    \param yBase The y arrow shaft base co-ordinate
    \param xPoint The x arrow shaft point ordinate
    \param yPoint The y arrow shaft point co-ordinate
    \param headToLengthRatio The fraction of the shaft length to be taken up by the arrow head
    \param width The width of the arrow
    */
    void findArrowHeadPoints(float xBase, float yBase, float xPoint, float yPoint, float headToLengthRatio, float width){
        vecX=xPoint-xBase, vecY=yPoint-yBase; // the vector form the base to the point translated to the origin.
        float shaftLength=sqrt(powf(vecX,2.)+powf(vecY,2.));
        float theta=2*M_PI*.25; // Find the orthogonal unit vector
        float cosTheta=cos(theta), sinTheta=sin(theta);
        float unitX=vecX/shaftLength, unitY=vecY/shaftLength;
        float rotX=unitX*cosTheta-unitY*sinTheta, rotY=unitX*sinTheta+unitY*cosTheta;
        float orthX=rotX*width, orthY=rotY*width;
        xSide1=xPoint-vecX*headToLengthRatio+orthX; // find the arrow head side points based on the orthogonal unit vector to the shaft
        xSide2=xPoint-vecX*headToLengthRatio-orthX;
        ySide1=yPoint-vecY*headToLengthRatio+orthY;
        ySide2=yPoint-vecY*headToLengthRatio-orthY;
    }

public:
    /** This constructor does nothing.
    Allows instantiation and usage like so :
    \code
    CairoArrow ca;
    ca.draw(cr, 200, 200, 250, 350, .2, 20, 0, 0);
    ca.draw(cr, 400, 400, 300, 200, .2, 20, 1, 0);
    \endcode
    */
    CairoArrow(void) {
    }

    /** Construct an arrow by calculating the head points and then use cairo to stroke or fill.
    The Cairo context which is passed in should already be configured for line width and colour.
    This medthod is good if you want to draw one arrow :
    \code
    CairoArrow ca(cr, 200, 200, 250, 350, .2, 20, 0, 0);
    \endcode
    \param cr The cairo context to draw in
    \param xBase The x arrow shaft base ordinate
    \param yBase The y arrow shaft base co-ordinate
    \param xPoint The x arrow shaft point ordinate
    \param yPoint The y arrow shaft point co-ordinate
    \param headToLengthRatio The fraction of the shaft length to be taken up by the arrow head
    \param width The width of the arrow
    \param closed Close the arrow head (draw a triangle)
    \param filled Fill the arrow head
    */
    CairoArrow(cairo_t  *cr, float xBase, float yBase, float xPoint, float yPoint, float headToLengthRatio, float width, bool closed, bool filled) {
        draw(cr, xBase, yBase, xPoint, yPoint, headToLengthRatio, width, closed, filled);
    }

    /** Construct an arrow by calculating the head points and then use cairo to stroke or fill
    The Cairo context which is passed in should already be configured for line width and colour
    \param cr The cairo context to draw in
    \param xBase The x arrow shaft base ordinate
    \param yBase The y arrow shaft base co-ordinate
    \param xPoint The x arrow shaft point ordinate
    \param yPoint The y arrow shaft point co-ordinate
    \param headToLengthRatio The fraction of the shaft length to be taken up by the arrow head
    \param width The width of the arrow
    \param closed Close the arrow head (draw a triangle)
    \param filled Fill the arrow head
    */
    void draw(cairo_t  *cr, float xBase, float yBase, float xPoint, float yPoint, float headToLengthRatio, float width, bool closed, bool filled) {
        findArrowHeadPoints(xBase, yBase, xPoint, yPoint, headToLengthRatio, width);
        cairo_move_to (cr, xBase, yBase); // start at the base and draw the shaft
        float scale=0.;
        if (closed|filled) // if closed, then just draw up to the arrow head
            scale=headToLengthRatio;
        cairo_line_to (cr, xBase+vecX*(1-scale), yBase+vecY*(1-scale));
        cairo_stroke (cr);
        // now draw the arrow head
        if (filled || closed) // close the path if filled or closed
            cairo_new_sub_path (cr);
        cairo_move_to (cr, xSide1, ySide1); // start at side1 and draw to the point, then side2
        cairo_line_to (cr, xPoint, yPoint);
        cairo_line_to (cr, xSide2, ySide2);
        if (filled || closed) // close the path if filled or closed
            cairo_close_path(cr);
        if (!filled)
            cairo_stroke (cr);
        else
            cairo_fill (cr);
    }

    virtual ~CairoArrow() {
    }
};

#endif // CAIROARROW_H_
