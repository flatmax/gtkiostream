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
#include "hash.H"
#include <math.h>
#include <time.h>

void testDynamicArray(){
    int cnt=10;
    Array<int> array(cnt);
    for (int i=0;i<cnt;i++)
        array[i]=i;
    for (int i=0;i<cnt+2;i++)
        array[i]=i;
    for (int i=0;i<cnt+1;i++)
        cout<<array[i]<<'\n';
    cout<<endl;
}

void testArrays(){
    Arrays< Array<int> > arrays(10);
    for (int i=0;i<10;i++)
        for (int j=0;j<i+1;j++)
            arrays[i][j]=j;
    //cout<<arrays<<endl;
    //arrays.dump();
    for (int i=0;i<10;i++)
        for (int j=0;j<i+1;j++)
            cout<<arrays[i][j]<<'\t';
    cout<<endl;
}

int main(int argc, char *argv[]){
    //testDynamicArray();
    //testArrays();
    //return 0;
    int cnt=10;
    srand(time(NULL));
    Array<int> array(cnt);
    for (int i=0;i<cnt;i++){
        array[i]=(int)round((float)(rand()-RAND_MAX/2)*2.0/(float)RAND_MAX*(float)cnt);
        cout<<array[i]<<'\t';
    }
    cout<<endl;

    Hash<int> hashArrays;
    hashArrays.hashSort(cnt, &array[0]);
//    for (int i=0;i<hashArrays.len();i++)
//        for (int j=0;j<hashArrays[i];j++)
//            cout<<hashArrays[i][j]<<'\t';
//    cout<<endl;

    return 0;
}
