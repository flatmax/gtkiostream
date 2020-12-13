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
#ifndef DRAGNDROP_H_
#define DRAGNDROP_H_

#include <vector>
#include <string.h>
#include "Debug.H"

#define DND_EMPTY_ERROR -0+DRAGNDROP_ERROR_OFFSET
#define DND_WHICHTARGET_ERROR -1+DRAGNDROP_ERROR_OFFSET

class DragNDropDebug : public Debug {
public:
    DragNDropDebug(void) {
#ifndef NDEBUG
        errors[DND_EMPTY_ERROR]=std::string("The DragNDrop known target list is empty. Fill the target list by constructing with a GdkDragContext, or possibly by using the operator<<. ");
        errors[DND_WHICHTARGET_ERROR]=std::string("The DragNDrop known target list is not empty and has more the one entry, please overload the Widget::chooseTheTargetToUse method to specify which target you want to use in this drop method. ");
#endif
    }
};

class Widget;

/** Class to setup a drag and drop set of targets for either a source or a sink.
This class does a few things. Firstly it allows you to specify different target data types.
Secondly, it allows you to specify custom user data to be used in the drag and drop calbbacks.

The data targets are in the inherited std::vector of GtkTargetEntry structures.

Setup data targets like so :
\code
DragNDrop dnd; // instantiate a drag and drop class
dnd<<(GtkTargetEntry){(char*)"INTEGER", 0, 0}; // setup a data type for the dnd system - as there is only one, it is the default
\endcode

Set the user data for drag and drop callbacks like so :
\code
Widget widget;
DragNDrop dnd; // instantiate a drag and drop class
dnd<<widget; // set the user data for drag and drop callbacks
\endcode

An example of how to use this class is as follows :
\code
Widget fromW, toW; // setup and source (from) and destination (to) widget.
DragNDrop dnd; // instantiate a drag and drop class
fromW>>dnd; // Tell the Widget it will be dragged around
toW<<dnd; // Tell the other widget it will be the dragging destination
\endcode
*/
class DragNDrop : public std::vector<GtkTargetEntry> {
    gpointer userData; ///< If we want DND callbacks to operate on different user data, then this will store it here.

    GtkDestDefaults destFlags; ///< The drop flags, see setDestDefaults
    GdkDragAction dragAction; ///< Action to take once the data is dropped.
    GdkModifierType modifierType; ///< The modifier which initiates the drag and drop process.

    /** Method called on a GList object to add a target pointer to the targets.
    \param targetData A pointer to the specific instance of the GtkTargetEntry to add to the known target list.
    \param data The pointer to the instance of this class to operate on.
    */
    static void addToTargetsStatic(gpointer targetData, gpointer data) {
        static_cast<DragNDrop*>(data)->addToTargets(GDK_POINTER_TO_ATOM (targetData));
    }

    /** Add a GtkTargetEntry to the known list of targets.
    */
    void addToTargets(GdkAtom targetData) {
        GtkTargetEntry gte;
        gte.target=gdk_atom_name(targetData);
        gte.flags=0;
        gte.info=0;
        push_back(gte);
    }

    /** Executed in the constructors to set the default member variables.
    Sets the following :
    \code
    destFlags=GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT;
    dragAction=GDK_ACTION_COPY;
    modifierType=GDK_BUTTON1_MASK; // first mouse button
    \endcode

    \sa setDestDefaults, setDragAction
    */
    void init() {
        userData=NULL;
        destFlags=(GtkDestDefaults)(GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT);
        dragAction=GDK_ACTION_COPY;
        modifierType=GDK_BUTTON1_MASK; // first mouse button
    }

public:
    /** Constructor
    */
    DragNDrop() {
        init();
    }

    /** Construct a set of targets from a drag context.
    \param context The GdkDragContext which is passed around during a drag and drop process. See Widget::dragDropStatic for more information.
    */
    DragNDrop(GdkDragContext *context) {
        init();
        g_list_foreach(gdk_drag_context_list_targets(context), reinterpret_cast<GFunc>(addToTargetsStatic), this);
        //for(GList *iter = gdk_drag_context_list_targets(context); iter != NULL; iter = g_list_next(iter))
        //    addToTargets((GtkTargetEntry*)iter->data);
    }

    /// Destructor
    virtual ~DragNDrop() {}

    /** Use this if you want drag and drop methods to callback using different user data to the Widget instance which has the Widget::operator<< and Widget::operator>> methods.
    \param w The userData to pass to the drag and drop callbacks.
    */
    void setUserData(gpointer w){
        userData=w;
    }

    /** Find which userData to be used by Widget::operator<< and Widget::operator>> to setup the drag and drop callbacks. NULL implies it will use the widget which executes the Widget::operator<< and Widget::operator>> methods.
    \return The widget to pass to the drag and drop callbacks.
    */
    gpointer getUserData(){
        return userData;
    }

    /** Alter the default set of flags for destination widgets.
    This only taked effect before the Widget::operator<<(DrageNDrop &) is called.
    \param destFlagsIn The new set of flags for the dest in.
    Possibilities are :

    GTK_DEST_DEFAULT_MOTION If set for a widget, GTK+, during a drag over this widget will check if the drag matches this widget’s list of possible targets and actions.

    GTK_DEST_DEFAULT_HIGHLIGHT If set for a widget, GTK+ will draw a highlight on this widget as long as a drag is over this widget and the widget drag format and action are acceptable.

    GTK_DEST_DEFAULT_DROP If set for a widget, when a drop occurs, GTK+ will will check if the drag matches this widget’s list of possible targets and actions.

    GTK_DEST_DEFAULT_ALL If set, specifies that all default actions should be taken.

    */
    void setDestDefaults(GtkDestDefaults destFlagsIn) {
        destFlags=destFlagsIn;
    }

    /** Retrieve the current state of the destFlags.
    */
    GtkDestDefaults getDestDefaults() {
        return destFlags;
    }

    /** Sets the drag action once dropped.
    \param dragActionIn The action to take once dropped, possiblities are:

    GDK_ACTION_DEFAULT Means nothing, and should not be used.

    GDK_ACTION_COPY Copy the data.

    GDK_ACTION_MOVE Move the data, i.e. first copy it, then delete it from the source using the DELETE target of the X selection protocol.

    GDK_ACTION_LINK Add a link to the data. Note that this is only useful if source and destination agree on what it means.

    GDK_ACTION_PRIVATE Special action which tells the source that the destination will do something that the source doesn't understand.

    GDK_ACTION_ASK Ask the user what to do with the data.
    */
    void setDragAction(GdkDragAction dragActionIn) {
        dragAction=dragActionIn;
    }

    /** Get the current dragAction.
    \return The current state of the dragAction variabl which is a possible combination of GdkDragAction flags, see setDragAction
    */
    GdkDragAction getDragAction() {
        return dragAction;
    }

    /** Get the modifier which initiates the drag operation.
    \param modifierTypeIn The modifier to use to start the drag and drop oepration. Known modifiers are :

    GDK_SHIFT_MASK the Shift key.

    GDK_LOCK_MASK a Lock key (depending on the modifier mapping of the X server this may either be CapsLock or ShiftLock).

    GDK_CONTROL_MASK the Control key.

    GDK_MOD1_MASK the fourth modifier key (it depends on the modifier mapping of the X server which key is interpreted as this modifier, but normally it is the Alt key).

    GDK_MOD2_MASK the fifth modifier key (it depends on the modifier mapping of the X server which key is interpreted as this modifier).

    GDK_MOD3_MASK the sixth modifier key (it depends on the modifier mapping of the X server which key is interpreted as this modifier).

    GDK_MOD4_MASK the seventh modifier key (it depends on the modifier mapping of the X server which key is interpreted as this modifier).

    GDK_MOD5_MASK the eighth modifier key (it depends on the modifier mapping of the X server which key is interpreted as this modifier).

    GDK_BUTTON1_MASK the first mouse button.

    GDK_BUTTON2_MASK the second mouse button.

    GDK_BUTTON3_MASK the third mouse button.

    GDK_BUTTON4_MASK the fourth mouse button.

    GDK_BUTTON5_MASK the fifth mouse button.

    GDK_SUPER_MASK the Super modifier.

    GDK_HYPER_MASK the Hyper modifier.

    GDK_META_MASK the Meta modifier.

    GDK_MODIFIER_RESERVED_29_MASK A reserved bit flag; do not use in your own code

    GDK_RELEASE_MASK not used in GDK itself. GTK+ uses it to differentiate between (keyval, modifiers) pairs from key press and release events.

    GDK_MODIFIER_MASK a mask covering all modifier types.
    */
    void getModifierType(GdkModifierType modifierTypeIn) {
        modifierType=modifierTypeIn;
    }

    /** Get the modifier which initiates the drag operation.
    */
    GdkModifierType getModifierType() {
        return modifierType;
    }

    /** Get the pointer to the array of GtkTargetEntry variables.
    \return Pointer to the first element in the array, NULL on error.
    */
    GtkTargetEntry *getTargetArray() {
        if (size()<1) {
            DragNDropDebug().evaluateError(DND_EMPTY_ERROR);
            return NULL;
        }
        return &operator[](0);
    }

    /** Add a particular type of target to be dragged and dropped.
    \param gte The GtkTargetEntry to add.
    */
    DragNDrop &operator<<(const GtkTargetEntry &gte) {
        push_back(gte);
        return *this;
    }

    /** Remove a particular type of target to be dragged and dropped.
    \param gte The GtkTargetEntry to remove.
    */
    DragNDrop &operator>>(const GtkTargetEntry &gte) {
        for (std::vector<GtkTargetEntry>::iterator gtei=begin(); gtei<end(); gtei++)
            if (strcmp(gtei->target, gte.target)==0)
                if (gtei->flags==gte.flags)
                    if (gtei->info==gte.info) {
                        erase(gtei);
                        break;
                    }
        return *this;
    }

    /** Load in a widget and set as the user data.
    */
    DragNDrop &operator<<(Widget &w){
        setUserData(&w);
        return *this;
    }
};
#endif // DRAGNDROP_H_
