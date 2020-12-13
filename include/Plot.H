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

#ifndef PLOT_H_
#define PLOT_H_

#ifdef _MSC_VER
typedef unsigned int uint;
#pragma warning( disable : 4554)
#endif

#include "ColourLineSpec.H"
#include "Table.H"
#include "Labels.H"

#include <iostream>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtkdatabox.h>
#include <gtkdatabox_points.h>
#include <gtkdatabox_lines.h>
#include <gtkdatabox_regions.h>
#include <gtkdatabox_ruler.h>
#include <gtkdatabox_grid.h>
#include <gtkdatabox_markers.h>
#include <gtkdatabox_offset_bars.h>

#include <typeinfo>
#include <stdint.h>
//#include <vector>
//#include <stdlib.h>
//#include <string.h>

using namespace std;

/** \brief simplistic plotting class which mimicks a octave approach to plotting
Use this class as if you were plotting in octave.

See the readme files for information on how to get/install GtkDatabox (which is required for this Plot class)

The Plot can support a title, xaxis, xlabel, ylabel, yaxis, plot, yaxis (right), ylabel (right) like so :
\code
==================================================================
|        | yLabelTL |             title       |        |         |
==================================================================
|        |          |                         |        |         |
| yLabel |   yaxis  |          plot           | yaxis2 | yLabel2 |
|        |          |                         |        |         |
==================================================================
|        | xLabelBL |           xaxis         |        |         |
==================================================================
|        |          |           xLabel        |        |         |
==================================================================
\endcode
Here is an example of how to use :
\code
    vector<float> x, y; // make sure you put some data into x and y
    Plot figure;
    figure.semilogx(&x[0],&y[0], x.size());
    //figure.limits(1, 100, 1, 100); // make sure you put suitable min/max values for x and y here, if you want to change the ranges shown by the plot.
    figure.set(figure.gca(), (char*)"XTick", FALSE, (char*)"YTick", FALSE, NULL); // remove ticks
    figure.set(figure.gca(), (char*)"YLabelRotate", "Horizontal", NULL); // put y labels horizontal
    // NOTE : many other set options
    figure.grid(true); // auto grid - NOTE : you can also set grid lines at specific points.
    figure.xlabel("x label"); // set some axis labels
    figure.ylabel("y label");
    figure.title("title");
    // once setup, if your data in x and y change, simply call : figure.replot(); This is a lightweight way to refresh the plot.
\endcode

Here is an example with two plots in the same window :
\image html PlotTest2.jpg

*/
class Plot : public Table {
    /** Given a variable, find the associated GType.
    \param val The variable to find the GType of.
    \tparam TYPE the type of the variable
    \return The GType on successful matching with gtkdatabox known gtypes. G_TYPE_INVALID otherwise.
    */
    template<typename TYPE>
    GType findType(TYPE val) {
        if (strcmp(typeid(val).name(), typeid(float).name())==0)
            return G_TYPE_FLOAT;
        if (strcmp(typeid(val).name(), typeid(double).name())==0)
            return G_TYPE_DOUBLE;
        if (strcmp(typeid(val).name(), typeid(int).name())==0)
            return G_TYPE_INT;
        if (strcmp(typeid(val).name(), typeid(unsigned int).name())==0)
            return G_TYPE_UINT;
        if (strcmp(typeid(val).name(), typeid(long).name())==0)
            return G_TYPE_LONG;
        if (strcmp(typeid(val).name(), typeid(unsigned long).name())==0)
            return G_TYPE_ULONG;
        if (strcmp(typeid(val).name(), typeid(int64_t).name())==0)
            return G_TYPE_INT64;
        if (strcmp(typeid(val).name(), typeid(uint64_t).name())==0)
            return G_TYPE_UINT64;
        if (strcmp(typeid(val).name(), typeid(char).name())==0)
            return G_TYPE_CHAR;
        if (strcmp(typeid(val).name(), typeid(unsigned char).name())==0)
            return G_TYPE_UCHAR;
        return G_TYPE_INVALID;
    }
protected:
    GtkWidget *axis; ///< The axis widget
    GtkDataboxGraph *gridGraph; ///< The grid which defaults to off
    bool holdOn; ///< Whether to hold the curves or not.
    bool gridOn; ///< Whether to the grid is on.
public:

    /*! Construct a generic axis with a white background
    */
    Plot(void) {
        // We are going to manually place everything
        setHomogeneous(false);
        resize(5,4); // setup the default table to be x dimension : ylabel, yruler, plot, yruler, ylabel
        //                               y dimension : title, plot, xruler, xlabel

        axis=gtk_databox_new ();
        setOptions((GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK), (GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK));
        setRegion(2,3,1,2); // put the plot in the middle
        *this<<axis; // put the plot in the middle

        GtkWidget *xRuler = gtk_databox_ruler_new (GTK_ORIENTATION_HORIZONTAL);
        //gtk_databox_ruler_set_scale_type (GTK_DATABOX_RULER (xRuler), GTK_DATABOX(axis)->priv->scale_type_x);
        gtk_databox_set_ruler_x (GTK_DATABOX(axis), GTK_DATABOX_RULER (xRuler));
        GtkWidget *yRuler = gtk_databox_ruler_new (GTK_ORIENTATION_VERTICAL);
        //gtk_databox_ruler_set_scale_type (GTK_DATABOX_RULER (yRuler), GTK_DATABOX(axis)->priv->scale_type_y);
        gtk_databox_set_ruler_y (GTK_DATABOX(axis), GTK_DATABOX_RULER (yRuler));

        setOptions((GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK), (GtkAttachOptions)(GTK_FILL));
        setRegion(2,3,2,3);  // put the x axis on the bottom
        *this<<xRuler;
        setOptions((GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK));
        setRegion(1, 2, 1, 2); // put the y axis on the left
        *this<<yRuler;

        show();

        setBackground("w"); // default white colour

        gridGraph=NULL;
    }

    /** Set the background a particular colour.
    \param clsStr The colour to set the background to.
    */
    void setBackground(const char *clsStr) {
        setBackground(ColourLineSpec(clsStr));
    }

    /** Set the background a particular colour.
    \param cls The colour to set the background to.
    */
    void setBackground(ColourLineSpec cls) {
#if GTK_MAJOR_VERSION>2
        GdkRGBA gdkC=cls.getColour();
#else
        GdkColor gdkC=cls.getColour();
#endif
        setBackground(gdkC);
    }

    /** Set the background a particular colour.
    \param gdkC The colour to set the background to.
    */
#if GTK_MAJOR_VERSION>2
    void setBackground(const GdkRGBA gdkC) {
        gtk_widget_override_background_color(axis, GTK_STATE_FLAG_NORMAL, &gdkC);
#else
    void setBackground(const GdkColor gdkC) {
        gtk_widget_modify_bg(axis, GTK_STATE_NORMAL, &gdkC); // set the background
#endif
    }

    /** \brief Get the current axis
    \return the axis of the plot
    */
    GtkWidget *gca(void) {
        return getCurrentAxis();
    }

    /** \brief Get the current axis
    \return the axis of the plot
    */
    GtkWidget *getCurrentAxis(void) {
        return axis;
    }

    /*! A method to show all of the rulers and graphs
    */
    void show(void) {
        gtk_widget_show_all(axis);
        gtk_widget_show_all(getWidget());
    }
    void hide(void) {
        gtk_widget_hide(axis);
        gtk_widget_hide(getWidget());
    }

//#if GTK_MAJOR_VERSION>2
//    /* Plots the cnt y values at x to the axis, with a default blue colour
//        \param x The ordinates for plotting
//        \param y The co-ordinates for plotting
//        \param cnt The number of elements to plot
//        \return the graph which was just added.
//        \tparam XTYPE The type of the ordinate
//        \tparam YTYPE The type of the co-ordinate
//    */
//    template<typename XTYPE, typename YTYPE>
//    GtkDataboxGraph *plot(XTYPE *x, YTYPE *y, int cnt) {
//#else
    /*! Plots the cnt y values at x to the axis, with a default blue colour
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \return the graph which was just added.
    */
    GtkDataboxGraph * plot(gfloat *x, gfloat *y, int cnt) {
//#endif
        ColourLineSpec cls;
        return plot(x, y, cnt, cls);
    }

//#if GTK_MAJOR_VERSION>2
//    /* Plots the cnt y values at x to the axis, with a specified colour and line type
//        \param x The ordinates for plotting
//        \param y The co-ordinates for plotting
//        \param cnt The number of elements to plot
//        \param clsChar The specification to be used with the ColourLineSpec class
//        \return the graph which was just added.
//        \tparam XTYPE The type of the ordinate
//        \tparam YTYPE The type of the co-ordinate
//    */
//    template<typename XTYPE, typename YTYPE>
//    GtkDataboxGraph *plot(XTYPE *x, YTYPE *y, int cnt, const char *clsChar) {
//#else
    /*! Plots the cnt y values at x to the axis, with a specified colour and line type
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param clsChar The specification to be used with the ColourLineSpec class
        \return the graph which was just added.
    */
    GtkDataboxGraph *plot(gfloat *x, gfloat *y, int cnt, const char *clsChar) {
//#endif
        ColourLineSpec cls(clsChar);
        return plot(x, y, cnt, cls);
    }

//#if GTK_MAJOR_VERSION<3
    /* Plots the cnt y values at x to the axis
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param cls The ColourLineSpec class which determines the colour line type and size
        \return the graph which was just added.
    */
    GtkDataboxGraph *plot(gfloat *x, gfloat *y, int cnt, ColourLineSpec &cls) {
        if (!holdOn) {
            clf();
            if (gridOn)
                grid(gridOn);
        }

        //if (gtk_widget_get_realized (axis)){
        GtkDataboxGraph *graph=NULL;
#if GTK_MAJOR_VERSION>2
        GdkRGBA gdkC=cls.getColour();
#else
        GdkColor gdkC=cls.getColour();
#endif
        int size=cls.getSize();
        if (cls.linePlot)
            graph = gtk_databox_lines_new (cnt, x, y, &gdkC, size);
        else
            graph = gtk_databox_points_new (cnt, x, y, &gdkC, size);
        gtk_databox_graph_add_front (GTK_DATABOX (axis), graph);
        //}
        return graph;
    }
//#else
//    /* Plots the cnt y values at x to the axis
//        \param x The ordinates for plotting
//        \param y The co-ordinates for plotting
//        \param cnt The number of elements to plot
//        \param cls The ColourLineSpec class which determines the colour line type and size
//        \return the graph which was just added.
//        \tparam XTYPE The type of the ordinate
//        \tparam YTYPE The type of the co-ordinate
//    */
//    template<typename XTYPE, typename YTYPE>
//    GtkDataboxGraph *plot(XTYPE *x, YTYPE *y, int cnt, const ColourLineSpec &cls) {
//        if (cnt<=0)
//            return NULL;
//        GType xType, yType;
//        if ((xType=findType(x[0]))==G_TYPE_INVALID)
//            return NULL;
//        if ((yType=findType(y[0]))==G_TYPE_INVALID)
//            return NULL;
//
//        if (!holdOn) {
//            clf();
//            if (gridOn)
//                grid(gridOn);
//        }
//
//        GtkDataboxGraph *graph=NULL;
//#if GTK_MAJOR_VERSION>2
//        GdkRGBA gdkC=cls.getColour();
//#else
//        GdkColor gdkC=cls.getColour();
//#endif
//        int size=cls.getSize();
//        if (cls.linePlot)
//            graph = gtk_databox_lines_new_full (cnt, cnt, x, 0, 1, xType, y, 0, 1, yType, &gdkC, size);
//        else
//            graph = gtk_databox_points_new_full (cnt, cnt, x, 0, 1, xType, y, 0, 1, yType, &gdkC, size);
//        gtk_databox_graph_add_front (GTK_DATABOX (axis), graph);
//        return graph;
//    }
//#endif

    /*! Plots the cnt y1 and y2 values at x to the axis
        This plot is a region of default colour
        \param x The ordinates for plotting
        \param y1 The first co-ordinate for plotting
        \param y2 The second co-ordinate for plotting
        \param cnt The number of elements to plot
        \return the graph which was just added.
    */
    GtkDataboxGraph *plot(gfloat *x, gfloat *y1, gfloat *y2, int cnt) {
        ColourLineSpec cls;
        return plot(x, y1, y2, cnt, cls);
    }

    /*! Plots the cnt y1 and y2 values at x to the axis.
        This plot is a region of colour if a line plot is specified: e.g. clsChar="k"
        This plot is two dotted lines if a dotted plot is specified: e.g. clsChar="k:"
        \param x The ordinates for plotting
        \param y1 The first co-ordinate for plotting
        \param y2 The second co-ordinate for plotting
        \param cnt The number of elements to plot
        \param clsChar The specification to be used with the ColourLineSpec class
        \return the graph which was just added.
    */
    GtkDataboxGraph *plot(gfloat *x, gfloat *y1, gfloat *y2, int cnt, const char *clsChar) {
        ColourLineSpec cls(clsChar);
        return plot(x, y1, y2, cnt, cls);
    }

    /*! Plots the cnt y1 and y2 values at x to the axis
        This plot is a region of colour if a line plot is specified: e.g. cls="k"
        This plot are offset bars if a dotted plot is specified: e.g. cls="k:"
        \param x The ordinates for plotting
        \param y1 The first co-ordinate for plotting
        \param y2 The second co-ordinate for plotting
        \param cnt The number of elements to plot
        \param cls The ColourLineSpec class which determines the colour line type and size
        \return the graph which was just added.
    */
    GtkDataboxGraph *plot(gfloat *x, gfloat *y1,  gfloat *y2, int cnt, ColourLineSpec &cls) {
        if (!holdOn) {
            clf();
            if (gridOn)
                grid(gridOn);
        }

        //if (gtk_widget_get_realized (axis)) {
        GtkDataboxGraph *graph=NULL;
#if GTK_MAJOR_VERSION>2
        GdkRGBA gdkC=cls.getColour();
#else
        GdkColor gdkC=cls.getColour();
#endif
        int size=cls.getSize();
        if (cls.linePlot)
            graph = gtk_databox_regions_new (cnt, x, y1, y2, &gdkC);
        else
            graph = gtk_databox_offset_bars_new(cnt, x, y1, y2, &gdkC, size);
        gtk_databox_graph_add_front (GTK_DATABOX (axis), graph);
        //}
        return graph;
    }

    /** \brief Plots with a logarithmic ordinate scale
        Plotting the data with a logarithmic ordinate scale and a default colour
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \return the graph which was just added.
    */
    GtkDataboxGraph * semilogx(gfloat *x, gfloat *y, int cnt) {
        ColourLineSpec cls;
        return semilogx(x, y, cnt, cls);
    }

    /** \brief Plots with a logarithmic ordinate scale
        Plotting the data with a logarithmic ordinate scale
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param clsChar The ColourLineSpec interpretable character array
        \return the graph which was just added.
    */
    GtkDataboxGraph * semilogx(gfloat *x, gfloat *y, int cnt, const char *clsChar) {
        ColourLineSpec cls(clsChar);
        return semilogx(x, y, cnt, cls);
    }

    /** \brief Plots with a logarithmic ordinate scale
        Plotting the data with a logarithmic ordinate scale
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param cls The ColourLineSpec object to use for plotting
        \return the graph which was just added.
    */
    GtkDataboxGraph *semilogx(gfloat *x, gfloat *y, int cnt, ColourLineSpec &cls) {
        GtkDataboxGraph *graph=plot(x, y, cnt, cls);
        gtk_databox_set_scale_type_x (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LOG);
        return graph;
    }

    /** \brief Plots with a logarithmic ordinate scale
        Plotting the data with a logarithmic ordinate scale and a default colour
        Each x ordinate has two y co-ordinates - resulting in offset bars
        \param x The ordinates for plotting
        \param y1 The co-ordinates for plotting
        \param y2 The co-ordinates for plotting
        \param cnt The number of elements to plot
        \return the graph which was just added.
    */
    GtkDataboxGraph *semilogx(gfloat *x, gfloat *y1, gfloat *y2, int cnt) {
        ColourLineSpec cls;
        return semilogx(x, y1, y2, cnt, cls);
    }

    /** \brief Plots with a logarithmic ordinate scale
        Plotting the data with a logarithmic ordinate scale and a default colour
        Each x ordinate has two y co-ordinates - resulting in solid regions if line plotting.
        \see plot where y1 and y2 are specified.
        \param x The ordinates for plotting
        \param y1 The co-ordinates for plotting
        \param y2 The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param clsChar The ColourLineSpec interpretable character array
        \return the graph which was just added.
    */
    GtkDataboxGraph *semilogx(gfloat *x, gfloat *y1, gfloat *y2, int cnt, const char *clsChar) {
        ColourLineSpec cls(clsChar);
        return semilogx(x, y1, y2, cnt, cls);
    }

    /** \brief Plots with a logarithmic ordinate scale
        Plotting the data with a logarithmic ordinate scale and a default colour
        Each x ordinate has two y co-ordinates - resulting in solid regions if line plotting.
        \see plot where y1 and y2 are specified.
        \param x The ordinates for plotting
        \param y1 The co-ordinates for plotting
        \param y2 The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param cls The ColourLineSpec object to use for plotting
        \return the graph which was just added.
    */
    GtkDataboxGraph *semilogx(gfloat *x, gfloat *y1, gfloat *y2, int cnt, ColourLineSpec &cls) {
        GtkDataboxGraph *graph=plot(x, y1, y2, cnt, cls);
        //if (gtk_widget_get_realized (axis))
        if (gtk_databox_get_scale_type_x(GTK_DATABOX (axis))!=GTK_DATABOX_SCALE_LOG)
            gtk_databox_set_scale_type_x (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LOG);
        return graph;
    }

    /** \brief Plots with a logarithmic co-ordinate scale
        Plotting the data with a logarithmic co-ordinate scale and a default colour
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \return the graph which was just added.
    */
    GtkDataboxGraph *semilogy(gfloat *x, gfloat *y, int cnt) {
        ColourLineSpec cls;
        return semilogy(x, y, cnt, cls);
    }

    /** \brief Plots with a logarithmic co-ordinate scale
        Plotting the data with a logarithmic co-ordinate scale
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param clsChar The ColourLineSpec interpretable character array
        \return the graph which was just added.
    */
    GtkDataboxGraph *semilogy(gfloat *x, gfloat *y, int cnt, const char *clsChar) {
        ColourLineSpec cls(clsChar);
        return semilogy(x, y, cnt, cls);
    }

    /** \brief Plots with a logarithmic co-ordinate scale
        Plotting the data with a logarithmic co-ordinate scale
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param cls The ColourLineSpec object to use for plotting
        \return the graph which was just added.
    */
    GtkDataboxGraph *semilogy(gfloat *x, gfloat *y, int cnt, ColourLineSpec &cls) {
        GtkDataboxGraph *graph=plot(x, y, cnt, cls);
        //if (gtk_widget_get_realized (axis))

        if (gtk_databox_get_scale_type_y(GTK_DATABOX (axis))!=GTK_DATABOX_SCALE_LOG)
            gtk_databox_set_scale_type_y (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LOG);
        return graph;
    }

    /** \brief Plots with logarithmic ordingate and co-ordinate scales
        Plotting the data with logarithmic ordinate and co-ordinate scales and a default colour
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \return the graph which was just added.
    */
    GtkDataboxGraph *loglog(gfloat *x, gfloat *y, int cnt) {
        ColourLineSpec cls;
        return loglog(x, y, cnt, cls);
    }

    /** \brief Plots with logarithmic ordingate and co-ordinate scales
        Plotting the data with logarithmic ordinate and co-ordinate scales and a default colour
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param clsChar The ColourLineSpec object to use for plotting
        \return the graph which was just added.
    */
    GtkDataboxGraph *loglog(gfloat *x, gfloat *y, int cnt, const char *clsChar) {
        ColourLineSpec cls(clsChar);
        return loglog(x, y, cnt, cls);
    }
    /** \brief Plots with logarithmic ordingate and co-ordinate scales
        Plotting the data with logarithmic ordinate and co-ordinate scales
        \param x The ordinates for plotting
        \param y The co-ordinates for plotting
        \param cnt The number of elements to plot
        \param cls The ColourLineSpec object to use for plotting
        \return the graph which was just added.
    */
    GtkDataboxGraph *loglog(gfloat *x, gfloat *y, int cnt, ColourLineSpec &cls) {
        GtkDataboxGraph *graph=plot(x, y, cnt, cls);
        if (gtk_databox_get_scale_type_x(GTK_DATABOX (axis))!=GTK_DATABOX_SCALE_LOG)
            gtk_databox_set_scale_type_x (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LOG);
        if (gtk_databox_get_scale_type_y(GTK_DATABOX (axis))!=GTK_DATABOX_SCALE_LOG)
            gtk_databox_set_scale_type_y (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LOG);
        return graph;
    }

    /*! Displays text at an x and y location, with a specified colour
        \param x The ordinate to place the text at
        \param y The co-ordinate to place the text at
        \param textDisp The text to display
        \param clsChar The specification to be used with the ColourLineSpec class
    */
    GtkDataboxGraph * text(gfloat *x, gfloat *y, const char *textDisp, const char *clsChar) {
        ColourLineSpec cls(clsChar);
        return text(x, y, textDisp, cls);
    }

    /*! Displays text at an x and y location, with a specified colour
        \param x The ordinate to place the text at
        \param y The co-ordinate to place the text at
        \param textDisp The text to display
        \param cls The colour of the text
        \return graph which is the added label
    */
    GtkDataboxGraph *text(gfloat *x, gfloat *y, const char *textDisp, ColourLineSpec &cls) {
#if GTK_MAJOR_VERSION>2
        GdkRGBA gdkC=cls.getColour();
#else
        GdkColor gdkC=cls.getColour();
#endif
        GtkDataboxGraph *graph = gtk_databox_markers_new (1, x, y, &gdkC, 1, GTK_DATABOX_MARKERS_NONE);
        bool boxed=false;
        gtk_databox_markers_set_label (GTK_DATABOX_MARKERS (graph), 0, GTK_DATABOX_MARKERS_TEXT_CENTER, (gchar*)textDisp, boxed);
        gtk_databox_graph_add_front (GTK_DATABOX (axis), graph);
        return graph;
    }

    /*! Clear the figure
    */
    void clear(void) {
        clf();
    }

    /*! Clear the figure
    */
    void clf(void) {
        //if (gtk_widget_get_realized (axis))
        gtk_databox_graph_remove_all(GTK_DATABOX (axis));
    }

    /*! Sets the axis to hold the current plots (curves) or to wipe them
    */
    void hold(bool on) {
        holdOn=on;
    }

    /*! Generates a grid on the axis
    Check if a grid exists in gtkdatabox, if it does, then re-use it.
    Defaults to 5 horizontal and vertical lines
    */
    void grid(bool on) {
        if (gridGraph) {
            GtkDataboxGrid *gridG=GTK_DATABOX_GRID(gridGraph);
            if (gtk_databox_grid_get_hline_vals(gridG)) { // reuse previously stored graph
                grid(on, gtk_databox_grid_get_hlines(gridG), gtk_databox_grid_get_hline_vals(gridG), gtk_databox_grid_get_vlines(gridG), gtk_databox_grid_get_vline_vals(gridG));
                return;
            }
        }
        grid(on, 5, NULL, 5, NULL);
    }

    /*! Generates a grid on the axis
    \param on whether to plot the grid
    \param hCnt The number of horizontal lines
    \param hLines The hCnt hLine points
    \param vCnt The number of vertical lines
    \param vLines The vCnt vLine points
    */
    void grid(bool on, int hCnt, gfloat *hLines, int vCnt, gfloat *vLines) {
        //if (gtk_widget_get_realized (axis))
        if (on) {
            if (gridGraph!=NULL) {
                gtk_databox_graph_remove(GTK_DATABOX (axis), gridGraph);
                gridGraph=NULL;
            }
            ColourLineSpec cls("a1"); // gray line of size 1
#if GTK_MAJOR_VERSION>2
            GdkRGBA gdkC=cls.getColour();
#else
            GdkColor gdkC=cls.getColour();
#endif
            int size=cls.getSize();
            if ((hLines!=NULL) & (vLines!=NULL))
                gridGraph=gtk_databox_grid_array_new(hCnt, vCnt, hLines, vLines, &gdkC, size);
            else
                gridGraph=gtk_databox_grid_new(hCnt, vCnt, &gdkC, size);
            gtk_databox_graph_add_front (GTK_DATABOX (axis), gridGraph);
        } else {
            if (gridGraph!=NULL) {
                gtk_databox_graph_remove(GTK_DATABOX (axis), gridGraph);
            }
        }
        gridOn=on;
    }

    /** \brief Set the limits of the viewable region of the plot.
    This will set the viewable rectangle to be between the min/max specified x and y values.
    \param min_x The minimum x value to show
    \param max_x The maximum x value to show
    \param min_y The minimum y value to show
    \param max_y The maximum y value to show
    */
    void limits(float min_x, float max_x, float min_y, float max_y) {
        if (min_y==max_y) // fix if min and max y are the same
            if (min_y==0.)
                max_y=-(min_y=-1.);
            else
                max_y=(min_y=max_y*.9)/.9*1.1;
        if (min_x==max_x) // fix if max and min y are the same
            if (min_x==0.)
                max_x=-(min_x=-1.);
            else
                max_x=(min_x=max_x*.9)/.9*1.1;
        gtk_databox_set_total_limits (GTK_DATABOX (axis), min_x, max_x, max_y, min_y);
    }

    /** \brief Set the limits to autoscale
    Rescales the figure to show everything plotted
    \param border The fraction of the width and height to shrink the graph by, producing a border
    */
    void limits(gfloat border=0.) {
        // guard against a plot with no real limits in either or both axes.
        gfloat min_x, max_x, min_y, max_y;
        gint extrema_success = gtk_databox_calculate_extrema (GTK_DATABOX (axis), &min_x, &max_x, &min_y, &max_y);
        if (min_x==max_x==0.)
            if (min_y==max_y==0.)
                limits(0.,1.,0.,1.);
            else
                limits(min_x-(max_y-min_y)*.1, max_x-(max_y-min_y)*.1,min_y,max_y);
        else if (min_y==max_y==0.)
            limits(min_x, max_x, min_y-(max_x-min_x)*.1, max_y-(max_x-min_x)*.1);

        gtk_databox_auto_rescale(GTK_DATABOX (axis), border);
    }

    /*! \brief Instruct gtk to replot
    If the underlying plot data pointers haven't changed, it is not necessary to call plot again.
    Simply call this method to get gtk to replot the contents of the data pointers used in the last call to Plot::plot
    */
    void replot(void) {
        if (gtk_widget_is_drawable (GTK_WIDGET (axis)))
            gtk_widget_queue_draw (GTK_WIDGET (axis));
    }

    /** \brief sets the property of the figure
    Various figure axis settings.
    This list of settings must be NULL terminated. Valid options are (either camel or lower case):
    \code
    // For x-axis and y-axis linear/log scaling :
    figure.set(figure.gca(), "YScale", "Log", NULL); // Y-axis has logarithmic scaling - similar for x-axis
    figure.set(figure.gca(), "YScale", "Linear", NULL); // Y-axis has inear scaling - similar for x-axis (default when "Log" or "log" aren't found)

    //Ticks
    figure.set(figure.gca(), "XMinorTick", false, "YMinorTick", false, NULL); // don't (false) or do (true) show minor ticks on the axis rulers
    figure.set(figure.gca(), "XTick", false, "YTick", false, NULL); // don't show ticks on the axis rulers

    //Manual tick locations
    int XCnt=5; gfloat xLocations[]= {500.0, 1000., 2000., 4000., 6000.};
    int YCnt=5; float yLocations[]= {500.0, 1000., 2000., 4000., 6000.};
    figure.set(figure.gca(), "XTick", XCnt, XLocations, "YTick", XCnt, XLocations, NULL); // show XCnt XTicks at XLocations, similar for Y

    //Label formatting :
    //           NOTE: This uses printf style formatting commands. Must use %%.2f to represent %.2f internally.
    figure.set(figure.gca(), "XLabelFormat", "%%.2f", NULL); // print two digits after the decimal point in this case
    // NOTE: in scientific mode, the end of the label is cropped, so change the max length like so :
    figure.set(figure.gca(), "XLabelFormat", "%%.2e", NULL); // scientific mode with two digits after the decimal point
    figure.set(figure.gca(), "XLabelLength", 9, NULL); // allow longer x tick labels so the end of the scientific notation doesn't get cropped.


    //Manual labels (at tick locations) :
    gchar *xLabels[]= {(gchar*)"These",(gchar*)"are",(gchar*)"manually",(gchar*)"placed", (gchar*)"labels"};
    figure.set(figure.gca(), "XTickLabel", xLabels, NULL); // Manual labels at tick locations, similar for Y

    // Current cursor positions : Whether to draw the arrows in the rulers which indicate the current cursor position.
    figure.set(figure.gca(), "XPosition", false, "YPosition", true, NULL); // Don't draw x arrow positions, but draw y arrow potitions.

    // Shift the ticks left or right : moves the tick labels left or right by this number of pixels.
    figure.set(figure.gca(), "XTickXOffset", 10, "YTickXOffset", 3, NULL); // Shift the tick labels left 10 pixels on the X axis and 3 on the y axis.

    // Vertical or horizontal label rotations
    figure.set(figure.gca(), "YLabelRotate", "Vertical", NULL); // Draw Y labels vertically. Similar for x.
    figure.set(figure.gca(), "YLabelRotate", "Horizontal", NULL); // Draw Y labels horizontally. Similar for x.

    // For vertical rulers with horizontal text, label alignment
    figure.set(figure.gca(), "YLabelAlignment", "Right", NULL); // Draw Y labels right adjusted - if the y-axis has horizontal text
    figure.set(figure.gca(), "YLabelAlignment", "center", NULL); // Draw Y labels centrally - if the y-axis has horizontal text

    // Label maximum lengths : Maximum length (digits or chars) of tick labels
    figure.set(figure.gca(), "XLabelLength", 3, NULL); // Draw Y labels horizontally. With a maximum length of 3 chars

    // Axis edge shadow : How to render the edge axis shadows
    //        GTK_SHADOW_NONE=0
    //        GTK_SHADOW_IN=1
    //        GTK_SHADOW_OUT=2
    //        GTK_SHADOW_ETCHED_IN=3
    //        GTK_SHADOW_ETCHED_OUT=4
    figure.set(figure.gca(), "XShadow", GTK_SHADOW_IN,  "YShadow", GTK_SHADOW_OUT, NULL); // Draw inward box shadow for X and outwards box shadow for Y

    // Main plotting box edge shadow : How to render the edge axis shadows
    figure.set(figure.gca(), "BoxShadow", GTK_SHADOW_OUT, NULL); // Draw a box around the main plotting window
    \endcode

    In general, all properties, are specified for both X and Y axes.
    */
    void set(GtkWidget *widget, ...) {
        va_list args;
        va_start(args, widget);
        //cout<<"starting"<<endl;
        while (1) { // terminates when a null is read
            char *token=va_arg(args, char *);
            if ((token==NULL) | (token==(char *)'\0'))
                break;

            // first attempt to process non-axis (box) settings
            if ((strcmp(&token[1],"oxShadow")==0) | (strcmp(&token[1],"oxshadow")==0))
                gtk_databox_set_box_shadow(GTK_DATABOX(widget), (GtkShadowType)va_arg(args, int));
            else if ((strcmp(&token[1],"Scale")==0) | (strcmp(&token[1],"scale")==0)) {
                char *linearOrLog=va_arg(args, char*);
                if (tolower(token[0])=='x') {
                    if (strcmp(&linearOrLog[1],"og")==0) // check for the right keyword
                        gtk_databox_set_scale_type_x (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LOG);
                    else
                        gtk_databox_set_scale_type_x (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LINEAR);
                }
                if (tolower(token[0])=='y') {
                    if (strcmp(&linearOrLog[1],"og")==0) // check for the right keyword
                        gtk_databox_set_scale_type_y (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LOG);
                    else
                        gtk_databox_set_scale_type_y (GTK_DATABOX (axis), GTK_DATABOX_SCALE_LINEAR);
                }
            } else {
                // process axis (ruler) settings
                GtkDataboxRuler *ruler;
                bool xRuler=1;
                if (tolower(token[0])=='x')
                    ruler=gtk_databox_get_ruler_x(GTK_DATABOX (widget));
                else {
                    ruler=gtk_databox_get_ruler_y(GTK_DATABOX (widget));
                    xRuler=0;
                }

                if ((strcmp(&token[1],"MinorTick")==0) | (strcmp(&token[1],"minortick")==0)) {

                    int draw_ticks=va_arg(args, int);
                    gtk_databox_ruler_set_draw_subticks(ruler,draw_ticks);

                } else if ((strcmp(&token[1],"Tick")==0) | (strcmp(&token[1],"tick")==0)) {
                    uint manual_tick_cnt=va_arg(args, int);
                    if (manual_tick_cnt==0)
                        gtk_databox_ruler_set_draw_ticks(ruler,manual_tick_cnt);
                    else {
                        gtk_databox_ruler_set_manual_tick_cnt(ruler, manual_tick_cnt);
                        gtk_databox_ruler_set_manual_ticks(ruler, va_arg(args, gfloat*));
                    }
                } else if ((strcmp(&token[1],"LabelFormat")==0) | (strcmp(&token[1],"labelformat")==0)) {
                    int scaleType;
                    if (xRuler)
                        scaleType=gtk_databox_get_scale_type_x(GTK_DATABOX (widget));
                    else
                        scaleType=gtk_databox_get_scale_type_y(GTK_DATABOX (widget));

                    if (scaleType==GTK_DATABOX_SCALE_LINEAR) // set the label format
                        gtk_databox_ruler_set_linear_label_format(ruler, va_arg(args, char*));
                    else
                        gtk_databox_ruler_set_log_label_format(ruler, va_arg(args, char*));

                } else if ((strcmp(&token[1],"LabelLength")==0) | (strcmp(&token[1],"labellength")==0)) {
                    int scaleType;
                    if (xRuler)
                        scaleType=gtk_databox_get_scale_type_x(GTK_DATABOX (widget));
                    else
                        scaleType=gtk_databox_get_scale_type_y(GTK_DATABOX (widget));

                    gtk_databox_ruler_set_max_length(ruler, va_arg(args, int));
                } else if ((strcmp(&token[1],"LabelLength")==0) | (strcmp(&token[1],"labellength")==0)) {
                    gtk_databox_ruler_set_max_length(ruler, va_arg(args, int));
                } else if ((strcmp(&token[1],"TickLabel")==0) | (strcmp(&token[1],"ticklabel")==0)) {
                    gtk_databox_ruler_set_manual_tick_labels(ruler, (gchar**)va_arg(args, gchar**));
                } else if ((strcmp(&token[1],"TickXOffset")==0) | (strcmp(&token[1],"tickxoffset")==0)) {
                    gtk_databox_ruler_set_text_hoffset(ruler, va_arg(args, int));
                } else if ((strcmp(&token[1],"Position")==0) | (strcmp(&token[1],"position")==0)) {
                    gtk_databox_ruler_set_draw_position(ruler, va_arg(args, int));
                } else if ((strcmp(&token[1],"Shadow")==0) | (strcmp(&token[1],"shadow")==0)) {
                    gtk_databox_ruler_set_box_shadow(ruler, (GtkShadowType)va_arg(args, int));
                } else if ((strcmp(&token[1],"LabelAlignment")==0) | (strcmp(&token[1],"labelalignment")==0)) {
                    char *which=va_arg(args, char*);
                    if (strcmp(&which[1],"ight")==0) // check for the right keyword
                        gtk_databox_ruler_set_text_alignment(ruler, PANGO_ALIGN_RIGHT);
                    else if (strcmp(&which[1],"enter")==0) // check for the center keyword
                        gtk_databox_ruler_set_text_alignment(ruler, PANGO_ALIGN_CENTER);
                    else // left aligned which is standard
                        gtk_databox_ruler_set_text_alignment(ruler, PANGO_ALIGN_LEFT);
                } else if ((strcmp(&token[1],"LabelRotate")==0) | (strcmp(&token[1],"labelrotate")==0)) {
                    char *which=va_arg(args, char*);
                    if (strcmp(&which[1],"ertical")==0) // check for the vertical keyword
                        gtk_databox_ruler_set_text_orientation(ruler, GTK_ORIENTATION_VERTICAL);
                    else
                        gtk_databox_ruler_set_text_orientation(ruler, GTK_ORIENTATION_HORIZONTAL);
                }

                if (tolower(token[0])=='x')
                    gtk_databox_set_ruler_x(GTK_DATABOX (widget), ruler);
                else
                    gtk_databox_set_ruler_y(GTK_DATABOX (widget), ruler);
            }
        }
        va_end(args);
    }

    /** Connect a signal callback to the axis
    \param event the signal to generate the callback e.g. "expose-event"
    \param callback the callback to call
    \param data the user data to pass to the callback
    */
    void connectAfter(const char *event, GCallback callback, gpointer data) {
        g_signal_connect_after(axis, event, callback, data);
    }

    /**
    Set an xlabel, by default the labels is placed below the plot.
    By changing the indexes, it can be placed in any table cell.
    For example this places it in the bottom left (same as xLabelBL) :
    \code
        int indexes[]={1,2,2,3};
        xLabel("bottom left",indexes);
    \endcode
    \param label The label to display
    \param indexes The table cell to place the label in
    */
    void xLabel(const char* label, int *indexes=NULL) {
        if (indexes==NULL) {
            setRegion(2, 3, 3, 4);
            setOptions((GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK), (GtkAttachOptions)(GTK_FILL));
        } else
            *this<<indexes;
        *this<<(Labels()<<label).setAlignment(0.5, 0.5);
    }

    /**
    Set an xlabel in the bottom left location (below the yaxis)
    \param label The label to display
    */
    void xLabelBL(const char* label) {
        setOptions((GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(GTK_FILL | GTK_SHRINK));
        int indexes[]= {1,2,2,3};
        xLabel(label,indexes);
    }

    /**
    Set a ylabel, by default the label is placed left of the plot.
    By changing the indexes, it can be placed in any table cell.
    For example this places it in the top left (same as yLabelTL) :
    \code
        int indexes[]={1,2,0,1};
        yLabel("top left",indexes);
    \endcode
    \param label The label to display
    \param indexes The table cell to place the label in
    */
    void yLabel(const char* label, int *indexes=NULL) {
        Labels ylabel;
        ylabel<<label;
        if (indexes==NULL) {
            setRegion(0, 1, 1, 2);
            setOptions((GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK));
            ylabel.setAngle(90.0);
        } else
            *this<<indexes;
        ylabel.setAlignment(0.5, 0.5);
        *this<<ylabel.current();
    }

    /**
    Set a ylabel, by default the label is placed top left of the plot.
    \param label The label to display
    */
    void yLabelTL(const char* label) {
        setOptions((GtkAttachOptions)(GTK_FILL | GTK_SHRINK), (GtkAttachOptions)(GTK_FILL));
        int indexes[]= {1,2,0,1};
        yLabel(label,indexes);
    }

    /**
    Set a title, by default the title is placed above the plot
    By changing the indexes, it can be placed in any table cell.
    \sa xLabel, yLabel
    \param titleStr The title to display
    \param indexes The table cell to place the label in
    */
    void title(const char* titleStr, int *indexes=NULL) {
        Labels labels;
        labels<<titleStr;
        labels.setAlignment(0.5, 0.5);
        title(labels.getWidget(), indexes);
    }

    /**
    Set a title, by default the title is placed above the plot
    By changing the indexes, it can be placed in any table cell.
    \sa xLabel, yLabel
    \param widget The title to display which is expected to be a label widget
    \param indexes The table cell to place the label in
    */
    void title(GtkWidget *widget, int *indexes=NULL) {
        if (indexes==NULL) {
            setRegion(2, 3, 0, 1);
            setOptions((GtkAttachOptions)(GTK_FILL | GTK_EXPAND | GTK_SHRINK), (GtkAttachOptions)(GTK_FILL));
        } else
            *this<<indexes;
        *this<<widget;
    }
//static bool expose(GtkWidget *widget, GdkEventExpose *event, gpointer data){
//    Plot*p=static_cast<Plot*>(data);
//    p->replot();
//    return false;
//}
//    cout<<"Plot::expose"<<endl;
//    if (gtk_widget_get_realized (axis))
//        return false;
//        else
//        return true;
//}
    /** Remove a single line or text from the graph (unplot).
    \param toRemove The object to remove from the plot figure.
    */
    void remove(GtkDataboxGraph *toRemove) {
        gtk_databox_graph_remove (GTK_DATABOX (axis), toRemove);
    }

    /** Remove a single line or text from the graph (unplot).
    \param toRemove The object to remove from the plot figure.
    \return A reference to the instance of this class.
    */
    Plot &operator>>(GtkDataboxGraph *toRemove) {
        remove(toRemove);
        return *this;
    }

    /** Find the Ordinate for a given pixel
    */
    gfloat pixelToX(gint16 pixel) {
        return gtk_databox_pixel_to_value_x(GTK_DATABOX (axis), pixel);
    }

    /** Find the Co-Ordinate for a given pixel
    */
    gfloat pixelToY(gint16 pixel) {
        return gtk_databox_pixel_to_value_y(GTK_DATABOX (axis), pixel);
    }
};
#endif //PLOT_H_
