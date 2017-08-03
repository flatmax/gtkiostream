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
#include "Window.H"
#include "Buttons.H"
#include "ComboBoxText.H"
#include "Box.H"

static void quit(void *wid, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {

    gtk_init( &argc, &argv );

    Window topWindow;
    Buttons buttons;
    HBox hBox;

    buttons<<BUTTONLABELSTRUCT("Quit", quit, &buttons); // Quit button

    ComboBoxText combo;
    combo<<"entry 1"<<"entry 2";
    combo<<string("entry 3");
    combo.show();
    hBox<<BoxIS(true, true, true)<<combo;

    combo.setSelection(1); // equivalent to combo.setSelection("entry 2");
    combo.setSelection("entry 2"); // equivalent to combo.setSelection(1);

    hBox<<buttons;   // Same as  hBox<< buttons.grab(1)<< buttons.grab(2)<< buttons.grab(3)<< buttons.grab(4);
    hBox.show();
    topWindow<< hBox;

    gtk_main();
}

/* IF you need to test against GTK+ 2.20, or other version. Install the sources. cd gtk+-2.20.1
./configure
make

Then go into the test directory and change testcombo* to your file name.

 g++ -DHAVE_CONFIG_H -I. -I.. -I.. -I../gdk -I../gdk  -pthread -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/atk-1.0       -DG_DISABLE_SINGLE_INCLUDES -DATK_DISABLE_SINGLE_INCLUDES -DGDK_PIXBUF_DISABLE_SINGLE_INCLUDES -DGTK_DISABLE_SINGLE_INCLUDES    -DGDK_PIXBUF_DISABLE_DEPRECATED -g -O2 -Wall -MT testcombo.o -MD -MP -MF .deps/testcombo.Tpo -c -o testcombo.o testcombo.c

g++ -DGDK_PIXBUF_DISABLE_DEPRECATED -g -O2 -Wall -o .libs/testcombo testcombo.o -pthread  ../gdk-pixbuf/.libs/libgdk_pixbuf-2.0.so ../gdk/.libs/libgdk-x11-2.0.so ../gtk/.libs/libgtk-x11-2.0.so /home/flatmax/temp/gtk+-2.20.1/gdk/.libs/libgdk-x11-2.0.so -lXext -lXrender -lXinerama -lXrandr -lXcursor /home/flatmax/temp/gtk+-2.20.1/gdk-pixbuf/.libs/libgdk_pixbuf-2.0.so -lgmodule-2.0 -lrt -lpangocairo-1.0 -lX11 -lXcomposite -lXdamage -lXfixes -latk-1.0 /usr/lib/x86_64-linux-gnu/libcairo.so -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 /usr/lib/x86_64-linux-gnu/libfreetype.so -lfontconfig -lgobject-2.0 -lglib-2.0 -lm -pthread
*/
