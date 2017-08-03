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
#include "JackPortMonitor.H"

void JackPortMonitor::init(bool monitorPorts) {
    init(monitorPorts, false); // start by default not autoconnecting network clients - this is a good security decision.
}

void JackPortMonitor::init(bool monitorPorts, bool autoConnectNetClientsIn){
    autoConnectNetClients=autoConnectNetClientsIn; // start not in silent mode
    if (!client) // the client has to exist to monitor port connections
        connect(JACK_PORT_MONITOR_CLIENT_NAME);
    if (monitorPorts)
        connectPortMonitoringCallbacks();
    if (jack_activate(client))
        JackDebug().evaluateError(JACK_ACTIVATE_ERROR);

    if (autoConnectNetClients){
        autoConnectNetClientsPorts();
        run(); // run the auto connection thread
    }
    reSyncPorts();
    reSyncConnections();
}


int JackPortMonitor::disconnect(void) {
    int res=JackBase::disconnect();
    for (vector<JackBaseWithPortNames *>::iterator kc=knownClients.begin(); kc!=knownClients.end(); ++kc)
        (*kc)->setClient(client);
    return res;
}

int JackPortMonitor::connect(const string &clientName_, const string &serverName) {
    int res=JackBase::connect(clientName_, serverName);
    for (vector<JackBaseWithPortNames *>::iterator kc=knownClients.begin(); kc!=knownClients.end(); ++kc)
        (*kc)->setClient(client);
    return res;
}

void JackPortMonitor::autoConnectNetClientsPorts(void) {
    cout<<"JackPortMonitor::autoConnectNetClientsPorts"<<endl;
    if (client) {
        vector<string> slavePorts; // find all ports with the '_slave_' in their names.
        if (getPortListAndCount((JackPortFlags)(JackPortIsInput), &slavePorts, "_slave_", NULL)<0)
            JackDebug().evaluateError(JACK_NETPORT_AUTOCONNECT_ERROR, "Problem finding input (writeable) ports. Skipping input ports.");
        if (getPortListAndCount((JackPortFlags)(JackPortIsOutput), &slavePorts, "_slave_", NULL)<0)
            JackDebug().evaluateError(JACK_NETPORT_AUTOCONNECT_ERROR, "Problem finding output (readable) ports. Skipping output ports.");

        // Find the list of unique client names
        vector<string> uniqueClientNames; // find a unique set of client names.
        for (vector<string>::iterator pn=slavePorts.begin(); pn!=slavePorts.end(); ++pn) {
            //cout<<"JackPortMonitor::autoConnectNetClientsPorts : found port "<<*pn<<endl;
            string cn=clientNameFromPortName(*pn);
            if (cn.size() && find(uniqueClientNames.begin(), uniqueClientNames.end(), cn.c_str()) == uniqueClientNames.end()) // if not present, then add to the list
                uniqueClientNames.push_back(cn);
        }

        // For each unique client name, attempt to auto-connect ports to the system.
        for (vector<string>::iterator cn=uniqueClientNames.begin(); cn!=uniqueClientNames.end(); ++cn){
            connectPorts("system", *cn);
            connectPorts(*cn, "system");
        }
    }
}

void *JackPortMonitor::threadMain(void){
    while (running()){ // while this thread is running - wait to be told to scan and connect network connections.
        cond.lock(); // lock the mutex and wait until ready.
        cond.wait();
        cond.unLock();
        autoConnectNetClientsPorts();
    }
    return NULL;
}

void JackPortMonitor::connectPortMonitoringCallbacks(void) {
    if (client) {
        connectPortRenameCallback();
        connectPortRegistrationCallback();
        connectPortConnectCallback();
    } else
        JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);
}

void JackPortMonitor::connectPortRenameCallback(void){
        if (jack_set_port_rename_callback(client, reinterpret_cast<JackPortRenameCallback>(jackPortRenameCallback), this)!=NO_ERROR)
            JackDebug().evaluateError(JACK_PORT_CALLBACK_SETUP_ERROR, " jack_set_port_rename_callback.");
}

void JackPortMonitor::connectPortRegistrationCallback(void){
        if (jack_set_port_registration_callback(client, (JackPortRegistrationCallback)jackPortRegistrationCallback, this)!=NO_ERROR)
            JackDebug().evaluateError(JACK_PORT_CALLBACK_SETUP_ERROR, " jack_set_port_registration_callback.");
}

void JackPortMonitor::connectPortConnectCallback(void){
        if (jack_set_port_connect_callback(client, reinterpret_cast<JackPortConnectCallback>(jackPortConnectCallback), this)!=NO_ERROR)
            JackDebug().evaluateError(JACK_PORT_CALLBACK_SETUP_ERROR, " jack_set_port_connect_callback.");
}

void JackPortMonitor::breakDownPortsToClients(vector<jack_port_t *> &ports) {
    for (vector<jack_port_t *>::iterator p=ports.begin(); p!=ports.end(); ++p) {
        CompareStrings cs(clientNameFromPort(*p)); // get the client name and create a functor which can compare a client pointer's client name to a client name string
        vector<JackBaseWithPortNames *>::iterator kc;
        JackBaseWithPortNames *c;
        // first check that this client is known ... if not then make it known
        kc=find_if(knownClients.begin(), knownClients.end(), cs);
        if (kc==knownClients.end()) { // if at the end, then it doesn't exist, so add
            knownClients.push_back(c=new JackBaseWithPortNames); // TODO : check memory creation here for errors.
            c->setClientName(cs.cn);
            c->setClient(client);
            c->connect1To1=connect1To1;
        } else // already exists
            c=*kc;
        // c now points to the client, so add it to the set of known ports.
        if (&ports==&inputPorts) {
            c->addInputPort(*p);
            c->inputPortNamesAndConnections[jack_port_short_name(*p)]=map<string, vector<string> >(); // each input port starts with an empty list of connections
        } else {
            c->addOutputPort(*p);
            c->outputPortNames.push_back(jack_port_short_name(*p));
        }
    }
}

void JackPortMonitor::reSyncPorts(JackPortFlags flags) {
    vector<jack_port_t *> *portsIO; // work out whether we are finding input or output ports and point to them.
    (flags==JackPortIsInput) ? portsIO=&inputPorts : portsIO=&outputPorts;

    // start with the number of physical ports and add them to the known list of ports
    getPortListAndCount((JackPortFlags)(JackPortIsPhysical|flags), portsIO, NULL, NULL);

    // find input and output ports which aren't physical and add.
    vector<jack_port_t *> ports;
    getPortListAndCount(flags, &ports, NULL, NULL);
    for (vector<jack_port_t *>::iterator p=ports.begin(); p!=ports.end(); ++p)
        if(std::find(portsIO->begin(), portsIO->end(), *p) == portsIO->end()) // if not already present. i.e. not a physical input port then add
            portsIO->push_back(*p);

    // Add the known ports to the list of known clients, this involves associating each port with the client object and setting their input and output ports.
    breakDownPortsToClients(*portsIO);
}

void JackPortMonitor::reSyncPorts(void) {
    //cout<<"JackPortMonitor::reSyncPorts"<<endl;
    JackBase::reSyncPorts();
    if (knownClients.size()>0) { // remove any known clients.
        for (int i=0; i<knownClients.size(); i++)
            delete knownClients[i];
        knownClients.resize(0);
    }

    if (client) { // recreate known clients
        reSyncPorts(JackPortIsInput); // get input ports and break down to client/port objects.
        reSyncPorts(JackPortIsOutput); // get output ports and break down to client/port objects.
    } else
        JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);
}

void JackPortMonitor::reSyncConnections(void) {
    //cout<<"JackPortMonitor::reSyncConnections"<<endl;
    vector<JackBaseWithPortNames *>::iterator kc;
    for (kc=knownClients.begin(); kc!=knownClients.end(); ++kc)
        (*kc)->findInputConnections(); // find all of the connections from inputs for each client.
}

JackPortMonitor::JackPortMonitor() : JackBase(JACK_PORT_MONITOR_CLIENT_NAME) {
    bool monitorPorts=true;
    init(monitorPorts);
}

JackPortMonitor::JackPortMonitor(bool monitorPorts) : JackBase(JACK_PORT_MONITOR_CLIENT_NAME) {
    init(monitorPorts);
}

JackPortMonitor::JackPortMonitor(bool monitorPorts, bool autoConnectNetClientsIn) : JackBase(JACK_PORT_MONITOR_CLIENT_NAME) {
    init(monitorPorts, autoConnectNetClientsIn);
}

JackPortMonitor::JackPortMonitor(string clientName_) : JackBase(clientName_) {
    bool monitorPorts=true;
    init(monitorPorts);
}

JackPortMonitor::JackPortMonitor(string clientName_, bool monitorPorts) : JackBase(clientName_) {
    init(monitorPorts);
}

JackPortMonitor::JackPortMonitor(string clientName_, bool monitorPorts, bool autoConnectNetClientsIn) : JackBase(clientName_) {
    init(monitorPorts, autoConnectNetClientsIn);
}


JackPortMonitor::JackPortMonitor(string clientName_, string serverName) : JackBase (clientName_, serverName) {
    bool monitorPorts=true;
    init(monitorPorts);
}

JackPortMonitor::JackPortMonitor(string clientName_, string serverName, bool monitorPorts) : JackBase (clientName_, serverName) {
    init(monitorPorts);
}

JackPortMonitor::JackPortMonitor(string clientName_, string serverName, bool monitorPorts, bool autoConnectNetClientsIn) : JackBase (clientName_, serverName) {
    init(monitorPorts, autoConnectNetClientsIn);
}

JackPortMonitor::JackPortMonitor(JackBase &jb){}

void JackPortMonitor::print(ostream &os) {
    os<<"=== "<<inputPorts.size()<<" input ports, "<<outputPorts.size()<<" output ports ===\n";
    map<string, map<string, vector<string> > >::iterator p; // the input port iterator
    for (vector<JackBaseWithPortNames*>::iterator kc=knownClients.begin(); kc!=knownClients.end(); ++kc) {
        //int cnCnt=(*kc)->getClientName().size();
        //cout<<(*kc)->getClientName()<<":\n";
        p=(*kc)->inputPortNamesAndConnections.begin(); // start the input port iterator at the beginning
        for (int i=0; i<max((*kc)->inputPortNamesAndConnections.size(), (*kc)->outputPortNames.size()); i++) {
            if ((*kc)->inputPortNamesAndConnections.size()>i) {
                os<<'\t'<<(*p).first<<"\t\t"; // the port name is the map key
                p++;
            } else
                os<<"\t\t\t";
            if ((*kc)->outputPortNames.size()>i)
                os<<(*kc)->outputPortNames[i]<<'\n';
            else
                os<<'\n';
        }
    }

    os<<"\n=== Connections ===\n";
    for (vector<JackBaseWithPortNames*>::iterator kc=knownClients.begin(); kc!=knownClients.end(); ++kc) {
        os<<(*kc)->getClientName()<<":\n";
        for (p=(*kc)->inputPortNamesAndConnections.begin(); p!=(*kc)->inputPortNamesAndConnections.end(); ++p) { // iterate through each client's port connections
            os<<(*p).first<<" ---> {";
            map<string, vector<string> >::iterator pcs; // The port connections which are connected to p
            for (pcs=(*p).second.begin(); pcs!=(*p).second.end(); ++pcs) { // for each client/port connection, print it.
                if (pcs!=(*p).second.begin())
                    os<<", ";
                os<<(*pcs).first<<"(";
                for (vector<string>::iterator pn=(*pcs).second.begin(); pn!=(*pcs).second.end(); ++pn) { // Iterate through the connected client's port names
                    if (pn!=(*pcs).second.begin())
                        os<<", ";
                    os<<*pn;
                }
                os<<")";
            }
            os<<"}\n";
        }
    }
}
