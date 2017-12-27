/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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

#ifndef AUDIOMASKCOMMON_H_
#define AUDIOMASKCOMMON_H_

#ifdef _MSC_VER

#ifndef MS_ROUND
/// Microsoft doesn't have a round function
#include <math.h>
double round(double number) {
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}
#else
extern double round(double);
#endif
#ifndef rint
#define rint round
#endif
#endif

#if !defined(_MSC_VER) && !defined(__CYGWIN__) && !defined(__MINGW32__) // Mingw/Microsoft doesn't have this header
#include <values.h>
#else
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <float.h>
#endif
#define MAXDOUBLE DBL_MAX
#endif

#endif //AUDIOMASKCOMMON_H_
