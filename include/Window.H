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
#ifndef WINDOW_H_
#define WINDOW_H_
#include "Container.H"

/** Window

Base class for an application window

*/
class Window : public Container {

    bool fillBorderImplementation(GtkWidget *winIn, int borderFillVar=-1){
        if (borderFillVar==-1) // if no border fill specified, use the class default
            borderFillVar=borderFill;
        GtkStyle *wStyle=gtk_widget_get_style(winIn); // get the widget's bg colour
        cairo_t *cr=gdk_cairo_create(gtk_widget_get_window(widget));
		gdk_cairo_set_source_color(cr, &wStyle->bg[GTK_STATE_NORMAL]);
        int x, y, width, height;
#if GTK_MAJOR_VERSION==3
        GtkAllocation winAlloc, widgetAlloc;
        gtk_widget_get_allocation(widget, &winAlloc);
        gtk_widget_get_allocation(winIn, &widgetAlloc);
#endif
        if (borderFillVar&BORDER_LEFT){ // fill from the left side of the widget to the edge of the window
#if GTK_MAJOR_VERSION==2
            x=widget->allocation.x; y=winIn->allocation.y; width=winIn->allocation.x-x; height=winIn->allocation.height;
#else
            x=winAlloc.x; y=widgetAlloc.y; width=widgetAlloc.x-x; height=widgetAlloc.height;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        if (borderFillVar&BORDER_RIGHT){ // fill from the left side of the widget to the edge of the window
#if GTK_MAJOR_VERSION==2
            x=winIn->allocation.x+winIn->allocation.width; y=winIn->allocation.y; width=(widget->allocation.x+widget->allocation.width)-x; height=winIn->allocation.height;
#else
            x=widgetAlloc.x+widgetAlloc.width; y=widgetAlloc.y; width=(winAlloc.x+winAlloc.width)-x; height=widgetAlloc.height;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        if (((borderFillVar&BORDER_LEFT)!=0)&((borderFillVar&BORDER_TOP)!=0)){ // fill the top left corner if both left and top are filled
#if GTK_MAJOR_VERSION==2
            x=widget->allocation.x; y=widget->allocation.y; width=winIn->allocation.x-x; height=winIn->allocation.y-widget->allocation.y;
#else
            x=winAlloc.x; y=winAlloc.y; width=widgetAlloc.x-x; height=widgetAlloc.y-winAlloc.y;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        if (((borderFillVar&BORDER_RIGHT)!=0)&((borderFillVar&BORDER_TOP)!=0)){ // fill the top right corner if both right and top are filled
#if GTK_MAJOR_VERSION==2
            x=winIn->allocation.x+winIn->allocation.width; y=widget->allocation.y; width=(widget->allocation.x+widget->allocation.width)-x; height=winIn->allocation.y-y;
#else
            x=widgetAlloc.x+widgetAlloc.width; y=winAlloc.y; width=(winAlloc.x+winAlloc.width)-x; height=widgetAlloc.y-y;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        if (borderFillVar&BORDER_TOP){ // fill from the left side of the widget to the edge of the window
#if GTK_MAJOR_VERSION==2
            x=winIn->allocation.x; y=widget->allocation.y; width=winIn->allocation.width; height=winIn->allocation.y-y;
#else
            x=widgetAlloc.x; y=winAlloc.y; width=widgetAlloc.width; height=widgetAlloc.y-y;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        if (borderFillVar&BORDER_BOTTOM){ // fill from the left side of the widget to the edge of the window
#if GTK_MAJOR_VERSION==2
            x=winIn->allocation.x; y=winIn->allocation.y+winIn->allocation.height; width=winIn->allocation.width; height=(widget->allocation.y+widget->allocation.height)-y;
#else
            x=widgetAlloc.x; y=widgetAlloc.y+widgetAlloc.height; width=widgetAlloc.width; height=(winAlloc.y+winAlloc.height)-y;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        if (((borderFillVar&BORDER_LEFT)!=0)&((borderFillVar&BORDER_BOTTOM)!=0)){ // fill the bottom left corner if both left and bottom are filled
#if GTK_MAJOR_VERSION==2
            x=widget->allocation.x; y=winIn->allocation.y+winIn->allocation.height; width=winIn->allocation.x-x; height=(widget->allocation.y+widget->allocation.height)-y;
#else
            x=winAlloc.x; y=widgetAlloc.y+widgetAlloc.height; width=widgetAlloc.x-x; height=(winAlloc.y+winAlloc.height)-y;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        if (((borderFillVar&BORDER_RIGHT)!=0)&((borderFillVar&BORDER_BOTTOM)!=0)){ // fill the bottom right corner if both right and bottom are filled
#if GTK_MAJOR_VERSION==2
            x=winIn->allocation.x+winIn->allocation.width; y=winIn->allocation.y+winIn->allocation.height; width=(widget->allocation.x+widget->allocation.width)-x; height=(widget->allocation.y+widget->allocation.height)-y;
#else
            x=widgetAlloc.x+widgetAlloc.width; y=widgetAlloc.y+widgetAlloc.height; width=(winAlloc.x+winAlloc.width)-x; height=(winAlloc.y+winAlloc.height)-y;
#endif
            cairo_rectangle(cr, x, y, width, height);
        }
        cairo_fill(cr);
        cairo_destroy(cr);
        return false;
    }
public:
    enum BorderFillOptions {BORDER_LEFT=1, BORDER_RIGHT=2, BORDER_TOP=4, BORDER_BOTTOM=8}; ///< The various border fill masks
    int borderFill; ///< Used if the fillBorder callback is in use
//
//  GtkWidget *win; ///< The window widget
//
//    /** Get the window widget
//    \return the GtkWidget win
//    */
//  GtkWidget * getWidget(void){return win;}

//  /** \brief default constructor
//  */
//  Window(void){
//#ifdef DEBUG_Window
//    cout<<"Window: init "<<this<<endl;
//#endif
//    init();
//  }

    /** \brief constructor specifying window size and resideable options
    \param width the width of the window
    \param height the height of the window
    \param resiseable whether the window can be resised.
    */
  Window(int width, int height, bool resiseable=true) : Container() {
    init(width, height, resiseable);
  }

    Window() : Container() {
        init();
    }


    /** \brief the initialisation of the window (common to both constructors)
    \param width the width of the window
    \param height the height of the window
    \param resiseable whether the window can be resised.
    */
    void init(int width=0, int height=0, bool resiseable=true){
    widget = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    if (width!=0 & height!=0) // only resize if both are non-zero
        gtk_widget_set_size_request(widget, width, height);
//    g_signal_connect(G_OBJECT (widget), "delete_event", reinterpret_cast<GCallback> (quit), NULL );
    show();
    }

  /** set the background colour of the top window
  \param colour The GdkColour to change the background to
  \param state The colour for a particular state. Defaults to GTK_STATE_NORMAL
  */
  void setBackgroundColour(GdkColor *colour, GtkStateType state=GTK_STATE_NORMAL){
    gtk_widget_modify_bg(widget, state, colour);
  }

  /** set the background colour of the top window
  \param colourName The name of the colour to use. e.g. "white"
  \param state The colour for a particular state. Defaults to GTK_STATE_NORMAL
  */
  void setBackgroundColour(const char* colourName, GtkStateType state=GTK_STATE_NORMAL){
    GdkColor colour; gdk_color_parse (colourName, &colour);
    setBackgroundColour(&colour, state);
  }

    /** Callback method to fill the boarder from the edge of the widget to the edge of this window.
    this->borderFill = BORDER_LEFT|BORDER_RIGHT will fill both left and right borders
    The fill colour is made to be the same as the widget backround colour.
	The following example is for a HBox, but could also be a VBox or any other widget.
	\code
    HBox.connectAfter("expose-event",  G_CALLBACK(GTKWindow.fillBorder), (gpointer)&GTKWindow);
    HBox.connectAfter("visibility-notify-event",  G_CALLBACK(GTKWindow.fillBorder), (gpointer)&GTKWindow);
    \endcode
	In this example, the border between the widget and the GTKWindow is filled with the widget's background colour.
    \param widget The widget which triggered the event
    \param event The event which was triggered
    \param data A pointer to this instance
    \return false to keep propagating the event
    */
    static bool fillBorder(GtkWidget *widget, GdkEventExpose *event, gpointer data){
        Window *gtki=static_cast<Window*>(data);
        return gtki->fillBorderImplementation(widget);
    }

    /** Callback method to fill the boarder as if a top banner were present
    \param widget The widget which triggered the event
    \param event The event which was triggered
    \param data A pointer to this instance
    \return false to keep propagating the event
    */
    static bool fillBannerTop(GtkWidget *widget, GdkEventExpose *event, gpointer data){
        Window *gtki=static_cast<Window*>(data);
        return gtki->fillBorderImplementation(widget, BORDER_TOP|BORDER_LEFT|BORDER_RIGHT);
    }

    /** Callback method to fill the boarder as if a middle banner were present
    \param widget The widget which triggered the event
    \param event The event which was triggered
    \param data A pointer to this instance
    \return false to keep propagating the event
    */
    static bool fillBannerMiddle(GtkWidget *widget, GdkEventExpose *event, gpointer data){
        Window *gtki=static_cast<Window*>(data);
        return gtki->fillBorderImplementation(widget, BORDER_LEFT|BORDER_RIGHT);
    }

    /** Callback method to fill the boarder as if a bottom banner were present
    \param widget The widget which triggered the event
    \param event The event which was triggered
    \param data A pointer to this instance
    \return false to keep propagating the event
    */
    static bool fillBannerBottom(GtkWidget *widget, GdkEventExpose *event, gpointer data){
        Window *gtki=static_cast<Window*>(data);
        return gtki->fillBorderImplementation(widget, BORDER_BOTTOM|BORDER_LEFT|BORDER_RIGHT);
    }
};

#endif // WINDOW_H_
/** \mainpage Brief intro to GTK+ IOStream.

 * \section intro_sec Introduction

 This software is for the following :
 - the implementation of audio clients for playing/recording and the processing of sound, good for digital signal processing (DSP) as well
 - building GUIs
 - the interfaceing of C++ to Octave.org using Eigen3 matrices.
 - the implementation of feeforward neural networks
 - Object Request Broking (ORB) classes over networks

 The audio system interfaces to the both ALSA and a <a href="http://www.jackaudio.org">Jack audio</a> server. The ALSA code allows you to produce ALSA plugins or directly output/input and do full duplex processing.
 This jackd server allows real time audio processing when you have a patched kernel or if your operating system is capable of schedueling in some real time manner.

 Apart from jackd and ALSA, this code also interfaces to SoX (the swiss army knife of audio processing). This allows you to read and write audio files of almost any type. Further, various tools for DSP are supplied, such
 as audio masking (psychoacoustic type audio masking as used in mp3), also WSOLA which allows you to speed up audio and slow it down, without changing its pitch.
 The IIO code in this project is also used in jackd as a driver for interfacing IIO capable chips.

 The GUI related classes are intended to be light weight GTK wrappers for use in C++.
 As far as the GUI software is concerned, it is all contanined in the class header files, this means that you can simply
 include the headers and code away without having to worry about compiling any source code files other then your own. I have put a set of tutorials to get the basic idea of GUI programming across to you up <a href="https://www.raspberrypi.org/forums/viewtopic.php?f=67&t=173531">here</a> it is targeting newbies.

 <a href="http://www.octave.org">Octave</a> is interfaced using <a href="http://eigen.tuxfamily.org">Eigen3</a> and/or <a href="http://opencv.org/">openCV matrices</a> which are hardware optimised matrix computation library, OpenCV also being a image processing library.

 The feedforward neural networks use <a href="http://eigen.tuxfamily.org">Eigen3</a> for their implementation, which ensures the efficient evaluation using hardware vector processing and parrallel computation, where possible.

 Various other parts of this distribution include
 - AudioMasker, an audio masking model which is similar to the psychoacoustical model used in mp3 compression.
 - WSOLA, time scale modification for multichannel audio, this allows you to speed up and slow down audio without chaning its pitch.
 - WSOLA and WSOLAJackGTK are also provided as binary executables which can be run from the command line with or without a GUI, respectively.
 - IIO processing which is embedded as a driver in jackd2.

 The ORB (object request brokering) classes ORBOriginator and ORBReplicator utilise <a href="http://www.zeroc.com">Zeroc Ice</a> to implement the actual ORB application layer.
 When the term ORB is used, it implies the following :
 - The mirroring of classes from one side of the network to the other.
 - The execution of class methods on the origintaing side of the network from the replicating side of the network.
 - The ability to call methods synchronously OR asynchronously (threaded execution).

 This distribution also ships with many examples of how to use the various classes contained within.

# <a href="http://www.audioinjector.net/rpi-shop">Support this project by purchasing our hardware.</a>

\verbatim
   _____                              _      __                            __ _
  / ____|                            | |    / _|                          / _| |
 | (___  _   _ _ __  _ __   ___  _ __| |_  | |_ _ __ ___  ___   ___  ___ | |_| |___      ____ _ _ __ ___
  \___ \| | | | '_ \| '_ \ / _ \| '__| __| |  _| '__/ _ \/ _ \ / __|/ _ \|  _| __\ \ /\ / / _` | '__/ _ \
  ____) | |_| | |_) | |_) | (_) | |  | |_  | | | | |  __/  __/ \__ \ (_) | | | |_ \ V  V / (_| | | |  __/
 |_____/ \__,_| .__/| .__/ \___/|_|   \__| |_| |_|  \___|\___| |___/\___/|_|  \__| \_/\_/ \__,_|_|  \___|
              | |   | |
              |_|   |_|
\endverbatim

 \subsection GUI_sec GUI detail

 The general paradigm is that you can very easily construct GTK object and in some cases use the C++ IOStream  paradigm to build graphical user interfaces (GUIs).

 For example, I will create labels in a Vertical box like so :

 \code
 VBox vbox; // create a VBox
 Labels labels; // Create a labels structure
 labels<<"First Label"<<"Second Label"; // Load some labels
 vbox<<labels; // Put the labels into the VBox
 vbox.show(); // show the VBox
 \endcode

 Similar things can be done with buttons (label buttons, toggle buttons, XPM and transparent XPM buttons are also supported)

 This example runs gtk as normal and loads an HBox into the GTK top window. The HBox is loaded with a quit label button.

 \code
 // The quit button callback
 static void quit(void *wid, gpointer data){
  gtk_main_quit();
 }

 int main(int argc, char *argv[]){
    gtk_init( &argc, &argv ); // start GTK
    Window topWindow; // create the top window

    HBox hbox; // create a horizontal box for holding the buttons
    Buttons buttons; // create a buttons holder
    buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL); // load a label button - other button types are also available
    hbox<< buttons; // add the button to the HBox
    hbox.show(); // show the HBox

    topWindow<< hBox; // load the HBox into the top window
    gtk_main(); // run GTK
 }
 \endcode

 The Labels and Buttons are actually based on the LinkList datatype - which is a templated lightweight doubly linked list.

 If you want to get compact, you can write like so :
 \code
    topWindow << (hbox << buttons<<BUTTONLABELSTRUCT("Quit", quit, NULL)).show();
 \endcode

 If you want access to the GTK+ widgets, it is easy

 \code
    GtkWidget* widget=hbox.getWidget(); // Standard GtkWidgets are readily accessible for all classes.
 \endcode

  \subsubsection Plotting_subsec Plotting example

  Plotting is easy and is similar to Octave.org (Matlab) style plotting commands.

  For example the following code :

  \code
    #define cnt 4
    float x[cnt]={0.0, 1., 2., 3.};
    float y1[cnt]={0.0, 1., 2., 3.};
    float y2[cnt]={1.0, 2., 3., 4.};

    Plot figure;  // create and show a Figure
    figure.plot(x,y1,cnt); // Plot the first line with default colours
    figure.hold(true); // hold the current curve and add another
    figure.plot(x,y2,cnt, "r1"); // plot this one as a red line

    // Changing the style of the plot
    figure.limits(-1.0,4.0,-1.,5.0); // open the window out to be able to see all
    figure.grid(true); // show a grid
    figure.title("Figure 1 title"); // add the title, x label and y label
    figure.xLabel("X axis label");
    figure.yLabel("Y axis label");
  \endcode

  Produces this :

    @image html PlotExample.png

 \subsection Octave_sec Octave detail

 Octave is simplistically instantiated and run like so :
 \code
 Octave octave(args);
 output = octave.runM("yourMFile.m", input);
 \endcode
 A more detailed description can be found in the Octave class detailed description.

 To utilise the Octave code, the Octave.C file must be linked with your code. The reason for separating the Octave source from the header
 is that octave uses libfftw.so, where as you may want to use libfftwf.so in your own code. By separating the Octave code out from the Octave header file,
 you are able to link against the Octave header and utilise octave without having to link against its shared libraries directly.

 \subsection ORB_sec Object Request Broking detail

    ORB is desigend to be trivial.

    On the originating side of the network, the following is executed
    \code
    ORBOriginator origin(argc, argv, string("BasePipe")); // This is the original locaiton where the classes are created

    // Create a class and add it to the ORB application layer
    ORBTestClass *otc = new ORBTestClass; // this is the class to be added to the ORB application layer
    origin.addClass(otc, otc->name); // add it to the originator
    \endcode

    On the replication side of the network, the following is executed
    \code
    // create the replicator which will connect to loopback
    ORBTestReplicator replicator(argc, argv, string("BasePipe"), string("127.0.0.1")); // This is the replicating locaiton where the classes are proxies

    // If you want the same class with the same state as it is on the originator, you can like so :
    ORBTestClass *otc = replicator.getObjectPointer<ORBTestClass>(ORBTestClass::name);
    cout<<"variable = "<<otc->variable<<endl;

    // By using a proxy, you can execute the originator class's methods
    ORBTestICEPrx oRBTestICEPrx = replicator.getObjectProxy<ORBTestICEPrx>(ORBTestClass::name);
    cout<<"input = "<<oRBTestICEPrx->method(1)<<endl;

    \endcode
    A more detailed example is in the ORBTest.C class. There is also other documentation in the ORBOriginator and ORBReplicator class documentation.

 To utilise the ORB (ORBOriginator and ORBReplicator) code, you must specify your network protocol in a .ice file.
 This .ice file specifies in object oriented code what objects you will be sending over the network (simple types are int, float, vector<TYPE> and many others).
 The slice2cpp application is run at compile time to machine generate .C code from the .ice files. These then are linked into your application.

 \subsection NN_sec Neural Network computation

 Neural network computation is also designed to be efficient and quick to use. For more detail and example code reference here NeuralNetwork.

 \subsection audio_sec Audio playback/recording and digital signal processing (DSP)

 Jack is utilised for audio playback. Currently Jack runs on OSX, IOS, Linux, Microsoft (and hopefully soon android!). See JackClient for more detail.

  \section Installing_sec Installation and Compilation

  If you only want to use the GTK GUI building classes, then there is no need to build and you can simply point your preprocessor or IDE to the correct header file location.

  If you want to use the ORBing system, Octave, Sox and some future additions, then you should install.

  Requirements : You will need the Octave system, libsox, and ZeroC Ice. On a Debian derived system (e.g. Ubuntu) you can use the following :

  \code
  sudo apt-get install zeroc-ice34 libzeroc-ice34-dev ice34-translators
  sudo apt-get install liboctave-dev libsox-dev
  sudo apt-get install doxygen graphviz
  \endcode

  To build the code, first run ./tools/autotools.sh then configure, make and make install.
  To clean the code, either make clean OR run ./tools/autotoolsClean.sh to clean everything !

 \section Copyright_sec Copyright
 Copyright 2001 to 2013 Matt Flax <flatmax@flatmax.org>

 \section licensing_sec License
 This software is governed by the GPL license.

 \section Interesting_sec Interesting multimedia and hardware from this author.

  ...

 \htmlonly
 <a href="http://www.flatmaxstudios.com/">Hardware</a>
 \endhtmlonly

 ...

 \htmlonly
 <a href="http://www.jamendo.com/en/list/a30616/wideband-cold-fusion">Music</a>
 \endhtmlonly


*/
