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
#ifndef BOX_H_
#define BOX_H_

#include <iostream>
using namespace std;
#include <gtk/gtk.h>

#include <mffm/LinkList.H>
#include "Container.H"

#ifndef uint
typedef unsigned int uint;
#endif

/*! This class handels the attributes expand and fill of a GTK box
*/
class BoxIS {
    //bool defaultVal=false;
    //#define defaultVal false ///< The default value
    /// The default state of the expand value = BoxIS#defaultVal
    bool expandDefault;
    /// The default state of the fill value = BoxIS#defaultVal
    bool fillDefault;
    /// The default state of the start value = ! BoxIS#defaultVal
    bool startDefault;

    /*! Sets the standard default values
    */
    void setDefaults(void){
        expandDefault=fillDefault=false; //defaultVal; // setup the default behaviour
        startDefault=true;//!defaultVal;

	expand = expandDefault;
	fill = fillDefault;
	start = startDefault;
    }
public:
    /// Set to expand the box contents
    bool expand;
     /// Set to fill the box
    bool fill;
    /// Set to pack the start
    bool start;

    /*! The contructor which sets expand and fill to the BoxIS#defaultVal
    */
    BoxIS(void) {
        setDefaults(); // load up the default values
    }

    /*! The contructor which sets expand and fill and defaults start to TRUE
        \param expandIn The value to set expand to upon construction
        \param fillIn The value to set fill to upon construction
    */
    BoxIS(bool expandIn, bool fillIn) {
        setDefaults(); // load up the default values
        expand=expandIn;
        fill=fillIn;
   }

    /*! The contructor which sets expand, fill and start.
        \param expandIn The value to set expand to upon construction
        \param fillIn The value to set fill to upon construction
        \param startIn The value to set start to upon construction
    */
    BoxIS(bool expandIn, bool fillIn, bool startIn) {
        setDefaults(); // load up the default values
        expand=expandIn;
        fill=fillIn;
        start=startIn;
    }

    /*! Reset the expand and fill values to their defaults
    */
    void reset(void) {
        expand=expandDefault;
        fill=fillDefault;
        start=startDefault;
    }

    /*! Changes the default start value.
        When reset is called, this will become the default.
    */
    void setDefaultStart(bool startIn){
        startDefault=startIn;
        start=startDefault;
    }

    /*! Changes the default expand value.
        When reset is called, this will become the default.
    */
    void setDefaultExpand(bool expandIn){
        expandDefault=expandIn;
        expand=expandDefault;
    }

    /*! Changes the default fill value.
        When reset is called, this will become the default.
    */
    void setDefaultFill(bool fillIn){
        fillDefault=fillIn;
        fill=fillDefault;
    }

    /*! operator to make the box input stream settings equal to a provided object.
    */
    BoxIS& operator=(BoxIS& bis) {
        expand=bis.expand;
        fill=bis.fill;
        start=bis.start;
        return *this;
    }

    /*! operator to make the box input stream settings equal to a provided object.
    */
    BoxIS& operator=(const BoxIS bis) {
        expand=bis.expand;
        fill=bis.fill;
        start=bis.start;
        return *this;
    }

    /*! Dump the values to std out
    */
    void dump(void){
        cout<<"BoxIS::expand="<<expand<<", fill="<<fill<<", start="<<start<<endl;
    }
};

/*/// The default values of expand and fill
bool BoxIS::defaultVal=false;
*/

/*! This class implements the foundation for both horizontal and vertical boxes.

*/
class Box : public BoxIS, public Container {
protected:

    /*! The function called to pack GTK widgets into the box.
        Once called, the default values for expand and fill are set.
        At each run, these values can be altered by using the << BoxIS(expand,fill) operator.
        Post packing, these values are reset.
      \param obj The GtkWidget to pack
    */
    virtual void pack(GtkWidget *obj) {
        if (start)
            gtk_box_pack_start (GTK_BOX (widget), obj, expand, fill, 0);
        else
            gtk_box_pack_end (GTK_BOX (widget), obj, expand, fill, 0);
        //BoxIS::reset(); // reset the starting values
    }
public:

    enum {EXPAND=1, FILL=2, START=4, RESET=8, END=16, NOFILL=32, NOEXPAND=64}; ///< Properties which can be sent in, for example Box()<<Box::RESET

    /** \brief constructor
    */
    Box(void) : BoxIS() {
    }

//    /** \brief destructor destroys the widget to neatly cleanup.
//    */
//    virtual ~Box(void){
//    }

    /*! \brief Operator to adjust the expand and fill values.
        This operator sets the expand and fill to either 0 or 1 and is used as follows << BoxIS(expandVal, fillVal).
        \param bis The box input stream variables to set expand and fill as.
        \return Returns a Box reference for further processing.
    */
    Box& operator<<(BoxIS bis) {
        this->BoxIS::operator=((const BoxIS)bis);
        return *this;
    }

//    /* Align the box : this is a bad idea, as it requires
//    \param a The alignment class
//    \return Returns a Box reference for further processing.
//    */
//    Box& operator<<(Alignment &a) {
//        Box *b=static_cast<Box*>(&a);
//        return *this<<BoxIS(true,true,true)<<b->getWidget();
//    }

    /** Operator to pack a different Box in this Box
    \param b The box to add.
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(Box& b) {
        return Box::operator<<(b.getWidget());
    }

    /*! Operator to change one of the packing values : EXPAND, FILL, START, RESET, END, NOFILL, NOEXPAND

    You can use like so : Box()<<EXPAND|START

    NOTE: Using opposites is undefined, e.g. : Box()<<START|END
    \param val One of : EXPAND, FILL, START, RESET, END, NOFILL, NOEXPAND
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(int val) {
        if (val&RESET)
            BoxIS::reset();
        else {
            if (val&FILL)
                fill=true;
            if (val&EXPAND)
                expand=true;
            if (val&START)
                start=true;
            if (val&NOFILL)
                fill=false;
            if (val&NOEXPAND)
                expand=false;
            if (val&END)
                start=false;
        }
        return *this;
    }

    /*! Operator to pack a pointed to box in this box
    \param b A pointer to the box to add.
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(Box * b) {
        return Box::operator<<(b->getWidget());
    }

    /*! Operator to pack a different box in this box
    \param b The container to add.
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(Container& b) {
        return Box::operator<<(b.getWidget());
    }

    /*! Operator to pack a pointed to box in this box
    \param b A pointer to the container to add.
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(Container * b) {
        return Box::operator<<(b->getWidget());
    }

    /*! Pack a GtkWidget into the box.
    \param b A pointer to the GtkWidget to pack.
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(GtkWidget * b) {
        Container::operator <<(b);
        return *this;
    }

    /*! Pack a GtkWidget into the box.
    \param w A Widget to pack.
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(Widget *w) {
        Container::operator <<(w->getWidget());
        return *this;
    }

    /*! Pack a GtkWidget into the box.
    \param w A Widget to pack.
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(Widget &w) {
        Container::operator <<(w.getWidget());
        return *this;
    }

    /*! Pack a LinkList GtkWidget into the box.
    \param ll A LinkList of GtkWidget* for packing from the first to the last
    \return Returns a Box reference for further processing.
    */
    Box& operator <<(LinkList<GtkWidget *> &ll) {
        Container::operator <<(ll);
        return *this;
    }

    /*! Unpack a widget from the box.
        Note that removal of the widget can destroy it if the only reference is the box.
    \param b The widget to be unpacked from the box.
    \return Returns a Box reference for further processing.
    */
    Box& operator >>(GtkWidget * b) {
        Container::operator>>(b);
        return *this;
    }

    /*! Unpack a box pointer from the box.
        Note that removal of the widget can destroy it if the only reference is the box.
    \param b The pointer to the box to be unpacked from the box.
    \return Returns a Box reference for further processing.
    */
    Box& operator >>(Container * b) {
        Container::operator>>(b);
        return *this;
    }

    /*! Unpack a box from the box.
        Note that removal of the widget can destroy it if the only reference is the box.
    \param b The pointer to the box to be unpacked from the box.
    \return Returns a Box reference for further processing.
    */
    Box& operator >>(Container &b) {
        Container::operator>>(b);
        return *this;
    }

//    /*! Unpack an unknown widget from the box.
//        Note that removal of the widget can destroy it if the only reference is the box.
//    \return Returns the widget removed from the box
//    */
//    GtkWidget* removeUnknownWidget() {
//    GtkWidget* wdgt=gtk_container_get_focus_child((GtkContainer *)this->getWidget());
//    *this>>wdgt;
//    return wdgt;
//    }

    /** Equals operator
    */
    Box& operator=(const Box b){
        //widget=b.getWidget();
        Container::operator=(b);
        BoxIS::operator=(b);
        return *this;
    }

};

/*! A vertical box container.
*/
class VBox : public Box {
public:
    /*! The VBox constructor
    */
    VBox() {
#if GTK_MAJOR_VERSION==2
        widget = gtk_vbox_new (FALSE, 0);
#else
        widget = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#endif
    }
};

/*! A horizontal box container.
*/
class HBox  : public Box {
public:
    /*! The HBox constructor
    */
    HBox() {
#if GTK_MAJOR_VERSION==2
        widget = gtk_hbox_new (FALSE, 0);
#else
        widget = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
#endif
    }
};

/*! A horizontal button box container.
*/
class HBBox : public HBox {
public:
    /*! The HBBox constructor
    */
    HBBox() {
#if GTK_MAJOR_VERSION==2
        widget = gtk_hbutton_box_new ();
#else
        widget = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
#endif
    }
};

/*! A vertical button box container.
*/
class VBBox : public VBox {
public:
    /*! The VBBox constructor
    */
    VBBox() {
#if GTK_MAJOR_VERSION==2
        widget = gtk_vbutton_box_new ();
#else
        widget = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#endif
    }
};
#endif //BOX_H_
