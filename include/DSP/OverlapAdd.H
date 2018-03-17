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
#ifndef OVERLAPADD_H_
#define OVERLAPADD_H_

#ifndef _MSC_VER
#include "Sox.H"
#else
// Note : Microsoft doesn't understand the different between upper and lower case in file names.
// on microsoft, you have to manually rename Sox.H to SoxWindows.H
#include "SoxWindows.H"
#endif

#define OVERLAP_DEFAULT 0.5
#define WINDOWSIZE_DEFAULT 2048

#include "Debug.H"
#define OVERLAPADD_CHCNT_ERROR OVERLAPADD_ERROR_OFFSET-1 ///< Error when the specified channel is larger then the number of channels in the input audio file.
#define OVERLAPADD_FILESIZE_MISMATCH_ERROR OVERLAPADD_ERROR_OFFSET-2 ///< Error when the number of audio samples required can't be read from the input audio file.
#define OVERLAPADD_FACTOR_TOO_LARGE_ERROR OVERLAPADD_ERROR_OFFSET-3 ///< Error when the overlap factor is too large.

/** Debug class for OverlapAdd.
As this class uses Sox, it has to know about sox errors.
*/
class OverlapAddDebug : public SoxDebug {
public:
    /** Constructor defining all debug strings which match the debug defined variables
    */
    OverlapAddDebug() {
#ifndef NDEBUG
        errors[OVERLAPADD_CHCNT_ERROR]=string("OverlapAdd: The requested channel is larger then the number available in the input audio file. If in the last window, this error will not be thrown. ");
        errors[OVERLAPADD_FILESIZE_MISMATCH_ERROR]=string("OverlapAdd: The requested number of audio samples can't be read from the input audio file. ");
#endif
    }
};

/** Generates an overlap matrix for an arbitrary 1D waveform.

Allows specification of the overlap factor <1.

The audio is read in from any audio file supported by Sox. The loadData method will read in the number of samples specified and
pad out the rest of the window with extra samples. The actual number of samples read in may be windowSize*getOverlapFactor() larger then requested.
\tparam TYPE Specifies the type of the data held in the matrix, e.g. float, double
*/
template<class TYPE>
class OverlapAdd {
    float overlapFactor; ///< Overlap factor, 0.5 for half

    /** Initialise this class, specifying an overlap factor.
    \param factor the factor to overlap by.
    */
    void init(float factor) {
        if (factor >= 1.) {
          fprintf(stderr, "OverlapAdd::init The specified factor %f is >= 1. This is not allowed : ERROR : exiting.",factor);
            exit(OVERLAPADD_FACTOR_TOO_LARGE_ERROR);
        }
        overlapFactor=factor;
        data.resize(WINDOWSIZE_DEFAULT,2); // ensure that the default window size is reasonable
    }

protected:
    /** The overlapping data is held in this matrix.
    \tparam TYPE The Matrix types to input and return
    */
    Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> data;

public:

    /// Empty constructor defaults to OVERLAP_DEFAULT
    OverlapAdd(void) {
        init(OVERLAP_DEFAULT);
    }

    /** Constructor specifying an overlap factor.
    \param factor the factor to overlap by.
    */
    OverlapAdd(float factor) {
        init(factor);
    }

    /// Destructor
    virtual ~OverlapAdd() {
    }
//
//    template<class TYPE>
//    void loadData(const Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &dataIn, uint windowSize){
//        uint windowCnt=(int)floor((float)dataIn.rows()/(float)windowSize); // find the number of full segments
//        uint rem=dataIn.rows()-windowCnt*windowSize; // find the remainder
//        uint totalWindowCnt=windowCnt+((rem)?1:0);
//        if (data.rows()!=windowSize || data.columns()!=totalWindowCnt)
//            data.resize(windowSize,totalWindowCnt);
//        data.block(0,0,windowSize, windowCnt)=dataIn.block(0,0,windowCnt*windowSize,1).reshape(windowSize,windowCnt);
//        if (totalWindowCnt!=windowCnt){
//            data.block(0,totalWindowCnt-1,windowSize, 1)=Eigen::Matrix<TYPE, Eigen::Dynamic, 1>::Zeros(windowSize,1);
//            data.block(0,totalWindowCnt-1,rem, 1)=dataIn.block(windowSize*windowCnt, 0, rem, 1);
//        }
//    }

    /** load data into the data matrix, with each column containing windowSize samples and overlapping by windowSize*getOverlapFactor() samples.
    Note : this method will try to read an extra windowSize*getOverlapFactor() samples to fill the last window.
    \param sox An open sox audiofile, positioned to the point to start reading from.
    \param windowSize The size of the audio window including the overlapped region
    \param sampleCount The total number of samples to read from the input file.
    \param whichCh Which channel to read from the input audio file.
    \return The number of samples not read (when trying to read the last window) on success and the private data matrix is populated, the apropriate error otherwise.
    */
    int loadData(Sox<float> &sox, uint windowSize, uint sampleCount, int whichCh=0) {
        int ret=NO_ERROR;
        if ((ret=sox.getChCntIn())<0) // check whether the file is opened
            return ret;
        if (whichCh+1>sox.getChCntIn())
            return OVERLAPADD_CHCNT_ERROR;

        uint N=(uint)floor((float)windowSize*overlapFactor); // the number of samples in the overlap region
        uint M=windowSize-N; // the number of samples to load in each time the audio file is read

//        cout<<"N="<<N<<endl;
//        cout<<"M="<<M<<endl;

        //cout<<15.-(float)sampleCount/(float)windowSize/(1.-overlapFactor)<<endl;
        uint windowCnt=(int)(ceil((float)((float)sampleCount/(float)windowSize/(1.-overlapFactor)))); // find the number of full segments
        // printf("sampleCount=%d\n",sampleCount);
        // printf("windowCnt=%d \t %f\n",windowCnt,15.-windowCnt);
        // printf("windowSize=%d\n",windowSize);

        if (data.cols()!=windowSize || data.rows()!=windowCnt)
            data.resize(windowSize, windowCnt);
        data=Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>::Zero(windowSize, windowCnt);

        ret=NO_ERROR;
        Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> audioData;
        for (int i=0; i<windowCnt; i++) {
            int toRead=M;
            (i==0) ? toRead=windowSize : toRead=M;
            if (i==windowCnt-1) toRead+=N; // if this is the last window, then try to read an extra overlap region
            if (toRead>sampleCount+N)
                toRead=sampleCount+N; // make sure not to read too many samples
            //cout<<"toRead "<<toRead<<'\t';
            if (toRead>0) {
                int cnt=sox.read(audioData, toRead);
                //cout<<"read "<<audioData.rows()<<endl;
                if (cnt<0 && cnt!=SOX_EOF_OR_ERROR)
                    return SoxDebug().evaluateError(cnt);
                if (audioData.rows()!=toRead) {
                    ret=windowCnt-toRead;
                    if (i!=windowCnt-1)
                        ret=OVERLAPADD_FILESIZE_MISMATCH_ERROR;
                }
            } else
                audioData=Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic>::Zero(M, whichCh+1);
            //cout<<audioData.transpose()<<endl;
            sampleCount-=audioData.rows(); // keep track of the number read
            if (i==0) // The first window contains a full window of data
                data.block(0, i, audioData.rows(), 1)=audioData.block(0, whichCh, audioData.rows(), 1); // load in the new samples to the end of the window
            else
                data.block(N, i, audioData.rows(), 1)=audioData.block(0, whichCh, audioData.rows(), 1); // load in the new samples to the end of the window
            if (i<windowCnt-1) // copy the overlap to the beginning of the next window - except the last window, in which case, don't copy as there are no more subsequent windows.
                data.block(0, i+1, N, 1)=data.block(windowSize-N, i, N, 1);
        }

        //cout<<data.transpose()<<endl;
        return ret;
    }

    /** unload data from the data matrix to the Sox output file, with each column is windowed using windowSize*getOverlapFactor() samples.
    \param sox An open sox audiofile, positioned to the point to start writing to.
    */
    int unloadData(Sox<float> &sox) {
        int ret=NO_ERROR;
        if ((ret=sox.getChCntOut())<0) // check whether the file is opened
            return ret;
        uint windowSize=data.rows();
        uint N=(uint)floor((float)windowSize*overlapFactor); // the number of samples in the overlap region
        uint M=windowSize-N; // the number of samples to write out each time the audio file is written to.

//        cout<<"N="<<N<<endl;
//        cout<<"M="<<M<<endl;

        uint windowCnt=data.cols(); // find the number of output segments
//        cout<<"sampleCount="<<sampleCount<<endl;
//        cout<<"windowCnt="<<windowCnt<<endl;
//        cout<<"windowSize="<<windowSize<<endl;

        ret=NO_ERROR;
        Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> audioData(M,1); // the audio data is written out N+M samples at a time
        // create the windowing data using wndData as a temporary buffer.
        Eigen::Array<TYPE, Eigen::Dynamic, Eigen::Dynamic> wndFront, wndBack, wndData=Eigen::Array<TYPE, 1, Eigen::Dynamic>::LinSpaced(2*N,0.,M_PI-M_PI/(2*N)).sin().square().transpose();
        wndFront=wndData.block(0, 0, N, 1); // ramp up window
        wndBack=wndData.block(N, 0, N, 1); // ramp down window
        //cout<<wndFront<<"\n\n"<<wndBack<<endl;
        for (int i=0; i<windowCnt; i++) {
            int toWrite=M;
            (i==0) ? toWrite=N: toWrite=M;

//            cout<<"wndData = "<<wndData<<endl;
            if (i==0) // The first output
                wndData=data.block(0, i, N, 1); // simply copy the first N samples to the output buffer
            else
                wndData+=data.block(0, i, N, 1).array()*wndFront;
//            cout<<"wndData = "<<wndData<<endl;
//            cout<<endl;
            audioData.block(0, 0, N, 1)=wndData; // copy the windowed data into the first N samples
            if (windowSize-2*N > 0) // if we are underlapping, copy any extra unwindowed data over (NOTE: windowSize-2N = M-N)
                audioData.block(N, 0, M-N, 1)=data.block(N, i, M-N, 1); // copy the

            //cout<<audioData.transpose()<<endl;

            wndData=data.block(windowSize-N, i, N, 1).array()*wndBack;

//                data.block(0, i, audioData.rows(), 1)=audioData.block(0, whichCh, audioData.rows(), 1); // load in the new samples to the end of the window
//            else
//                data.block(N, i, audioData.rows(), 1)=audioData.block(0, whichCh, audioData.rows(), 1); // load in the new samples to the end of the window
//            if (i<windowCnt-1) // copy the overlap to the beginning of the next window - except the last window, in which case, don't copy as there are no more subsequent windows.
//                data.block(0, i+1, N, 1)=data.block(windowSize-N, i, N, 1);

//            cout<<audioData<<endl;

            int cnt=sox.write(audioData);
            //cout<<"written "<<audioData.rows()<<endl;
            if (cnt!=audioData.rows())
                return SoxDebug().evaluateError(cnt);

            if (i==windowCnt-1) { // last window so copy the last block out.
                audioData=data.block(windowSize-N, i, N, 1);
            //cout<<audioData.transpose()<<endl;
                cnt=sox.write(audioData);
                if (cnt!=audioData.rows())
                    return SoxDebug().evaluateError(cnt);
            }
        }

        //cout<<data.transpose()<<endl;
        return ret;
    }

    /** find out by how much the windows are overlapping, 0.5 implies half window overlap.
        \return the overlap factor
    */
    float getOverlapFactor() {
        return overlapFactor;
    }

    /** Find how many samples in each window of data.
    \return the number of samples in each window (block)
    */
    float getWindowSize() {
        return data.rows();
    }

    /** Find how many windows of getWindowSize() samples are in memory.
    \return the number of windows which are in memory.
    */
    float getWindowCount() {
        return data.cols();
    }

    // NOTE : removed due to requirement of <iostream>
    // /* Method to print the contents window by window to stdout.
    // */
    // void dump() {
    //     cout<<data.transpose()<<endl;
    // }

    Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> getDataCopy(){
      return data;
    }

    /** Get the absolute maximum value from the data.
    \return The maximum value held in the data array max(max(abs(data))).
    */
    TYPE getMaxVal(){
        //cout<<"getMaxVal "<<data.cwiseAbs().maxCoeff()<<endl;
        return data.cwiseAbs().maxCoeff();
    }
};
#endif // OVERLAPADD_H_
