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

#include <iostream>
#include <sstream>
using namespace std;

#include "Sox.H"
#include "OptionParser.H"
#include "DSP/ImpulseBandLimited.H"

int printUsage(string name, int chCnt, unsigned int N, unsigned int fs, char type, bool logStep, float fi, float fa) {
    cout<<name<<" : An application to generate filters and save them in audio files."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"     "<<name<<" [options] outFileName"<<endl;
    cout<<"     e.g. "<<name<<" [options] /tmp/FIR.wav"<<endl;
    cout<<"     -c : The number of channels : (-c "<<chCnt<<")"<<endl;
    cout<<"     -N : The number of samples : (-N "<<N<<")"<<endl;
    cout<<"     -r : The sample rate to use in Hz : (-r "<<fs<<")"<<endl;
    cout<<"     -t : Use noise (n) or a band pass (b) or an impulse (i) or an imulse with (h) hushed very low level noise :  (-t "<<type<<")"<<endl;
    cout<<"     -S : Log2 step filter sizes down every 2 channels (not with -t i) :  (-S "<<logStep<<")"<<endl;
    cout<<"     -i : The mInimum frequency (in the case of -t b a bandpass) : (-i "<<fi<<")"<<endl;
    cout<<"     -a : The mAximum frequency (in the case of -t b a bandpass) : (-a "<<fa<<")"<<endl;
    Sox<float> sox;
    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;
    return 0;
}

int main(int argc, char *argv[]) {
  int chCnt=2; // The number of channels to record
  int fs=48000; // The sample rate
  unsigned int N=fs; // The number of samples to use
  bool logStep=false; // Whether to log step sizes down every 2 channles

  float fi=10.; // The minimum band limited impulse frequency
  float fa=(float)fs/2.-10.; // The maximum band limited impulse frequency

  OptionParser op;
  int i=0, ret;
  string help;

  char type='i';

  if (op.getArg<int>("c", argc, argv, chCnt, i=0)!=0)
      ;

  if (op.getArg<unsigned int>("N", argc, argv, N, i=0)!=0)
      ;
  if (op.getArg<char>("t", argc, argv, type, i=0)!=0)
      ;

  if (op.getArg<int>("r", argc, argv, fs, i=0)!=0)
      ;

  if (op.getArg<bool>("S", argc, argv, logStep, i=0)!=0)
      logStep=true;

  if (op.getArg<float>("i", argc, argv, fi, i=0)!=0)
      ;

  if (op.getArg<float>("a", argc, argv, fa, i=0)!=0)
      ;

  if (argc<2 || op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0], chCnt, N, fs, type, logStep, fi, fa);
  if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
    return printUsage(argv[0], chCnt, N, fs, type, logStep, fi, fa);

  int res;
  float maxVal=1.;
  cout<<"maxval = "<<maxVal<<endl;

  ImpulseBandLimited<double> ibl;
  Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> filters(N, chCnt);
  filters.setZero();
  switch (type){
    case 'i':
      printf("Generating an impulse filter.\n");
      filters.row(0)=maxVal;
      break;
    case 'h':
      printf("Generating an impulse filter with a very low level hushed noise.\n");
      filters.setRandom();
      filters=filters/(int)pow(2.,24.);
      filters.row(0)=(int)pow(2.,31.);
      maxVal=filters.abs().maxCoeff();
      break;
    case 'n':
      printf("Generating a noise filter.\n");
      filters.setRandom();
      filters=filters/2;
      maxVal=filters.abs().maxCoeff();
      break;
    case 'b':
      res = ibl.generateImpulseShift((float)N/(float)fs, (float)fs, fi, fa);
      if (res<0)
        return res;
      if (ibl.rows() != N){
        cout<<"Band pass filter generated the wrong size filter, exiting"<<endl;
        return -1;
      }
      if (ibl.abs().maxCoeff() >=1.){
        cout<<"Band pass filter is >=1. error"<<endl;
        return -1;
      }

      ibl*=pow(2.,31.)/2;
      filters = ibl.replicate(1, chCnt).cast<int>();
      maxVal=filters.abs().maxCoeff();
      break;
    default:
      printf("Unknown filter type %c.\n", type);
      return -1;
      break;
  }
  if (logStep) // put zeros at the end of filters, halving effective lengths every 2 channels
    for (int m=2; m<(filters.cols()-1); m+=2)
        filters.block(filters.rows()/m, m, filters.rows()-filters.rows()/m, 2).setZero();

  Sox<int> sox;
  res=sox.openWrite(argv[argc-1], fs, chCnt, maxVal);
  if (res<0)
    return SoxDebug().evaluateError(res,string("when opening the file ")+argv[argc-1]);
  cout<<"opened the file : "<<argv[argc-1]<<" for writing"<<endl;
  if (sox.write(filters)!=N*chCnt)
    cout<<"Error: wrote the wrong number of samples to file"<<endl;
  sox.closeWrite();
  return 0;
}
