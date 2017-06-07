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
#ifndef _MSC_VER
#include "Sox.H"
#else
// Note : Microsoft doesn't understand the different between upper and lower case in file names.
// on microsoft, you have to manually rename Sox.H to SoxWindows.H
#include "SoxWindows.H"
#endif

typedef float FP_TYPE;

#include "OptionParser.H"
#include "AudioMask/AudioMasker.H"

#define DEFAULT_FBANK_CNT 50 ///< The default number of auditory filters.

#include <fstream>

void printUsage(const char *str){
    cerr<<"Usage: "<<str<<" -h or --help"<<endl;
    cerr<<"Usage: "<<str<<" fileName.wav windowSize"<<endl;
    cerr<<"\t the fileName can be any readable audio file format."<<endl;
    cerr<<"\t the windowSize specifies how many samples to use as a window."<<endl;
    cerr<<"\n Outputs are in three files (where applicable, interleaved row per channel) :"<<endl;
    cerr<<"\t The audio windows are in the file fileName.wav.audio.dat"<<endl;
    cerr<<"\t The masking threshold is in the file fileName.wav.mask.dat"<<endl;
    cerr<<"\t The frquency indexes for the masking thresholds are in the file fileName.wav.f.dat"<<endl;
    cerr<<"\n\t In the audio and mask, the channels are interleaved as rows, M rows per window, where M is the channel count."<<endl;
    cerr<<"\n Author : Matt Flax <flatmax@flatmax.org>"<<endl;
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

    FP_TYPE windowSize;
    op.convertArg<FP_TYPE>(argv[argc-1], windowSize);
    cout<<"using windowSize = "<<windowSize<<" s"<<endl;

    string fileName(argv[argc-2]);
    cout<<"input file = "<<fileName<<endl;
    Sox<FP_TYPE> sox;
    int ret;
    if ((ret=sox.openRead(fileName))<0  && ret!=SOX_READ_MAXSCALE_ERROR)
        return SoxDebug().evaluateError(ret, argv[argc-2]);
    sox.setMaxVal(1.0);

    int chCnt=sox.getChCntIn(); // the channel count

    Matrix<FP_TYPE, Dynamic, Dynamic> audioData;
    ret=sox.read(audioData, windowSize);
    if (audioData.rows()!=windowSize){
        cerr<<"couldn't read audio, wanted "<<windowSize<<" samples got "<<audioData.rows()<<endl;
        return -1;
    }
    if (audioData.cols()!=chCnt){
        cerr<<"couldn't read audio, wanted "<<windowSize<<" channels got "<<audioData.cols()<<endl;
        return -1;
    }

    double fs=sox.getFSIn();
    AudioMasker masker(fs, DEFAULT_FBANK_CNT);

    string fileNameOut;
    cout<<"output file = "<<(fileName+'.'+argv[argc-1]+".mask.dat")<<endl;
    ofstream maskOut((fileName+'.'+argv[argc-1]+".mask.dat").c_str());
    if (!maskOut){
        cerr<<"Couldn't open the output file "<<(fileName+'.'+argv[argc-1]+".mask.dat")<<endl;
        exit(-1);
    }
    maskOut<<scientific; // put output into sci mode

    ofstream audioOut((fileName+'.'+argv[argc-1]+".audio.dat").c_str());
    if (!audioOut){
        cerr<<"Couldn't open the output file "<<(fileName+'.'+argv[argc-1]+".audio.dat")<<endl;
        exit(-1);
    }
    audioOut<<scientific; // put output into sci mode

    ofstream fOut((fileName+'.'+argv[argc-1]+".f.dat").c_str());
    if (!audioOut){
        cerr<<"Couldn't open the output file "<<(fileName+'.'+argv[argc-1]+".f.dat")<<endl;
        exit(-1);
    }
    fOut<<scientific; // put output into sci mode
    // output the frequencies as the first row
    int halfSampleCount=audioData.rows()/2+1;
    double fact=fs/((double)windowSize-1.0); // convert from an index to the equivalent Fourier bin frequency
    for (int i=0; i<DEFAULT_FBANK_CNT; i++)
        fOut<<masker.pfb->cf[i]<<'\t';
//    for (int i=0; i<halfSampleCount; i++)
//        maskOut<<i*fact<<'\t';
    fOut<<endl;
    fOut.close();

    ret=NO_ERROR;
    while (ret==NO_ERROR){
        // process each channel of the audio data
        for (int i=0; i<chCnt; i++){
//            masker.excite<FP_TYPE>(audioData.block(0, i, audioData.rows(), 1).data(), windowSize);
            int ret=masker.excite(audioData.block(0, i, audioData.rows(), 1));
            if (ret!=NO_ERROR)
                AudioMaskerDebug().evaluateError(ret);
//            for (int j=0; j<halfSampleCount; j++)
//                maskOut<<masker.findThreshold(j*fact)<<'\t';
            for (int j=0; j<DEFAULT_FBANK_CNT; j++)
                maskOut<<masker.mask[j]<<'\t';
            maskOut<<endl;
            for (int j=0; j<windowSize; j++)
                audioOut<<audioData(j,i)<<'\t';
            audioOut<<endl;
        }

        // read more audio data
        ret=sox.read(audioData, windowSize);
        if (ret!=NO_ERROR)
            break;
        if (audioData.rows()!=windowSize){
            cout<<"Couldn't read audio, wanted "<<windowSize<<" samples got "<<audioData.rows()<<endl;
            cout<<"This must be the end of the file"<<endl;
            break;
        }
    }

    audioOut.close();
    maskOut.close();
    sox.closeRead();

    cout<<"Please run the following .m file to see the output "<<endl;
    cout<<"function view"<<endl;
    cout<<"fs="<<fs<<";"<<endl;
    cout<<"load /tmp/11.Neutral.44k.wav."<<windowSize<<".audio.dat"<<endl;
    cout<<"load /tmp/11.Neutral.44k.wav."<<windowSize<<".f.dat"<<endl;
    cout<<"load /tmp/11.Neutral.44k.wav."<<windowSize<<".mask.dat"<<endl;
    cout<<"[M,Nm]=size(X11_Neutral_44k_wav_"<<windowSize<<"_mask);"<<endl;
    cout<<"[M,N]=size(X11_Neutral_44k_wav_"<<windowSize<<"_audio);"<<endl;
    cout<<"f=linspace(0,fs,N);"<<endl;
    cout<<"for i=1:M"<<endl;
    cout<<"    loglog(f, abs(fft(X11_Neutral_44k_wav_"<<windowSize<<"_audio(i,:)))); hold on"<<endl;
    cout<<"    loglog(X11_Neutral_44k_wav_"<<windowSize<<"_f, X11_Neutral_44k_wav_"<<windowSize<<"_mask(i,:), 'r'); hold off"<<endl;
    cout<<"    legend('audio','mask'); xlabel('f (Hz)'); ylabel('amplitude');"<<endl;
    cout<<"    pause"<<endl;
    cout<<"end"<<endl;
    return ret;
}
