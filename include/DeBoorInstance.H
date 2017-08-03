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
#ifndef DEBOORINSTANCE_H
#define DEBOORINSTANCE_H

#include "DeBoor.h"

DeBoor deBoor;

float *x;
float *y;
float *dy;
float *result;
float p;
int N;

void csaps(){
    deBoor.csaps(x, y, dy, N, p);
    for (int i=0; i<N; i++)
        result[i]=deBoor[i];
}

void create(int n){
    if (N<n){
        deleteData();
    }
}

void deleteData(){
    if (x)
        delete [] x;
    x=NULL;
    if (y)
        delete [] y;
    y=NULL;
    if (dy)
        delete [] dy;
    dy=NULL;
    if (result)
        delete [] result;
    result=NULL;
    N=0;
}

#endif // DEBOORINSTANCE_H
