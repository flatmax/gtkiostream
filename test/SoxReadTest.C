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

#include <iostream>

int main(int argc, char *argv[]) {

  if (argc<2){
    cout<<"usage : "<<argv[0]<<" fileName"<<endl;
    return -1;
  }

  int ret;
  Sox<double> sox;

  vector<string> formats=sox.availableFormats();
  cout<<"The known output file extensions (output file formats) are the following :"<<endl;
  for (int i=0; i<formats.size(); i++)
    cout<<formats[i]<<' ';
  cout<<endl;

  string fileName(argv[1]);
  cout<<"opening "<<fileName<<endl;
  Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  x; ///< The reference signal
  x=Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>::Random(100000,5);

  Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  y; ///< The loaded reference signal
  if (ret=sox.openRead(fileName))
    return SoxDebug().evaluateError(ret);
  cout<<"opened"<<endl;
  if (ret=sox.read(y))
    return SoxDebug().evaluateError(ret);
  cout<<"read"<<endl;
  cout<<y.rows()<<'\t'<<y.cols()<<endl;
  return 0;
}
