/*
 libaudiomask - hybrid simultaneous audio masking threshold evaluation library
    Copyright (C) 2000-2018  Dr Matthew Raphael Flax

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEPUKFB_H_
#define DEPUKFB_H_

#include <iostream>
#include <math.h>
//#include "../utils/perceptual.H"
#include <stdlib.h>

#include "AudioMask/MooreSpread.H"

extern double rint(double);

#define FREQBINCOUNT 44100

//Filter bank central frequencies will be placed between this frequency fs/2
#define LOWFREQ 50

#define AM_C1 24.673
#define AM_C2 4.368
#define AM_C3 21.366

//We are using a level invariant filterbank Hence define X as static
#define AM_X 51.0

/** Roex filters.
*    This class defines power spectrum shapes for auditory filters based on :
*    %[1]  ``A Model for the Prediction of Thresholds, Loudness, and Partial
*    %        Loudness'' Moore B.C.J., Glasberg B.R. and Baer T., Journal of the
*    %       Audio Engineering Society, vol. 45, no. 4, April 1997, pp.224-40.
 */
class DepUKFB{
  static double p_51_1k;
  int fCount;

/**
* Conversion from central frequency to ERB.
* @ fc the central frequency
*/
  double erb(double fc){
    return 24.7*(4.37*(fc/1000.0)+1.0);
  }

/**
* Frequency to ERB conversion.
* @ freq The frequency in Hz
*/
  double freq2ERB(double freq){
    return (AM_C3*log10((AM_C2 * freq/1000.0) + 1.0));
  }
/**
* ERB to frequency conversion.
* @ erb The ERB
*/
  double ERB2freq(double erb){
    return 1000.0 * (pow(10.0,(erb/AM_C3)) - 1.0) / AM_C2;
  }

  /**
  * Auditory Filter procedure.
  * @ fc the central frequency of the filter
  * @ whichFilter the filter index
  */
  virtual void af(double fc, int whichFilter){
    //    std::cout<<"DepUKFB::af"<<std::endl;
    double freqFact=((double)fs/2.0)/(double)FREQBINCOUNT;
    //    std::cout<<freqFact<<'\t';
    double freq=0.0;
    for (int i=0;i<FREQBINCOUNT;i++){
      g[i]=fabs((freq-fc)/fc);
      freq+=freqFact;
    }

    double *filt=w[whichFilter], p;
    freq=0.0;

    for (int i=0;i<FREQBINCOUNT;i++){
      if (freq<fc)
	p=p_l(fc);
      else
	p=p_u(fc);
      filt[i]=(1.0+p*g[i])*exp(-p*g[i]);
      freq+=freqFact;
    }
  }


/**
* Find the central frequency of the filter.
*/
  void findCF(void){
    double step=(freq2ERB((double)fs/2.0-1.0)-freq2ERB(LOWFREQ))/(fCount-1.0);
    //std::cout<<"step "<<step<<std::endl;
    double erbval=freq2ERB(LOWFREQ)-step;
    for (int i=0;i<fCount;i++){
      cf[i]=ERB2freq(erbval+=step); //centre frequency locations
      //std::cout<<cf[i]<<std::endl;
    }

    erbval=freq2ERB(LOWFREQ)-step/2.0;
    ef[0]=0.0;
    for (int i=1;i<fCount;i++){
      ef[i]=ERB2freq(erbval+=step); //edge frequency locations
      //std::cout<<cf[i]<<'\t'<<ef[i]<<std::endl;
    }
  }
protected:
  int fs; //!< The sample frequency.
  double *g; //!< g coeff.
  double **w; //!< The filters.

  DepUKFB(){   //!< Constructor called by child classes.
  }

/**
* Lower side p evaluation.
* @ fc The central frequency of the filter
*/
  double p_l(double fc){
    // The following reduces to p_51 for constant X
    //double p_51=4.0*fc/erb(fc);
    //    return p_51-0.35*(p_51/p_51_1k)*(X-51.0);
    return 4.0*fc/erb(fc);
  }

/**
* Upper side p evaluation.
* @ fc The central frequency of the filter
*/
  double p_u(double fc){
    // The following reduces to p_51 for constant X
    //double p_51=4.0*fc/erb(fc);
    //return p_51+0.118*(X-51.0);
    return 4.0*fc/erb(fc);
  }
public:
  double *cf; //!< The filter centre frequencies.
  double *ef; //!< The filter edge frequencies.

  /**
  * Roex filter shape constructor.
  * @ sampleFreq The sample rate
  * @ fCnt The number of filters
  */
  DepUKFB(int sampleFreq, int fCnt=50){
    init(sampleFreq, fCnt);
  }

  /**
  * Roex filter shape constructor - actual method.
  * @ sampleFreq The sample rate
  * @ fCnt The number of filters
  */
  void init(int sampleFreq, int fCnt=50){
    fCount=fCnt;
    fs=sampleFreq;
    cf=ef=g=NULL;
    w=NULL;
    if (!(g=new double[FREQBINCOUNT])){
      std::cerr<<"DepUKFB::DepUKFB: g malloc error"<<std::endl;
      exit(-1);
    }
    if (!(w=new double*[fCount])){
      std::cerr<<"DepUKFB::DepUKFB: w malloc error"<<std::endl;
      exit(-1);
    } else {
      for (int i=0;i<fCount;i++)
	w[i]=NULL;
      for (int i=0;i<fCount;i++){
	if (!(w[i]=new double[FREQBINCOUNT])){
	  std::cerr<<"DepUKFB::DepUKFB: w[i] malloc error"<<std::endl;
	  exit(-1);
	}
      }
    }

    if (!(cf=new double[fCount])){
      std::cerr<<"DepUKFB::DepUKFB: cf malloc error"<<std::endl;
      exit(-1);
    }

    if (!(ef=new double[fCount])){
      std::cerr<<"DepUKFB::DepUKFB: ef malloc error"<<std::endl;
      exit(-1);
    }

    //Place the filter centre freqs ...
    findCF();
    //Step through and fill each filter ...
    for (int i=0;i<fCount;i++)
      af(cf[i],i);
  }

  virtual ~DepUKFB(){ //!< Destructor.
    if (g) delete [] g;
    if (w){
      for (int i=0;i<fCount;i++)
	if (w[i]) delete [] w[i];
      delete [] w;
    }
    if (cf) delete [] cf;
    if (ef) delete [] ef;
  }

  int filterCount(void){return fCount;} //!< Returns the number of filters.

    /**
    * Operator returning an array of filter values for one sub-band in the filter bank.
    * @ i the index
    */
  double* operator[](int i){return w[i];}
    /**
    * Operator returning the filter magnitude for one filter in a bank at a particular Fourier index.
    * @ i the filter index
    * @ j the frequency index
    * @ binCount the number of Fourier bins
    */
  double operator()(int i, int j, int binCount){
    int index=(int)rint((double)j*((double)FREQBINCOUNT/(double)binCount));
    //std::cout<<i<<'\t'<<j<<'\t'<<binCount<<'\t'<<index<<std::endl;
    return w[i][index];
  }
};

#endif //DEPUKFB_H_



