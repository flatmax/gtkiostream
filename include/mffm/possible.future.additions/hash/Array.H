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
#ifndef ARRAY_H
#define ARRAY_H
#include <iostream>
using namespace std;
#include <stdlib.h>

template<class ATYPE>
class Array {
    ATYPE *array;
    unsigned int size;

    void allocMem(int sizeIn){
        if (array)
        deleteMem();
        //cout<<"allocMem("<<sizeIn<<")"<<endl;
        array= new ATYPE[sizeIn];
        if (!array){
            cerr<<"Couldn't allocate array memory of size "<<size<<endl;
            exit(-1);
        }
        size=sizeIn;
    }

    void deleteMem(ATYPE* ptr=NULL){
        if (ptr=NULL) ptr=array;
        //cout<<"deleteMem"<<endl;
        if (ptr){
            delete [] ptr;
            ptr=NULL;
            size=0;
        }
    }
public:
    Array(void){
        size=0;
        array=NULL;
    }
    Array(unsigned int sizeIn){
        size=0;
        array=NULL;
        allocMem(sizeIn);
    }

    ~Array(void){
        deleteMem();
    }

    int len(void){return size;}

    ATYPE& operator[](unsigned int index){
        if (array==NULL) // handle the case when there are no initialised arrays
            allocMem(index+1);
        else if ((index+1)>size){// create a larger array and copy data across
            ATYPE* oldArray=array;
            int oldSize=size;
            allocMem(index+1);
            for (int i=0;i<oldSize;i++)
                array[i]=oldArray[i];
            deleteMem(oldArray);
        }
        return array[index];
    }

//    friend ostream& operator<<(ostream& c, const Array<ATYPE>& array);

    void dump(void){
        for (int i=0;i<len();i++)
            cout<<array[i]<<'\t';
    }

};

//ostream& operator<<(ostream& c, const Array<ATYPE>& array){
//    for (int i=0;i<array.len();i++)
  //      c<<array[i]<<'\t';
//    return c;
//}
#endif //ARRAY_H
