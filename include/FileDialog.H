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
#ifndef FILEDIALOG_H_
#define FILEDIALOG_H_

#include "Container.H"

/** A file dialog for choosing a file name.
*/
class FileDialog : public Container {
public:
    /// Constructor
    FileDialog() {
        widget = gtk_file_chooser_dialog_new ("File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    }

    /** Constructor
    */
    FileDialog(const std::string title, GtkWindow *parent, GtkFileChooserAction action, const char *stockImageName) {
        widget = gtk_file_chooser_dialog_new (title.c_str(), parent, action, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, stockImageName, GTK_RESPONSE_ACCEPT, NULL);
    }

    /// Destructor
    virtual ~FileDialog() {}

    /** Set the directory.
    \param name The directory name
    */
    void setDirectory(const std::string name){
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), name.c_str());
    }

    /** Set the file name.
    If this file name doesn't exist, then the text field is left empty, and the directory is reset to the current path.
    \param name The name to set including the path to the file name.
    */
    void setFileName(const std::string name) {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(widget), name.c_str());
    }

    /** Get the name set in the file chooser dialog.
    \return The name
    */
    std::string getName(){
        char *fn=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
        std::string res;
        if (fn!=NULL){
            res=fn;
            g_free(fn);
        }
        return res;
    }

//    /* Allow setting the filename using the = operator.
//    \param name The name to set the dialog entry box to.
//    \return A reference to this class.
//    */
//    FileDialog &operator=(const std::string name) {
//        setName(name);
//        return *this;
//    }
//
//    /* Allow setting the filename using the = operator.
//    \param name The name to set the dialog entry box to.
//    \return A reference to this class.
//    */
//    FileDialog &operator=(const char *name) {
//        setName(std::string(name));
//        return *this;
//    }

    /** run the dialog and return the response.
    \return The response {GTK_RESPONSE_NONE, GTK_RESPONSE_REJECT, GTK_RESPONSE_ACCEPT, GTK_RESPONSE_DELETE_EVENT, GTK_RESPONSE_OK, GTK_RESPONSE_CANCEL, GTK_RESPONSE_CLOSE, GTK_RESPONSE_YES, GTK_RESPONSE_NO, GTK_RESPONSE_APPLY, GTK_RESPONSE_HELP}
    */
    gint run(){
        return gtk_dialog_run(GTK_DIALOG(widget));
    }
};

class FileOpenDialog : public FileDialog {
public:
    FileOpenDialog() : FileDialog("", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN) {}
    FileOpenDialog(const std::string title) : FileDialog(title, NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN) {}
};

class FileSaveDialog : public FileDialog {
    void init(){
        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(widget), TRUE); // set the dialog to warn on overwrite
    }
public:
    FileSaveDialog() : FileDialog("", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_SAVE) {
        init();
    }
    FileSaveDialog(const std::string title) : FileDialog(title, NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_SAVE) {
        init();
    }
};

class FolderSelectDialog : public FileDialog {
public:
    FolderSelectDialog() : FileDialog("", NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_OPEN) {}
    FolderSelectDialog(const std::string title) : FileDialog(title, NULL, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_OPEN) {}
};

class FolderCreateDialog : public FileDialog {
public:
    FolderCreateDialog() : FileDialog("", NULL, GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER, GTK_STOCK_NEW) {}
    FolderCreateDialog(const std::string title) : FileDialog(title, NULL, GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER, GTK_STOCK_NEW) {}
};

#endif // FILEDIALOG_H_
