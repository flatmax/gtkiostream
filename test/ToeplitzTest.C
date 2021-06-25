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

#include <iostream>
using namespace std;
#include "DSP/Toeplitz.H"
using namespace Eigen;
int main(int argc, char *argv[]){
  Matrix<double, Dynamic, Dynamic> m(8,1);
  m.setRandom();
  Toeplitz<Matrix<double, Dynamic, Dynamic>> h(m);
  cout<<m<<'\n'<<endl;
  cout<<h<<endl;
  return 0;
}
