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
#include "Buttons.H"
#include "Labels.H"
#include "ProgressBar.H"
#include "Box.H"

float accumulator; // goes between 0 and 1
float fraction; // set to +-0.1

// shifts the accumulator up and down between 0 and 1
gboolean changeProgress(gpointer data) {

    ProgressBar *bar=static_cast<ProgressBar*>(data);
    *bar=accumulator;

    accumulator+=fraction;
    if (accumulator>1.0) { // if we hit 1 reverse
        accumulator=1.0;
        fraction=-fraction;
    }
    if (accumulator<.0) { // if we hit 0 reverse
        accumulator=.0;
        fraction=-fraction;
    }
    return 1;
}

// shifts the pulse back and forth (negative numbers pulse)
gboolean pulseProgress(gpointer data) {

    ProgressBar *bar=static_cast<ProgressBar*>(data);
    *bar=-accumulator;
    return 1;
}

static void quit(void *widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    accumulator =0.0;
    fraction=0.1;
    gtk_init( &argc, &argv );

    Window topWindow;

    Buttons buttons;
    buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL);

    ProgressBar bar, pulsingBar;

    bar=0.5;
    pulsingBar=-0.5;

    bar.show();
    pulsingBar.show();

    VBox vbox;

    Labels labels;
    labels<<"progress bar"<<"pulse bar";

    vbox<<buttons.grab(1);
    vbox<<labels.grab(1)<<BoxIS(true,false)<<bar.getWidget(); // BoxIS is ussed to give widgets space and don't fill
    vbox<<labels.grab(2)<<BoxIS(true,false)<<pulsingBar.getWidget();
    vbox.show();

    topWindow<<vbox;
    g_timeout_add_seconds(1, changeProgress, (gpointer)&bar);
    g_timeout_add_seconds(1, pulseProgress, (gpointer)&pulsingBar);
    gtk_main();
}
