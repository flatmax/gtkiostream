/* Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
#include "Octave.H"

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]){

    string tempPath="/tmp"; //note: if you don't have /tmp on your system, then you will ahve to change the tempmorary dir path name

    // start octave - adding the tempPath to the octave path
    vector<string> args(3); args[0]=string("--silent"); args[1]=string("--path"); args[2]=tempPath;
    Octave octave(args);

    // create the OctaveTest.m script
    ofstream out((tempPath+"/OctaveTest.m").c_str());
    out<<"function out=OctaveTest(in)"<<endl;
    out<<"in"<<endl;
    out<<"'octave out'"<<endl;
    out<<"out=(rand(3,3)-.5)*100"<<endl;
    out.close();

    // run the script OctaveTest.m ... which is in the tempPath
    vector<cv::Mat> input(1), output(1);
    input[0].create(3, 3, CV_64F); // if you don't use CV_64F it will exit with error as the expected types will mismatch and you will not give and get the correct values to/from octave.
    randu(input[0], -255., 255.);
    cout<<"C++ input"<<endl;
    cout<<input[0]<<endl;
    octave.runM("OctaveTest", input, output);
    cout<<"c++ output:"<<endl;
    cout<<output[0]<<endl;

    return 0;
}
