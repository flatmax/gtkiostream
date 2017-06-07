/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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
#ifndef JACKBASE_H_
#define JACKBASE_H_

#include <Debug.H>

#include <jack/jack.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>

#include <assert.h>

#define JACK_SERVER_NOT_FOUND_ERROR -10+JACK_ERROR_OFFSET ///< The jack server wasn't found
#define JACK_CLIENT_OPEN_ERROR -11+JACK_ERROR_OFFSET ///< The Client failed to start
#define JACK_SET_CALLBACK_ERROR -12+JACK_ERROR_OFFSET ///< The Client failed to register its callback with the server
#define JACK_INPUT_PORT_CREATION_ERROR -13+JACK_ERROR_OFFSET ///< The Client failed to register input ports
#define JACK_OUTPUT_PORT_CREATION_ERROR -14+JACK_ERROR_OFFSET ///< The Client failed to register output ports
#define JACK_ACTIVATE_ERROR -15+JACK_ERROR_OFFSET ///< Couldn't activate the client with the server
#define JACK_HAS_NO_CAPTURE_PORTS_ERROR -16+JACK_ERROR_OFFSET ///< The server has no capture ports
#define JACK_HAS_NO_PLAYBACK_PORTS_ERROR -17+JACK_ERROR_OFFSET ///< The server has no playback
#define JACK_CLOSE_CLIENT_ERROR -18+JACK_ERROR_OFFSET ///< Couldn't stop the client error
#define JACK_SET_BUFFER_SIZE_ERROR -19+JACK_ERROR_OFFSET ///< Couldn't set the buffer size
#define JACK_SET_BUFFSIZE_CALLBACK_ERROR -20+JACK_ERROR_OFFSET ///< Couldn't set the buffer size callback
#define JACK_DEACTIVATE_CLIENT_ERROR -21+JACK_ERROR_OFFSET ///< Couldn't deactivate the client
#define JACK_CLIENT_NULL_ERROR -22+JACK_ERROR_OFFSET ///< client is null
#define JACK_PORT_CALLBACK_SETUP_ERROR -23+JACK_ERROR_OFFSET ///< error when trying to connect the port monitoring callbacks
#define JACK_NOPORTS_ERROR -24+JACK_ERROR_OFFSET ///< error when no ports are found matching a name
#define JACK_PORT_CONNECT_ERROR -25+JACK_ERROR_OFFSET ///< error when ports can't be connected
#define JACK_PORTNAME_NOTFOUND_ERROR -26+JACK_ERROR_OFFSET ///< error when the port name is not found
#define JACK_DRAG_OPTION_UNKNOWN_ERROR -27+JACK_ERROR_OFFSET ///< error when the the drag and drop drops to a widget which can't be found
#define JACK_PORT_DISCONNECT_ERROR -28+JACK_ERROR_OFFSET ///< error when ports can't be connected
#define JACK_UNKNOWN_DND_TYPE_ERROR -29+JACK_ERROR_OFFSET ///< error when a GUI drop signals neither CONNECT_PORTS nor DISCONNECT_PORTS
#define JACK_NETPORT_AUTOCONNECT_ERROR -30+JACK_ERROR_OFFSET ///< error when trying to connect network ports automatically.

class JackDebug : public Debug {
public:
    JackDebug(void) {
#ifndef NDEBUG
        errors[JACK_SERVER_NOT_FOUND_ERROR]=string("The jack server wasn't found");
        errors[JACK_CLIENT_OPEN_ERROR]=string("The jack client failed to start");
        errors[JACK_SET_CALLBACK_ERROR]=string("The Client failed to register its callback with the server");
        errors[JACK_INPUT_PORT_CREATION_ERROR]=string("The Client failed to register input ports with the server");
        errors[JACK_OUTPUT_PORT_CREATION_ERROR]=string("The Client failed to register output ports with the server");
        errors[JACK_ACTIVATE_ERROR]=string("Couldn't activate the client");
        errors[JACK_HAS_NO_CAPTURE_PORTS_ERROR]=string("The server has no capture ports");
        errors[JACK_HAS_NO_PLAYBACK_PORTS_ERROR]=string("The server has no playback ports");
        errors[JACK_CLOSE_CLIENT_ERROR]=string("Couldn't close the client");
        errors[JACK_SET_BUFFER_SIZE_ERROR]=string("Couldn't set the buffer size");
        errors[JACK_SET_BUFFSIZE_CALLBACK_ERROR]=string("Couldn't set the callback to handle Jack audio buffer size changes.");
        errors[JACK_DEACTIVATE_CLIENT_ERROR]=string("Couldn't deactivate the Jack client.");
        errors[JACK_CLIENT_NULL_ERROR]=string("Client doesn't exist, please use the connect method first.");
        errors[JACK_PORT_CALLBACK_SETUP_ERROR]=string("Couldn't connect the port monitoring callback. ");
        errors[JACK_NOPORTS_ERROR]=string("Couldn't find any ports with that name. ");
        errors[JACK_PORT_CONNECT_ERROR]=string("Couldn't connect the ports together. ");
        errors[JACK_PORTNAME_NOTFOUND_ERROR]=string("Couldn't find that port name. ");
        errors[JACK_DRAG_OPTION_UNKNOWN_ERROR]=string("Couldn't find the target widget, perhaps the client has closed. ");
        errors[JACK_PORT_DISCONNECT_ERROR]=string("Couldn't disconnect the ports. ");
        errors[JACK_UNKNOWN_DND_TYPE_ERROR]=string("Unknown drop signal, expecting either CONNECT_PORTS or DISCONNECT_PORTS. ");
        errors[JACK_NETPORT_AUTOCONNECT_ERROR]=string("Error when trying to autoconnect networked ports. ");

#endif
    }
};

/** Base for Jack classes.
Minimal knowledge of port handling.
*/
class JackBase {
    /** Common intialiser.
    */
    void init() {
        connect1To1=true; // start by default connecting one to one.
        client=NULL;
        reSyncPorts();
    }

    /** Clear all port knowledge and latency knowledge.
    */
    void clearPorts() {
        physicalInputPortCnt=physicalOutputPortCnt=0;
        inputPorts.resize(0);
        inputLatencies.resize(0);
        outputPorts.resize(0);
        outputLatencies.resize(0);
    }
protected:
    jack_client_t *client; ///< The jack client
    string clientName; ///< The name of this client

    //string serverName; ///< The name of the server
    jack_options_t jackOptions; ///< Options for jack
    jack_status_t status; ///< The status of jack

    int physicalInputPortCnt; ///< The number of physical input ports
    int physicalOutputPortCnt; ///< The number of physical input ports

    vector<jack_port_t *> inputPorts; ///< The input ports
    vector<jack_port_t *> outputPorts; ///< The output ports

    vector<jack_nframes_t> inputLatencies; ///< The input latencies, one for each port.
    vector<jack_nframes_t> outputLatencies; ///< The output latencies, one for each port.

    /** Determine the capture and playback latencies. These latencies are defined by Jack as :

    Capture latency  : how long since the data read from the buffer of a port arrived at at a port marked with JackPortIsTerminal. The data will have come from the "outside world" if the terminal port is also marked with JackPortIsPhysical, or will have been synthesized by the client that owns the terminal port.

    Playback latency : how long until the data written to the buffer of port will reach a port marked with JackPortIsTerminal.
    */
    virtual void determineLatencies() {
        jack_latency_range_t range;
        for (unsigned int i=0; i < inputPorts.size(); i++) {
            jack_port_get_latency_range(inputPorts[i], JackCaptureLatency, &range);
            if (range.min != range.max) {
                cout<<"JackClient::determineLatencies : input : min = "<<range.min<<" max = "<<range.max<<endl;
                assert("don't know how to handle latencies which has a range which is different");
            }
            inputLatencies[i]=range.min;
            cout<<"input port "<<i<<" latency = "<<inputLatencies[i]<<endl;
        }
        for (unsigned int i=0; i < outputPorts.size(); i++) {
            jack_port_get_latency_range(outputPorts[i], JackPlaybackLatency, &range);
            if (range.min != range.max) {
                cout<<"JackClient::determineLatencies : output : min = "<<range.min<<" max = "<<range.max<<endl;
                assert("don't know how to handle latencies which has a range which is different");
            }
            outputLatencies[i]=range.min;
            cout<<"output port "<<i<<" latency = "<<outputLatencies[i]<<endl;
        }
    }

    /** Get the port list and the number of ports.
    \param flags, the JackPortFlags {JackPortIsOutput, JackPortIsPhysical, JackPortCanMonitor, JackPortIsTerminal}.
    \param ports The port vector to be returned, null if not to be populated.
    \param portNamePattern name pattern to match when searching for ports.
    \param typeNamePattern type name pattern to match when searching for ports.
    \return >=0 representing the number of ports or <0 on error.
    */
    int getPortListAndCount(JackPortFlags flags, vector<jack_port_t *> *ports, const char *portNamePattern, const char *typeNamePattern) {
        int cnt=NO_ERROR;
        if (client) {
            const char **portsStrs = jack_get_ports(client, portNamePattern, typeNamePattern, flags);
            for (cnt=0; portsStrs[cnt]!=NULL; cnt++)
                if (ports)
                    ports->push_back(jack_port_by_name(client, portsStrs[cnt]));
            free(portsStrs);
        } else
            cnt=JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);
        return cnt;
    }

    /** Get the port name list and the number of ports.
    \param flags, the JackPortFlags {JackPortIsOutput, JackPortIsPhysical, JackPortCanMonitor, JackPortIsTerminal}.
    \param ports The port vector to be returned, null if not to be populated.
    \param portNamePattern name pattern to match when searching for ports.
    \param typeNamePattern type name pattern to match when searching for ports.
    \return >=0 representing the number of ports or <0 on error.
    */
    int getPortListAndCount(JackPortFlags flags, vector<string> *ports, const char *portNamePattern, const char *typeNamePattern) {
        int cnt=NO_ERROR;
        if (client) {
            const char **portsStrs = jack_get_ports(client, portNamePattern, typeNamePattern, flags);
            if (portsStrs) {
                for (cnt=0; portsStrs[cnt]!=NULL; cnt++)
                    if (ports)
                        ports->push_back(portsStrs[cnt]);
                free(portsStrs);
            }
        } else
            cnt=JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);
        return cnt;
    }

    /** Find all physical input and output ports.
    */
    virtual void reSyncPorts(void) {
        //cout<<"JackBase::reSyncPorts"<<endl;
        clearPorts(); // remove previous knowledge
    }

    /** Find all input and output ports which aren't already in the port list.
    */
    virtual void reSyncConnections(void) {
        // do nothing
    }

public:
    bool connect1To1; ///< When true, then connections are made in a 1 to 1 manner. When False, then connections are made in an all to all manner.

    /** Constructor.
    */
    JackBase() {
        init();
    }

    /** Constructor. Connecting the client to the default server.
    \param clientName_ The client name, which will initiate a server connection.
    */
    JackBase(string clientName_) {
        cout<<"JackBase "<<clientName_<<endl;
        init();
        int res=connect(clientName_);
        if (res!=0)
            JackDebug().evaluateError(res);
        else
            cout<<"JackBase connected"<<endl;
    }

    /** Constructor. Connecting the client to a particular server.
    \param clientName_ The client name, which will initiate a server connection.
    \param serverName The server to connect to.
    */
    JackBase(string clientName_, string serverName) {
        init();
        connect(clientName_, serverName);
    }

    /// Destructor
    virtual ~JackBase() {}

    /** Stop the client running
    \return 0 on no error, or JACK_CLOSE_CLIENT_ERROR on failure
    */
    virtual int stopClient(void) {
        if (client)
            if (jack_deactivate(client)!=0)
                return JACK_DEACTIVATE_CLIENT_ERROR;
        return 0;
    }

    /** Stop the client running
    \return 0 on no error, or JACK_CLOSE_CLIENT_ERROR on failure
    */
    virtual int disconnect(void) {
        stopClient();
        if (client)
            if (jack_client_close(client)!=0)
                return JACK_CLOSE_CLIENT_ERROR;
        client=NULL;
        return 0;
    }

    /** Get the server samplerate
    \return the current sample rate
    */
    int getSampleRate(void) const {
        int fs=jack_get_sample_rate ( client );
        return fs;
    }

    /** Create the client's ports
    \param inName The input port base name to use
    \param inCnt The number of ports to create
    \param outName The output port base name to use
    \param outCnt The number of output ports to create
    */
    virtual int createPorts(string inName, int inCnt, string outName, int outCnt) {
        inputLatencies.resize(inCnt);
        inputPorts.resize(inCnt);
        for (int i=0; i < inCnt; i++) {
            ostringstream convert;
            convert<<i;
            string portName=inName+convert.str();
            inputPorts[i] = jack_port_register(client, portName.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
            if (inputPorts[i] == NULL)
                return JACK_INPUT_PORT_CREATION_ERROR;
        }

        outputLatencies.resize(outCnt);
        outputPorts.resize(outCnt);
        for (int i=0; i < outCnt; i++) {
            ostringstream convert;
            convert<<i;
            string portName=outName+convert.str();
            outputPorts[i] = jack_port_register(client, portName.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
            if (outputPorts[i] == NULL)
                return JACK_OUTPUT_PORT_CREATION_ERROR;
        }
        determineLatencies();
        return 0;
    }

    /** Destroy all ports.
    \returns NO_ERROR on success
    */
    virtual int destroyPorts(){
        int ret=NO_ERROR;
        if (client){
            for (int i=0; i<inputPorts.size(); i++){
                if (inputPorts[i])
                    jack_port_unregister(client, inputPorts[i]);
                inputPorts[i]=NULL;
            }
            inputPorts.resize(0);
            for (int i=0; i<outputPorts.size(); i++){
                if (outputPorts[i])
                    jack_port_unregister(client, outputPorts[i]);
                outputPorts[i]=NULL;
            }
            outputPorts.resize(0);
        } else
            ret=JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);
        return ret;
    }

    /** Find out how many physical input and outputs ports are present.
    \param[out] in The number of input ports.
    \param[out] out The number of output ports.
    */
    void getPhysicalPorts(vector<jack_port_t *> &inPorts, vector<jack_port_t *> &outPorts) {
        if (client) {
            // find the number of input ports
            getPortListAndCount((JackPortFlags)(JackPortIsPhysical|JackPortIsInput), &inPorts, NULL, NULL);
            // find the number of output ports
            getPortListAndCount((JackPortFlags)(JackPortIsPhysical|JackPortIsOutput), &outPorts, NULL, NULL);
        } else
            JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);
    }

    /** Find out how many physical input and outputs ports are present.
    \param[out] in The number of input ports.
    \param[out] out The number of output ports.
    */
    void getPhysicalPortCount(int &in, int &out) {
        in=out=0;
        if (client) {
            // find the number of input ports
            in=getPortListAndCount((JackPortFlags)(JackPortIsPhysical|JackPortIsInput), (vector<jack_port_t *> *)NULL, NULL, NULL);
            // find the number of output ports
            out=getPortListAndCount((JackPortFlags)(JackPortIsPhysical|JackPortIsOutput), (vector<jack_port_t *> *)NULL, NULL, NULL);
        } else
            JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);
    }

    /** Find out if we are connected to a server.
    \return true if connected to a server, false if not connected.
    */
    bool connected(){
        if (client)
            return true;
        return false;
    }

    /** Connect to the server
    This starts the server and sets up the process callback/arg to use.
    \param clientName_ The name of the client to use
    \return NO_ERROR on success
    */
    virtual int connect(string clientName_) {
        return connect(clientName_, string());
    }

    /** Connect to the server
        This starts the server and sets up the process callback/arg to use.
        \param clientName_ The name of the client to use
        \param serverName The name of the server to use
        \return NO_ERROR on success
    */
    virtual int connect(const string &clientName_, const string &serverName) {
        clientName = clientName_;

        if (client) // make sure not connected
            disconnect();

        /* open a client connection to the JACK server */
        //char *server_name;
        jackOptions=JackNullOption;
        if (serverName.length()>0) {
            jackOptions=JackServerName;
            client=jack_client_open(clientName.c_str(), jackOptions, &status, serverName.c_str());
        } else
            client=jack_client_open(clientName.c_str(), jackOptions, &status);
        if (client==NULL) {
            cerr<<"connect: jack_client_open() failed, status = 0x"<<hex<<status<<endl;
            if ( status & JackServerFailed )
                return JACK_SERVER_NOT_FOUND_ERROR;
            return JACK_CLIENT_OPEN_ERROR;
        }

        if (status & JackServerStarted)
            cerr<<"JACK server started"<<endl;
        if ( status & JackNameNotUnique ) {
            clientName = jack_get_client_name ( client );
            cerr<<"unique name '"<<clientName<<"' assigned";
        }

        //jack_on_shutdown ( client, jack_shutdown, 0 );

        inputLatencies.resize(0);
        outputLatencies.resize(0);

        return NO_ERROR;
    }

    /** Print ports and clients.
    \param os The output stream to print to.
    */
    virtual void print(ostream &os) {
        os<<"JackBase : "<<inputPorts.size()<<" input ports, "<<outputPorts.size()<<" output ports.\n";
        os<<"Input ports : \t\t\t Output ports:\n";
        for (int i=0; i<max(inputPorts.size(), outputPorts.size()); i++) {
            if (inputPorts.size()>i)
                os<<'\t'<<jack_port_name(inputPorts[i])<<"\t\t";
            else
                os<<"\t\t\t\t\t";
            if (outputPorts.size()>i)
                os<<jack_port_name(outputPorts[i])<<'\n';
            else
                os<<'\n';
        }
    }

    /** Get the client name.
    \return the client name.
    */
    string getClientName() {
        return clientName;
    }

    /** Set the client name.
    \param the client name.
    */
    virtual void setClientName(string cn) {
        clientName=cn;
    }

    /** Given a port id, extract the port name.
    \param p The port id to find the client name from.
    \return the port name (final portion) of the full port name.
    */
    string portNameFromPort(jack_port_id_t p) {
        if (client)
            return shortPortNameFromPortName(jack_port_name(jack_port_by_id(client, p)));
        else
            return string();
    }

    /** Given a port id, extract the port name.
    \param p The port to find the client name from.
    \return the port name (final portion) of the full port name.
    */
    string portNameFromPort(jack_port_t *p) {
        return shortPortNameFromPortName(jack_port_name(p));
    }

    /** Given a port id, extract the client name.
    \param p The port id to find the client name from.
    \return the client name (initial portion) of the port name.
    */
    string clientNameFromPort(jack_port_id_t p) {
        if (client)
            return clientNameFromPort(jack_port_by_id(client, p));
        else
            return string();
    }

    /** Given a port, extract the client name.
    \param p The port to find the client name from.
    \return the client name (initial portion) of the port name.
    */
    string clientNameFromPort(jack_port_t *p) {
        return clientNameFromPortNames(jack_port_name(p), jack_port_short_name(p));
    }

    /** Given a full port name, extract the client name.
    \param fullPortName The client:port name convention.
    \return the client name (initial portion) of the port name.
    */
    string clientNameFromPortName(string fullPortName) {
        return fullPortName.substr(0, fullPortName.find(':'));
    }

    /** Given a full port name and the short port name (the port name without the client portion), extract the client name.
    \param fullPortName The client:port name convention.
    \param shortPortName The port name.
    \return the client name (initial portion) of the port name.
    */
    string clientNameFromPortNames(string fullPortName, string shortPortName) {
        fullPortName=fullPortName.substr(0, fullPortName.find(shortPortName)-1);
        return fullPortName;
    }

    string shortPortNameFromPortName(const string fullPortName) {
        string res=fullPortName;
        return res.substr(res.find(":")+1, string::npos);
    }

    /** Test if the JackBase::clientName equals a string.
    \param cn The string to compare to.
    */
    bool operator==(const string &cn) {
        return clientName.find(cn)!=string::npos;
    }

    /** Get the number of input ports.
    \return the number of input ports.
    */
    int getInputPortSize() {
        return inputPorts.size();
    }

    /** Get an input port.
    \param i The input port to retrieve.
    \return NULL on failure, otherwise the port.
    */
    jack_port_t *getInputPort(int i) {
        if (i<0 || i>inputPorts.size())
            return NULL;
        return inputPorts[i];
    }

    /** Get an output port.
    \param i The input port to retrieve.
    \return NULL on failure, otherwise the port.
    */
    jack_port_t *getOutputPort(int i) {
        if (i<0 || i>outputPorts.size())
            return NULL;
        return outputPorts[i];
    }

    /** Add an input port to the list of known input ports.
    \param inP The port to add.
    */
    void addInputPort(jack_port_t *inP) {
        inputPorts.push_back(inP);
    }

    /** Add an output port to the list of known output ports.
    \param inP The port to add.
    */
    void addOutputPort(jack_port_t *outP) {
        outputPorts.push_back(outP);
    }

    /** Given an input name and an output name, of either form, "ClientName" or "ClientName:PortName", populate a vector of strings matching all of the possible ports.
    \param inName The input port name
    \param inPorts A vector of strings naming all of the ports found matching the inName.
    \param outName The output port name
    \param outPorts A vector of strings naming all of the ports found matching the outName.
    \return NO_ERROR on success, otherwise the appropriate error number.
    \sa connectPorts, disConnectPorts
    */
    int populatInOutPorts(const string &inName, vector<string> &inPorts, const string &outName, vector<string> &outPorts) {
        if (inName.find(':')==string::npos) // if we have no ':' then get all ports
            getPortListAndCount((JackPortFlags)JackPortIsInput, &inPorts, inName.c_str(), NULL);
        else
            inPorts.push_back(inName);
        if (outName.find(':')==string::npos) // if we have no ':' then get all ports
            getPortListAndCount((JackPortFlags)JackPortIsOutput, &outPorts, outName.c_str(), NULL);
        else
            outPorts.push_back(outName);

        // if the vector of ports to connect is empty, report error
        if (inPorts.size()==0)
            return JackDebug().evaluateError(JACK_NOPORTS_ERROR, string("Tried using the input name ")+inName);
        if (outPorts.size()==0)
            return JackDebug().evaluateError(JACK_NOPORTS_ERROR, string("Tried using the output name ")+outName);

        return NO_ERROR;
    }

    /** Connect ports together which are given by name.
    If the names have ':' in them, then they are of the client:port form and can be directly connected together.
    If one name has no ':' in it, then connect to all of it's ports.
    If both names have no ':' in them, then map ports one to one until one client runs out of ports.
    \param inName The input port name
    \param outName The output port name
    \return 0 on success, <0 on error.
    */
    int connectPorts(const string inName, const string outName) {
        //cout<<"JackBase::connectPorts : inName = "<<inName<<" outName = "<<outName<<endl;
        if (!client)
            return JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);

        int res=NO_ERROR;
        vector<string> inPorts, outPorts;
        if ((res=populatInOutPorts(inName, inPorts, outName, outPorts))!=NO_ERROR)
            return res;

        // if the vector of ports is >1
        vector<string>::iterator op=outPorts.begin();
        for (vector<string>::iterator ip=inPorts.begin(); ip!=inPorts.end(); ++ip) {
            for (; op!=outPorts.end(); ++op) {
                if (!jack_port_connected_to(jack_port_by_name(client, (*op).c_str()), (*ip).c_str())) // if not already connected, then try to connect
                    if (jack_connect(client, (*op).c_str(), (*ip).c_str())<0)
                        res=JackDebug().evaluateError(JACK_PORT_CONNECT_ERROR, string("Tried to connect the output port ")+(*op)+" to the input port "+(*ip));
                if (connect1To1) { // when connecting 1 to 1, connect only one and then break
                    op++;
                    break;
                }
            }
            if (connect1To1 && op==outPorts.end()) // if we are connecting 1 to 1 and the output port list is completely iterated out, then exit.
                break;
            else if (!connect1To1) // if we aren't connecting 1 to 1, then start the output ports from the beginning so it can iterate all the way through.
                op=outPorts.begin();
        }
        return res;
    }

    /** Disconnect ports which are given by name.
    If the names have ':' in them, then they are of the client:port form and can be directly disconnected.
    If one name has no ':' in it, then disconnect all of it's ports.
    If both names have no ':' in them, then disconnect ports one to one.
    \param inName The input port name
    \param outName The output port name
    \return 0 on success, <0 on error.
    */
    virtual int disconnectPorts(const string &inName, const string &outName) {
        cout<<"JackBase::disconnectPorts : inName = "<<inName<<" outName = "<<outName<<endl;
        if (!client)
            return JackDebug().evaluateError(JACK_CLIENT_NULL_ERROR);

        int res=NO_ERROR;
        vector<string> inPorts, outPorts;
        if ((res=populatInOutPorts(outName, outPorts, inName, inPorts))!=NO_ERROR) // in the case of disconnection, the input and output port name meanings are reversed.
            return res;

        // if the vector of ports is >1
        for (vector<string>::iterator ip=inPorts.begin(); ip!=inPorts.end(); ++ip)
            for (vector<string>::iterator op=outPorts.begin(); op!=outPorts.end(); ++op)
                if (jack_port_connected_to(jack_port_by_name(client, (*ip).c_str()), (*op).c_str())) // if connected, then try to dis-connect
                    if (jack_disconnect(client, (*ip).c_str(), (*op).c_str())<0)
                        res=JackDebug().evaluateError(JACK_PORT_DISCONNECT_ERROR, string("Tried to disconnect the output port ")+(*ip)+" to the input port "+(*op));
        return res;
    }

    /** Method to set the client.
    \param c The client to set
    */
    void setClient(jack_client_t *c) {
        client=c;
    }

//    /** Allow printing of port information to an ostream.
//    \param os The ostream to output to.
//    \param jpm The JackBase to print out.
//    */
//    friend ostream &operator<<(ostream &os, JackBase &jpm);
};

//ostream &operator<<(ostream &os, JackBase &jpm){
//    jpm.print(os);
//    return os;
//}


/** Adds port name member variables to the JackBase class.
Inter client connections are also managed here. Only input port connections are stored on the assumption that they are all connected to output ports.
*/
class JackBaseWithPortNames : public JackBase {
public:
    map<string, map<string, vector<string> > > inputPortNamesAndConnections; ///< The input port names which map to a vector of the connected clients and each of their connected ports
    vector<string> outputPortNames; ///< The output port names

    /** Iterating through the inputPorts, find what they are connected to.
    */
    void findInputConnections() {
        // iterate through each of the input ports and make a vector of their connections.
        for (vector<jack_port_t *>::iterator p=inputPorts.begin(); p!=inputPorts.end(); ++p) {
            const char**cons=jack_port_get_connections(*p); // for each input port find all of its connections
            string thisPortName=jack_port_short_name(*p); // this is the port name we are interrogating which is the key to the port connections map
            inputPortNamesAndConnections[thisPortName]=map<string, vector<string> >(); // start with an empty list of connections
            //cout<<"Port : "<<jack_port_name(*p)<<endl;
            if (cons) {
                for (int i=0; cons[i]!=NULL; i++) { // add each client/port which this input port is connected to.
                    string shortPN=shortPortNameFromPortName(cons[i]); // This is the name of the port which is connected to thisPortName
                    // if this port name isn't already in the list of connected port names for this client, then add it to the vector for this client
                    if (find(inputPortNamesAndConnections[thisPortName][clientNameFromPortNames(cons[i], shortPN)].begin(), inputPortNamesAndConnections[thisPortName][clientNameFromPortNames(cons[i], shortPN)].end(), shortPN)
                            == inputPortNamesAndConnections[thisPortName][clientNameFromPortNames(cons[i], shortPN)].end())
                        inputPortNamesAndConnections[thisPortName][clientNameFromPortNames(cons[i], shortPN)].push_back(shortPN); // add it to the connected client's port list
                    //cout<<"\t"<<clientNameFromPortNames(cons[i], shortPN)<<":"<<shortPN<<endl;
                }
                delete [] cons;
            }
        }
    }
};
#endif // JACKBASE_H_
