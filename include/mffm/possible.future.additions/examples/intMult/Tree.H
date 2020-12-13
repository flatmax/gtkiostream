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
#ifndef TREE_H_
#define TREE_H_

#include <iostream>
using namespace std;

#include <math.h>
#include <stdlib.h>

class Tree {
Tree *childL, *childR;
int val; // value of this node
uint maximum, minimum; // represents the maximumimum and minimumimum numbers below this node

public:

Tree (void){
    childL=(Tree*)NULL;
    childR=(Tree*)NULL;
    val=0;
    maximum=minimum=0;
}

~Tree (void){
    if (childL)
        delete childL;
    if (childR)
        delete childR;
}

void addLayers(uint N, int *vals, uint minimumIn=1){
    if (N>=0){
        minimum=minimumIn;
        maximum=minimum+(uint)pow(2.0,(double)N)-1;
        if (minimum==maximum)
            val=vals[minimum];
        N=N-1;
        if (N!=-1){
            childL= new Tree;
            if (!childL){
            cerr<<"Couldn't add childL"<<endl; exit(-1);
            }
            childL->addLayers(N,vals,minimum);
            childR= new Tree;
            if (!childR){
            cerr<<"Couldn't add childR"<<endl; exit(-1);
            }
            childR->addLayers(N,vals,minimum+(maximum-minimum+1)/2);
        }
        if (minimum!=maximum)
            val=childL->val*childR->val;
        cout<<"layer="<<N<<", minimum="<<minimum<<", maximum="<<maximum<<", val="<<val<<endl;
    }
    return;
}

int multiply(uint whichNode){
    if (childL==NULL & childR==NULL) // the case where this is 'whichNode'
        if ((whichNode==maximum) & (whichNode==minimum)){
            //cout<<"1returning 1"<<endl;
            return 1;
        } else {
            //cout<<"1returning minimum="<<minimum<<", maximum="<<maximum<<", val="<<val<<endl;
            return val;
        }

    int value=1;
    if (childL) // If the left child exists ... compute
        if ((whichNode<childL->minimum) | (whichNode>childL->maximum)){
            //cout<<"2lreturning , value="<<childL->val<<endl;
            value*=childL->val;
        }else{
            //cout<<"2lreturning minimum="<<childL->minimum<<", maximum="<<childL->maximum<<", value*=->mult"<<endl;
            value*=childL->multiply(whichNode);
        }
    else // else the right child must exist - so set to 1
        value*=1;
    if (childR) // If the right child exists ... compute
        if ((whichNode<childR->minimum) | (whichNode>childR->maximum)){
            //cout<<"2rreturning , value="<<childR->val<<endl;
            value*=childR->val;
        } else {
            //cout<<"2rreturning minimum="<<childR->minimum<<", maximum="<<childR->maximum<<", value*=->mult"<<endl;
            value*=childR->multiply(whichNode);
        }
    else // else the left child must exist so multiply out its valueue
        value*=1;
    return value;
}

};

#endif //TREE_H_
