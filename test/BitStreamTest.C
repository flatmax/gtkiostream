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

int main(int argc, char *argv[]) {
    {
        BitStreamTestClass bitStream;
        if (bitStream.basicTypeSize()*CHAR_BIT!=32) {
            cerr<<argv[0]<<" BitStream VTYPE size != 32 bits - this test is not setup for that bit count"<<endl;
            abort();
        }

    }
    {
        const int i=17;
        unsigned short c=0x5c5c;
        BitStreamTestClass bitStream2;
        bitStream2<<c;
        bitStream2.push_back(c, 5);
        bitStream2.push_back(c, i);
        bitStream2.hexDump(cout);
        cout<<endl;
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<bitset<16>(c)<<bitset<5>(c)<<bitset<i>(c)<<endl;
        if (testBits(result, reference)<0) return -1;
    }
    {
        // test inputs which are correctly sized and also the case where more bits are specified then are given.
        BitStreamTestClass bitStream;

        int ss=0b1111010100001010;
        short s=(short int)ss;
        short s2=0b1100110011110000;
        int i3=0x1f2f3f4f;

        cout<<"standard test "<<endl;

        bitStream.push_back(s,4);
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<'\n'<<endl;
        bitStream.push_back(s2,16);
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<(bitset<16>)s2<<'\n'<<endl;
        bitStream<<i3;
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<(bitset<16>)s2<<(bitset<32>)i3<<'\n'<<endl;
        bitStream.push_back(s,1);
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<(bitset<16>)s2<<(bitset<32>)i3<<(bitset<1>)s<<'\n'<<endl;
        bitStream.push_back(s,17);
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<(bitset<16>)s2<<(bitset<32>)i3<<(bitset<1>)s<<(bitset<17>)ss<<'\n'<<endl;
        ostringstream result, reference;
        result<<bitStream;
        reference<<(bitset<4>)s<<(bitset<16>)s2<<(bitset<32>)i3<<(bitset<1>)s<<(bitset<17>)ss;
        if (testBits(result, reference)<0) return -1;
    }

    {
        // test inputs where more bits are specified then are given.
        BitStreamTestClass bitStream;

        char c=0x5c;
        int s=0x5c;
        cout<<"extended test "<<endl;

        bitStream.push_back(c,9);
        cout<<bitStream<<endl;
        cout<<(bitset<9>)s<<'\n'<<endl;
        bitStream.push_back(c,16);
        cout<<bitStream<<endl;
        cout<<(bitset<9>)s<<(bitset<16>)s<<'\n'<<endl;
        bitStream.push_back(c,18);
        cout<<bitStream<<endl;
        cout<<(bitset<9>)s<<(bitset<16>)s<<(bitset<18>)s<<'\n'<<endl;
        bitStream.push_back(c,25);
        cout<<bitStream<<endl;
        cout<<(bitset<9>)s<<(bitset<16>)s<<(bitset<18>)s<<(bitset<25>)s<<'\n'<<endl;
        bitStream.push_back(c,32);
        cout<<bitStream<<endl;
        cout<<(bitset<9>)s<<(bitset<16>)s<<(bitset<18>)s<<(bitset<25>)s<<(bitset<32>)s<<'\n'<<endl;
        ostringstream result, reference;
        result<<bitStream;
        reference<<(bitset<9>)s<<(bitset<16>)s<<(bitset<18>)s<<(bitset<25>)s<<(bitset<32>)s;
        if (testBits(result, reference)<0) return -1;
    }

    cout<<endl;
    cout<<endl;
    long l=0xfedcba9876543210;
    //for (int i=0; i<88; i++)
    {
        const int i=0;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l, i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<bitset<i>(l)<<endl;
        if (testBits(result, reference)<0) return -1;
    }

    cout<<"large type test "<<endl;
    cout<<endl;
    cout<<endl;

    {
        const int i=1;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=2;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=3;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=4;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=5;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=6;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=7;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=8;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=9;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=10;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=11;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=12;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=13;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=14;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=15;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=16;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=17;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=18;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=19;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=20;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=21;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=22;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=23;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=24;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=25;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=26;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=27;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=28;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=29;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=30;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=31;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=32;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=33;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=34;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=35;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=36;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=37;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=38;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=39;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=40;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=41;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=42;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=43;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=44;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=45;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=46;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=47;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=48;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=49;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=50;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=51;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=52;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=53;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=54;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=55;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=56;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=57;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=58;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=59;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=60;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=61;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=62;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=63;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    {
        const int i=64;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }

    {
        const int i=65;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }

    {
        const int i=66;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }

    {
        const int i=128;
        BitStreamTestClass bitStream2;
        bitStream2.push_back(l,i);
        ostringstream result, reference;
        result<<bitStream2<<endl;
        reference<<(bitset<i>)l<<endl;
        cout<<"result : "<<result.str()<<endl;
        cout<<"ref    : "<<reference.str()<<endl;
        if (result.str().compare(reference.str())!=0) {
            cout<<"ERROR : result and reference don't align"<<endl;
            return -1;
        }
    }
    cout<<"all passed"<<endl;
    return 0;
}
