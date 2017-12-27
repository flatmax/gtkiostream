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

#ifndef IIO_H_
#define IIO_H_

#include "IIODevice.H"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <Eigen/Dense>
#pragma GCC diagnostic pop

#include "DirectoryScanner.H"

#include <sstream>
#include <fstream>

#include <Thread.H>

struct iio_channel_info; ///< The iio_channel_info structure is external.

/** Industrial IO class.

In this version, this class assumes that all devices are driven by the same chip. For this reason, each device has the same number of channels and word size.

This class will open and maintain all IIO devices with the same chip name. For example :
\code
    std::string chipName("AD7476A"); // we are looking for devices on the IIO system with the "AD7476A" chip
    IIO iio;
    iio.findDevicesByChipName(chipName);

    iio.printInfo(); // print out detail about the devices which were found ...
\endcode

Which produces the following output :

\code
Device 0	chip AD7476A

 channel 0
Name		in_voltage0
Generic name	in
Index		0
unsigned, little endian
This channel has 16 bits, which require shifting down by 0 bits.
This device has 32 bits in one frame of DMA data shifting.

 channel 1
Name		in_voltage1
Generic name	in
Index		1
unsigned, little endian
This channel has 16 bits, which require shifting down by 16 bits.
This device has 32 bits in one frame of DMA data shifting.
\endcode
*/
class IIO : public std::vector<IIODevice> {
    static const char *iioDir; ///< The sys fs location of iio devcies "/sys/bus/iio/devices/"

    /** Find the name of the chip on the device at devicePath.
    \param devicePath = "/sys/bus/iio/devices/iio:deviceN"
    \return The chip name, or an empty std::string on error.
    */
    std::string findChipName(std::string devicePath) {
        std::string chipName;
        std::ifstream nameFile((devicePath+"/name").c_str());
        if (nameFile.good()) // if we can open the name
            nameFile>>chipName;
        return chipName;
    }

public:
    IIO() {}
    virtual ~IIO() {
        close();
    }

    /** Find all IIO devices with a particular chip name
    \param chipName The name of the iio chip, e.g. "AD7476"
    \return NO_ERROR on success, or the appropriate error number otherwise.
    */
    int findDevicesByChipName(std::string chipName) {
        vector<std::string> excluded; // paths we don't want to search
        excluded.push_back(".");
        excluded.push_back("..");

        DirectoryScanner ds(iioDir);
        if (ds.findAll(excluded)!=NO_ERROR) // find all of the directories
            return IIODebug().evaluateError(IIO_BAD_DEVICE_NAME_ERROR);
        ds.keepWithPattern("iio:device"); // only keep the entries of interest, namely the iio devices

        // If any of the devices have a name which matches the chipName we are after, then keep it.
        for (unsigned int i=0; i<ds.size(); i++) {
            std::string devicePath(iioDir);
            devicePath+=ds[i];
            //cout<<findChipName(devicePath)<<endl;
            // search for chipName (e.g. "ADAU7476") in the sysfs name file, which can be chipName+"_1" or some other suffix.
            if (findChipName(devicePath).find(chipName)!=std::string::npos) {
                push_back(IIODevice(devicePath, chipName)); // found a relevant device, so add it to the known devices
                operator[](size()-1).scanDevice();
            }
        }
        return NO_ERROR;
    }

//    /** Open an iio device by name.
//    \param deviceName The name of the iio device, e.g. "iio:1"
//    \return NOERROR on success, or the appropriate error number otherwise.
//    */
//    int open(std::string deviceName);

    /** Find the number of devices present.
    \return the number of devices maintained by this IIO system.
    */
    uint getDeviceCnt() {
        return size();
    }

    /** Find the total number of channels across all devices.
    \return The total number of channels which exist on all devices
    */
    uint getChCnt() {
        int chCnt=0;
        for (uint i=0; i<getDeviceCnt(); i++)
            chCnt+=operator[](i).getChCnt();
        return chCnt;
    }

    /** Print the info for each device.
    */
    void printInfo() {
        for (uint i=0; i<getDeviceCnt(); i++) {
            printf("Device %d\t",i);
            operator[](i).printInfo();
        }
    }

    /** Open all of the devices all channels.
    \return NO_ERROR on success, or the appropriate error number on failure.
    */
    int open(void) {
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        int ret=NO_ERROR;
        for (unsigned int i=0; i<getDeviceCnt(); i++) {
            if ((ret=operator[](i).open())!=NO_ERROR) {
                std::stringstream deviceDetail;
                deviceDetail<<" Device "<<i;
                IIODebug().evaluateError(ret, deviceDetail.str());
                break;
            }
        }
        int closeRet;
        if (ret!=NO_ERROR) // if we couldn't open everything, then close everything
            for (unsigned int i=0; i<getDeviceCnt(); i++)
                if ((closeRet=operator[](i).close())!=NO_ERROR)
                    IIODebug().evaluateError(closeRet);
        return ret;
    }

    /** Close all of the devices.
    \return NO_ERROR on success, or the appropriate error number on failure.
    */
    int close(void) {
        int closeRet;
        for (unsigned int i=0; i<getDeviceCnt(); i++)
            if ((closeRet=operator[](i).close())!=NO_ERROR)
                return IIODebug().evaluateError(closeRet);
        return NO_ERROR;
    }

    /** Find the number of bytes per channel.
    \return The total number of bytes in one channel or the appropriate error on failure
    */
    int getChFrameSize(void) {
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        uint frameSize=operator[](0).getChFrameSize();
        if (frameSize<0)
            return frameSize;
        for (unsigned int i=1; i<getDeviceCnt(); i++)
            if (operator[](i).getChFrameSize() != frameSize)
                return IIODebug().evaluateError(IIO_FRAEMSIZE_MISMATCH_ERROR);
        return frameSize;
    }

    /** Find the number of bytes per device frame (all channels on one device).
    \return The total number of bytes in one device frame or the appropriate error on failure
    */
    int getDevFrameSize(void) {
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);

        int frameSize=getChFrameSize();
        if (frameSize<0)
            return frameSize;
        return frameSize*getDeviceCnt();
    }

    /** Get an Array suitable for reading N samples from each channel on each device.
    The arrays are shaped as N*chCnt rows and device count columns.
    \param N The number of samples to read from each channel.
    \param[out] array The array to create and resize appropriately
    \return NO_ERROR or the suitable error. The array is returned correctly sized for the number of channels.
    */
    template<typename TYPE>
    int getReadArray(uint N, Eigen::Array<TYPE, Eigen::Dynamic, Eigen::Dynamic> &array) {
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        if (sizeof(TYPE)!=operator[](0).getChFrameSize()) {
            std::stringstream msg;
            msg<<"The provided array type has "<<sizeof(TYPE)<<" bytes per sample, where as the IIO devices have "<<getChFrameSize()<<" bytes per sample\n";
            return IIODebug().evaluateError(IIO_ARRAY_FRAME_MISMATCH_ERROR, msg.str());
        }

        if (array.rows()!=N*operator[](0).getChCnt() && array.cols()!=getDeviceCnt())
            array.resize(N*operator[](0).getChCnt(), getDeviceCnt());
        return NO_ERROR;
    }

    /** Get the numbers of samples per channel which the read array can handle.
    \param array The array to create and resize appropriately
    \return NO_ERROR or the suitable error. The array is returned correctly sized for the number of channels.
    */
    template<typename TYPE>
    int getReadArraySampleCount(Eigen::Array<TYPE, Eigen::Dynamic, Eigen::Dynamic> &array) {
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        return array.rows()/operator[](0).getChCnt();
    }

    /** Read N samples from each channel.
    \param N The number of samples to read from each channel.
    \param array The array to fill with data.
    \return NO_ERROR on success, or the appropriate error on failure.
    \tparam TYPE the type of the samples to read in, for example signed 16 bit is short int.
    */
    template<typename TYPE>
    int read(uint N, const Eigen::Array<TYPE, Eigen::Dynamic, Eigen::Dynamic> &array) {
        if (sizeof(TYPE)!=operator[](0).getChFrameSize()) {
            std::stringstream msg;
            msg<<"The provided array type has "<<sizeof(TYPE)<<" bytes per sample, where as the IIO devices have "<<getChFrameSize()<<" bytes per sample\n";
            return IIODebug().evaluateError(IIO_ARRAY_FRAME_MISMATCH_ERROR, msg.str());
        }
        if (array.rows()!=N*operator[](0).getChCnt() && array.cols()>getDeviceCnt()) {
            std::stringstream msg;
            msg<<"The provided array is not shaped correctly, size=("<<array.rows()<<", "<<array.cols()<<") but size=(N*device ch cnt, device cnt) is required, where size=("<<N*getChCnt()<<", "<<getDeviceCnt()<<")\n";
            return IIODebug().evaluateError(IIO_ARRAY_SIZE_MISMATCH_ERROR, msg.str());
        }
        uint NOrig=N;
        while (N){
            uint toRead=NOrig;
            //uint toRead=1024;
            if (toRead>N) toRead=N;
            for (int i=0; i <array.cols(); i++) { // read N samples from each device which is requested
                int ret=operator[](i).read(toRead, (void*)array.col(i).data());
                if (ret<0){ // error
                    std::stringstream msg;
                    msg<<"Couldn't read the desired number of samples from device "<<i<<std::endl;
                    return IIODebug().evaluateError(IIODEVICE_READ_ERROR, msg.str());
                } else
                    if (i==0){ // follow the lead of the first device
                        toRead=ret;
                        N-=ret;
                        //cout<<"read "<<ret<<" frames \t with "<<N<<" frames left\n";
                    }
            }
        }
        return NO_ERROR;
    }

    /** Enable or disable all of the devices.
    \param enable True to enable, false to disable.
    */
    int enable(bool enable) {
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        for (unsigned int i=0; i<getDeviceCnt(); i++)
            operator[](i).enable(enable);
		return NO_ERROR;
    }

    /** Find the number of samples per channel which the buffer can hold.
    \return the maximum number of samples which the buffer can hold, otherwise an error.
    */
    int getChannelBufferCnt(){
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        int chBufCnt=operator[](0).getChannelBufferCnt();
        for (unsigned int i=1; i<getDeviceCnt(); i++)
            if (operator[](i).getChannelBufferCnt()!=chBufCnt)
                return IIODebug().evaluateError(IIODEVICE_CHBUFCNT_ERROR);
        return chBufCnt;
    }

    /** Set the number of samples per channel which the buffer can hold.
    \param chBufferCnt The number of samples per channel the buffer can hold.
    \return the maximum number of samples which the buffer can hold, otherwise an error.
    */
    int setChannelBufferCnt(int chBufCnt){
        if (getDeviceCnt()<1)
            return IIODebug().evaluateError(IIO_NODEVICES_ERROR);
        int chBufCntOrig=getChannelBufferCnt();
        chBufCnt=operator[](0).setChannelBufferCnt(chBufCnt);
        for (unsigned int i=1; i<getDeviceCnt(); i++)
            if (operator[](i).setChannelBufferCnt(chBufCnt)!=chBufCnt){
                setChannelBufferCnt(chBufCntOrig); // try to rewind ... may be dissasterous ! TODO remove recurrence here
                return IIODebug().evaluateError(IIODEVICE_CHBUFCNT_ERROR);
            }
        return chBufCnt;
    }
};

const char *IIO::iioDir = "/sys/bus/iio/devices/";

#endif // IIO_H_
