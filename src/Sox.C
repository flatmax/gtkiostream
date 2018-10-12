/* This file was contributed by Deqx.com
   Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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

#if !defined(_MSC_VER) && !defined(__CYGWIN__)
#include "Sox.H"
#else
// Note : Microsoft doesn't understand the different between upper and lower case in file names.
// on microsoft, you have to manually rename Sox.H to SoxWindows.H
#include "SoxWindows.H"
#endif

#include <fstream>
#include <sox.h>
#include <stdio.h>
//#include <valarray>

using namespace Eigen;

template<typename FP_TYPE_>
Sox<FP_TYPE_>::Sox() {
    sox_globals.output_message_handler = output_message; // setup the message handler
    sox_globals.verbosity = 1;
    // we aren't using effects here so don't init.
    // assert(sox_init() == SOX_SUCCESS); // init the sox library effects
    in=out=NULL;
}

template<typename FP_TYPE_>
Sox<FP_TYPE_>::~Sox() {
    close(in); // ensure all memory is destroyed as required
    close(out);
    // sox_quit(); // not initted, so don't quit.
}

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::close(bool inputFile) {
    int retVal=NO_ERROR;
    if (inputFile) {
        if (in) {
            if (sox_close(in)!=SOX_SUCCESS)
                retVal=SOX_CLOSE_FILE_ERROR;
            in=NULL;
        }
    } else {
        if (out) {
            if (sox_close(out)!=SOX_SUCCESS)
                retVal=SOX_CLOSE_FILE_ERROR;
            out=NULL;
        }
    }
    return retVal;
}

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::openRead(string fileName) {
    bool inputFile=true;
    close(inputFile);

    in = sox_open_read(fileName.c_str(), NULL, NULL, NULL);
    if (!in)
        return SOX_READ_FILE_OPEN_ERROR;

    // try to get the intended max value
    typedef std::numeric_limits<double> Info;
    maxVal=Info::quiet_NaN();
    ifstream maxFile((fileName+".max").c_str());
    if (!maxFile)
        return SOX_READ_MAXSCALE_ERROR;
    char maxValStr[256];
    maxFile>>maxValStr;
    maxVal=(float)::atof(maxValStr);
    maxFile.close();
    return 0;
}

#include <iostream>

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::openRead(intptr_t buffer, size_t len){
  bool inputFile=true;
  close(inputFile);

  // for (int i=0;i<len;i++)
  //   std::cout<<(int)((char*)buffer)[i]<<'\t';
  // std::cout<<std::endl;

  in = sox_open_mem_read((void*)buffer, len, NULL, NULL, NULL);
  if (!in)
      return SOX_READ_FILE_OPEN_ERROR;

  // No defined max value, user has to set manually - indicate
  typedef std::numeric_limits<double> Info;
  maxVal=Info::quiet_NaN();
  return SOX_READ_MAXSCALE_ERROR;
}

template<typename FP_TYPE_>
void Sox<FP_TYPE_>::output_message(unsigned level, const char *filename, const char *fmt, va_list ap) {
    char const * const str[] = {"FAIL", "WARN", "INFO", "DBUG"};
    if (sox_globals.verbosity >= level) {
        char base_name[128];
        sox_basename(base_name, sizeof(base_name), filename);
        //fprintf(stderr, "%s %s: ", str[min(level - 1, 3)], base_name);
        fprintf(stderr, "%s %s: ", str[((level - 1) <= (3) ? (level - 1) : (3))], base_name);

        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
    }
}

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::openWrite(const string &fileName, double fs, int channels, double maxVal, unsigned int wordSize, bool switchEndian, int revBytes, int revNibbles, int revBits){
    int retVal=NO_ERROR; // start assuming no error
    bool inputFile=false;
    close(inputFile);

    // setup the desired signalinfo
    sox_signalinfo_t si; // the signal info.
    si.rate=fs;
    si.channels=channels;
    si.precision=wordSize; // the precision is the number of bits of the data type
    si.length=0;
    si.mult=NULL;

    sox_encodinginfo_t encoding; // get default encodings
    sox_init_encodinginfo(&encoding);
    encoding.encoding=SOX_ENCODING_UNKNOWN; // setup the endcoding
    encoding.bits_per_sample=wordSize;
    encoding.opposite_endian=(switchEndian)?sox_true:sox_false;
    encoding.reverse_bytes=(revBytes==0)?sox_option_no:((revBytes==1)?sox_option_yes:sox_option_default);
    encoding.reverse_nibbles=(revNibbles==0)?sox_option_no:((revNibbles==1)?sox_option_yes:sox_option_default);
    encoding.reverse_bits=(revBits==0)?sox_option_no:((revBits==1)?sox_option_yes:sox_option_default);

    // the output file
    out=sox_open_write(fileName.c_str(), &si, &encoding, NULL, NULL, NULL);
    if (out==NULL)
        retVal=SOX_WRITE_FILE_OPEN_ERROR;
/*    cout<<"out->encoding.encoding "<<out->encoding.encoding<<endl;
    cout<<"out->encoding.bits_per_sample "<<out->encoding.bits_per_sample<<endl;
    cout<<"out->encoding.compression "<<out->encoding.compression<<endl;
    cout<<"out->encoding.reverse_bytes "<<out->encoding.reverse_bytes<<endl;
    cout<<"out->encoding.reverse_nibbles "<<out->encoding.reverse_nibbles<<endl;
    cout<<"out->encoding.reverse_bits "<<out->encoding.reverse_bits<<endl;
    cout<<"out->encoding.opposite_endian "<<out->encoding.opposite_endian<<endl;
*/
    outputMaxVal=maxVal;
    // write the maximum value to file
    ofstream outf((fileName+".max").c_str());
    outf.precision(20);
    outf<< scientific << outputMaxVal;
    outf.close();
    return retVal;
}

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::openMemWrite(void *buffer, size_t len, double fs, int channels, double maxVal, unsigned int wordSize, bool switchEndian, int revBytes, int revNibbles, int revBits){
  int retVal=NO_ERROR; // start assuming no error
  bool inputFile=false;
  close(inputFile);

  // setup the desired signalinfo
  sox_signalinfo_t si; // the signal info.
  si.rate=fs;
  si.channels=channels;
  si.precision=wordSize; // the precision is the number of bits of the data type
  si.length=0;
  si.mult=NULL;

  sox_encodinginfo_t encoding; // get default encodings
  sox_init_encodinginfo(&encoding);
  encoding.encoding=SOX_ENCODING_UNKNOWN; // setup the endcoding
  encoding.bits_per_sample=wordSize;
  encoding.opposite_endian=(switchEndian)?sox_true:sox_false;
  encoding.reverse_bytes=(revBytes==0)?sox_option_no:((revBytes==1)?sox_option_yes:sox_option_default);
  encoding.reverse_nibbles=(revNibbles==0)?sox_option_no:((revNibbles==1)?sox_option_yes:sox_option_default);
  encoding.reverse_bits=(revBits==0)?sox_option_no:((revBits==1)?sox_option_yes:sox_option_default);

  // the output memory buffer
  out=sox_open_mem_write(buffer, len, &si, &encoding, NULL, NULL);
  if (out==NULL)
      retVal=SOX_WRITE_FILE_OPEN_ERROR;
  outputMaxVal=maxVal;
  return retVal;
}


template<typename FP_TYPE_>
int Sox<FP_TYPE_>::write(const vector<vector<FP_TYPE_> > &audioData) {
    int retVal=NO_ERROR; // start assuming no error
    if (out) { // if the output file has been opened...
        if (out->signal.channels!=audioData.size())
            retVal=SOX_WRITE_OUT_CHANNEL_MISMATCH;
        else {
            int ch=out->signal.channels;
            int len=audioData[0].size();
            int total=ch*len;
            if (outputBuffer.size()<total)
                outputBuffer.resize(total);
            for (int i=0; i<audioData.size(); i++) // stride the interleaved outputBuffer with each channel
                for (int j=0; j<len; j++)
                    outputBuffer[j*ch+i]=(sox_sample_t)((double)audioData[i][j]/outputMaxVal*(double)numeric_limits<sox_sample_t>::max());
            size_t writeCount=sox_write(out, &outputBuffer[0], total);
            retVal=writeCount;
        }
    } else
        retVal=SOX_WRITE_FILE_NOT_OPENED_ERROR;
    return retVal;
}

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::closeRead(void) {
    bool inputFile=true;
    return close(inputFile);
}

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::closeWrite(void) {
    bool inputFile=false;
    return close(inputFile);
}

template<typename FP_TYPE_>
vector<string> Sox<FP_TYPE_>::availableFormats(void) {
    size_t i;
    char const * const * names;
    vector<string> formatExts;

    sox_format_init();
    for (i = 0; sox_format_fns[i].fn; ++i)
        char const * const *names = sox_format_fns[i].fn()->names;

    printf("AUDIO FILE FORMATS:");
    for (i = 0; sox_format_fns[i].fn; ++i) {
        sox_format_handler_t const * handler = sox_format_fns[i].fn();
        if (!(handler->flags & SOX_FILE_DEVICE))
            for (names = handler->names; *names; ++names)
                if (!strchr(*names, '/'))
                    formatExts.push_back(*names);
    }
    std::sort(formatExts.begin(), formatExts.end());
    return formatExts;
}

template<typename FP_TYPE_>
void Sox<FP_TYPE_>::printFormats(){
  printf("The known output file extensions (output file formats) are the following :\n");
  vector<string> formats=availableFormats();
  for (int i=0; i<formats.size(); i++)
      printf("%s ",formats[i].c_str());
  printf("\n");
}

#ifndef HAVE_EMSCRIPTEN
template class Sox<short int>;
template class Sox<int>;
template class Sox<float>;
#endif
template class Sox<double>;

#ifdef HAVE_EMSCRIPTEN

template<typename FP_TYPE_>
FP_TYPE_ Sox<FP_TYPE_>::getSample(unsigned int r, unsigned int c){
  if (r>=Sox<FP_TYPE_>::audio.rows()) {
    SoxDebug().evaluateError(SOX_ROW_BOUNDS_ERROR);
    typedef std::numeric_limits<double> Info;
    return Info::quiet_NaN();
  }
  if (c>=audio.cols()) {
    SoxDebug().evaluateError(SOX_COL_BOUNDS_ERROR);
    typedef std::numeric_limits<double> Info;
    return Info::quiet_NaN();
  }
  return audio(r,c);
}

template<typename FP_TYPE_>
unsigned int Sox<FP_TYPE_>::getRows(){
  return audio.rows();
}

template<typename FP_TYPE_>
unsigned int Sox<FP_TYPE_>::getCols(){return Sox<FP_TYPE_>::audio.cols();}

template<typename FP_TYPE_>
int Sox<FP_TYPE_>::readJS(unsigned int count){
  return read(audio, count);
}

#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(Sox_ex) {
  emscripten::class_<Sox<double>>("Sox")
  .constructor()
  .function("printFormats", &Sox<double>::printFormats)
  .function("getSample", &Sox<double>::getSample)
  .function("getRows", &Sox<double>::getRows)
  .function("getCols", &Sox<double>::getCols)
  .function("openRead", emscripten::select_overload<int(intptr_t, size_t)>(&Sox<double>::openRead), emscripten::allow_raw_pointers())
  .function("read", &Sox<double>::readJS)
  ;
}
#endif
