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
#include "JackPortMonitorGui.H"
#include "DragNDrop.H"

void ClientIOGui::init() {
    *this<<BoxIS(true, true, true)<<portVBox.show();
    show();
}

ClientIOGui::ClientIOGui() {
    init();
}

ClientIOGui::ClientIOGui(string clientName_) {
    renameClient(clientName_);
    init();
}

void ClientIOGui::renameClient(string clientName_) {
    while (clientNameButton.getCount())
        *this>>clientNameButton.remove();
    clientNameButton<<BUTTONLABELSTRUCT(clientName_, NULL, NULL); // create a button which is the client name
    *this<<clientNameButton; // Add the button and the port verticall box to the HBox
}

void ClientIOGui::setPorts(vector<string> &portNames) {
    vector<string>::iterator p; // this will be the knownClient if it already exists
    for (p=portNames.begin(); p!=portNames.end(); ++p) {
        portButtons<<BUTTONLABELSTRUCT(p->c_str(), reinterpret_cast<GCallback>(NULL), NULL); // add a new button.
        portVBox<<BoxIS(true, false, true)<<portButtons.current(); // add the new button to the vertical box.
    }
}

void ClientIOGui::setPorts(map<string, map<string, vector<string> > > &portNames) {
    map<string, map<string, vector<string> > >::iterator p; // this will be the knownClient if it already exists
    for (p=portNames.begin(); p!=portNames.end(); ++p) { // the port names are the keys to the primary map
        portButtons<<BUTTONLABELSTRUCT(p->first.c_str(), reinterpret_cast<GCallback>(NULL), NULL); // add a new button.
        portVBox<<BoxIS(true, false, true)<<portButtons.current(); // add the new button to the vertical box.
    }
}

void ClientIOGui::reverseHBoxStacking() {
    *this>>Widget(clientNameButton.grab(1)).ref()>>portVBox.ref(); // make sure the widgets don't dissappear.
    *this<<BoxIS(true, true, true)<<clientNameButton.grab(1)<<portVBox;
    Widget(clientNameButton.grab(1)).unref(); // ensure they can be garbage collected by decrementing their reference.
    portVBox.unref();
}

string ClientIOGui::getFullPortName(GtkWidget *wid){
    string ret;
    int index;
    if ((index=portButtons.findIndex(wid))==0) // if not found. Indicate an error
        return ret;
    portButtons.grab(index);
    ret+=getClientName()+":"+portButtons.getLabel();
    return ret;
}

void ClientIOGui::clearAllWidgetConnections(){
    inputWidgetConnections=map<GtkWidget*, vector<GtkWidget*> >();
}

void ClientIOGui::addWidgetConnections(GtkWidget *wid, GtkWidget*cWid) {
    // if cWid isn't already known, then add it to the list.
    if (find(inputWidgetConnections[wid].begin(), inputWidgetConnections[wid].end(), cWid)==inputWidgetConnections[wid].end())
        inputWidgetConnections[wid].push_back(cWid);
    //cout<<inputWidgetConnections[wid].size()<<endl;
}

GtkWidget* ClientIOGui::getPortWidget(string portName) {
    GtkWidget *ret=NULL;
    for (int i=0; i<portButtons.getCount(); i++)
        if (portName==portButtons.getLabel()) // If the current port button is the one we are after, then return it
            ret=portButtons.current();
        else // if not the one we are after, then try the next
            portButtons.next();
    return ret;
}

void ClientIOGui::setupDrag(DragNDrop &dnd){
    dnd<<*this; // tell dnd that this class is the user data for DND callbacks (in the case of dragging)
    Widget(clientNameButton.current())>>dnd; // Tell the client button that it will be dragged around
    for (int i=0; i<portButtons.getCount(); i++) // tell each of the port buttons that they will be dragged around
        Widget(portButtons.next())>>dnd;
}

void ClientIOGui::setupDrop(DragNDrop &dnd){
    Widget(clientNameButton.current())<<dnd; // Tell the client button that it will be dragged to
    for (int i=0; i<portButtons.getCount(); i++) // tell each of the port buttons that they will be dragged to
        Widget(portButtons.next())<<dnd;
}

string ClientIOGui::getWidgetsPortName(GtkWidget *wid){
    string dragClientPortInfo;
    dragClientPortInfo=getClientName(); // start with the client name
    if (wid!=clientNameButton.current()){ // if it isn't the client widget, the find the actual port
        dragClientPortInfo+=':'; // append the port name after a semicolon delimiter
        int index;
        if ((index=portButtons.findIndex(wid))==0) // if not found. Indicate an error
            dragClientPortInfo=""; // use the empty string to indicate an error.
        else {
            portButtons.grab(index);
            dragClientPortInfo+=portButtons.getLabel(); // append the port name to indicate the complete client:port string to pass when dropped.
        }
    }
    return dragClientPortInfo;
}

void ClientIOGui::dragDataGet(GtkWidget *w, GtkSelectionData *selectionData, guint info) {
    // For this widget which has been dragged from here, find its client name and if it isn't the client button then the port name as well.
    //cout<<"ClientIOGui::dragDataGet"<<endl;
    string dragClientPortInfo=getWidgetsPortName(w);
    gtk_selection_data_set(selectionData, gtk_selection_data_get_target(selectionData), sizeof(char), (unsigned char*)dragClientPortInfo.c_str(), dragClientPortInfo.length());
}

void JackBaseWithPortNamesGui::reSyncPortGui(void) {
    inputPortGui.renameClient(getClientName());
    outputPortGui.renameClient(getClientName());
    outputPortGui.reverseHBoxStacking(); // show the client name on the right. This makes ports face each other

    // work through each client and the known ports ensuring the Gui elements are present.
    inputPortGui.setPorts(inputPortNamesAndConnections);
    outputPortGui.setPorts(outputPortNames);
}

bool JackBaseWithPortNamesGui::dragDataReceived(GtkWidget *w, GtkWidget *sourceWidget, int info, const unsigned char *selectionStoredData, int length) {
    //cout<<"JackBaseWithPortNamesGui::dragDataReceived"<<endl;
    string inputData((const char*)selectionStoredData);
    // if the client button is dropped on by an input client, attempt to connect all client ports together.
    // if the client button is dropped on by an input port, connect the port to all of this client's ports.
    if (info==JackPortMonitorGui::CONNECT_PORTS) // connecting ports
        if (w==inputPortGui.getClientWidget()) // if the dropped to widget is the client, then attempt to connect all client ports together
            connectPorts(inputPortGui.getClientName(), inputData); // connect as many ports as possible from the input client to the output client's ports
        else
            connectPorts(inputPortGui.getFullPortName(w), inputData); // connect as many ports as possible from the input client to the output client's ports
    if (info==JackPortMonitorGui::DISCONNECT_PORTS) // disconnecting ports
        if (w==outputPortGui.getClientWidget()) // if the dropped to widget is the client, then attempt to disconnect all client ports
            disconnectPorts(outputPortGui.getClientName(), inputData); // disconnect as many ports as possible from the input client to the output client's ports
        else
            disconnectPorts(outputPortGui.getFullPortName(w), inputData); // disconnect as many ports as possible from the input client to the output client's ports

    if (info==JackPortMonitorGui::CONNECT_PORTS || info==JackPortMonitorGui::DISCONNECT_PORTS) // either connecting nor disconnecting, all ok.
        return true;

    JackDebug().evaluateError(JACK_UNKNOWN_DND_TYPE_ERROR, "JackBaseWithPortNamesGui::dragDataReceived : unknown drop request, neither connecting nor disconnecting : bad GtkTargetEntry value");
    return false;
}

int JackBaseWithPortNamesGui::chooseTheTargetToUse(GtkWidget *w, GtkWidget *sourceWidget, DragNDrop &dnd){
    // find out whether the target widget (w) is in the inputs (returning CONNECT_PORTS) or outputs (returning DISCONNECT_PORTS).
    if (inputPortGui.getWidgetsPortName(w)!="") // if the widget belongs to the input ports, then the port name isn't empty.
        return JackPortMonitorGui::CONNECT_PORTS; // indicate that the ports are connecting, as the destination widget was found in the input ports.
    if (outputPortGui.getWidgetsPortName(w)!="") // if the widget belongs to the output ports, then the port name isn't empty.
        return JackPortMonitorGui::DISCONNECT_PORTS; // indicate that the ports are disconnecting, as the destination widget was found in the output ports.

    return JACK_DRAG_OPTION_UNKNOWN_ERROR; // return the error when the widget can't be found
}

void ConnectionSurface::drawConnections(const map<GtkWidget*, vector<GtkWidget*> > &connections) {
    //cout<<"ConnectionSurface::drawConnections"<<endl;
    cairo_t *cr=cairo_create(getSurface()); // get the cairo context to draw to
    cairo_set_source_rgb(cr, 1.,1.,1.); // set the cairo colour to draw with
    float width=1.5; // the line width
    cairo_set_line_width(cr, width);
    bool closed=true, filled=false; // whether to draw the arrow heads closed and filled.
    map<GtkWidget*, vector<GtkWidget*> >::const_iterator wids;
    for (wids=connections.begin(); wids!=connections.end(); ++wids) { // for each input port widget, draw an arrow to the output port widgets
        GtkWidget *inWid=(*wids).first;
        int inX, inY, inW, inH; // the input port widget's location and size
        Widget iWid(inWid);
        iWid.getLocationSize(inX, inY, inW, inH);

        vector<GtkWidget*>::const_iterator oWids;
        for (oWids=(*wids).second.begin(); oWids!=(*wids).second.end(); ++oWids) { // for each output port widget, draw an arrow from the input port to this widget
            int outX, outY, outW, outH; // the output port widget's location and size
            Widget((GtkWidget*)(*oWids)).getLocationSize(outX, outY, outW, outH);

            int surfaceW, surfaceH;
            getSize(surfaceW, surfaceH);
            //cout<<"drawing from : "<<inX<<','<<inY<<" to "<<outX<<','<<outY<<endl;
            //cout<<"surface w, h "<<surfaceW<<'\t'<<surfaceH<<endl;
            CairoArrow().draw(cr, 0, outY+outH/2, surfaceW, inY+inH/2, .08, width, closed, filled);
        }
    }

    // destroy the cairo context and redraw the widget
    cairo_destroy(cr);
    //gtk_widget_queue_draw(getWidget());
}

void JackPortMonitorGui::init() {
    if (knownClients.size()>0) { // remove any known clients.
        for (int i=0; i<knownClients.size(); i++)
            delete knownClients[i];
        knownClients.resize(0);
    }

    Buttons controlButtons;


    *this<<BoxIS(true, true, true)
    <<(VBox()<<BoxIS(true, true, true)
         <<(HBox()<<BoxIS(true, true, true)<<(Frame("Readers")<<outputPortBox.show()).show() // show the input ports
                <<BoxIS(true, true, true)<<connectionSurface // show the connection surface
                <<BoxIS(true, true, false)<<(Frame("Writers")<<inputPortBox.show()).show() // show the output ports
            ).show()
        <<BoxIS(true, false, false)
        <<(HBox()<<
                    (controlButtons<<BUTTONCHECKSTRUCT("Connect 1 to 1", reinterpret_cast<GCallback>(connect1To1Static), this)
                                   <<BUTTONCHECKSTRUCT("Autoconnect net clients", reinterpret_cast<GCallback>(autoConnectNetClientsPortsStatic), this))
           ).show()
       ).show();
    reSyncPorts();
    reSyncConnections();

    connectExposeEventCallback(reinterpret_cast<GCallback>(exposeEventStatic), this);
    connectConfigureEventCallback(connectionSurface.getWidget(), reinterpret_cast<GCallback>(reDrawConnectionsStatic), this);

    controlButtons.grab(1); // the first button is the connect1To1 button, the second is the net autoconnect
    controlButtons.setActive(connect1To1); // sync the check box to the state of the connection mode
    controlButtons.next(); // the second is the net autoconnect
    controlButtons.setActive(autoConnectNetClients); // sync the check box to the state of the autoconnect flag
}

void JackPortMonitorGui::jackPortConnected(jack_port_id_t a, jack_port_id_t b, int connect) {
    gdk_threads_enter();
    JackPortMonitor::jackPortConnected(a, b, connect);
    gdk_threads_leave();
}

void JackPortMonitorGui::jackPortRegistered(jack_port_id_t port, int reg) {
    gdk_threads_enter();
    JackPortMonitor::jackPortRegistered(port, reg);
    gdk_threads_leave();
}

void JackPortMonitorGui::jackPortRenamed(jack_port_id_t port, const char *oldName, const char *newName) {
    gdk_threads_enter();
    JackPortMonitor::jackPortRenamed(port, oldName, newName);
    gdk_threads_leave();
}

void JackPortMonitorGui::reSyncPorts(void) {
    //cout<<"JackBaseWithPortNamesGui::reSyncPorts"<<endl;
    for (typename vector<JackBaseWithPortNames*>::iterator kc=knownClients.begin(); kc!=knownClients.end(); ++kc) {
        inputPortBox>>static_cast<HBox>(dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui).getWidget();
        outputPortBox>>static_cast<HBox>(dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->outputPortGui).getWidget();
    }

    JackPortMonitor::reSyncPorts();

    // go through each of the known clients and replace them with their Gui counterparts.
    for (typename vector<JackBaseWithPortNames*>::iterator kc=knownClients.begin(); kc!=knownClients.end(); ++kc) {
        JackBaseWithPortNames *oldJB=*kc;
        //cout<<"original ports "<<*oldJB<<endl;
        *kc=new JackBaseWithPortNamesGui; // TODO : check that there aren't any memory allocation issues here.
        *(*kc)=*oldJB;
        delete oldJB;
        //cout<<"new ports "<<*kc<<endl;
    }

    DragNDrop dnd; // Setup the drag and drop feature
    dnd<<(GtkTargetEntry){(char*)"CONNECT", 0, CONNECT_PORTS}<<(GtkTargetEntry){(char*)"DISCONNECT", 0, DISCONNECT_PORTS}; // setup a data type for the dnd system
    for (typename vector<JackBaseWithPortNames*>::iterator kc=knownClients.begin(); kc!=knownClients.end(); ++kc) {
        dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->reSyncPortGui();
        // ensure the client is shown in both half duplex modes.
        inputPortBox<<BoxIS(true,true,true)<<static_cast<HBox>(dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui).show();
        outputPortBox<<BoxIS(true,true,true)<<static_cast<HBox>(dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->outputPortGui).show();

        // connect the drag and drop feature for making connections - drags from inputs to outputs
        dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui.setupDrag(dnd); // input widgets will be dragged around - this one for making connections
        dnd<<*dynamic_cast<JackBaseWithPortNamesGui*>(*kc); // tell dnd that this class is the user data for DND callbacks (in the case of dropping)
        dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->outputPortGui.setupDrop(dnd); // output widgets will be dropped onto - this one for making connections

        // connect the drag and drop feature for disconnections - drags from outputs to inputs
        dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->outputPortGui.setupDrag(dnd); // output widgets will be dragged around - this one for disconnecting
        dnd<<*dynamic_cast<JackBaseWithPortNamesGui*>(*kc); // tell dnd that this class is the user data for DND callbacks (in the case of dropping)
        dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui.setupDrop(dnd); // input widgets will be dropped onto - this one for disconnecting
    }
}

void JackPortMonitorGui::reSyncConnections(void) {
    //cout<<"JackPortMonitorGui::reSyncConnections"<<endl;
    JackPortMonitor::reSyncConnections(); // resync the list of input port connections

    // fill the list of widget connections
    typename vector<JackBaseWithPortNames *>::iterator kc;
    for (kc=knownClients.begin(); kc!=knownClients.end(); ++kc) { // go through the connections for each known client
        dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui.clearAllWidgetConnections(); // make sure we can rebuild the list of widget connections from scratch
        map<string, map<string, vector<string> > >::iterator ipnac; // Iterate through each of the known input ports for this client
        for (ipnac=(*kc)->inputPortNamesAndConnections.begin(); ipnac!=(*kc)->inputPortNamesAndConnections.end(); ++ipnac) {
            string portName=(*ipnac).first;
            GtkWidget *ip=dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui.getPortWidget(portName);
            if (ip) {
                map<string, vector<string> >::iterator connectedClientsPorts; // Iterate through each of the known connected ports for this clients port
                for (connectedClientsPorts=(*ipnac).second.begin(); connectedClientsPorts!=(*ipnac).second.end(); ++connectedClientsPorts) {
                    CompareStrings cs((*connectedClientsPorts).first); // get the client name and create a functor which can compare a client pointer's client name to a client name string
                    typename vector<JackBaseWithPortNames *>::iterator cc=find_if(knownClients.begin(), knownClients.end(), cs);
                    if (cc!=knownClients.end()) { // if we have found the client, then add each of the connected the port's widgets to the list
                        vector<string>::iterator connectedPorts;
                        for (connectedPorts=(*connectedClientsPorts).second.begin(); connectedPorts!=(*connectedClientsPorts).second.end(); ++connectedPorts) {
                            GtkWidget *op=dynamic_cast<JackBaseWithPortNamesGui*>(*cc)->outputPortGui.getPortWidget(*connectedPorts); // find the output port widget which matches the port
                            if (op) {// If we have successfully located the corresponding output port widget, then add it to the list
                                //cout<<"port ... "<<(*kc)->getClientName()<<":"<<portName<<" connected client "<<cs.cn<<" port found "<<*connectedPorts<<endl;
                                dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui.addWidgetConnections(ip, op); // get the connected widgets for this connection
                                //cout<<(*kc)->inputPortGui.getWidgetConnections(ip).size()<<endl;
                            } else // if we couldn't find the corresponding port widget, print the error.
                                JackDebug().evaluateError(JACK_PORTNAME_NOTFOUND_ERROR, string("port ... ")+(*kc)->getClientName()+":"+portName+" connected client "+cs.cn+" couldn't find the port "+*connectedPorts);
                        }
                    } else
                        JackDebug().evaluateError(JACK_PORTNAME_NOTFOUND_ERROR, string("port ... ")+(*kc)->getClientName()+":"+portName+" couldn't find the connected client "+cs.cn);
                }

            } else
                JackDebug().evaluateError(JACK_PORTNAME_NOTFOUND_ERROR, string("JackPortMonitorGui::reSyncConnections port ")+portName+" not found");
        }
    }
    reDrawConnections();
}

void JackPortMonitorGui::reDrawConnections() {
    //cout<<"JackPortMonitorGui::reDrawConnections : knowclient count = "<<knownClients.size()<<endl;
    // Draw connections to the connectionSurface
    connectionSurface.clear();
    typename vector<JackBaseWithPortNames *>::iterator kc;
    for (kc=knownClients.begin(); kc!=knownClients.end(); ++kc) // go through the connections for each known client
        connectionSurface.drawConnections(dynamic_cast<JackBaseWithPortNamesGui*>(*kc)->inputPortGui.getInputWidgetConn());
    gtk_widget_queue_draw(connectionSurface.getWidget());
}

JackPortMonitorGui::JackPortMonitorGui()  : JackPortMonitor() {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(bool monitorPorts) : JackPortMonitor(monitorPorts) {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(bool monitorPorts, bool autoConnectNetClientsIn) : JackPortMonitor(monitorPorts, autoConnectNetClientsIn) {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(string clientName_) : JackPortMonitor(clientName_) {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(string clientName_, bool monitorPorts) : JackPortMonitor(clientName_, monitorPorts) {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(string clientName_, bool monitorPorts, bool autoConnectNetClientsIn) : JackPortMonitor(clientName_, monitorPorts, autoConnectNetClientsIn) {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(string clientName_, string serverName) : JackPortMonitor(clientName_, serverName) {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(string clientName_, string serverName, bool monitorPorts) : JackPortMonitor(clientName_, serverName, monitorPorts) {
    init();
}

JackPortMonitorGui::JackPortMonitorGui(string clientName_, string serverName, bool monitorPorts, bool autoConnectNetClientsIn) : JackPortMonitor(clientName_, serverName, monitorPorts, autoConnectNetClientsIn) {
    init();
}
