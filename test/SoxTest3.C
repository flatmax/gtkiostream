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

    int ret;
    Sox<double> sox;

    vector<string> formats=sox.availableFormats();
    cout<<"The known output file extensions (output file formats) are the following :"<<endl;
    for (int i=0; i<formats.size(); i++)
        cout<<formats[i]<<' ';
    cout<<endl;

    int M=2;
    Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  x; ///< The reference signal
    x=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Zero(10,M);
    x(2,1)=1.2;
    double fs=96.e3;
    // if ((ret=sox.openWrite(fileName, fs, x.cols(), x.maxCoeff())))
    //     return SoxDebug().evaluateError(ret);
    // if ((ret=sox.write(x))!=x.rows()*x.cols())
    //     return SoxDebug().evaluateError(ret);
    // sox.closeWrite();
    // test memory write
    char * bufferWM;
    size_t buffer_size;
    if (ret=sox.openMemWrite((intptr_t)&bufferWM, &buffer_size, fs, M, x.maxCoeff(), "wav"))
      return SoxDebug().evaluateError(ret);

    if ((ret=sox.write(x))!=(x.rows()*x.cols()))
      return SoxDebug().evaluateError(ret);
    sox.closeWrite(); // close the output file
    cout<<"written"<<endl;
    cout<<"size "<<buffer_size<<endl;

    string fileName("/tmp/sox.memTest.wav");
    ofstream oFile(fileName, std::ios::binary);
    oFile.write(bufferWM, buffer_size);
    if (oFile.bad()){
      oFile.close();
      cout<<"failed to write the file"<<endl;
      return -1;
    }
    oFile.close();
    free(bufferWM);
    oFile.open(fileName+".max");
    oFile<<x.maxCoeff()<<endl;
    oFile.close();

    Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>  y; ///< The loaded reference signal
    if (ret=sox.openRead(fileName))
        return SoxDebug().evaluateError(ret);
    if (ret=sox.read(y))
        return SoxDebug().evaluateError(ret);

    cout<<x<<"\n\n"<<y<<endl;

    return 0;
}
