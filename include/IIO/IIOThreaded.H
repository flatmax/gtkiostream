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

#ifndef IIOTHREADED_H_
#define IIOTHREADED_H_

#include "IIO.H"
#include "Thread.H"
#include "mffm/LinkList.H"

class IIOThreaded : public IIO, public ThreadedMethod, public Cond {

//    Mutex fillRegion, emptyRegion;
//    Mutex emptyRegion;

    LinkList<Eigen::Array<unsigned short int, Eigen::Dynamic, Eigen::Dynamic> *> buffers; ///< The load and unload buffers.
    Eigen::Array<unsigned short int, Eigen::Dynamic, Eigen::Dynamic> *fullBuffer;

    /** All reading is done in a threaded environment.
    This ensures that you can process data whilst new data is being read in.
    */
    void *threadMain(void) {
        struct timespec lockStart, lockStop;

        fullBuffer=NULL;
        int nframes=getReadArraySampleCount(*buffers.current());
        while (1) {
            if( clock_gettime( CLOCK_REALTIME, &lockStart) == -1 ) {
                cout<<"clock lockStart get time error"<<endl;
            }

            int ret=read(nframes, *buffers.current());
            if (ret!=NO_ERROR)
                break;

            // store the pointer to the full buffer, switch to the empty buffer and indicate to any external thread that the full buffer is ready for reading.
            fullBuffer=buffers.current(); // store a pointer to the full buffer
            buffers.next(); // switch to the empty buffer.

            lock(); // lock the mutex, indicate the condition and wake the thread.
            bufFull=true;
            signal(); // Wake the WaitingThread
            unLock(); // Unlock so the WaitingThread can continue.

            if( clock_gettime( CLOCK_REALTIME, &lockStop) == -1 ) {
                cout<<"clock lockStop get time error"<<endl;
            }
            double duration = 1.e3*( lockStop.tv_sec - lockStart.tv_sec ) + (double)( lockStop.tv_nsec - lockStart.tv_nsec )/1.e6;
            cout<<"thread duration = "<<duration<<'\n';
//            if (duration<1800)
//                usleep(2000);

        }

        cout<<"IIO read thread stopped due to error"<<endl;
        return NULL;
    }

    /** Resize the internal buffers for reading.
    The end result will be buffers which capture N samples per channel, where the total number of channels is the number requested + the remainder non-requested channels on the last device.
    i.e. the total number of channels will be ceil(ch / number of channels per device) * number of channels per device. For example, if ch=3 but there are 2 channels per device, we will get ceil(3/2)*2 = 4.
    \param N the number of samples to read.
    \param ch the number of channels to read.
    \return NO_ERROR or the suitable error. The arrays are returned correctly sized for reading N samples.
    */
    int resizeBuffers(int N, int ch) {
        // if the data matrix is larger in columns then the number of capture channels, then resize it.

        // ensure that the buffer exist
        while (buffers.getCount()<2) {
            buffers.add(new Eigen::Array<unsigned short int, Eigen::Dynamic, Eigen::Dynamic>);
            if (!buffers.current()) {
                buffers.remove();
                return IIODebug().evaluateError(IIODEVICE_RESIZEBUF_NEW_ERROR);
            }
        }
        int retVal;
        for (int i=0; i<buffers.getCount(); i++) {
            if ((retVal=getReadArray(N, *buffers.next()))!=NO_ERROR)
                return retVal;
            int cols=(int)ceil((float)ch/(float)operator[](0).getChCnt()); // check whether we require less then the available number of channels
            if (cols<buffers.current()->cols())
                buffers.current()->resize(buffers.current()->rows(), cols);
        }
        bufferInfo();
        //setChannelBufferCnt(N*2);
        return NO_ERROR;
    }

public:
    bool bufFull; ///< Indicates when the read has completed and a buffer is full.

    IIOThreaded () {
        bufFull=false;
        fullBuffer=NULL;
    }

    virtual ~IIOThreaded() {
        while (buffers.getCount()) // remove and delete any buffers which are present
            delete buffers.remove();
    }

    int setSampleCountChannelCount(uint N, uint ch) {
        return resizeBuffers(N, ch);
    }

    void bufferInfo() {
        cout<<"IIOThreaded :: there are "<<buffers.getCount()<<" buffers."<<endl;
        for (int i=0; i<buffers.getCount(); i++)
            cout<<"\tbuffer "<<i<<" has "<<buffers.grab(i+1)->cols()<<" channels with N="<<buffers.grab(i+1)->rows()<<endl;
    }

    /** Get the full buffer.
    \return The buffer which was last filled.
    */
    Eigen::Array<unsigned short int, Eigen::Dynamic, Eigen::Dynamic> *getFullBuffer() {
        return fullBuffer;
    }

//    int fillRegionLock(void){
//        return fillRegion.lock();
//    }
//
//    int emptyRegionLock(bool lock){
//        if (lock)
//            return emptyRegion.lock();
//        else
//            return emptyRegion.unLock();
//    }
};

#endif // IIOTHREADED_H_
