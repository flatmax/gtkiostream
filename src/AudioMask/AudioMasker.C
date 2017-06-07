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

#include "AudioMask/AudioMasker.H"
#include <math.h>
//#include <strings.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#endif

double DepUKFB::p_51_1k=4.0*1000.0/(AM_C1*(AM_C2 + 1.0));

AudioMasker::
AudioMasker(int sampFreq, int fBankCount) : AudioMask(sampFreq, fBankCount) {
    output=powOutput=NULL;
    input=NULL;
    //gtfb=NULL;
    pfb=NULL;
    fftData=NULL;
    fft=NULL;

    bankCount=fBankCount;
    std::cout<<"Bank Count "<<bankCount<<std::endl;
    //  sampleFreq=DEFAULT_SAMPLEFREQ;
    sampleCount=DEFAULT_SAMPLECOUNT;
    FBMalloc();
}

AudioMasker::
AudioMasker(void) : AudioMask(DEFAULT_SAMPLEFREQ, DEFAULT_FBCOUNT) {
    output=powOutput=NULL;
    input=NULL;
    //gtfb=NULL;
    pfb=NULL;
    fftData=NULL;
    fft=NULL;

    bankCount=DEFAULT_FBCOUNT;
    //  std::cout<<"Bank Count "<<bankCount<<std::endl;
    //sampleFreq=DEFAULT_SAMPLEFREQ;
    sampleCount=DEFAULT_SAMPLECOUNT;
    FBMalloc();
}

AudioMasker::
~AudioMasker(void) {
    //std::cout<<"AudioMasker::~AudioMasker"<<std::endl;
    FBDeMalloc();
}

// Filter bank memory de-allocation routine
void AudioMasker::
FBDeMalloc(void) {
    //std::cout<<"AudioMasker::FBDeMalloc"<<std::endl;
    if (output) {
        for (int i=0; i<bankCount; i++)
            if (output[i]) delete [] output[i];
        delete [] output;
    }
    output=NULL;

    if (powOutput) {
        for (int i=0; i<bankCount; i++)
            if (powOutput[i]) delete [] powOutput[i];
        delete [] powOutput;
    }
    powOutput=NULL;

    if (input) delete [] input;
    input=NULL;

    if (pfb) delete pfb;
    pfb=NULL;

    //  if (gtfb) delete gtfb;
    //gtfb=NULL;

    if (fftData) delete fftData;
    fftData=NULL;
    if (fft) delete fft;
    fft=NULL;
}

// Filter bank memory allocation routine
void AudioMasker::
FBMalloc(void) {
    FBDeMalloc(); //Ensure not malloced already
    // Allocate the output matrix of the perceptual filters
    if (!(output=new double*[bankCount])) {
        std::cerr<<"AudioMasker::FBMalloc : filter bank malloc error initial"<<std::endl;
        exit(-1);
    } else {
        for (int i=0; i<bankCount; i++)
            output[i]=NULL;
        for (int i=0; i<bankCount; i++)
            if (!(output[i]=new double[sampleCount])) {
                std::cerr<<"AudioMasker::FBMalloc : filter bank malloc error secondary"<<std::endl;
                FBDeMalloc();
                exit(-1);
            }
    }

    // Allocate the powOutput matrix of the perceptual filters
    if (!(powOutput=new double*[bankCount])) {
        std::cerr<<"AudioMasker::FBMalloc : filter bank malloc error initial"<<std::endl;
        exit(-1);
    } else {
        for (int i=0; i<bankCount; i++)
            powOutput[i]=NULL;
        for (int i=0; i<bankCount; i++)
            //      if (!(powOutput[i]=new double[(int)ceil((double)sampleCount/2.0)])){
            if (!(powOutput[i]=new double[(int)ceil((double)fs/2.0)])) {
                std::cerr<<"AudioMasker::FBMalloc : filter bank malloc error secondary"<<std::endl;
                FBDeMalloc();
                exit(-1);
            }
    }

    if (!(input=new double[sampleCount])) {
        std::cerr<<"AudioMasker::FBMalloc : input malloc error"<<std::endl;
        FBDeMalloc();
        exit(-1);
    }

    //  if (!(gtfb= new GTFB(DEFAULT_LOWFERQ, sampleFreq, bankCount))){
    //  std::cerr<<"AudioMasker::FBMalloc : gtfb malloc error"<<std::endl;
    //  FBDeMalloc();
    //  exit(-1);
    //}


    if (!(pfb= new DepUKFB(fs, bankCount))) {
        std::cerr<<"AudioMasker::FBMalloc : pfb malloc error"<<std::endl;
        FBDeMalloc();
        exit(-1);
    }

    //  if (!(fftData=new realFFTData(sampleCount))){
    if (!(fftData=new RealFFTData(fs))) {
        std::cerr<<"AudioMasker::FBMalloc : fftData malloc error"<<std::endl;
        FBDeMalloc();
        exit(-1);
    }

    if (!(fft=new RealFFT(fftData))) {
        std::cerr<<"AudioMasker::FBMalloc : fft malloc error"<<std::endl;
        FBDeMalloc();
        exit(-1);
    }
}

/** These should be implemented differently for different Input types
 */
void AudioMasker::
excite(short int *Input, int sCount) {
    if (sCount != sampleCount)// Check for matrix re-size
        FBDeMalloc();

    sampleCount=sCount;

    if (!output) // Check for null matrix
        FBMalloc();

    for (int i=0; i<sCount; i++) //copy the input as double
        input[i]=(double)Input[i];

    process(); //Do the processing
}

void AudioMasker::
excite(double *Input, int sCount) {
    if (sCount != sampleCount)// Check for matrix re-size
        FBDeMalloc();

    sampleCount=sCount;

    if (!output) // Check for null matrix
        FBMalloc();

    for (int i=0; i<sCount; i++) //copy the input as double
        input[i]=Input[i];

    process(); //Do the processing
}

double AudioMasker::
findThreshold(double freq) {
    for (int i=bankCount-1; i>=0; i--) {
        //    if (freq>=gtfb->edgeFreq[i])
        if (freq>=pfb->ef[i]) {
            //      std::cout<<i<<std::endl;
            return mask[i];
        }
    }
    std::cerr <<"AudioMasker::findThreshold : freq !=> gtfb->edgeFreq["<<bankCount-1<<"] returning 0"<<std::endl;
    return 0;
}

#include <fstream>
void AudioMasker::
process(void) {
    bzero(fftData->in, fs*sizeof(fftw_real));//Ensure we start with a zero array
    for (int j=0; j<sampleCount; j++) //Find pow spec of input
        fftData->in[j]=input[j];
    fft->fwdTransform();
    fftData->compPowerSpec();
    fftData->sqrtPowerSpec();
    //ofstream output("w");
    //int halfSampleCount=(int)rint((double)sampleCount/2.0);
    int halfFS=(int)rint(fs/2.0);
    for (int i=0; i<bankCount; i++) { // Find power spectra and copy over
        //    for (int j=0; j<halfSampleCount;j++){
        for (int j=0; j<halfFS; j++) {
            //  output<<(*pfb)(i,j,halfSampleCount)<<'\t';
            //      //powOutput[i][j]=fftData->power_spectrum[j]*(*pfb)(i,j,halfSampleCount);
            powOutput[i][j]=fftData->power_spectrum[j]*(*pfb)(i,j,halfFS);
        }
        //output<<std::endl;
    }
    /*
    ofstream outF("filter.dat");
    for (int i=0;i<bankCount;i++){
      for (int j=0;j<sampleCount/2;j++)
        outF<<powOutput[i][j]<<'\t';
      outF<<std::endl;
    }
    outF.close();
    */

    //  gtfb->grab(1);
    for (int i=0; i<bankCount; i++) //Set up freq of interest (pfb centre freqs.)
        setCFreq(i, pfb->cf[i]);
    //    setCFreq(i, gtfb->prev()->cf);
    exciteTerhardt(powOutput, fs);// Find the masking function
    //exciteTerhardt(powOutput, sampleCount);// Find the masking function
    //exciteBeerends(powOutput, sampleCount);// Find the masking function
}

/*
#include <fstream>
#include "../gammatone/GTSensitivity.H"
void AudioMasker::
process(void){
  //for (int i=0;i<bankCount;i++){ //Zero the filter bank output(due to IIR nature)
  //  bzero(output1[i], sampleCount*sizeof(double));
  //  bzero(output2[i], sampleCount*sizeof(double));
  //  bzero(output[i], sampleCount*sizeof(double));
  //  }
  LinkList<double **> outputs;
  outputs.add(output1);
  outputs.add(output2);
  outputs.grab(1);

  for (int i=0;i<bankCount;i++) //Zero filter bank output(due to IIR nature)
    bzero(outputs.current()[i], sampleCount*sizeof(double));
  for (int i=1; i<=bankCount;i++) // Filter the input
    gtfb->grab(i)->filter(input, &outputs.current()[i-1][0], sampleCount);

  double **last, **now;
  for (int j=1;j<RECURSE;j++){
    last=outputs.current();
    now=outputs.next();
    for (int i=0;i<bankCount;i++) //Zero filter bank output(due to IIR nature)
      bzero(now[i], sampleCount*sizeof(double));
    for (int i=1; i<=bankCount;i++) // Filter the input
      gtfb->grab(i)->filter(&last[i-1][0], &now[i-1][0], sampleCount);
  }

  for (int i=1; i<=bankCount;i++) // Filter the input - originally once
    gtfb->grab(i)->filter(input, &output[i-1][0], sampleCount);

  //  GTSensitivity gts; // sensitivity adjustement for the IIR gammatone filterbank

  for (int i=0;i<bankCount;i++){ // Find power spectra and copy over
    for (int j=0; j<sampleCount;j++)
      fftData->in[j]=outputs.current()[i][j];
    //fftData->in[j]=output[i][j];
    fft->fwdTransform();
    fftData->compPowerSpec();

    double cf=gtfb->grab(i+1)->cf;
    for (int j=0; j<(int)rint((double)sampleCount/2.0);j++){
      //      powOutput[i][j]=pow(10.0,gts.find(j,cf,fs)/20.0)*sqrt(fftData->power_spectrum[j]);
      powOutput[i][j]=sqrt(fftData->power_spectrum[j]);
    }

    //double shift=fftData->power_spectrum[fftData->minPowerBin];
    ////The following tries to correct for the shift up in the gammatone response
    //if (shift>10.0)
    //  for (int j=0; j<(int)rint((double)sampleCount/2.0);j++)
    //powOutput[i][j]=sqrt(fftData->power_spectrum[j]/shift);
    //else
    //  for (int j=0; j<(int)rint((double)sampleCount/2.0);j++)
    //powOutput[i][j]=sqrt(fftData->power_spectrum[j]);
  }

  ofstream outF("filter.dat");
  for (int i=0;i<bankCount;i++){
    for (int j=0;j<sampleCount/2;j++)
      outF<<powOutput[i][j]<<'\t';
    outF<<std::endl;
  }
  outF.close();

  gtfb->grab(1);
  for (int i=0; i<bankCount;i++)//Set up freq of interest (pfb centre freqs.)
    setCFreq(i, gtfb->prev()->cf);

  exciteTerhardt(powOutput, sampleCount);// Find the masking function
  //exciteBeerends(powOutput, sampleCount);// Find the masking function
}
*/
