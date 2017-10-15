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
#include <time.h>

#include <iostream>
using namespace std;

// function to measure time
double diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return (double)temp.tv_sec+(double)temp.tv_nsec*1.e-9;
}

timespec time0; // start time

/** The simple interface for threading a single method.
*/
class FutexWaitThread : public ThreadedMethod {
  Futex *f;
  double sleepTime;
  void *threadMain(void){
    timespec timeHere;
    f->wait();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeHere);
      sleepTime=diff(time0,timeHere);
      sleep(1);
    return NULL;
  }
public:
  FutexWaitThread(Futex *fIn){
    f=fIn;
  }

  void wakeDelay(){
    cout<<"woken after "<<sleepTime<<"seconds\n\n";
  }
};

class WaitingThreadTest : public WaitingThread {
	double sleepTime;
public:
    void *threadMain(void){
			timespec timeHere;
      cond.lock();
      cond.wait();
      cond.unLock();
			sleepTime=diff(time0,timeHere);
			sleep(1);
      return NULL;
    }

		void wakeDelay(){
	    cout<<"woken after "<<sleepTime<<"seconds\n\n";
	  }
};


int main(int argc, char *argv[]){
	cout<<"This test shows that futex signalling is order(s) of magnitude faster then pthread signalling"<<endl;
	cout<<"calling futex wait test "<<endl;
  Futex f; // The futex class
  FutexWaitThread fwt(&f);
  fwt.run();
  FutexWaitThread fwt1(&f);
  fwt1.run();
  FutexWaitThread fwt2(&f);
  fwt2.run();
  sleep(1);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time0);
  f.wakeAll();
  sleep(2);
  fwt.wakeDelay();
  fwt1.wakeDelay();
  fwt2.wakeDelay();

	cout<<"calling pthread wait test "<<endl;

	WaitingThreadTest wt;
	wt.run(); // start the waiting thread
	sleep(1);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time0);
	wt.cond.lock(); // lock the mutex, indicate the condition and wake the thread.
	wt.cond.signal(); // Wake the WaitingThread
	wt.cond.unLock(); // Unlock so the WaitingThread can continue.
	sleep(1);
	wt.wakeDelay();

  return NO_ERROR;
}
