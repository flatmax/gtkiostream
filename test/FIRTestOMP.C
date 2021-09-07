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

#define DSP_FIR_USE_OMP
#include "DSP/FIR.H"
#include <iostream>
using namespace std;
int main(int argc, char *argv[]){
  int MProcessors = omp_get_max_threads();
  omp_set_num_threads(MProcessors);
  // omp_set_num_threads(4);
  printf("MProcessors=%d, omp_get_num_threads()=%d\n", MProcessors, omp_get_num_threads());
  Eigen::initParallel();
  fftw_make_planner_thread_safe();
	if (fftw_init_threads()==0){
		printf("couldn't init threads for fftw\n");
		return -1;
	}
 	fftw_plan_with_nthreads(MProcessors);

    int N=1024*128;
    int chCnt=32;

    // Generate a filter
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> h;
    h=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(N,chCnt);

    // Generate the input data and reserve the output data space
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> x, y;
    x=Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>::Random(1024,chCnt);
    y.setZero(x.rows(), x.cols());

    // instantiate the FIR object
    FIR<double> fir;
    fir.init(1024);
    fir.loadTimeDomainCoefficients(h);
    for (int i=0; i<128; i++) // apply the filter in windows of N samples
      fir.filter(x, y);

    return 0;
}
