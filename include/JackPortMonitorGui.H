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
#ifndef JACKPORTMANAGERGUI_H_
#define JACKPORTMANAGERGUI_H_

#include "JackPortMonitor.H"
#include "Box.H"
#include "Buttons.H"
#include "Frame.H"
#include "Surface.H"
#include "CairoArrow.H"
#include "EventBox.H"

/** Each client has a button for all ports, segmented into input and output ports.
This class is a half duplex representation of the client.
The client name is packed horizontally next to the vertically stacked port box.
Dragging a client button or port button will send either the client's name or the client and port name using ':' as the delimiter name when dropped.
*/
class ClientIOGui : public HBox {
    Buttons clientNameButton; ///< Button representing all of the half duplex client ports.
    VBox portVBox; ///< A holder for the port buttons
    Buttons portButtons; ///< The clients half duplex ports. either input or output.
    map<GtkWidget*, vector<GtkWidget*> > inputWidgetConnections; ///< Each of the port buttons map to connected port buttons.

    /** Initialiser loads the vBox and shows the hBox.
    */
    void init();

    /** When the drag widget is dropped, this is executed on the widget from where it drag started.
    \param w the object which is the source of the drag.
    \param selectionData the data to send.
    \param info the info that has been registered with the target in the GtkTargetList.
    */
    virtual void dragDataGet(GtkWidget *w, GtkSelectionData *selectionData, guint info);

public:
    /** Construct a half duplex port Gui representation.
    \param clientName The name of the client this half duplex lane.
    */
    ClientIOGui();

    /** Construct a half duplex port Gui representation.
    \param clientName_ The name of the client this half duplex lane.
    */
    ClientIOGui(string clientName_);

   virtual ~ClientIOGui() {} ///< Destructor

    void renameClient(string clientName_);

    /** Create the widgets for each of the ports.
    \param portName A vector of port names.
    */
    void setPorts(vector<string> &portNames);

    /** Create the widgets for each of the ports.
    \param portNames Ports which are mapped to their connected ports.
    */
    void setPorts(map<string, map<string, vector<string> > > &portNames);

    void reverseHBoxStacking();

    /** Get the client name this class represents.
    \return The client name.
    */
    string getClientName(void){
        return clientNameButton.getLabel();
    }

    /** Given a widget, return the full port name "client:port".
    \param wid The port widget to find and create the full port name for.
    \return A full port name "client:port" or an empty string if not found.
    */
    string getFullPortName(GtkWidget *wid);

    /** Clear the list of know widget connections.
    */
    void clearAllWidgetConnections(void);

    /** Add a widget to the connection map for a specific port button widget.
    \param wid The port widget whos connected widgets are of interest.
    \param cWid The widget which is connected to the port wid.
    */
    void addWidgetConnections(GtkWidget *wid, GtkWidget*cWid);

    /** Find the port button widget which has the same name as the portName.
    \param portName The port name to search for.
    \return The widget which matches the port name, NULL if not found.
    */
    GtkWidget* getPortWidget(string portName);

    /** Get the port name from the widget.
    \param wid The port widget whos connected widgets are of interest.
    \return The port name if found, or an empty string on error.
    */
    string getWidgetsPortName(GtkWidget *wid);

    /** Get the input widget connections.
    \return The mapping of all output widgets to this input widget.
    */
    map<GtkWidget*, vector<GtkWidget*> > &getInputWidgetConn(){return inputWidgetConnections;}

    /** Setup the drag connections for widgets.
    \param dnd The drag and drop class.
    */
    void setupDrag(DragNDrop &dnd);

    /** Setup the drop connections for widgets.
    \param dnd The drag and drop class.
    */
    void setupDrop(DragNDrop &dnd);

    /** Return the GtkWidget of the client.
    \return The client widget on success or NULL if it doesn't exist.
    */
    GtkWidget *getClientWidget(){return clientNameButton.current();}
};

/** A full duplex client GUI components. Maintains two half duplex port systems for input and output.
Inherits from an EventBox to handle drag and drop features.

*/
class JackBaseWithPortNamesGui : public JackBaseWithPortNames, public EventBox {
    /** Executed when the target is dropped.
    This method specifies whether we are connecting or disconnecting ports. Drags from inputs connect and drags from outputs disconnect.
    \param w the GtkWidget which is the destination of the drag.
    \param sourceWidget The widget being dragged.
    \param dnd The drag and drop data contexts
    \return CONNECT_PORTS or DISCONNECT_PORTS when connecting or disconnecting respectively. Returns <0 on error.
    */
    virtual int chooseTheTargetToUse(GtkWidget *w, GtkWidget *sourceWidget, DragNDrop &dnd);

public:
    ClientIOGui inputPortGui; ///< The input port gui
    ClientIOGui outputPortGui; ///< The output port gui

    virtual ~JackBaseWithPortNamesGui() {} ///< Destructor

    /** Resynchronise the Gui from the knows set of ports.
    */
    void reSyncPortGui(void);

    /** Executed when the user drops and completes the drag.
    This is executed in the destination widget's class.
    \param w the GtkWidget which is the destination of the drag.
    \param info the info that has been registered with the target in the GtkTargetList.
    \param selectionStoredData The raw data which is sent by the other side, cast to a unsigned char pointer.
    \param length The length of the stored data
    \return true on success or false on failure
    */
    virtual bool dragDataReceived(GtkWidget *w, GtkWidget *sourceWidget, int info, const unsigned char *selectionStoredData, int length);
};

/** The surface to draw connections to.
*/
class ConnectionSurface : public Surface {

public:
    /** On the surface, draw arrows from input ports to connected output ports.
    \param connections The map of connections between input port widgets and the vector of output port widgets.
    */
    void drawConnections(const map<GtkWidget*, vector<GtkWidget*> > &connections);
};

/** GUI to give control and a visual of the various Jack clients/ports and their connections.
The inherited horizontally stacking box allows the input clients, connections and output clients to be stacked.

NOTE: This class requires linking against the gtkIOStream library.
*/
class JackPortMonitorGui : public JackPortMonitor, public HBox {

    VBox inputPortBox; ///< A box with all of the input half duplex GUIs stacked.
    VBox outputPortBox; ///< A box with all of the output half duplex GUIs stacked.

    ConnectionSurface connectionSurface; ///< Draws client port connections.

    static bool exposeEventStatic(GtkWidget *widget, GdkEvent *event, gpointer data){
        static_cast<JackPortMonitorGui*>(data)->reSyncConnections();
        return true;
    }

    /** Initialiser for this class.
    */
    void init();

    /** Method to handle the occasion where ports were connected or disconnected.
    \param a A port (dis)connected
    \param b A port (dis)connected
    \param connect 0 for connection removed, connection made otherwise
    */
    virtual void jackPortConnected(jack_port_id_t a, jack_port_id_t b, int connect);

    /** Method to handle to handle port registration or deregistration.
    \param port The port which was (de)registered.
    \param reg Zero if the port is deregistered, otherwise registration.
    */
    virtual void jackPortRegistered(jack_port_id_t port, int reg);

    /** Method to handle port renaming.
    \param port The port which was renamed.
    \param oldName The original name of the port.
    \param newName The new name of the port.
    */
    virtual void jackPortRenamed(jack_port_id_t port, const char *oldName, const char *newName);

    /** When a configure-event is triggered on the connection surface,
    \param widget The widget receiving the event.
    \param event The event which triggered the configure-event signal
    \param data The instance of Surface which is associated with the widget/event
    \return true to finish processing, false to allow other handlers to process.
    */
    static gboolean reDrawConnectionsStatic(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
        //cout<<"JackPortMonitorGui::reDrawConnectionsStatic"<<endl;
        static_cast<JackPortMonitorGui*>(data)->reDrawConnections();
        return false;
    }

    /** When checked, then connections are made 1 to 1. When not checked then connections are made all to all.
    \param widget The widget receiving the event.
    \param data The instance of Surface which is associated with the widget/event
    */
    static void connect1To1Static(GtkWidget *widget, gpointer data) {
        static_cast<JackPortMonitorGui*>(data)->connect1To1=Buttons::getActive(widget);
        static_cast<JackPortMonitorGui*>(data)->reSyncPorts(); // resyncronise all of the clients with the original client.
        static_cast<JackPortMonitorGui*>(data)->reSyncConnections();
    }

    /** When checked, then network clients are auto-connected to system ports.
    \param widget The widget receiving the event.
    \param data The instance of Surface which is associated with the widget/event
    */
    static void autoConnectNetClientsPortsStatic(GtkWidget *widget, gpointer data) {
        if (static_cast<JackPortMonitorGui*>(data)->autoConnectNetClients=Buttons::getActive(widget))
            static_cast<JackPortMonitorGui*>(data)->autoConnectNetClientsPorts();
    }
public:
    enum {CONNECT_PORTS, DISCONNECT_PORTS}; ///< Possible drag and drop operations

    /** Rebuild the client and port connection GUI from the client's knowledge of their own IO ports.
    */
    virtual void reSyncPorts(void);

    /** Ensure all ports connections are depicted.
    */
    virtual void reSyncConnections(void);

    /** Redraw all of the connections on the connectionSurface.
    */
    void reDrawConnections();

    /** Constructor. Doesn't connect the jack client.
    By default monitors ports.
    */
    JackPortMonitorGui();

    /** Constructor.  Doesn't connect the jack client.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    */
    JackPortMonitorGui(bool monitorPorts);

    /** Constructor.  Doesn't connect the jack client.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    \param autoConnectNetClientsIn If true, autoconnect network clients.
    */
    JackPortMonitorGui(bool monitorPorts, bool autoConnectNetClientsIn);

    /** Constructor. Connecting the client to the default server.
    By default monitors ports.
    \param clientName_ The client name, which will initiate a server connection.
    */
    JackPortMonitorGui(string clientName_);

    /** Constructor. Connecting the client to a particular server.
    \param clientName_ The client name, which will initiate a server connection.
    \param monitorPorts If true, then monitor changes to the prots and connections.
    */
    JackPortMonitorGui(string clientName_, bool monitorPorts);

    /** Constructor. Connecting the client to a particular server.
    \param clientName_ The client name, which will initiate a server connection.
    \param monitorPorts If true, then monitor changes to the prots and connections.
    \param autoConnectNetClientsIn If true, autoconnect network clients.
    */
    JackPortMonitorGui(string clientName_, bool monitorPorts, bool autoConnectNetClientsIn);

    /** Constructor. Connecting the client to a particular server.
    By default monitors ports.
    \param clientName_ The client name, which will initiate a server connection.
    \param serverName The server to connect to.
    */
    JackPortMonitorGui(string clientName_, string serverName);

    /** Constructor. Connecting the client to a particular server.
    \param clientName_ The client name, which will initiate a server connection.
    \param serverName The server to connect to.
    \param monitorPorts If true, then monitor changes to the prots and connections.
    */
    JackPortMonitorGui(string clientName_, string serverName, bool monitorPorts);

    /** Constructor. Connecting the client to a particular server.
    \param clientName_ The client name, which will initiate a server connection.
    \param serverName The server to connect to.
    \param monitorPorts If true, then monitor changes to the prots and connections.
    \param autoConnectNetClientsIn If true, autoconnect network clients.
    */
    JackPortMonitorGui(string clientName_, string serverName, bool monitorPorts, bool autoConnectNetClientsIn);

    virtual ~JackPortMonitorGui(){} ///< Destructor
};
#endif // JACKPORTMANAGERGUI_H_
