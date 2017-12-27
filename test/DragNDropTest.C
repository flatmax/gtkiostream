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
#include "DragNDrop.H"
#include "Buttons.H"
#include "Labels.H"
#include "Box.H"
#include "Frame.H"

/* In this GTK Drag and Drop example, it is more complex.
Firstly on dropping, data is passed.
Secondly, on dropping, the button widget is moved from the source frame to the destination frame.
*/

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

static void nothingFunction(void *wid, gpointer data) {
    cout<<"nothing"<<endl;
}

/* Overload the source and destination Frame Widget's drag and drop methods to enable data sending and reception.
*/
class DNDFrame : public Frame {
    /* When the drag widget is dropped, this is executed. It sends some data and removes the widget from the container.
    This is executed in the source widget's user data class ('fromFrame' below).
    \param w the object which is the source of the drag.
    \param selectionData the data to send.
    \param info the info that has been registered with the target in the GtkTargetList.
    */
    virtual void dragDataGet(GtkWidget *w, GtkSelectionData *selectionData, guint info) {
        char output[]="test this one"; // send this data
        gtk_selection_data_set(selectionData, gtk_selection_data_get_target(selectionData), sizeof(char), (unsigned char*) output, strlen(output));

        operator>>(w); // remove the widget from the frame
    }

    /* Executed when the user drops and completes. This prints out the sent data and adds the dropped widget to the Frame container.
    This is executed in the destination widget's class.
    \param w the GtkWidget which is the destination of the drag.
    \param info the info that has been registered with the target in the GtkTargetList.
    \param selectionStoredData The raw data which is sent by the other side, cast to a unsigned char pointer.
    \param length The length of the stored data
    \return true on success or false on failure
    */
    virtual bool dragDataReceived(GtkWidget *w, GtkWidget *sourceWidget, int info, const unsigned char *selectionStoredData, int length) {
        cout<<"Read the following from the drop data :\n"<<selectionStoredData<<endl;

        operator<<(sourceWidget); // add the widget to the frame
        return true;
    }

public:

    // constructor
    DNDFrame(const char *label) : Frame(label){}

    // destructor
    virtual ~DNDFrame(){}
};

int main(int argc, char *argv[]) {

    // Normal GTK from here

    gtk_init( &argc, &argv );

    DNDFrame toFrame("to frame"); // DND destination widget - instantiate the widget to drag to

    Buttons draggingButton; // DND source widget - create the widget which will be dragged around
    draggingButton<<BUTTONLABELSTRUCT("drag button", nothingFunction, NULL); // just a regular button
    GtkWidget *db=draggingButton.current(); // point to the actual widget

    DNDFrame fromFrame("from frame"); // DND Source container widget (nothing special) - instantiate the container widget to hold the button to be dragged
    fromFrame<<draggingButton; // load the widget to drag around into the frame

    // DND related code from here

    DragNDrop dnd; // instantiate a drag and drop class
    dnd<<(GtkTargetEntry){(char*)"INTEGER", 0, 0}; // setup a data type for the dnd system - as there is only one, it is the default

    *(Widget*)(&toFrame)<<dnd; // Tell the DNDFrame it will be the dragging destination

    dnd<<fromFrame; // tell dnd that fromFrame is the user data for DND callbacks
    Widget(db)>>dnd; // Tell the Widget it will be dragged around - note, it has to be of type Widget()

    // Normal GTK code from here

    VBox vBox; // vertical box to hold all of the widgets
    vBox<<BoxIS(false, true, false)<<(Buttons()<<BUTTONLABELSTRUCT("Quit", quit, NULL)); // Add the quit button to the end
    vBox<<BoxIS(true, true, true)<<fromFrame.show(); // put the from frame at the start of the vertical box
    vBox<<BoxIS(true, true, false)<<toFrame.show(); // put the to frame at the end of the vertical box

    Window topWindow; // the top window
    topWindow<<vBox.show();
    gtk_main();
    return 0;
}

