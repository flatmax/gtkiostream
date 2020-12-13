/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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
#ifndef FFTCOMMON_H_
#define FFTCOMMON_H_

#define PLANTYPE FFTW_ESTIMATE

#include <fftw3.h>
#ifndef fftw_real
#define fftw_real double ///< use double by default
#endif
#define c_re(c) ((c)[0]) ///< The real part of the complex number
#define c_im(c) ((c)[1]) ///< The imaginary part of the complex number

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#if !defined(_MSC_VER) && !defined(__CYGWIN__) && !defined(__MINGW32__) // Mingw/Microsoft doesn't have this header
#include <values.h>
#else
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <float.h>
#endif
#define MAXDOUBLE DBL_MAX
#endif

#endif // FFTCOMMON_H_
