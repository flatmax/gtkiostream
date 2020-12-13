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
