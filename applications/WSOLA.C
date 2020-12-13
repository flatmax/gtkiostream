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
#include "WSOLA.H"
#include "OptionParser.H"

#ifndef _MSC_VER
#include "Sox.H"
#else
// Note : Microsoft doesn't understand the different between upper and lower case in file names.
// on microsoft, you have to manually rename Sox.H to SoxWindows.H
#include "SoxWindows.H"
#endif

#include <iostream>

template<typename Derived>
int readAudio(Sox<FP_TYPE> &sox, DenseBase<Derived> const &audioData, int sampleCount){
    Matrix<FP_TYPE, Dynamic, Dynamic> output;
    int ret=sox.read(output, sampleCount);
    output.transposeInPlace();
    const_cast< DenseBase<Derived>& >(audioData)=output;
//    cout<<output<<endl;
    return output.cols();
}

void printUsage(const char *str){
    cerr<<"Usage: "<<str<<" -h or --help"<<endl;
    cerr<<"Usage: "<<str<<" fileName.wav rate"<<endl;
    cerr<<"\t the fileName can be any readable audio file format."<<endl;
    cerr<<"\t the rate can be any number within a reasonable range where 0 < rate < 5 or some reasonable speed."<<endl;
    cerr<<"\n Outputs to the file fileName.wav.rate.wav"<<endl;
    cerr<<"\n Author : Matt Flax <flatmax@>"<<endl;
    exit(0);
}

int main(int argc, char *argv[]){
    if (argc<3)
        printUsage(argv[0]);

    OptionParser op;
    int i=0;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0)
        printUsage(argv[0]);
    if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
        printUsage(argv[0]);

    FP_TYPE timeScale;
    op.convertArg<FP_TYPE>(argv[argc-1], timeScale);
    cout<<"using timescale = "<<timeScale<<endl;

    string fileName(argv[argc-2]);
    cout<<"input file = "<<fileName<<endl;
    Sox<FP_TYPE> sox;
    int ret;
    if ((ret=sox.openRead(fileName))<0  && ret!=SOX_READ_MAXSCALE_ERROR)
        return WSOLADebug().evaluateError(ret, argv[argc-2]);
    sox.setMaxVal(1.0);

    int chCnt=sox.getChCntIn(); // the channel count

    string fileNameOut;
    fileNameOut=fileName+'.'+argv[argc-1]+".wav";
    cout<<"output file = "<<fileNameOut<<endl;
    if ((ret=sox.openWrite(fileNameOut, sox.getFSIn(), chCnt, 1.))<0)
        return WSOLADebug().evaluateError(ret, fileNameOut);

    WSOLA wsola(chCnt);
    wsola.setFS(sox.getFSIn());
    Matrix<FP_TYPE, Dynamic, Dynamic> audioData;
    int N=wsola.getSamplesRequired();
    ret=readAudio(sox, audioData, N);
    if (ret!=N)
        cerr<<"couldn't read audio, wanted "<<N<<" got "<<ret<<endl;

    ret=NO_ERROR;
    while (ret==NO_ERROR){
        // process the audio data
        N=wsola.process(timeScale, audioData);

        // write to file
        ret=sox.writeTransposed(wsola.output.block(0,0,chCnt,wsola.getOutputSize()));
        if (ret!=wsola.output.cols())
            cerr<<"couldn't write audio, tried to write "<<N<<" wrote "<<ret<<endl;

        // read more audio data
        ret=readAudio(sox, audioData, N);
        if (ret!=N)
            cerr<<"couldn't read audio, wanted "<<N<<" got "<<ret<<endl;
        else
            ret=NO_ERROR;
    }

    sox.closeWrite();
    sox.closeRead();

    return 0;
}

