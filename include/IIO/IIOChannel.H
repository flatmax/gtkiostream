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
#ifndef IIOCHANNEL_H_
#define IIOCHANNEL_H_

#include <iostream>
#include <fstream>
#include <sstream>

#include "Debug.H"
#define IIO_BAD_DEVICE_NAME_ERROR IIO_ERROR_OFFSET-1 ///< Error when the specified device name is bad or can't be found.
#define IIO_BAD_DEVICE_TRIGGER_ERROR IIO_ERROR_OFFSET-2 ///< Error when the specified trigger doesn't exist.
#define IIO_BAD_CH_ARRAY_ERROR IIO_ERROR_OFFSET-3 ///< Error when the channel array can't be built or populated.
#define IIODEVICE_FILE_OPEN_ERROR IIO_ERROR_OFFSET-4 ///< Error when trying to open an IIODevice subdirectory file
#define IIODEVICE_NOCHANNELS_ERROR IIO_ERROR_OFFSET-5 ///< Error when trying to open the device, but the channels aren't initialised
#define IIODEVICE_READANDWRITE_ERROR IIO_ERROR_OFFSET-6 ///< Don't know how to hbandle read and write yet
#define IIODEVICE_WRITEABLE_ERROR IIO_ERROR_OFFSET-7 ///< Don't know how to write yet
#define IIODEVICE_OPEN_ERROR IIO_ERROR_OFFSET-8 ///< Couldn't open the device.
#define IIODEVICE_FRAEMSIZE_MISMATCH_ERROR IIO_ERROR_OFFSET-9 ///< The frame sizes differ between channels on one device
#define IIO_FRAEMSIZE_MISMATCH_ERROR IIO_ERROR_OFFSET-9 ///< The channel frame sizes differ between devices
#define IIO_NODEVICES_ERROR IIO_ERROR_OFFSET-10 ///< There are no devices
#define IIO_ARRAY_FRAME_MISMATCH_ERROR IIO_ERROR_OFFSET-11 ///< The sample type of the provided array doesn't match the sample type of the devices.
#define IIO_ARRAY_SIZE_MISMATCH_ERROR IIO_ERROR_OFFSET-12 ///< The provided array doesn't match the number of channels and requested samples.
#define IIODEVICE_READ_ERROR IIO_ERROR_OFFSET-13 ///< There was an error whilst reading from a device
#define IIODEVICE_ENABLEFILE_ERROR IIO_ERROR_OFFSET-14 ///< There was an error when trying to open the buffer enable file
#define IIODEVICE_LENGTHFILE_ERROR IIO_ERROR_OFFSET-15 ///< There was an error when trying to open the buffer length file
#define IIODEVICE_CHBUFCNT_ERROR IIO_ERROR_OFFSET-16 ///< One of the devices has a different buffer size to the other devices.
#define IIODEVICE_RESIZEBUF_NEW_ERROR IIO_ERROR_OFFSET-17 ///< One of the devices has a different buffer size to the other devices.
#define IIOMMAP_ALLOCATE_ERROR IIO_ERROR_OFFSET-18 ///< Couldn't allocate the iio memory buffers in the kernel using ioctl
#define IIOMMAP_QUERY_ERROR IIO_ERROR_OFFSET-19 ///< Couldn't query the iio device for allocated kernel memory.
#define IIOMMAP_MMAP_ERROR IIO_ERROR_OFFSET-20 ///< Couldn't execute mmap.
#define IIOMMAP_ENQUEUE_ERROR IIO_ERROR_OFFSET-21 ///< Couldn't enqueue the mmaped block
#define IIOMMAP_NOINIT_ERROR IIO_ERROR_OFFSET-22 ///< The MMapedBlocks system is not initialised
#define IIOMMAP_WRONGOPEN_ERROR IIO_ERROR_OFFSET-23 ///< The wrong open method was called.
#define IIOMMAP_BLOCK_SIZE_MISMATCH_ERROR IIO_ERROR_OFFSET-24 ///< The user and mmaped block sizes don't match

#ifndef uint
typedef unsigned int uint; ///< The uint type definition
#endif

/** Debug class for IIO.
*/
class IIODebug : public Debug {
public:
    /** Constructor defining all debug std::strings which match the debug defined variables
    */
    IIODebug() {
#ifndef NDEBUG
        errors[IIO_BAD_DEVICE_NAME_ERROR]=std::string("IIO: Empty or invalid device name specified for opening the device. ");
        errors[IIO_BAD_DEVICE_TRIGGER_ERROR]=std::string("IIO: Trigger couldn't be found. ");
        errors[IIO_BAD_CH_ARRAY_ERROR]=std::string("Problem reading scan element information for the channel array. ");
        errors[IIODEVICE_FILE_OPEN_ERROR]=std::string("Problem opening a subfile for the device. ");
        errors[IIODEVICE_NOCHANNELS_ERROR]=std::string("There are no channels, please scan elements first. ");
        errors[IIODEVICE_READANDWRITE_ERROR]=std::string("This class can not read and write to different channels - YET! Please expand the functionality. ");
        errors[IIODEVICE_WRITEABLE_ERROR]=std::string("This class can not write to channels - YET! Please expand the functionality. ");
        errors[IIODEVICE_OPEN_ERROR]=std::string("The device couldn't be opened for reading. ");
        errors[IIODEVICE_FRAEMSIZE_MISMATCH_ERROR]=std::string("The channels in one device have different word lengths, not able to handle this functionality YET!, please expand functionality. ");
        errors[IIO_FRAEMSIZE_MISMATCH_ERROR]=std::string("The device's channels have different word lengths, not able to handle this functionality YET!, please expand functionality. ");
        errors[IIO_NODEVICES_ERROR]=std::string("No devices have been found, run the method IIO::findDevicesByChipName first. ");
        errors[IIO_ARRAY_FRAME_MISMATCH_ERROR]=std::string("The provided array has an incorrect type size. It doesn't match a single channel byte count. ");
        errors[IIO_ARRAY_SIZE_MISMATCH_ERROR]=std::string("The provided array is not large enough or is too large to capture the specified number of samples. ");
        errors[IIODEVICE_READ_ERROR]=std::string("Error whilst reading. ");
        errors[IIODEVICE_ENABLEFILE_ERROR]=std::string("Error whilst trying to open the buffer enable file. ");
        errors[IIODEVICE_LENGTHFILE_ERROR]=std::string("Error whilst trying to open the buffer length file. ");
        errors[IIODEVICE_CHBUFCNT_ERROR]=std::string("Error one (or more) of the devices has a different DMA buffer size to the rest of the devices. The size is in the buffer/length file on the iio file system and should have the same size and channel count for all devices.");
        errors[IIODEVICE_RESIZEBUF_NEW_ERROR]=std::string("Error couldn't malloc the required read buffer.");
        errors[IIOMMAP_ALLOCATE_ERROR]=std::string("Error failed to allocate memory buffers using ioctl. ");
        errors[IIOMMAP_QUERY_ERROR]=std::string("Error query ioctl for the allocated memory buffer. Did you allocate them using ioctl first ? ");
        errors[IIOMMAP_MMAP_ERROR]=std::string("Error memory mapping the memory block. ");
        errors[IIOMMAP_ENQUEUE_ERROR]=std::string("Error enqueueing the memory mapped memory block. ");
        errors[IIOMMAP_NOINIT_ERROR]=std::string("Error the memory mapped IIO blocks aren't initialised, do that first. ");
        errors[IIOMMAP_WRONGOPEN_ERROR]=std::string("Error when using MMAP, you must use the IIOMMap::open(int) method, noth the IIOMMap::open() method. ");
        errors[IIOMMAP_BLOCK_SIZE_MISMATCH_ERROR]=std::string("Error when about to copy memory from the mmaped block to the user provided memory.\nMemory byte count mismatch. ");

#endif
    }
};

/** Maintains channel information for an IIO device. Adds capability for the channel to scan its elements and work out its nature.
*/
class IIOChannel {
public:
//    unsigned int offset; ///< The offset [explain this more]
    unsigned int index; ///< The index in the interleaved stream
    std::string chName; ///< The name of each of the enabled channels
    std::string chGenericName; ///< The generic name of each of the enabled channels
    bool isLittleEndian; ///< Indicates whether the channel is little endian
    bool isSigned; ///< Indicates whether the data is signed or unsigned.
    uint bitCnt; ///< The number of bits in one word of data for one channel
    uint deviceBitCnt; ///< The number of bits in one device word - this is all the channels combined into a device frame
    uint bitShiftCnt; ///< The number of bits to shift the channel word down by to get a proper reading.

    /** Setup the relevant channel parameters from the scan_elements directory in sysfs
    \param scanPath The path to the scan_elements directory
    \param cIndex The sysfs channel element to scan
    \return NO_ERROR, or the arropriate error on failure.
    */
    int scanElements(std::string scanPath, uint cIndex){
        int ret=NO_ERROR;
        // get the index
        std::ifstream inputF((scanPath+"/"+chName+"_index").c_str());
        if (!inputF.good())
            return IIODebug().evaluateError(IIODEVICE_FILE_OPEN_ERROR, " Couldn't open the index file "+scanPath+"/"+chName+"_index");
        inputF>>index;
        inputF.close();

        // Get the type information for this channel ...
        // one example : "le:u16/32>>0", another example : "le:u16/32>>16"
        std::stringstream typeInfo;
        inputF.open((scanPath+"/"+chName+"_type").c_str()); // Using the chName_type file,
        if (!inputF.good())
            return IIODebug().evaluateError(IIODEVICE_FILE_OPEN_ERROR, " Couldn't open the type file "+scanPath+"/"+chName+"_type");
        typeInfo << inputF.rdbuf(); // get all of the type info in the std::string stream

        // break the type info up into parameters.
        std::string token(10,'\0'); // check the endian-ness
        typeInfo.getline((char*)token.c_str(), 10, ':');
        if (token.find("le")!=std::string::npos)
            isLittleEndian=true;

        char signedChar, slash; // slash is for irrelevent characters like the '/' and '>'
        typeInfo>>signedChar>>bitCnt>>slash>>deviceBitCnt>>slash>>slash>>bitShiftCnt;
        if (signedChar=='u')
            isSigned=false;
        return ret;
    }

    void printInfo(){
        std::cout<<"Name\t\t"<<chName<<std::endl;
        std::cout<<"Generic name\t"<<chGenericName<<std::endl;
        //std::cout<<"Offset "<<offset<<"\t index "<<index<<std::endl;
        std::cout<<"Index\t\t"<<index<<std::endl;
        if (isSigned)
            std::cout<<"signed, ";
        else
            std::cout<<"unsigned, ";
        if (isLittleEndian)
            std::cout<<"little endian"<<std::endl;
        else
            std::cout<<"big endian"<<std::endl;
        std::cout<<"This channel has "<<bitCnt<<" bits, which require shifting down by "<<bitShiftCnt<<" bits."<<std::endl;
        std::cout<<"This device has "<<deviceBitCnt<<" bits in one frame of DMA data shifting."<<std::endl;
    }

    /** Find if this channel is an input channel or and output channel.
    \return true if this is a read channel, false if it is a write channel.
    */
    bool isReadCh(void){
        if (chGenericName.find("in")!=std::string::npos)
            return true;
        return false;
    }
};

#endif // IIOCHANNEL_H_
