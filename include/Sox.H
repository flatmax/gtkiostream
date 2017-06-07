/* This file was contributed by Deqx.com
   Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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
#ifndef SOX_H_
#define SOX_H_

#if defined(_MSC_VER) || defined(__CYGWIN__)
#error On microsoft, please rename this file to SoxWindows.H and delete this line.
#endif

#include <Debug.H>
#include <Eigen/Dense>

#define SOX_READ_FILE_OPEN_ERROR SOX_ERROR_OFFSET-1 ///< Error when libsox couldn't open the input file
#define SOX_CLOSE_FILE_ERROR SOX_ERROR_OFFSET-2 ///< Error when libsox couldn't close the file
#define SOX_READ_FILE_NOT_OPENED_ERROR SOX_ERROR_OFFSET-3 ///< Error when the input file hasn't already been opened using Sox::openRead
#define SOX_EOF_OR_ERROR SOX_ERROR_OFFSET-4 ///< Sox returned from a read or write either at the end of the file or in error
#define SOX_READ_MAXSCALE_ERROR SOX_ERROR_OFFSET-5 ///< Sox couldn't open the filename.max to read the rescale value for the audio file.
#define SOX_WRITE_FILE_OPEN_ERROR SOX_ERROR_OFFSET-6 ///< Error when libsox couldn't open the output file
#define SOX_WRITE_FILE_NOT_OPENED_ERROR SOX_ERROR_OFFSET-7 ///< Error when the output file hasn't already been opened using Sox::openRead
#define SOX_WRITE_OUT_CHANNEL_MISMATCH SOX_ERROR_OFFSET-8 ///< Error when trying to write data and the channel count doesn't match
#define SOX_WRITE_SAMPLES_WRITTEN_MISMATCH_ERROR SOX_ERROR_OFFSET-9 ///< Error when trying to write data and the incorrect amount of data is written

#include <sox.h>

#include <limits> // for NaN
#include <vector>
using namespace std;

/** Debug class for Sox
*/
class SoxDebug : virtual public Debug {
public:
    /** Constructor defining all debug strings which match the debug defined variables
    */
    SoxDebug() {
#ifndef NDEBUG
        errors[SOX_READ_FILE_OPEN_ERROR]=string("SOX: Couldn't open the input file");
        errors[SOX_CLOSE_FILE_ERROR]=string("SOX: Couldn't open the output file");
        errors[SOX_READ_FILE_NOT_OPENED_ERROR]=string("SOX: Couldn't read from file as the input file hasn't been opened yet");
        errors[SOX_EOF_OR_ERROR]=string("SOX: Either an error or the EOF was hit whilst operating on the file");
        errors[SOX_READ_MAXSCALE_ERROR]=string("SOX: couldn't open the max file to find the value to rescale the maximum to, if you continue the audio file will not be re-scaled correctly");
        errors[SOX_WRITE_FILE_OPEN_ERROR]=string("SOX: Couldn't write to file as the output file hasn't been opened yet");
        errors[SOX_WRITE_FILE_NOT_OPENED_ERROR]=string("SOX: Couldn't write to the file as the output file hasn't been opened yet");
        errors[SOX_WRITE_OUT_CHANNEL_MISMATCH]=string("SOX: you provided data to write out which doesn't match the number of channels specified during openWrite for the output file");
        errors[SOX_WRITE_SAMPLES_WRITTEN_MISMATCH_ERROR]=string("SOX: The incorrect number of samples were written to file. ");
#endif
    }

    /// Destructor
    virtual ~SoxDebug() {}
};

/** This class handles audio files using the libsox C library. libsox has advantages in that is can read and write to many many different uncompressed, compressed, lossy and non-lossy audio file formats.

The input reading uses the Eigen matrix library as the input matrix to load into.

The output writing uses a generic C++ vector<vector > > type of matrix to write out from.

In general the Eigen matrices have each channel on a column. So for example a 3 channel 1024 long audio file is returned as a matrix which is 1024x3.
This class assumes that all counts are specified in per channel, for example, Sox::read(matrix, count) will read count samples for each channel. libsox on the other hand assumes all counts specify the number of samples*channels.

NOTE: In order to be able to scale correctly, all audio files should have an associated max value file, for example test.f32 and test.f32.max. The .max file will specify to what value to rescale the magnitude to get the correct time domain sclaing.

When writing to file, the max file will be auto generated.
This class interleaves output data into the private outputBuffer member variable when the Sox::write method is called. If the write method is called with a total number of samples (channels*length) <= the current size of the outputBuffer, then it is not resized.
If the total number of samples of the outputBuffer (channels*length) > the current size of the outputBuffer, then it is resized. For this reason, a good approach to writing output data
is to use constant output total sample counts OR output total sample counts which don't continuously increase between calls.
*/
template<typename FP_TYPE_>
class Sox {
    sox_format_t *in; ///< input file
    double maxVal; ///< maxVal stored in the file fileName.max
    sox_format_t *out; ///< output file

    double outputMaxVal; ///< The maximum value passed to write
    vector<sox_sample_t> outputBuffer; ///< The output buffer for interleaving output data before writing.

    /** Close the file
    \param inputFile is either true (closes in) or false (closes out)
    \return a negative error number on failure, 0 on success
    */
    int close(bool inputFile);

    static void output_message(unsigned level, const char *filename, const char *fmt, va_list ap);

public:
    Sox(); ///< Constructor
    virtual ~Sox(); ///< Destructor

    /** Open a file for reading.
    If the input file is already open, then it is closed first.
    \param fileName the audio file to open, can be of many different types, wav, mp3, ogg, etc. etc.
    \return A negative error code on failure. SOX_READ_MAXSCALE_ERROR is returned when fileName.max is not found or opened - indicating that audio will not be rescaled correctly.
    */
    int openRead(string fileName);

    /** Read audio data from an already opened file.
    The audioData is returned with each column as an audio channel.
    \param audioData The Matrix to place the read audio into. It is resized as required and will match the size of the number of samples read where each column is a channel.
    \param count the number of samples per channel to read, 0 for everything
    \return NO_ERROR on success, or the error code otherwise. audioData holds the read data.
    */
    template <typename Derived>
    int read(Eigen::DenseBase<Derived> &audioData, int count=0){
        typedef typename Derived::Scalar Scalar;
        int retVal=NO_ERROR; // start assuming no error
        if (in) { // if the input file has been opened...
            if (count==0) // if we want everything
                count = in->signal.length/in->signal.channels;
            Eigen::Array<sox_sample_t, Eigen::Dynamic, Eigen::Dynamic> readData(count*in->signal.channels,1); // generate the necessary data store to read into.
            size_t readCount=sox_read(in, readData.data(), count*in->signal.channels); // try to read
            if (readCount==SOX_EOF) { // if we hit the end of file or have an error
                retVal=SOX_EOF_OR_ERROR;
                audioData.resize(0,0);
            } else { // all requested audio has been read, now resize the readData and

                // ensure the audioData matrix is the correct size
                if (audioData.cols()!= in->signal.channels | audioData.rows()!=readCount/in->signal.channels)
                     audioData.derived().resize(readCount/in->signal.channels, in->signal.channels);
                // this would be more efficient for conversion (but doesn't work) :
                for (int i=0; i<readCount; i++) {
                    audioData(i/in->signal.channels,i%in->signal.channels)=(double)readData(i)/(double)numeric_limits<sox_sample_t>::max();
                }
                double scaleFact=(double)numeric_limits<sox_sample_t>::max();
                audioData.derived().operator*=(scaleFact);

                if (!(maxVal != maxVal))
                    audioData.derived().operator*=(maxVal/scaleFact);
            }
        } else
            retVal=SOX_READ_FILE_NOT_OPENED_ERROR;
        return retVal;
    }

    /** Open a file for writing.
    If the output file is already open, then it is closed first.

    The size of FP_TYPE_ specifies the bit width for the output file to be created.
    \param fileName The name and path of the file to open for writing. The extension determines the type of file.
    \param fs The same rate in Hz.
    \param channels The number of channels the file should carry.
    \param maxVal The maximum value to be contained in the output vector when passed to Sox::write
    \return A negative error code on failure.
    */
    virtual int openWrite(const string &fileName, double fs, int channels, double maxVal){
        return openWrite(fileName, fs, channels, maxVal, sizeof(FP_TYPE_)*CHAR_BIT, false, sox_option_default, sox_option_default, sox_option_default);
    }

    /** Open a file for writing. Specifying the precision.
    If the output file is already open, then it is closed first.

    \param fileName The name and path of the file to open for writing. The extension determines the type of file.
    \param fs The same rate in Hz.
    \param channels The number of channels the file should carry.
    \param maxVal The maximum value to be contained in the output vector when passed to Sox::write
    \param wordSize The number of bits in the word.
    \param switchEndian Switch the endian from big to little or little to big
    \param revBytes Reverse the bytes {sox_option_no = 0, sox_option_yes = 1, sox_option_default = 2}
    \param revNibbles Reverse the nibbles {sox_option_no = 0, sox_option_yes = 1, sox_option_default = 2}
    \param revBits Reverse the bits {sox_option_no = 0, sox_option_yes = 1, sox_option_default = 2}
    \return A negative error code on failure.
    */
    virtual int openWrite(const string &fileName, double fs, int channels, double maxVal, unsigned int wordSize, bool switchEndian, int revBytes, int revNibbles, int revBits);

    /** Write the audio data to an already open file.
    The audioData is written with each column as an audio channel.
    \param audioData The matrix to store to file. Each vector is a channel.
    \return The number of samples written otherwise the appropriate error otherwise.
    */
    virtual int write(const vector<vector<FP_TYPE_> > &audioData);

    /** Write the audio data to an already open file.
    The audioData is written with each column as an audio channel.
    \param audioData The matrix to store to file. Each column is a channel.
    \return The number of samples written otherwise the appropriate error otherwise.
    */
    template <typename Derived>
    int write(const Eigen::DenseBase<Derived> &audioData) {
        int retVal=NO_ERROR; // start assuming no error
        if (out) { // if the output file has been opened...
            if (out->signal.channels!=audioData.cols())
                retVal=SOX_WRITE_OUT_CHANNEL_MISMATCH;
            else {
                int ch=out->signal.channels;
                int len=audioData.rows();
                int total=ch*len;
                if (outputBuffer.size()<total)
                    outputBuffer.resize(total);
                for (int i=0; i<audioData.cols(); i++) // stride the interleaved outputBuffer with each channel
                    for (int j=0; j<len; j++)
                        outputBuffer[j*ch+i]=(sox_sample_t)((double)audioData(j,i)*((double)numeric_limits<sox_sample_t>::max()/outputMaxVal));
                size_t writeCount=sox_write(out, &outputBuffer[0], total);
                retVal=writeCount;
            }
        } else
            retVal=SOX_WRITE_FILE_NOT_OPENED_ERROR;
        return retVal;
    }

    /** Write the audio data to an already open file.
    The audioData is written with each row as an audio channel.
    \param audioData The matrix to store to file.
    \return NO_ERROR on success, or the number written otherwise.
    */
    template <typename Derived>
    int writeTransposed(const Eigen::DenseBase<Derived> &audioData) {
        int retVal=NO_ERROR; // start assuming no error
        if (out) { // if the output file has been opened...
            if (out->signal.channels!=audioData.rows())
                retVal=SOX_WRITE_OUT_CHANNEL_MISMATCH;
            else {
                int ch=out->signal.channels;
                int len=audioData.cols();
                int total=ch*len;
                if (outputBuffer.size()<total)
                    outputBuffer.resize(total);
                for (int i=0; i<audioData.rows(); i++) // stride the interleaved outputBuffer with each channel
                    for (int j=0; j<len; j++)
                        outputBuffer[j*ch+i]=(sox_sample_t)((double)audioData(i,j)/outputMaxVal*(double)numeric_limits<sox_sample_t>::max());
                size_t writeCount=sox_write(out, &outputBuffer[0], total);
                retVal=writeCount;
            }
        } else
            retVal=SOX_WRITE_FILE_NOT_OPENED_ERROR;
        return retVal;
    }

    /** If open, close the read file.
    \return A negative error code on failure.
    */
    int closeRead(void);

    /** If open, close the write file.
    \return A negative error code on failure.
    */
    int closeWrite(void);

    /** Set the maximum value to scale input samples by.
    All read audio data will be scaled by newMax divided by the sox maximum sample value.
    \param newMax The new maximum value.
    */
    void setMaxVal(double newMax) {
        maxVal=newMax;
    }

    /** Get the maximum value to scale samples by. The maximum value for input files is read in from the max value file or set manually.
    \sa setMaxVal
    \return The maximum value used for audio input reading.
    */
    double getMaxVal(void) {
        return maxVal;
    }

    /** Get the input audio sample rate.
    \return the sample rate if the input exists, 0. otherwise.
    */
    double getFSIn(void) {
        if (in)
            return in->signal.rate;
        return 0.;
    }

    /** Get the output audio sample rate.
    \return the sample rate if the output exists, 0. otherwise.
    */
    double getFSOut(void) {
        if (out)
            return out->signal.rate;
        return 0.;
    }

    /** Get the input audio channel count.
    \return the channel count if the input exists, SOX_READ_FILE_NOT_OPENED_ERROR if the file isn't open for reading.
    */
    int getChCntIn(void) {
        if (in)
            return in->signal.channels;
        return SOX_READ_FILE_NOT_OPENED_ERROR;
    }

    /** Get the input audio channel count.
    \return the channel count if the input exists, SOX_WRITE_FILE_NOT_OPENED_ERROR if the file isn't open for writing.
    */
    int getChCntOut(void) {
        if (out)
            return out->signal.channels;
        return SOX_WRITE_FILE_NOT_OPENED_ERROR;
    }

    /** Print a list of the available file formats.
    \return A vector containing the available file format names.
    */
    vector<string> availableFormats(void);
};
#endif // SOX_H_
