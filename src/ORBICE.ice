// Author Matt Flax <flatmax@>
// Date: 2013.04.26

#ifndef ORBICE_ICE_
#define ORBICE_ICE_

/// The namespace is ORB
module ORB {
    /** This network protocol specifies the class ORBICE. The methods specified here must be overloaded
    in your language by your equivalent of the ORBOriginator class which has been written for C++.
    */
    interface  ORBICE {
        /** Given a name, this will return the idempotent class objectPointer to be passed over the network.
        This method is called on the ORBOriginator by a call over the network using the ORBReplicator::getObjectPointer
        method.
        \param name The name used to register the desired class using ORBOriginator::addClass
        \param objectPointer If it exists, the idempotent pointer to be replicated on the ORBReplicator thread. If it doesn't exist, then NULL.
        */
        void getObjectPointer(string name, out Object objectPointer);
        void shutdown(); ///< shut down the communicator on the remote host
    };
};
#endif // ORBICE_ICE_

