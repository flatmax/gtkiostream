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

#ifndef AUDIOMASKER_H_
#define AUDIOMASKER_H_

#ifdef _MSC_VER
#ifndef rint
#define rint round
#endif
#endif

#include <Debug.H>
#include <Eigen/Dense>
using namespace Eigen;

#define AUDIOMASKER_MULTICHANNEL_ERROR AUDIOMASKER_ERROR_OFFSET-1 ///< Error when the user passes in multichannel audio, currently not handled.
#define AUDIOMASKER_SAMPLECOUNT_ERROR AUDIOMASKER_ERROR_OFFSET-2 ///< Error when the user passes in audio with too few samples, currently not handled.

/** Debug class for Decomposition
*/
class AudioMaskerDebug : virtual public Debug {
public:
    /** Constructor defining all debug strings which match the debug defined variables
    */
    AudioMaskerDebug() {
#ifndef NDEBUG
    errors[AUDIOMASKER_MULTICHANNEL_ERROR]=std::string("AudioMasker: Can not handle more then one channel of audio. Please provide audio in a single column");
    errors[AUDIOMASKER_SAMPLECOUNT_ERROR]=std::string("AudioMasker: Please supply a sufficient number audio samples, try to provide at least 10*AudioMasker.getBankCount(). Please provide audio in a single column");
#endif
    }

    /// Destructor
    virtual ~AudioMaskerDebug() {}
};

#include "AudioMask/AudioMask.H"
#include "AudioMask/depukfb.H"
#include <fft/RealFFT.H>
#include <stdlib.h>

#define DEFAULT_FBCOUNT 100
#define DEFAULT_SAMPLECOUNT 512
#define DEFAULT_SAMPLEFREQ 44100
//#define DEFAULT_LOWFERQ 25

// Recurse defines the number of times we recurse the output to input >=2
//#define RECURSE 7

/** libAudioMask - Simultaneous audio mask threshold estimation library
* \image html masking.example.jpg

* This code also implements the roex auditory filters.
*
* Please read the article located on the debian file system file:///usr/shar/doc/libaudiomask-1.0/Flax.2000.Improved.Auditory.Masking.Models.pdf
*
* It may also be found on the web http://www.assta.org/sst/SST-00/cache/SST-00-Chapter9-p2.pdf

* This example shows how to use the hybrid
* simultaneous audio masking class to find the masking threshold of a time domain signal.
*
* ========================= HOWTO ===============================
* See the AudioMaskerExample.C example for a more detailed scenario.
* \code
*     // First find the masking threshold
*     AudioMasker masker(sampleFreq, count); // Create the audio masker class using fs=sampleFreq and count filters
*     masker.excite(input, sampleCount); // find the mask for the array of input data which has sampleCount time samples.
*
*     // Now do something with the masking threshold ...
*
*     // The frequency domain mask is now located here
*     for (int j=0; j<count;j++)
*         masker.mask[j]; // This is the mask at each of the count frequencies of interest
*
*     // A more sophisticated example - find the threshold for each Fourier bin
*     double fact=(double)sampleFreq/((double)sampleCount-1.0); // convert from an index to the equivalent * Fourier bin frequency
*     for (int j=0; j<halfSampleCount;j++){
*         cout<<"finding for freq "<<j*fact<<'\t'; // The frequency we are inspecting
*         double threshold=masker.findThreshold(j*fact); // The masking threshold
*         20*log10(threshold); // The threshold in decibels (dB)
*     }
* \endcode

*/
class AudioMasker : public AudioMask {
    double **output; //!< Filter bank output
    double **powOutput; //!< Filter bank output power
    double *input; //!<Filter bank input
    int sampleCount; //!<The sample count
    int bankCount; //!<The filter bank count

    RealFFTData *fftData; //!< The FFT data
    RealFFT *fft; //!< The FFT

    void FBDeMalloc(void);//!< Filter bank output matrix memory de-allocation

    void FBMalloc(void);  //!< Filter bank output matrix memory allocation

    void process(void); //!< Process the transformation
public:
    DepUKFB *pfb; //!< roex filters
    //  GTFB *gtfb; //!< gammatone filters

    /**
    * The Audio masking constructor
    * @ sampFreq The sample frequency of the time domain data
    * @ fBankCount The number of filter banks
    */
    AudioMasker(int sampFreq, int fBankCount);
    AudioMasker(void); //!< Audio masker constructor - allowing specification of fs and sub-band count later
    ~AudioMasker(void); //!< Audio masker deconstructor

    /**
    * Finds the excitation for input data
    * @ Input Using short int input data
    * @ sCount samples
    */
    void excite(short int *Input, int sCount);

    /**
    * Finds the excitation for input data
    * @ Input Using double input data
    * @ sCount samples
    */
    void excite(double *Input, int sCount);

    /**
    * Finds the excitation for input data.
    * @ Input The data in the form of an Eigen column.
    * \return NO_ERROR on success, or the appropriate error otherwise.
    */
    template<typename Derived>
    int excite(const Eigen::DenseBase<Derived> &Input) {
        if (Input.cols()>Input.rows() || Input.cols()>1)
            return AUDIOMASKER_MULTICHANNEL_ERROR;
        if (Input.rows() < 10*bankCount)
            return AUDIOMASKER_SAMPLECOUNT_ERROR;

        if (Input.rows() != sampleCount)// Check for matrix re-size
            FBDeMalloc();

        sampleCount=Input.rows();

        if (!output) // Check for null matrix
            FBMalloc();

        for (int i=0;i<sampleCount;i++) //copy the input as double
            input[i]=Input(i,0);

        process(); //Do the processing
        return NO_ERROR;
    }

    /**
    * Returns the simultaneous masking threshold at a particular frequency
    * @ freq The frequency of interest
    */
    double findThreshold(double freq);

    /** \return The number of auditoy filters in use.
    */
    int getBankCount(void){return bankCount;}
};
#endif //AUDIOMASKER_H_

