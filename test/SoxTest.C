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

#include <iostream>

int main(int argc, char *argv[]) {

    int ret;
    {
      Sox<double> sox;

      vector<string> formats=sox.availableFormats();
      cout<<"The known output file extensions (output file formats) are the following :"<<endl;
      for (int i=0; i<formats.size(); i++)
          cout<<formats[i]<<' ';
      cout<<endl;

      string fileName("/tmp/soxTest.w64");

      Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  x; ///< The reference signal
      x=Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>::Random(100000,5);

      double fs=96.e3;
      if ((ret=sox.openWrite(fileName, fs, x.cols(), x.maxCoeff())))
          return SoxDebug().evaluateError(ret);
      if ((ret=sox.write(x))!=x.rows()*x.cols())
          return SoxDebug().evaluateError(ret);
      sox.closeWrite();

      Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  y; ///< The loaded reference signal
      if (ret=sox.openRead(fileName))
          return SoxDebug().evaluateError(ret);
      if (ret=sox.read(y))
          return SoxDebug().evaluateError(ret);

      // cout<<x-y<<endl;
      // cout<<endl;
      //cout<<100.*(x-y)*x<<endl;
    }
    {
      Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  x; ///< The reference signal
      x=Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>::Random(100000,5);
      Sox<int> sox;
      ret=sox.openWrite("/tmp/soxTest.wav", 96.e3, x.cols(), x.maxCoeff());
    	if (ret<0)
    		return SoxDebug().evaluateError(ret);
    	sox.write(x);

    }

    return 0;
}
