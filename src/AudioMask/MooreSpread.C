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
#include "AudioMask/MooreSpread.H"
#include "AudioMask/AudioMaskCommon.H"

MooreSpread::
MooreSpread(int fBankCount){
  //  std::cout<<"MooreSpread: init"<<std::endl;
  bankCount=fBankCount;
  memory=NULL;
  spread=NULL;
  centreFreqs=NULL;
  if (!(spread=new double*[bankCount]))
    std::cerr <<"MooreSpread: spread malloc 1: Out of memory"<<std::endl;
  else{
    if (!(memory=new double[(1+bankCount)*bankCount]))
      std::cerr <<"MooreSpread: spread malloc 2: Out of memory"<<std::endl;
    for (int i=0;i<bankCount;i++)
      spread[i]=&memory[i*bankCount];
  }
  centreFreqs=&memory[bankCount*bankCount];

  //std::cerr<<"MooreSpread: init .... done "<<spread<<std::endl;
}

MooreSpread::
~MooreSpread(void){
  //  std::cout<<"MooreSpread: deinit"<<std::endl;
  if (memory) delete [] memory;
  memory=NULL;
  //std::cerr<<spread<<std::endl;
  if (spread) delete [] spread;
  spread=NULL;
  //  std::cerr<<spread<<std::endl;
  //std::cout<<"MooreSpread: deinit exit"<<std::endl;
}

//#include <fstream>
void MooreSpread::
excite(double **filterBankOutput, int sampleCount, int sampleFreq){
  // The factor to mult by to find the frequency in the filtered signal
  //  double factor=((double)sampleFreq/2.0)/((double)sampleCount/2.0);
  double factor=((double)sampleCount)/(double)sampleFreq;
  int binOfInterest;
  //  std::cout <<"MooreSpread: excite: factor: "<<factor<<std::endl;
  //  ofstream temp("temp.dat");
  //ofstream temp1("temp1.dat");
  for (int i=0; i<bankCount;i++){
    binOfInterest=(int)rint(centreFreqs[i]*factor);
    //    std::cout <<i<<" center freq. " <<centreFreqs[i]<<" binOfInterest: "<<binOfInterest<<std::endl;
    for (int j=0;j<bankCount;j++){
      spread[i][j]=filterBankOutput[j][binOfInterest];
      //temp <<10*log10(spread[i][j])<<'\t';
      //      temp <<20*log10(spread[i][j])<<'\t';
      //temp1 <<10*log10(filterBankOutput[j][binOfInterest]+1.001)<<'\t';
      //std::cout <<spread[i][j]<<" ";
    }
    //temp <<std::endl;
    //temp1 <<std::endl;
  }
}



