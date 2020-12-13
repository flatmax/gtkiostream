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
#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <iomanip>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <mffm/LinkList.H>
#include <string>
using namespace std;

#ifdef _MSC_VER
#pragma warning( disable : 4005 4554)
#endif

/** \brief class to pass using connectSignalAfter for connecting a signal to a widget
*/
class AnyFuncData {
public:
    AnyFuncData(void) {
        str=NULL;
        func=NULL;
        data=NULL;
    }
    //AnyFuncData(char *s, GCallback f, gpointer d){str=s; func=f; data=d;}
    AnyFuncData(const string &s, GCallback f, gpointer d) {
        str=s.c_str();
        func=f;
        data=d;
    }
    AnyFuncData(const char *s, GCallback f, gpointer d) {
        str=s;
        func=f;
        data=d;
    }
    //AnyFuncData(char *s, void (*f)(void *, void *), gpointer d){str=s; func=reinterpret_cast<GCallback>(f); data=d;}
    AnyFuncData(const char *s, void (*f)(void *, void *), gpointer d) {
        str=s;
        func=reinterpret_cast<GCallback>(f);
        data=d;
    }
    const char *str; ///< The label to show in the button
    void (*func)(); ///< The function to call when the button is pressed
    gpointer data;  ///< The data to pass to func when the button it pressed
};
#define ANYSTRUCT(s,f,d) AnyFuncData(s,f,d)
#define BUTTONANYSTRUCT(s,f,d) AnyFuncData(s,f,d)

/** \brief class to pass using operator<< for creating a label button
*/
class LabelFuncData : public AnyFuncData {
public:
    LabelFuncData(void) : AnyFuncData() {}
    LabelFuncData(const string &s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    LabelFuncData(const char *s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    LabelFuncData(const char *s, void (*f)(void *, void *), gpointer d) : AnyFuncData(s, f, d) {}
};
#define BUTTONLABELSTRUCT(s,f,d) LabelFuncData(s,f,d) // visual C++ doesn't support compound literals

/// Used to create a PIXMAP for the button
#define GDKPIXMAPCREATE(win, ref) gdk_pixmap_colormap_create_from_xpm_d(gtk_widget_get_window(win),gtk_widget_get_colormap(win),NULL,NULL,ref)
/// Used to create a PIXMAP for the button
#define GTKPIXMAPCREATE(pixGdk) (GtkPixmap*)gtk_image_new_from_pixmap(pixGdk, NULL)
/** \brief class to pass using operator<< for creating a regular pixmap on a button
*/
class PixmapFuncData  : public AnyFuncData {
public:
    PixmapFuncData(const char **p, void (*f)(void *, void *), gpointer d) : AnyFuncData(NULL, f, d) {
        pixGtk=p;
    }
    PixmapFuncData(const char **p, GCallback f, gpointer d) : AnyFuncData(NULL, f, d) {
        pixGtk=p;
    }
    PixmapFuncData(void) : AnyFuncData() {
        pixGtk=NULL;
    }
    const char **pixGtk; ///< The pixmap to show
};
#define BUTTONPIXMAPSTRUCT(p,f,d) PixmapFuncData(p,f,d)

/** \brief class to pass for creating a transparent XPM overlay on a button
*/
class PixmapFuncDataTransparent : public AnyFuncData {
public:
    PixmapFuncDataTransparent(const char **i, void (*f)(void *, void *), gpointer d) : AnyFuncData(NULL, f, d) {
        imageXPM=i;
    }
    PixmapFuncDataTransparent(const char **i, GCallback f, gpointer d) : AnyFuncData(NULL, f, d) {
        imageXPM=i;
    }
    PixmapFuncDataTransparent(void) : AnyFuncData() {
        imageXPM=NULL;
    }
    const char **imageXPM; ///< The pointer to the image for overlaying on the button
};
#define BUTTONPIXMAPSTRUCTTRANSPARENT(i,f,d) PixmapFuncDataTransparent(i,f,d)

/** \brief class to pass using operator<< for creating a toggle button with a label
*/
class ToggleFuncData : public AnyFuncData {
public:
    ToggleFuncData(void) : AnyFuncData() {}
    ToggleFuncData(const string &s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    ToggleFuncData(const char *s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    ToggleFuncData(const char *s, void (*f)(void *, void *), gpointer d) : AnyFuncData(s, f, d) {}
};
#define BUTTONTOGGLESTRUCT(s,f,d) ToggleFuncData(s,f,d)

/** \brief class to pass using operator<< for creating an arrow button
*/
class ArrowFuncData : public AnyFuncData {
public:
    ArrowFuncData(void) : AnyFuncData() {
        whichArrow=(GtkArrowType)0;
        whichShadow=(GtkShadowType)0;
    }
    ArrowFuncData(void (*f)(void *, void *), gpointer d, GtkArrowType at, GtkShadowType sh, GtkWidget *a) : AnyFuncData(NULL, f, d) {
        whichArrow=at;
        whichShadow=sh;
    }
    ArrowFuncData(GCallback f, gpointer d, GtkArrowType at, GtkShadowType sh, GtkWidget *a) : AnyFuncData(NULL, f, d) {
        whichArrow=at;
        whichShadow=sh;
    }
    GtkArrowType whichArrow; ///< The type of arrow to create {GTK_ARROW_UP, GTK_ARROW_DOWN, GTK_ARROW_LEFT, GTK_ARROW_RIGHT, GTK_ARROW_NONE}
    GtkShadowType whichShadow; ///< The shadow to use {GTK_SHADOW_NONE, GTK_SHADOW_IN, GTK_SHADOW_OUT, GTK_SHADOW_ETCHED_IN, GTK_SHADOW_ETCHED_OUT}
};
#define BUTTONARROWSTRUCT(f,d,a,sh) arrowFuncData(f,d,a,sh)

/** \brief class to pass using operator<< for creating a radio button with a label
*/
class RadioFuncData : public AnyFuncData  {
public:
    RadioFuncData(void) : AnyFuncData() {}
    RadioFuncData(const string &s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    RadioFuncData(const char *s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    RadioFuncData(const char *s, void (*f)(void *, void *), gpointer d) : AnyFuncData(s, f, d) {}
};
#define BUTTONRADIOSTRUCT(s,f,d) RadioFuncData(s,f,d)

/** \brief class to pass using operator<< for creating a check button with a label
*/
class CheckFuncData : public AnyFuncData {
public:
    CheckFuncData(void) : AnyFuncData() {}
    CheckFuncData(const string &s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    CheckFuncData(const char *s, GCallback f, gpointer d) : AnyFuncData(s, f, d) {}
    CheckFuncData(const char *s, void (*f)(void *, void *), gpointer d) : AnyFuncData(s, f, d) {}
};
#define BUTTONCHECKSTRUCT(s,f,d) CheckFuncData(s,f,d)

/** \brief Class to setup buttons whic are either text, image or toggle types.
Buttons are defined in terms of their callbacks functions and data as well as their type (image, text or toggle)
You stream in any new buttons you want to create, for example :
\code
Buttons buttons << BUTTONLABELSTRUCT {"display text", callbackFunc, (void *)dataVariable};
\endcode
This will create a button with the display text, when pressed, the callbackFunc will be called with the dataVariable as a variable.
You can add buttons to your window like so :
\code
// create 2 buttons
Buttons buttons << BUTTONLABELSTRUCT {"button1", callbackFunc1, (void *)dataVariable1} << BUTTONLABELSTRUCT {"button2", callbackFunc2, (void *)dataVariable2};
HBox box; // A horizontal box to show the buttons in
for (int i=0;i<buttons.getCount();i++) // add the buttons to the box
    box << buttons.next();
box.show(); // make sure the box is not hidden
\endcode
*/
class Buttons : public LinkList<GtkWidget *> {
public:

    /** \brief Constructor
    */
    Buttons(void) {
#if GTK_MAJOR_VERSION==2
        gdk_colormap_new(gdk_visual_get_best(),1);
#endif
    }

    /** Set the alignment within the button.
    Operates on the GtkWidget* which is LinkList::current - i.e. currently pointed to
    \param xAlign the x-alignment 0.0 is a left alignment, 1.0 is for right alignment
    \param yAlign the y-alignment 0.0 is a left alignment, 1.0 is for right alignment
    */
    void setAlignment(float xAlign, float yAlign) {
        gtk_button_set_alignment(GTK_BUTTON(current()), xAlign, yAlign);
        gtk_widget_queue_draw(current());
    }

    /** Get the alignment within the button.
    Operates on the GtkWidget* which is LinkList::current - i.e. currently pointed to
    Fills the pointers to the floats with the alignment settings.
    \param xAlign the x-alignment 0.0 is a left alignment, 1.0 is for right alignment
    \param yAlign the y-alignment 0.0 is a left alignment, 1.0 is for right alignment
    */
    void getAlignment(float *xAlign, float *yAlign) {
        gtk_button_get_alignment(GTK_BUTTON(current()), xAlign, yAlign);
    }

//    /** \brief Destructor
//    */
//    ~Buttons(void){
//        for (int i=1; i<=getCount(); i++)
//            if (g_object_is_floating(current()))
//                delete remove();
//    }
//
    /** \brief connect a callback to execute after the currently connected callbacks
    \param nf labelFuncData matching the callback to connect
    */
    void signalConnectAfter(LabelFuncData nf) {
        if (nf.func)
            g_signal_connect_after(G_OBJECT((GtkButton*)current()), "released",reinterpret_cast<GCallback>(nf.func), nf.data);
    }

    /** \brief connect a callback to execute after the currently connected callbacks
    \param af anyFuncData matching the callback to connect
    */
    void signalConnectAfter(const AnyFuncData &af) {
        if (af.func)
            g_signal_connect_after(G_OBJECT((GtkButton*)current()), "released", af.func, af.data);
    }

    /** \brief disconnect the old callback and reconnect a new callback
    \param nfOld labelFuncData matching the old callback to be removed
    \param nfNew labelFuncData matching the new to be connected callback
    */
    void signalReplace(LabelFuncData nfOld, LabelFuncData nfNew) {
        if (nfOld.func)
            g_signal_handlers_disconnect_by_func(G_OBJECT((GtkButton*)current()), (void *)(nfOld.func), nfOld.data);
        if (nfNew.func)
            g_signal_connect(G_OBJECT((GtkButton*)current()), "released",reinterpret_cast<GCallback>(nfNew.func), nfNew.data);
    }


    /** \brief disconnect the old callback and reconnect a new callback
    \param nfOld AnyFuncData matching the old callback to be removed
    */
    void signalRemove(AnyFuncData nfOld) {
        if (nfOld.func) {
            g_signal_handlers_disconnect_by_func(G_OBJECT((GtkButton*)current()), (void *)(nfOld.func), nfOld.data);
        }
    }

    /** \brief generate a label button
    \param nf The label, callback function and data to show.
    \return This Buttons class for further overloading
    */
    Buttons& operator <<(LabelFuncData nf) {
        if (nf.str)
            LinkList<GtkWidget *>::add(gtk_button_new_with_label(nf.str));
        else
            LinkList<GtkWidget *>::add(gtk_button_new());

        if (nf.func)
            g_signal_connect(G_OBJECT((GtkButton*)current()), "released",reinterpret_cast<GCallback>(nf.func), nf.data);
        gtk_widget_show(current());
        return *this;
    }

    /** \brief generate a button with a static pixmap
    \param nf The pixmap, callback function and data to show.
    \return This Buttons class for further overloading
    */
    Buttons& operator <<(const PixmapFuncData nf) {
        LinkList<GtkWidget *>::add(gtk_button_new());
        g_signal_connect_after(current(), "expose-event", G_CALLBACK(exposePixmap), (gpointer)nf.pixGtk);

        if (nf.func)
            g_signal_connect(G_OBJECT(current()), "released",reinterpret_cast<GCallback>(nf.func), nf.data);
        gtk_widget_show(current());
        return *this;
    }

    /** \brief generate a toggle button with label
    \param nf The label, callback function and data to show.
    \return This Buttons class for further overloading
    */
    Buttons& operator <<(ToggleFuncData nf) {
        LinkList<GtkWidget *>::add(gtk_toggle_button_new_with_label(nf.str));
        // working on insensitive colour modifications ... for later.
//        #if GTK_MAJOR_VERSION==2
//////            GdkColor yellow = { 0, 0xFFFF, 0xFFFF, 0x0000 };
//////            gtk_widget_modify_bg(current(), GTK_STATE_NORMAL, &yellow);
////            GdkColor white = { 0xfff, 0xFFF, 0xFFF, 0xFFF };
////            gtk_widget_modify_bg(current(), GTK_STATE_ACTIVE, &white);
//
//        #else
//            GdkRGBA yellow =  { 1., 1., 0., 0.5 };
//            gtk_widget_override_background_colorcurrent(), GTK_STATE_FLAG_INCONSISTENT, &yellow);
//        #endif

        if (nf.func)
            g_signal_connect(G_OBJECT((GtkButton*)current()), "released",reinterpret_cast<GCallback>(nf.func), nf.data);
        gtk_widget_show(current());
        return *this;
    }

    /** \brief generate a button with a pixmap which is partially transparent
    \param pfdt The pixmap, callback function and data to show.
    \return This Buttons class for further overloading
    */
    Buttons& operator <<(const PixmapFuncDataTransparent pfdt) {
        LinkList<GtkWidget *>::add(gtk_button_new());

        g_signal_connect_after(current(), "expose-event", G_CALLBACK(exposePixmap), (gpointer)pfdt.imageXPM);

        if (pfdt.func)
            g_signal_connect(G_OBJECT(current()), "released",reinterpret_cast<GCallback>(pfdt.func), pfdt.data);
        gtk_widget_show(current());
        return *this;
    }

    /** \brief generate an arrow button
    If the arrow widget is supplied in the ArrowFuncData class, then it will be used instead of generating a new one.
    See arroFuncData for the various class member variables which are used in this method.
    \param af The callback function, data, arrow type, shadow type and possibly a pre-existing arrow to use/show. If the pre-existing arrow is to be used, then the ArrowFuncData.whichArrow and ArrowFuncData.whichShdow variables are ignored.
    \return This Buttons class for further overloading
    */
    Buttons& operator <<(ArrowFuncData af) {
        LinkList<GtkWidget *>::add(gtk_button_new());
        GtkWidget *arrow=gtk_arrow_new(af.whichArrow,af.whichShadow);
        gtk_container_add (GTK_CONTAINER (current()), arrow);
        gtk_widget_show(arrow);

        if (af.func)
            g_signal_connect(G_OBJECT((GtkButton*)current()), "released",reinterpret_cast<GCallback>(af.func), af.data);
        gtk_widget_show_all(current());
        return *this;
    }

    /** \brief generate a radio button with label
    If other radio buttons are present, then they are all added to the same group
    \param nf The label, callback function and data to show.
    \return This Buttons class for further overloading
    */
    Buttons& operator <<(RadioFuncData nf) {
        if (!LinkList<GtkWidget *>::getCount()) // this is the first
            LinkList<GtkWidget *>::add(gtk_radio_button_new_with_label(NULL,nf.str));
        else
            LinkList<GtkWidget *>::add(gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (current()), nf.str));
        if (nf.func)
            g_signal_connect(G_OBJECT((GtkButton*)current()), "released",reinterpret_cast<GCallback>(nf.func), nf.data);
        gtk_widget_show(current());
        return *this;
    }

    /** \brief generate a check button with label
    \param nf The label, callback function and data to show.
    \return This Buttons class for further overloading
    */
    Buttons& operator <<(CheckFuncData nf) {
        if (nf.str)
            LinkList<GtkWidget *>::add(gtk_check_button_new_with_label(nf.str));
        else
            LinkList<GtkWidget *>::add(gtk_check_button_new());

        if (nf.func)
            g_signal_connect(G_OBJECT((GtkButton*)current()), "released",reinterpret_cast<GCallback>(nf.func), nf.data);
        gtk_widget_show(current());
        return *this;
    }

    /** \brief overlay the pixmap image onto the button whenever required
    This method allows transparent XPMs to be displayed on buttons.
    Whenever the background is changed, the transparent pixmap is re-drawn
    \brief widget The button to re-draw
    \brief event the event (we are looking for an expose event)
    \brief data The XPM pointer (char**) to use for display
    \return false to keep propagating the event
    */
    static gboolean exposePixmap(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
        GdkPixbuf *tipGDKPixBuf = gdk_pixbuf_new_from_xpm_data(static_cast<const char**>(data)); // get the pixbuf
        int width = gdk_pixbuf_get_width(tipGDKPixBuf), height = gdk_pixbuf_get_height(tipGDKPixBuf);
        gint allocWidth, allocHeight, allocX;//, allocY;
#if GTK_MAJOR_VERSION==2
        allocX=widget->allocation.x;
        allocWidth=widget->allocation.width;
        allocHeight=widget->allocation.height;
        //allocY=widget->allocation.y;
#else
        GtkAllocation widgetAlloc;
        gtk_widget_get_allocation(widget, &widgetAlloc);
        allocX=widgetAlloc.x;
        allocWidth=widgetAlloc.width;
        allocHeight=widgetAlloc.height;
        //allocY=widgetAlloc.y;
#endif
        if ((allocWidth<width) | (allocHeight<height))
            gtk_widget_set_size_request(widget, width, height); // resize the button to match  the pixmap
        else {
            cairo_t  *cr=gdk_cairo_create(gtk_widget_get_window(widget)); // get a cairo context for rendering

            // use alignment to draw appropriately
            float xAlign, yAlign;
            gtk_button_get_alignment(GTK_BUTTON(widget), &xAlign, &yAlign);
            int xOffset=(int)((float)(allocWidth-width-1)*xAlign);
            if (xOffset<0) xOffset=0;
            int yOffset=(int)((float)(allocHeight-height-1)*yAlign);
            if (yOffset<0) yOffset=0;

            gdk_cairo_set_source_pixbuf(cr, tipGDKPixBuf,  allocX+xOffset,  allocX+yOffset); // set the source to the pixbuf at the correct location
            cairo_paint(cr); // paint it to the cairo context
            cairo_destroy(cr); // destroy the context
        }
        return FALSE;
    }

    /** Set the current botton's label's font
    For example :
    \code
    #include "Pango.H"

    Font font("bold"); // could be something else, like "Sans 12"

    Buttons button<<BUTTONLABELSTRUCT("I want this to be bold", (void (*)(void *, void *))buttonPressed, (void *)this);
    button.setLabelsFont(font.getPangoFontDescription());
    \endcode
    */
    void setLabelsFont(PangoFontDescription *pangoFontDescription) {
        if (pangoFontDescription) {
            GtkWidget *child=gtk_bin_get_child(GTK_BIN(current()));
            if (child)
                gtk_widget_modify_font(child,pangoFontDescription);
        }
    }

    /** Set the current() button's label.
    \param l the label the set on the button.
    */
    void setLabel(const char*l) {
        gtk_button_set_label(GTK_BUTTON(current()),l);
    }

    /** Get the label associated with the button.
    \return The label, if none, then NULL.
    */
    const gchar *getLabel() {
        return gtk_button_get_label(GTK_BUTTON(current()));
    }

    /** Find the GTK widget index (from 1) in the list of buttons
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
        //std::cout<<"Buttons::findIndex = "<<ret<<std::endl;
        return ret;
    }

    /** Get the widget at index (starting from 1)
    \param index the widget at this index to grab (starting from 1)
    \return The widget at index, or NULL on error
    */
    GtkWidget *findWidget(int index) {
        GtkWidget *retVal=NULL;
        if (index<getCount() && index>0)
            retVal=grab(index);
        return retVal;
    }

    /** Method to set the currently selected button to active.
    Only works for toggle buttons (toggle, checkbox, etc.).
    \param wid The toggle button to query for its active state.
    \param state The state of the button
    */
    static void setActive(GtkWidget *wid, bool state) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wid), state);
    }

    /** Method to set the currently selected button to active.
    Only works for toggle buttons (toggle, checkbox, etc.).
    \param state The state of the button
    */
    void setActive(bool state) {
        if (getCount())
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(current()), state);
    }

    /** Method to get the currently selected button's active state.
    Only works for toggle buttons (toggle, checkbox, etc.).
    \return the current state of the button, or false if no buttons exist
    */
    bool getActive(void) {
        if (getCount())
            return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(current()));
        return false;
    }

    /** Method to get an arbitrary button's active state.
    Only works for toggle buttons (toggle, checkbox, etc.).
    \param wid The toggle button to query for its active state.
    \return the current state of the button, or false if no buttons exist
    */
    static bool getActive(GtkWidget *wid) {
        return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(wid));
    }

    /** Finds the active button.
    Only works for toggle buttons (toggle, checkbox, etc.).
    \return the index of the button which is active, or 0 if none active.
    */
    int findActiveButtonIndex() {
        grab(1);
        for (int i=1; i<=getCount(); i++)
            if (getActive())
                return i;
            else
                next();
        return 0;
    }
};
#endif //BUTTONS_H_
