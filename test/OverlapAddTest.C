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
#include "DSP/OverlapAdd.H"

#ifndef _MSC_VER
#include "Sox.H"
#else
// Note : Microsoft doesn't understand the different between upper and lower case in file names.
// on microsoft, you have to manually rename Sox.H to SoxWindows.H
#include "SoxWindows.H"
#endif

int main(int argc, char *argv[]) {

    OverlapAdd<float> overlapAdd(1./3.);

    Sox<float> sox;

    string fileName("test/testVectors/ramp.wav");

    int ret;
    if ((ret=sox.openRead(fileName))<0  && ret!=SOX_READ_MAXSCALE_ERROR)
        return SoxDebug().evaluateError(ret, fileName);

    int N=9;
    int cnt=overlapAdd.loadData(sox, N, N*10);
    if (cnt<0)
        OverlapAddDebug().evaluateError(cnt);
    else
        if (cnt>0)
            cout<<"OverlapAdd couldn't read in the last "<<cnt << " samples";
    cout<<overlapAdd.getDataCopy().transpose()<<endl;
    sox.closeRead();

    fileName="/tmp/OverlapAddTest.wav";
    if ((ret=sox.openWrite(fileName, sox.getFSIn(), 1, overlapAdd.getMaxVal()))<0)
        return SoxDebug().evaluateError(ret, fileName);
    overlapAdd.unloadData(sox);

    sox.closeWrite();


    return ret;
}
