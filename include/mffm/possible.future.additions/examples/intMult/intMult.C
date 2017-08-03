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
// g++ intMult.C -o intMult

#include "Tree.H"

int main (int argc, char *argv[]){

Tree rootNode;

int N=3;
int count=(uint)pow(2.0,3.0);
int vals[count];
int output[count];

for (int i=1;i<=count;i++)
    vals[i]=i;

rootNode.addLayers(N,vals);

cout<<"tree results:\n";
for (int i=1;i<=count;i++){
    output[i]=rootNode.multiply(i);
    cout<<output[i]<<endl;
}


//brute force
cout<<"tree results:\n";
for (int i=1;i<=count;i++){
    int val=1;
    for (int j=1; j<i; j++)
        val*=j;
    for (int j=i+1; j<=count; j++)
        val*=j;
    cout<<val<<endl;
}

return 0;
}
