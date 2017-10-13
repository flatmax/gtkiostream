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

/*
Author: Matt Flax <flatmax@flatmax.org>
Date: 2013.05.03
*/

#include "Futex.H"
#include "Thread.H"
#include <unistd.h>

#include <iostream>
using namespace std;

/** The simple interface for threading a single method.
*/
class FutexWaitThread : public ThreadedMethod {
  Futex *f;

  void *threadMain(void){
    cout<<"sleeping"<<endl;
    f->wait();
    cout<<"woken"<<endl;
    return NULL;
  }
public:
  FutexWaitThread(Futex *fIn){
    f=fIn;
  }
};


int main(int argc, char *argv[]){
  cout<<INT_MAX<<endl;
  Futex f; // The futex class
  FutexWaitThread fwt(&f);
  fwt.run();
  FutexWaitThread fwt1(&f);
  fwt1.run();
  FutexWaitThread fwt2(&f);
  fwt2.run();
  sleep(1);
  cout<<"about to call wake"<<endl;
  f.wakeAll();
  cout<<"called wake"<<endl;
  sleep(1);
    return NO_ERROR;
}
