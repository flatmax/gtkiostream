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

#ifndef CAIROBOX_H_
#define CAIROBOX_H_

#include <cairo/cairo.h>

#define CAIROBOX_DEFAULT_START_ORDINATE 0. ///< The default starting ordinate/coordinate for the box.
#define CAIROBOX_DEFAULT_WIDTH_HEIGHT 10. ///< The default width and height for the box.
#define CAIROBOX_DEFAULT_FILLED false ///< The default fill value.

/** Given a Cairo context, draw a box.
The box is specified by a starting point (x, y) a width, height and whether it is filled or not.
Assumes that the Cairo context is already configured for line width and colour.
*/
class CairoBox {
    float x; ///< The base ordinate
    float y; ///< The base coordinate
    float width; ///< The box with
    float height; ///< The box height
    bool filled; ///< Whether to fill the box
public:
    /// Empty constructor - sets default start point and width, height
    CairoBox() {
        setXY(CAIROBOX_DEFAULT_START_ORDINATE, CAIROBOX_DEFAULT_START_ORDINATE);
        width=height=CAIROBOX_DEFAULT_WIDTH_HEIGHT;
        filled=CAIROBOX_DEFAULT_FILLED;
    }

    /** Construct a box given starting point and the width, height and use cairo to stroke or fill.
    The Cairo context which is passed in should already be configured for line width and colour
    \param cr The cairo context to draw in
    \param xBase The x box corner ordinate
    \param yBase The y box corner coordinate
    \param w The width of the box
    \param h The height of the box
    \param fill Fill the arrow head
    */
    CairoBox(cairo_t  *cr, float xBase, float yBase, float w, float h, bool fill) {
        setXY(xBase, yBase);
        width=w;
        height=h;
        filled=fill;
        draw(cr);
    }

    /// Destructor
    virtual ~CairoBox() {
    }

    /** Set the box size given starting point and the width, height and the cairo stroke or fill.
    \param xBase The x box corner ordinate
    \param yBase The y box corner coordinate
    \param w The width of the box
    \param h The height of the box
    \param fill Fill the arrow head
    */
    void boxSettings(float xBase, float yBase, float w, float h, bool fill) {
        setXY(xBase, yBase);
        width=w;
        height=h;
        filled=fill;
    }

    /** Construct a box given starting point and the width, height and use cairo to stroke or fill.
    The Cairo context which is passed in should already be configured for line width and colour
    \param cr The cairo context to draw in
    \param xBase The x box corner ordinate
    \param yBase The y box corner coordinate
    \param w The width of the box
    \param h The height of the box
    \param fill Fill the arrow head
    */
    void draw(cairo_t  *cr, float xBase, float yBase, float w, float h, bool fill) {
        boxSettings(xBase, yBase, w, h, fill);
        draw(cr);
    }

    /** Construct a box given starting point and the width, height and use cairo to stroke or fill.
    The Cairo context which is passed in should already be configured for line width and colour
    \param cr The cairo context to draw in
    \param xBase The x box corner ordinate
    \param yBase The y box corner coordinate
    */
    void draw(cairo_t  *cr, float xBase, float yBase) {
        setXY(xBase, yBase);
        draw(cr);
    }

    ///< Draw the box using the member variable state to define the box.
    void draw(cairo_t  *cr) {
        if (!cr) return;
        //cout<<x<<'\t'<<y<<'\t'<<width<<'\t'<<height<<endl;
        cairo_rectangle(cr, x, y, width, height);
        if (!filled)
            cairo_stroke (cr);
        else
            cairo_fill (cr);
    }

    /** Get the ordinate
    \return the ordinate (x)
    */
    float getX(void) {
        return x;
    }
    /** Get the coordinate
    \return the coordinate (y)
    */
    float getY(void) {
        return y;
    }
    /** Get the width
    \return the with
    */
    float getWidth(void) {
        return width;
    }
    /** Get the height
    \return the height
    */
    float getHeight(void) {
        return height;
    }

    /** Set a new x and y location for the box
    \param xBase The x box corner ordinate
    \param yBase The y box corner coordinate
    */
    void setXY(float xBase, float yBase) {
        x=xBase;
        y=yBase;
    }

    /** Set a new width and height for the box
    \param widthIn The new box width
    \param heightIn The new box height
    */
    void setWidthHeight(int widthIn, int heightIn) {
        width=widthIn;
        height=heightIn;
    }
};

#endif // CAIROBOX_H_
