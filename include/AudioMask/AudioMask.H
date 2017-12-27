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

#ifndef AUDIOMASK_H_
#define AUDIOMASK_H_

#include "AudioMask/MooreSpread.H"

/**
* Evaluates the Dr M.R. Flax (2000) hybrid simultaneous audio mask
* Handles the evaluation of the simultaneous audio mask given a filter bank sub-banded output and sample count

*/
class AudioMask : private MooreSpread {
    double *excitation; //!< The excitation of the roex filters
    double *Lvmu;
    double factor;
protected:
    int fs; //!< Sample frequency
public:
    double *mask; //!< The audio mask
    double max; //!< The maximum value of the mask

    /**
    * Constructs the memory and factors for calculating the audio mask
    * @ sampFreq The sample frequency
    * @ fBankCount The number of filter sub-bands
    */
    AudioMask(int sampFreq, int fBankCount);
    ~AudioMask(void); //!< Deconstructor

    /**
    * Method for setting the centre freqs
    * @ which The sub-band
    * @ value The correcponding central frequency
    */
    void setCFreq(int which, double value) {
        MooreSpread::setCFreq(which, value);
    }
    /**
    * Method for finding the Preceptual Audio Mask using the Terhardt model
    * @ filterBankOutput The output of the filter bank
    * @ sampleCount The number of samples
    */
    void exciteTerhardt(double **filterBankOutput, int sampleCount);
    void exciteBeerends(double **filterBankOutput, int sampleCount);  /// Method for finding the Preceptual Audio Mask using the Beerends model
};

#endif //AUDIOMASK_H_
