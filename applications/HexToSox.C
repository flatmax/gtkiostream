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

/*
Author: Matt Flax <flatmax@flatmax.org>
Date: 2016.12.27
*/

#include <iostream>
#include "OptionParser.H"
#include "Sox.H"
#include <unistd.h>
#include <stdint.h>
using namespace std;

using namespace std;

int printUsage(string name) {
    name=name.substr(name.find_last_of("\\/")+1, name.size());
    cout<<"\nWrite hexValues to audio file"<<endl;
    cout<<"\nUseage: \n"<<endl;
    cout<<name<<" [-o num] [-u num] [-d num] [-r num] [-b bits] hexValues outputFileName.ext : the output file name with ext replaced by a known output format extension (see below)"<<endl;
    cout<<name<<" -o num : number of channels to output [2]"<<endl;
    cout<<name<<" -u : don't use signed, use unsigned"<<endl;
    cout<<name<<" -d num : duration in seconds [1]"<<endl;
    cout<<name<<" -r num : sample rate in Hz [48000]"<<endl;
    cout<<name<<" -b num : bits [32]"<<endl;
    cout<<name<<" -s : switch the endian for the audio file"<<endl;

    Sox<int32_t> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;

    return 0;
}

/*int saveAudioToFile(const char *fn, const CrossoverAudio& crossAudio, const int chCnt) {
    int ret=sox.openWrite(fn, crossAudio.getSampleRate(), chCnt, 1.0);
    if (ret!=NO_ERROR)
        return SoxDebug().evaluateError(ret);
    int written=sox.write(crossAudio.audio);
    if (written!=crossAudio.audio.rows()*crossAudio.audio.cols()) {
        cout<<SoxDebug().evaluateError(written)<<endl;
        cout<<"written "<<written<<endl;
        cout<<"Output matrix size (rows, cols) = ("<<crossAudio.audio.rows()<<", "<<crossAudio.audio.cols()<<")"<<endl;
        cout<<"Matrix is a total of "<<crossAudio.audio.rows()*crossAudio.audio.cols()<<" samples"<<endl;
        cout<<"Error writing, exiting."<<endl;
    }
    sox.closeWrite();
    return ret;
}*/

int main(int argc, char *argv[]) {
    OptionParser op;

    ostringstream msg; msg<<"Format : ";

    int i=0, dummy;
    string help;
    if (op.getArg<string>("h", argc, argv, help, i=0)!=0 || argc<3)
        return printUsage(argv[0]);

    bool signd=true;
    dummy=op.getArg<int>("u", argc, argv, dummy, i=0);
    if (dummy!=0){
        signd=false;
        msg<<" unsigned ";
    } else
        msg<<" signed ";

    int outChCnt=1;
    op.getArg<int>("o", argc, argv, outChCnt, i=0);
    msg<<outChCnt<<" ch ";

    int bits=32;
    op.getArg<int>("b", argc, argv, bits, i=0);
    msg<<bits<<" bit ";

    float fs=48000.;
    op.getArg<float>("r", argc, argv, fs, i=0);
    msg<<fs<<" Hz \n";

    int N=strlen(argv[argc-2]);
    msg<<"Repeating : "<<argv[argc-2]<<" ("<<N<<" nibbles)";

    float duration=1.0;
    op.getArg<float>("d", argc, argv, duration, i=0);
    msg<<" for "<<duration<<" s";

    int byteCnt=ceil(fs*duration/N)*N;
    byteCnt=ceil(byteCnt/sizeof(sox_sample_t))*sizeof(sox_sample_t);

    cout<<msg.str()<<endl;

/*    // break the type info up into parameters.
    bool isLittleEndian=false;
    std::string token(10,'\0'); // check the endian-ness
    std::stringstream typeInfo;
    typeInfo.getline((char*)token.c_str(), 10, ':');
    if (token.find("le")!=std::string::npos)
        isLittleEndian=true;
*/
    bool switchEndian=false;
    dummy=op.getArg<int>("s", argc, argv, dummy, i=0);
    if (dummy!=0){
        switchEndian=true;
        cout<<"switching endian in the output file "<<endl;
    } else
        cout<<"not switching endian in the output file "<<endl;

    stringstream hexString;
    for (int i=0; i*N<byteCnt*2; i++)
        hexString<<argv[argc-2];

    hexString.seekg(0, ios::end); // find the number of nibbles in the stream
    int size = hexString.tellg();
    hexString.seekg(0, ios::beg);

    ostringstream charString;
    unsigned char charLSB, charMSB, charTogether;
    char chars[2];
    for (int i=0; i<size/2; i++){
        hexString>>charMSB;
        hexString>>charLSB;
//        cout<<"0x"<<charMSB<<charLSB<<"  ";
        if (charMSB<87) charMSB+=87;
        if (charLSB<87) charLSB+=87;
        charTogether=(((charMSB-87)<<4)&0xf0)|((charLSB-87)&0xf);
  //      cout<<(int)charTogether<<'\n';
        charString<<charTogether;
    }

//    cout<<charString.str().c_str()<<endl;
    Eigen::Matrix<sox_sample_t, Eigen::Dynamic, Eigen::Dynamic> audio(byteCnt/sizeof(sox_sample_t),outChCnt); ///< The first channel is the same data sent over each output channel, then the output channels, then the input channels
    for (int i=0; i<outChCnt; i++)
        memcpy(audio.col(i).data(), charString.str().c_str(), byteCnt);

    // open sox
    Sox<sox_sample_t> sox;
    int revBytes=0, revNibbles=0, revBits=0;
    sox.openWrite(argv[argc-1], fs, outChCnt, (double)numeric_limits<sox_sample_t>::max(), bits, switchEndian, revBytes, revNibbles, revBits);
    sox.write(audio);
    sox.closeWrite();
    return 0;
}
