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
#include <strstream>

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

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
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

    ofstream viewM("viewLocal.m");
    viewM<<"function viewLocal"<<endl;
    viewM<<"fs="<<fs<<";"<<endl;
    viewM<<"load "<<fileName<<"."<<windowSize<<".audio.dat"<<endl;
    viewM<<"load "<<fileName<<"."<<windowSize<<".f.dat"<<endl;
    viewM<<"load "<<fileName<<"."<<windowSize<<".mask.dat"<<endl;
    string nu = replaceAll(fileName,".","_");
    ostrstream name; name<<nu<<"_"<<windowSize;
    viewM<<"x="<<name.str()<<"_audio';"<<endl;
    viewM<<"xMask="<<name.str()<<"_mask';"<<endl;
    viewM<<"fMask="<<name.str()<<"_f';"<<endl;

    viewM<<"X=fft(x);\n\
[M,Nm]=size(xMask);\n\
[N,M]=size(x);\n\
f=linspace(0,fs,N+1); f(end)=[];\n\
\n\
if 0 % use this to look at block by block output\n\
for m=1:M\n\
    semilogx(f, 20*log10(abs(X(:,m)))); hold on\n\
    semilogx(fMask, 20*log10(xMask(:,m)), 'r'); hold off\n\
    legend('audio','mask'); xlabel('f (Hz)'); ylabel('amplitude');\n\
    pause\n\
end\n\
end\n\
\n\
XMasked=X;\n\
Xi=min(min(abs(XMasked)));\n\
for m=1:M\n\
	thresh=interp1([0; fMask;  fs/2],[0; xMask(:,m); 0],f,'pchip')';\n\
	XMasked(find(abs(X(:,m))<thresh),m)=Xi;\n\
end\n\
\n\
% plot surfaces of the masked and unmasked audio\n\
clf\n\
[xg,yg]=meshgrid(1:M,f(1:N/2));\n\
ax(1)=subplot(211);\n\
surf(xg, yg, 20*log10(abs(X(1:N/2,:))),'edgecolor','none');\n\
set(gca,'yscale','log')\n\
view(0,90);\n\
axis tight\n\
ylabel('f (Hz)')\n\
title('audio unmasked')\n\
ax(2)=subplot(212);\n\
surf(xg, yg, 20*log10(abs(XMasked(1:N/2,:))),'edgecolor','none');\n\
set(gca,'yscale','log')\n\
view(0,90);\n\
axis tight\n\
xlabel('time block')\n\
ylabel('f (Hz)')\n\
title('audio masked')\n\
end\n\
"<<endl;

    cout<<"please run the script view.m in octave or Matlab"<<endl;
    return ret;
}
