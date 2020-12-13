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

#include <NeuralNetwork.H>
#include <fstream>
#include <iostream>
using namespace std;

#include "Window.H"
#include "Buttons.H"
#include "Plot.H"
#include "Box.H"

// Quit button callback
static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]){
    // To construct a network, we load some weights and biases from file and create a layer using them
    // This is done for each layer.
    // Finally a NeuralNetwork is created to execute each layer

    // load the input layer weights and biases
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> weights(21,21);
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> bias(21,1);

    Eigen::Matrix<float, Eigen::Dynamic, 1> input(21,1);
    input=Eigen::Matrix<float, Eigen::Dynamic, 1>::Zero(21,1);

//cout<<"weights "<<endl;
//cout<<weights<<endl;
//cout<<"bias "<<endl;
//cout<<bias<<endl;
//cout<<"input "<<endl;
//cout<<input<<endl;
    // Begin constructing the neural network topology
    vector<NeuralLayer<float> *> networkLayers;
    bias<<-1.,-.9,-.8,-.7,-.6,-.5,-.4,-.3,-.2,-.1,0.,.1,.2,.3,.4,.5,.6,.7,.8,.9,1.;
    bias*=10.;
    networkLayers.push_back(new SigmoidLayer<float>(weights, bias));
    networkLayers.push_back(new TanhLayer<float>(weights, bias));
    //networkLayers.push_back(new PosLayer<float>(weights, bias));

    NeuralNetwork<float> nn;
    nn.activate(networkLayers, input);
//    cout<<networkLayers[0]->output<<endl;
    Eigen::Matrix<float, Eigen::Dynamic, 1> biasSig=bias;
    Eigen::Matrix<float, Eigen::Dynamic, 1> outputSig=networkLayers[0]->output;
    Eigen::Matrix<float, Eigen::Dynamic, 1> outputTanh=networkLayers[1]->output;
    //Eigen::Matrix<float, Eigen::Dynamic, 1> outputPos=networkLayers[2]->output;

    // clean up
    for (vector<NeuralLayer<float> *>::iterator nl=networkLayers.begin(); nl!=networkLayers.end(); ++nl)
        delete (*nl);

    // plot the results

    gtk_init( &argc, &argv ); // start the GUI

    Plot figure;
    figure.plot(biasSig.data(), outputSig.data(), outputSig.rows(),"b");
    figure.hold(true);
    figure.plot(biasSig.data(), outputTanh.data(), outputTanh.rows(),"r");

    figure.limits(); // autoscale
    figure.hold(false);


    Window topWindow;

    VBox vBox;
    vBox<<BoxIS(true, true, true)<<figure.getWidget()<<BoxIS(false, false, false)<<(Buttons()<<BUTTONLABELSTRUCT((char*)"Quit", quit, NULL));
    topWindow<< vBox.show(); // add it to the top window

    gtk_main();

    return 0;
}
