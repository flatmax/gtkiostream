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

#ifndef TOEPLITZ_H
#define TOEPLITZ_H

// Debug
#include "Debug.H"
#define TOEPLITZ_COLS_ERROR TOEPLITZ_ERROR_OFFSET-1 ///< Error when a vector is not provided.


class ToeplitzDebug :  virtual public Debug  {
public:
    ToeplitzDebug(){
#ifndef NDEBUG
errors[TOEPLITZ_COLS_ERROR]=std::string("Toeplitz :: You didn't provide a vector, you gave either nothing or a matrix. I require a vector.\n");
#endif // NDEBUG
    }
};

#include <Eigen/Dense>
/** Create a Toeplitz matrix given a vetor
*/
template<typename Derived>
class Toeplitz : public Derived {
public:
  Toeplitz(const Derived &A){
    int err=0;
    if (A.cols()!=1)
      err=TOEPLITZ_COLS_ERROR;
    if (err)
      ToeplitzDebug().evaluateError(err);
    else {
      int N=A.rows();
      this->resize(N, N);
      this->setZero();
      for (int i=0; i<N; i++)
        this->col(i).bottomRows(N-i)=A.topRows(N-i);
    }
  }
};
#endif // TOEPLITZ_H
