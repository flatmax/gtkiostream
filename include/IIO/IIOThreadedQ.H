/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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

#ifndef IIOTHREADEDQ_H_
#define IIOTHREADEDQ_H_

#include "IIO.H"
#include "Thread.H"
#include "BlockBuffer.H"

class IIOThreadedQ : public IIO, public ThreadedMethod, public Cond, public BlockBuffer {

    /** All reading is done in a threaded environment.
    This ensures that you can process data whilst new data is being read in.
    */
    void *threadMain(void) {
        struct sched_param param;
        param.sched_priority = 96;
        if (sched_setscheduler(0, SCHED_FIFO, & param) == -1) {
            perror("sched_setscheduler");
            return NULL;
        }

        struct timespec lockStart, lockStop;

        Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *b; // get an empty buffer for query

        cout<<"entering the thread while loop"<<endl;
        while (1) {

            if( clock_gettime( CLOCK_REALTIME, &lockStart) == -1 )
                cout<<"clock lockStart get time error"<<endl;
            int nframes=0;
            b=getEmptyBuffer(); // get an empty buffer
            if (!b) {
                cout<<"threadMain : Error : couldn't get a valid empty buffer - possibly dropping samples.\n";
                usleep(1000); // sleep for a ms
            } else {
                nframes=getReadArraySampleCount(*b);
//                cout<<"b rows,cols = "<<b->rows()<<","<<b->cols()<<'\n';
//                cout<<"planning to read nframes="<<nframes<<" per cycle\n";
                int ret=read(nframes, *b);
                if (ret!=NO_ERROR)
                    break;

                putFullBuffer(b); // put the now full buffer onto the full buffer queue

                lock(); // lock the mutex, indicate the condition and wake the thread.
                newbufReady=true;
                signal(); // Wake the WaitingThread
                unLock(); // Unlock so the WaitingThread can continue.

            }
            if( clock_gettime( CLOCK_REALTIME, &lockStop) == -1 )
                cout<<"clock lockStop get time error"<<endl;

            double duration = 1.e3*( lockStop.tv_sec - lockStart.tv_sec ) + (double)( lockStop.tv_nsec - lockStart.tv_nsec )/1.e6;
            cout<<"thread duration = "<<duration<<'\t';
            if (duration<(.5*(float)nframes/sampleRate*1.e3)){
                //cout<<"too quick, adding more time"<<endl;
                usleep((int)floor((float)nframes*.5/sampleRate*1.e6));
            }

            if( clock_gettime( CLOCK_REALTIME, &lockStop) == -1 )
                cout<<"clock lockStop get time error"<<endl;

            duration = 1.e3*( lockStop.tv_sec - lockStart.tv_sec ) + (double)( lockStop.tv_nsec - lockStart.tv_nsec )/1.e6;
            cout<<"thread duration now = "<<duration<<'\n';
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

        // find out how big the buffers should be
        Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> b;
        int retVal=getReadArray(N, b);
        if (retVal!=NO_ERROR)
            return retVal;

        ch=(int)ceil((float)ch/(float)operator[](0).getChCnt()); // check whether we require less then the available number of channels
        if (b.cols()<ch)
            ch=b.cols();
        cout<<"resizing buffers to "<<b.rows()<<" rows and "<<ch<<" cols"<<endl;

        // ensure that the buffers exist with the correct sizes
        BlockBuffer::resizeBuffers(b.rows(), ch);
        //setChannelBufferCnt(N*2);
        return NO_ERROR;
    }

public:
    float sampleRate;
    bool newbufReady;

    IIOThreadedQ () {
        BlockBuffer::resize(10);
        newbufReady=false;
    }

    virtual ~IIOThreadedQ() {}

    int setSampleCountChannelCount(uint N, uint ch) {
//        int oneDevChCnt=operator[](0).getChCnt();
//        int cols=(int)ceil((float)ch/(float)oneDevChCnt); // check whether we require less then the available number of channels
//        return IIOThreadedQ::resizeBuffers(N*oneDevChCnt, ch);
        return IIOThreadedQ::resizeBuffers(N, ch);
    }
};

#endif // IIOTHREADEDQ_H_
