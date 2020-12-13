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

#ifndef IIRCASCADE_H
#define IIRCASCADE_H

#include <DSP/IIR.H>

/** Class to cascade IIR filters. Each IIR coefficient column represents a cascade section.
*/
class IIRCascade : public IIR
{
    Eigen::Matrix<double, Eigen::Dynamic, 1> xTemp; ///< Temporary casecading signal

    void process(); ///< Inner process
    int processStepped(const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &BStep, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &AStep);
public:
    IIRCascade();
    virtual ~IIRCascade();
    /** Cascade IIR filters (columns) with an input signal
    \param x The input to cascade through all of the IIR columns
    \param[out] y The output response of the IIR filter casecade
    */
    int process(const Eigen::Matrix<double, Eigen::Dynamic, 1> &x, Eigen::Matrix<double, Eigen::Dynamic, 1> const &y);
    int process(const Eigen::Matrix<float, Eigen::Dynamic, 1> &x, Eigen::Matrix<float, Eigen::Dynamic, 1> const &y);
    int process(const Eigen::Matrix<double, Eigen::Dynamic, 1> &x, Eigen::Matrix<double, Eigen::Dynamic, 1> const &y,
                const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &BStep, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &AStep);
    int process(const Eigen::Matrix<float, Eigen::Dynamic, 1> &x, Eigen::Matrix<float, Eigen::Dynamic, 1> const &y,
                const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &BStep, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &AStep);

    /** Used to catch input arguments which are of non-static single column format. process will not work with these types
    */
    int process(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &x, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const &y);
};
#endif // IIRCASCADE_H
