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

#ifndef SELECTIONAREA_H_
#define SELECTIONAREA_H_

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "DrawingArea.H"
#include "CairoBox.H"
#include "ColourLineSpec.H"

#define SELECTIONAREA_DEFAULT_BOX_DIMENSION 100. ///< The default width/height of the box to draw

/** Overloads the DrawingArea methods to show a moving selection box in a pixmap.
Draws on the widget rather then the backing pixmap.
This class is used to show a moving selection box (movingBox) when the mouse moves over the drawing area. When the user presses
the button, then the static selection box (staticBox) is shown at the selected location.
\image html SelectionAreaTest.png
*/
class SelectionArea : public DrawingArea {
    CairoBox staticBox; ///< The box which is drawn on the mouse click
    CairoBox movingBox; ///< The box which highlights the current hover point

    ColourLineSpec backgroundColour; ///< The colour of the background
    ColourLineSpec staticColour; ///< The colour of the static button clicked box
    ColourLineSpec movingColour; ///< The colour of the static button clicked box

    cairo_surface_t *staticSurface; ///< The original pixmap rectangle for the static box
    cairo_surface_t *movingSurface; ///< The original pixmap rectangle for the moving box

    bool constrainAspectRatio; ///< If set, then retain the aspect ratio when setting the width or height.

    /** Clear only the moving box.
    Also sets up the source surface.
    */
    void clearMovingBox(cairo_t *cr) {
        // get rid of the boxes
#ifdef PIXMAP_ORIG_VER
        gdk_cairo_set_source_pixmap(cr, pixmap, 0., 0.);
#else
        cairo_set_source_surface(cr, surface, 0., 0.);
#endif
        movingBox.draw(cr);
    }

    /// Reraw the backing pixmap as appropriate
    void clearBoxes(cairo_t *cr) {
        clearMovingBox(cr); // setup the source and clear the moving box
        staticBox.draw(cr);
    }

    /** Move the movingBox.
    Redraw the backing pixmap region of the last box and then draw the box on the widget.
    \param widget The widget to move the brush on.
    \param x The ordinate to draw to.
    \param y The coordinate to draw to.
    */
    virtual void move(GtkWidget *widget, gdouble x, gdouble y) {
        cairo_t  *cr=gdk_cairo_create(getWindow()); // get a cairo context for rendering
        clearBoxes(cr);

        float width=staticBox.getWidth(), height=staticBox.getHeight();
        // paint the static box
        staticColour>>cr; // get the colour and line width
        staticBox.draw(cr);

        width=movingBox.getWidth(), height=movingBox.getHeight();
        // paint the moving box
        movingBox.setXY(x-width/2., y-height/2.); // position the box correctly
        movingColour>>cr; // get the colour and line width
        movingBox.draw(cr);

        cairo_destroy(cr);
    }

    /** Draw the static box on the screen.
    Redraw the backing pixmap and then the staticBox.
    \param widget The widget to draw on.
    \param x The ordinate to draw to.
    \param y The coordinate to draw to.
    */
    virtual void draw(GtkWidget *widget, gdouble x, gdouble y) {
        cairo_t *cr=gdk_cairo_create(getWindow()); // get a cairo context for rendering
        clearBoxes(cr);

        float width=movingBox.getWidth(), height=movingBox.getHeight();
        // paint the moving box
        movingBox.setXY(x-width/2., y-height/2.); // position the box correctly
        movingColour>>cr; // get the colour and line width
        movingBox.draw(cr);

        width=staticBox.getWidth(), height=staticBox.getHeight();
        // paint the static box
        staticBox.setXY(x-width/2., y-height/2.); // position the box correctly
        staticColour>>cr; // get the colour and line width
        staticBox.draw(cr);
        cairo_destroy(cr);
    }

    /** DrawingArea::erase is mapped to reSize
    Redraw the backing pixmap and then the staticBox.
    \param widget The widget to draw on.
    \param x The ordinate to draw to.
    \param y The coordinate to draw to.
    */
    virtual void erase(GtkWidget *widget, gdouble x, gdouble y) {
        reSize(widget, x, y);
    }

    /** resize the boxes on the screen.
    Redraw the backing pixmap and then the staticBox.
    \param widget The widget to draw on.
    \param xIn The ordinate for finding the new width size.
    \param yIn The coordinate for finding the new height size.
    */
    virtual void reSize(GtkWidget *widget, gdouble xIn, gdouble yIn) {
        // Find the distance between the static box and the x, y points and resize accordingly
        int x=(int)fabs((float)staticBox.getX()-xIn);
        int y=(int)fabs((float)staticBox.getY()-yIn);
        if (constrainAspectRatio){ // if the aspect ratio is constrained, then work with the hypotenuse
            float hyp=sqrt(pow((float)staticBox.getX()-xIn,2.)+pow((float)staticBox.getY()-yIn,2.));
            float hypOrig=sqrt(x*x+y*y);
            float factor=hyp/hypOrig;
            cout<<"hyp "<<hyp<<" hypOrig"<<hypOrig<<" factor "<<factor<<endl;
            cout<<"old x,y "<<x<<','<<y<<'\t'<<"new x,y ";
            x=(int)((float)x*factor);
            y=(int)((float)y*factor);
            cout<<x<<','<<y<<endl;
        }

        setBoxHeights(y);
        if (!constrainAspectRatio)
            setBoxWidths(x);
    }

    /** When the mouse leaves, clear the movingBox, and put it off screen but not the staticBox.
    */
    virtual void leaveEvent() {
		//cout<<"SelectionArea::leaveEvent"<<endl;
        cairo_t *cr=gdk_cairo_create(getWindow()); // get a cairo context for rendering
        clearMovingBox(cr); // setup the source and clear the moving box
        float width=movingBox.getWidth(), height=movingBox.getHeight();
        movingBox.setXY(0.-width, 0.-height);
    }

    /** Initialise the member variables.
    constrainAspectRation is false, allowing arbitrary changes in width and height.
    The moving box colour is orange and the static box colour is red.
    \param w The width of the rectangles
    \param h The height of the rectangles
    */
    void init(float w, float h) {
        constrainAspectRatio=false;
        movingColour = "o2"; // orange when moving.
        staticColour = "r2"; // red when static.
        staticBox.boxSettings(-w, -h, w, h, false); // false implies don't fill
        movingBox.boxSettings(-w, -h, w, h, false); // false implies don't fill
        staticSurface= cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
        movingSurface= cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
    }

public:
    /// Constructor
    SelectionArea() {
        init(SELECTIONAREA_DEFAULT_BOX_DIMENSION, SELECTIONAREA_DEFAULT_BOX_DIMENSION);
    }

    /** Construct give box dimensions.
    \param boxWidth The rectangular selection window width.
    \param boxHeight The rectangular selection window height.
    */
    SelectionArea(float boxWidth, float boxHeight) {
        movingSurface=staticSurface=NULL;
        init(boxWidth, boxHeight);
    }

    /// Destructor
    virtual ~SelectionArea() {
        if (staticSurface)
            cairo_surface_destroy(staticSurface);
        staticSurface=NULL;
        if (movingSurface)
            cairo_surface_destroy(movingSurface);
        movingSurface=NULL;
    }

    /** Refresh (redraw) the static and mooving boxes.
    \param cr the cairo context to draw in.
    */
    virtual void reDrawBoxes(cairo_t *cr) {
        // paint the static box
        movingColour>>cr; // get the colour and line width
        movingBox.draw(cr);
        staticColour>>cr; // get the colour and line width
        staticBox.draw(cr);
    }

    /** Get a copy of the staticBox.
    \return A copy of the staticBox
    */
    CairoBox getStaticBox(void) {
        return staticBox;
    }

    /** Move the static box to the centre of the image.
    */
    void centerStaticBox(void) {
        GdkRectangle rect;
        rect.x=rect.y=0;
        getSize(rect.width, rect.height);
        staticBox.setXY((rect.width-movingBox.getWidth())/2, (rect.height-movingBox.getHeight())/2);
        //g_signal_emit_by_name(G_OBJECT(widget), "leave_notify_event", this);
        //queueDraw();
        gdk_window_invalidate_rect(getWindow(), &rect, true);
    }

    /** On the expose event ensure that the boxes are redrawn.
    */
    virtual bool expose() {
        cairo_t  *cr=gdk_cairo_create(getWindow()); // get a cairo context for rendering
        reDrawBoxes(cr);
        cairo_destroy(cr);
        return FALSE;
    }

    /** Resize the moving and static box's width
    \param width The new width.
    */
    void setBoxWidths(int width) {
        if (width>0) {
            GdkRectangle rect; // first resize the boxes
            if (constrainAspectRatio)
                rect.height=(int)((float)movingBox.getHeight()/(float)movingBox.getWidth()*(float)width);
            else
                rect.height=movingBox.getHeight();
            movingBox.setWidthHeight(width, rect.height);
            if (constrainAspectRatio)
                rect.height=(int)((float)staticBox.getHeight()/(float)staticBox.getWidth()*(float)width);
            else
                rect.height=staticBox.getHeight();
            staticBox.setWidthHeight(width, rect.height);

            rect.x=rect.y=0; // now redraw
            getSize(rect.width, rect.height);
            gdk_window_invalidate_rect(getWindow(), &rect, true);
        }
    }

    /** Resize the moving and static box's height
    \param height The new height.
    */
    void setBoxHeights(int height) {
        if (height>0) {
            GdkRectangle rect; // first resize the boxes
            if (constrainAspectRatio)
                rect.width=(int)((float)movingBox.getWidth()/(float)movingBox.getHeight()*(float)height);
            else
                rect.width=movingBox.getWidth();
            movingBox.setWidthHeight(rect.width, height);
            if (constrainAspectRatio)
                rect.width=(int)((float)staticBox.getWidth()/(float)staticBox.getHeight()*(float)height);
            else
                rect.width=staticBox.getWidth();
            staticBox.setWidthHeight(rect.width, height);

            rect.x=rect.y=0; // now redraw
            getSize(rect.width, rect.height);
            gdk_window_invalidate_rect(getWindow(), &rect, true);
        }
    }

    /** Resize the moving and static box's width and height
    \param width The new width.
    \param height The new height.
    */
    void setBoxsWidthHeight(int width, int height) {
        if (width>0 || height>0) {
            GdkRectangle rect; // first resize the boxes
            if (width>0) {
                if (constrainAspectRatio)
                    rect.height=(int)((float)movingBox.getHeight()/(float)movingBox.getWidth()*(float)width);
                else
                    rect.height=movingBox.getHeight();
                movingBox.setWidthHeight(width, rect.height);
                if (constrainAspectRatio)
                    rect.height=(int)((float)staticBox.getHeight()/(float)staticBox.getWidth()*(float)width);
                else
                    rect.height=staticBox.getHeight();
                staticBox.setWidthHeight(width, rect.height);
//                if (constrainAspectRatio)
//                    goto redraw;
            }
            if (height>0) {
                if (constrainAspectRatio)
                    rect.width=(int)((float)movingBox.getWidth()/(float)movingBox.getHeight()*(float)height);
                else
                    rect.width=movingBox.getWidth();
                movingBox.setWidthHeight(rect.width, height);
                if (constrainAspectRatio)
                    rect.width=(int)((float)staticBox.getWidth()/(float)staticBox.getHeight()*(float)height);
                else
                    rect.width=staticBox.getWidth();
                staticBox.setWidthHeight(rect.width, height);
            }
//    redraw:
            rect.x=rect.y=0; // now redraw
            getSize(rect.width, rect.height);
            gdk_window_invalidate_rect(getWindow(), &rect, true);
        }
    }

    /** Set whether the keep the aspect ratio (constrainAspectRatio).
    \param kAR True to maintain and false to allow widths and heights to change as they wish.
    */
    void keepAspectRatio(bool kAR){
        constrainAspectRatio=kAR;
    }

	/** Set the static box's X and Y locations.
	\param x The x location of the static box
	\param y The y location of the static box
	*/
	void setStaticXY(float x, float y){
		staticBox.setXY(x, y);
        GdkRectangle rect; // first resize the boxes
        rect.x=rect.y=0; // now redraw
        getSize(rect.width, rect.height);
        gdk_window_invalidate_rect(getWindow(), &rect, true);
	}
};

#endif // SELECTIONAREA_H_
