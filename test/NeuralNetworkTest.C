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

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
using namespace std;

#include "NeuralNetwork.H"

/* Function to read double data from file.
*/
Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> loadFromFile(string fileName){
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix;
    ifstream input(fileName.c_str(), ios::binary); // open the file
    if (input){
        double r, c;
        input.read( reinterpret_cast<char*>( &r), sizeof(r));
        input.read( reinterpret_cast<char*>( &c), sizeof(c));
//        cout<<"rows "<<r<<" cols "<<c<<endl;
        matrix.resize(r,c);
        for (int i=0; i<c; i++)
            for (int j=0; j<r; j++)
                input.read( reinterpret_cast<char*>( &matrix(j,i)), sizeof(double));
//        cout<<matrix<<endl;
        if (matrix.rows()==1)
            matrix.transposeInPlace();
    }
    return matrix;
}

int main(int argc, char *argv[]){
    // To construct a network, we load some weights and biases from file and create a layer using them
    // This is done for each layer.
    // Finally a NeuralNetwork is created to execute each layer

    // load the input layer weights and biases
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> weights=loadFromFile(string("testVectors/inputWeights.dat"));
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> bias=loadFromFile(string("testVectors/inputBias.dat"));

    // Begin constructing the neural network topology
    vector<NeuralLayer<double> *> networkLayers;
    networkLayers.push_back(new TanhLayer<double>(weights, bias));

    // load the hidden layer weights and biases
    weights=loadFromFile(string("testVectors/hiddenWeights.dat"));
    bias=loadFromFile(string("testVectors/hiddenBias.dat"));
    networkLayers.push_back(new TanhLayer<double>(weights, bias));

    // load the output layer weights and biases
    weights=loadFromFile(string("testVectors/outputWeights.dat"));
    bias=loadFromFile(string("testVectors/outputBias.dat"));

    networkLayers.push_back(new SigmoidLayer<double>(weights, bias));

    // setup some input
    Eigen::Matrix<double, Eigen::Dynamic, 1> input(10,1);
    input<<0.8333,0.8333,0.8333,0.8333,0.8333,0.6871,0.5833,0.4371,0.3333,0.4000;

    NeuralNetwork<double> nn;
    nn.activate(networkLayers, input);
//cout<<"layer 0 output"<<endl;
//    cout<<networkLayers[0]->output<<endl;
//cout<<"layer 1 output"<<endl;
//    cout<<networkLayers[1]->output<<endl;
//cout<<"layer 2 output"<<endl;
//    cout<<networkLayers[2]->output<<endl;

    Eigen::Matrix<double, Eigen::Dynamic, 1> outputExpected(9,1);
    outputExpected<<0.2039,0.5875,0.2798,0.6588,0.5064,0.5675,0.3414,0.6927,0.3164;

    cout<<"difference = "<<networkLayers[2]->output-outputExpected<<endl;

    // clean up
    for (vector<NeuralLayer<double> *>::iterator nl=networkLayers.begin(); nl!=networkLayers.end(); ++nl)
        delete (*nl);
    return 0;
}
