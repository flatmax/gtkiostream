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
#ifndef NEURALNETWORK_H_
#define NEURALNETWORK_H_

#include <Eigen/Dense>
#include <vector>
using namespace std;

/** Implements a single neural layer
\tparam TYPE the precision of the data to use, e.g. float, double
*/
template<typename TYPE>
class NeuralLayer {
protected:
    Eigen::Matrix<TYPE, Eigen::Dynamic, Eigen::Dynamic> weights; ///< The neural weights for this layer
    Eigen::Matrix<TYPE, Eigen::Dynamic, 1> bias; ///< The biases for this layer
public:

    Eigen::Matrix<TYPE, Eigen::Dynamic, 1> output; ///< The output from this layer

    /** Generate a neural layer of particular size
    \param inputSize The number of the inputs
    \param outputSize The number of outputs
    */
    NeuralLayer(int inputSize, int outputSize) {
        weights.resize(inputSize, outputSize);
        bias.resize(1,outputSize);
        output.resize(1,outputSize);
    }

    /** Generate a neural layer of particular size providing the weights
    \param weightsIn The weights to set
    \param biasIn The biases to set
    \tparam Derived is used by Eigen's Curiously recurring template pattern (CRTP)
    */
    template <typename Derived>
    NeuralLayer(const Eigen::MatrixBase<Derived> &weightsIn, const Eigen::MatrixBase<Derived> &biasIn) {
        weights=weightsIn;
        bias=biasIn;
        output.resize(bias.rows(),1);
    }

    /// Destructor
    virtual ~NeuralLayer(void) {}

    /** The activation function
    This evaluates the neural network
    \param  input The input to this layer
    \return The result of the layer after processing the input
    */
    virtual Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &activate(const Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &input) {
//        cout<<"weights r,c "<<weights.rows()<<'\t'<<weights.cols()<<endl;
//        cout<<"bias r,c "<<bias.rows()<<'\t'<<bias.cols()<<endl;
//        cout<<"input r,c "<<input.rows()<<'\t'<<input.cols()<<endl;
//        cout<<"output r,c "<<output.rows()<<'\t'<<output.cols()<<endl;
        output=bias;
        output.noalias()+=weights*input;
//        output.noalias()+=input.transpose()*weights.transpose();
//        cout<<"bias "<<bias<<endl;
        return output;
    }

    /// \return the number of inputs to this layer.
    int inputSize(void){
        return weights.rows();
    }

    /// \return the number of outputs from this layer.
    int outputSize(void){
        return weights.cols();
    }
};

/** Implements a neural layer with a sigmoid activation function
\tparam TYPE the precision of the data to use, e.g. float, double
*/
template<typename TYPE>
class SigmoidLayer : public NeuralLayer<TYPE> {
public:
    /** Generate a neural layer of particular size
    \param inputSize The number of the inputs
    \param outputSize The number of outputs
    */
    SigmoidLayer(int inputSize, int outputSize) : NeuralLayer<TYPE>(inputSize, outputSize) {
    }

    /** Generate a neural layer of particular size providing the weights
    \param weightsIn The weights to set
    \param biasIn The biases to set
    \tparam Derived is used by Eigen's Curiously recurring template pattern (CRTP)
    */
    template <typename Derived>
    SigmoidLayer(const Eigen::MatrixBase<Derived> &weightsIn, const Eigen::MatrixBase<Derived> &biasIn) : NeuralLayer<TYPE>(weightsIn, biasIn) {
    }

    /// Destructor
    virtual ~SigmoidLayer(void) {}

    /** The sigmoidal activation function
    Evaluate the neural layer using the sigmoid as the activation function
    \param  input The input to this layer
    \return The result of the layer after processing the input
    */
    virtual Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &activate(const Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &input) {
        NeuralLayer<TYPE>::activate(input);
        NeuralLayer<TYPE>::output=1./(1.+(-NeuralLayer<TYPE>::output).array().exp());
//        cout<<"output "<<NeuralLayer<TYPE>::output<<endl;
        return NeuralLayer<TYPE>::output;
    }
};

/** Implements a neural layer with an scaled and offset tanh activation function
\tparam TYPE the precision of the data to use, e.g. float, double
*/
template<typename TYPE>
class TanhLayer : public NeuralLayer<TYPE> {
public:
    /** Generate a neural layer of particular size
    \param inputSize The number of the inputs
    \param outputSize The number of outputs
    */
    TanhLayer(int inputSize, int outputSize) : NeuralLayer<TYPE>(inputSize, outputSize) {
    }

    /** Generate a neural layer of particular size providing the weights
    \param weightsIn The weights to set
    \param biasIn The biases to set
    \tparam Derived is used by Eigen's Curiously recurring template pattern (CRTP)
    */
    template <typename Derived>
    TanhLayer(const Eigen::MatrixBase<Derived> &weightsIn, const Eigen::MatrixBase<Derived> &biasIn) : NeuralLayer<TYPE>(weightsIn, biasIn) {
    }

    /// Destructor
    virtual ~TanhLayer(void) {}

    /** The sigmoidal activation function
    Evaluate the neural network using the sigmoid as the activation function
    \param  input The input to this layer
    \return The result of the layer after processing the input
    */
    virtual Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &activate(const Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &input) {
        NeuralLayer<TYPE>::activate(input);
        NeuralLayer<TYPE>::output=2./(1.+(-2.*NeuralLayer<TYPE>::output).array().exp())-1.;
        return NeuralLayer<TYPE>::output;
    }
};

/* Implements a neural layer with an scaled and offset tanh activation function
\tparam TYPE the precision of the data to use, e.g. float, double
*/
//template<typename TYPE>
//class PosLayer : public NeuralLayer<TYPE> {
//public:
    /* Generate a neural layer of particular size
    \param inputSize The number of the inputs
    \param outputSize The number of outputs
    */
//    PosLayer(int inputSize, int outputSize) : NeuralLayer<TYPE>(inputSize, outputSize) {
//    }

    /* Generate a neural layer of particular size providing the weights
    \param weightsIn The weights to set
    \param biasIn The biases to set
    \tparam Derived is used by Eigen's Curiously recurring template pattern (CRTP)
    */
//    template <typename Derived>
//    PosLayer(const Eigen::MatrixBase<Derived> &weightsIn, const Eigen::MatrixBase<Derived> &biasIn) : NeuralLayer<TYPE>(weightsIn, biasIn) {
//    }

    // Destructor
//    virtual ~PosLayer(void) {}

    /* The positive only layer
    Evaluate the neural network using the sigmoid as the activation function
    \param  input The input to this layer
    \return The result of the layer after processing the input
    */
//    virtual Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &activate(const Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &input) {
//        NeuralLayer<TYPE>::activate(input);
//        NeuralLayer<TYPE>::output=(NeuralLayer<TYPE>::output>0.).select(NeuralLayer<TYPE>::output,0.);
//    }
//};

/** Implements a feed forward neural network.
The network is used as follows :
\code
    // Begin constructing the neural network topology
    vector<NeuralLayer<double> *> networkLayers;
    networkLayers.push_back(new TanhLayer<double>(weights, bias));
    networkLayers.push_back(new TanhLayer<double>(weights, bias));

    // setup some input
    Eigen::Matrix<double, Eigen::Dynamic, 1> input(10,1);
    input<<0.8333,0.8333,0.8333,0.8333,0.8333,0.6871,0.5833,0.4371,0.3333,0.4000;

    // Actiave the Neural Network
    NeuralNetwork<double> nn;
    nn.activate(networkLayers, input);

    // the result is in the last layer
    cout<<networkLayers[networkLayers.size()-1]->output<<endl;
\endcode
\tparam TYPE the precision of the data to use, e.g. float, double
*/
template<typename TYPE>
class NeuralNetwork {
public:
    /// Constructor
    NeuralNetwork(void) {}

    /// Destructor
    virtual ~NeuralNetwork(void) {}

    /** Activates all layers in the neural network.
    The last layer has the output
    \param layers Various neural network layers, 0 being the input layer
    \param input The input vector to feed forward
    */
    void activate(vector<NeuralLayer<TYPE> *> &layers, Eigen::Matrix<TYPE, Eigen::Dynamic, 1> &input) {
        int layerCount=layers.size();
        if (layerCount>0) {
            // process the first layer
            layers[0]->activate(input);
            for (int i=1; i<layerCount; i++) {
//                cout<<i<<endl;
                layers[i]->activate(layers[i-1]->NeuralLayer<TYPE>::output);
//        cout<<layers[i-1]->NeuralLayer<TYPE>::output.transpose()<<endl;
            }
        }
    }
};
#endif // NEURALNETWORK_H_
