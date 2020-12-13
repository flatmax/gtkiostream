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

#ifndef IIODEVICE_H_
#define IIODEVICE_H_

#include "DirectoryScanner.H"
#include "IIOChannel.H"

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <poll.h>

//#define IIO_NONBLOCK_READS ///< define this to enable nonblocking read

/** Maintain an IIO device. It inherits a std::vector of channels.
*/
class IIODevice : public std::vector<IIOChannel> {
    std::string devicePath; ///< The device path
    std::string chipName; ///< The device chip name
    float scale; ///< How to scale the input value to get a real world value

    std::string readDev; ///< The read buffer device, e.g. "/dev/iio:device0"

    int fd; ///< The file descriptor of the open device

public:
    /** Constructor specifying the device path and the chip name
    \param devicePathIn The device's path, such as "/sys/bus/iio/devices/iio:device0"
    \param chipNameIn The device's chip, such as "AD7476A"
    */
    IIODevice(const std::string &devicePathIn, const std::string &chipNameIn) {
        fd=-1;
        devicePath=devicePathIn;
        chipName=chipNameIn;
        // format the expected read device name :
        readDev="/dev/iio:device"+devicePath.substr(devicePath.find(":device")+7);
    }

    virtual ~IIODevice() {
        close(); // if the device is open then close it.
    }

    /** Scan an IIO device for channel requirements.
    \return NO_ERROR, or the arropriate error on failure.
    */
    int scanDevice(void) {
        int ret=NO_ERROR;
        if (size()>0) {
            close();
            resize(0);
        }

        // find how many channes exist on the device
        std::string scanPath=devicePath+"/scan_elements";
        std::string extName="_en";
        DirectoryScanner ds(scanPath);
        ds.findAll(std::vector<std::string>());
        ds.keepWithPattern(extName);

        // count the number of enabled channels and store their base names
        int enabled; // temporary variable to store enabled flag
        for (unsigned int i=0; i<ds.size(); i++) {
            std::ifstream inputF((scanPath+"/"+ds[i]).c_str());
            if (!inputF.good())
                return IIODebug().evaluateError(IIODEVICE_FILE_OPEN_ERROR, " Couldn't open the enabled file "+scanPath+"/"+ds[i]);
            inputF>>enabled;
            inputF.close();
            if (enabled) {// create a channel, note its name and scan the elements.
                IIOChannel iIOChannel;
                iIOChannel.chName=ds[i].substr(0,ds[i].length()-extName.length()); // such as "in_voltage"
                iIOChannel.chGenericName=iIOChannel.chName.substr(0,iIOChannel.chName.find("_")); // such as "in"
                ret=iIOChannel.scanElements(scanPath, size()); // scan the elements from the _type file
                if (ret!=NO_ERROR)
                    return ret;

                // For the new channel, add it to the list and scan its nature ...
                push_back(iIOChannel); // Add the channel to the list of enabled channels
//                operator[](size()-1).printInfo();
//                std::cout<<std::endl;
            }
        }

        std::cout<<"IIODevice : bufferSize="<<getBufferSize()<<std::endl;
        return ret;
    }

    /** Get the number of enabled channels for this device.
    \return The number of enabled channels.
    */
    uint getChCnt(void) {
        return size();
    }

    /** Print the info for each channel.
    */
    void printInfo() {
        std::cout<<"chip "<<chipName<<std::endl;
        if (size()<1)
            IIODebug().evaluateError(IIODEVICE_NOCHANNELS_ERROR);
        for (unsigned int i=0; i<getChCnt(); i++) {
            std::cout<<"\n channel "<<i<<'\n';
            operator[](i).printInfo();
        }
    }

    /** Open the device.
    \return NO_ERROR, or the arropriate error on failure.
    */
    int open(void) {
        int flags=O_RDONLY;
#ifdef IIO_NONBLOCK_READS
        int flags|=O_NONBLOCK;
#endif
        return open(flags);
    }

    /** Open the device.
    \param flags The flags to open the device with.
    \return NO_ERROR, or the arropriate error on failure.
    */
    int open(int flags) {
        close();
        // can't read and write yet
        int cntRead=0, cntWrite=0;
        for (unsigned int i=0; i<size(); i++)
            if (operator[](i).isReadCh())
                cntRead++;
            else
                cntWrite++;
        if (cntRead!=0 && cntWrite!=0)
            return IIODebug().evaluateError(IIODEVICE_READANDWRITE_ERROR);
        if (cntWrite!=0)
            return IIODebug().evaluateError(IIODEVICE_WRITEABLE_ERROR);

        fd=::open(readDev.c_str(), flags);
        if (fd<0) {
            perror(NULL);
            return IIODebug().evaluateError(IIODEVICE_OPEN_ERROR, "When trying to open the device path "+readDev);
        }
        return NO_ERROR;
    }

    /** Close the device.
    \return NO_ERROR, or the arropriate error on failure.
    */
    int close(void) {
        enable(false); // disable the buffer
        if (fd!=-1) {
            fd=::close(fd);
            if (fd<0) {
                perror(NULL);
                return IIODebug().evaluateError(IIODEVICE_OPEN_ERROR, "When trying to close the device path "+readDev);
            }
            fd=-1;
        }
        return NO_ERROR;
    }

    /** Read from the device.
    \param frames The number of samples to read
    \param buf Pointer to the read location.
    \return NO_ERROR, or the arropriate error on failure. The buffer holds count bytes in it.
    */
    int read(uint frames, void *buf) {
        int bytesPerFrame=getChFrameSize()*getChCnt();
        uint bytesToRead=frames*bytesPerFrame;
        if (fd<0)
            return IIODebug().evaluateError(IIODEVICE_OPEN_ERROR, "The device should already be opened before trying to read error. ");

#ifdef IIO_NONBLOCK_READS
        struct pollfd pfd = { .fd = fd, .events = POLLIN,};
        poll(&pfd, 1, -1);
#endif

        ssize_t N=::read(fd, buf, bytesToRead);
        if (N!=bytesToRead) {
            if (N>=0)
                return N/bytesPerFrame;
            else {
                std::cout<<"read returned "<<N<<"\n";
                if (N == -EAGAIN)
                    std::cout<<"nothing available\n";
                else {
                    std::cout<<strerror(errno)<<"\n";
                    perror(NULL);
                    return IIODebug().evaluateError(IIODEVICE_READ_ERROR, "Error during read.");
                }
            }
        }
        //std::cout<<"read "<<N<<" bytes, wanted "<<bytesToRead<<" bytes"<<std::endl;
        return frames;
    }

    /** Find the number of bytes per channel.
    \return The total number of bytes in one channel or the appropriate error on failure
    */
    uint getChFrameSize(void) {
        if (size()<1)
            return IIODebug().evaluateError(IIODEVICE_NOCHANNELS_ERROR);
        unsigned int frameSize=operator[](0).bitCnt/8;
        for (unsigned int i=1; i<size(); i++)
            if (operator[](i).bitCnt/8 != frameSize)
                return IIODebug().evaluateError(IIODEVICE_FRAEMSIZE_MISMATCH_ERROR);
        return frameSize;
    }

    /** Enable or disable the device.
    \param enable True to enable, false to disable.
    \return NO_ERROR or the appropriate error on failure.
    */
    int enable(bool enable) {
        std::ofstream enableFile((devicePath+"/buffer/enable").c_str());
        if (!enableFile.good())
            return IIODebug().evaluateError(IIODEVICE_ENABLEFILE_ERROR, "Error when trying to open the enable file "+devicePath+"/buffer/enable");
        enableFile<<enable;
        enableFile.close();
        return NO_ERROR;
    }

    /** Find the DMA buffer size.
    \return The buffer size in samples, or the appropriate error on failure.
    */
    int getBufferSize() {
        std::ifstream lengthFile((devicePath+"/buffer/length").c_str());
        if (!lengthFile.good())
            return IIODebug().evaluateError(IIODEVICE_LENGTHFILE_ERROR, "Error when trying to open the buffer length file "+devicePath+"/buffer/length");
        int bufferSize;
        lengthFile>>bufferSize;
        return bufferSize;
    }

    /** Set the DMA buffer size
    \return The resulting buffer size in samples for all channels, or the appropriate error on failure.
    */
    int setBufferSize(int bufferSize) {
        if (getBufferSize()!=bufferSize){
            std::ofstream lengthFile((devicePath+"/buffer/length").c_str());
            if (!lengthFile.good())
                return IIODebug().evaluateError(IIODEVICE_LENGTHFILE_ERROR, "Error when trying to open the buffer length file "+devicePath+"/buffer/length");
            lengthFile<<bufferSize;
        }
        return getBufferSize();
    }

    /** Find the number of samples per channel which the buffer can hold.
    \return the maximum number of samples which the buffer can hold.
    */
    int getChannelBufferCnt() {
        return getBufferSize()/getChCnt();
    }

    /** Set the number of samples per channel which the buffer can hold.
    \param chBufferCnt The number of samples per channel the buffer can hold.
    \return the maximum number of samples which the buffer can hold or error.
    */
    int setChannelBufferCnt(int chBufferCnt) {
        setBufferSize(chBufferCnt*getChCnt()); // if an error is returned, assume that the buffer length file is still valid
        return getBufferSize()/getChCnt();
    }

    /** Get the device's file descriptor.
    \return the file descriptor
    */
    int getFD(){
        return fd;
    }
};
#endif // IIODEVICE_H_
