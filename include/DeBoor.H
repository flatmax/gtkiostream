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
#ifndef DEBOOR_H
#define DEBOOR_H

#include <vector>
//#include <array>
using namespace std;

/** DeBoor's spline implementation : http://pages.cs.wisc.edu/~deboor
*/
class DeBoor {
    float breakp[5]; ///< break for the cubic smoothing spline pp-representation
    float coef[4][4]; ///< coef for the cubic smoothing spline pp-representation
    int l; ///< Number of poly. pieces making up the cubic smoothing spline pp-rep.

    float **v; ///< work array
    float **a; ///< results array
    int N; ///< The number of samples int he work and results array

//    vector<array<float, 7>> v; ///< work array
//    vector<array<float, 4>> a; ///< results array

    /** Create the matrices v and a, setting N to n
    \param n The number of rows to create in v and a, on exit N=n
    */
    void createMatrices(int n);

    /** Create a matrix containing N rows
    \param k The number of columns
    \return The created matrix
    */
    float **&createMatrix(int k);
    void deleteMatrices(); ///< Delete memory

    /** Delete a matrix
    \param m The matrix of pointers to delete
    */
    void deleteMatrix(float **m);

    /// The result of csaps
    vector<float> resultCSAPS;
public:
    DeBoor();
    virtual ~DeBoor();

    /** Cubic smoothing spline.
    \param x The ordinates
    \param y The co-ordinates
    \param dy The uncertainty estimate >0
    \param n The number of samples
    \param s The smoothing parameter
    */
    void csaps(float *x, float *y, float *dy, int n, float s);

    float operator[](int i);
};

#endif // DEBOOR_H
