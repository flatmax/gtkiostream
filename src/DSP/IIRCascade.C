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
#include "DSP/IIRCascade.H"

IIRCascade::IIRCascade()
{
    //ctor
}

IIRCascade::~IIRCascade()
{
    //dtor
}

int IIRCascade::process(const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> &x, Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> const &y){
  return IIRDebug().evaluateError(IIR_REQUIRE_COL_ERROR);
}

void IIRCascade::process(){
  for (int j=0; j<A.cols(); j++){
      for (int i=0; i<xTemp.rows(); i++){
          mem(0,j)=-xTemp(i,0);
          mem(0,j)=-(A.col(j)*mem.col(j).topRows(A.rows())).sum();
          yTemp(i,0)=(B.col(j)*mem.col(j).topRows(B.rows())).sum();
          for (int k=mem.rows()-1; k>0; k--)
              mem(k,j)=mem(k-1,j);
      }
      xTemp=yTemp;
  }
}

int IIRCascade::process(const Eigen::Matrix<double, Eigen::Dynamic, 1> &x, Eigen::Matrix<double, Eigen::Dynamic, 1> const &y){
    if (x.rows()!=y.rows()){
        printf("Input sample count %lld not equal to output sample count %lld", (long long)x.rows(), (long long)y.rows());
        return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
    }

    if (x.rows() != yTemp.rows())
        yTemp.resize(x.rows(), 1);
    if (x.rows() != xTemp.rows())
        xTemp.resize(x.rows(), 1);
    xTemp=x;

    process();

    const_cast< Eigen::Matrix<double, Eigen::Dynamic, 1>& >(y)=xTemp;
    return 0;
}

int IIRCascade::process(const Eigen::Matrix<float, Eigen::Dynamic, 1> &x, Eigen::Matrix<float, Eigen::Dynamic, 1> const &y){
    if (x.rows()!=y.rows()){
        printf("Input sample count %lld not equal to output sample count %lld", (long long)x.rows(), (long long)y.rows());
        return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
    }

    if (x.rows() != xTemp.rows())
        xTemp.resize(x.rows(), 1);
    if (x.rows() != yTemp.rows())
        yTemp.resize(x.rows(), 1);
    xTemp=x.cast<double>();
    process();

    const_cast< Eigen::Matrix<float, Eigen::Dynamic, 1>& >(y)=xTemp.cast<float>();
    return 0;
}

int IIRCascade::processStepped(const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &BStep, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &AStep){
  if ((BStep.cols()!=B.cols()) || (AStep.cols()!=A.cols()) || (B.cols()!=A.cols())){
      printf("BStep or AStep channel count (%lld, %lld) mismatch to filter channel count %lld", (long long)BStep.cols(), (long long)AStep.cols(), (long long)A.cols());
      return IIRDebug().evaluateError(IIR_CH_CNT_ERROR);
  }

  if ((BStep.rows()!=B.rows()) || (AStep.rows()!=A.rows())){
      printf("BStep order %lld not equal to B order %lld", (long long)BStep.rows(), (long long)B.rows());
      printf("OR AStep order %lld not equal to A order %lld", (long long)AStep.rows(), (long long)A.rows());
      return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
  }

  for (int j=0; j<A.cols(); j++){
      for (int i=0; i<xTemp.rows(); i++){
          mem(0,j)=-xTemp(i,0);
          mem(0,j)=-(A.col(j)*mem.col(j).topRows(A.rows())).sum();
          yTemp(i,0)=(B.col(j)*mem.col(j).topRows(B.rows())).sum();
          for (int k=mem.rows()-1; k>0; k--)
              mem(k,j)=mem(k-1,j);
          B.col(j)+=BStep.col(j); // step the filter coefficients on
          A.col(j)+=AStep.col(j);
      }
      xTemp=yTemp;
  }
  return 0;
}

int IIRCascade::process(const Eigen::Matrix<double, Eigen::Dynamic, 1> &x, Eigen::Matrix<double, Eigen::Dynamic, 1> const &y,
            const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &BStep, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &AStep){
    if (x.rows()!=y.rows()){
        printf("Input sample count %lld not equal to output sample count %lld", (long long)x.rows(), (long long)y.rows());
        return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
    }

    if (x.rows() != yTemp.rows())
        yTemp.resize(x.rows(), 1);
    if (x.rows() != xTemp.rows())
        xTemp.resize(x.rows(), 1);
    xTemp=x;

    processStepped(BStep, AStep);

    const_cast< Eigen::Matrix<double, Eigen::Dynamic, 1>& >(y)=xTemp;
    return 0;
}

int IIRCascade::process(const Eigen::Matrix<float, Eigen::Dynamic, 1> &x, Eigen::Matrix<float, Eigen::Dynamic, 1> const &y,
            const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &BStep, const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic> &AStep){
    if (x.rows()!=y.rows()){
        printf("Input sample count %lld not equal to output sample count %lld", (long long)x.rows(), (long long)y.rows());
        return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
    }

    if (x.rows() != yTemp.rows())
        yTemp.resize(x.rows(), 1);
    if (x.rows() != xTemp.rows())
        xTemp.resize(x.rows(), 1);
    xTemp=x.cast<double>();

    processStepped(BStep, AStep);

    const_cast< Eigen::Matrix<float, Eigen::Dynamic, 1>& >(y)=xTemp.cast<float>();
    return 0;
}
