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

using namespace std;
#include "BitStream.H"
#include <bitset>
#include <sstream>
#include <iostream>

#ifndef uint
typedef unsigned int uint;
#endif

int testBits(ostringstream &result, ostringstream &reference) {
    cout<<"result : "<<result.str()<<'\n';
    cout<<"ref    : "<<reference.str()<<'\n';
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

int main(int argc, char *argv[]) {
    {
        BitStreamTestClass bitStream;
        if (bitStream.basicTypeSize()*CHAR_BIT!=32) {
            cerr<<argv[0]<<" BitStream VTYPE size != 32 bits - this test is not setup for that bit count"<<endl;
            abort();
        }

    }
    cout<<"test char reversals"<<endl;
    {
        unsigned char test;
        for (unsigned int i=0; i<(uint)pow(2.,sizeof(test)*8); i++){
            test=i;
            BitStreamTestClass bitStream;
            test=bitStream.reverseBits(test, sizeof(test)*8);
            bitStream<<test;
            ostringstream result, reference;
            result<<bitStream;

            bitset<8> bs=i, bs2=i;
            for (int j=0; j<sizeof(test)*8; j++)
                bs2[sizeof(test)*8-1-j]=bs[j];
            reference<<bs2;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<"test short int reversals"<<endl;
    {
        unsigned short int test;
        for (unsigned int i=0; i<(uint)pow(2.,sizeof(test)*8); i++){
            test=i;
            BitStreamTestClass bitStream;
            test=bitStream.reverseBits(test, sizeof(test)*8);
            bitStream<<test;
            ostringstream result, reference;
            result<<bitStream;

            bitset<16> bs=i, bs2=i;
            for (int j=0; j<sizeof(test)*8; j++)
                bs2[sizeof(test)*8-1-j]=bs[j];
            reference<<bs2;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<" test longer 32 bits"<<endl;
    {
        unsigned int test;
        unsigned int startPoint=(uint)0;
        unsigned int duration=(uint)pow(2.,14);
        for (unsigned int i=startPoint; i<startPoint+duration; i++){
            test=i;
            BitStreamTestClass bitStream;
            test=bitStream.reverseBits(test, sizeof(test)*8);
            bitStream<<test;
            ostringstream result, reference;
            result<<bitStream;

            bitset<32> bs=i, bs2=i;
            for (int j=0; j<sizeof(test)*8; j++)
                bs2[sizeof(test)*8-1-j]=bs[j];
            reference<<bs2;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<" test longer 32 bits - middle overlap point"<<endl;
    {
        unsigned int test;
        unsigned int startPoint=(uint)pow(2.,8)-1;
        unsigned int duration=(uint)pow(2.,14);
        for (unsigned int i=startPoint; i<startPoint+duration; i++){
            test=i<<8;
            BitStreamTestClass bitStream;
            test=bitStream.reverseBits(test, sizeof(test)*8);
            bitStream<<test;
            ostringstream result, reference;
            result<<bitStream;

            bitset<32> bs=i<<8, bs2=i<<8;
            for (int j=0; j<sizeof(test)*8; j++)
                bs2[sizeof(test)*8-1-j]=bs[j];
            reference<<bs2;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<" test longer 32 bits - higher ending at the end"<<endl;
    {
        unsigned int test;
        unsigned int startPoint=(uint)pow(2.,18)-1;
        unsigned int duration=(uint)pow(2.,14);
        for (unsigned int i=startPoint; i<startPoint+duration; i++){
            test=i<<18;
            BitStreamTestClass bitStream;
            test=bitStream.reverseBits(test, sizeof(test)*8);
            bitStream<<test;
            ostringstream result, reference;
            result<<bitStream;

            bitset<32> bs=i<<18, bs2=i<<18;
            for (int j=0; j<sizeof(test)*8; j++)
                bs2[sizeof(test)*8-1-j]=bs[j];
            reference<<bs2;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<" test longer 32 bits with fewer bits packed"<<endl;
    {
        unsigned int test;
        unsigned int bits=17;
        for (unsigned int i=0; i<(uint)pow(2.,(float)bits); i++){
            test=i;
            BitStreamTestClass bitStream;
            test=bitStream.reverseBits(test, bits);
            bitStream.push_back(test, bits);
            ostringstream result, reference;
            result<<bitStream;

            bitset<17> bs=i, bs2=i;
            for (int j=0; j<bits; j++)
                bs2[bits-1-j]=bs[j];
            reference<<bs2;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<" test 1 bit"<<endl;
    {
        unsigned int test;
        for (unsigned int i=0; i<2; i++){
            BitStreamTestClass bitStream;
            bitStream.push_back(bitStream.reverseBits(i, 1), 1);
            ostringstream result, reference;
            result<<bitStream;

            bitset<1> bs=i;
            reference<<bs;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<" test 2 bits"<<endl;
    {
        unsigned int test;
        unsigned int bits=2;
        for (unsigned int i=0; i<pow(2., (float)bits); i++){
            BitStreamTestClass bitStream;
            bitStream.push_back(bitStream.reverseBits(i, bits), bits);
            ostringstream result, reference;
            result<<bitStream;

            bitset<2> bs=i, bs2=i;
            for (int j=0; j<bits; j++)
                bs2[bits-1-j]=bs[j];
            reference<<bs2;

            if (testBits(result, reference)<0) return -1;
        }
    }
    cout<<"all passed"<<endl;
    return 0;
}

