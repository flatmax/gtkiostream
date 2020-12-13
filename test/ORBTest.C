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

/*
Author: Matt Flax <flatmax@flatmax.org>
Date: 2013.05.03
*/

#include "ORB.H"
#include "OptionParser.H"

/* To compile this, look in the Makefile
First machine generate the C++ classes using slice2cpp then compile using your C++ compiler
*/

#include "ORBTestICE.H"
using namespace ORB;

/** This class is only for testing and shouldn't be used in your code.
*/
class ORBTestClass : public ORBObject<ORBTEST::ORBTestICE> {
public:
    /* implement the virtual method which is in the ORBTestICE.ice protocol
    Note that the "Ice::Current& c" argument should be present and end the method definition when the method has been defined in the .ice file.
    */
    int method(int input, const Ice::Current& c=Ice::Current()){
        cout<<"input was = "<<input<<endl;
        input++;
        cout<<"input now = "<<input<<endl;
        return input;
    }
};
template<> const string ORBTestClass::ORBObject<ORBTEST::ORBTestICE>::name("ORBTestClass"); // setup the type identity of the class

// The following empty class is not necessary but is possible ... if you want to expand the methods and variables of the ORBOriginator
//// The following class runs on the originating end, it is the end which creates the objects
//// create a class which maintains a factory which inherits from the ORBOriginator class
//class ORBTestOriginator : public ORBOriginator {
//public:
//    ORBTestOriginator(int argc, char *argv[], string transportNameIn) : ORBOriginator(argc, argv, transportNameIn){
//    }
//};

/** This class is only for testing and shouldn't be used in your code.

 The following class runs on the replicating end, it is the end which creates the proxies
 create a class which maintains a factory which inherits from the ORBOriginator class
*/
class ORBTestReplicator : public ORBReplicator {
    /* This implements the factory for classes.
    It creates the correct class matching the id type
    */
    Ice::ObjectPtr create(const std::string& type){
        if(type == ORBTestClass::ice_staticId()) {
            Ice::ObjectPtr objectPtr = new ORBTestClass;
            return objectPtr;
        }

        // if we get here, then we don't know the class nor how to create it
	cout<<"Can't find that class !"<<endl;
        assert(false);
        return 0;
    }
public:
    ORBTestReplicator(int argc, char *argv[], string transportNameIn, string ipAddress) : ORBReplicator(argc, argv, transportNameIn, ipAddress){
    }
    ORBTestReplicator(int argc, char *argv[], string transportNameIn, string ipAddress, vector<pair<string,string> > extraProperties) : ORBReplicator(argc, argv, transportNameIn, ipAddress, extraProperties){
    }
};

void printUsage(char *str){
        cout<<"Usage : "<<str<<" -o  "<<endl;
        cout<<"Usage : "<<str<<" -r  "<<endl;
        cout<<"\t -o : run the originator\n\t -r : run the replicator"<<endl;
        cout<<"Usage : "<<str<<" -host hostName  "<<endl;
        cout<<"Usage : "<<str<<" --host=hostName  "<<endl;
        cout<<"\t Where hostName is the ip address or host name to connect to : defaults to 127.0.0.1"<<endl;
}

/** This main function is executed as either the originator (with arg -o) or as the replicator (with arg -r).
When executed as the originator, a class is created and added to the ORB application layer, ready for a replicator to use it.
When executed as the replicator, two things occur. Firstly a proxy is collected off the network and the methods are run remotely (on the replicator).
                                                   Secondly a class is created matching the satate of the class on the originator and its methods are run locally.
*/
int main(int argc, char *argv[]){

    // parse options
    int i=0;
    OptionParser op;
    string dummy;
    bool orig=(op.getArg("o", argc, argv, dummy, i)!=i);
    bool rep=(op.getArg("r", argc, argv, dummy, i=0)!=i);

	if (orig & rep){
        printUsage(argv[0]);
        return -1;
    }

    if (!orig & !rep){
        printUsage(argv[0]);
        return -1;
    }

    string hostName("127.0.0.1");
	if (op.getArg("host", argc, argv, hostName, i=0)==i)
        cout<<"defaulting host to  : "<<hostName<<endl;

	/////// Run either the origin or replicator
    // test using protocol version 1.0 - only necessary for the embedded version of Icee
    vector<pair<string,string> > extraOptions;
    extraOptions.push_back(pair<string,string>(string("Ice.Default.EncodingVersion"),string("1.0")));

    // this is the originator
    if (orig){
        // create the origin
        ORBOriginator origin(argc, argv, string("BasePipe"), extraOptions); // This is the original locaiton where the classes are created

        // Create a class and add it to the ORB application layer
        ORBTestClass *stc = new ORBTestClass; // this is the class to be added to the ORB application layer
        stc->__setNoDelete(true); // ensure that it isn't memory managed when not expected by the ORBing system
        stc->variable=1.23456;
        origin.addClass(stc,  stc->name); // add it to the originator
        // run the ORB thread until instructred to shutdown
        origin.waitForShutdown();
        stc->__setNoDelete(false); // free the ORB based class for memory collection
    }

    // this is the replicator
    if (rep){
        // create the replicator which will connect to hostName
        ORBTestReplicator replicator(argc, argv, string("BasePipe"), hostName, extraOptions); // This is the replicating locaiton where the classes are proxies
        if (!replicator.connectedOK()){
            cout<<"Couldn't connect to the origin on the host "<<hostName<<endl;
            cout<<"exiting ..."<<endl;
            return -1;
        }

        // By using a proxy, you can execute the originator class's methods (on the originator), but you will not have access to the variables
        ORBTEST::ORBTestICEPrx oRBTestICEPrx = replicator.getObjectProxy<ORBTEST::ORBTestICEPrx>(ORBTestClass::name);
        cout<<"input = "<<oRBTestICEPrx->method(1)<<endl;

         // If you want access to the class's member variables (idempotent on the replicator - i.e. if you change
        // them here they will not change on the originator) you must get the class locally
        ORBTestClass *stc = replicator.getObjectPointer<ORBTestClass>(ORBTestClass::name);
        cout<<"variable = "<<stc->variable<<endl;
        cout<<"\n\nNotice : if you call the method of the local Pointer (not the proxy) then it only executes locally, not on the originator"<<endl;
        cout<<"input = "<<stc->method(4)<<endl;

        stc->__setNoDelete(false); // ensure that this class can be deleted by the memory manager - otherwise you will have a memory leak

        replicator.shutdown();
    }

    return 0;
}

// these tests are for the base classes - which were used during debugging.

//void test1(ORBBase &sb){
//    cout<<sb.getPortOffset()<<endl;
//    cout<<sb.getPortOffset()<<endl;
//    cout<<sb.getPortOffset()<<endl;
//    cout<<sb.getPortOffset()<<endl;
//}
//
//void test2(ORBBase &sb){
//    int portOffset=0;
//    cout<<"portOffset="<<portOffset<<endl;
//    cout<<"TCP UDP port = "<<sb.getTCPUDPPort(portOffset)<<endl;
//    cout<<"SSL port = "<<sb.getSSLPort(portOffset)<<endl;
//    portOffset=sb.getPortOffset();
//    cout<<"portOffset="<<portOffset<<endl;
//    cout<<"TCP UDP port = "<<sb.getTCPUDPPort(portOffset)<<endl;
//    cout<<"SSL port = "<<sb.getSSLPort(portOffset)<<endl;
//}
//
//void test3(ORBBase &sb){
//    string transportName("transportName");
//    int portOffset=sb.getPortOffset();
//    cout<<sb.generateEndPoint(transportName, string(), portOffset)<<endl;
//    cout<<sb.generateEndPoint(transportName, string("192.168.0.1"), portOffset)<<endl;
//}
//
//int main(int argc, char *argv[]){
//    ORBBase sb;
//    test1(sb);
//    test2(sb);
//    test3(sb);
//
//    return 0;
//}
