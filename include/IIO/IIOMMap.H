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
#ifndef IIOMMAP_H_
#define IIOMMAP_H_

#include "IIO.H"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <stdio.h>
#include <sys/ioctl.h>

// referenced from iio_fm_radio.c
#define IIO_BLOCK_ALLOC_IOCTL   _IOWR('i', 0xa0, struct iio_buffer_block_alloc_req)
#define IIO_BLOCK_FREE_IOCTL    _IO('i', 0xa1)
#define IIO_BLOCK_QUERY_IOCTL   _IOWR('i', 0xa2, struct iio_buffer_block)
#define IIO_BLOCK_ENQUEUE_IOCTL _IOWR('i', 0xa3, struct iio_buffer_block)
#define IIO_BLOCK_DEQUEUE_IOCTL _IOWR('i', 0xa4, struct iio_buffer_block)

struct iio_buffer_block_alloc_req {
    __u32 type;
    __u32 size;
    __u32 count;
    __u32 id;
};

struct iio_buffer_block {
    __u32 id;
    __u32 size;
    __u32 bytes_used;
    __u32 type;
    __u32 flags;
    union {
        __u32 offset;
    } data;
    __u64 timestamp;
};

struct block {
    struct iio_buffer_block block;
    unsigned short *addr;
};

#define DEFAULT_BLOCK_COUNT 4
#define DEFAULT_BLOCK_SIZE 0x100000
/** Memory map multiple blocks for the IIO read subsystem.
*/
class MMappedBlocks {
    int fd; ///< The file descriptior of the device
    struct iio_buffer_block_alloc_req req;

    /** Method to get the kernel to allocate memory blocks.
    \return Returns <0 on error.
    */
    int allocate() {
        std::cout<<__func__<<std::endl;
        int ret = ioctl(fd, IIO_BLOCK_ALLOC_IOCTL, &req);
        if (ret < 0) {
            perror("Failed to allocate memory blocks");
            return IIODebug().evaluateError(IIOMMAP_ALLOCATE_ERROR);
        }
        return NO_ERROR;
    }

    /** Deallocate kernel space memory.
    */
    void deAllocate() {
        std::cout<<__func__<<std::endl;
        ioctl(fd, IIO_BLOCK_FREE_IOCTL, 0);
    }

    /** Memory map the blocks to the device.
    \return Returns !=0 on error.
    */
    int memoryMap() {
        std::cout<<__func__<<std::endl;
        std::cout << "MMappedBlocks::memoryMap resizing blocks "<<req.count<<std::endl;
        blocks.resize(req.count);
        for (uint i = 0; i < req.count; i++) {
            std::cout << "MMappedBlocks::memoryMap query i="<<i<<std::endl;
            blocks[i].block.id = i;
            if (ioctl(fd, IIO_BLOCK_QUERY_IOCTL, &blocks[i].block)!=0) {
                perror("Failed to query block");
                return IIODebug().evaluateError(IIOMMAP_QUERY_ERROR);
            }

            std::cout << "MMappedBlocks::memoryMap mapping"<<std::endl;
            blocks[i].addr = (unsigned short *)mmap(0, blocks[i].block.size, PROT_READ, MAP_SHARED, fd, blocks[i].block.data.offset);
            if (blocks[i].addr == MAP_FAILED) {
                perror("Failed to mmap block");
                return IIODebug().evaluateError(IIOMMAP_MMAP_ERROR);
            }

            std::cout << "MMappedBlocks::memoryMap enqueueing "<<std::endl;
            if (ioctl(fd, IIO_BLOCK_ENQUEUE_IOCTL, &blocks[i].block)!=0) {
                perror("Failed to enqueue block");
                return IIODebug().evaluateError(IIOMMAP_ENQUEUE_ERROR);
            }

            fprintf(stderr, "Sucessfully mapped block %d (offset %x, size %d) at %p\n", i, blocks[i].block.data.offset, blocks[i].block.size, blocks[i].addr);
        }
        return NO_ERROR;
    }

    /** unmap memory blocks.
    */
    void memoryUnmap() {
        std::cout<<__func__<<std::endl;
        for (uint i = 0; i < req.count; i++)
            munmap(blocks[i].addr, blocks[i].block.size);
        blocks.resize(0);
    }
public:
    std::vector<struct block> blocks; ///< The block information and address.

    /** Constructor.
    */
    MMappedBlocks() {
        req.size=0;
        req.count=0;
        blocks.resize(0);
    }

    /// Destructor
    ~MMappedBlocks() {
        memoryUnmap();
        deAllocate();
    }

    /** Setup the mmap blocks. Uses DEFAULT_BLOCK_COUNT and DEFAULT_BLOCK_SIZE for the count and size respectively.
    \param fdIn The open file descriptor of the IIO device to be read from.
    \return NO_ERROR or appropriate error
    */
    int reset(int fdIn) {
        return reset(fdIn, DEFAULT_BLOCK_COUNT, DEFAULT_BLOCK_SIZE);
    }

    /** Setup the mmap blocks
    \param fdIn The open file descriptor of the IIO device to be read from.
    \param count The number of blocks to use.
    \param size The size of each block (in bytes).
    \return NO_ERROR or appropriate error
    */
    int reset(int fdIn, int count, int size) {
        if (req.size!=0) {
            memoryUnmap();
            deAllocate();
        }

        int ret=NO_ERROR;
        fd=fdIn;
        req.type = 0x0;
        req.size = size;
        req.count = count;
        if (count>0 && size>0) {
            ret=allocate();
            if (ret!=NO_ERROR)
                return ret;
            ret=memoryMap();
            if (ret!=NO_ERROR) {
                memoryUnmap();
                deAllocate();
            }
        }
        return ret;
    }
};

class IIOMMap : public IIO {
    std::vector<MMappedBlocks> mMappedBlocks; ///< The memory mapped blocks.
public:
    IIOMMap() {} ///< Constructor

    /// Destructor
    virtual ~IIOMMap() {
        close();
    }

    /** Overload the IIO::open method. doesn't make sense here.
    \return NO_ERROR on success, or the appropriate error number on failure.
    */
    int open(void) {
        return IIODebug().evaluateError(IIOMMAP_WRONGOPEN_ERROR);
    }

    /** Open all of the devices all channels.
    \param count The number of memory buffers to use.
    \param sizeIn The size of each of the memory buffers in samples per channel.
    \return NO_ERROR on success, or the appropriate error number on failure.
    */
    int open(int count, int sizeIn) {
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        int ret=NO_ERROR;
        for (unsigned int i=0; i<getDeviceCnt(); i++) {
            std::cout << "opening device "<<i<<std::endl;
            if ((ret=operator[](i).open(O_RDWR))!=NO_ERROR) {
                std::ostringstream deviceDetail;
                deviceDetail<<" Device "<<i;
                IIODebug().evaluateError(ret, deviceDetail.str());
                break;
            }
        }

        std::cout << "mmapping"<<std::endl;
        if (ret==NO_ERROR) // init the memory mapped buffers
            resizeMMapBlocks(count, sizeIn);

        int closeRet;
        if (ret!=NO_ERROR) // if we couldn't open everything, then close everything
            for (unsigned int i=0; i<getDeviceCnt(); i++)
                if ((closeRet=operator[](i).close())!=NO_ERROR)
                    IIODebug().evaluateError(closeRet);
        return ret;
    }

    /** Resize the memory mapped blocks.
    \param count The number of memory buffers to use.
    \param sizeIn The size of each of the memory buffers in samples per channel.
    \return NO_ERROR on success, or the appropriate error number on failure.
    */
    int resizeMMapBlocks(int count, int sizeIn) {
        int ret=NO_ERROR;
        mMappedBlocks.resize(IIO::size());
        std::cout << "mMappedBlocks resized"<<std::endl;
        for (unsigned int i=0; i<getDeviceCnt(); i++) {
            std::cout << "resetting mmapped blocks "<<i<<std::endl;
            int ret=mMappedBlocks[i].reset(operator[](i).getFD(), count, sizeIn*operator[](i).getChCnt()*operator[](i).getChFrameSize());
            if (ret!=NO_ERROR)
                break;
        }
        return ret;
    }

    /** Close all of the devices.
    \return NO_ERROR on success, or the appropriate error number on failure.
    */
    int close(void) {
        mMappedBlocks.resize(0); // remove any memory mapping stuff.
        return IIO::close();
    }

    /** Read N samples from each channel.
    \param N The number of samples to read from each channel.
    \param array The array to fill with data.
    \return NO_ERROR on success, or the appropriate error on failure.
    \tparam TYPE the type of the samples to read in, for example signed 16 bit is short int.
    */
    template<typename TYPE>
    int read(uint N, const Eigen::Array<TYPE, Eigen::Dynamic, Eigen::Dynamic> &array) {
        // error check that the blocks are initialised, the data types match and the array is of the minimum required size.
        if (mMappedBlocks.size()<=0)
            return IIODebug().evaluateError(IIOMMAP_NOINIT_ERROR);
        if (sizeof(TYPE)!=operator[](0).getChFrameSize()) {
            std::ostringstream msg;
            msg<<"The provided array type has "<<sizeof(TYPE)<<" bytes per sample, where as the IIO devices have "<<getChFrameSize()<<" bytes per sample\n";
            return IIODebug().evaluateError(IIO_ARRAY_FRAME_MISMATCH_ERROR, msg.str());
        }
        if (array.rows()!=N*operator[](0).getChCnt() && array.cols()>getDeviceCnt()) {
            std::ostringstream msg;
            msg<<"The provided array is not shaped correctly, size=("<<array.rows()<<", "<<array.cols()<<") but size=(N*device ch cnt, device cnt) is required, where size=("<<N*getChCnt()<<", "<<getDeviceCnt()<<")\n";
            return IIODebug().evaluateError(IIO_ARRAY_SIZE_MISMATCH_ERROR, msg.str());
        }

        // grab blocks off the queue and memory copy them to the input array and re-enqueue them
        struct iio_buffer_block block; // the block to grab off the mmap queue
        for (int i=0; i <array.cols(); i++) { // read N samples from each device which is requested
            int ret = ioctl(operator[](i).getFD(), IIO_BLOCK_DEQUEUE_IOCTL, &block);
            if (ret!=0) {
                std::ostringstream msg;
                msg<<"Couldn't dequeue a mmaped block from device "<<i<<std::endl;
                return IIODebug().evaluateError(IIODEVICE_READ_ERROR, msg.str());
            }
            int bytesPerFrame=operator[](i).getChFrameSize()*operator[](i).getChCnt();
            uint bytesToRead=N*bytesPerFrame;
            if (bytesToRead!=block.size) {
                std::ostringstream msg;
                msg<<"The mmapped block has a size="<<block.size<<" and the input array requires size="<<bytesToRead<<"\n";
                return IIODebug().evaluateError(IIOMMAP_BLOCK_SIZE_MISMATCH_ERROR, msg.str());
            }

            TYPE *dataSrc=mMappedBlocks[i].blocks[block.id].addr;
            TYPE *dataDest=(unsigned short *)array.col(i).data();
            for (uint j=0; j<N*operator[](i).getChCnt(); j++)
                dataDest[j]=dataSrc[j];

            ret = ioctl(operator[](i).getFD(), IIO_BLOCK_ENQUEUE_IOCTL, &block);
            if (ret!=0) {
                std::ostringstream msg;
                msg<<"Couldn't enqueue the mmaped block to device "<<i<<std::endl;
                return IIODebug().evaluateError(IIODEVICE_READ_ERROR, msg.str());
            }
        }
        return NO_ERROR;
    }

    /** Get the maximum available time in all of the buffers.
    \return The maximum duration the buffers can hold.
    */
    double getMaxDelay(float fs){
        if (mMappedBlocks.size()<=0)
            return 0.;
        double chCnt=(double)operator[](0).getChCnt();
        double periodN=(double)mMappedBlocks[0].blocks.size();
        if (periodN<=0)
            return 0.;
        double N=(double)mMappedBlocks[0].blocks[0].block.size;
        double frameByteCnt=operator[](0).getChFrameSize();
        return periodN*N/fs/chCnt/frameByteCnt;
    }
};

#endif // IIOMMAP_H_
