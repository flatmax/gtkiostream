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

#ifndef HANKEL_H
#define HANKEL_H

// Debug
#include "Debug.H"
#define HANKEL_SIZE_ERROR HANKEL_ERROR_OFFSET-1 ///< Error when the requested number of rows is larger then the available number of rows
#define HANKEL_COLS_ERROR HANKEL_ERROR_OFFSET-2 ///< Error when a vector is not provided.


class HankelDebug :  virtual public Debug  {
public:
    HankelDebug(){
#ifndef NDEBUG
errors[HANKEL_SIZE_ERROR]=std::string("Hankel :: You gave a matrix with rows <= N\n");
errors[HANKEL_COLS_ERROR]=std::string("Hankel :: You didn't provide a vector, you gave either nothing or a matrix. I require a vector.\n");
#endif // NDEBUG
    }
};

#include <Eigen/Dense>
/** Create a Hankel matrix given a vetor
*/
template<typename Derived>
class Hankel : public Derived {
public:
  Hankel(const Derived &A, int N){
    int err=0;
    if (A.rows()<N)
      err=HANKEL_SIZE_ERROR;
    if (A.cols()!=1)
      err=HANKEL_COLS_ERROR;
    if (err)
      HankelDebug().evaluateError(err);
    else {
      this->resize(N, A.rows()-N+1);
      int i=0;
      while (i<(A.rows()-N+1))
        this->col(i++)=A.block(i,0,N,1);
    }
  }
};
#endif // HANKEL_H
