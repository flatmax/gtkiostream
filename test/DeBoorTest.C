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

#include <iostream>
#include <fstream>
using namespace std;

#include "DeBoor.H"
#include <stdlib.h>


int main(int argc, char*argv[]){
    srand48(time(NULL));
    DeBoor deBoor;
    int N=10;
    float x[N], y[N], dy[N];
    for (int n=0; n<N; n++){
        x[n]=(float)n+1;
        dy[n]=1.e0;
//        y[n]=2.*(drand48()-.5);
        y[n]=2.*(drand48());
    }
    float s=1.e2;
    s=0.;
    cout<<"calling csaps"<<endl;
    deBoor.csaps(x, y, dy, N, s);
    cout<<"done csaps"<<endl;
    ofstream output("/tmp/x.txt");
    for (int n=0; n<N; n++)
        output<<x[n]<<endl;
    output.close();

    output.open("/tmp/y.txt");
    for (int n=0; n<N; n++)
        output<<y[n]<<endl;
    output.close();

    output.open("/tmp/result.txt");
    for (int n=0; n<N; n++)
        output<<deBoor[n]<<endl;
    output.close();

    cout<<"done"<<endl;
    return 0;
}
