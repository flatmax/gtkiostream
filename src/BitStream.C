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

/*
Author: Matt Flax <flatmax@flatmax.org>
Date: 2013.08.23
*/

#include "BitStream.H"
#include <bitset>
#include <Debug.H>

const unsigned char BitStream::revChars[]= {0, 128,  64, 192,  32, 160,  96, 224,  16, 144,  80, 208,  48, 176, 112, 240,   8, 136,  72, 200,  40, 168, 104, 232,  24, 152,  88, 216,  56, 184, 120, 248,   4, 132,  68, 196,  36, 164, 100, 228,  20, 148,  84, 212,  52, 180, 116, 244,  12, 140,  76, 204,  44, 172, 108, 236,  28, 156,  92, 220,  60, 188, 124, 252,   2, 130,  66, 194,  34, 162,  98, 226,  18, 146,  82, 210,  50, 178, 114, 242,  10, 138,  74, 202,  42, 170, 106, 234,  26, 154,  90, 218,  58, 186, 122, 250,   6, 134,  70, 198,  38, 166, 102, 230,  22, 150,  86, 214,  54, 182, 118, 246,  14, 142,  78, 206,  46, 174, 110, 238,  30, 158,  94, 222,  62, 190, 126, 254,   1, 129,  65, 193,  33, 161,  97, 225,  17, 145,  81, 209,  49, 177, 113, 241,   9, 137,  73, 201,  41, 169, 105, 233,  25, 153,  89, 217,  57, 185, 121, 249,   5, 133,  69, 197,  37, 165, 101, 229,  21, 149,  85, 213,  53, 181, 117, 245,  13, 141,  77, 205,  45, 173, 109, 237,  29, 157,  93, 221,  61, 189, 125, 253,   3, 131,  67, 195,  35, 163,  99, 227,  19, 147,  83, 211,  51, 179, 115, 243,  11, 139,  75, 203,  43, 171, 107, 235,  27, 155,  91, 219,  59, 187, 123, 251,   7, 135,  71, 199,  39, 167, 103, 231,  23, 151,  87, 215,  55, 183, 119, 247,  15, 143,  79, 207,  47, 175, 111, 239,  31, 159,  95, 223,  63, 191, 127, 255};

BitStream::BitStream() {
    freeBits=0; // start with empty, no bits free
    // check the extreme mask generation of all the bits
    testMask(VTYPEBits());
}

BitStream::~BitStream() {
    //dtor
}

std::vector<BitStream::VTYPE>::size_type BitStream::size() const {
    return data.size()*sizeof(data[0])*CHAR_BIT-freeBits;
}

float BitStream::byteSize() const {
    return (float)size()/(float)CHAR_BIT;
}

BitStream &BitStream::push_backVType(const VTYPE *tempBits, int N, const int sizeOfT) {
//    std::cout<<"BitStream::push_backVType N "<<N<<" sizeOfT "<<sizeOfT<<std::endl;
    if (sizeOfT>sizeof(VTYPE)) { // first handle the case where the bits type is larger in size then the VTYPE
        if (fmod(sizeOfT,sizeof(VTYPE))>0.)
            assert("BitStream::push_back sizeof(T)/sizeof(VTYPE) should be zero : This means that your specified type is larger then the BitStream base type and is not an integer multiple of the BitStream base type. This shouldn't be possible.");
        for (int i=0; i<sizeOfT/sizeof(VTYPE); i++) { // break up the larger type into multiples of VTYPE
            int NN=-((sizeOfT*CHAR_BIT-N)-(i+1)*sizeof(VTYPE)*CHAR_BIT);
            if (NN>0) {
                push_back(tempBits[sizeOfT/sizeof(VTYPE)-i-1], NN);
                N-=NN;
            }
        }
    }
    if (N>sizeOfT*CHAR_BIT) { // handle the case where more bits are specified then exist in the given type.
        push_back((VTYPE)0,N-sizeOfT*CHAR_BIT);
        N-=N-sizeOfT*CHAR_BIT;
    }
    if (N<=freeBits) { // if we have enough free bits to pack these new bits, then simply do so.
        VTYPE mask=(VTYPE)pow(2.,(float)freeBits)-1;
        data[data.size()-1]|=((*tempBits)<<(freeBits-N))&mask;
        freeBits-=N;
    } else { // if there aren't enough free bits, create some ...
        if (data.size()) {
            VTYPE mask=(VTYPE)pow(2.,(float)freeBits)-1;
            data[data.size()-1]|=(VTYPE)(*tempBits>>(N-=freeBits))&mask;
        }
        data.push_back((VTYPE)0.);
        freeBits=sizeof(VTYPE)*CHAR_BIT;
        return push_backVType(tempBits, N, sizeOfT);
    }
    return *this;
}

BitStream::VTYPE BitStream::shiftLeftSubword(std::vector<VTYPE>::iterator firstWord, std::vector<VTYPE>::iterator lastWord, const unsigned int N) {
    if (N==0)
        return (VTYPE)0;
    if (N>=VTYPEBits())
        assert("BitStream::shiftLeftSubword : You are trying to shift left too many bits > sizeof(VTYPE)*8\n");

    VTYPE bitReg=maskBitsToRight(N, *firstWord); // bitReg holds N bits from the MSBs of the first word in the LSB location
    VTYPE notMask=~genMask(N); // the last N LSBs are zeros, the rest are ones
    for (; firstWord!=lastWord; ++firstWord) {
        VTYPE nextBits=maskBitsToRight(N, *(firstWord+1)); // get the first N bits from the next word.
        *firstWord=((*firstWord)<<N)&notMask; // rotate the current word and clear the N LSBs
        *firstWord|=nextBits; // inject the N LSBs from the next word
    }
    *lastWord=((*lastWord)<<N)&notMask; // rotate the last word and clear the N LSBs
    return bitReg;
}

BitStream &BitStream::rotateL(const unsigned int N) {
    if (data.size()) { // only rotate if there is data to rotate.
        unsigned int cycleCnt=N/size(); // remove any full cycles
        // find how many non-whole word bits require rotation
        int wholeWordN=N-freeBits-cycleCnt*size(); // whole words occur once freeBits have been shifted.
        unsigned int wholeWords = (wholeWordN<0)?0:wholeWordN/VTYPEBits(); // find the number of whole words, guard the case where freeBits > N-cycleCnt*size(), which means no whole words, rather then a negative number
        unsigned int M = N-cycleCnt*size(); // set the number of bits to shift, remove any full cycles

        if (wholeWords) { // this will shift by wholeWords
            rotate(data.begin(), data.begin()+wholeWords, data.end()); // rotate the wholeWords
            if (freeBits) { // account for any freeBits in the last word
                VTYPE bitReg=shiftLeftSubword(data.end()-(wholeWords+1), data.end()-1, freeBits);
                data[data.size()-(wholeWords+1)]=(data[data.size()-(wholeWords+1)]&genMask(freeBits))|((bitReg<<freeBits)&~genMask(freeBits)); // position the first shiftBits to be in the correct last location and add to the last word
            }
            M-=wholeWords*VTYPEBits(); // remove the number of bits shifted
        }
        while (M>takenBits()) { // if we are trying to shift data which will roll off the end and be lost, then shift twice so as not to lose data
            unsigned int shiftBits=std::min<unsigned int>(M, takenBits());
            M-=shiftBits;
            rotateL(shiftBits);
        }
        if (M) { // if we still have something left to shift, it will be less then a whole word
            VTYPE bitReg=shiftLeftSubword(data.begin(), data.end()-1, M);
            data[data.size()-1]=(data[data.size()-1]&~genMask(freeBits+M))|(bitReg<<freeBits); // position the first M to be in the correct last location and add to the last word
        }
    }
    return *this;
}

BitStream::VTYPE BitStream::shiftRightSubword(std::vector<VTYPE>::iterator firstWord, std::vector<VTYPE>::iterator lastWord, const unsigned int N) {
    if (N==0)
        return (VTYPE)0;
    if (N>=VTYPEBits())
        assert("BitStream::shiftRightSubword : You are trying to shift right too many bits > sizeof(VTYPE)*CHAR_BIT\n");
    if (N>=takenBits())
        assert("BitStream::shiftRightSubword : You are trying to shift more bits then are present, ensure N <= takenBits()\n");

    VTYPE bitReg=maskOffsetBitsToRight(N, takenBits()-N,*lastWord)<<(VTYPEBits()-N);
    VTYPE mask=genMask(VTYPEBits()-N); // the last N LSBs are zeros, the rest are ones
    for (; lastWord!=firstWord; --lastWord) {
        VTYPE nextBits=maskBitsToLeft(N, *(lastWord-1)); // get the least significant N bits from the next word in the MSB location
        *lastWord=((*lastWord)>>N)&mask; // rotate the current word and clear the N MSBs
        *lastWord|=nextBits; // inject the N LSBs from the next word
    }
    *firstWord=((*firstWord)>>N)&mask; // rotate the first word and clear the N MSBs
    return bitReg;
}

BitStream &BitStream::rotateR(const unsigned int N) {
    if (data.size()) { // only rotate if there is data to rotate.
        unsigned int cycleCnt=N/size(); // remove any full cycles
        unsigned int M = N-cycleCnt*size(); // set the number of bits to shift, remove any full cycles
        // find how many non-whole word bits require rotation
        int wholeWordN=N-freeBits-cycleCnt*size(); // whole words occur once freeBits have been shifted.
        unsigned int wholeWords = (wholeWordN<0)?0:wholeWordN/VTYPEBits(); // find the number of whole words, guard the case where freeBits > N-cycleCnt*size(), which means no whole words, rather then a negative number

        if (wholeWords) { // this will shift by wholeWords
            rotate(data.begin(), data.end()-wholeWords, data.end()); // rotate the wholeWords
            if (freeBits) { // account for any freeBits in the last word
                VTYPE bitReg=shiftRightSubword(data.begin(), data.begin()+wholeWords-1, freeBits);
                data[0]=(data[0]&genMask(takenBits()))|maskBitsToLeft(freeBits, data[data.size()-1]);
            }
            M-=wholeWords*VTYPEBits(); // remove the number of bits shifted
        }
        while (M>takenBits()) { // if we are trying to shift data which will roll off the end and be lost, then shift twice so as not to loose data
            // TODO : this is inefficient if there is only 1 or a few bits stored in the last word, as it will be executed many times
            unsigned int shiftBits=std::min<unsigned int>(M, takenBits());
            M-=shiftBits;
            rotateR(shiftBits);
        }
        if (M) { // if we still have something left to shift, it will be less then a whole word
            VTYPE bitReg=shiftRightSubword(data.begin(), data.end()-1, M); // shift right
            data[0]=(data[0]&genMask(VTYPEBits()-M))|bitReg; // add the last M bits to the beginning of the first word
        }
    }
}

std::ostream& BitStream::hexDump(std::ostream& stream) {
    stream<<std::hex;
    for (std::vector<VTYPE>::iterator word=data.begin(); word!=data.end(); ++word)
        stream<<*word;
    stream<<std::dec;
    return stream;
}

void BitStream::clear() {
    data.clear();
    freeBits=0;
}

int BitStream::reserve(std::vector<BitStream::VTYPE>::size_type N) {
    if (capacity()<N)
#ifndef __MINGW32__
        data.reserve((unsigned long)ceil((double)N/(double)VTYPEBits()));
#else
        data.reserve((unsigned long long)ceil((double)N/(double)VTYPEBits()));
#endif
    if (capacity()<N)
        return Debug().evaluateError(MALLOC_ERROR);
    return NO_ERROR;
}

std::vector<BitStream::VTYPE>::size_type BitStream::capacity() const {
    return data.capacity()*VTYPEBits();
}


std::ostream& operator<<(std::ostream& stream, const BitStream& bitStream) {
    const int N=sizeof(BitStream::VTYPE)*CHAR_BIT; // work with char
    const int M=(int)fmod(bitStream.size(),N); // the initial bit count short of N
    for (unsigned int i=0; i<bitStream.size()/N; i++)
        stream<<std::bitset<N>(bitStream.operator[]<BitStream::VTYPE>(i*N));
    if (M) {
        std::bitset<N> bits(std::bitset<N>(bitStream.operator[]<BitStream::VTYPE>(bitStream.size()-M)));
        bits>>=N-M;
        for (int i=0; i<M; i++)
            stream<<bits[M-i-1];
    }
    return stream;
}

void BitStream::dump(void) {
    const int N=sizeof(BitStream::VTYPE)*CHAR_BIT; // work with char
    const int M=(int)fmod(size(),N); // the initial bit count short of N
    for (int i=0; i<size()/N; i++)
        printf("%lu ",((std::bitset<N>)data[i]).to_ulong());
    if (M) {
        std::bitset<N> bits(data[size()/N]);
        bits>>=N-M;
        for (int i=0; i<M; i++)
            printf("%d", bits[M-i-1]==1);
    }
    printf("\n");
}

void BitStream::dumpHex(void) {
    const int N=sizeof(BitStream::VTYPE)*CHAR_BIT; // work with char
    const int M=(int)fmod(size(),N); // the initial bit count short of N
    for (int i=0; i<size()/N; i++)
        printf("%x ",data[i]);
    if (M) {
        printf("%x ",data[size()/N]);

    }
    printf("\n");
}

std::vector<std::vector<BitStream::VTYPE>::size_type> BitStream::find(BitStream toFind, const unsigned int N) const {
    std::vector<std::vector<VTYPE>::size_type> indexes; // the vector of matching indexes
    if (N>0 && toFind.size()>0)
        for (int j=0; j<size()-toFind.size(); j++) { // step through each of the vector's elements
            unsigned int M=N;
            while (M>0) {
                // go through all of the bits from this location, comparing for equality
                unsigned int K=std::min<unsigned int>(VTYPEBits(), M);
                if (getBits<VTYPE>(j+N-M, K)!=toFind.getBits<VTYPE>(N-M, K))
                    break;
                M-=K;
            }
            if (M<=0) // if we have found one, add it to the list
                indexes.push_back(j);
        }
    return indexes;
}
