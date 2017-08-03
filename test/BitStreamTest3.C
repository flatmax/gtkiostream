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

using namespace std;
#include "BitStream.H"
#include <bitset>
#include <sstream>
#include <iostream>

int testBits(ostringstream &result, ostringstream &reference) {
    cout<<"result : "<<result.str()<<endl;
    cout<<"ref    : "<<reference.str()<<endl;
    if (result.str().compare(reference.str())!=0) {
        cout<<"ERROR : result and reference don't align"<<endl;
        return -1;
    }
    return 0;
}

typedef unsigned int VTYPELOCAL; ///< Use this type for the basic stream type

class BitStreamTestClass : public BitStream {
public:
    BitStreamTestClass() : BitStream(){
        if (sizeof(VTYPE)!=sizeof(VTYPELOCAL))
            assert("BitStream size mismatch for testing");
        }
    unsigned int basicTypeSize() {
        return sizeof(VTYPE);
    }
};

#ifndef __MINGW32__
typedef unsigned long ulong;
#else
typedef unsigned long long ulong;
#endif
int main(int argc, char *argv[]) {
    {
        BitStreamTestClass bitStream;
        if (bitStream.basicTypeSize()*CHAR_BIT!=32) {
            cerr<<argv[0]<<" BitStream VTYPE size != 32 bits - this test is not setup for that bit count"<<endl;
            abort();
        }

    }
    {
        const ulong l[]={(ulong)0xfedcba987654321, (ulong)0x123456789abcdef, (ulong)0xfefcfaf8f6f4f2f};
        BitStreamTestClass bitStream;
        bitStream<<l[0]<<l[1]<<l[2];
        const int size=3*64;
        if (size != bitStream.size()){
            cout<<"size mismatch please fix...."<<endl;
            return -1;
        }

        int cnt=0;
        for (unsigned int i=1; cnt<size; cnt+=i, i++){
            cout<<"i "<<i<<" cnt "<<cnt<<endl;
            ostringstream result, reference;
            BitStreamTestClass bitStream2;
            bitStream2.push_back(bitStream.pop_back<ulong>(i), i);
            result<<bitStream2;

            bitset<size> bsl(l[0]);
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bsl[j]=(l[0]>>j)&0x1;
            bsl<<=(sizeof(ulong)*8);
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bsl[j]=(l[1]>>j)&0x1;
            bsl<<=(sizeof(ulong)*8);
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bsl[j]=(l[2]>>j)&0x1;

            ulong maskL=(ulong)pow(2.,(float)i)-1;
            bitset<size> mask;
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                mask[j]=(maskL>>j)&0x1;

            reference<<((bsl&(mask<<cnt))>>cnt);
            reference.str(reference.str().substr(bsl.size()-i, i));
            if (testBits(result, reference)<0) return -1;
            cout<<endl;
        }
    }
    {
        const ulong l[]={(ulong)0xfedcba987654321, (ulong)0x123456789abcdef, (ulong)0xfefcfaf8f6f4f2f};
        BitStreamTestClass bitStream;
        bitStream<<l[0]<<l[1]<<l[2];

        const int size=3*64;
        if (size != bitStream.size()){
            cout<<"size mismatch please fix...."<<endl;
            return -1;
        }

        int cnt=0;
        for (int i=19; i>=0; cnt+=i, i--){
            cout<<"i "<<i<<" cnt "<<cnt<<endl;
            ostringstream result, reference;
            BitStreamTestClass bitStream2;
            bitStream2.push_back(bitStream.pop_back<unsigned long>(i), i);
            result<<bitStream2;

            bitset<size> bsl(l[0]);
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bsl[j]=(l[0]>>j)&0x1;
            bsl<<=(sizeof(ulong)*8);
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bsl[j]=(l[1]>>j)&0x1;
            bsl<<=(sizeof(ulong)*8);
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bsl[j]=(l[2]>>j)&0x1;

            ulong maskL=(ulong)pow(2.,(float)i)-1;
            bitset<size> mask;
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                mask[j]=(maskL>>j)&0x1;

            reference<<((bsl&(mask<<cnt))>>cnt);
            reference.str(reference.str().substr(bsl.size()-i, i));
            if (testBits(result, reference)<0) return -1;
            cout<<endl;
        }
    }
//    {
//        const unsigned int l[]={0x87654321, 0x9abcdefc, 0x12345678};
//        BitStreamTestClass bitStream;
//        bitStream<<l[0]<<l[1]<<l[2];
//
//        const int size=3*32;
//        if (size != bitStream.size()){
//            cout<<"size mismatch please fix...."<<endl;
//            return -1;
//        }
//
//        int cnt=0;
//        int i=size+8;
//        cout<<sizeof(unsigned long long)<<endl;
//        cout<<"i "<<i<<" cnt "<<cnt<<endl;
//        ostringstream result, reference;
//        BitStreamTestClass bitStream2;
//        unsigned  __int128 data=bitStream.pop_back<unsigned  __int128>(i);
//        unsigned long long *dataLL=(unsigned long long*)&data;
//        cout<<"0x"<<hex<<dataLL[1]<<dataLL[0]<<dec<<endl;
//        bitStream2.push_back(data, i);
//        result<<bitStream2;
//
//        bitset<size> bsl(l[0]);
//        bsl<<=(sizeof(unsigned int)*8);
//        bsl|= bitset<size>(l[1]);
//        bsl<<=(sizeof(unsigned int)*8);
//        bsl|= bitset<size>(l[2]);
//
//        cout<<bitStream2<<endl;
//        cout<<bsl<<endl;
//
////        bitset<size> mask((long)pow(2.,(float)i)-1);
////        reference<<((bsl&(mask<<cnt))>>cnt);
////        reference.str(reference.str().substr(bsl.size()-i, i));
////        if (testBits(result, reference)<0) return -1;
////        cout<<endl;
//    }


    {
        BitStreamTestClass bitStream;
        for (int i=0; i<135; i++)
        bitStream<<(unsigned int)i;
        bitStream.clear();
        if (bitStream.size()!=0){
            cout<<"the bitstream didn't clear : error"<<endl;
            return -1;
        }
    }
    cout<<"all passed"<<endl;
    return 0;
}

