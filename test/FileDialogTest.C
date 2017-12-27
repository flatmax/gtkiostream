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
#include "Window.H"
#include "FileDialog.H"

#include <iostream>
using namespace std;

void runDialog(FileDialog *fd){
    gint res=fd->run();
    if (res==GTK_RESPONSE_ACCEPT)
        cout<<"you chose the file "<<fd->getName()<<endl;
    if (res==GTK_RESPONSE_CANCEL)
        cout<<"you didn't chose the file "<<fd->getName()<<endl;
    fd->hide();
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv );

    Window topWindow;

    FileDialog fileDialog;
    fileDialog.setDirectory("/tmp");
    runDialog(&fileDialog);

    FileOpenDialog fod;
    runDialog(&fod);

    FileSaveDialog fsd;
    runDialog(&fsd);

    FolderSelectDialog foldSD;
    runDialog(&foldSD);

    FolderCreateDialog fcd;
    runDialog(&fcd);

    //    gtk_main(); // in this case the FileDialog is its own dialog.
    return 0;
}

