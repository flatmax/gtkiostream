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
#ifndef COLOURLINESPEC_H_
#define COLOURLINESPEC_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <gtk/gtk.h>

#include <cairo/cairo.h>

using namespace std;

#ifndef uint
typedef unsigned int uint;
#endif

/** The colour definitions are specified in this class.
*/
template <int dummy>
class StandardColours {
protected:
    static int colourCntPrivate; ///< The number of colours defined in the following char ** variables
    static const char *shortColoursPrivate[]; ///< Default octave colours which match ColourLineSpec::X11ColoursPrivate
    static const char *X11ColoursPrivate[]; ///< Default X11 colours to match ColourLineSpec::shortColoursPrivate
};
// setup the possible plot colours ...
/// The number of standard colours
template <int dummy> int StandardColours<dummy>::colourCntPrivate=10;
/// The short representation of X11ColoursPrivate colours
template <int dummy> const char *StandardColours<dummy>::shortColoursPrivate[]= {(char *)"b",(char *)"g",(char *)"r",(char *)"c",(char *)"m",(char *)"y",(char *)"w",(char *)"k",(char *)"o",(char *)"a"}; // standard colours
/// The long form representation of shortColoursPrivate colours
template <int dummy> const char *StandardColours<dummy>::X11ColoursPrivate[]= {(char *)"blue", (char *)"green", (char *)"red", (char *)"cyan", (char *)"magenta", (char *)"yellow", (char *)"white", (char *)"black", (char *)"orange", (char *)"gray"};

/** \brief encapsulates a method to specify colour and line or point plotting
This class tries to encapsulate octave style colours and line type specification.
See ColourLineSpec::ColourLineSpec for a description

This class allows you to specify plot specifications like so :
"c1" for a cyan plot of line width 1. Or "y2:" for a yellow dotted plot of size 2.
i.e. : plot(x,y,x.size(),"y2");

NOTE: in the future there are plans to allow the user to specify their own colours.
*/
class ColourLineSpec : protected StandardColours<0> {
    bool sizeNotSpecified; ///< Set to true if the size was not specified ... i.e. "r" for red with no size
    bool colourNotSpecified; ///< Set to true if the colour was not specified ... i.e. "12" for size 12 with no colour
    float size; ///< The size of the line or point

#if GTK_MAJOR_VERSION==3
    GdkRGBA colour; ///< The colour holder
#else
    GdkColor colour; ///< The colour holder
#endif
public:

    vector<string> shortColours; ///< User specified colours in character notation : \sa shortColoursPrivate
    vector<string> X11Colours; ///< User specified colour strings : \sa X11ColoursPrivate

    bool linePlot; ///< The indication for lines or points plotting

    /** \brief default constructor with blue colour
    Initiates the defaul blue line type of size 1
    */
    ColourLineSpec(void) {
        setColourAndLinesOrPoints("b1");
    }

    /** \brief use a octave like string to define the line/points or colour
    Speciy whether to use points or lines and the colour.
    For example, "r" means, plot red with lines, ":k" means plot black in points.
    Also you can specify size, eg : ":r10" means points, red, size 10
    */
    ColourLineSpec(const char* clsChar) {
        if (strlen(clsChar)==0)
            setColourAndLinesOrPoints("b1");
        else
            setColourAndLinesOrPoints(clsChar);
    }

    /** Enquire whether the size was specified as well as the colour.
    For example:
    \code
    ColourLineSpec cls("r"); // is red without size.
    cls.wasSizeSpecified(); // is false
    \endcode
    \return true if the size was specified, false otherwise
    */
    bool wasSizeSpecified(void){
        return !sizeNotSpecified;
    }

    /** Enquire whether the colour was specified as well as the size
    For example:
    \code
    ColourLineSpec cls("12"); // is size 12 without colour.
    cls.wasColourSpecified(); // is false
    \endcode
    \return true if the colour was specified, false otherwise
    */
    bool wasColourSpecified(void){
        return !colourNotSpecified;
    }

    /** Manually specify the size
    \param s the size
    */
    void setSize(int s){
        sizeNotSpecified=false;
        size=(float)s;
    }
    /** Manually specify the size
    \param s the size
    */
    void setSize(float s){
        sizeNotSpecified=false;
        size=s;
    }

    /** Request the size
    \return the size
    */
    float getSize(void) const {
        return size;
    }

    /** Manually specify the colour
    \param c the GtkColor
    */
#if GTK_MAJOR_VERSION>2
    void setColour(GdkRGBA c){
#else
    void setColour(GdkColor c){
#endif
        colourNotSpecified=false;
        colour=c;
    }

    /** Manually specify the colour
    \param c the char* color (could be something like "red" or "#b2b2b2")
    */
    void setColour(const char* c){
        colourNotSpecified=false;
#if GTK_MAJOR_VERSION>2
                    gdk_rgba_parse(&colour, c);
#else
        gdk_color_parse(c,&colour);
#endif
    }

    /** Request the colour
    \return the Gdk color
    */
#if GTK_MAJOR_VERSION>2
    GdkRGBA getColour(void) const {
#else
    GdkColor getColour(void){
#endif
        return colour;
    }

    /** Request the colour string
    \return the Gdk colour as a newly allocated string
    */
    gchar *getColourString(void){
#if GTK_MAJOR_VERSION>2
        return gdk_rgba_to_string(&colour);
#else
        return gdk_color_to_string(&colour);
#endif
    }

    /** \brief interprets the octave style line/colour spec.
    By default implements lines specify points using the ':' character.
    Understands the default octave colours.

    */
    void setColourAndLinesOrPoints(const char* clsChar) {
        sizeNotSpecified=true;
        colourNotSpecified=true;

        linePlot=1;
        string clsStr(clsChar);
#if GTK_MAJOR_VERSION>2
                    gdk_rgba_parse(&colour, "blue");
#else
        gdk_color_parse("blue",&colour);
#endif
        //cout<<"ColourLineSpec::setColourAndLinesOrPoints colour = "<<"blue"<<endl;
        if (shortColours.size()==X11Colours.size() && X11Colours.size()!=0){ // if the user has specified their own colours
            for (unsigned int i=0; i<X11Colours.size(); i++)
                if (clsStr.find(shortColours[i].c_str())!=string::npos) {
#if GTK_MAJOR_VERSION>2
                    gdk_rgba_parse(&colour, X11Colours[i].c_str());
#else
                    gdk_color_parse(X11Colours[i].c_str(),&colour);
#endif
                    colourNotSpecified=false; // indicate that the colour was specified
                    break;
                }
        } else {
            for (int i=0; i<colourCntPrivate; i++) // if the user didn't specify their own colours, then use the standard colours.
                if (clsStr.find(shortColoursPrivate[i])!=string::npos) {
#if GTK_MAJOR_VERSION>2
                    gdk_rgba_parse(&colour, X11ColoursPrivate[i]);
#else
                    gdk_color_parse(X11ColoursPrivate[i],&colour);
#endif
                    colourNotSpecified=false; // indicate that the colour was specified
                    //      cout<<"ColourLineSpec::setColourAndLinesOrPoints colour = "<<X11ColoursPrivate[i]<<endl;
                    break;
                }
        }

        if (clsStr.find(':')!=string::npos) { // check for the points plotting style
            linePlot=false;
            //cout<<"ColourLineSpec::setColourAndLinesOrPoints linePlot = "<<linePlot<<endl;
        } //else            cout<<"ColourLineSpec::setColourAndLinesOrPoints linePlot = "<<linePlot<<endl;

        size=1.;
        unsigned int minC=clsStr.find_first_of("0123456789");
        //cout<<"clsStr : "<<clsStr<<endl;
        if ((minC!=string::npos) & (minC<=clsStr.size())) {
            int maxC=clsStr.find_last_of("0123456789");
            size=atof(clsStr.substr(minC,maxC-minC+1).c_str());
            sizeNotSpecified=false; // indicate that the size was specified
        }
        //cout<<"ColourLineSpec::setColourAndLinesOrPoints size = "<<size<<endl;
    }

    ColourLineSpec &operator=(string &c){
        if (c.size()==0)
            setColourAndLinesOrPoints("b1");
        else
            setColourAndLinesOrPoints(c.c_str());
        return *this;
    }

    ColourLineSpec &operator=(const char *c){
        if (strlen(c)==0)
            setColourAndLinesOrPoints("b1");
        else
            setColourAndLinesOrPoints(c);
        return *this;
    }

    /** Get the pointer to the short colour names.
    \return A pointer to shortColoursPrivate.
    */
    static const char ** getColourLabels(void){
        return shortColoursPrivate;
    }

    /** Given an integer, find the matching short colour name. If the number given is larger then the number of available colours, then find the modulus and return that colour.
    \param i The desired colour index into shortColoursPrivate.
    \return A shortColoursPrivate char array pointer which is indexed by i, and if i > colourCntPrivate, then find the index from the modulus [i%colourCntPrivate].
    */
    static const char *getColour(uint i) {
        return shortColoursPrivate[i%colourCntPrivate];
    }

    /** Sets the cairo drawing colour and width.
    \param cr The cairo context to set the colour and width in.
    \return A reference to this class.
    */
    ColourLineSpec &operator>>(cairo_t  *cr){
        if (cr){
            //float maxVal=pow(2.,8.*(float)sizeof(colour.red));
            float maxVal = (float)(1<<8*sizeof(colour.red));
            cairo_set_source_rgb(cr, colour.red/maxVal, colour.green/maxVal, colour.blue/maxVal);
            cairo_set_line_width(cr, size);
        }
        return *this;
    }

	/** Set the RGBA for the cairo context .
    \param cr The cairo context to set the colour and width in.
	\param alpha The number to scale by. 0.0 <= alpha <= 1.0
	*/
	ColourLineSpec setRGBA(cairo_t  *cr, double alpha){
        if (cr){
            float maxVal = (float)(1<<8*sizeof(colour.red));
            cairo_set_source_rgba(cr, colour.red/maxVal, colour.green/maxVal, colour.blue/maxVal, alpha);
            cairo_set_line_width(cr, size);
        }
        return *this;
	}
};

#endif // COLOURLINESPEC_H_
