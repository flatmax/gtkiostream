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
#include "Window.H"
#include "Plot.H"
#include "Box.H"

// some definitions
#define cnt 4
float x[cnt]={0.0, 1., 2., 3.};
float y1[cnt]={0.0, 1., 2., 3.};
float y2[cnt]={1.0, 2., 3., 4.};

#define TYPE float

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv );

    Window topWindow;

    gtk_widget_set_size_request (topWindow.getWidget(), 1000, 500);

    /* ////////////////////////////////////////////////////////*/
    // The actual plotting

    Plot figure;  // create and show a Figure
    figure.plot(x, y1, cnt); // Plot the first line with default colours
    figure.hold(true); // hold the current curve and add another
    figure.plot(x,y2,cnt, "r1"); // plot this one as a red line

    // Changing the style of the plot
    figure.limits(-1.0,4.0,-1.,5.0); // open the window out to be able to see all
    figure.grid(true); // show a grid
    figure.title("Figure 1 title"); // add the title, x label and y label
    figure.xLabel("X axis label");
    figure.yLabel("Y axis label");
    /* /////////////////////////////////////////////////////////*/

    HBox hBox;
    hBox << BoxIS(TRUE, TRUE)<<figure.getWidget();  // show the figure
    hBox.show(); // show the horizontal box
    topWindow<< hBox; // add it to the top window

    gtk_main();
}
