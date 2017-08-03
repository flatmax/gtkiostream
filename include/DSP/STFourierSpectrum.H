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
#ifndef STFOURIERSPECTRUM_H_
#define STFOURIERSPECTRUM_H_

/** Given a time domain (1D) waveform, get OverlapAdd to turn it into an overlapping matrix and then find the DFT of each window.
*/
class STFourierSpectrum : public OverlapAdd {
    public:
        /// Empty constructor
        STFourierSpectrum() {
                        assert("STFourierSpectrum:: still under development, not ready for use.");
        }

    /** Constructor specifying an overlap factor.
    \param factor the factor to overlap by.
    */
        STFourierSpectrum(float factor) : OverlapAdd(factor) {}

    /// Destructor
        virtual ~STFourierSpectrum() {}
};

#endif // STFOURIERSPECTRUM_H_
