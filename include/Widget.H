/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
#ifndef WIDGET_H_
#define WIDGET_H_

#include <gtk/gtk.h>
#include <cairo/cairo.h>
#include <iostream>

#include "DragNDrop.H"
#include <map>

#define WIDGET_DRAGHIGHLIGHTSTATE_DATA "DragHighlightState"

/** Encapsulates the GtkWidget object
*/
class Widget {
protected:
    GtkWidget *widget; ///< The container based widget
public:
    /// Construcotr
    Widget() {
        widget=NULL;
    }

    /** Construct this class given a widget.
    \param w The widget to use.
    */
    Widget(GtkWidget *w) {
        widget=w;
    }

    virtual ~Widget() {} ///< Destructor

    /** Add a reference to the widget
    */
    GtkWidget *ref(void) {
        g_object_ref(widget);
        return widget;
    }

    /** Remove a reference from the widget
    */
    void unref(void) {
        g_object_unref(widget);
    }

    /** Method to show the widget.
    \return box the GtkWidget* to the box
    */
    GtkWidget* show(void) {
        gtk_widget_show(widget);
        return widget;
    }
    /** Method to hide the widget.
    \return box the GtkWidget* to the box
    */
    GtkWidget* hide(void) {
        gtk_widget_hide(widget);
        return widget;
    }

    /** Find whether a widget is visible.
    \return true if visible, false if not.
    */
    bool getVisible(void) {
#if GTK_MAJOR_VERSION==2
        return GTK_WIDGET_VISIBLE(widget);
#else
        return gtk_widget_get_visible(widget);
#endif
    }

    /** Method to get the GtkWidget
    \return box The box GtkWidget
    */
    GtkWidget* getWidget(void) {
        return widget;
    }

    /** Set whether the widget is sensitive or not.
    \return True if sensitive or false if not.
    */
    bool getSensitive(void) {
        return gtk_widget_get_sensitive(widget);
    }

    /** Set whether the widget is sensitive or not.
    \param sens True if sensitive or false if not.
    */
    void setSensitive(bool sens) {
        gtk_widget_set_sensitive(widget, sens);
    }

    /** Find the prefered size of the widget.
    \param[out] width the width of the widget
    \param[out] height the height of the widget
    */
    void getSize(int &width, int &height) {
#if GTK_MAJOR_VERSION>2 // if using GTK3 or more recent, then use get_preferred_size
        GtkRequisition requisitionMin;
        GtkRequisition naturalRequisition;
        gtk_widget_get_preferred_size(widget, &requisitionMin, &naturalRequisition);
        width=requisitionMin.width;
        height=requisitionMin.height;
#else
        GtkAllocation allocation;
        gtk_widget_get_allocation(widget, &allocation);
        width=allocation.width;
        height=allocation.height;
#endif
    }

    /** Put in a request to change the prefered size of the widget.
    \param width the width of the widget
    \param height the height of the widget
    */
    virtual void setSize(int width, int height) {
        gtk_widget_set_size_request(widget, width, height);
    }

    /** Find the widgets location and size.
    \param[out] x The x location
    \param[out] y The y location
    \param[out] width The width
    \param[out] height The height
    */
    void getLocationSize(int &x, int &y, int &width, int &height){
        GtkAllocation alloc;
        gtk_widget_get_allocation(widget, &alloc);
        x=alloc.x;
        y=alloc.y;
        width=alloc.width;
        height=alloc.height;
    }

    /** queue the widget for redrawing
    */
    void queueDraw() {
        gtk_widget_queue_draw(widget);
    }

    /** Get the parent window of the widget
    */
    GdkWindow* getWindow() {
        return gtk_widget_get_window(widget);
    }

    /* Save the top most container widget to file.
    \param fileName The target file name ending with the appropriate extension.
    \return <0 on error.
    */
    /*	int saveTopWindowToPNGFile(const char *fileName){
    		return saveToPNGFile(gdk_get_default_root_window(), fileName);
    	}*/

#ifdef gdk_cairo_set_source_window
    /** Save the widget to file.
    \param fileName The target file name ending with the appropriate extension.
    \return <0 on error.
    */
    int saveToPNGFile(const char *fileName) {
        return saveToPNGFile(getWindow(), fileName);
    }

    /** Save the widget to file.
    \param wid The widget to save to file.
    \param fileName The target file name ending with the appropriate extension.
    \return <0 on error.
    */
    int saveToPNGFile(GdkWindow *win, const char *fileName) {
        gint width, height;
        getSize(width, height);
//        gdk_drawable_get_size(win, &width, &height);
//#if GTK_MAJOR_VERSION>2 // if using GTK3 or more recent, then use get_preferred_size
//        gint x_orig, y_orig;
//        gdk_window_get_origin(win, &x_orig, &y_orig);
//        GdkPixbuf *screenshot = gdk_pixbuf_get_from_drawable(NULL, win, NULL, x_orig, y_orig, 0, 0, width, height);
//        cairo_surface_t *cs=gdk_cairo_surface_create_from_pixbuf(screenshot, 0, NULL);
//#else
        cairo_surface_t *cs=gdk_window_create_similar_surface(win, CAIRO_CONTENT_COLOR_ALPHA, width, height);
        cairo_t *cr=cairo_create(cs);
        gdk_cairo_set_source_window(cr, win, 0, 0);
        cairo_paint(cr);
//#endif
        cairo_status_t res=cairo_surface_write_to_png(cs, fileName);
//#if GTK_MAJOR_VERSION>2 // if using GTK3 or more recent, then use get_preferred_size
//        if (screenshot) gdk_pixbuf_unref(screenshot);
//#else
        cairo_destroy(cr);
//#endif
        if (cs) cairo_surface_destroy(cs); // cleanup
        if (res!=CAIRO_STATUS_SUCCESS) {
            if (res==CAIRO_STATUS_NO_MEMORY)
                std::cerr<<"Widget::saveToPNGFile : memory could not be allocated for the operation"<<std::endl;
            else if (res==CAIRO_STATUS_SURFACE_TYPE_MISMATCH)
                std::cerr<<"Widget::saveToPNGFile : the surface does not have pixel contents"<<std::endl;
            else if (res==CAIRO_STATUS_WRITE_ERROR)
                std::cerr<<"Widget::saveToPNGFile : an I/O error occurs while attempting to write the file."<<std::endl;
            return -1;
        } else
            return 0;
    }
#endif
    /** Connect a signal callback.
    \param event the signal to generate the callback e.g. "expose-event"
    \param callback the callback to call
    \param data the user data to pass to the callback
    */
    void connect(const char *event, GCallback callback, gpointer data) {
        g_signal_connect(widget, event, callback, data);
    }

    /** Connect a signal callback after the other callbacks have run.
    \param event the signal to generate the callback e.g. "expose-event"
    \param callback the callback to call
    \param data the user data to pass to the callback
    */
    void connectAfter(const char *event, GCallback callback, gpointer data) {
        g_signal_connect_after(widget, event, callback, data);
    }

    /** Connect a callback function for when the widget is exposed.

    Your expose event should be of the following definition : static gboolean callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

    The callback returns true to stop/complete processing of the expose signal or false to continue.
    \param callback The callback to execute
    \param data The user data to pass to the callback
    */
    void connectExposeEventCallback(GCallback callback, void *data){
        connectExposeEventCallback(widget, callback, data);
    }

    /** Connect a callback function for when the widget is exposed.

    Your expose event should be of the following definition : static gboolean callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

    The callback returns true to stop/complete processing of the expose signal or false to continue.
    \param w The widget to connect the signal handler to.
    \param callback The callback to execute
    \param data The user data to pass to the callback
    */
    void connectExposeEventCallback(GtkWidget*w, GCallback callback, void *data){
        g_signal_connect_after(G_OBJECT(w), "expose-event", callback, data);
    }

    /** Connect a callback function for when the widget is configured. This includes size, position and stacking changes.

    Your expose event should be of the following definition : static gboolean callback(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

    The callback returns true to stop/complete processing of the expose signal or false to continue.
    \param callback The callback to execute
    \param data The user data to pass to the callback
    */
    void connectConfigureEventCallback(GCallback callback, void *data){
        connectConfigureEventCallback(widget, callback, data);
    }

    /** Connect a callback function for when the widget is configured. This includes size, position and stacking changes.

    Your expose event should be of the following definition : static gboolean callback(GtkWidget *widget, GdkEventConfigure *event, gpointer data);

    The callback returns true to stop/complete processing of the expose signal or false to continue.
    \param w The widget to connect the signal handler to.
    \param callback The callback to execute
    \param data The user data to pass to the callback
    */
    void connectConfigureEventCallback(GtkWidget* w, GCallback callback, void *data){
        g_signal_connect_after(G_OBJECT(w), "configure-event", callback, data);
    }

// Drag and drop entries below
public:
    /** Make this widget a destination by adding a set of drag and drop targets to this widget.
    This widget becomes a drag and drop destination (sink)
    \param dnd The DragNDrop class specifying the set of targets.
    */
    Widget &operator<<(DragNDrop &dnd) {
        gtk_drag_dest_set(getWidget(), dnd.getDestDefaults(), dnd.getTargetArray(), dnd.size(), dnd.getDragAction());
/*        } else {
            w=(Widget*)userData;
            GdkDragProtocol protocol;
            gdk_drag_get_protocol_for_display(gtk_widget_get_display(w->getWidget()), gdknamespace::gdk_x11_drawable_get_xid(w->getWindow()), &protocol);
            gboolean useCoordinates=true;
            gtk_drag_dest_set_proxy(w->getWidget(), w->getWindow(), protocol, useCoordinates);
        }*/

        gpointer userData=dnd.getUserData(); // check if we are using a different widget for user data in the callbacks.
        if (userData==NULL)
            userData=this;
        // Sink signals
        connectAfter("drag-data-received", G_CALLBACK(dragDataReceivedStatic), userData);
        connectAfter("drag-leave", G_CALLBACK(dragLeaveStatic), userData);
        connectAfter("drag-motion", G_CALLBACK(dragMotionStatic), userData);
        connectAfter("drag-drop", G_CALLBACK(dragDropStatic), userData);
        return *this;
    }

    /** Make this widget a source by adding a set of drag and drop targets to this widget.
    This widget becomes a drag and drop source
    \param dnd The DragNDrop class specifying the set of targets.
    */
    Widget &operator>>(DragNDrop &dnd) {
        //if (GTK_WIDGET_NO_WINDOW(widget))
        //    std::cout<<"Can't use this widget for a drag and drop source as it has no associated window."<<std::endl;
        gtk_drag_source_set(getWidget(), dnd.getModifierType(), dnd.getTargetArray(), dnd.size(), dnd.getDragAction());
        gpointer userData=dnd.getUserData(); // check if we are using a different widget for user data in the callbacks.
        if (userData==NULL)
            userData=this;
        // Source signals
        connectAfter("drag-data-get", G_CALLBACK(dragDataGetStatic), userData);
        connectAfter("drag-data-delete", G_CALLBACK(dragDataDeleteStatic), userData);
        connectAfter("drag-begin", G_CALLBACK(dragBeginStatic), userData);
        connectAfter("drag-end", G_CALLBACK(dragEndStatic), userData);
        return *this;
    }

private:
    // below are the drag and drop sink (destination) methods
    /** Executed on the drop site when the dragged data has been received.
    Also executed when a drag motion event requests knowledge on whether the drop will be accepted successfully or not.
    \param w the GtkWidget which is the destination of the drag.
    \param dragContext the drag context.
    \param x where the drop happened.
    \param y where the drop happened.
    \param selectionData the received data.
    \param info the info that has been registered with the target in the GtkTargetList.
    \param time the timestamp at which the data was received.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator<<
    */
    static void dragDataReceivedStatic(GtkWidget *w, GdkDragContext *dragContext, gint x, gint y, GtkSelectionData *selectionData, guint info, guint time, gpointer data) {
        //std::cout<<"Widget::dragDataReceivedStatic "<<w<<'\t'<<data<<std::endl;
        bool success=false;

        /* To come ... answer the modtion events requests to know if the data will be accepted or not in advance of dropping
        PrivateData *private_data = GET_PRIVATE_DATA (w);
        if (private_data->suggested_action){ // is this a request to check validity from dragMotionStatic ?
            private_data->suggested_action = 0;
            success=checkDropValidity(info, gtk_selection_data_get_data(selectionData);
            if (!success) // not ok to drop
                gdk_drag_status(dragContext, 0, time);
            else
                gdk_drag_status(dragContext, gdk_drag_context_get_suggested_action(dragContext), time);
            return;
        }
        */

        // not a validity check, so a physical drop ...
        bool deleteSelectionData=false;
        if((selectionData != NULL) && (gtk_selection_data_get_length(selectionData) >= 0)) {
            if (gdk_drag_context_get_suggested_action(dragContext) == GDK_ACTION_ASK) {
                // currently not implemented
                // Ask the user to move or copy, then set the context action.
            }
            bool deleteSelectionData=false; // work out whether to copy or move.
            if (gdk_drag_context_get_suggested_action(dragContext) == GDK_ACTION_COPY)
                ;
            else if (gdk_drag_context_get_suggested_action(dragContext) == GDK_ACTION_MOVE)
                deleteSelectionData=true;
            // execute the user's overloaded virtual method
            success=static_cast<Widget*>(data)->dragDataReceived(w, gtk_drag_get_source_widget(dragContext), info, gtk_selection_data_get_data(selectionData), gtk_selection_data_get_length(selectionData));
        }
        gtk_drag_finish(dragContext, success, deleteSelectionData, time);
    }

    /** Executed when the cursor leaves the drop widget.
    A typical reason to exectue this method is to undo things executed in dragMotionStatic.
    \param w the GtkWidget which is the destination of the drag.
    \param dragContext the drag context.
    \param time the timestamp at which the data was received.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator<<
    */
    static void dragLeaveStatic(GtkWidget *w, GdkDragContext *dragContext, guint time, gpointer data) {
        //std::cout<<"Widget::dragLeaveStatic"<<std::endl;
        static_cast<Widget*>(data)->dragUnhighlight(w); // execute the un highlighting
        static_cast<Widget*>(data)->dragLeave(w, gtk_drag_get_source_widget(dragContext));
    }

    /** Executed when the widget must be 'unhighlighted'.
    \param w the GtkWidget which is the destination of the drag.
    */
    virtual void dragUnhighlight(GtkWidget *w){
        if (g_object_get_data(G_OBJECT(w), WIDGET_DRAGHIGHLIGHTSTATE_DATA)){ // if the widget is highlit, then un highlight
            g_object_set_data(G_OBJECT(w), WIDGET_DRAGHIGHLIGHTSTATE_DATA, 0); // store the highlight state in the object
            gtk_drag_unhighlight(w);
        }
    }

    /** Executed when the user moves the cursor over the drop widget during a drag.
    \param w the GtkWidget which is the destination of the drag.
    \param dragContext the drag context.
    \param x where the drop happened.
    \param y where the drop happened.
    \param time the timestamp at which the data was received.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator<<
    \return true if in the drop zone and to be accepted. false otherwise and no more processing.
    */
    static gboolean dragMotionStatic(GtkWidget *w, GdkDragContext *dragContext, gint x, gint y, guint time, gpointer data) {
        static_cast<Widget*>(data)->dragHighlight(w); // execute the highlighting
        /* To come ... more intricate tests requesting validation from the "drag-data-received" signal (dragDataReceivedStatic)
        GdkAtom target = gtk_drag_dest_find_target(widget, dragContext, NULL);
        if (target == GDK_NONE)
            gdk_drag_status (dragContext, 0, time);
        else {
            private_data->pending_status = dragContext->suggested_action;
            gtk_drag_get_data(w, dragContext, target, time);
        }
        */

        bool success=static_cast<Widget*>(data)->dragMotion(w, gtk_drag_get_source_widget(dragContext), x, y);

        return success;
    }

    /** Executed when the widget must be highlighted.
    \param w the GtkWidget which is the destination of the drag.
    */
    virtual void dragHighlight(GtkWidget *w){
        if (!g_object_get_data(G_OBJECT(w), WIDGET_DRAGHIGHLIGHTSTATE_DATA)){ // if not highlit, highlight
            g_object_set_data (G_OBJECT(w), WIDGET_DRAGHIGHLIGHTSTATE_DATA, (gpointer)1); // store the highlight state in the object
            gtk_drag_highlight (w);
        }
    }

    /** Executed when the user drops the data onto the widget.
    The user must determine whether is has been dropped onto a suitable drop zone.
    \param w the GtkWidget which is the destination of the drag.
    \param dragContext the drag context.
    \param x where the drop happened.
    \param y where the drop happened.
    \param time the timestamp at which the data was received.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator<<
    \return true if in the drop zone, valid data and to be accepted, false otherwise.
    */
    static bool dragDropStatic(GtkWidget *w, GdkDragContext *dragContext, gint x, gint y, guint time, gpointer data) {
        //std::cout<<"Widget::dragDropStatic"<<std::endl;
        // Check to see if (x,y) is a valid drop site within widget
        gboolean validDropSite = static_cast<Widget*>(data)->dragMotion(w, gtk_drag_get_source_widget(dragContext), x, y);
        if (validDropSite)
            if (gdk_drag_context_list_targets(dragContext)) { // If the source offers a target
                // Choose the best target type
                DragNDrop dnd(dragContext); // construct a DragNDrop class to hold all of the target types.
                int whichTarget=static_cast<Widget*>(data)->chooseTheTargetToUse(w, gtk_drag_get_source_widget(dragContext), dnd);
                std::cout<<"whichTarget "<<whichTarget<<std::endl;
                if (whichTarget<0)
                    validDropSite = FALSE;
                else {
                    GdkAtom targetType = GDK_POINTER_TO_ATOM(g_list_nth_data(gdk_drag_context_list_targets(dragContext), whichTarget));
                    // Request the data from the source. This triggers the received signal/execution
                    // the widget (w) will receive the 'drag-data-received' signal, which executes dragDataReceivedStatic
                    gtk_drag_get_data(w, dragContext, targetType, time);
                }
            } else // No target offered by source => error
                validDropSite = FALSE;
        return  validDropSite;
    }

    // below are the drag and drop source methods

    /** Executed by the drag source when the drop site requests the data which is dragged.
    It fulls the selectionData.
    \param w the object which is sending the data.
    \param dragContext the drag context.
    \param selectionData the data to send.
    \param info the info that has been registered with the target in the GtkTargetList.
    \param time the time stamp at which the data was requested.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator>>
    */
    static void dragDataGetStatic(GtkWidget *w, GdkDragContext *dragContext, GtkSelectionData *selectionData, guint info, guint time, gpointer data) {
        //std::cout<<"Widget::dragDataGetStatic"<<std::endl;
        g_assert (selectionData != NULL);
        static_cast<Widget*>(data)->dragDataGet(w, selectionData, info);
    }

    /** Executed for deleting the data that has been dropped.
    \param w the object which is sending the data.
    \param dragContext the drag context.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator>>
    */
    static void dragDataDeleteStatic(GtkWidget *w, GdkDragContext *dragContext, gpointer data) {
        //std::cout<<"Widget::dragDataDeleteStatic"<<std::endl;
        static_cast<Widget*>(data)->dragDataDelete(w);
    }

    /** Executed on the drag source when a drag is started.
    A typical reason to connect to this signal is to set up a custom drag icon with gtk_drag_source_set_icon().
    \param w the object which is the source of the drag.
    \param dragContext the drag context.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator>>
    */
    static void dragBeginStatic(GtkWidget *w, GdkDragContext *dragContext, gpointer data) {
        //std::cout<<"Widget::dragBeginStatic"<<std::endl;
        static_cast<Widget*>(data)->dragBegin(w);
    }

    /** Executed on the drag source when a drag is finished.
    A typical reason to connect to this signal is to undo things done in dragBegin.
    \param w the object which is the source of the drag.
    \param dragContext the drag context.
    \param data user data set when the signal handler was connected, typically the widget which executed the operator>>
    */
    static void dragEndStatic(GtkWidget *w, GdkDragContext *dragContext, gpointer data) {
        //std::cout<<"Widget::dragEndStatic"<<std::endl;
        static_cast<Widget*>(data)->dragEnd(w);
    }

protected:
    // below are the drag and drop sink (destination) methods
    /** Executed when a motion even requests knowledge from dragDataReceivedStatic as to whether the data is valid or not.
    \param w the GtkWidget which is the destination of the drag.
    \param info the info that has been registered with the target in the GtkTargetList.
    \param selectionStoredData The raw data which is sent by the other side, cast to a unsigned char pointer.
    \return true if the data will be accepted or false on failure
    */
    virtual bool checkDropValidity(GtkWidget *w, GtkWidget *sourceWidget, int info, const unsigned char *selectionStoredData) {
        //std::cout<<"Widget::checkDropValidity not overloaded"<<std::endl;
        return false;
    }

    /** Executed when the user drops and completes. You must overload this method to execute on the data passed during the drop.
    \param w the GtkWidget which is the destination of the drag.
    \param info the info that has been registered with the target in the GtkTargetList.
    \param selectionStoredData The raw data which is sent by the other side, cast to a unsigned char pointer.
    \param length The length of the stored data
    \return true on success or false on failure
    */
    virtual bool dragDataReceived(GtkWidget *w, GtkWidget *sourceWidget, int info, const unsigned char *selectionStoredData, int length) {
        //std::cout<<"Widget::dragDataReceived not overloaded"<<std::endl;
        return false;
    }

    /** Executed when the cursor leaves the drop widget.
    A typical reason to execute this method is to undo things executed in dragMotion.
    \param w the GtkWidget which is the destination of the drag.
    */
    virtual void dragLeave(GtkWidget *w, GtkWidget *sourceWidget) {
        //std::cout<<"Widget::dragLeave not overloaded"<<std::endl;
    }

    /** Executed to work out whether the drop action will be successful or not based on current location and the user data.
    This is currently executed when the user drops the widget on the target.
    \param w the GtkWidget which is the destination of the drag.
    \param x where the drop happened.
    \param y where the drop happened.
    \return true if in the drop zone and to be accepted. false otherwise and no more processing.
    */
    virtual bool dragMotion(GtkWidget *w, GtkWidget *sourceWidget, int x, int y) {
        //std::cout<<"Widget::dragMotion not overloaded, returning true to indicate ok"<<std::endl;
        return true;
    }

    /** Executed when the target is dropped.
    This method specifies which target of all of the known targets to use.
    If there are zero known targets, the error is returned. If there is only one target, it is returned. Otherwise, you must overload this method.
    If you overload this method, be sure to check for and return <0 when the known target list is empty, like so :
    \code
    if (dnd.size()==0) // if there are no known targets, report error.
        return DragNDropDebug().evaluateError(DND_EMPTY_ERROR);
        if (dnd.size()==1) // There can be only one
            return 0;
    \endcode
    \param w the GtkWidget which is the destination of the drag.
    \param sourceWidget The widget being dragged.
    \param dnd The drag and drop data contexts
    \return the number matching the desired target data record from the DragNDrop::targets class. Returns <0 on error.
    */
    virtual int chooseTheTargetToUse(GtkWidget *w, GtkWidget *sourceWidget, DragNDrop &dnd){
        if (dnd.size()==0) // if there are no known targets, report error.
            return DragNDropDebug().evaluateError(DND_EMPTY_ERROR);
        if (dnd.size()==1) // There can be only one
            return 0;
        std::cout<<"Widget::chooseTheTargetToUse not overloaded, handling the empty and single target case"<<std::endl;
        return DragNDropDebug().evaluateError(DND_WHICHTARGET_ERROR);
    }

    // below are the drag and drop source methods

    /** Overload this to allow your source to send data. This method is executed when the destination requests the data.
    You can send data by loading in to the selectionData using one of the gtk_selection_data_set methods, which include the following
    functions : gtk_selection_data_set, gtk_selection_data_set_text, gtk_selection_data_set_pixbuf, gtk_selection_data_set_uris.
    Executed by the drag source when the drop site requests the data which is dragged. It fulls the selectionData.
    An example use of the function is :
    \code
    gtk_selection_data_set(selectionData, gtk_selection_data_get_target(selectionData), sizeof(gunsigned char), (gunsigned char*) string_data, strlen(string_data));
    \endcode
    \param w the object which is the source of the drag.
    \param selectionData the data to send.
    \param info the info that has been registered with the target in the GtkTargetList.
    */
    virtual void dragDataGet(GtkWidget *w, GtkSelectionData *selectionData, guint info) {
        std::cout<<"Widget::dragDataGet not overloaded, this is where you load in the data for sending"<<std::endl;
    }

    /** Executed for deleting the data that has been dropped.
    Overload this method to execute something when something has to change to your application once the data has been successfully dropped.
    \param w the object which is the source of the drag.
    */
    virtual void dragDataDelete(GtkWidget *w) {
        //std::cout<<"Widget::dragDataDelete not overloaded"<<std::endl;
    }

    /** Executed on the drag source when a drag is started.
    A typical reason to connect to this signal is to set up a custom drag icon with gtk_drag_source_set_icon().
    \param w the object which is the source of the drag.
    */
    virtual void dragBegin(GtkWidget *w) {
        //std::cout<<"Widget::dragBegin not overloaded"<<std::endl;
    }

    /** Executed on the drag source when a drag is finished.
    A typical reason to connect to this signal is to undo things done in dragBegin.
    \param w the object which is the source of the drag.
    */
    virtual void dragEnd(GtkWidget *w) {
        //std::cout<<"Widget::dragEnd not overloaded"<<std::endl;
    }
};
#endif // WIDGET_H_
