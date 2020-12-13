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
#ifndef MESSAGEDIALOG_H_
#define MESSAGEDIALOG_H_

#include "Container.H"
#include "Pango.H"

/**
The default message dialog which displays either text or Pango.

Here is an example of a MessageInfo dialog using Pango :
\image html MessageInfo.pango.jpg

See MessageInfo, MessageError, MessageQuestion and MessageWarning for other examples.

*/
class MessageDialog : public Container {
public:
    /** A dialog which defaults to 'other' type with no buttons
    \param parent The parent window to show (default=NULL)
    \param flags which control the behaviour GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT (default), GTK_DIALOG_NO_SEPARATOR
    \param whichType the type of dialog which controls the icon (default=GTK_MESSAGE_OTHER)
    \param buttonsType The buttons which are shown (default=GTK_BUTTONS_NONE)
    \param text The text to show (default=NULL)
    */
    MessageDialog(GtkWidget *parent, GtkDialogFlags flags, GtkMessageType whichType, GtkButtonsType buttonsType, const char *text) {
        widget=gtk_message_dialog_new(GTK_WINDOW(parent), flags, whichType, buttonsType, "%s", text);
    }

    /** A dialog which defaults to 'other' type with no buttons
    \param parent The parent window to show (default=NULL)
    \param flags which control the behaviour GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT (default), GTK_DIALOG_NO_SEPARATOR
    \param whichType the type of dialog which controls the icon (default=GTK_MESSAGE_OTHER)
    \param buttonsType The buttons which are shown (default=GTK_BUTTONS_NONE)
    \param text The text to show (default=NULL)
    */
    MessageDialog(GtkWidget *parent, GtkDialogFlags flags, GtkMessageType whichType, GtkButtonsType buttonsType, string &text) {
        widget=gtk_message_dialog_new(GTK_WINDOW(parent), flags, whichType, buttonsType, "%s", text.c_str());
    }

    /** A dialog using Pango
    \param parent The parent window to show (default=NULL)
    \param flags which control the behaviour GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT (default), GTK_DIALOG_NO_SEPARATOR
    \param whichType the type of dialog which controls the icon (default=GTK_MESSAGE_OTHER)
    \param buttonsType The buttons which are shown (default=GTK_BUTTONS_NONE)
    \param pango The pango markup to show
    */
    MessageDialog(GtkWidget *parent, GtkDialogFlags flags, GtkMessageType whichType, GtkButtonsType buttonsType, Pango &pango) {
        // the following has tring literal warning messages
        //widget=gtk_message_dialog_new_with_markup(GTK_WINDOW(parent), flags, whichType, buttonsType, pango.getMarkup().c_str());
        widget=gtk_message_dialog_new(GTK_WINDOW(parent), flags, whichType, buttonsType, NULL);
        gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(widget), pango.getMarkup().c_str());
    }

    /** Destructor
    */
    virtual ~MessageDialog() {
        gtk_widget_destroy(widget);
    }

    /** Load the string into the dialog
    \param str the string to show
    \return A reference to this instance of the class
    */
    MessageDialog& operator<<(char *str) {
        return operator<<(string(str));
    }

    /** Load the char* string into the dialog
    \param str the string to show
    \return A reference to this instance of the class
    */
    MessageDialog& operator<<(string str) {
        Pango pango;
        pango<<str;
        return operator<<(pango);
    }

    /** Set the box text to the Pango markup
    \param pango the Pango markup to use
    \return A reference to this instance of the class
    */
    MessageDialog& operator<<(Pango &pango) {
        gtk_message_dialog_set_markup(GTK_MESSAGE_DIALOG(widget), pango.getMarkup().c_str());
        cout<<"created"<<endl;
        return *this;
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

/** MessageDialog for displaying info from text or Pango
\image html MessageInfo.jpg
*/
class MessageInfo : public MessageDialog {
public:
    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageInfo(GtkWidget *win, char *text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageInfo(char *text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, text) {
    }

    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageInfo(GtkWidget *win, string text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageInfo(string text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, text) {
    }

    /** Show Pango marked up text with a parent window
    \param win The parent window
    \param pango The marked up text to show
    */
    MessageInfo(GtkWidget *win, Pango &pango) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, pango) {
    }

    /** Show Pango marked without associating a parent window
    \param pango The marked up text to show
    */
    MessageInfo(Pango &pango) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, pango) {
    }
};

/** MessageDialog for displaying errors from text or Pango
\image html MessageError.jpg
*/
class MessageError : public MessageDialog {
public:
    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageError(GtkWidget *win, char *text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageError(char *text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, text) {
    }
    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageError(GtkWidget *win, string text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageError(string text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, text) {
    }
    /** Show Pango marked up text with a parent window
    \param win The parent window
    \param pango The marked up text to show
    */
    MessageError(GtkWidget *win, Pango &pango) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, pango) {
    }
    /** Show Pango marked without associating a parent window
    \param pango The marked up text to show
    */
    MessageError(Pango &pango) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, pango) {
    }
};

/** MessageDialog for displaying questions from text or Pango
\image html MessageQuestion.jpg
*/
class MessageQuestion : public MessageDialog {
public:
    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageQuestion(GtkWidget *win, char *text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageQuestion(char *text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, text) {
    }
    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageQuestion(GtkWidget *win, string text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageQuestion(string text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, text) {
    }
    /** Show Pango marked up text with a parent window
    \param win The parent window
    \param pango The marked up text to show
    */
    MessageQuestion(GtkWidget *win, Pango &pango) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, pango) {
    }
    /** Show Pango marked without associating a parent window
    \param pango The marked up text to show
    */
    MessageQuestion(Pango &pango) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, pango) {
    }
};

/** MessageDialog for displaying warnings with the message as text or Pango
\image html MessageWarning.jpg
*/
class MessageWarning : public MessageDialog {
public:
    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageWarning(GtkWidget *win, string text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageWarning(string text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, text) {
    }
    /** Show text with a parent window
    \param win The parent window
    \param text The text to show
    */
    MessageWarning(GtkWidget *win, char *text) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, text) {
    }
    /** Show text without associating a parent window
    \param text The text to show
    */
    MessageWarning(char *text) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, text) {
    }
    /** Show Pango marked up text with a parent window
    \param win The parent window
    \param pango The marked up text to show
    */
    MessageWarning(GtkWidget *win, Pango &pango) : MessageDialog(win, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, pango) {
    }
    /** Show Pango marked without associating a parent window
    \param pango The marked up text to show
    */
    MessageWarning(Pango &pango) : MessageDialog(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, pango) {
    }
};

#endif // MESSAGEDIALOG_H_
