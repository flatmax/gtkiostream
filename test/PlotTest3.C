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

// Quit button callback
static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv );

    Window topWindow;

    gtk_widget_set_size_request (topWindow.win, 1000, 500);

    int cnt=1024;
    float xf[cnt], yf[cnt]; // some more data for a second plot
    int yi[cnt];
    for (int i=1;i<cnt;i++){
        xf[i]=(float)i/cnt;
        yf[i]=(float)i/cnt;
        yi[i]=(int)(yf[i]);
    }

    Plot figure;  // create and show a Figure
    figure.show();
    figure.plot(xf,yf,cnt); // Plot the first line with default colours
    figure.hold(true);
    figure.plot(xf, yi, cnt, ColourLineSpec("b2"));
//    figure.limits(1.0,101.0,1.0,201.0);
    figure.grid(true);
    figure.title("Plotting a float y and int y on the same plot");
    figure.xLabel("i/cnt");
    figure.yLabel("x/cnt");

//    HBox hBox;
//    hBox << BoxIS(TRUE, TRUE)<<figure.getWidget();
//    hBox.show(); // show the horizontal box
    topWindow<< figure.getWidget(); // add it to the top window

    gtk_main();
}
