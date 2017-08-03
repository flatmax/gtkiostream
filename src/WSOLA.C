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
#include "WSOLA.H"

#include <stdlib.h>

WSOLA::WSOLA() {
    fs=FS_DEFAULT; // set the sample rate to default
    init();
    reset(DEFAULT_CH_CNT);
}

WSOLA::WSOLA(int chCnt){
    fs=FS_DEFAULT; // set the sample rate to default
    init();
    reset(chCnt);
}

WSOLA::~WSOLA() {
}

void WSOLA::init(void){
    N=(int)(2.*floor((fs*TAU)/2.)); // audio samples in one window - an even number
    NO2=N/2; // half the window size
    M=M_DEFAULT; // vectors of NO2 audio samples
    if ((M*N)%2)
        exit(WSOLADebug().evaluateError(WSOLA_MOD2_ERROR));
}

void WSOLA::OLAWnd(void) {
    int chCnt=buffer.rows();
    wnd.resize(chCnt,N);
    wnd.row(0)=Array<FP_TYPE, 1, Dynamic>::LinSpaced(N,0.,M_PI-M_PI/N).sin().square();
    for (int i=1; i<chCnt; i++)
        wnd.row(i)=wnd.row(0);
//    cout<<"wnd "<<endl;
//    cout<<wnd<<endl;
//    cout<<"wnd overlap check"<<endl;
//    cout<<wnd.block(0,NO2,1,NO2)+wnd.block(0,0,1,NO2)<<endl;
}

template<typename Derived>
int WSOLA::findSimilarityInBuffer(const DenseBase<Derived> &buffer) {
//    cout<<nextOutput<<endl;
//    cout<<buffer.block(0,0,1,N)*wnd<<endl;
//    cout<<"similarity "<<nextOutput-buffer.block(0,0,1,N)*wnd<<endl;

    FP_TYPE bestMeasure=3e8;
    int bestI=0;
//    for (int i=0; i<M; i++) {
//        FP_TYPE measureTest=findSimilarity(buffer.block(0,i*NO2,1,N));
    for (int i=0; i<(M-1)*NO2; i++) {
        FP_TYPE measureTest=findSimilarity(buffer.block(0,i,buffer.rows(),N));
//        cout<<"measureTest="<<measureTest<<" bestMeasure "<<bestMeasure<<endl;
        if (measureTest<bestMeasure) {
            bestMeasure=measureTest;
            bestI=i;
        }
//        cout<<"measureTest="<<measureTest<<" bestMeasure "<<bestMeasure<<endl;
    }
//    cout<<"bestI "<<bestI<<endl;
//    cout<<"similarity "<<nextOutput-buffer.block(0,bestI*NO2,1,N)*wnd<<endl;

    return bestI;
}

//template<typename Derived>
//int WSOLA::findSimilarityInBufferDFT(const DenseBase<Derived> &buffer) {
//    Array<FFT<FP_TYPE>::Complex, Dynamic, 1> WND(buffer.cols(),1);
//    Array<FFT<FP_TYPE>::Complex, Dynamic, 1> wndPadded(buffer.cols(),1);
//    wndPadded.block(
//                    wnd.rows(),1,wnd.cols(),1)
//    fft.fwd(chirp.col(0).data(), WND.col(0).data(),M);
//
//    FP_TYPE bestMeasure=3e8;
//    int bestI=0;
//    for (int i=0; i<(M-1)*NO2; i++) {
//        FP_TYPE measureTest=findSimilarity(buffer.block(0,i,1,N));
//        if (measureTest<bestMeasure) {
//            bestMeasure=measureTest;
//            bestI=i;
//        }
//    }
//
//    return bestI;
//}

void WSOLA::processInner(void) {
    int chCnt=buffer.rows();
    if (output.cols()!=0) { // not the first run
        output.block(0,0,chCnt,NO2)=output.block(0,NO2,chCnt,NO2); // shift the output NO2 on
        m=findSimilarityInBuffer(buffer); // find the most similar index in the buffer
//        m=findSimilarityInBufferDFT(buffer); // find the most similar index in the buffer
//        cout<<"most similar m="<<m<<endl;
    } else { // this is the first run ... need to inverse window the first half block and pad with zeros
        output=buffer.block(0,m*NO2,chCnt,N); // the output = the first buffer
//            cout<<"wnd \n"<<wnd<<endl;
//            cout<<"output \n"<<output<<endl;
        output.block(0,0,chCnt,NO2)*=wnd.block(0,NO2,chCnt,NO2); // the first half of the output is inverse windowed
//            cout<<"output \n"<<output<<endl;
//            cout<<"output \n"<<output<<endl;
    }
    output.block(0,NO2,chCnt,NO2)=Matrix<FP_TYPE, Dynamic, Dynamic>::Zero(chCnt,NO2); // the second half is zero padded

    // We are now pointing at the most similar vector ...
    // use nextOutput as a temporary buffer to overlap add the output
    //nextOutput=buffer.block(0,m*NO2,1,N);
    nextOutput=buffer.block(0,m,chCnt,N);
    nextOutput*=wnd;
    output+=nextOutput; // overlap add
//        cout<<"output \n"<<output<<endl;

    // Next output is meant to be the next row
//    nextOutput=buffer.block(0,(m+1)*NO2,1,N);
    nextOutput=buffer.block(0,m+NO2,chCnt,N);
//        cout<<"nextOutput \n"<<nextOutput<<endl;
    nextOutput*=wnd; // window the next block to match against
//        cout<<"nextOutput \n"<<nextOutput<<endl;
}

void WSOLA::reset(int chCnt){
    inputSamplesRequired=getMaxInputSamplesRequired();
    buffer=Matrix<FP_TYPE, Dynamic, Dynamic>::Zero(chCnt,inputSamplesRequired);
    m=0;
    rem=0.;
    output.resize(0,0); // this indicates to the inner algporithm that this will be the first run.
    OLAWnd(); // prepare the window
    input.resize(chCnt, inputSamplesRequired);
}

int WSOLA::loadInput(int n, int m, FP_TYPE val){
    if (n>input.rows()-1 || n<0)
        return WSOLADebug().evaluateError(WSOLA_ROWS_ERROR);
    if (m>input.cols()-1 || m<0)
        return WSOLADebug().evaluateError(WSOLA_COLS_ERROR);
    input(n,m)=val;
    return 0;
}

FP_TYPE WSOLA::unloadOutput(int n, int m){
    if (n>output.rows()-1 || n<0)
        return (FP_TYPE)WSOLADebug().evaluateError(WSOLA_ROWS_ERROR);
    if (m>output.cols()-1 || m<0)
        return (FP_TYPE)WSOLADebug().evaluateError(WSOLA_COLS_ERROR);
    return output(n,m);
}

void WSOLA::setFS(float fsIn){
    fs=fsIn;
    init();
    reset(buffer.rows());
}


#ifdef HAVE_EMSCRIPTEN
EMSCRIPTEN_BINDINGS(WSOLA_ex) {
  emscripten::class_<WSOLA>("WSOLA")
    .constructor()
    .constructor<int>()
    .function("loadInput", &WSOLA::loadInput)
    .function("unloadOutput", &WSOLA::unloadOutput)
    .function("getSamplesRequired", &WSOLA::getSamplesRequired)
    .function("process", &WSOLA::processOurInput)
    .function("getOutputSize", &WSOLA::getOutputSize)
    .function("setFS", &WSOLA::setFS)
//    .function("getFS", &EQ::getFS)
//    .property("x", &MyClass::getX, &MyClass::setX)
//    .class_function("getStringFromInstance", &MyClass::getStringFromInstance)
    ;
}
#endif
