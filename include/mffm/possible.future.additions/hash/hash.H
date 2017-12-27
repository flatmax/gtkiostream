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
#ifndef HASH_H
#define HASH_H
#include "Array.H"

template<class ATYPE>
class Arrays : public Array<ATYPE>{
public:
    Arrays(void) : Array<ATYPE>(){}
    Arrays(unsigned int size) : Array<ATYPE>(size){}
////    friend ostream& operator<<(ostream& c, const Arrays<ATYPE>& array);
};

//ostream& operator<<(ostream& c, const Arrays<ATYPE>& array){
//    for (int i=0;i<array.len();i++)
//        c<<array[i]<<'\n';
//    return c;
//}

template<class HTYPE>
class Hash {
    Arrays< Array<HTYPE> > arrays;

    int hashFunction(int input, int min){//, int max){
        //cout<<"input, min, index = "<<input<<", "<<min<<", "<<input-min<<endl;
        return input-min;
    }
public:
    void hashSort(unsigned int size, int*data){
        // O(N) find min and max
        int min=RAND_MAX;//, max=-1;
        for (int i=0;i<size;i++){
            //if (data[i]>max) max=data[i];
            if (data[i]<min) min=data[i];
        }
        //cout<<"max="<<max<<endl;
        cout<<"min="<<min<<endl;

        // O(N) - use hash function to index the arrays
        for (int i=0;i<size;i++){
            unsigned int hashIndex=hashFunction(data[i],min);//,max);
            //cout<<"hashIndex="<<hashIndex<<'\t';
            //cout<<"array len="<<arrays[hashIndex].len()<<'\n';
            arrays[hashIndex][arrays[hashIndex].len()]=data[i];
        }

        for (int i=0;i<arrays.len();i++){
            if (arrays[i].len()){
                arrays[i].dump();
                cout<<endl;
            }
        }
    }
};
#endif //HASH_H
