/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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
using namespace std;
int main(int argc, char *argv[]) {

  ifstream file("/tmp/soxTest.wav", std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)){
      cout<<"failed to read the file"<<endl;
      return -1;
  }

  Sox<double> sox;
  int ret=0;
  Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  x; ///< The reference signal
  cout<<size<<endl;
  if (ret=sox.openRead((intptr_t)buffer.data(), size))
    if (ret!=SOX_READ_MAXSCALE_ERROR)
      return SoxDebug().evaluateError(ret);
  sox.setMaxVal(10.);
  if (ret=sox.read(x))
      return SoxDebug().evaluateError(ret);
  cout<<x<<endl;
  return 0;
}
