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

#ifndef MOORESPREAD_H_
#define MOORESPREAD_H_

#include <iostream>
//#include "../gammachirp/GCFB.H"
//#include <mffm/GTFB.H>

/** Finds the Moore spreading function.
    First implementation for the Gamma Chirp filter bank, but possible for
    any perceptual filter bank.
    References :
    [1] Moore B.C.J., Glasberg B.R. (1983) ``Suggested formulae for
        calculating auditory-filter bandwidths and excitation patterns'',
	J. Acous. Soc. Am., 74, 750-753
 */
class MooreSpread {
protected:
  int bankCount; //!< The number of sub-bankds in the filter bank
  double *memory; //!< Memory used in the operation
  double **spread; //!< The Moore/Glasberg spreading due to the filters
public:
  double *centreFreqs; //!< The centreFrequencies of each filter bank
  MooreSpread(int fBankCount);  //!< Instantiation requiring the number of filter banks
  ~MooreSpread(void); //!< Destructor
  void setCFreq(int which, double value){centreFreqs[which]=value;} //!< Method for setting the centre freqs
  /** Method for finding the Moore Spread
       * @param filterBankOutput The output of the filter bank, an array of magnitudes for each filter
       * @param sampleCount The number of samples in each sub-band or filter
       * @param sampleFreq The sample frequency of the time domain signal
       */
  void excite(double **filterBankOutput, int sampleCount, int sampleFreq);
};
#endif // MOORESPREAD_H_
