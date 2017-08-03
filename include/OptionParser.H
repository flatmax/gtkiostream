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
#ifndef OPTIONPARSER_H_
#define OPTIONPARSER_H_

#include <sstream>
#include <string>
using namespace std;
#include <string.h>

/** Class to parse options from the command line.
Example option forms on the command line are :
\code
binaryFile --opt=val -opt val
\endcode
*/
class OptionParser {
public:
    /** Get the integer matching key from argc/argv
    \param key The option key to search for
    \param argc The option count
    \param argv The option array of c strings
    \param ret [out] The value of the option matching key, if found, then it is set to that value, otherwise left the same
    \param i the arg number to start from
    \return the location of the argument, if == i then not found
    \tparam TYPE The type of the output argument
    */
    template<typename TYPE>
    int getArg(string key, int argc, char *argv[], TYPE &ret, int i) {
        string args;
        int rv=getArgString(key, argc, argv, args, i);
        stringstream arg;
        arg<<args;
        arg>>ret;
        return rv;
    }

    /** Get the integer matching key from argc/argv
    \param arg The argument to convert
    \param ret [out] The value of the arg typecase to TYPE
    \tparam TYPE The type of the output argument
    */
    template<typename TYPE>
    void convertArg(const char* arg, TYPE &ret) {
        stringstream argC;
        argC<<arg;
        argC>>ret;
    }

    /** Get the string matching key from argc/argv
    \param key The option key to search for
    \param argc The option count
    \param argv The option array of c strings
    \param ret [out] The integer value of the option matchin key, if found, then it is set to that value, otherwise left the same
    \param i the arg number to start from
    \return The location of the new argument, if == i then not found
    */
    int getArgString(string key, int argc, char *argv[], string &ret, int i) {
        int loc=i;
        do {
            // if found, then break, with loc==i.
            int startIndex=0; // strip any leading '-'
            while (argv[i][startIndex]=='-') startIndex++;
            string arg(&argv[i][startIndex]); // find the length of the arg
            istringstream input(arg);
            input.ignore(arg.length(),'=');
            unsigned int length=arg.find('=');
			if (length==string::npos)
				length=arg.size();
            if ((startIndex!=0) && (key.compare(0,key.length(),arg, 0, length)==0) && (arg.compare(0,length, key)==0)) { // we have a match !
                // find if the key is in the first position
                loc=i;
                loc++;
                if ((key.length()==arg.length()) & (i+1<argc)) { // argument is of the form : -count 10, so get the next arg.
                    if (argv[i+1][0]!='-') // guard against the -h -v case
                        ret=argv[++i];
                } else // argument is for the form : --count=10, so strip the initial part
                    if (length==key.length()) // ensure that we havn't matched -n for -net
                        input>>ret;
                break;
            }
        } while (++i<argc);
        return loc;
    }
};

#endif // OPTIONPARSER_H_
