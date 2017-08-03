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

#ifndef SELECTION_H_
#define SELECTION_H_
#if GTK_MAJOR_VERSION>2
#error "Support not ready for GTK major version > 2"
#endif

#include <iomanip>
#include <utility>
#include <gtk/gtk.h>

#include <string.h>
#include <string>

#include "mffm/LinkList.H"

using namespace std;

/** Column setup for a Selection
Inherits from a LinkList of paired name and type.
This allows each column to handle many renderers, with string and type identifiers
Column title and contents default to center justification.
*/
class SelectionColumn : public LinkList<pair<string *, GType> *> {
    GtkTreeViewColumn *column;
    int number; ///< The number of this column
public:
    /** Create an empty column with a title
    \param title The title to show for this column
    \param number_ The column number to use, starting from 0
    */
    SelectionColumn(const char*title, int number_) {
        number=number_;
        column=gtk_tree_view_column_new();
        gtk_tree_view_column_set_title (column, title);
        setTitleAlignment(0.5);
    }

    /** destructor
    */
    virtual ~SelectionColumn(void) {
        while (getCount()) { // free up the string pointers in the inherited LinkList<string*>
            pair<string *, GType> *deleteMe=remove();
            delete deleteMe->first;
            delete deleteMe;
        }
    }

    /** \brief Set the alignment of the column title in the header
    Sets the alignment of the column heading to 0.0 for left and 1.0 for right, (0.5 center).
    */
    void setTitleAlignment(float align) {
        gtk_tree_view_column_set_alignment(column, align);
    }

    /** \brief Set the alignment of the column contents
    Sets the alignment of the column contents - applies to all renderers
    \param xAlign The x alignment 0.0 for left and 1.0 for right, (0.5 center).
    \param yAlign The y alignment 0.0 for left and 1.0 for right, (0.5 center).
    */
    void setAlignment(float xAlign, float yAlign) {
        GList*renderers=gtk_tree_view_column_get_cell_renderers(column);
        for (unsigned int i=0;i<g_list_length(renderers);i++)
            //g_object_set(GTK_CELL_RENDERER(g_list_nth_data(renderers,i)), "align-set", 1, "xalign", xAlign, "yalign", yAlign, NULL);
            gtk_cell_renderer_set_alignment(GTK_CELL_RENDERER(g_list_nth_data(renderers,i)), xAlign, yAlign);
    }

//    /* \brief Set the justification of the text column contents
//    Justify the text renderer contents with respect to the edges of the column.
//    param justification The justification 0.0 for left and 1.0 for right, (0.5 center).
//    */
//    void setJustification(float justification){
//        GList*renderers=gtk_tree_view_column_get_cell_renderers(column);
//        cout<<"flatmax : setJustification : g_list_length(renderers) "<<g_list_length(renderers)<<endl;
//        for (uint i=0;i<g_list_length(renderers);i++){
//            cout<<"flatmax "<<i<<endl;
//            g_object_set(GTK_CELL_RENDERER(g_list_nth_data(renderers,i)), "align-set", 1, "alignment", justification, NULL);
//            //g_object_set(GTK_CELL_RENDERER(g_list_nth_data(renderers,i)), "xalign", justification, NULL);
//        }
//    }

//    /* \brief Set the justification of the text column contents
//    Justify the text renderer contents with respect to the edges of the column.
//    param justification The justification PangoAlignment = {PANGO_ALIGN_LEFT, PANGO_ALIGN_CENTER, PANGO_ALIGN_RIGHT};
//    */
//    void setJustification(PangoAlignment justification){
//        GList*renderers=gtk_tree_view_column_get_cell_renderers(column);
//        cout<<"flatmax : setJustification : g_list_length(renderers) "<<g_list_length(renderers)<<endl;
//        for (uint i=0;i<g_list_length(renderers);i++){
//            cout<<"flatmax "<<i<<endl;
//            g_object_set(GTK_CELL_RENDERER_TEXT(g_list_nth_data(renderers,i)), "align-set", 1, "alignment", justification, NULL);
//            ////g_object_set(GTK_CELL_RENDERER(g_list_nth_data(renderers,i)), "alignment", justification, NULL);
//        }
//    }
//
    /** Add a renderer to the column.
    Defaults with middle alignment.
    \param name is the text identification of the renderer to be added to the column.
    \param whichType The GType of the column which is to be added.
    \param renderer is the GtkCellRenderer * to be use for rendering the item to the column
    \param source is the column which contains the source data to be rendered in this column. This number may be a different column to the current column, if the source is the other column.
    \param expand whether the expand within the column during render
    \return a reference to this class.
    */
    SelectionColumn &add(string *name, GType whichType, GtkCellRenderer *renderer, int source, gboolean expand=true) {
        gtk_cell_renderer_set_alignment(renderer, 0.5,0.5); // place in the middle
        LinkList<pair<string*,GType> *>::add(new pair<string*,GType>(name, whichType));
        gtk_tree_view_column_pack_start (column, renderer, expand);
        gtk_tree_view_column_add_attribute (column, renderer, name->c_str(), source);
        return *this;
    }

//    /** Add a renderer to the column.
//    This operator uses this column as the source, and doesn't expand during render
//    \param detail get<0>(detail) is the name, get<1>(detail) is the renderer, get<2>(detail) is the source, get<3>(detail) is the expansion
//    */
//    SelectionColumn &operator<<(tuple<string*, GtkCellRenderer*, int, gboolean> detail){
//        return add(get<0>(detail),get<1>(detail),get<2>(detail),get<3>(detail));
//    }
//
//    /** Add a renderer to the column.
//    This operator uses this column as the source, and doesn't expand during render
//    \param detail get<0>(detail) is the name, get<1>(detail) is the renderer, get<2>(detail) is the source
//    */
//    SelectionColumn &operator<<(tuple<string*, GtkCellRenderer*, int> detail){
//        return add(get<0>(detail),get<1>(detail),get<2>(detail));
//    }

    /** Add a text renderer to the column.
    This operator uses this column as the source, and doesn't expand during render
    \param detail pair.first is the name, pair.second is the renderer
    */
    SelectionColumn &operator<<(pair<string*, GtkCellRendererText*> detail) {
        return add(detail.first,G_TYPE_STRING,(GtkCellRenderer *)detail.second, number);
    }

    /** Add a GdkPixbuf renderer to the column.
    This operator uses this column as the source, and doesn't expand during render
    \param detail pair.first is the name, pair.second is the renderer
    */
    SelectionColumn &operator<<(pair<string*, GtkCellRendererPixbuf*> detail) {
        return add(detail.first,GDK_TYPE_PIXBUF,(GtkCellRenderer *)detail.second, number);
    }

    /** Get the GtkTreeViewColumn *
    \return the underlying GtkTreeViewColumn *
    */
    GtkTreeViewColumn *getColumn(void) {
        return column;
    }

    /** Get the column number.
    \return the column number
    */
    int getNumber(void) {
        return number;
    }

    /** Set the way the column changes size/
    \code
    GTK_TREE_VIEW_COLUMN_GROW_ONLY Columns only get bigger in reaction to changes in the model

    GTK_TREE_VIEW_COLUMN_AUTOSIZE Columns resize to be the optimal size everytime the model changes.

    GTK_TREE_VIEW_COLUMN_FIXED Columns are a fixed numbers of pixels wide.
    \endcode
    \param sz the sizing style  {GTK_TREE_VIEW_COLUMN_GROW_ONLY, GTK_TREE_VIEW_COLUMN_AUTOSIZE, GTK_TREE_VIEW_COLUMN_FIXED}
    */
    void setSizing(GtkTreeViewColumnSizing sz){
        gtk_tree_view_column_set_sizing(column,sz);
    }

    /** Get the way the columns are sizing.
    \sa setSizing
    \return The way in which the columns are sized, GtkTreeViewColumnSizing type
    */
    GtkTreeViewColumnSizing getSizing(void){
        return gtk_tree_view_column_get_sizing(column);
    }

};


/** \brief Selection GtkTreeView based widget
Inherits from a LinkList of GtkTreeViewColumn* which allows modification of the columns.

In its simplest form, you can easily generate a text selection like so :
\image html SelectionTest.jpg
\code
Selection selection("test list items",G_CALLBACK(selectionCallback));
selection<<"test"<<"this"<<"one"; // display each string on their own row.
\endcode

In a more complex form, you can have many columns, in this case text and a pixbuf :
\image html SelectionTest2.jpg
\code
    Selection selection("test list items"); // construct a Selection class
    SelectionColumn *sc= new SelectionColumn("text column", 1); // create the columns class with a new column which handles text
    *sc<<pair<string*, GtkCellRendererPixbuf*>(new string("pixbuf"), GTK_CELL_RENDERER_PIXBUF(gtk_cell_renderer_pixbuf_new())); // create a second column which handles pixbufs
    selection<<sc; // setup the Selection with the columns

    GdkPixbuf *testPixBuf = gdk_pixbuf_new_from_xpm_data(static_cast<const char**>(playXPM)); // get the pixbuf

    selection.grab(1); // get the first column
    selection<<"test col 1"; // add some text to the column
    selection.grab(2); // get the second column - the pixbuf column
    selection<<testPixBuf; // add the pix buf to the column

    // add a third row with both text and a pixbuf
    selection.grab(1); // get the text column
    selection<<"test row 2";
    selection.grab(2); // get the second pixbuf
    selection.add(testPixBuf, true); // add to the same row

    // or add the pixbuf first and then the text
    selection.grab(2); // get the second column - the pixbuf column
    selection.add(testPixBuf); // add to the same row
    selection.grab(1); // get the second column - the pixbuf column
    selection.add("test row 3", true);
\endcode
*/
class Selection : public LinkList<SelectionColumn *> {

	gulong changedHandlerID; ///< The callback handler id for when the selection is changed.

    /** Initialise this class - common to all constructors.
    */
    void init(void) {
        tree = gtk_tree_view_new();
		changedHandlerID=0;
    }

//    GtkTreeIter* getLastIter(void){
//        GtkTreeIter *iter=new GtkTreeIter;
//        GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
//
//        GtkTreeIter last;
//        if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), iter)){
//            cout<<"n children "<<gtk_tree_model_iter_n_children(GTK_TREE_MODEL(store), iter)<<endl;
//            while (gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &last))
//                   (*iter)=last;
//        } else{
//            delete iter;
//            return NULL;
//        }
//        return iter;
//    }

    /** Get the last root row of the tree store
    \param store The GtkTreeStore to append a row to, can be null to use the store associated with the tree
    \return GtkTreeIter* to the last row, NULL on failure/if there isn't a row
    */
    GtkTreeIter* getLastIter(GtkTreeStore *store=NULL) {
        if (store==NULL) // get the local tree store
            store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
        GtkTreeIter* iter = new GtkTreeIter;
        if (gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(store), iter, NULL,gtk_tree_model_iter_n_children(GTK_TREE_MODEL(store), NULL)-1))
            return iter;
        else
            return NULL;
    }

    /** append a row to a given store
    The returned row must be deleted
    \param store The GtkTreeStore to append a row to, can be null to use the store associated with the tree
    \return GtkTreeIter* to the created row, NULL on failure
    */
    GtkTreeIter* appendRow(GtkTreeStore *store=NULL) {
        if (store==NULL) // get the local tree store
            store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
        GtkTreeIter* iter=NULL;
        if (store) {
            iter=new GtkTreeIter;
            if (iter)
                gtk_tree_store_append(store, iter, NULL);
        }
        return iter;
    }

    /** Get a GtkTreeIter.
    If the sameRow is requested, try to get it, if no rows, then create a new row and append as last.
    \param store The GtkTreeStore to append a row to
    \param sameRow try to return the last row as the iter if true, otherwise, create a new row and append
    \param sameColumn don't increment the column, use the same column
    */
    GtkTreeIter* appendNewIter(GtkTreeStore *store, gboolean sameRow, gboolean sameColumn) {
        GtkTreeIter* iter=NULL;
        if (!sameColumn)
            next();
        if (sameRow) { // find the correct column and add to the same row
            iter=getLastIter(store);
        }

        if (iter==NULL)
            iter=appendRow(store);
        return iter;
    }

protected:
    GtkWidget *tree; ///< The tree of items for the selection widget
    GtkTreeSelection *selection; ///< The tree selection box

    /** Find which column is current
    \return The column number matching current(), -1 on failure
    */
    int currentColumnNumber(void) {
        if (!getCount())
            return -1;
        int which=0;
        if (getCount()!=1){
            SelectionColumn *currentCol=current(); // remember the current column number
            grab(1);
            prev(); // get the last column
            while (next()!=currentCol) // keep getting the next unitl it is the same as the original state
                which++;
        }
        return which; // return the column number, starting from zero, the LinkList's state is as it was upon entry
    }
public:

    /** Simple constructor, giving a Selection class with no columns.
    */
    Selection(void) {
        init();
    }

    /** Construct a Selection widget with one column and assigns a callback
    When this constructor is used, one text column is added. This automates everything for you if you only want a simple text column selection.
    \param listName The title for the Selection
    \param callback the callback to use when a selection is made
    \param data User data to pass to the callback.
    */
    Selection(const char *listName, GCallback callback=NULL, void *data=NULL) {
        init(listName, callback, data);
    }

    virtual ~Selection(){
        //cout<<"Selection::~Selection"<<endl;
    }

    /** Initialise a new Selection GUI.
    Adds a new column and the related callback
    \param listName The title for the Selection
    \param callback the callback to use when a selection is made (if callback==NULL then it is not connected)
    \param data the user data to pass to the selection callback static method.
    */
    void init(const char *listName, GCallback callback=NULL, void *data=NULL) {
        init();
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));

        SelectionColumn *sc= new SelectionColumn(listName, getCount()); // create the new column
        //        gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE); // use this to hide the headers

        *sc<<pair<string*, GtkCellRendererText*>(new string("text"), GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new()));

        add(sc);

        if (callback && data)
            setSelectionCallback("changed", callback, data);
        else
            setSelectionCallback(callback);
    }

    /** Calls the callback when the "changed" event is called on the selection.
    \param callback The callback to call when a "changed" event is caught
    */
    void setSelectionCallback(GCallback callback) {
        setSelectionCallback("changed", callback, this);
    }

    /** Setup a new selection callback
    \param event The event to connect to
    \param callback The callback to call when the event is encountered
    \param data The user data to pass to the callback
    */
    void setSelectionCallback(const char *event, GCallback callback, gpointer data) {
        changedHandlerID=g_signal_connect(selection, event, callback, data);
    }

    /** Sets the tree to call the callback when the button is released.
    This is good for generic event and table related event handling.
    For example your class could use the following :
    \code
    Selection list;
    list.setTreeCallback("button-release-event", reinterpret_cast<GCallback>(treeCallBack), (void*)this);
    \endcode
    \param event The event to connect to.
    \param callback The callback to execute when the button is released (after clicking the tree)
    \param data The user data to pass to the callback on execution.
    */
    void setTreeCallback(const char *event, GCallback callback, gpointer data) {
        g_signal_connect(tree, event, G_CALLBACK(callback), data);
    }

    /** add a new column to the Selection
    */
    void add(SelectionColumn *newCol) {
        LinkList<SelectionColumn *>::add(newCol); // add to the list of columns

        gtk_tree_view_append_column(GTK_TREE_VIEW(tree), current()->getColumn());

        // find the types and create a new store, then attach to the current treeview
        // find the total number of renderers.
//        int rendCnt=0; // in the future support one column with many renderers
//        for (int i=0;i<getCount();i++)
//            rendCnt+=next()->getCount();
//        GType types[rendCnt]; // create the type holder
//        grab(1); prev(); // get the last column and iterate from there
//        int index=0;
//        for (int i=0;i<getCount();i++){
//            for (int j=0; j<current()->getCount();j++){
//                types[index++]=current()->next()->second;
//            }
//            next();
//        }

        GType *types =new GType[getCount()]; // create the type holder
        grab(1);
        prev(); // get the last column and iterate from there
        for (int i=0; i<getCount(); i++) {
            types[i]=next()->current()->second;
        }

        while (next()!=newCol) ; // get the linklist back to its original location

        GtkTreeStore *store = gtk_tree_store_newv(getCount(), types); // get the column type
        gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(store));
        g_object_unref(store);
		delete [] types;
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    }

    /** \brief change the callback data to be returned with the callback.
    \param callback the callback function to be called upon selection change
    \param data the data to be returned with the selection changed callback.
    */
    void setCallbackAfter(GCallback callback, void *data) {
        g_signal_connect_after(selection, "changed", G_CALLBACK(callback), data);
    }

    /*! Gets the widget for loading into containers
    */
    GtkWidget *getWidget(void) {
        return tree;
    }

    /** A method to show the Selection widget
    */
    void show(void) {
        gtk_widget_show_all(tree);
    }

    /** A method to hide the Selection widget
    */
    void hide(void) {
        gtk_widget_hide(tree);
    }

    /** Get the selected text from the current() column
    \param value The character array to hold the selected text. NOTE: Ensure the length of the char * passed in is large enough to contain the selected text.
    */
    void getSelection(char *value) {
		string valueS;
        getSelection(valueS);
        strcpy(value,valueS.c_str());
    }

    /** Get the selected text from the current() column
    \param value The character array to hold the selected text.
    */
    void getSelection(string &value){
        GtkTreeIter iter;
        GtkTreeModel *model;
        char *val;

        int colNum=currentColumnNumber();
        if (colNum>=0){
            if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
                gtk_tree_model_get(model, &iter, colNum, &val,  -1);
                if (val)
                    value=val;
                else
                    value.resize(0);
                g_free(val);
            }
        }
    }

    /** add a text item to the selection column
    if you want to add to a different column, shift the inherited LinkList<SelectionColumn *> to the correct one.
    For example use one of these to locate the desired column prior to adding : LinkList<SelectionColumn *>::grab, LinkList<SelectionColumn *>::next, LinkList<SelectionColumn *>::prev
    \param text the text item to add to the list
    \param sameRow Use the same row. Defaults to false, append a new row
    \param sameColumn Use the same column, defaults to true
    \return This Selection object
    */
    void add(const char *text, gboolean sameRow=false, gboolean sameColumn=true) {
        GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
        GtkTreeIter* iter=appendNewIter(store, sameRow, sameColumn);

        int whichColumn=currentColumnNumber();
        if (iter) {
            gtk_tree_store_set(store, iter, whichColumn, text, -1);
            delete iter;
        }
    }

    /** add a pixbuf item to the selection column
    if you want to add to a different column, shift the inherited LinkList<SelectionColumn *> to the correct one.
    For example use one of these to locate the desired column prior to adding : LinkList<SelectionColumn *>::grab, LinkList<SelectionColumn *>::next, LinkList<SelectionColumn *>::prev
    \param pixbuf the GdkPixbuf item to add to the list
    \param sameRow Use the same row. Defaults to false, append a new row
    \param sameColumn Use the same column, defaults to true
    \return This Selection object
    */
    void add(GdkPixbuf *pixbuf, gboolean sameRow=false, gboolean sameColumn=true) {
        GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
        GtkTreeIter* iter=appendNewIter(store, sameRow, sameColumn);

        int whichColumn=currentColumnNumber();
        if (iter) {
            gtk_tree_store_set(store, iter, whichColumn, pixbuf, -1);
            delete iter;
        }
    }

    /** Clear all the contents of the selection
    */
    void clear(void) {
		if (changedHandlerID)
			g_signal_handler_block(selection, changedHandlerID);
        //GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (tree)));
        GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (tree));
        GtkTreeIter  iter;

        if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
            return;
        gtk_tree_store_clear(GTK_TREE_STORE(model));

		if (changedHandlerID)
			g_signal_handler_unblock(selection, changedHandlerID);
    }
//    /* add a pixbuf item to the selection column
//    if you want to add to a different column, shift the inherited LinkList<SelectionColumn *> to the correct one.
//    For example use one of these to locate the desired column prior to adding : LinkList<SelectionColumn *>::grab, LinkList<SelectionColumn *>::next, LinkList<SelectionColumn *>::prev
//    \param firstColumn, which column to start with, 1 for the first column
//    \return This Selection object
//    */
//    void addRow(int firstColumn, ...) {
//        GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
//        GtkTreeIter* iter=appendNewIter(store, false, false);
//        grab(firstColumn);
//        va_list var_args;
//        va_start (var_args, firstColumn);
//        gtk_tree_store_set_valist (store, iter, var_args);
//        va_end (var_args);
//    }

//    /* add a list of items to the same row in each column - starting from the first column
//    \param ll the LinkList of items to add to the list on the current row
//    \return This Selection object
//    */
//    void add(LinkList<GValue> &ll) {
//  //      int whichColumn=currentColumnNumber(); // restore current column at the end
//        GtkTreeIter iter;
//        GtkTreeStore *store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
//        int cols[ll.getCount()];
//        GValue values[ll.getCount()];
//
//        ll.grab(1); ll.prev(); // get the last element on the linked list
//        for (int i=0;i<ll.getCount();i++){
//            cols[i]=i;
//            g_value_init (&values[i], next()->current->second); // setup the current type
//            =ll.next();
//        }
//
//        gtk_tree_store_append(store, &iter, NULL);
//        gtk_tree_store_set_valuesv(store, &iter, cols, values, ll.getCount());
////        grab(whichColumn); // restore the column state
//    }

    /** add a text item to the Selection column
    if you want to add to a different column, shift the inherited LinkList<SelectionColumn *> to the correct one.
    For example use one of these to locate the desired column prior to adding : LinkList<SelectionColumn *>::grab, LinkList<SelectionColumn *>::next, LinkList<SelectionColumn *>::prev
    \param text the text item to add
    \return This Selection object
    */
    Selection & operator<<(const string text) {
        add(text.c_str());
        return *this;
    }

    /** add a text item to the Selection column
    if you want to add to a different column, shift the inherited LinkList<SelectionColumn *> to the correct one.
    For example use one of these to locate the desired column prior to adding : LinkList<SelectionColumn *>::grab, LinkList<SelectionColumn *>::next, LinkList<SelectionColumn *>::prev
    \param text the text item to add
    \return This Selection object
    */
    Selection & operator<<(const char *text) {
        add(text);
        return *this;
    }

    /** add a GdkPixbuf item to the Selection column.
    If you want to add to a different column, shift the inherited LinkList<SelectionColumn *> to the correct one.
    For example use one of these to locate the desired column prior to adding : LinkList<SelectionColumn *>::grab, LinkList<SelectionColumn *>::next, LinkList<SelectionColumn *>::prev
    \param pixbuf the text item to add
    \return This Selection object
    */
    Selection & operator<<(GdkPixbuf *pixbuf) {
        add(pixbuf);
        return *this;
    }

//    /* add a row with multiple items
//    starts from row 0
//    \param ll the LinkList of items to add
//    \return This Selection object
//    */
//    Selection & operator<<(LinkList<void*> &ll) {
//        add(ll);
//        return *this;
//    }

    /** add a new column to the Selection.
    \param newCol the SelectionColumn to add
    \return This Selection object
    */
    Selection & operator<<(SelectionColumn *newCol) {
        add(newCol);
        return *this;
    }

    /** Activates a row for the current LinkList selected column
    For example :
    \code
    selection.grab(2); // get the second column
    selection.activateRow("0"); // activate the first row
    selection.activateRow("1:1"); // activate the second row, second node of the tree
    \endcode
    \param path a GtkTreePath string, such as "A" for a root node number A or "A:B:C" root node A, sub-node B, sub-node C.
    */
    void selectRow(char *path){
        GtkTreePath *pathLocal = gtk_tree_path_new_from_string (path);
        //gtk_tree_view_row_activated(GTK_TREE_VIEW(tree), pathLocal, current()->getColumn());
        gtk_tree_selection_select_path(selection, pathLocal);
    }
};
#endif //SELECTION_H_
