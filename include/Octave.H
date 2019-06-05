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
#ifndef OCTAVE_H_
#define OCTAVE_H_

#include <Debug.H>

#ifndef OCTAVE_ERROR_OFFSET
#define OCTAVE_ERROR_OFFSET -2000
#endif
//#define GEN_ERROR_OCTAVE(errNo) OCTAVE_ERROR_OFFSET+errNo

//#define OCTAVE_ERROR_INCORRECT_RETCOUNT GEN_ERROR_OCTAVE(-1)
//#define OCTAVE_ERROR_INCORRECT_RETCOUNT_STR std::string("Octave returned the incorrect number of elements - error")

#define OCTAVE_NOBASE_ERROR -1+OCTAVE_ERROR_OFFSET
#define OCTAVE_OPENCV_TYPE_ERROR -2+OCTAVE_ERROR_OFFSET
#define OCTAVE_INCONSISTENT_COL_CNT_ERROR -3+OCTAVE_ERROR_OFFSET

/** Debug class for Octave
*/
class OctaveDebug : virtual public Debug {
public:
    /** Constructor defining all debug std::strings which match the debug defined variables
    */
    OctaveDebug() {
#ifndef NDEBUG
    errors[OCTAVE_NOBASE_ERROR]=std::string("Octave: You didn't specify a global name. Global variables must start with a std::string and have sub std::strings separated by '.', e.g. 'base.name'");
    errors[OCTAVE_OPENCV_TYPE_ERROR]=std::string("Octave: OpenCV Mat types must all be CV_64F. ");
    errors[OCTAVE_INCONSISTENT_COL_CNT_ERROR]=std::string("Octave: You have specified a matrix with inconsistent column sizes, for the type vector<vector<vector<TYPE> > >.");
#endif
    }

    /// Destructor
    virtual ~OctaveDebug() {}
};

#include <Eigen/Dense>

#include <string>
#include <vector>
using namespace std;

class Matrix; ///< Octave's Matrix
class octave_value; ///< Octave's octave octave_value class
//class Octave_map; ///< Octave's Octave_map class
class octave_map;
#define Octave_map octave_map
class octave_value_list; ///< Octave's octave_value_list class

#ifdef HAVE_OPENCV
namespace cv {
class Mat; ///< The openCV matrix type
};
#endif

/** Class to interface to GNU Octave (octave.org).
Requires octave headers installed on the system.

First implementation is limited to matrix inputs and outputs to arbitrary .m files.

Usage like so :
\code
    // start octave
    vector<std::string> args(3); args[0]=std::string("--silent"); args[1]=std::string("--path"); args[2]=std::string("/tmp");
    Octave octave(args);

    // create the input and output vectors. Load the input vector
    vector<Eigen::Matrix<float, Dynamic, Dynamic> > input(1), output;
    Eigen::Matrix<float, Dynamic, Dynamic> a(10,10); Eigen::Matrix<float, Dynamic, Dynamic>::Zero(10,10);
    input[0]=a;

    // run the file yourMFile.m with the input arguments
    output = octave.runM("yourMFile", input); // where yourMFile.m is on the path say /tmp/yourMFile.m

    // output now holds the results

\endcode

*/
class Octave {
    /** Method to create an octave matrix of a particular size : Matrix(r,c).
    \sa setMatrixElem, deleteMatrix
    \param r The number of rows
    \param c The number of cols
    */
    Matrix *newMatrix(int r, int c);

    /** Method to set an octave matrix : m.elem(i,j)=val;
    \sa newMatrix, deleteMatrix
    \param m The matrix
    \param i The row
    \param j The col
    \param val The value to set.
    */
    void setMatrixElem(Matrix *m, int i, int j, double val);

    /** Delete a previously created octave matrix.
    \param m The matrix to delete
    \sa newMatrix, setMatrixElem
    */
    void deleteMatrix(Matrix *m);

    /// Initialisation method common to all constructors.
    void init(void);

public:
    octave_value_list *input; ///< The inputs to pass to the octave .m file.

    /** Constructor starts an octave instance using args to pass to octave
    \param args Arguments to pass into octave
    */
    Octave(const vector<std::string> &args);

    /** Constructor starts an octave instance using args to pass to octave
    \param args Arguments to pass into octave
    */
    void startOctave(const vector<std::string> &args);

    /** Construct Octave, but don't start, make sure you call startOctave before you try to runM
    */
    Octave(void);

    /// Destructor
    virtual ~Octave();

    /** Check if octave is ready.
    \return true if ready
    */
    bool isReady(void);


    /** Stop the octave instance and exit.
    */
    void stopOctaveAndExit();

    /** Stop the octave instance.
    */
    void stopOctave();

    /** Runs the matlab script commandName+".m", passing in a vector of std matrices (a vector of a vector) and returning the out vector of matrices (vector of a vector).
    NOTE: All vector<vector<vector<TYPE> > > data types are vectors of matrices, where the vector.size() are the number of rows and vector[0].size() is the number of columns.
    \tparam TYPE The data type to input and return, must be a scalar type such as int, float, double, etc.
    \param commandName The .m file name to run
    \param in The vector of matrices (the vector of vectors of vectors) to input to the .m file.
    \param out The vector of matrices (the vector of vectors of vectors) output from the .m file.
    \return The a reference to the variable out.
    */
    template<class TYPE>
    vector<vector<vector<TYPE> > > &runM(const char* commandName, const vector<vector<vector<TYPE> > > &in, vector<vector<vector<TYPE> > > &out);

    /** Runs the matlab script commandName+".m", passing in a vector of eigen matrices and returning the out vector of matrices which contains the results.
    \tparam TYPE The Eigen::Matrix types to input and return
    \param commandName The .m file name to run
    \param in The vector of Eigen::Matrix (the vector of matrices) to input to the .m file.
    \param out The vector of Eigen::Matrix (the vector of matrices) output from the .m file.
    \return The a reference to the variable out.
    */
    template<class TYPE>
    vector<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > &runM(const char* commandName, const vector<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > &in, vector<Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> > &out);

    /** Runs the matlab script commandName+".m", this version takes no input (you can specify input by manually filling in the input class variable)
    \param commandName The .m file name to run
    \return The output variables are returned in an Octave octave_value_list
    */
    octave_value_list runMWithInput(const char* commandName);

    /** Runs the matlab script commandName+".m", this version takes no input and output and returns no output.
    \param commandName The .m file name to run
    */
    void runM(const char* commandName);

#ifdef HAVE_OPENCV
    /** Runs the matlab script commandName+".m", passing in a vector of openCV CV::Mat matrices and returning the same altered vector of matrices passed in.
    Currently this method requires openCV formats of double or CV_64F. The out matrices are create to be of format CV_64F, if they don't exist. If you provide the out matrices, ensure they are of format CV_64F.
    \param commandName The .m file name to run
    \param in The vector of cv::Mat (the vector of matrices) to input to the .m file.
    \param out The vector of cv::Mat (the vector of matrices) output from the .m file.
    \return The a reference to the variable out.
    */
    vector<cv::Mat> &runM(const char* commandName, const vector<cv::Mat> &in, vector<cv::Mat> &out);
#endif

//    void setGlobalVariable(const std::string &name) {
////        std::string token;
////        istd::stringstream tokens(name);
////        octave_value none;
//        set_global_value (name, octave_value());
//    }


    /** Method to set an Eigen3 matrix or array as a global variable.
    \param name The global variable name of the following format "base1.base2.name"
    \param var An Eigen3 Matrix, Vector or Array type.
    */
    template<typename Derived>
    int setGlobalVariable(const std::string &name, const Eigen::DenseBase<Derived> &var) {
        Matrix *m=newMatrix(var.rows(),var.cols());
        for (int j=0; j<var.rows(); j++)
            for (int k=0; k<var.cols(); k++)
                setMatrixElem(m, j, k, (double)var(j,k));
        int ret=setGlobalVariable(name, *m);
        deleteMatrix(m);
        return ret;
    }

    /** Method to set an octave matrix as a global variable.
    \param name The global variable name of the following format "base1.base2.name"
    \param m An octave matrix to use for setting.
    */
    int setGlobalVariable(const std::string &name, Matrix &m);

    /** Method helpiing setGlobalVariable parse the global variabl structure at the base of name.
    This method searches an already existing octave_value class for the desired varName[index+1] path.
    \param varNames The tokenised list of base names and find name from setGlobalVariable where the base1 name is stripped off.
    \param m The matrix to set the global variable to.
    \param index The current index into the varNames vector.
    \param base The octave_value to search for the varNames index+1 in.
    */
    Octave_map setGlobalSubVariable(const vector<std::string> &varNames, const Matrix &m, int index, octave_value &base);

    /** This method is used to construct the new branch from varNames[index] onwards.
    \param varNames The tokenised list of base names and find name from setGlobalVariable where the base1 name is stripped off.
    \param m The matrix to set the global variable to.
    \param index The current index into the varNames vector.
    \param base The octave_value to search for the varNames index+1 in.
    */
    Octave_map createGlobalSubVariable(const vector<std::string> &varNames, const Matrix &m, int index, Octave_map &base);

    /** Clear everything.
    */
    // void clearAll(void);

    /** Convert a std::string to a matrix.
    \param str The std::string.
    \return A matrix containing the ascii representation of the str.
    */
    template<typename TYPE>
    Eigen::Matrix<TYPE, Eigen::Dynamic, 1> str2Mat(char *str){
        return str2Mat<TYPE>(std::string(str));
    }

    /** Convert a std::string to a matrix.
    \param str The std::string.
    \return A matrix containing the ascii representation of the str.
    */
    template<typename TYPE>
    Eigen::Matrix<TYPE, Eigen::Dynamic, 1> str2Mat(std::string str){
        return Eigen::Map<Eigen::Matrix<char,Eigen::Dynamic,1> >((char*)str.c_str(), str.size()).cast<TYPE>();
    }

    /** Convert a std::string to a vector.
    \param str The std::string.
    \return A matrix containing the ascii representation of the str.
    */
    template<typename TYPE>
    vector<TYPE> str2Vec(std::string str){
        vector<TYPE> vec;
        std::copy(str.begin(), str.end(), back_inserter(vec));
        return vec;
    }
};
#endif // OCTAVE_H_
