/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
#include "DSP/Decomposition.H"

#include <Eigen/SVD>

#include <fstream>

template<typename TYPE>
Decomposition<TYPE>::Decomposition() : OverlapAdd<TYPE>() {
    // start octave
    vector<string> args(5);
    args[0]=string("--silent");
    args[1]=string("--path");
    args[2]=string(MFILE_PATH1);
    args[3]=string("--path");
    args[4]=string(MFILE_PATH2);
    octave.startOctave(args);
}

template<typename TYPE>
Decomposition<TYPE>::~Decomposition() {
    //dtor
}

template<typename TYPE>
int Decomposition<TYPE>::findSubSpace(void) {
    //if (OverlapAdd<TYPE>::data.cols()==0 || OverlapAdd<TYPE>::data.rows()==0)
    if (!this->getWindowCount() || !this->getWindowSize())
        return DECOMPOSITION_NODATA_ERROR;
    int ret=NO_ERROR;

    // flatten the signal - whiten the noise
    int M=OverlapAdd<TYPE>::getWindowCount(); // find out how many windows to process.
    vector<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > octaveInput(3), octaveOutput; // octave input and output data
    octaveInput[1].resize(masker.getBankCount(), 1); // resize the masker related data
    octaveInput[2].resize(masker.getBankCount(), 1);
    for (int j=0; j<masker.getBankCount(); j++) // the masker central frequencies are constant
        octaveInput[1](j,0)=masker.pfb->cf[j];
    for (int i=0; i<M; i++) {
        masker.excite(OverlapAdd<TYPE>::data.col(i)); // find the simeltaneous masking of the audio

        octaveInput[0]=OverlapAdd<TYPE>::data.col(i); // load in the audio for octave to use

        for (int j=0; j<masker.getBankCount(); j++) // load in the masker's mask in for octave to use.
            octaveInput[2](j,0)=masker.mask[j];

//        octave.runM("findSubSpaceCorrMatrix", octaveInput, octaveOutput);
//        //JacobiSVD<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > svd(octaveOutput[0], ComputeThinU | ComputeThinV);
//        cout<<"corr matrix rows, cols "<<octaveOutput[0].rows()<<", "<<octaveOutput[0].cols()<<endl;
//
//        struct timespec start, stop;
//        if( clock_gettime( CLOCK_REALTIME, &start) == -1 ){
//            cout<<"clock start get time error"<<endl;
//            exit(-1);
//        }
//
//
//        JacobiSVD<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > svd(octaveOutput[0], ComputeThinV);
//        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ){
//            cout<<"clock stop get time error"<<endl;
//            exit(-1);
//        }
//
//        double duration = 1.e3*( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec )/1.e9;
//        cout<<"Duration "<<duration<<" ms\n";
//
//        if( clock_gettime( CLOCK_REALTIME, &start) == -1 ){
//            cout<<"clock start get time error"<<endl;
//            exit(-1);
//        }
        octave.runM("findSubSpace", octaveInput, octaveOutput); // run the find subspace m file and return information about subspace frequency weighting.
//        if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ){
//            cout<<"clock stop get time error"<<endl;
//            exit(-1);
//        }

//        duration = 1.e3*( stop.tv_sec - start.tv_sec ) + (double)( stop.tv_nsec - start.tv_nsec )/1.e9;
//        cout<<"Duration "<<duration<<" ms\n";
//        //cout<<"singular values Eigen"<<endl;
//        ofstream tempOut("/tmp/comparison.txt");
//        tempOut<<svd.singularValues().transpose().array().square()<<endl;
//        //cout<<svd.singularValues()<<endl;
//        //cout<<"singular values octave"<<endl;
//        tempOut<<octaveOutput[0].transpose()<<endl;
//        //cout<<octaveOutput[0]<<endl;

        break;
    }

    return ret;
}

template class Decomposition<float>;
template class Decomposition<double>;
