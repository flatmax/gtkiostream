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

#include "Thread.H"
#include <unistd.h>

#include <iostream>
using namespace std;

class ThreadedClass : public Thread {
    int variable;
public:
#ifdef USE_GLIB_THREADS
    static void staticMethod(void *data){
        ThreadedClass *tc=static_cast<ThreadedClass*>(data);
        tc->method();
    }
#else
    static void *staticMethod(void *data){
        ThreadedClass *tc=static_cast<ThreadedClass*>(data);
        tc->method();
        cout <<"data"<<data<<endl;
        tc->exit(data);
        return data;
    }
#endif

    void setVariable(int var){
        variable=var;
    }

    void method(void){
        sleep(1);
        cout<<"varibale = "<<++variable<<endl;
    }
};

/** The simple interface for threading a single method.
*/
class ThreadedMethodClass : public ThreadedMethod {
    void *threadMain(void){
        sleep(2);
        cout<<"hi again"<<endl;
        return NULL;
    }
};

/** Test class for locking the Mutex class.
*/
class ThreadedLockClass : public ThreadedMethod {
    virtual void *threadMain(void){
        mutex->lock();
        cout<<"Mutex locked"<<endl;
        return NULL;
    }
protected:
    Mutex *mutex;
public:
    ThreadedLockClass(Mutex *m) : ThreadedMethod() {
        mutex=m;
    }
};

/** Test class for locking the Mutex class.
*/
class ThreadedUnLockClass : public ThreadedLockClass {
    void *threadMain(void){
        sleep(2);
        mutex->unLock();
        cout<<"Unlocking the mutex"<<endl;
        return NULL;
    }
public:
    ThreadedUnLockClass(Mutex *m) : ThreadedLockClass(m) {
    }
};

/** The simple interface for threading a single method.
*/
class ForeverThread : public ThreadedMethod {
    void *threadMain(void){
        while (1){
            sleep(1);
            cout<<"Forever thread, 1s has passed"<<endl;
        }
        return NULL;
    }
};

class WaitingThreadTest : public WaitingThread {
public:
    bool ready;

    WaitingThreadTest(){
        ready=false;
    }

    void *threadMain(void){
        while (running()){ // while this thread is running
            cond.lock();
            while (!ready)
                cond.wait();
            cout<<"WaitingThreadTest: I have been signalled and ready is true!"<<endl;
            ready=false;
            cond.unLock();
        }
        return NULL;
    }
};

class WaitingThreadTest2 : public WaitingThread {
public:
    void *threadMain(void){
        cond.lock();
        cond.wait();
        cout<<"WaitingThreadTest2 : I have been signalled "<<endl;
        cond.unLock();
        return NULL;
    }
};

int main(int argc, char *argv[]){

    ThreadedClass threadedClass;
    threadedClass.setVariable(3);
    int res=threadedClass.run(threadedClass.staticMethod, static_cast<void*>(&threadedClass));
    cout<<&threadedClass<<'\t'<<threadedClass.meetThread()<<endl;

    ThreadedMethodClass threadedMethodClass;
    res=threadedMethodClass.run();
    cout<<"\nwaiting for the second thread to exit"<<endl;
    cout<<threadedMethodClass.meetThread()<<endl;

    Mutex mutex;
    cout<<"testing mutex lock unlock"<<endl;
    mutex.lock();
    mutex.unLock();
    cout<<"testing mutex unlock twice"<<endl;
    mutex.unLock();

    ThreadedLockClass tlc(&mutex); tlc.run();
    ThreadedUnLockClass tuc(&mutex); tuc.run();
    sleep(1);
    cout<<"Main thread: waiting for the mutex to unlock."<<endl;
    mutex.lock();
    cout<<"Main thread: mutex has unlocked."<<endl;
    mutex.unLock();

    ForeverThread ft;
    ft.run();
    sleep(2);
    cout<<"about to stop the forever thread"<<endl;
    ft.stop();
    cout<<"Stopped"<<endl;

    WaitingThreadTest2 waitingThread2;
    waitingThread2.run(); // start the waiting thread
    sleep(1);
    waitingThread2.cond.lock(); // lock the mutex, indicate the condition and wake the thread.
    cout<<"\nsignalling the thread"<<endl;
    waitingThread2.cond.signal(); // Wake the WaitingThread
    cout<<"unlocking the thread"<<endl;
    waitingThread2.cond.unLock(); // Unlock so the WaitingThread can continue.

    WaitingThreadTest waitingThread;
    waitingThread.run(); // start the waiting thread
    sleep(1);
    waitingThread.cond.lock(); // lock the mutex, indicate the condition and wake the thread.
    waitingThread.ready=true;
    cout<<"\nsignalling the thread"<<endl;
    waitingThread.cond.signal(); // Wake the WaitingThread
    cout<<"unlocking the thread"<<endl;
    waitingThread.cond.unLock(); // Unlock so the WaitingThread can continue.

    sleep(2); // wait for the waiting thread to complete
    return NO_ERROR;
}
