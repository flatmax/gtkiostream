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
#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>

#ifndef uint
typedef unsigned int uint;
#endif

#ifdef NO_ERROR
#if (NO_ERROR!=0)
#error "NO_ERROR previously defined but not equal to zero"
#endif
#else
#define NO_ERROR 0 ///< There is no error.
#endif

#ifndef DEBUG_ERROR_OFFSET
#define  DEBUG_ERROR_OFFSET -39901 ///< Define WSOLA_ERROR_OFFSET in your code (<0) to offset the WSOLA client errors to different locations.
#endif

// Values between -39 900 and -1 are reserved for the user.
#ifndef WSOLA_ERROR_OFFSET
#define  WSOLA_ERROR_OFFSET -40001 ///< Define WSOLA_ERROR_OFFSET in your code (<0) to offset the WSOLA client errors to different locations.
#endif

#ifndef SOX_ERROR_OFFSET
#define SOX_ERROR_OFFSET -40020 ///< Sox errors from -0 onwards to more negative numbers (relevant if the Sox is being used)
#endif

#ifndef JACK_ERROR_OFFSET
#define JACK_ERROR_OFFSET -40040 ///< Define JACK_ERROR_OFFSET in your code (<0) to offset the Jack client errors.
#endif

#ifndef DECOMPOSITION_ERROR_OFFSET
#define DECOMPOSITION_ERROR_OFFSET -40080 ///< Define DECOMPOSITION_ERROR_OFFSET in your code (<0) to offset the Decomposition errors.
#endif

#ifndef AUDIOMASKER_ERROR_OFFSET
#define AUDIOMASKER_ERROR_OFFSET -40090 ///< Define AUDIOMASKER_ERROR_OFFSET in your code (<0) to offset the AudioMasker errors.
#endif

#ifndef OVERLAPADD_ERROR_OFFSET
#define OVERLAPADD_ERROR_OFFSET -40110 ///< Define OVERLAPADD_ERROR_OFFSET in your code (<0) to offset the OverlapAdd errors.
#endif

#ifndef IIO_ERROR_OFFSET
#define IIO_ERROR_OFFSET -40150 ///< Define IIO_ERROR_OFFSET in your code (<0) to offset the IIO errors.
#endif

#ifndef DIRSCAN_ERROR_OFFSET
#define DIRSCAN_ERROR_OFFSET -40200 ///< Define DIRSCAN_ERROR_OFFSET in your code (<0) to offset the DirectoryScanner errors.
#endif

#ifndef THREAD_ERROR_OFFSET
#define THREAD_ERROR_OFFSET -40300 ///< Define THREAD_ERROR_OFFSET in your code (<0) to offset the DirectoryScanner errors.
#endif

#ifndef DRAGNDROP_ERROR_OFFSET
#define DRAGNDROP_ERROR_OFFSET -40400 ///< Define DRAGNDROP_ERROR_OFFSET in your code (<0) to offset the DragNDrop errors.
#endif

#ifndef ALSA_ERROR_OFFSET
#define ALSA_ERROR_OFFSET -40500 ///< Define JACK_ERROR_OFFSET in your code (<0) to offset the Jack client errors.
#endif

#ifndef IIR_ERROR_OFFSET
#define IIR_ERROR_OFFSET -40600 ///< Define JACK_ERROR_OFFSET in your code (<0) to offset the Jack client errors.
#endif

#define MAX_ERROR_OFFSET DEBUG_ERROR_OFFSET ///< The lowest debug error magnitude from gtkiostream
#define MIN_ERROR_OFFSET (IIR_ERROR_OFFSET-1000) ///< The highest debug error magnitude from gtkiostream

#include <map>
#include <string>

/** Debug information for handling errors.
This class has two modes, testing on the NDEBUG flag is NOT present, it outputs to std::cerr and has filled member variables.
This class outputs to std::cerr upon error.
This class does nothing and is completely minimal (including compile time presence) when NDEBUG is present
NOTE: Values < -39 900 are reserved for GTK IOStream.
NOTE: You should use values between -39900 <= val <= -1
*/
class Debug {
    #define MALLOC_ERROR DEBUG_ERROR_OFFSET-0 ///< Error when trying to allocate memory
protected:
    std::map<int, std::string> errors; ///< This will contain a map between error numbers and descriptive std::strings for the errors.
public:
    Debug() {
#ifndef NDEBUG
        errors[NO_ERROR]=std::string("No error. ");
        errors[MALLOC_ERROR]=std::string("Couldn't allocate the memory error. ");
#endif
    }
    virtual ~Debug() {}

#ifdef NDEBUG
    inline
#endif
    /** Displays the error header, which includes the error number and the mapped error std::string
    \param errorNum The error number to use to index the errors map for textual information
    \return The errorNum for further processing.
    */
    virtual int evaluateError(int errorNum) {
#ifndef NDEBUG
        if (errorNum!=NO_ERROR)
            fprintf(stderr, "Error : %d %s ", errorNum, errors[errorNum].c_str());
#endif
        return errorNum;
    }

#ifdef NDEBUG
    inline
#endif
    /** Appends a std::string to the error header.
    \param errorNum The error number to use to index the errors map for textual information
    \param append A std::string message to append to the standard error.
    \return The errorNum for further processing.
    */
    virtual int evaluateError(int errorNum, std::string append) {
#ifndef NDEBUG
        evaluateError(errorNum);
        fprintf(stderr, "%s", append.c_str());
#endif
        return errorNum;
    }

    /** Get the std::string matching an error.
    \param errorNum The error number to use to index the errors map for textual information
    \return The std::string matching the error number.
    */
    std::string getErrorString(int errorNum){
        if (errors.find(errorNum)==errors.end()) // if the error doesn't exist, then return an unknown error std::string.
            return std::string("Unknown Error");
        return errors[errorNum];
    }
};

#if !defined(DEBUG_OUTPUT) || !defined(DEBUG_LOCAL_OUTPUT)
class Debugger {
public:
    enum {NOTHING} endl;

    Debugger &operator<<(const char*str){
        return *this;
    }

    Debugger &operator<<(const int nothing){
        return *this;
    }

};
#include <streambuf>
#include <ostream>
template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf: public std::basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c) {
        return traits::not_eof(c); // indicate success
    }
};

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits> {
public:
    basic_onullstream(): std::basic_ios<cT, traits>(&m_sbuf), std::basic_ostream<cT, traits>(&m_sbuf) {
        //init(&m_sbuf);
    }

private:
    basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> NullStream;
static NullStream nullstream;
#endif

#ifdef DEBUG_OUTPUT
#define Debugger std::cout
// #else
// #define Debugger nullstream
#endif

#ifdef DEBUG_LOCAL_OUTPUT
#define DebuggerLocal std::cout
// #else
// #define DebuggerLocal nullstream
#endif
#endif // DEBUG_H_
