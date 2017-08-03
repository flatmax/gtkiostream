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

typedef unsigned int VTYPELOCAL; ///< Use this type for the basic stream type

int testIndexes(vector<std::vector<VTYPELOCAL>::size_type> &manualMatches, vector<std::vector<VTYPELOCAL>::size_type> &matches){
    cout<<"        matches size="<<matches.size()<<endl;
    for (int i=0; i<matches.size(); i++)
        cout<<matches[i]<<", ";
    cout<<endl;
    cout<<" maunal matches size="<<manualMatches.size()<<endl;
    for (int i=0; i<manualMatches.size(); i++)
        cout<<manualMatches[i]<<", ";
    cout<<endl;
    if (matches.size()==0 && manualMatches.size()==0)
        return 0;
    if (matches!=manualMatches){
        return -1;
    }
    return 0;
}

unsigned long genMask(int M) {
    return (unsigned long )pow(2.,(float)M)-1;
}


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
        cout<<"testing pop_front for an int size bitStream"<<endl;
        unsigned int i=0x12345678;
        BitStreamTestClass bitStream;
        bitStream<<i;
        int j=0;
        int alreadyPoped=0;
        while (j<=bitStream.size()) {
            cout<<"j = "<<j<<endl;
            unsigned long ret=bitStream.pop_front<unsigned long>(j);
            unsigned long mask=genMask(j)<<((sizeof(i)*CHAR_BIT-j)-alreadyPoped);
            cout<<" bitStream "<<hex<<bitStream<<dec<<endl;
            cout<<"i    "<<hex<<i<<dec<<endl;
            cout<<"mask "<<hex<<mask<<dec<<endl;
            unsigned long expected=(i&mask)>>((sizeof(i)*CHAR_BIT-j)-alreadyPoped);
            if (ret!=expected) {
                cout<<"Error whilst extracting "<<j<<" bits"<<endl;
                cout<<"Already poped "<<alreadyPoped<<" bits from the stream"<<endl;
                cout<<"Expected "<<hex<<expected<<dec<<'\t';
                cout<<"Got "<<hex<<ret<<dec<<'\n';
            }
            alreadyPoped+=j;
            j++;
        }
    }
    {
        const int N=1;
        cout<<"testing pop_front for a long int size bitStream"<<endl;
        unsigned long i=0x123456789abcdef1;
        BitStreamTestClass bitStream;
        bitStream<<i;
        int j=0;
        int alreadyPoped=0;
        while (j<=bitStream.size()) {
            cout<<"j = "<<j<<endl;
            unsigned long ret=bitStream.pop_front<unsigned long>(j);
            unsigned long mask=genMask(j)<<((sizeof(i)*CHAR_BIT-j)-alreadyPoped);
            cout<<" bitStream "<<hex<<bitStream<<dec<<endl;
            cout<<"i    "<<hex<<i<<dec<<endl;
            cout<<"mask "<<hex<<mask<<dec<<endl;
            unsigned long expected=(i&mask)>>((sizeof(i)*CHAR_BIT-j)-alreadyPoped);
            if (ret!=expected) {
                cout<<"Error whilst extracting "<<j<<" bits"<<endl;
                cout<<"Already poped "<<alreadyPoped<<" bits from the stream"<<endl;
                cout<<"Expected "<<hex<<expected<<dec<<'\t';
                cout<<"Got "<<hex<<ret<<dec<<'\n';
            }
            alreadyPoped+=j;
            j++;
        }
    }
    {
        cout<<"testing [] for a long bitStream"<<endl;
        unsigned long ul=0x123456789abcdef1;
        BitStreamTestClass bitStream;
        bitStream<<ul<<ul<<ul;
        const int M=sizeof(long)*CHAR_BIT;
        const int N=3*M;
        for (int j=0; j<sizeof(long)*CHAR_BIT; j++) {
            for (int i=0; i<sizeof(long)*CHAR_BIT; i++) {
                    cout<<"\ni "<<i<<" j "<<j<<'\n'<<endl;
                bitset<N> bitSet(ul);
                bitSet=(bitSet<<M) | bitset<N>(ul);
                bitSet=(bitSet<<M) | bitset<N>(ul);
                ostringstream result, reference;
                reference<<(bitSet<<j);

                BitStream testBitStream;
                testBitStream.push_back(bitStream.getBits<long>(j, i), i);
                cout<<"results loading"<<endl;
                result<<testBitStream;
                if (testBits(result, reference)<0) {
                    cout<<"testing getBits failed"<<endl;
                    return -1;
                }
            }
        }
    }
    {
        cout<<"testing find for a long bitStream"<<endl;
        unsigned long ul=0x123456789abcdef1;
        BitStreamTestClass bitStream;
        bitStream<<ul<<ul<<ul;
        const int M=sizeof(long)*CHAR_BIT;
        int N=1;
        while (N<sizeof(long)*CHAR_BIT){
            cout<<"N "<<N<<endl;
            unsigned long searchTarget=bitStream.getBits<unsigned long>(0, N);
            vector<std::vector<VTYPELOCAL>::size_type> manualMatches;
            for (int i=0; i<bitStream.size()-N; i++){ // search through each possible combination
                if (bitStream.getBits<unsigned long>(i, N)==searchTarget)
                    manualMatches.push_back(i);
            }
            vector<std::vector<VTYPELOCAL>::size_type> matches=bitStream.find(searchTarget, N);
            if (testIndexes(manualMatches, matches)<0) {
                cout<<"testing find failed"<<endl;
                return -1;
            }
            N++;
        }
    }

    cout<<"all passed"<<endl;
    return 0;
}

