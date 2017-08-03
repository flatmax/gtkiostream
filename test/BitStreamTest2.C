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

#ifndef __MINGW32__
typedef unsigned long ulong;
#else
typedef unsigned long long ulong;
#endif

int testBitsL(ostringstream &result, ostringstream &reference) {
    string refComp;
    if (reference.str().size()>result.str().size())
        refComp=reference.str().substr(0,result.str().size());
    else
        refComp=reference.str();

    cout<<"result: "<<result.str()<<endl;
    cout<<"ref   : "<<refComp<<endl;
    if (result.str().compare(refComp)!=0) {
        cout<<"ERROR : result and reference don't align"<<endl;
        return -1;
    }
    return 0;
}

unsigned int genMask(int M) {
    return (unsigned int)((ulong)pow(2.,(float)M)-1);
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
    {
        const int N=1;
        cout<<"testing rotateL partial words loaded - small stream < 32bits"<<endl;
        ulong i=0x123456789abcdef1;
        for (int j=0; j<32; j++) {
            cout<<"\ntesting j="<<j<<endl;
            cout<<endl;
            BitStreamTestClass bitStream;
            bitStream.push_back(i, N);

            bitStream.rotateL(j);
            ostringstream result, reference;
            result<<bitStream;
            bitset<32*N> bitSet(i&genMask(N)), bitSet2(i&genMask(N));
            for (int k=0; k<32-1; k++)
                bitSet=(bitSet<<N) | bitSet2;

            reference<<(bitSet<<j);
            if (testBitsL(result, reference)<0) {
                bitStream.dump();
                cout<<endl;
                cout<<"testing rotateL failed"<<endl;
                return -1;
            }
        }
    }
    {
        const int N=10;
        cout<<"testing rotateL partial words loaded - small stream < 32bits"<<endl;
        ulong i=0x123456789abcdef1;
        for (int j=0; j<32; j++) {
            cout<<"\ntesting j="<<j<<endl;
            cout<<endl;
            BitStreamTestClass bitStream;
            bitStream.push_back(i, N);

            bitStream.rotateL(j);
            ostringstream result, reference;
            result<<bitStream;
            bitset<5*N> bitSet(i&genMask(N)), bitSet2(i&genMask(N));
            bitSet=(bitSet<<N) | bitSet2;
            bitSet=(bitSet<<N) | bitSet2;
            bitSet=(bitSet<<N) | bitSet2;
            bitSet=(bitSet<<N) | bitSet2;

            reference<<(bitSet<<j);
            if (testBitsL(result, reference)<0) {
                bitStream.dump();
                cout<<endl;
                cout<<"testing rotateL failed"<<endl;
                return -1;
            }
        }
    }
    {
        const int N=32;
        cout<<"testing rotateL partial words loaded - small stream = 32bits"<<endl;
        ulong i=0x123456789abcdef1;
        for (int j=0; j<32; j++) {
            cout<<'\n';
            cout<<"\ntesting j="<<j<<'\n';
            cout<<'\n';
            BitStreamTestClass bitStream;
            bitStream.push_back(i, N);

            bitStream.rotateL(j);
            ostringstream result, reference;
            result<<bitStream;
            bitset<5*N> bitSet(i&genMask(N)), bitSet2(i&genMask(N));
            bitSet=(bitSet<<N) | bitSet2;
            bitSet=(bitSet<<N) | bitSet2;
            bitSet=(bitSet<<N) | bitSet2;
            bitSet=(bitSet<<N) | bitSet2;

            reference<<(bitSet<<j);
            if (testBitsL(result, reference)<0) {
                bitStream.dump();
                cout<<endl;
                cout<<"testing rotateL failed"<<endl;
                return -1;
            }
        }
    }
    {
        cout<<"testing rotateL partial words loaded - small stream > 32bits"<<endl;
        cout<<"size of ulong ="<<sizeof(ulong)<<endl;
        ulong i=0x123456789abcdef1;
        for (int j=0; j<32; j++) {
            cout<<"\ntesting j="<<j<<endl;
            cout<<endl;
            BitStreamTestClass bitStream;
            bitStream<<i;

            bitStream.rotateL(j);
            ostringstream result, reference;
            result<<bitStream;
            bitset<2*(sizeof(ulong)*CHAR_BIT)> bitSet;
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bitSet[j]=(i>>j)&0x1;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet;

            cout<<bitSet<<endl;

            reference<<(bitSet<<j);
            if (testBitsL(result, reference)<0) {
                bitStream.dump();
                cout<<endl;
                cout<<"testing rotateL failed"<<endl;
                return -1;
            }
        }
    }
    {
        cout<<"testing rotateL partial words loaded"<<endl;
        ulong i=0x123456789abcdef1;
        ulong ii=0x1a2b3c4d5e6f789;
        unsigned short s=0x789a;
        for (int j=0; j<32; j++) {
            cout<<"\ntesting j="<<j<<endl;
            cout<<endl;
            BitStreamTestClass bitStream;
            bitStream<<i<<i<<i;
            bitStream<<s;

            bitStream.rotateL(j);
            ostringstream result, reference;
            result<<bitStream;
            bitset<2*(sizeof(ulong)*CHAR_BIT*3+sizeof(unsigned short)*CHAR_BIT)> bitSet, bitSet2, bitSet3(s);
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bitSet[j]=(i>>j)&0x1;
            bitSet2=bitSet;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            reference<<(bitSet<<j);
            if (testBitsL(result, reference)<0) {
                bitStream.dump();
                cout<<endl;
                cout<<"testing rotateL failed"<<endl;
                return -1;
            }
        }
    }
    {
        cout<<"testing whole words "<<endl;
        ulong i=0x123456789abcdef1;
        ulong ii=0x1a2b3c4d5e6f789;
        unsigned short s=0x789a;
        unsigned int totalSize=sizeof(ulong)*CHAR_BIT*3+sizeof(unsigned short)*CHAR_BIT;
        for (int j=32; j<totalSize; j++) {
            cout<<"\ntesting j="<<j<<endl;
            cout<<endl;
            BitStreamTestClass bitStream;
            bitStream<<i<<ii<<i;
            bitStream<<s;

            bitStream.rotateL(j);
            ostringstream result, reference;
            result<<bitStream;
            bitset<2*(sizeof(ulong)*CHAR_BIT*3+sizeof(unsigned short)*CHAR_BIT)> bitSet, bitSet2, bitSet3(s), bitSet4;
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bitSet[j]=(i>>j)&0x1;
            bitSet2=bitSet;
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bitSet4[j]=(ii>>j)&0x1;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet4;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet4;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            reference<<(bitSet<<j);
            if (testBitsL(result, reference)<0) {
                bitStream.dump();
                cout<<endl;
                cout<<"testing rotateL failed"<<endl;
                return -1;
            }
        }
    }
    {
        cout<<"testing longer then available whole words "<<endl;
        ulong i=0x123456789abcdef1;
        ulong ii=0x1a2b3c4d5e6f789;
        unsigned short s=0x789a;
        unsigned int totalSize=sizeof(ulong)*CHAR_BIT*3+sizeof(unsigned short)*CHAR_BIT;
        for (int j=totalSize; j<3*totalSize; j++) {
            cout<<"\ntesting j="<<j<<endl;
            cout<<endl;
            BitStreamTestClass bitStream;
            bitStream<<i<<ii<<i;
            bitStream<<s;

            bitStream.rotateL(j);
            ostringstream result, reference;
            result<<bitStream;
            bitset<4*(sizeof(ulong)*CHAR_BIT*3+sizeof(unsigned short)*CHAR_BIT)> bitSet, bitSet2, bitSet3(s), bitSet4;
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bitSet[j]=(i>>j)&0x1;
            bitSet2=bitSet;
            for (int j=0; j<sizeof(ulong)*CHAR_BIT; j++) // has to be done this way as MINGW has problems with long sizes
                bitSet4[j]=(ii>>j)&0x1;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet4;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet4;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet4;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet4;
            bitSet=(bitSet<<sizeof(ulong)*CHAR_BIT) | bitSet2;
            bitSet=(bitSet<<sizeof(unsigned short)*CHAR_BIT) | bitSet3;

            reference<<(bitSet<<j);
            if (testBitsL(result, reference)<0) {
                bitStream.dump();
                cout<<endl;
                cout<<"testing rotateL failed"<<endl;
                return -1;
            }
        }
    }

    cout<<"all passed"<<endl;
    return 0;
}
