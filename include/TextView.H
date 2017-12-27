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
#ifndef TEXTVIEW_H_
#define TEXTVIEW_H_

#include "Container.H"
#include <string>
#include <sstream>

/** TextView

Simple TextView widget.

*/
class TextView : public Container {
public:
    /** Constructor
    */
    TextView(void){
        widget=gtk_text_view_new (); // create the entry box
    }

    /** Set the font
    For example :
    \code
    #include "Pango.H"

    Font font("bold"); // could be something else, like "Sans 12"

    TextView textView;
    textView.setLabelsFont(font.getPangoFontDescription());
    \endcode
    */
    void setFont(PangoFontDescription *pangoFontDescription){
        if (pangoFontDescription)
            gtk_widget_modify_font(widget,pangoFontDescription);
    }

    /** Connect a callback to capture when the text has changed in the text view  buffer
    */
    void connectBufferChangedSignal(GCallback callBack, void *data){
        //g_signal_connect(widget, "insert-at-cursor", callBack, data);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(widget));
        g_signal_connect(GTK_WIDGET(buffer), "changed", callBack, data);
    }

    /** Set the buffer text, deleting any text which is present.
    \param text The string to insert.
    */
    void setText(string &text){
        setText(text.c_str());
    }

    /** Set the buffer text, deleting any text which is present.
    \param text The text to insert. It must be null terminated.
    */
    void setText(const char *text){
        GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
        gtk_text_buffer_set_text(buffer,text,-1);
//        gtk_widget_queue_draw(widget);
    }

    /** Get the buffer's text.
    \return text The text to insert. It must be null terminated.
    */
    string getText(void){
        string text;
        GtkTextIter start, end;
        bool includeInvisible=true;
        char *text_;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
        gtk_text_buffer_get_bounds (buffer, &start, &end);
        text_ = gtk_text_buffer_get_text(buffer, &start, &end, includeInvisible);
        text=text_;
        g_free(text_);
        return text;
    }

    /** Operator to insert text
    \param text the text to insert
    \return a reference to this class
    */
    TextView &operator<<(string &text){
        setText(text);
        return *this;
    }

    /** Operator to insert text
    \param text the text to insert. Must be null terminated.
    \return a reference to this class
    */
    TextView &operator<<(const char *text){
        setText(text);
        return *this;
    }

    /** Operator to get text.
    \param text the text to fill with the content of the TextView buffer.
    \return a reference to this class
    */
    TextView &operator>>(string &text){
        text=getText();
        return *this;
    }

    /** Operator to get an integer.
    \param num the int to fill with the content of the TextView buffer.
    \return a reference to this class
    */
    TextView &operator>>(int &num){
        string text=getText();
        int isHex=(text.find("0x")==string::npos)?-1:text.find("0x")+2;

        if (isHex!=-1){
            istringstream iss(text.substr(isHex,text.size()-isHex));
            iss>>hex>>num;
        } else {
            istringstream iss(text);
            iss>>num;
        }

        return *this;
    }

    /** Operator to get an integer.
    \param num the long to fill with the content of the TextView buffer.
    \return a reference to this class
    */
    TextView &operator>>(long &num){
        string text=getText();
        int isHex=(text.find("0x")==string::npos)?-1:text.find("0x")+2;

        if (isHex!=-1){
            istringstream iss(text.substr(isHex,text.size()-isHex));
            iss>>hex>>num;
        } else {
            istringstream iss(text);
            iss>>num;
        }

        return *this;
    }

    /** Operator to get an arbitrary type.
    \param var the var to fill with the content of the TextView buffer.
    \return a reference to this class
    \tparam The type to format the TextView buffer contents to.
    */
    template<typename TYPE>
    TextView &operator>>(TYPE &var){
        string text=getText();

        istringstream iss(text);
        iss>>var;
        return *this;
    }
};

#endif // TEXTVIEW_H_
