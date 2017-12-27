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
#if defined(_MSC_VER) || defined(__CYGWIN__) // Cygwin/Microsoft 
#include "TimeTools.H"
#else
#include "Time.H"
#endif

#include <iostream>
using namespace std;

int main(int argc, char *argv[]){
    Time t1, t2(100l, 100l);
    Time t3(t2);

    cout<<"t1 = "<<t1<<endl;
    cout<<"t2 = "<<t2<<endl;
    cout<<"t3 = "<<t3<<endl;

    cout<<"t2-t3 = "<<(t2-t3)<<endl;
    cout<<"t1-=t3 = "<<(t1-=t3)<<endl;

    cout<<"t1 secs : "<<t1.getSecs()<<endl;
    cout<<"t1 usecs : "<<t1.getUSecs()<<endl;

    cout<<"t2+t3 = "<<(t2+t3)<<endl;
    cout<<"t1+=t3 = "<<(t1+=t3)<<endl;

    t1.getTime();
    cout<<"current time = "<<t1<<endl;

    return 0;
}

