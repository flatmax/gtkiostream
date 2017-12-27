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

#include "DSP/Decomposition.H"

#ifndef _MSC_VER
#include "Sox.H"
#else
// Note : Microsoft doesn't understand the different between upper and lower case in file names.
// on microsoft, you have to manually rename Sox.H to SoxWindows.H
#include "SoxWindows.H"
#endif

int main(int argc, char *argv[]) {

//omp_set_num_threads(4);
//    Eigen::setNbThreads(4);
    Sox<float> sox;

    string fileName("test/testVectors/11.Neutral.44k.wav");

    int ret;
    if ((ret=sox.openRead(fileName))<0  && ret!=SOX_READ_MAXSCALE_ERROR)
        return SoxDebug().evaluateError(ret, argv[argc-2]);
    sox.setMaxVal(1.0);

    Decomposition<float> decomp; // instantiate using the default overlap factor

    int N=decomp.getWindowSize(); // use the default window size. about 2048 samples
    int M=20; // specify the number non-overlapping windows to read

    int cnt=decomp.OverlapAdd<float>::loadData(sox, N, M*N);
    if (cnt<0)
        exit(DecompositionDebug().evaluateError(cnt));

    if ((ret=decomp.findSubSpace())!=NO_ERROR)
        exit(DecompositionDebug().evaluateError(ret));

    return 0;
}

