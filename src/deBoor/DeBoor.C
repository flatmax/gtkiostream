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
#include "DeBoor.H"

#include <iostream>
using namespace std;

#include <stdio.h>

extern "C" {
    void bsplpp_(float *t, float *bcoef,int *n, int *k, float scrtch[][4], float *breakp, float coef[][4], int *l);
    float smooth_(float *x, float *y, float *dy, int *npoint, float *s, float v[][7], float a[][4]);
    void csaps_(float *x, float *y, float *dy, int *npoint, float *s, float *result);
}

DeBoor::DeBoor() {
    v=a=NULL;
    N=0;
    float t[11]= {0., 0., 0., 0., 1., 3., 4., 6., 6., 6., 6.};
    float bcoef[7] = {0., 0., 0., 1., 0., 0., 0.};

    int n=7, k=4;
    float scrtch[4][4];
    bsplpp_(t,bcoef,&n,&k,scrtch,breakp,coef,&l);
    cout<<"breakp : ";
    for (int i=0; i<5; i++)
        cout<<breakp[i]<<'\t';
    cout<<endl;
    cout<<"coef : ";
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            cout<<coef[j][i]<<'\t';
    cout<<endl;
    cout<<"l : "<<l<<endl;
}

void DeBoor::deleteMatrix(float **m) {
    cout<<N<<endl;
    cout<<m<<endl;
    if (m) {
        cout<<m<<endl;
        for (int i=0; i<N; i++) {
            cout<<i<<endl;
            if (m[i]) {
                cout<<m[i]<<endl;
                delete [] m[i];
                cout<<i<<endl;
                m[i]=NULL;
                cout<<i<<endl;
            }
        }
        delete [] m;
        m=NULL;
    }
}

void DeBoor::deleteMatrices() {
    cout<<"DeBoor::deleteMatrices"<<endl;
    deleteMatrix(v);
    deleteMatrix(a);
}

DeBoor::~DeBoor() {
    deleteMatrices();
}


float **&DeBoor::createMatrix(int k) {
    float **m=NULL;
    cout<<"DeBoor::createMatrix, k="<<k<<endl;
    m=new float*[N];
    if (m) {
        for (int i=0; i<N; i++)
            m[i]=NULL;
        for (int i=0; i<N; i++) {
            m[i]=new float[k];
            if (!m[i]) {
                deleteMatrix(m);
                break;
            }
            cout<<m[i]<<endl;
        }
    }
    return m;
}

void DeBoor::createMatrices(int n) {
    if (N<n) {
        deleteMatrices();
        N=n;
        v=createMatrix(7);
        a=createMatrix(4);
    }
}

//void DeBoor::csapsOrig(float *x, float *y, float *dy, int n, float p) {
//    createMatrices(n);
////    if (v.size()<n)
////        v.resize(n);
////    if (a.size()<n)
////        a.resize(n);
//    cout<<v<<endl;
//    cout<<a<<endl;
//    if (v && a) {
//        float ret=smooth_(x, y, dy, &n, &p, (float (*)[7])&v[0], (float (*)[4])&a[0]);
//    } else
//        printf("DeBoor::csaps : memory error\n");
//}

void DeBoor::csaps(float *x, float *y, float *dy, int n, float s) {
    if (resultCSAPS.size()<n)
        resultCSAPS.resize(n);
    csaps_(x, y, dy, &n, &s, &resultCSAPS[0]);
    cout<<"y, results"<<endl;
    for (int i=0; i<n; i++)
        cout<<y[i]<<'\t'<<resultCSAPS[i]<<'\n';
}

float DeBoor::operator[](int i){
    if (i<resultCSAPS.size() && i>0)
        return resultCSAPS[i];
}
