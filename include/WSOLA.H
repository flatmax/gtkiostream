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
#ifndef WSOLA_H_
#define WSOLA_H_

#include "gtkiostream_config.h"
#ifdef HAVE_EMSCRIPTEN
#include <emscripten/bind.h>
#endif

#ifdef _MSC_VER
#define MS_ROUND
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

/// Microsoft doesn't have a round function
double round(double number) {
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}
#endif

#include <Debug.H> ///< Provided by GTKIOStream on sf.net

#define WSOLA_MOD2_ERROR -10+WSOLA_ERROR_OFFSET ///< Occurs when the BUFF_SIZE is not divisible by 2
#define WSOLA_NFRAMES_JACK_ERROR -11+WSOLA_ERROR_OFFSET ///< Occurs when jack wants to process nframes which is not divisible by N/2
#define WSOLA_ROWS_ERROR -12+WSOLA_ERROR_OFFSET ///< Occurs when trying to access a row > the input or output Array rows.
#define WSOLA_COLS_ERROR -13+WSOLA_ERROR_OFFSET ///< Occurs when trying to access a col > the input or output Array cols.

/** Debug class for WSOLA
*/
class WSOLADebug : virtual public Debug {
public:
    /** Constructor defining all debug strings which match the debug defined variables
    */
    WSOLADebug() {
#ifndef NDEBUG
        errors[WSOLA_MOD2_ERROR]=std::string("Developer error : BUFF_SIZE must be divisible by 2. ");
        errors[WSOLA_NFRAMES_JACK_ERROR]=std::string("Jack nframes request error : Jack wants to process a number of frames which WSOLA can't handle. ");
        errors[WSOLA_ROWS_ERROR]=std::string("Row request error : You are trying to access beyond the end of the array. ");
        errors[WSOLA_COLS_ERROR]=std::string("Col request error : You are trying to access beyond the end of the array. ");
#endif
    }

    /// Destructor
    virtual ~WSOLADebug() {}
};

#include <Eigen/Dense>
using namespace Eigen;

typedef float FP_TYPE; ///< The floating point type to use if not previously declared.

#define FS_DEFAULT 48000. ///< Hz the sample rate
#define TAU 0.02 ///< ms The window size

#define DEFAULT_CH_CNT 1 ///< The default number of channels to use

#define M_DEFAULT 3; ///< The default number of buffers to search.

/** Class which implements the Waveform Similarity Overlap Add (Embedded WSOLA).

This class allows you to time scale modify multi-channel audio. It speeds up or slows down audio without changing its pitch.

This Class uses Eigen to compute all vector operations in the aim of ensuring efficient hardware utilisation and speed.
*/
class WSOLA {

    float fs; ///< The sample rate in Hz

    int N; ///< The size of the window
    int NO2; ///< Half the window size
    int M; ///< The number of windows in the search buffer
    int m; ///< The current row index into the buffer
    double rem; ///< The remainder fraction of a sample to remember for next time (can't move on by fractions of a sample).

    Array<FP_TYPE, Dynamic , Dynamic> buffer; ///< The buffer of audio, each channel on its own row

    Array<FP_TYPE, Dynamic, Dynamic> wnd; ///< The overlap add window

    Array<FP_TYPE, Dynamic, Dynamic> nextOutput; ///< The output vector to search for, each channel per row

    Array<FP_TYPE, Dynamic, Dynamic> simComp; ///< Temporary vector used for computation

    /** Find the most similar vector in a buffer of vectors to the input reference.
    \param buffer The matrix of vectors to compare against the reference
    \tparam Derived The CRTP class operated on.
    */
    template<typename Derived>
    int findSimilarityInBuffer(const DenseBase<Derived> &buffer);

    /** Method to find the similarity between an output vector and the nextOutput.
    \param outputIn The vector to compare against the reference
    \tparam Derived The CRTP class operated on.
    */
    template<typename Derived>
    FP_TYPE findSimilarity(const ArrayBase<Derived> &outputIn) {
        simComp=outputIn;
        simComp*=wnd;
        return rms(nextOutput-simComp);
    }

    /** Method to return the RMS power of the input vector/matrix
    \param s The matrix or vector to find the 2 Norn of
    \return The RMS (2 Norm) of s
    \tparam Derived The CRTP class operated on.
    */
    template<typename Derived>
    FP_TYPE rms(const ArrayBase<Derived> &s) {
        //r=sqrt(mean(s.^2));
//        cout<<"norm"<<endl;
//        cout<<s.matrix().norm()<<endl;
        return s.matrix().norm();
    }

    /// Generate the overlap add window based on the window size N
    void OLAWnd(void);

    int inputSamplesRequired; ///< The number of audio samples required for the next call to process

    /** Initialise the system.
    */
    void init(void);
public:

    Array<FP_TYPE, Dynamic, Dynamic> output; ///< The output vector, each row is a channel
    Array<FP_TYPE, Dynamic, Dynamic> input; ///< The input vector, each row is a channel

    /** Constructor, initialises the window size and buffer.
    Uses a default channel count.
    */
    WSOLA();

    /** Constructor, initialises the window size and buffer.
    int chCnt The number of channels to use.
    */
    WSOLA(int chCnt);

    virtual ~WSOLA(); ///< Destructor

#ifdef HAVE_EMSCRIPTEN
    int processOurInput(FP_TYPE timeScale) {
        return process(timeScale, input);
    }
#endif

    /** Process the input vector and indicate how many samples are required to reload the input vector.
    \param timeScale The scaling factor for the time, <1 is slower, >1 is faster
    \param input The input to load into the buffer for future use.
    \return the number of samples required
    \tparam Derived The CRTP class operated on.
    */
    template<typename Derived>
    int process(FP_TYPE timeScale, const DenseBase<Derived> &input) {
//        cout<<"input : "<<endl;
//        cout<<input<<endl;
//        cout<<"buffer pre shift : "<<endl;
//        cout<<buffer<<endl;
        // shift the buffer on
        int chCnt=buffer.rows();
        buffer.block(0,0,chCnt,buffer.cols()-inputSamplesRequired)=buffer.block(0,inputSamplesRequired,chCnt,buffer.cols()-inputSamplesRequired);
//        cout<<"buffer post shift : "<<endl;
//        cout<<buffer<<endl;
        // load the required input samples into the buffer
        buffer.block(0,buffer.cols()-inputSamplesRequired,chCnt,inputSamplesRequired)=input.block(0,0,chCnt,inputSamplesRequired);
//        cout<<"buffer with input : "<<endl;
//        cout<<buffer<<endl;

        processInner(); // do the inner processing

        // shift on by the ratio of N/2
//        cout<<"rem "<<rem<<endl;
        double shiftOn=timeScale*NO2+rem;
//        cout<<"shiftOn "<<shiftOn<<endl;
        inputSamplesRequired=static_cast<int>(round(shiftOn));
//        cout<<"inputSamplesRequired "<<inputSamplesRequired<<endl;
        rem=shiftOn-(double)inputSamplesRequired;
//        cout<<"rem "<<rem<<endl;
        return inputSamplesRequired;
    }

    /** The inner process.
    Find the most similar block to the desired and overlap add.
    */
    void processInner(void);

//    template<typename Derived>
//    int loadBuffer(, const DenseBase<Derived> &input);

    /** Find out what the largest possible request of input samples
    */
    int getMaxInputSamplesRequired(void) {
        return NO2*(M+2);
    }

    /** Get the number of input samples WSOLA requires as input to process.
    \return The number of required samples
    */
    int getSamplesRequired(void) {
        return inputSamplesRequired;
    }

    /** Get the number of samples returned for each process.
    */
    int getOutputSize(void){
        return NO2;
    }

    /** Reset the system to start fresh.
    \param chCnt The number of channels to use.
    */
    void reset(int chCnt);

    /** Indicate that there is no more input audio to process,
    begin rolling out the last of the buffer.
    \return <= 0 to stop, any other number to continue.
    */
    int noMoreAudio(){
        return inputSamplesRequired-=NO2;
    }

    /** Load the WSOLA.input Array - for use with languages where Eigen types aren't exposed.
    Implements WSOLA.input(n,m)=val;
    \param n The row to load
    \param m The column to load
    \param val The value to load into the input Array
    \return <0 on error.
    */
    int loadInput(int n, int m, FP_TYPE val);

    /** Unload the WSOLA.output Array - for use with languages where Eigen types aren't exposed.
    The value is returned.
    \param n The row to unload
    \param m The column to unload
    \return <0. on error or the value of WSOLA.output(n,m).
    */
    FP_TYPE unloadOutput(int n, int m);

    /** Set the sample rate in Hz
    \param fsIn The new sample rate in Hz
    */
    void setFS(float fsIn);

};

#endif // WSOLA_H_
