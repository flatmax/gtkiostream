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

int printUsage(string name, int chCnt, unsigned int N, unsigned int fs, char type) {
    cout<<name<<" : An application to generate filters and save them in audio files."<<endl;
    cout<<"Usage:"<<endl;
    cout<<"\t "<<name<<" [options] outFileName"<<endl;
    cout<<"\t e.g. "<<name<<" [options] /tmp/FIR.wav"<<endl;
    cout<<"\t -c : The number of channels : (-c "<<chCnt<<")"<<endl;
    cout<<"\t -N : The number of samples : (-N "<<N<<")"<<endl;
    cout<<"\t -r : The sample rate to use in Hz : (-r "<<fs<<")"<<endl;
    cout<<"\t -t : Use noise (n) or an impulse (i) :  (type "<<type<<")"<<endl;
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

  if (argc<2 || op.getArg<string>("h", argc, argv, help, i=0)!=0)
      return printUsage(argv[0], chCnt, N, fs, type);
  if (op.getArg<string>("help", argc, argv, help, i=0)!=0)
    return printUsage(argv[0], chCnt, N, fs, type);

  int res;
  float maxVal=1.;
  cout<<"maxval = "<<maxVal<<endl;

  Eigen::Array<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> filters(N, chCnt);
  filters.setZero();
  if (type=='i'){
    printf("Generating an impulse filter.\n");
    filters.row(0)=maxVal;
  } else {
    printf("Generating a noise filter.\n");
    filters.setRandom();
    filters=filters/2;
    maxVal=filters.maxCoeff();
  }
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
