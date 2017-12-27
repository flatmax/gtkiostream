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
#include <iostream>
using namespace std;
#include "OptionParser.H"

int main(int argc, char *argv[]){
cout<<argc<<endl;
    if (argc<2){
        cout<<"Usage: "<<argv[0]<<" -string someString --num=someNum -h"<<endl;
        return -1;
    }
    OptionParser op;
    string ret;
    int i=0, j;
    cout<<"key = h: i = "<<i<<", getArg returns "<<op.getArg("h", argc, argv, ret, i)<<endl;
    cout<<"argument was : "<<ret<<endl;
    cout<<"key = string: i = "<<i<<", getArg returns "<<op.getArg("string", argc, argv, ret, i)<<endl;
    cout<<"argument was : "<<ret<<endl;
    cout<<"key = num: i = "<<i<<", getArg returns "<<op.getArg("num", argc, argv, j, i)<<endl;
    cout<<"argument was : "<<j<<endl;

    return 0;
}
