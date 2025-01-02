/* Copyright 2000-2025 Matt Flax <flatmax@flatmax.org>
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
#ifndef DCBLOCKER_H_
#define DCBLOCKER_H_

#include <Eigen/Dense>
#include <limits>

/**
 * @file DCBlocker.H
 * @brief DC Blocker filter implementation
 *
 * This class implements a simple DC blocking filter to remove DC offset from signals
 */
class DCBlocker {
    double dcA; ///< Filter coefficient
    Eigen::Matrix<double, 1, Eigen::Dynamic> prevX; ///< Previous input state
    Eigen::Matrix<double, 1, Eigen::Dynamic> prevY; ///< Previous output state
    Eigen::Matrix<double, 1, Eigen::Dynamic> yLocal; ///< The DC blocked value
public:
    Eigen::Matrix<double, 1, Eigen::Dynamic> dcBypass; ///< The DC value removed

    /**
     * @brief Apply DC blocking filter to input signal
     * @tparam Derived Eigen matrix type
     * @param x Input signal to process
     * @param yOut Output signal after DC blocking
     */
    template <typename Derived>
    inline void process(const Eigen::MatrixBase<Derived>& x, Eigen::MatrixBase<Derived> const & yOut) {
        static double dcA = 1. - .01;
        static Eigen::Matrix<double, 1, Eigen::Dynamic> prevX = Eigen::Matrix<double, Eigen::Dynamic, 1>::Zero(x.cols());
        static Eigen::Matrix<double, 1, Eigen::Dynamic> prevY = Eigen::Matrix<double, Eigen::Dynamic, 1>::Zero(x.cols());

        yLocal = x - prevX + dcA * prevY;
        // yLocal = x - prevX; // A single zero rolls in too slowly with F
        dcBypass = x - yLocal;
        prevX = x;
        prevY = yLocal; 
        const_cast<Eigen::MatrixBase<Derived>&>(yOut) = yLocal;
    }
};

#endif // DCBLOCKER_H_
