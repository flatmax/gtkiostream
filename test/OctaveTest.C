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

int main(int argc, char *argv[]){


    string tempPath="/tmp"; //note: if you don't have /tmp on your system, then you will ahve to change the tempmorary dir path name

    // start octave - adding the tempPath to the octave path
    vector<string> args(3); args[0]=string("--silent"); args[1]=string("--path"); args[2]=tempPath;
    Octave octave(args);

    cout<<"octave is init "<<octave.isReady()<<endl;

    int ret=0;
    octave.runM("ls");

    // set some global variables
/*    cout<<"global var test"<<endl;
    Eigen::Matrix<float, Eigen::Dynamic,Eigen::Dynamic> m=Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>::Random(2,2);
    octave.setGlobalVariable(string("m"),m);
    cout<<"global var test1.test2.test3.test4"<<endl;
    octave.setGlobalVariable(string("test1.test2.test3.test4"),m);
    cout<<"global var test1.test2.test3.test4a"<<endl;
    octave.setGlobalVariable(string("test1.test2.test3.test4a"),m);
    cout<<"global var test1.test2.test3a"<<endl;
    octave.setGlobalVariable(string("test1.test2.test3a"),m);
    cout<<"global var test1.test2a"<<endl;
    octave.setGlobalVariable(string("test1.test2a"),m);
    cout<<"global var test1a"<<endl;
    octave.setGlobalVariable(string("test1a"),m);
    cout<<"global var test empty"<<endl;
    if ((ret=octave.setGlobalVariable(string(""),m))<0)
        cout<<OctaveDebug().evaluateError(ret)<<endl;
*/
    cout<<"create a script"<<endl;
    // create the OctaveTest.m script
    ofstream out((tempPath+"/OctaveTest.m").c_str());
    out<<"function out=OctaveTest(in1, in2)"<<endl;
    out<<"whos"<<endl;
/*    out<<"global test1 test1a"<<endl;
    out<<"global m"<<endl;
    out<<"whos"<<endl;
    out<<"m"<<endl;
    out<<"test1a"<<endl;
    out<<"test1"<<endl;
    out<<"test1.test2"<<endl;
    out<<"test1.test2.test3"<<endl;
    out<<"test1.test2.test3.test4"<<endl;
    out<<"test1.test2.test3.test4a"<<endl;*/
    out<<"'octave in1'"<<endl;
    out<<"in1"<<endl;
    out<<"'octave in2'"<<endl;
    out<<"in2"<<endl;
    out<<"'octave out'"<<endl;
    out<<"out=rand(2,2)"<<endl;
    out.close();

    cout<<"script test"<<endl;
    // run the script OctaveTest.m ... which is in the tempPath
    vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> > input(1), output(1);
    Eigen::Matrix<float, 1, 1> dVal;
    dVal(0,0)=10.;
    input[0].resize(2,2); input[0]<<1,2,3,4;
    input[1]=dVal;
    cout<<"C++ input"<<endl;
    cout<<input[0]<<endl;
    octave.runM("OctaveTest", input, output);
    cout<<"c++ output:"<<endl;
    cout<<output[0]<<endl;

    vector<vector<vector<float> > > inputV(1),  outputV;
    inputV[0].resize(2);
    inputV[0][0].resize(2);
    inputV[0][1].resize(2);
    inputV[0][0][0]=1.; inputV[0][0][1]=2.;
    inputV[0][1][0]=3.; inputV[0][1][1]=4.;
    input[1]=dVal;
    octave.runM("OctaveTest", inputV, outputV);
    cout<<"c++ outputV:"<<endl;
    for (int i=0; i<2; i++){
        for (int j=0; j<2; j++)
            cout<<outputV[0][i][j]<<'\t';
        cout<<endl;
    }
    cout<<endl;

    string testStr("testStr");
    cout<<"test string converted to int : "<<endl;
    for (int i=0; i<testStr.size(); i++)
        cout<<(int)testStr[i]<<'\t';
    cout<<endl;

    Eigen::Matrix<int, Eigen::Dynamic, 1> strMat=octave.str2Mat<int>(testStr);
    cout<<"octave.str2Mat : "<<endl;
    cout<<strMat.transpose()<<endl;

    vector<int> strVec=octave.str2Vec<int>(testStr);
    cout<<"octave.str2Vec : "<<endl;
    for (int i=0; i<strVec.size(); i++)
        cout<<strVec[i]<<'\t';
    cout<<endl;

    return 0;
}
