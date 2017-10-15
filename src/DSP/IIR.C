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
#include "DSP/IIR.H"

IIR::IIR()
{
//	std::cout<<__func__<<std::endl;
}

IIR::~IIR()
{
    //dtor
}

int IIR::reset(const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &Bin, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &Ain){
//    if (!(A.row(0).array()==Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>::Ones(1,A.cols())).all())
    if (!(Ain.row(0).array()==1.0).all())
        return IIRDebug().evaluateError(IIR_A0_ERROR);
    if (Ain.cols()!=Bin.cols())
        return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
    B=Bin;
    A=Ain;
    int maxRows=std::max(B.rows(),A.rows());
    mem=Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>::Zero(maxRows, A.cols());
    return 0;
}

int IIR::process(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &x, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const &y){
    if (x.cols()!=A.cols()){
        printf("Input channel count %lld mismatch to filter channel count %lld", (long long)x.cols(), (long long)A.cols());
        return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
    }
    if (y.cols()!=A.cols()){
        printf("Output channel count %lld mismatch to filter channel count %lld", (long long)y.cols(), (long long)A.cols());
        return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
    }
    if (x.rows()!=y.rows()){
        printf("Input sample count %lld not equal to output sample count %lld", (long long)x.rows(), (long long)y.rows());
        return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
    }

    if (y.rows() != yTemp.rows() && y.cols() != yTemp.cols())
        yTemp.resize(y.rows(), y.cols());

    for (int i=0; i<x.rows(); i++){
        mem.row(0)=-x.row(i);
        mem.row(0)=-(A*mem.block(0, 0, A.rows(), A.cols())).colwise().sum();
        yTemp.row(i)=(B*mem.block(0, 0, B.rows(), B.cols())).colwise().sum();
        for (int j=mem.rows()-1; j>0; j--)
            mem.row(j)=mem.row(j-1);
    }

    const_cast< Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& >(y)=yTemp;
    return 0;
}
