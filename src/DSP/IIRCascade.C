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

int IIRCascade::process(const Eigen::Matrix<double, Eigen::Dynamic, 1> &x, Eigen::Matrix<double, Eigen::Dynamic, 1> const &y){
//    if (x.cols()!=A.cols()){
//        ostrstream errStr; errStr<<"Input channel count ("<<x.cols()<<") mismatch to filter channel count ("<<A.cols()<<")"<<ends;
//        return EQDebug().evaluateError(EQ_CH_CNT_ERROR, errStr.str());
//    }
//    if (y.cols()!=A.cols()){
//        ostrstream errStr; errStr<<"Output channel count ("<<y.cols()<<") mismatch to filter channel count ("<<A.cols()<<")"<<ends;
//        return EQDebug().evaluateError(EQ_CH_CNT_ERROR, errStr.str());
//    }
    if (x.rows()!=y.rows()){
        printf("Input sample count (%l) not equal to output sample count (%l)", x.rows(), y.rows());
        return IIRDebug().evaluateError(IIR_N_CNT_ERROR);
    }

    if (x.rows() != yTemp.rows())
        yTemp.resize(x.rows(), 1);
    if (x.rows() != xTemp.rows())
        xTemp.resize(x.rows(), 1);
    xTemp=x;

    for (int j=0; j<A.cols(); j++){
        for (int i=0; i<x.rows(); i++){
            mem(0,j)=-xTemp(i,0);
            mem(0,j)=-(A.col(j)*mem.col(j).topRows(A.rows())).sum();
            yTemp(i,0)=(B.col(j)*mem.col(j).topRows(B.rows())).sum();
            for (int k=mem.rows()-1; k>0; k--)
                mem(k,j)=mem(k-1,j);
        }
        xTemp=yTemp;
    }

    const_cast< Eigen::Matrix<double, Eigen::Dynamic, 1>& >(y)=xTemp;
    return 0;
}
