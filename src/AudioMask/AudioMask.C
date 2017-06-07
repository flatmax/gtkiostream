/*
 libaudiomask - hybrid simultaneous audio masking threshold evaluation library
    Copyright (C) 2000-2010  Dr Matthew Raphael Flax

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

#include <math.h>
#include <complex>
#include "AudioMask/AudioMask.H"

#if !defined(_MSC_VER) && !defined(__CYGWIN__) && !defined(__MINGW32__) // Mingw/Microsoft doesn't have this header
#include <values.h>
#else
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <float.h>
#endif
#define MAXDOUBLE DBL_MAX
#endif

#include <assert.h>

AudioMask::
AudioMask(int sampFreq, int fBankCount) : MooreSpread(fBankCount){
  fs=sampFreq;
  mask=Lvmu=NULL;
  if (!(excitation=new double[fBankCount]))
    std::cerr << "AudioMask: excitation malloc: Out of Memory"<<std::endl;
  if (!(mask=new double[fBankCount]))
    std::cerr << "AudioMask: mask malloc: Out of Memory"<<std::endl;
  if (!(Lvmu=new double[fBankCount*fBankCount]))
    std::cerr << "AudioMask: Lvmu malloc: Out of Memory"<<std::endl;
}

AudioMask::
~AudioMask(void){
  if (excitation) delete [] excitation;
  excitation=NULL;
  if (mask) delete [] mask;
  mask=NULL;
  if (Lvmu) delete [] Lvmu;
  Lvmu=NULL;
}

#include <fstream>
#define F2CB(f) (13.3*atan(0.75*f/1000))
void AudioMask::
exciteTerhardt(double **filterBankOutput, int sampleCount){
  max=-MAXDOUBLE;
  // Find the factor to scale by and scale ...
  factor=fabs(bankCount-F2CB((double)fs/2.0));
  //  std::cout <<"factor "<<factor<<std::endl;

  // Find the excitation ...
  for (int i=0;i<bankCount;i++){
    //    int which=bankCount-i-1;
    //int which=i;
    excitation[i]=0.0;
    for (int j=0;j<sampleCount;j++)
      excitation[i]+=filterBankOutput[i][j];
    if (sampleCount!=fs)
        excitation[i]*=((fs/2.0)/sampleCount);
    excitation[i]=10.0*log10(excitation[i]);
//    if (sampleCount!=fs){
//      //      std::cout<<"adjusting"<<std::endl;
//            excitation[i]+=20*log10((fs/2.0-sampleCount)/sampleCount);
//    }
  }

  /*  ofstream ex("excite.dat");
  for (int i=0;i<bankCount;i++)
    ex<<excitation[i]<<'\n';
    ex.close();*/

  // Find the spreading function ...
    //MooreSpread::excite(filterBankOutput, sampleCount, fs);
  MooreSpread::excite(filterBankOutput, fs, fs);
  /*
  ofstream os("spread.dat");
  for (int i=0;i<bankCount;i++){
    for (int j=0;j<bankCount;j++)
      os<<10.0*log10(spread[i][j])<<'\t';
    os<<std::endl;
  }
  os.close();
  */
  // Find the mask ...
  for (int i=0;i<bankCount;i++){
    for (int j=0;j<bankCount;j++)
      Lvmu[j+i*bankCount]=excitation[j]+10.0*log10(spread[i][j]);
  }

  /*  ofstream lvmu("Lvmu.dat");
  for (int i=0;i<bankCount;i++){
    for (int j=0;j<bankCount;j++){
      lvmu<<Lvmu[j+i*bankCount]<<'\t';
    }
    lvmu<<std::endl;
    }*/

  for (int i=0;i<bankCount;i++){
    mask[i]=0.0;
    for (int j=0;j<i;j++) // Lower Freqs.
      mask[i]+=pow(10.0,Lvmu[i+j*bankCount]/20.0);
    for (int j=i+1;j<bankCount;j++) // Higher Freqs.
      mask[i]+=pow(10.0,Lvmu[i+j*bankCount]/20.0);
    mask[i]/=factor;
    if (mask[i]>max) max=mask[i];
    //    std::cout<<excitation[i]<< '\t'<<mask[i]<<std::endl;
  }
}

#define ALPHA 0.8
void AudioMask::
exciteBeerends(double **filterBankOutput, int sampleCount){
    assert(-1); // this method requres debugging for sample counts which aren't the same as the sample rate.
  max=-1000000.0;
  // Find the factor to scale by and scale ...
  factor=fabs(bankCount-F2CB((double)fs/2.0));

  // Find the excitation ...
  for (int i=0;i<bankCount;i++){
    int which=bankCount-i-1;
    excitation[which]=0.0;
    for (int j=0;j<sampleCount;j++){
      excitation[which]+=filterBankOutput[i][j];
    }
    excitation[which]=10.0*log10(excitation[which]);
  }

  // Find the spreading function ...
  MooreSpread::excite(filterBankOutput, sampleCount, fs);

  // Find the mask ...
  for (int i=0;i<bankCount;i++){
    for (int j=0;j<bankCount;j++)
      Lvmu[j+i*bankCount]=excitation[j]+10.0*log10(spread[i][j]);
  }

  for (int i=0;i<bankCount;i++){
    mask[i]=0.0;
    for (int j=0;j<i;j++) // Lower Freqs.
      mask[i]+=pow(10.0,pow(Lvmu[i+j*bankCount],ALPHA)/20.0);
    for (int j=i+1;j<bankCount;j++) // Higher Freqs.
      mask[i]+=pow(10.0,pow(Lvmu[i+j*bankCount],ALPHA)/20.0);
    mask[i]=pow(mask[i], 1.0/ALPHA);
    mask[i]/=factor;
    if (mask[i]>max) max=mask[i];
    //    std::cout<<excitation[i]<< '\t'<<mask[i]<<std::endl;
  }
}

