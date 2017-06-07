/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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
#ifndef TIME_H_
#define TIME_H_

#ifdef _MSC_VER
//#error On microsoft, please rename this file to TimeTools.H and delete this line.
#endif

#include <glib.h>

#include <strstream>
using namespace std;

/** Class to specify the timeFormat string for formatted output.
Implemented in this manner, rather then a simple member variable of the class Time to allow multiple inheritance of this header file.
*/
template <int dummy>
class TimeFormat {
protected:
	static char timeFormat[]; ///< The time format to use for converting seconds to a human readable time.
};
template <int dummy> char TimeFormat<dummy>::timeFormat[]="%Y-%m-%d.%H:%M:%S";

/** Cross platform time.
*/
class Time : protected TimeFormat<0> {
    GTimeVal time;
public:
    // Constructor
    Time() {time.tv_sec=time.tv_usec=0;}

    /** Construct and initialise the time.
    \param sec The seconds component of the time.
    \param usec The micro-seconds component of the time.
    */
    Time(long sec, long usec) {
        time.tv_sec=sec;
        time.tv_usec=usec;
    }

    virtual ~Time() {}

    /** get sex : returns the seconds component of the time variable.
    \return the seconds value of the private time variable.
    */
    glong getSecs() const {return time.tv_sec;}

    /** get u sex : returns the micro-seconds component of the time variable.
    \return the seconds value of the private time variable.
    */
    glong getUSecs() const {return time.tv_usec;}

    /** Get the current time.
    \return A reference to this object for use with operators.
    */
    Time &getTime(){
        g_get_current_time(&time);
        return *this;
    }

    /** Subtraction operator : subtract the provided time from the local time variable.
    \param t The time to subtract.
    \return this time minus the provided time.
    */
    Time operator-(const Time &t){
        return Time(time.tv_sec-t.getSecs(), time.tv_usec-t.getUSecs());
    }

    /** Subtract and assign operator : subtract the provided time from the local time variable.
    \param t The time to subtract.
    \return A reference to this Time class, this time minus the provided time.
    */
    Time &operator-=(const Time &t){
        time.tv_sec-=t.getSecs(); time.tv_usec-=t.getUSecs();
        return *this;
    }

    /** Addition operator : add the provided time from the local time variable.
    \param t The time to add.
    \return this time plus the provided time.
    */
    Time operator+(const Time &t){
        return Time(time.tv_sec+t.getSecs(), time.tv_usec+t.getUSecs());
    }

    /** Addition and assign operator : add the provided time from the local time variable.
    \param t The time to add.
    \return A reference to this Time class, where this time plus the provided time.
    */
    Time &operator+=(const Time &t){
        time.tv_sec+=t.getSecs(); time.tv_usec+=t.getUSecs();
        return *this;
    }

    friend ostream &operator<<(ostream &o, const Time &t){
        ostrstream oss;
		long double tDD=(long double)t.getSecs()+(long double)t.getUSecs()*1.e-6;
        oss<<tDD<<'\0';
        o<<oss.str();
        return o;
    }

	/** Get a textual representation of the current time.
	Uses the timeFormat string to format the time into a string.
	\return the time as a string, and empty string on error
	*/
	static string getTimeString(){
		return getTimeString(NULL);
	}

	/** Get a textual representation of the current time.
	\param formatStr the format to use when turning the time into a string, if NULL, then use timeFormat.
	\return the time as a string, and empty string on error
	*/
	static string getTimeString(char *formatStr){
		time_t timeNow; ::time(&timeNow); // find out the time now as a nice string
		struct tm *tmLocal = localtime(&timeNow);
		//cout<<tmLocal<<endl;
		int maxSize=128;
		string strTime(maxSize, '\0'); // the string to hold the current time.
		int ret;
		if (formatStr==NULL)
			ret=strftime((char*)strTime.c_str(), maxSize, timeFormat, tmLocal);
		else
			ret=strftime((char*)strTime.c_str(), maxSize, formatStr, tmLocal);
		if (ret<=0)
			strTime="";
		else
			strTime[ret]='\0';
		//cout<<strTime<<endl;
		return strTime;
	}
};
#endif // TIME_H_
