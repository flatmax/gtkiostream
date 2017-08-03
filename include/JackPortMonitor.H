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
#ifndef JACKPORTMANAGER_H_
#define JACKPORTMANAGER_H_

#include "JackBase.H"

#define JACK_PORT_MONITOR_CLIENT_NAME "Jack Port Monitor" ///< The name to give port monitoring clients.

#include "Thread.H"

/** Maintains knowledge of jack ports.
Operates by reconstructing clients to hold only their ports in the knownClients member variable.
The list of physical ports are maintained first.
Provides methods for connection, disconnection and monitoring.

NOTE: This class requires linking against the gtkIOStream library.

*/
class JackPortMonitor : virtual public JackBase, public WaitingThread {

    /** Common intialiser.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    */
    void init(bool monitorPorts);

    /** Common intialiser.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    \param autoConnectNetClientsIn If true, autoconnect network clients.
    */
    void init(bool monitorPorts, bool autoConnectNetClientsIn);

    /** Connect callbacks to execute when ports change in any way.
    */
    void connectPortMonitoringCallbacks(void);

    /** Static callback to handle port connections and disconnections.
    \param a A port (dis)connected
    \param b A port (dis)connected
    \param connect 0 for connection removed, connection made otherwise
    \param arg The pointer to the instance of this class for which the callback was connected.
    */
    static void jackPortConnectCallback(jack_port_id_t a, jack_port_id_t b, int connect, void *arg) {
        //cout<<"\njackPortConnectCallback\n"<<endl;
        static_cast<JackPortMonitor*>(arg)->jackPortConnected(a, b, connect);
    }

    /** Static callback to handle port registration or deregistration.
    \param port The port which was (de)registered.
    \param reg Zero if the port is deregistered, otherwise registration.
    \param arg The pointer to the instance of this class for which the callback was connected.
    */
    static void jackPortRegistrationCallback(jack_port_id_t port, int reg, void *arg) {
        //cout<<"\njackPortRegistrationCallback\n"<<endl;
        static_cast<JackPortMonitor*>(arg)->jackPortRegistered(port, reg);
    }

    /** Static callback to handle port renaming.
    \param port The port which was renamed.
    \param oldName The original name of the port.
    \param newName The new name of the port.
    \param arg The pointer to the instance of this class for which the callback was connected.
    */
    static void jackPortRenameCallback(jack_port_id_t port, const char *oldName, const char *newName, void *arg) {
        //cout<<"\njackPortRenameCallback\n"<<endl;
        static_cast<JackPortMonitor*>(arg)->jackPortRenamed(port, oldName, newName);
    }

    /** Given a list of ports, associate each port with the client it belongs to.
    If the client doesn't exist, then create the client.
    \param ports The vector of port ids to associate.
    */
    virtual void breakDownPortsToClients(vector<jack_port_t *> &ports);

    /** Resynchronise either input or output ports.
    Find all physical ports first and then other ports. Using these ports, rebuild the client objects, breaking down ports to which client they belong to and
    whether they belong to the input or output set for that client.
    \param flags Either JackPortIsInput or JackPortIsOutput, indicating whether we are looking for input or output ports.
    */
    void reSyncPorts(JackPortFlags flags);


    /** Stop the client running.
    When the client stops running and is set to NULL, reset all known client's client pointers.
    \return 0 on no error, or JACK_CLOSE_CLIENT_ERROR on failure
    */
    virtual int disconnect(void);


    /** Connect to the server
    This starts the server and sets up the process callback/arg to use.
    \param clientName_ The name of the client to use
    \return NO_ERROR on success
    */
    virtual int connect(string clientName_){return JackBase::connect(clientName_);}

    /** Connect to the server.
        This starts the server and sets up the process callback/arg to use.
        When the client starts running and is set, reset all known client's client pointers.
        \param clientName_ The name of the client to use
        \param serverName The name of the server to use
        \return NO_ERROR on success
    */
    virtual int connect(const string &clientName_, const string &serverName);

    /** This threaded method waits for a signal to attempt to autoconnect netjack ports to the system ports.
    */
    virtual void *threadMain(void);

protected:

    /** Functor to compare a jack client name to a string.
    */
    struct CompareStrings : unary_function<string,bool> {
    public:
        string cn; ///< The string to compare against.

        /** Constructor specifying the string to compare against.
        \param cn_ The string to compare against.
        */
        CompareStrings(string cn_) : cn(cn_) {}

        /** A comparison operatator to compare a client name to a string.
        \param cl A pointer to the jack client to compare to.
        */
        bool operator()(JackBaseWithPortNames *cl) {
            return cl->getClientName()==cn;
        }
    };

    vector<JackBaseWithPortNames *> knownClients; ///< A vector of clients and their ports both ids and names

    bool autoConnectNetClients; ///< When true, autoconnect networked client's ports to the system ports.

    /** Find net client's ports and autoconnect them to system ports.
    */
    virtual void autoConnectNetClientsPorts(void);

    /** Find all input and output ports which aren't already in the port list.
    */
    virtual void reSyncPorts(void);

    /** Find all input and output ports which aren't already in the port list.
    */
    virtual void reSyncConnections(void);

    /** Method to handle the occasion where ports were connected or disconnected.
    \param a A port (dis)connected
    \param b A port (dis)connected
    \param connect 0 for connection removed, connection made otherwise
    */
    virtual void jackPortConnected(jack_port_id_t a, jack_port_id_t b, int connect) {
        reSyncConnections();
        this->print(cout);
    }

    /** Method to handle to handle port registration or deregistration.
    \param port The port which was (de)registered.
    \param reg Zero if the port is deregistered, otherwise registration.
    */
    virtual void jackPortRegistered(jack_port_id_t port, int reg) {
        if (autoConnectNetClients){
            //autoConnectNetClientsPorts(); // can't connect ports in a critical server thread.
            // Instruct the network port manager thread to connect the net ports.
            cond.lock(); // lock the mutex and wake the thread.
            cond.signal(); // Wake the WaitingThread
            cond.unLock(); // Unlock so the WaitingThread can continue.
        }
        reSyncPorts();
        reSyncConnections();
        this->print(cout);
    }

    /** Method to handle port renaming.
    \param port The port which was renamed.
    \param oldName The original name of the port.
    \param newName The new name of the port.
    */
    virtual void jackPortRenamed(jack_port_id_t port, const char *oldName, const char *newName) {
        reSyncPorts();
        reSyncConnections();
            this->print(cout);
    }

public:
    /** Constructor. Doesn't connect the jack client.
    By default monitors ports.
    */
    JackPortMonitor();

    /** Constructor.  Doesn't connect the jack client.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    */
    JackPortMonitor(bool monitorPorts);


    /** Constructor.  Doesn't connect the jack client.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    \param autoConnectNetClientsIn If true, autoconnect network clients.
    */
    JackPortMonitor(bool monitorPorts, bool autoConnectNetClientsIn);

    /** Constructor. Connecting the client to the default server.
    By default monitors ports.
    \param clientName_ The client name, which will initiate a server connection.
    */
    JackPortMonitor(string clientName_);

    /** Constructor. Connecting the client to the default server.
    \param clientName_ The client name, which will initiate a server connection.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    */
    JackPortMonitor(string clientName_, bool monitorPorts);

    /** Constructor. Connecting the client to the default server.
    \param clientName_ The client name, which will initiate a server connection.
    \param monitorPorts True if port monitoring callbacks are connected, false and ports aren't monitored.
    \param autoConnectNetClientsIn If true, autoconnect network clients.
    */
    JackPortMonitor(string clientName_, bool monitorPorts, bool autoConnectNetClientsIn);

    /** Constructor. Connecting the client to a particular server.
    By default monitors ports.
    \param clientName_ The client name, which will initiate a server connection.
    \param serverName The server to connect to.
    */
    JackPortMonitor(string clientName_, string serverName);

    /** Constructor. Connecting the client to a particular server.
    \param clientName_ The client name, which will initiate a server connection.
    \param serverName The server to connect to.
    \param monitorPorts If true, then monitor changes to the prots and connections.
    */
    JackPortMonitor(string clientName_, string serverName, bool monitorPorts);

    /** Constructor. Connecting the client to a particular server.
    \param clientName_ The client name, which will initiate a server connection.
    \param serverName The server to connect to.
    \param monitorPorts If true, then monitor changes to the prots and connections.
    \param autoConnectNetClientsIn If true, autoconnect network clients.
    */
    JackPortMonitor(string clientName_, string serverName, bool monitorPorts, bool autoConnectNetClients);

    /** This constructor does no setup nor port monitoring.
    No connections to servers or port monitoring are setup.
    \param jb The instance of a JackBase object to
    */
    JackPortMonitor(JackBase &jb);

    /// Destructor
    virtual ~JackPortMonitor() {}

    /** Print ports and clients. On a client by client basis.
    \param os The output stream to print to.
    */
    virtual void print(ostream &os);

    /** Connect the jackPortRenamed method for callback.
    This can be done automatically at class construction time.
    */
    void connectPortRenameCallback(void);

    /** Connect the jackPortRegistered method for callback.
    This can be done automatically at class construction time.
    */
    void connectPortRegistrationCallback(void);

    /** Connect the jackPortConnected method for callback.
    This can be done automatically at class construction time.
    */
    void connectPortConnectCallback(void);


//    /** Allow printing of port information to an ostream.
//    \param os The ostream to output to.
//    \param jpm The JackPortMonitor to print out.
//    */
//    friend ostream &operator<<(ostream &os, JackPortMonitor &jpm);
};

//ostream &operator<<(ostream &os, JackPortMonitor &jpm){
//    jpm.print(os);
//    return os;
//}

#endif // JACKPORTMANAGER_H_
