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

#include <octave/parse.h>
#include <octave/octave-config.h>
#include <octave/octave.h>
#include <octave/variables.h>
// #include <octave/oct-obj.h>
#include <octave/oct-map.h>
#include <octave/symtab.h>
//#include <octave/ov-scalar.h>
#include "Octave.H"
#include <iostream>

#include <Eigen/Dense>

#ifdef HAVE_OPENCV
#include <opencv2/opencv.hpp>
#endif

// taken from octave/toplev.h
typedef void (*octave_exit_func) (int);
extern OCTINTERP_API octave_exit_func octave_exit;
extern OCTINTERP_API void clean_up_and_exit (int, bool);

// extern OCTINTERP_API octave_value_list
// octave::feval (const string& name, const octave_value_list& args = octave_value_list (), int nargout = 0);

//extern void do_octave_atexit (void);
//extern void clean_up_and_exit (int retval);

extern void set_global_value (const string& nm, const octave_value& val);

// extern OCTINTERP_API bool octave_initialized;
extern OCTINTERP_API bool octave_interpreter_ready;

#define BASENAME_CNT 512 ///< the name max length

static void local_exit(int){}
void (*octave_exit) (int) = local_exit;

Octave::Octave(void) {
    init();
}

bool Octave::isReady(void) {
    return octave_interpreter_ready;
}

void Octave::init(void) {
    input=NULL;
    //output=NULL;
    input=new octave_value_list;
//    output=new octave_value_list;
    if (!input){ //} || !output){
        cerr<<"Octave::Octave couldn't malloc the input and output lists.";
        assert(-1);
    }
}

Octave::Octave(const vector<string> &args) {
    init();
    printf("NOTE: Newer versoins of Octave no longer support cmd line options.\n");
    startOctave(args);
}

void Octave::startOctave(const vector<string> &args) {
    int argc=args.size();
    vector<const char *> argsIn(argc);
    for (int i=0; i<argc; i++)
        argsIn[i]=&args[i][0];
    int embedded=1;
    octave_main((int)argsIn.size(), (char**)&argsIn[0], embedded);
}

Octave::~Octave() {
    if (input)
        delete input;
    input=NULL;
    stopOctaveAndExit();
}

void Octave::stopOctaveAndExit() {
    clean_up_and_exit (0, true);
}

void Octave::stopOctave() {
    //do_octave_atexit();
}

template<class TYPE>
vector<vector<vector<TYPE> > > &Octave::runM(const char* commandName, const vector<vector<vector<TYPE> > > &in, vector<vector<vector<TYPE> > > &out){
    //octave_value_list input(in.size()); ///< Input variables to pass to Octave
    if ((*input).length()<in.size())
         (*input).resize(in.size());

    // check that the number of columns are consistent.
    for (int i=0; i<in.size(); i++) { // cycle through each input
        int colCnt;
        for (int j=0; j<in[i].size(); j++){
            if (j==0)
                colCnt=in[i][j].size();
            else
            if (in[i][j].size()!=colCnt){
                //out.resize(1); out[0].resize(1); out[0][0].resize(1); // in future, this could be another method for returning on error.
                //out[0][0][0]=(TYPE)OCTAVE_INCONSISTENT_COL_CNT_ERROR;
                exit(OctaveDebug().evaluateError(OCTAVE_INCONSISTENT_COL_CNT_ERROR, string(". When trying to execute the command ")+commandName)); // no good way to pass back an error when columns are mismatched in size, so exit for now.
            }
        }
    }

    for (int i=0; i<in.size(); i++) { // cycle through each input
        int r=in[i].size();
        for (int j=0; j<r; j++){ // cycle through each row
            int c=in[i][j].size();
            if ((*input)(i).rows()!=r || (*input)(i).columns()!=c) // resize the matrix if necessary
                if (r==c && r==1) // scalar
                    (*input)(i)=octave_value(in[i][j][0]);
                else
                    (*input)(i)=Matrix(r, c);
            if (r>=1 && c>=1){ // if the value is a matrix
                octave_base_value *obv=(*input)(i).internal_rep();
                for (int k=0; k<c; k++)
                    obv->fast_elem_insert(j+k*r, (double)in[i][j][k]);
            }
        }
    }

    // execute
    octave_value_list output; ///< Output variables returned from Octave
    output = octave::feval(string(commandName), (*input));

    // unload outputs
    if (out.size() != output.length())
        out.resize(output.length());
    for (int i=0; i<out.size(); i++) {
        octave_base_value *obv=output(i).internal_rep();
        int r=output(i).rows(), c=output(i).columns();
        if (r!=out[i].size()) // resize the matrix if necessary
            out[i].resize(r);
        for (int j=0; j<r; j++){
            if (c!=out[i][j].size())
                out[i][j].resize(c);
            for (int k=0; k<c; k++)
                out[i][j][k]=(TYPE)obv->fast_elem_extract(j+k*r).double_value();
        }
    }

    return out;
}
template vector<vector<vector<double> > > &Octave::runM(const char* commandName, const vector<vector<vector<double> > > &in, vector<vector<vector<double> > > &out);
template vector<vector<vector<float> > > &Octave::runM(const char* commandName, const vector<vector<vector<float> > > &in, vector<vector<vector<float> > > &out);
template vector<vector<vector<long> > > &Octave::runM(const char* commandName, const vector<vector<vector<long> > > &in, vector<vector<vector<long> > > &out);
template vector<vector<vector<int> > > &Octave::runM(const char* commandName, const vector<vector<vector<int> > > &in, vector<vector<vector<int> > > &out);

template<class TYPE>
vector<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > &Octave::runM(const char* commandName, const vector<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > &in, vector<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > &out){
    //octave_value_list input(in.size()); ///< Input variables to pass to Octave
    if ((*input).length()<in.size())
         (*input).resize(in.size());

    // this code can most likely be sped up using Eigen::Matrix.block operations for memory copies.
    for (int i=0; i<in.size(); i++) { // cycle through each input
        int r=in[i].rows(), c=in[i].cols();
        if ((*input)(i).rows()!=r || (*input)(i).columns()!=c) // resize the matrix if necessary
            if (r==c && r==1) // scalar
                (*input)(i)=octave_value(in[i](0,0));
            else
                (*input)(i)=Matrix(r, c);
        if (r>=1 && c>=1){ // if the value is a matrix
            octave_base_value *obv=(*input)(i).internal_rep();
            for (int j=0; j<r; j++)
                for (int k=0; k<c; k++)
                    obv->fast_elem_insert(j+k*r, (double)in[i](j,k));
        }
    }

    octave_value_list output; ///< Output variables returned from Octave
    output = octave::feval(string(commandName), (*input));

    if (out.size() != output.length())
        out.resize(output.length());
    for (int i=0; i<out.size(); i++) {
        octave_base_value *obv=output(i).internal_rep();
        int r=output(i).rows(), c=output(i).columns();
        if (r!=out[i].rows() || c!=out[i].cols()) // resize the matrix if necessary
            out[i].resize(r, c);
        for (int j=0; j<r; j++)
            for (int k=0; k<c; k++)
                out[i](j,k)=obv->fast_elem_extract(j+k*r).double_value();
    }

    return out;
}
template vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> >  &Octave::runM(const char* commandName, const vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> > &in, vector<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> > &out);
template vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> >  &Octave::runM(const char* commandName, const vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > &in, vector<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> > &out);

octave_value_list Octave::runMWithInput(const char* commandName){
    octave_value_list output; ///< Output variables returned from Octave
    output = octave::feval(string(commandName), (*input));
    return output;
}

void Octave::runM(const char* commandName){
    //output = octave::feval(string(commandName), input, retCount);
    octave::feval(string(commandName), octave_value_list (), 0);
}

#ifdef HAVE_OPENCV
vector<cv::Mat> &Octave::runM(const char* commandName, const vector<cv::Mat> &in, vector<cv::Mat> &out){
    //octave_value_list input(in.size()); ///< Input variables to pass to Octave
    if ((*input).length()<in.size())
         (*input).resize(in.size());

    // this code can most likely be sped up using Eigen::Matrix.block operations for memory copies.
    for (int i=0; i<in.size(); i++) { // cycle through each input
        if (in[i].type() != CV_64F){
            cerr<<"Octave::runM : input matrix in["<<i<<"] openCV Matrix types must be CV_64F : error."<<endl;
            exit(OctaveDebug().evaluateError(OCTAVE_OPENCV_TYPE_ERROR));
        }
        int r=in[i].rows, c=in[i].cols;
        if ((*input)(i).rows()!=r || (*input)(i).columns()!=c) // resize the matrix if necessary
            (*input)(i)=Matrix(r, c);
        //Array<double> *m=dynamic_cast<Array<double>*>((*input)(i).internal_rep());
        octave_base_value *obv=(*input)(i).internal_rep();
        for (int j=0; j<r; j++){
            const double* Mj = in[i].ptr<double>(j);
            for (int k=0; k<c; k++)
                obv->fast_elem_insert(j+k*r, (double)Mj[k]);
                //obv->fast_elem_insert(j+k*r, in[i].at<double>(j,k));
        }
    }

    octave_value_list output; ///< Output variables returned from Octave
    //output = octave::feval(string(commandName), input, retCount);
    output = octave::feval(string(commandName), (*input));

    if (out.size() != output.length())
        out.resize(output.length());
    for (int i=0; i<out.size(); i++) {
        octave_base_value *obv=output(i).internal_rep();
        int r=output(i).rows(), c=output(i).columns();
        if (r!=out[i].rows || c!=out[i].cols) // resize the matrix if necessary
            out[i].create(r, c, CV_64F);
        if (out[i].type() != CV_64F){
            cerr<<"Octave::runM : output matrix out["<<i<<"] openCV Matrix types must be CV_64F : error."<<endl;
            exit(OctaveDebug().evaluateError(OCTAVE_OPENCV_TYPE_ERROR));
        }
        for (int j=0; j<r; j++){
            double* Mj = out[i].ptr<double>(j);
            for (int k=0; k<c; k++)
                Mj[k]=obv->fast_elem_extract(j+k*r).double_value();
                //out[i].at<double>(j,k)=obv->fast_elem_extract(j+k*r).double_value();
        }
    }

    return out;
}
#endif

int Octave::setGlobalVariable(const string &name, Matrix &m) {
    vector<string> varNames; // hold all the global var levels and name: level1.level2.name
    istringstream tokens(name);
    char baseName[BASENAME_CNT];
    if (tokens.getline(baseName, BASENAME_CNT, '.')) { // if there is a base name.
        char subName[BASENAME_CNT];
        while (tokens.getline(subName, BASENAME_CNT, '.'))
            varNames.push_back(string(subName));

        if (varNames.size()) {
            octave_value ov=get_global_value(string(baseName), true);
            set_global_value (string(baseName), setGlobalSubVariable(varNames, m, 0, ov));
        } else
            set_global_value (string(baseName), m);
        return 0;
    } else
        return OCTAVE_NOBASE_ERROR;
}

Octave_map Octave::setGlobalSubVariable(const vector<string> &varNames, const Matrix &m, int index, octave_value &base) {
    cout<<"setGlobalVariable varName = "<<varNames[index]<<" index = "<<index<<" octave_value "<<endl;
    Octave_map om;
    Matrix size=base.size();
    int whichIdx;
    cout<<"here"<<endl;
    if (size(0,0)!=0) { // exists, so do a search ...
        cout<<"here1"<<endl;
        if (index+1<varNames.size()){
            cout<<"here1a"<<endl;
            whichIdx=base.map_keys().lookup(varNames[index+1]); // map_keys returns a string_vector
        } else { // at the last entry and this must be the location to put
            cout<<"here1b"<<endl;
            //cout<<"last var name, must be location in current global struct ... appending"<<endl;
//            if (base.is_cell())
//                om=octave_map(base.cell_value().elem(0));
//            else
            om=base.map_value();
//            om=base.cell_value();
            octave_value nv=m;
            if (nv.is_cell())
                cout<<"created a cell"<<endl;
            om.assign(varNames[index], nv);
//            om=base(0);
//            om.setfield(varNames[index], Cell(m));
            return om;
        }
    } else // not present
        whichIdx=0;

    cout<<"here2"<<endl;
    if (whichIdx) { // if present, then keep parsing and assign
        cout<<"here3"<<endl;
        cout<<varNames[index+1]<<" found in the map at location "<<whichIdx<<", appending"<<endl;
        om=base.map_value();
        //octave_value ov=om.seek(varNames[index])->second.elem(0);
        //octave_value ov=om.key(om.seek(varNames[index]));
        //octave_value ov=om.contents(om.seek(varNames[index])).elem(0);
        octave_value ov=om.getfield(varNames[index]);
        om.assign(varNames[index], octave_value(setGlobalSubVariable(varNames, m, index+1, ov)));
        return om;
    } else { // not present, so start creating a new branch
        cout<<"here4"<<endl;
        cout<<varNames[index]<<" not found in the map, creating new"<<endl;
        if (varNames.size()>index+1) { // more to walk through
            cout<<varNames[index]<<" not last, walking more"<<endl;
            Octave_map ret; //(varNames[index].c_str(), dim_vector(1,1));
            //Cell ret;
            createGlobalSubVariable(varNames, m, index+1, ret);
            om.assign(varNames[index].c_str(), octave_value(ret));
            //om.setfield(varNames[index].c_str(), ret);
            return om;
        }
        cout<<"here5"<<endl;
        // no more to walk through, assign here
        cout<<varNames[index]<<" last, appending"<<endl;
        om.assign(varNames[index].c_str(), octave_value(m));
        return om;
    }
}
Octave_map Octave::createGlobalSubVariable(const vector<string> &varNames, const Matrix &m, int index, Octave_map &base) {

    //cout<<"setGlobalVariable varName = "<<varNames[index]<<" index = "<<index<<" Octave_map "<<endl;
    if (index<varNames.size()-1) {
        //cout<<"not last, creating new to append to"<<endl;
        Octave_map ret;
        base.assign(varNames[index].c_str(), createGlobalSubVariable(varNames, m, index+1, ret));
        return base;
    }
    //cout<<"last, appending to"<<endl;
    base.assign(varNames[index].c_str(), octave_value(m));
    return base;
}

Matrix *Octave::newMatrix(int r, int c){
    return new Matrix(r,c);
}
void Octave::setMatrixElem(Matrix *m, int i, int j, double val){
    m->elem(i,j)=val;
}

void Octave::deleteMatrix(Matrix *m){
    delete m;
}

// void Octave::clearAll(void){
//     symbol_table::clear_all();
// }
