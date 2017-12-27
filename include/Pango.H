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
#ifndef PANGO_H_
#define PANGO_H_

#include "ColourLineSpec.H"
#include <sstream>

/** Type defines a font for Pango to use
For example :
Pango pango << Font("Sans 12");
*/
class Font : public string {
    PangoFontDescription *pangoFontDescription;

    /** Common constructor initialisation
    */
    void init(){pangoFontDescription=NULL;}
    public:
    Font() : string(){init();} ///< Empty constructor
    Font(const char *str) : string(str){init();} ///< Construct a font description from a character array
    Font(string str) : string(str){init();} ///< Construct a font description from a string

    /**
    Destruct the class.
    */
    ~Font(void){
        if (pangoFontDescription)
            pango_font_description_free(pangoFontDescription);
        pangoFontDescription=NULL;
    }

    /** Construct a PangoFontDescription * from the current contents of the inherited string
    */
    PangoFontDescription *getPangoFontDescription(void){
        if (pangoFontDescription)
            pango_font_description_free(pangoFontDescription);
        pangoFontDescription=NULL;
        return pangoFontDescription=pango_font_description_from_string(c_str());
    }
};

/** Pango input output stream
Special flags which can be used by the Pango class.
*/
class PangoIOS {
    public:
    /** special flags for use with the Pango class.
    Next : This will Indicate to start a new tag (i.e. '<span') the Pango stream. When inputting, empty strings or markups may be provided
    Reset : Remove any previous Pango::markups and Pango::strings
    */
    enum specialFlags {Next, Reset};

//    bool operator==(PangoIOS &pios, int psf){
//        return
//    }
};

/** Construct a pango markup
\image html PangoTest.jpg
This class enables the user to construct pango markups like so :
\code
Pango pango << "Normal text" << ColourLineSpec("g5") << "Green size 5 text"<< "Normal text";
pango << Font("Sans 12") << ColourLineSpec("r") << " Sans font size 12 text";
\endcode

Markups must be added before the string they are to makrup. String may then be added.

PangoIOS provides special features for the Pango input stream.

For example using PangoIOS::Next, strings without markups can be provided :
\code
Pango pango << ColourLineSpec("g") << "green." << PagnoIOS::Next << "text without markup";
\endcode

Or markups without text can be provided :
\code
Pango pango << Font("Sans 12") << PagnoIOS::Next << ColourLineSpec("g") << "green.";
\endcode

To remove previous markups and strings :
\code
Pango pango << ColourLineSpec("g") << "green.";
pango<<PangoIOS::Reset<<"new string";
\endcode

*/
class Pango {
    /*
    The internal workings of the class are such that each string is associated with a list of markup/variable pairs.
    In this manner, many markups may be applied to a single string.

    Thoughts for future expansion to handle <b> <i> and similar tags are to firstly add extra PangoIOS variables, such as PangoIOS::Bold, PangoIOS::Italic and so on.
    How to structure this is confounded by the following two concepts of which is dominant, a ColourLineSpec/Font tag OR a PangoIOS tag ? ...
    Pango pango;
    // want to carry the ColourLineSpec tag along, past the PangoIOS::Italic tag, suggesting that ColourLineSpec is superior.
    pango<<ColourLineSpec("r10000")<<"red 10 non italic"<<Pango::Italic<<"red 10 italic text";
    // want green to be italic as well which suggests that PangoIOS tags are superior
    pango<<Pango::Italic<<ColourLineSpec("r10000")<<"red 10 italic text"<<ColourLineSpec("g10000")<<"green 10 italic";
    */

    LinkList<LinkList<pair<string*, string*> *> *> markups; ///< Each of the strings is associated with a set of markup and markup variable strings
    LinkList<string*> strings; ///< The strings to apply the markups to

    enum pangoMarkupsIDs {COLOUR=1, SIZE, FONT}; ///< The IDs of the markups in pangoMarkups
    LinkList<string *> pangoMarkups; ///< The pango markup strings which are pointed to by markups

    string formattedMarkup; ///< The complete marked up string, set using getMarkup()

    /** Add a new markup linked list to markups and string to strings.
    This is called if strings.current()!=NULL and a markup operator<< is asked for.
    It is also called if the Pango::Reset is requested.
    */
    void nextTag(void){
        markups.add(new LinkList<pair<string*, string*> *>); // add a new markups link
        strings.add(NULL); // add a new strings link
    }
public:

    /** Constructor
    Starts in the state where both markups and strings have an empty entry.
    */
    Pango(void){
        pangoMarkups.add(new string("foreground")); // these must be in the same order of pangoMarkupsIDs
        pangoMarkups.add(new string("size"));
        pangoMarkups.add(new string("font"));
    }

    /** \brief Deconstructor
    Destructor, delete allocated memory and empty the linked lists
    */
    ~Pango(void){
        reset();
        while (pangoMarkups.getCount())
            delete pangoMarkups.remove();
    }

    /** Format the markups and strings to produce a final Pango markup.
    \return a reference to the formatted markup string.
    */
    string &getMarkup(void){
        formattedMarkup=""; // clear the markup string
        if (markups.getCount()){ // if markups and strings exist
            markups.grab(1); markups.prev(); // set both to the first element in their lists
            strings.grab(1); strings.prev();
            for (int i=1; i<=markups.getCount(); i++){ // cycle through each element
                markups.next(); strings.next();
                LinkList<pair<string*, string*> *> *currentMarkups= markups.current(); // get the list of markups strings and variables
                for (int j=1;j<=currentMarkups->getCount();j++){ // for each markup instruction, concat the markup and variable into formattedString
                    if (j==1) // if this is the last markup instruction, then complete the group of markups
                        formattedMarkup+="<span ";
                    currentMarkups->grab(j);
                    formattedMarkup+=(*currentMarkups->current()->first)+"='"+(*currentMarkups->current()->second)+"'";
                    if (j==currentMarkups->getCount()) // if this is the last markup instruction, then complete the group of markups
                        formattedMarkup+=">";
                    else // more to come, add a space
                        formattedMarkup+=" ";
                }
                // Now if the string is specified, add the string which is receiving the markup
                if (strings.current())
                    formattedMarkup+=(*strings.current());
                if (currentMarkups->getCount()) // finally, if a markup was specified, then end the markup session
                    formattedMarkup+="</span>";
            }
        }
        return formattedMarkup;
    }

    /** Given a GtkLabel, set its text using the current state of the markups and strings.
    */
    void setLabelText(GtkWidget *l){
        gtk_label_set_markup(GTK_LABEL(l), getMarkup().c_str());
    }

    /** Add a font name to the markup.
    If there is a string already existing, then start a new markups / strings pair
    The Font class is copied, to allow temporaries in overloading like so : << Font("Sans 20") <<
    */
    Pango &operator<<(Font f){
        if (!strings.getCount() | (strings.current()!=NULL)) // we have completed the last string, start a new markup and string
            nextTag();
        markups.current()->add(new pair<string*, string*>(pangoMarkups.grab(FONT), new string(f))); // add the font markup and the associated font string
        return *this;
    }

    /** Add a ColourLineSpec for marking up.
    If a string already exists, then start a new markups / strings pair.
    The ColourLineSpec class is copied, to allow temporaries in overloading like so : << ColourLineSpec("r10000") <<

    \param cls ColourLineSpec, where the integer is in thousands of a point, e.g. ColourLineSpec("r10000") is 10 point red.
    \return A reference to this Pango class for more overloading.
    */
    Pango &operator<<(ColourLineSpec cls){
        if (!strings.getCount() | (strings.current()!=NULL)) // we have completed the last string, start a new markup and string
            nextTag();
        if (cls.wasColourSpecified()){
            gchar *c=cls.getColourString();
            markups.current()->add(new pair<string*, string*>(pangoMarkups.grab(COLOUR), new string(c))); // add the size markup and the associated size string
            g_free(c);
        }
        if (cls.wasSizeSpecified()){
            ostringstream oss; oss<<cls.getSize(); // turn the size into a string
            markups.current()->add(new pair<string*, string*>(pangoMarkups.grab(SIZE), new string(oss.str()))); // add the size markup and the associated size string
        }
        return *this;
    }

    /** Add a string to be marked up.
    If a string has been added previously, then append to it.
    */
    Pango &operator<<(string &s){
        if (!strings.getCount()) nextTag(); // incase there is no markup just text
        if (strings.current()==NULL)
            strings.change(new string(s));
        else
            (*strings.current())+=s;
        return *this;
    }

    /** Add a character array to be marked up.
    If a string has been added previously, then append to it.
    */
    Pango &operator<<(const char *s){
        string str(s);
        return operator<<(str);
    }

    /** Handle the PangoIOS class specifiers
    */
    Pango &operator<<(PangoIOS::specialFlags sf){
        if (sf==PangoIOS::Next) // handle a nextTag request
            nextTag();
        if (sf==PangoIOS::Reset) // handle a reset request
            reset();
        return *this;
    }

    /** Reset the Pango object.
    Will remove and previous strings and markups
    */
    void reset(void){
        // remove any previous strings and markups
        while (markups.getCount()){ // remove the markup pair linked lists and remove/delete each pair/string
            LinkList<pair<string*, string*> *> *llp=markups.remove();
            while (llp->getCount()){
                pair<string*, string*> *p=llp->remove();
                delete p->second;
                delete p;
            }
            delete llp;
        }
        while (strings.getCount()) // remove and delete the markup strings
            delete strings.remove();
    }
};
#endif // PANGO_H_
