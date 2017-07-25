#include "fft/RealFFT.H"

void RealFFT::createPlan(void) {
    if (data) {
        //fftw3
        fwdPlan=fftw_plan_r2r_1d(data->getSize(), data->in, data->out, FFTW_R2HC, PLANTYPE);
        invPlan=fftw_plan_r2r_1d(data->getSize(), data->out, data->in, FFTW_HC2R, PLANTYPE);
    }
}

void RealFFT::destroyPlan(void) {
    if (data) {
        fftw_destroy_plan(fwdPlan);
        fftw_destroy_plan(invPlan);
    }
}

RealFFT::RealFFT(void) {
    data=NULL;
    createPlan();
}

RealFFT::RealFFT(RealFFTData *d) {
    //  cout <<"RealFFT init:"<<this<<std::endl;
    data=d;
    createPlan();
}

RealFFT::~RealFFT(void) {
    destroyPlan();
}

void RealFFT::switchData(RealFFTData *d) {
    destroyPlan();
    data=d;
    createPlan();
}

void RealFFT::switchData(RealFFTData &d) {
    switchData(&d);
}

void RealFFT::fwdTransform() {
    if (!data)
        printf("RealFFT::fwdTransform : data not present, please switch data");
    else
        fftw_execute(fwdPlan);
}

void RealFFT::invTransform() {
    if (!data)
        printf("RealFFT::invTransform : data not present, please switch data");
    else
        fftw_execute(invPlan);
}

#include "config.h"
#ifdef HAVE_EMSCRIPTEN
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(RealFFT_ex) {
emscripten::class_<RealFFT>("RealFFT")
    .constructor() // empty constructor - requires switchData to be called
    .constructor<RealFFTData*>() // the constructor takes in a data class
    .function("switchData", emscripten::select_overload<void(RealFFTData&)>(&RealFFT::switchData))
    .function("fwdTransform", &RealFFT::fwdTransform)
    .function("invTransform", &RealFFT::invTransform);
}
#endif
