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
#ifndef DECOMPOSITION_H_
#define DECOMPOSITION_H_

#include "Octave.H"
#include "AudioMask/AudioMasker.H"
#include "DSP/OverlapAdd.H"

#include <Debug.H>

#define DECOMPOSITION_NODATA_ERROR DECOMPOSITION_ERROR_OFFSET-1 ///< Error when the data matrix is zero in either dimension.

/** Debug class for Decomposition
*/
class DecompositionDebug : public OverlapAddDebug, public AudioMaskerDebug {
public:
    /** Constructor defining all debug strings which match the debug defined variables
    */
    DecompositionDebug() {
#ifndef NDEBUG
    errors[DECOMPOSITION_NODATA_ERROR]=string("Decomposition: There is no data to process, please run the Decomposition::OverlapAdd::loadData method first.");
#endif
    }

    /// Destructor
    virtual ~DecompositionDebug() {}
};

/** Subspace decomposition class.
Decomposes a 1D waveform into tonal and noise subspaces.
\tparam TYPE Specifies the type of the data held in the matrix, e.g. float, double
*/
template<typename TYPE>
class Decomposition : public OverlapAdd<TYPE> {

    AudioMasker masker; ///< The audio masking model, used to whiten the signal before decomposition.

    Octave octave; ///< The octave instance

public:
    /// Constructor
    Decomposition();
    /// Destructor
    virtual ~Decomposition();

    /** For a previously loaded signal, decompose into noise and tonal subspaces.
    */
    int findSubSpace(void);
};

#endif // DECOMPOSITION_H_
