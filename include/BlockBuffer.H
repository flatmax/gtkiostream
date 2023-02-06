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
#ifndef BLOCKBUFFER_H_
#define BLOCKBUFFER_H_

#include <queue>
#include <Thread.H>
#include <Eigen/Dense>

#define BLOCK_BUFFER_DEFAULT_COUNT 3

/** Class to manage used and unused buffers for double or more buffering.
Uses mutexes so this class is thread safe.
All buffers start on the emptyBuffers queue.
*/
class BlockBuffer {
    std::vector<Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> > buffers; ///< The vector of buffers
    std::queue<Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *> emptyBuffers; ///< The empty buffer queue
    std::queue<Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *> fullBuffers; ///< The full buffer queue

    Mutex fullBufferMutex; ///< Used for
    Mutex emptyBufferMutex; ///< Used for

    void init(int count) {
        buffers.resize(count);
        for (int c=0; c<count; c++)
            emptyBuffers.push(&buffers[c]);
    }
public:
    /** Constructor
    \param count The number of buffers to create.
    */
    BlockBuffer(int count) {
        init(count);
    }

    /// Constructor - creates BLOCK_BUFFER_DEFAULT_COUNT buffers
    BlockBuffer(void) {
        init(BLOCK_BUFFER_DEFAULT_COUNT);
    }

    /** Pop the next empty buffer off the empty queue.
    The returned buffer pointer is no longer held by the BlockBuffer::emptyBuffers nor BlockBuffer::fullBuffers and must be put back onto the empty or full buffers after use.
    \return An empty buffer for use, if none are available the NULL.
    */
    Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *getEmptyBuffer(void){
        Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *retBuf=NULL;
        emptyBufferMutex.lock();
        if (emptyBuffers.size()){
            retBuf=emptyBuffers.front();
            emptyBuffers.pop();
        }
        emptyBufferMutex.unLock();
        return retBuf;
    }

    /** Pop the next full buffer off the full queue.
    The returned buffer pointer is no longer held by the BlockBuffer::fullBuffers nor BlockBuffer::emptyBuffers and must be put back onto the empty or full buffers after use.
    \return A full buffer for use, if none are available the NULL.
    */
    Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *getFullBuffer(void){
        Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *retBuf=NULL;
        fullBufferMutex.lock();
        if (fullBuffers.size()){
            retBuf=fullBuffers.front();
            fullBuffers.pop();
        }
        fullBufferMutex.unLock();
        return retBuf;
    }

    /** Push a full buffer to the full queue.
    \param fb The full buffer to add to the full queue.
    */
    void putFullBuffer(Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *fb){
        fullBufferMutex.lock();
        fullBuffers.push(fb);
        fullBufferMutex.unLock();
    }

    /** Push an empty buffer to the empty queue.
    \param fb The full buffer to add to the full queue.
    */
    void putEmptyBuffer(Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *eb){
        emptyBufferMutex.lock();
        emptyBuffers.push(eb);
        emptyBufferMutex.unLock();
    }

    /** Find the number of buffers available in total.
    \return the total buffer count.
    */
    int getBufferCount(){
        return buffers.size();
    }

    /** resize all of the buffers
    \param rows The number of rows to create in each buffer.
    \param cols The number of cols to create in each buffer.
    */
    void resizeBuffers(int rows, int cols){
        emptyBufferMutex.lock();
        fullBufferMutex.lock();
        for (int i=0; i<buffers.size(); i++)
            buffers[i].resize(rows, cols);
        emptyBufferMutex.unLock();
        fullBufferMutex.unLock();
    }

    /** Resise the number of buffers contained. Each buffer is resized to the current buffer row/col sizes.
    All buffers are created and the emptyBuffers queue contains them. The fullBuffers queue is empty.
    Note: This should not be run whilst in operation. Ensure no other threads are accessing this class.
    \param count The number of buffers to create.
    */
    void resize(int count){
        buffers.resize(count);
        int rows=0, cols=0;
        // find the current matrix sizes and mimick - try either a full or empty buffer. All should be the same.
        Eigen::Array<unsigned short, Eigen::Dynamic, Eigen::Dynamic> *b=getFullBuffer();
        if (!b)
            b=getEmptyBuffer();
        if (b) {
            rows=b->rows();
            cols=b->cols();
        }

        // remove any existing pointers
        while ((b=getFullBuffer())!=NULL);
        while ((b=getEmptyBuffer())!=NULL);

        // create all of the buffers and set them up as empty.
        emptyBufferMutex.lock();
        fullBufferMutex.lock();
        init(count);
        emptyBufferMutex.unLock();
        fullBufferMutex.unLock();
        resizeBuffers(rows, cols);
    }

// IF you plan to use a class which inherits from a stl container, then this will conflict, leaving out for now.
//    /** Return a pointer to one of the buffers in the buffer vector which contains all buffers in constant disorder.
//    \param i The i'th buffer to index from the buffer vector (0 <= i < getBufferCount()).
//    \return A pointer to the i'th buffer or NULL on failure.
//    */
//    Eigen::DenseBase<Derived> *operator[](int i){
//        if (i<buffers.size() & i>=0)
//            return &buffers[i];
//        return NULL;
//    }
};

#endif // BLOCKBUFFER_H_
