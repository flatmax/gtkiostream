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
#ifndef LABELS_H_
#define LABELS_H_

#include <iomanip>
#include <gtk/gtk.h>
#include <mffm/LinkList.H>

#include "Buttons.H"
#include "Pango.H"

#include <string>

/** \brief class to create and handle labels.

Generally you use istream operators as follows :
   \code
Window topWindow; // Create the top box
Labels labels; // create the class
labels<<"label 1"<<"label 2"; // create two labels
VBox vbox; // create a suitable container
vbox<<labels.grab(1); // add "label 1" to the vbox
vbox<<labels.grab(2); // add "label 2" to the vbox
vbox.show(); // show the vbox with the two labels present

topWindow << vBox; // Load the VBox into the top window
   \endcode

You can also do the same thing inline :
    \code
    Window() << (VBox()<<(Labels()<<"label 1"<<"label 2")).show();
    \endcode

   You can have clickable labels like so :
   \code
   Labels::operator<<(BUTTONLABELSTRUCT((char *)"clickable label", (void (*)(void *, void *))labelCallback, (gpointer)this));

   gboolean labelCallback(GtkWidget *widget, GdkEventButton *event, gpointer data){
   	// handle the label callback here.
   }
   \endcode
   */
class Labels : public LinkList<GtkWidget *> {
public:

    /** Create a label from a string
    \param str The string to show as the label
    \return A reference to this Labels class for further overloading
    */
    Labels& operator <<(std::string str) {
        return operator<<(str.c_str());
    }

    /** Create a label from a string
    \param str The char * string to show as the label
    \return A reference to this Labels class for further overloading
    */
    Labels& operator <<(const char *str) {
        LinkList<GtkWidget *>::add(gtk_label_new(str));
        setAlignment(0.0, 0.0);
        gtk_widget_show (current());
        //cout<<"Labels : "<<current()<<endl;
        return *this;
    }

    /** Add a label with an event box.
    This creates a label with a clickable eventBox, which will call a callback.
    \param nf The labelFuncData which specifies the label string, the callback function and the data to pass to the callback function.
     \return A reference to this Labels class for further overloading
    */
    Labels& operator <<(LabelFuncData nf) {
        GtkWidget * eventBox=gtk_event_box_new(); // construct the event box
        LinkList<GtkWidget *>::add(eventBox); // add the eventbox to the list of widgets
        //gtk_event_box_set_above_child(GTK_EVENT_BOX(eventBox), FALSE);
        GtkWidget *label=gtk_label_new(nf.str); // construct the label
        gtk_container_add((GtkContainer *)eventBox,label); // add the label to the eventbox
        if (nf.func)
            g_signal_connect(G_OBJECT(eventBox), "button_press_event", G_CALLBACK(nf.func), nf.data);
        gtk_widget_show(current());
        gtk_widget_show (label);
        return *this;
    }

    /** \brief returns the current GtkWidget* in the list
        \return GtkWidget* The current GtkWidget* in the list
    */
    GtkWidget* getWidget(void) {
//        cout<<"Labels : getWidget "<<current()<<endl;
        return LinkList<GtkWidget *>::current();
    }

    /** \brief Set the pango markup of the currently displayed text.
    Allows you to set the current text to different pango text styles, specified in str.
    Specify only the pango styles here.
    For example, to set the colour to red we require str="foreground='#ffffa5a50000'",
    Which we can generate like so:
    \code
    ColourLineSpec cls="r"; // specify the desired colour
    gchar *gdkCString=gdk_color_to_string(&cls.colour); // generate the equivalent colour specification in a string
    string pangoColour("foreground='"); pangoColour=pangoColour+gdkCString+"'"; // create the string "foreground='#ffffa5a50000'"
    g_free(gdkCString); // free the created colour string
    pangoMarkup(pangoColour.c_str()); // set the text to the correct colour.
    \endcode
    \param str Can be a valid pango markup string. See http://developer.gnome.org/pango/stable/PangoMarkupFormat.html
    */
    void pangoMarkup(const char *str) {
        if (getCount()){
            string pangoString; pangoString=pangoString+"<span "+str+">%s</span>"; // parse the current text into the markup
            char *markup = g_markup_printf_escaped (pangoString.c_str(), gtk_label_get_text(GTK_LABEL(current()))); // generate the required markup
            gtk_label_set_markup(GTK_LABEL(current()), markup);
            g_free(markup);
        }
    }

    /** \brief Set the pango markup of the currently displayed text.
    The Pango class is used to setup the label.
    \param p The pango marked up string to set (see Pango)
    */
    void pangoMarkup(Pango &p) {
        p.setLabelText(current());
    }

    /** Set the alignment of the label for parents.
    \param xalign the x alignment  (0.0 for left, 1.0 for right)
    \param yalign the y alignment  (0.0 for top, 1.0 for bottom)
    \return the current label which was adjusted, NULL if there are no labels
    */
    GtkWidget * setAlignment(float xalign, float yalign){
        if (getCount()){
            gtk_misc_set_alignment (GTK_MISC(current()), xalign, yalign);
            return current();
        }
        return NULL; // if there are no labels then return NULL
    }

    /** Create a label from a Pango markup class
    \param p The Pango class which contains the formatting
    \return A reference to this Labels class for further overloading
    */
    Labels& operator <<(Pango &p) {
        LinkList<GtkWidget *>::add(gtk_label_new(NULL));
        pangoMarkup(p); // set the markup
        gtk_widget_show (current());
        return *this;
    }

    /** Given a PangoFontDescription, set a label's font.
    \code
    Font font("bold");
    Labels lablel<<"Text I want to Bold";
    label.setLabelsFont(font.getPangoFontDescription());
    \endcode
    */
    void setLabelsFont(PangoFontDescription *pangoFontDescription){
        if (pangoFontDescription)
            gtk_widget_modify_font(current(),pangoFontDescription);
    }

    /** Set the label's angle.
    The angle setting for the label is ignored if the label is selectable, wrapped, or ellipsized.
    \param angle An angle of 90 reads from from bottom to top, an angle of 270, from top to bottom.
    */
    void setAngle(double angle){
        if (getCount())
            gtk_label_set_angle(GTK_LABEL(current()), angle);
    }

    /** Set the labels text
    \param text The new text for the label
    */
    void setText(const char*text){
        if (getCount())
            gtk_label_set_text(GTK_LABEL(current()), text);
    }

    /** Set the labels text
    \param text The new text for the label
    */
    const char* getText(void){
        if (getCount())
            return gtk_label_get_text(GTK_LABEL(current()));
    }

	/** Set the labels text
	\param text The new text for the label
	*/
	Labels &operator=(string text){
		setText(text.c_str());
		return *this;
	}

		/** Set the labels text
	\param text The new text for the label
	*/
	Labels &operator=(const char*text){
		setText(text);
		return *this;
	}

#ifdef DEBUG
    ~Labels(void) {
        cout<<"Labels::~Labels"<<endl;
    }
#endif
};
#endif //LABELS_H_
