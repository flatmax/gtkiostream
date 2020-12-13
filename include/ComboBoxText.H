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
#ifndef COMBOBOXTEXT_H_
#define COMBOBOXTEXT_H_

#include <Container.H>
#include <string.h>

/** A combo widget : pull down list
\image html ComboBoxTextTest.jpg
This is a simple to fill pull down list. It is operated like so :
\code
ComboBoxText combo;
combo<<"entry 1"<<"entry 2";
combo<<string("entry 3");
\endcode
\image html ComboBoxTextTest.open.jpg
*/
class ComboBoxText : public Container {
    void init(void) {
#if GTK_MAJOR_VERSION==2 && GTK_MINOR_VERSION<24
        GtkListStore *model = gtk_list_store_new (1, G_TYPE_STRING);
        widget = gtk_combo_box_new_with_model(GTK_TREE_MODEL (model));
        GtkCellRenderer *cell_renderer = gtk_cell_renderer_text_new ();
        gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (widget), cell_renderer, TRUE);
        gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (widget), cell_renderer, "text", 0, NULL);
#else
        widget = gtk_combo_box_text_new();
#endif

    }
public:
    /*! The Combo constructor
    */
    ComboBoxText(void) {
        init();
    }

    /*! The Combo constructor which has a callback associated.
    \param callBack the callback function to execute.
    \param data The user data to pass to the callback function.
    */
    ComboBoxText(GCallback callBack, void *data) {
        init();
        setChangedCallback(callBack, data);
    }

    /** Set the callback for when the combo box is changed
    \param callBack the callback function to execute.
    \param data The user data to pass to the callback function.
    */
    gulong setChangedCallback(GCallback callBack, void *data) {
        return g_signal_connect( G_OBJECT( widget ), "changed" ,G_CALLBACK(callBack), data);
    }

    /** Remove the callback for when the combo box is changed
    \param id the callback id to remove from the event execution list
    */
    void removeChangedCallback(gulong id) {
        return g_signal_handler_disconnect( G_OBJECT( widget ), id);
    }

    /** Get the selected number from the ComboBoxText
    \param value The number of the selected element (from zero)
    */
    void getSelection(int &value) {
        value=gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    }

    /** Get the selected text from the ComboBoxText
    \param value The character array to hold the selected text. NOTE: Ensure the length of the char * passed in is large enough to contain the selected text.
    */
    void getSelection(char *value) {
#if GTK_MAJOR_VERSION==2 && GTK_MINOR_VERSION<24
        gchar *val=gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
#else
        gchar *val=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
#endif
        strcpy(value,val);
        g_free(val);
    }

    /** Get the selected text from the ComboBoxText
    \param value The string to hold the selected text.
    */
    void getSelection(string &value) {
#if GTK_MAJOR_VERSION==2 && GTK_MINOR_VERSION<24
        gchar *val=gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
#else
        gchar *val=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
#endif
        value=val;
        g_free(val);
    }

    /** Get the selected text from the ComboBoxText.
    \param wid The widget to get the selection from.
    \param value The string to hold the selected text.
    */
    static void getSelection(GtkWidget *wid, string &value) {
#if GTK_MAJOR_VERSION==2 && GTK_MINOR_VERSION<24
        gchar *val=gtk_combo_box_get_active_text(GTK_COMBO_BOX(wid));
#else
        gchar *val=gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(wid));
#endif
        value=val;
        g_free(val);
    }

    /** Sets the ComboBoxText to 'which'
    \param which The item to set as actave or 'selected', indexed from zero (0 is the first entry).
    */
    void setSelection(int which) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(widget), which);
    }

    /** Set the selection to the matched char array.
    If the character array text is in the ComboBoxText, then set the ComboBoxText to select that item.
    \param text The character array to search for in the ComboBoxText
    \return 0 on success, -1 if the text is not found
    */
    int setSelection(const char *text) {
        return setSelection(string(text));
    }

    int setSelection(string text) {
        int ret=-1;
        GtkTreeIter iter;
        gboolean valid;
        GtkTreeModel *model=gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
        if (model==NULL) {
            cerr<<"ComboBoxText::setSelection : A value GtkTreeModel is not associated with this GtkComboBoxText"<<endl;
            return ret;
        }
        valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter); // Get first row in list store
        int which=0;
        cout<<valid<<endl;
        while (valid) {
            char *val;
            gtk_tree_model_get(model, &iter, 0, &val,  -1);
            //cout<<val<<endl;
            if (strcmp(text.c_str(),val)==0) {
                ret=valid=0;
                setSelection(which);
            } else
                valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter); // Make iter point to the next row in the list store
            which++;
            g_free(val);
        }
        return ret;
    }

    /** Overloads the input operator.
    This allows the ComboBoxText to be loaded like so :
    \code
    ComboBoxText()<<"entry 1"<<"entry 2";
    \endcode
    \param text The character array to add to the combo box
    \return This object for further use
    */
    ComboBoxText& operator<<(const char*text) {
#if GTK_MAJOR_VERSION==2 && GTK_MINOR_VERSION<24
        gtk_combo_box_append_text(GTK_COMBO_BOX(widget), text);
#else
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), text);
#endif
        return *this;
    }

    /** Overloads the input operator.
    This allows the ComboBoxText to be loaded like so :
    \code
    ComboBoxText()<<string("entry 1")<<string("entry 2");
    \endcode
    \param text The string to add to the combo box
    \return This object for further use
    */
    ComboBoxText& operator<<(string text) {
#if GTK_MAJOR_VERSION==2 && GTK_MINOR_VERSION<24
        gtk_combo_box_append_text(GTK_COMBO_BOX(widget), text.c_str());
#else
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widget), text.c_str());
#endif
        return *this;
    }
};
#endif //COMBOBOXTEXT_H_
