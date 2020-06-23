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
#ifndef TABLE_H_
#define TABLE_H_

#include "Container.H"
#include <vector>

/** Table

This class allows both auto and manual widget placement into a table.
\image html TableTest.jpg

Allows you to set-up a table like so :
\code
 // a 2x2 table
 Table table(2, 2); table.show();

 // auto-place 4 buttons into the table
 table<<buttons.grab(1)<<buttons.grab(2)<<buttons.grab(3)<<buttons.grab(4);

 // manually place 4 buttons into the table :
  table<<0, 1, 0, 1<<buttons.grab(1); // top left
  table<<1, 2, 0, 1<<buttons.grab(2); // top right
  table<<0, 1, 1, 2<<buttons.grab(3); // bottom left
  table<<1, 2, 1, 2<<buttons.grab(4); // bottom right
\endcode
*/
class Table : public Container {
    short int xStart; ///< The table column location to start attachment from.
    short int xEnd; ///< The table column location to end attachment.
    short int yStart; ///< The table row location to start attachment from.
    short int yEnd; ///< The table row location to end attachment.

    std::vector<int> regionTemp; ///< This region starts empty, when it gets 4 elements, it sets the region

    guint xPadding; ///< The horizontal padding between widgets
    guint yPadding; ///< The vertical padding between widgets
    GtkAttachOptions xOptions; ///< The horizontal fill options, one of : GTK_EXPAND, GTK_SHRINK, GTK_FILL
    GtkAttachOptions yOptions; ///< The vertical fill options, one of : GTK_EXPAND, GTK_SHRINK, GTK_FILL
public:

    /** Constructor
    Sets up a table with 2 rows and 2 columns and everything else default. See initialise
    */
    Table(void){
        initialise();
    }

    /** Constructor
    \param rows the number of rows in the table
    \param cols the number of columns in the table
    \param homogeneous (optional) If true, resizes the table cells to the largest widget. Defaults to true.
    \param xOpt (optional) the default GtkAttachOptions for the horizontal fill type. Defaults to GTK_EXPAND.
    \param yOpt (optional) the default GtkAttachOptions for the horizontal fill type. Defaults to GTK_EXPAND.
    \param xPad (optional) the default horizontal padding. Defaults to 0.
    \param yPad (optional) the default vertical padding. Defaults to 0.
    */
    Table(short int rows, short int cols, bool homogeneous=true, GtkAttachOptions xOpt=GTK_EXPAND, GtkAttachOptions yOpt=GTK_EXPAND, uint xPad=0, uint yPad=0){
        initialise(rows, cols, homogeneous, xOpt, yOpt, xPad, yPad);
    }

    /** Initialiser
    The actual table setup method.
    \param rows the number of rows in the table
    \param cols the number of columns in the table
    \param homogeneous (optional) If true, resizes the table cells to the largest widget. Defaults to true.
    \param xOpt (optional) the default GtkAttachOptions for the horizontal fill type. Defaults to GTK_FILL.
    \param yOpt (optional) the default GtkAttachOptions for the horizontal fill type. Defaults to GTK_FILL.
    \param xPad (optional) the default horizontal padding. Defaults to 0.
    \param yPad (optional) the default vertical padding. Defaults to 0.
    */
    void initialise(short int rows=2, short int cols=2, bool homogeneous=true, GtkAttachOptions xOpt=GTK_FILL, GtkAttachOptions yOpt=GTK_FILL, uint xPad=0, uint yPad=0){
        widget=gtk_table_new(rows, cols, homogeneous); // create the table
        setOptions(xOpt, yOpt); // store the default values
        setPadding(xPad, yPad);
        xStart=yStart=0; xEnd=yEnd=1; // default the first placement to the top left
    }

    /** Set whether the table is homogeneous
    \param homogeneous (optional) If true, resizes the table cells to the largest widget.
    */
    void setHomogeneous(bool homogeneous){
        gtk_table_set_homogeneous(GTK_TABLE(widget),homogeneous);
    }

    /** Set the widget expansion options.
    \param xOpt (optional) the default GtkAttachOptions for the horizontal fill type.
    \param yOpt (optional) the default GtkAttachOptions for the horizontal fill type.
    */
    void setOptions(GtkAttachOptions xOpt, GtkAttachOptions yOpt){
        xOptions=xOpt; // store the default values
        yOptions=yOpt;
    }

    /** Selects the region of the table to load the widget into.
    \param x_s the starting column to place the widget in.
    \param x_e the end column to place the widget in.
    \param y_s the starting row to place the widget in.
    \param y_e the end row to place the widget in.
    */
    void setRegion(short int x_s, short int x_e, short int y_s, short int y_e){
        xStart=x_s; xEnd=x_e;
        yStart=y_s; yEnd=y_e;
    }

    /** Places a widget into the table.
    \param obj the widget to palce.
    \param xOpt The GtkAttachOptions for horizonal fill.
    \param yOpt The GtkAttachOptions for vertical fill.
    */
    void placeWidget(GtkWidget *obj, GtkAttachOptions xOpt,  GtkAttachOptions yOpt){
        xOptions=xOpt; yOptions=yOpt;
        placeWidget(obj);
    }

    /** Places a widget into the table.
    The options, start, end, padding parameters have previously been set, see setRegion, setPadding and placeWidget
    Upon placement, it steps to the next column (right) to auto update the xStart, xEnd, yStart and yEnd variables.
    If the end of the row has been reached, the next row column 1 is set (down one row, left side).
    If you placeWidget to the last cell (bottom right) it will auto-wrap to the top left.
    This allows you to load in many widgets one after another and auto-shifts to the next ` cell.
    \param obj the widget to palce.
    */
    void placeWidget(GtkWidget *obj){
        gtk_table_attach(GTK_TABLE(widget), obj, xStart, xEnd, yStart, yEnd, xOptions, yOptions, xPadding, yPadding);
        // step to the next cell
        // check we haven't exceeded our boundaries.
        uint rows, cols; // update the table size
        getSize(&rows, &cols);
        xEnd++;
        if (++xStart==(int)cols){
            xStart=0; xEnd=1;
            yEnd++;
            if (++yStart==(int)rows){
                yStart=0; yEnd=1;
            }
        }
    }

    /** Get the row and column count of the table
    \param[out] rows The pointer to the uint to set the row count to
    \param[out] cols The pointer to the uint to set the column count to
    */
    void getSize(uint *rows, uint *cols){
        if ((gtk_major_version<=2) & (gtk_minor_version<22))
             g_object_get (GTK_TABLE(widget), "n-rows", rows, "n-columns", cols, NULL);
        else
            gtk_table_get_size(GTK_TABLE(widget), rows, cols);
    }

    /** Sets the ammount of padding to use when placeWidget is called.
    \param xPad the default horizontal padding.
    \param yPad the default vertical padding.
    */
    void setPadding(uint xPad, uint yPad){
        xPadding = xPad; yPadding = yPad;
    }

    /** Resizes the table
    \param rows The number of rows to resize to > 0
    \param cols The number of rows to resize to > 0
    */
    void resize(uint rows, uint cols){
        gtk_table_resize(GTK_TABLE(widget), rows, cols);
    }

    /** Operator to set the location of the next widget to load.

    This must be used like so :
    \code
    Table t; t<<1, 2, 1, 2;  // xStart, xEnd, yStart, yEnd
    \endcode
    \param i The new region point, must be specified in this order : {xStart, xEnd, yStart, yEnd}
    \return CommaRegionSpec for further region or widget or LinkedList operator handling.
    */
    Table &operator<<(int i){
        if (regionTemp.size()<4){
            regionTemp.push_back(i);
            if (regionTemp.size()==4){
                setRegion(regionTemp[0], regionTemp[1], regionTemp[2], regionTemp[3]);
                regionTemp.clear();
            }
        } else // a complete region is defined, alter the table region
            std::cerr<<"Table region is already specified, try dropping this ',int' variable"<<std::endl;
        return *this;
    }

    /** Operator to set the location of the next widget to load.

    This must be used like so :
    \code
    Table t; t<<1, 2, 1, 2;  // xStart, xEnd, yStart, yEnd
    \endcode
    \param i The new region point, must be specified in this order : {xStart, xEnd, yStart, yEnd}
    \return CommaRegionSpec for further region or widget or LinkedList operator handling.
    */
    Table &operator,(int i){
        return operator<<(i);
    }
    /** Operator to set the location of the next widget to load.
    \param region a pointer to short int array of the following form : {xStart, xEnd, yStart, yEnd}
    \return This Table for further operator handling.
    */
    Table &operator<<(int *region){
        setRegion(region[0], region[1], region[2], region[3]);
        return *this;
    }

    /** Operator to load a widget into the table.
    This call should be preceeded with either operator<<(short int *region) i.e. << {1,2,1,2}
    or setRegion. This preceeding call with setup where to load the widget into the table.
    \param obj The widget to load in the table.
    \return This Table for further operator handling.
    */
    Table &operator<<(GtkWidget *obj){
        placeWidget(obj);
        return *this;
    }

    /*! Pack a LinkList of GtkWidget into the table.
    \param ll A LinkList of GtkWidget* for packing from the first to the last
    \return Returns a Table reference for further processing.
    */
    Table& operator <<(LinkList<GtkWidget *> &ll) {
        ll.grab(1); ll.prev();
        for (int i=0;i<ll.getCount();i++)
            placeWidget(ll.next());
        return *this;
    }

    /*! Operator to pack a Container in this table
    \param b The Container to add.
    \return Returns a Container reference for further processing.
    */
    Table& operator <<(Container& b) {
        return operator<<(b.getWidget());
    }

    /*! Operator to pack a pointed to Container
    \param b A pointer to the Container to add.
    \return Returns a Container reference for further processing.
    */
    Table& operator <<(Container * b) {
        return operator<<(b->getWidget());
    }

    /*! Operator to pack a Widget class
    \param b The widget to add.
    \return Returns a Container reference for further processing.
    */
    Table& operator <<(Widget& b) {
        return operator<<(b.getWidget());
    }

    /*! Operator to pack a Widget pointer
    \param b A pointer to the Widget to add.
    \return Returns a Container reference for further processing.
    */
    Table& operator <<(Widget * b) {
        return operator<<(b->getWidget());
    }

    /** Unload the widget from the table
    \param obj The widget to unload from the table
    \return Returns a Table reference for further processing.
    */
    Table &operator>>(GtkWidget *obj){
        gtk_container_remove(GTK_CONTAINER(widget),obj);
        return *this;
    }

    /** Unload the container from the table
    \param c The container to unload from the table
    \return Returns a Table reference for further processing.
    */
    Table &operator>>(Container *c){
        gtk_container_remove(GTK_CONTAINER(widget),c->getWidget());
        return *this;
    }

    /** Unload the container from the table
    \param c The container to unload from the table
    \return Returns a Table reference for further processing.
    */
    Table &operator>>(Container &c){
        gtk_container_remove(GTK_CONTAINER(widget),c.getWidget());
        return *this;
    }

    /*! Unpack a LinkList of GtkWidget from the table.
    \param ll A LinkList of GtkWidget* for packing from the first to the last
    \return Returns a Table reference for further processing.
    */
    Table& operator >>(LinkList<GtkWidget *> &ll) {
        ll.grab(1); ll.prev();
        for (int i=0;i<ll.getCount();i++)
            operator>>(ll.next());
        return *this;
    }
};

#endif // TABLE_H_
