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
#include "Window.H"
#include "Plot.H"
#include "Box.H"

// Quit button callback
static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

// some definitions
#define cnt 101
float x[cnt], y[cnt];

#include <time.h>
#include <math.h>

#define TYPE float

string text("test this");
float textX, textY;

// timeout function - shifts the sinusoid along and replots - shows how simple it is to plot dynamically changing data.
gboolean sinusoid(gpointer user_data){
    // shift the sinusoid on.
    int s=time(NULL);
    int maxY=-1;
    for (int i=0;i<cnt;i++){
        y[i]=100.0*sin((TYPE)i/((TYPE)cnt-1)*2*M_PI+s);
        if (y[i]>maxY){
            maxY=y[i];
            textX=(float)i;
        }
    }


    // replot the data
    Plot **p = (Plot**) user_data;

    textY=160.;
    p[0]->text(&textX, &textY, text.c_str(), "k");
    p[1]->text(&textX, &textY, text.c_str(), "k");

    p[0]->replot();
    p[1]->replot();
    return 1;
}

void setupCustomColours(ColourLineSpec &cls){
    // setup the possible plot colours ...
    cls.shortColours.push_back("r");
    cls.X11Colours.push_back("red");
    cls.shortColours.push_back("b");
    cls.X11Colours.push_back("blue");
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv );

    Window topWindow;

    gtk_widget_set_size_request (topWindow.getWidget(), 1000, 500);
    //hBox<< figure.getWidget();

    TYPE x1[cnt], y1[cnt]; // some more data for a second plot
    for (int i=1;i<cnt;i++)
        x[i]=y[i]=(TYPE)i;
    for (int i=1;i<cnt;i++){
        x1[i]=(TYPE)i;
        y1[i]=(TYPE)i+100.0;
    }

    /* ////////////////////////////////////////////////////////*/
    // The actual plotting

    Plot figure;  // create and show a Figure
    ColourLineSpec cls;
    setupCustomColours(cls); // setup our own colours
    figure.show();
    figure.plot(x,y,cnt); // Plot the first line with default colours
    figure.hold(true);
    cls=(char*)"b2";
    figure.plot(x1,y1,cnt, cls);
    figure.limits(1.0,101.0,1.0,201.0);
    figure.set(figure.gca(), "XLabelFormat", "%%.1f", NULL);
    figure.grid(true);
    figure.title("Figure 1");
    figure.xLabel("first plot x label");
    figure.yLabel("just an example y");

    Plot figure2; // create a second figure for plotting a region
    figure2.show();
    cls=(char*)"r";
    figure2.plot(x,y,y1,cnt, cls); // Plot the first line with default colours
    figure2.limits(1.0,101.0,1.0,201.0);
    figure2.grid(true);

    figure2.title("Figure 2");
    figure2.xLabel("second plot x label");
    /* /////////////////////////////////////////////////////////*/

    HBox hBox;
    hBox << BoxIS(TRUE, TRUE)<<figure.getWidget()  // show the figures
         << BoxIS(TRUE, TRUE)<<figure2.getWidget();
    hBox.show(); // show the horizontal box
    topWindow<< hBox; // add it to the top window

    // setup a 1 second timer to change the plot
    Plot *plots[2]; plots[0]=&figure; plots[1]=&figure2;
    g_timeout_add_seconds(1, sinusoid, (gpointer)plots);

    gtk_main();
}
