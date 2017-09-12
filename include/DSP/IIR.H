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

#ifndef IIR_H
#define IIR_H

// Debug
#include "Debug.H"
#define IIR_A0_ERROR IIR_ERROR_OFFSET-1 ///< Error when feedback coefficient A0 is not = 1
#define IIR_CH_CNT_ERROR IIR_ERROR_OFFSET-2 ///< Channel count mismatch error
#define IIR_N_CNT_ERROR IIR_ERROR_OFFSET-3 ///< Channel count mismatch error
#define IIR_REQUIRE_COL_ERROR IIR_ERROR_OFFSET-4 ///< Channel count mismatch error
#define IIR_REQUIRE_MATRIX_ERROR IIR_ERROR_OFFSET-5 ///< Channel count mismatch error

class IIRDebug :  virtual public Debug  {
public:
    IIRDebug(){
#ifndef NDEBUG
        errors[IIR_A0_ERROR]=std::string("IIR feedback coefficient a0 != 1. ");
        errors[IIR_CH_CNT_ERROR]=std::string("The channel counts aren't the same. ");
        errors[IIR_N_CNT_ERROR]=std::string("The sample counts aren't the same. ");
        errors[IIR_REQUIRE_COL_ERROR]=std::string("The Matrix must be defined as a single col matrix, not dynamic. ");
        errors[IIR_REQUIRE_MATRIX_ERROR]=std::string("The Matrix must be defined as a Matrix, not Array. ");

#endif // NDEBUG
    }
};

#include <Eigen/Dense>
#include <strstream>

/** An IIR filter. The Direct Form II algorithm doesn't suit signals which get large, i.e. 1e12. Best to use this direct form II
for signal which are bounded small, such as acoustic signals -1<=x<=1
*/
class IIR {
protected:
    Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> B; // feed forward
    Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> A; // feed back
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> yTemp; // temporary output variables
    Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> mem; // memory

public:
    IIR();
    virtual ~IIR();
    int reset(const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &Bin, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &Ain);
    int reset(){
        mem.setZero();
        return 0;
    }

    /** Direct form II algorithm */
    int process(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &x, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const &y);
    int getChannelCount(){return B.cols();}
    void resetMem(){mem.setZero();}
};
#endif // IIR_H
