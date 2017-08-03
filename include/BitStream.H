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

#ifndef BITSTREAM_H_
#define BITSTREAM_H_

#include <vector>
#include <math.h>
#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "iobitstream not tested on big endian systems"
#endif

#include <assert.h>

/** Create bit streams using this class.

This stream is packed with the oldest entries at the beginning, newest entries added to the end. I.e. push_back. The front is the beginning and the back is the end.
The char 0x 80 for example, will be packed as 0b 1000 0000.

In general MSBs are regarded as being on the 'left' and LSBs are regarded as being on the 'right'.

If you push_back in more bits then the data type you provide, leading zeros will be added, for example : If you pack 9 bits from a char, 0x8 again, you get : 0100 0000 0

Here are some examples :

<code>
        short s=0b1111010100001010;
        short s2=0b1100110011110000;
        int i3=0x1f2f3f4f;

        bitStream.push_back(s,4); // add specific bits from a variable
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<'\n'<<endl;

        bitStream.push_back(s2,16);
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<(bitset<16>)s2<<'\n'<<endl;

        bitStream<<i3; // add all bits from a variable
        cout<<bitStream<<endl;
        cout<<(bitset<4>)s<<(bitset<16>)s2<<(bitset<32>)i3<<'\n'<<endl;

        // to get leading zeros, increase the number of bits to push_back. For example, for a char c : bitsStream.push_back(c, 10); This will add two leading zeros.
<\endcode>

Outputs the following;
<code>
1010
1010

10101100110011110000
10101100110011110000

1010110011001111000000011111001011110011111101001111
1010110011001111000000011111001011110011111101001111
<\endcode>
*/
class BitStream {
protected:
    typedef unsigned int VTYPE; ///< Use this type for the basic stream type

    /** When debugging this method can be executed to test that the mask is being generated correctly.
    \param mSize The mask size in bits
    */
    void testMask(unsigned int mSize) const {
#ifndef NDEBUG
        testMask(mSize, genMask(mSize));
#endif
    }

    /** When debugging this method can be executed to test that the mask is being generated correctly.
    \param mSize The mask size in bits
    \param compareAgainst The pre-generated mask to compare against.
    */
    void testMask(unsigned int mSize, VTYPE compareAgainst) const {
#ifndef NDEBUG
        VTYPE maskCheck=0;
        for (int j=0; j<mSize; j++)
            maskCheck|=(maskCheck<<1|1);
        if (maskCheck!=compareAgainst) {
            fprintf(stderr,"BitStream::testMask(%d) failed\n",mSize);
            fprintf(stderr,"BitStream::compare against  0x%x\n",compareAgainst);
            fprintf(stderr,"BitStream::compare maskcheck 0x%x\n",maskCheck);
            assert("BitStream : testMask is failing");
            abort();
        }
#endif
    }

private:
    std::vector<VTYPE> data; ///< The array to hold the bitstream
    int freeBits; ///< The number of free bits in the array

    /// Characters reversed. 8 bit reversals
    static const unsigned char revChars[];

    /** Pack some bits at the end of the stream.
    The input variables N least significant bits are packed into the stream.
    \param tempBits A pointer to the vector of bits to store.
    \param N The number of bits to store >0
    \param sizeOfT The size of the type of the original variable from the templated method push_back_back
    \return A reference to this object.
    */
    BitStream &push_backVType(const VTYPE *tempBits, int N, const int sizeOfT);

    /** The number of bits in the base data type.
    \return The number of bits in the base data type.
    */
    unsigned int VTYPEBits() const {
        return sizeof(VTYPE)*CHAR_BIT;
    }

    /** The number of bits used (taken) in the last word.
    \return The number of bits used in the last word.
    */
    unsigned int takenBits() const {
        return (size()>0) ? VTYPEBits()-freeBits : 0;
    }

    /** Get the first M bits from a word. Returns these bits right shifted so to the LSB location.
    \param M The number of bits to get.
    \param bits The bits to get the M bits from.
    \return The M bits from the MSB location in the LSB location
    */
    VTYPE maskBitsToRight(const unsigned int M, const VTYPE &bits) const {
        if (M>sizeof(VTYPE)*CHAR_BIT)
            assert("BitStream::maskBitsToRight : you can't shift right more bits then the word size. M > sizeof(VTYPE)*8");
        return ((genMask(M)<<(VTYPEBits()-M))&bits)>>(VTYPEBits()-M);
    }

    /** Get M bits from a word, with an offset from the MSB. Returns these bits right shifted so to the LSB location.
    \param M The number of bits to get.
    \param offset The number of bits from the MSB in to get the bits from.
    \param bits The bits to get the M bits from.
    \return The M bits from the MSB location in the LSB location
    */
    VTYPE maskOffsetBitsToRight(const unsigned int M, const unsigned int offset, const VTYPE &bits) const {
        if (M+offset>sizeof(VTYPE)*CHAR_BIT)
            assert("BitStream::maskOffsetBitsToRight : you can't shift right more bits then the word size. M+offset > sizeof(VTYPE)*CHAR_BIT");
        return ((genMask(M)<<(VTYPEBits()-M-offset))&bits)>>(VTYPEBits()-M-offset);
    }

    /** Get the last M bits from a word. Returns these bits left shifted so to the MSB location.
    \param M The number of bits to get.
    \param bits The bits to get the M bits from.
    \return The M bits from the LSB location in the MSB location
    */
    VTYPE maskBitsToLeft(const unsigned int M, const VTYPE bits) const {
        if (M>sizeof(VTYPE)*CHAR_BIT)
            assert("BitStream::maskBitsToLeft : you can't shift left more bits then the word size. M > sizeof(VTYPE)*8");
        return (genMask(M)&bits)<<(VTYPEBits()-M);
    }

    /** Reverse the provided bits in the character array.

    This bit reverse method uses an 8 bit hash lookup table to reverse the bits in a very fast manner.
    \param bits the variable to bit reverse.
    \param N The number of char to reverse.
    */
    void reverseBits(unsigned char *bits, const unsigned int N) const {
        // switch each char ...
        for (int i=0; i<N/2; i++) {
            char lastChar=revChars[bits[N-1-i]];
            bits[N-1-i]=revChars[bits[i]];
            bits[i]=lastChar;
        }
        if (N%2) // the case where the number of char is odd
            bits[N/2]=revChars[bits[N/2]];
    }

    /** Shift left by N sub-word length bits, returning the left most N bits.
    \param firstWord The first word to be shifted left, its left most bits are returned in the LSB location
    \param lastWord The last word to be shifted left, its right most bits are zero upon return.
    \return firstWord's left most N bits (MSBs) shifted to the right (LSB) location. The lastWord's N LSBs are zero.
    */
    VTYPE shiftLeftSubword(std::vector<VTYPE>::iterator firstWord, std::vector<VTYPE>::iterator lastWord, const unsigned int N);

    /** Shift right by N sub-word length bits, returning the right most (lest significant) N bits shifted to the MSB location.
    \param firstWord The first word to be shifted right, its left most bits are zeroed on return.
    \param lastWord The last word to be shifted left, its right most bits are returned in the MSB location
    \return lastWord's right most N bits (LSBs). The firstWord's N MSBs are zero.
    */
    VTYPE shiftRightSubword(std::vector<VTYPE>::iterator firstWord, std::vector<VTYPE>::iterator lastWord, const unsigned int N);

protected:
    /** Generate an M bit mask.
    \return VTYPE with the first M bits set.
    */
#ifndef __MINGW32__
    VTYPE genMask(int M) const {
#ifndef NDEBUG // if debugging, test the mask by default
        testMask(M, (VTYPE)((unsigned long )pow(2.,(float)M)-1));
#endif
        return (VTYPE)((unsigned long )pow(2.,(float)M)-1);
    }
#else // mingw seems to have trouble with the unsigned long
    VTYPE genMask(int M) const {
#ifndef NDEBUG // if debugging, test the mask by default
        testMask(M, (VTYPE)((unsigned long long )pow(2.,(float)M)-1));
#endif
        return (VTYPE)((unsigned long long)pow(2.,(float)M)-1);
    }
#endif

public:

    BitStream(); ///< Constructor
    virtual ~BitStream(); ///< Destructor

    /** How many bits in the stream.
    \return How many bits in the stream.
    */
    std::vector<VTYPE>::size_type size() const;

    /** How many bytes in the stream.
    \return How many bytes in the stream, can be fractional when some bits are free.
    */
    float byteSize() const;

    /** Reverse all of the provided bits.

    This bit reverse method uses an 8 bit hash lookup table to reverse the bits in a very fast manner.
    \param bits the variable to bit reverse. All of the bits are reversed.
    \tparam T The type of input bit variable
    */
    template<typename T>
    T reverseBits(T bits) const {
        unsigned int N=sizeof(T)*CHAR_BIT;
        return reverseBits(bits, N);
    }

    /** Reverse a subset of the provided bits (possibly all of the bits).

    This bit reverse method uses an 8 bit hash lookup table to reverse the bits in a very fast manner.
    \param bits the variable to bit reverse. Some or all of the bits are reversed, depending on N, the number of bits to reverse requested.
    \param N The number of bits >0 to store.
    \tparam T The type of input bit variable
    */
    template<typename T>
    T reverseBits(T bits, unsigned int N) const {
        unsigned int charCnt=sizeof(T);
        if (N>charCnt*CHAR_BIT) N=charCnt*CHAR_BIT;
        reverseBits((unsigned char *)&bits, charCnt);
        bits>>=charCnt*CHAR_BIT-N; // roll the word so that there aren't any empty spaces in the LSB
        return bits;
    }

    /** Add a simple type to the bit stream.
    \param bits The data to add to the stream.
    \tparam T The type of the data.
    \return A reference to this class.
    */
    template<typename T>
    BitStream &operator<<(T bits) {
        int N=sizeof(T)*CHAR_BIT;
        return push_back(bits, N);
    }

    /** Pack some bits at the end of the stream.
    The input variables N least significant bits are packed into the stream.
    \param bits the variable to get bits from in order to fill the bit stream
    \param N The number of bits >0 to store.
    \tparam T The type of input bit variable
    \return A reference to this BitStream.
    */
    template<typename T>
    BitStream &push_back(const T bits, const int N) {
        if (N>0) {
            VTYPE *tempBits=(VTYPE*)&bits;
            return push_backVType(tempBits, N, sizeof(T));
        }
        return *this;
    }

    /** Pop N bits from the end of the stream.

    If the requested bit count (N) is larger then the provided return type, then only sizeof(T)*8 bits are poped from the back.
    \param N The number of bits to pop.
    \tparam T The type of the data to return.
    \return N bits from the back of the stream if sizeof(T)*8 >= N. If sizeof(T)*8 < N, return bits from the end, where the returned variable (type T) contains the sizeof(T)*8 bits from the end - the other bits (N-sizeof(T)*8) are lost.
    */
    template<typename T>
    T pop_back(const unsigned int N) {
        unsigned int NN=N;
        T bits=0;

        if (NN && (size()>0)) {
            //if (NN>sizeof(T)*CHAR_BIT) NN=sizeof(T)*CHAR_BIT;
            if (takenBits()>=NN) { // if we have enough bits loaded in the last word, the acquire from there
                bits=maskBitsToRight(takenBits(), data[data.size()-1])&genMask(NN);
                freeBits+=NN;
                if (freeBits==VTYPEBits()) { // if we have an completely empty last word, then remove it
                    data.resize(data.size()-1);
                    freeBits-=VTYPEBits();
                }
            } else { // handle the case where the takenBits are less then the number requested
                unsigned int tb=takenBits();
                NN-=tb; // start with the available bits
                bits=pop_back<T>(tb);
                T newBits=pop_back<T>(NN);
                bits|=(genMask(NN)&newBits)<<tb;
            }
        }
        return bits;
    }

    /** Pop N bits from the front of the stream.

    If the requested bit count (N) is larger then the provided return type, then only sizeof(T)*8 bits are poped from the front.
    \param N The number of bits to pop.
    \tparam T The type of the data to return.
    \return N bits from the front of the stream if sizeof(T)*8 >= N. If sizeof(T)*8 < N, return bits from the front, where the returned variable (type T) contains the sizeof(T)*8 bits from the front - the other bits (N-sizeof(T)*8) are lost.
    */
    template<typename T>
    T pop_front(const unsigned int N) {
        rotateL(N); // rotate so that the required bits are at the end of the strea,
        return pop_back<T>(N); // pop the back of the stream returning those bits
    }

    /** Rotate the stream to the left, left most bits are rotated to the right as required.
    \param N The number of bits to rotate left by.
    \return A reference to this class.
    */
    BitStream &rotateL(const unsigned int N);

    /** Rotate the stream to the right, right most bits are rotated to the right as required.
    \param N The number of bits to rotate left by.
    \return A reference to this class.
    */
    BitStream &rotateR(const unsigned int N);

    /** Output the bit sequence in hexadecimal format.
    \param stream The ostream to output to.
    \return The ostream which was output to.
    */
    std::ostream& hexDump(std::ostream& stream);

    /** Overload the ostream operator for printing.
    This method outputs the string representation of the bit stream.
    \param stream The ostream to output the bitstream to.
    \param iob The iobitstream to output.
    \return The ostream with the string rep. of the bitstream output.
    */
    friend std::ostream& operator<<(std::ostream& stream, const BitStream& bitStream);

    /** Clear the BitStream to an empty state.
    */
    void clear();

    /** Resize the underlying data structure to have enough memory to store N bits.
    \param N The number of bits to store.
    \return NO_ERROR on success, or the resulting error number on failure.
    */
    int reserve(std::vector<VTYPE>::size_type N);

    /** Return the capacity (in bits) of the bitstream
    \return the capacity in bit
    */
    std::vector<VTYPE>::size_type capacity() const;

    /** print out with spaces between vector words ... good for debugging.
    */
    void dump(void);

    /** print out with spaces between vector words ... good for debugging.
    The last word is printed including the freeBits
    */
    void dumpHex(void);

    /** Get bits from the stream. Location starting i, the number of bits is sizeof(T)*CHAR_BIT.
    \param i The location to retrieve from.
    \param N The number of bits to retrieve.
    \return The bits from the stream starting at location i, of length N located in the LSB of the return variable
    \tparam T The type to return the bits in.
    */
    template<typename T>
    T getBits(std::vector<VTYPE>::size_type i, unsigned int N) const {
        if ((i+N)>size()) // if none of the requested bits are available, then assert
            assert("BitStream::operator[] : you requested an index which is out of range. The bitstream is smaller then your starting point and the size of your requested type.");
        unsigned int whichWord=i/VTYPEBits(); // the word to extract the data from.
        unsigned int wordLoc=i-whichWord*VTYPEBits(); // the MSB to get from the word
        unsigned int M=std::min<unsigned int>(N,VTYPEBits()-wordLoc);
        T ret=(T)0.;
        while (N>0) { // indicate that some bits were retrieved and keep going until complete
            ret|=maskOffsetBitsToRight(M, wordLoc, data[whichWord++]); // next word's data from the (possibly offset) MSB location shifted to the LSB location
            M=std::min<unsigned int>(N-=M,VTYPEBits()); // find how many to retrieve from the next word
            ret<<=M; // ensure there is enough room in T for the left over bits.
            wordLoc=0;
        }
        return ret;
    }


    /** Get bits from the stream. Location starting i, the number of bits is sizeof(T)*CHAR_BIT.
    \param i The location to retrieve from.
    \return The bits from the stream starting at location i, of size sizeof(T)*CHAR_BIT
    \tparam T The type to return the bits in.
    */
    template<typename T>
    T operator[](std::vector<VTYPE>::size_type i) const {
        return getBits<T>(i, sizeof(T)*CHAR_BIT);
    }

    /** Search through the bits of the contained data.
    \param toFind The bits to find in this BitStream
    \param N the number of LSBs to use from the variable toFind.
    \return A vector of indexes where toFind exists in the stream.
    */
    template<typename T>
    std::vector<std::vector<VTYPE>::size_type> find(T toFind, const unsigned int N) const {
        BitStream toFindRef; // construct a vector to use for searching
        toFindRef.push_back(toFind, N);
        return find(toFindRef, N);
    }

    /** Search through the bits of the contained data.
    \param toFind The bitStream to find in this BitStream
    \param N the number of bits to use from the variable toFind.
    \return A vector of indexes where toFind exists in the stream.
    */
    std::vector<std::vector<VTYPE>::size_type> find(BitStream toFind, const unsigned int N) const;
};

#endif // BITSTREAM_H_
