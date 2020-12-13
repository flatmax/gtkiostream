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
#ifndef DIALOG_H_
#define DIALOG_H_

#include "Container.H"

/**

*/
class Dialog : public Container {
public:
    /** A dialog which defaults to 'other' type with no buttons
    \param parent The parent window to show (default=NULL)
    \param flags which control the behaviour GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT (default), GTK_DIALOG_NO_SEPARATOR
    \param whichType the type of dialog which controls the icon (default=GTK_MESSAGE_OTHER)
    \param buttonsType The buttons which are shown (default=GTK_BUTTONS_NONE)
    \param text The text to show (default=NULL)
    */
    Dialog() {
        widget=gtk_dialog_new();
    }

    /** Destructor
    */
    virtual ~Dialog() {
    }

    /** run the dialog
    */
    void run(void) {
        gtk_dialog_run(GTK_DIALOG(widget));
    }

    /** Set the title
    \param text the title to show
    */
    void title(char *text) {
        string t(text);
        title(t);
    }

    /** Set the title
    \param text the title to show
    */
    void title(string &text) {
        gtk_window_set_title(GTK_WINDOW(widget), text.c_str());
    }

};

#endif // DIALOG_H_
