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
#include "DSP/IIR.H"

IIR::IIR()
{
//	std::cout<<__func__<<std::endl;
}

IIR::~IIR()
{
    //dtor
}

// int IIR::reset(const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &Bin, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &Ain, Eigen::Dynamic, Eigen::Dynamic> &memIn){
//   int ret=reset(Bin, Ain);
//   if (ret!=0)
//     return ret;
//   return setMem(memIn);
// }
//
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

int IIR::setMem(const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &memIn){
  if (mem.cols()!=memIn.cols())
      return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
  if (mem.rows()!=memIn.rows())
      return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
  mem=memIn;
  return 0;
}

int IIR::setMem(const IIR &iir){
  if (mem.rows()!=iir.mem.rows())
      return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
  unsigned int ch=std::min(mem.cols(), iir.mem.cols());
  mem.block(0,0,mem.rows(),ch)=iir.mem.block(0,0,mem.rows(),ch);
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

int IIR::process(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &x, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const &y,
            const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &BStep, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &AStep){
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

    if ((BStep.cols()!=B.cols()) || (AStep.cols()!=A.cols()) || (B.cols()!=A.cols())){
        printf("BStep or AStep channel count (%lld, %lld) mismatch to filter channel count %lld", (long long)BStep.cols(), (long long)AStep.cols(), (long long)A.cols());
        return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
    }

    if ((BStep.rows()!=B.rows()) || (AStep.rows()!=A.rows())){
        printf("BStep order %lld not equal to B order %lld", (long long)BStep.rows(), (long long)B.rows());
        printf("OR AStep order %lld not equal to A order %lld", (long long)AStep.rows(), (long long)A.rows());
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
        B+=BStep; // step the filter coefficients on
        A+=AStep;
    }

    const_cast< Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& >(y)=yTemp;
    return 0;
}


// void IIR::copyTo(IIR iirIn){
//   if (B.cols()!=iirIn.B.cols()){
//       printf("B channel count %lld mismatch to iirIn.B channel count %lld", (long long)B.cols(), (long long)iirIn.B.cols());
//       return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
//   }
//   if (A.cols()!=iirIn.A.cols()){
//       printf("A channel count %lld mismatch to iirIn.A channel count %lld", (long long)A.cols(), (long long)iirIn.A.cols());
//       return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
//   }
//   if (mem.cols()!=iirIn.mem.cols()){
//       printf("mem channel count %lld mismatch to iirIn.mem channel count %lld", (long long)mem.cols(), (long long)iirIn.mem.cols());
//       return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
//   }
//   unsigned int ch=::min(B.cols(), iirIn.B.cols());
//   iirIn.B.block(0,0,iirIn.B.rows(),ch)=B.block(0,0,B.rows(),ch);
//   iirIn.A.block(0,0,iirIn.A.rows(),ch)=A.block(0,0,A.rows(),ch);
//   iirIn.mem.block(0,0,iirIn.mem.rows(),ch)=mem.block(0,0,mem.rows(),ch);
// }
