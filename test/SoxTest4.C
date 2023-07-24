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
#include <fstream>
// using namespace std;

int main(int argc, char *argv[]) {
  int ret=0;

  if (argc<2) {
    cerr<<"Usage: "<<argv[0]<<" fileName"<<endl;
    exit(-1);
  }

  Sox<double> sox;
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> b; // the time domain representation of the filter
  if ((ret=sox.openRead(string(argv[1])))<0 && ret!=SOX_READ_MAXSCALE_ERROR) // try to open the file
      SoxDebug().evaluateError(ret, argv[1]+'\n');
  if (ret=sox.read(b)<0) // Try to read the entire file into the coefficient Matrix B.
      SoxDebug().evaluateError(ret, argv[1]);
  printf("rows %ld\n",b.rows());
  printf("cols %ld\n",b.cols());

  return 0;
}
