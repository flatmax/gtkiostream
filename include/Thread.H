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
#ifndef THREAD_H_
#define THREAD_H_

#ifdef USE_USE_GLIB_THREADS
#include <glib.h>
#else
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#endif

#include <string.h>
#include "Debug.H"

#define THREAD_CREATE_ERROR -0+THREAD_ERROR_OFFSET ///< Couldn't create the thread error
#define THREAD_MUTEX_UNINIT_ERROR -1+THREAD_ERROR_OFFSET ///< The mutex wasn't initialised error
#define THREAD_MUTEX_DEADLK_ERROR -2+THREAD_ERROR_OFFSET ///< The mutex is already locked
#define THREAD_MUTEX_DONTOWN_ERROR -3+THREAD_ERROR_OFFSET ///< The thread doesn't own the mutex
#define THREAD_MUTEX_DESTROY_LOCKED_ERROR -4+THREAD_ERROR_OFFSET ///< The mutex can't be destroyed when locked.
#define THREAD_MUTEX_LOCKBUSY_WARNING -5+THREAD_ERROR_OFFSET ///< The mutex can't be locked as it is already locked.
#define THREAD_NOTFOUND_ERROR -6+THREAD_ERROR_OFFSET ///< The thread can't be found
#define THREAD_COND_WAITBUSY_ERROR -7+THREAD_ERROR_OFFSET ///< One or more threads is waiting on the Cond variable.
#define THREAD_SCHED_ERROR -8+THREAD_ERROR_OFFSET ///< Scheduling setting error

class ThreadDebug : public Debug {
public:
    ThreadDebug(void) {
#ifndef NDEBUG
        errors[THREAD_CREATE_ERROR]=std::string("The thread couldn't be started.");
        errors[THREAD_MUTEX_UNINIT_ERROR]=std::string("The mutex has not been properly initialized.");
        errors[THREAD_MUTEX_DEADLK_ERROR]=std::string("The mutex has already been locked. Deadlock error.");
        errors[THREAD_MUTEX_DONTOWN_ERROR]=std::string("The mutex is not owned by the thread attempting to unlock, error.");
        errors[THREAD_MUTEX_DESTROY_LOCKED_ERROR]=std::string("You are trying to destroy the mutex which is locked, error.");
        errors[THREAD_MUTEX_LOCKBUSY_WARNING]=std::string("You are trying to lock a mutex which is already locked, you didn't acquire lock this time, try again.");
        errors[THREAD_NOTFOUND_ERROR]=std::string("That thread couldn't be found.");
        errors[THREAD_COND_WAITBUSY_ERROR]=std::string("One or more threads is waiting on the Cond variable. Can't destroy. ");
        errors[THREAD_SCHED_ERROR]=std::string("Problem setting the scheduling. ");

#endif
    }
};

/** Class to spawn a thread and meet an exited thread.
Can use GLib threads if you define USE_GLIB_THREADS, uses pthread otherwise. The main difference between using pthread and glib threads is
discussed in the meetThread method.
*/
class Thread {

#ifdef USE_GLIB_THREADS
    GThread* thread; ///< The thread pointer
    GError *threadErrorPtr; ///< The thread error pointer
#else
    pthread_t thread; ///< The thread structure
    int policy; ///< The thread policy, e.g. SCHED_FIFO
#endif

protected:
  /** manually clear the thread - generally not a good idea.
  */
  void clearThread(){
    #ifdef USE_GLIB_THREADS
                thread=NULL;
    #else
                thread=0;
    #endif
  }

public:
    /** Constructor
    */
    Thread(void) {
#ifdef USE_GLIB_THREADS
        thread=NULL;
        threadErrorPtr=NULL;
#else
        thread=0;
        policy = SCHED_FIFO;
#endif
    }

    /** Destructor
    */
    virtual ~Thread(void) {
#ifdef USE_GLIB_THREADS
        g_thread_join(thread);
        thread=NULL;
#else
//         void *retVal;
        pthread_cancel(thread); // this returns error of ESRCH if the thread is already finished

//        int threadResp=pthread_join(thread, &retVal);
        // on destruction, not interested in the return value here, just want to make sure the thread has exited.
//        if (threadResp!=0) {
//            errno=threadResp;
//            perror("Thread::meetThread : Couldn't meet the thread");
//            cerr<<"Most likely that the thread isn't running."<<endl;
//        }
#endif
    }

    /** Force the thread to stop.
    \return The exit value once the thread is met.
    */
    void *stop(void){
        int retVal = pthread_cancel(thread);
        if (retVal==ESRCH)
            ThreadDebug().evaluateError(THREAD_NOTFOUND_ERROR, " When trying to stop.");

        return meetThread();
    }

    /** Start a thread running start_routine, passing data to that function
    \param start_routine The function to run after the thread has been started
    \param data The argument to be pased to the thread
    \param priority The priority to schedule with if =0 then don't set the priority (pthreads only)
    \return NO_ERROR on success or THREAD_CREATE_ERROR on failure
    */
#ifdef USE_GLIB_THREADS
    int run(void (*start_routine) (void *), void *data, int priority=0) {
      priority=0;
#else
    int run(void *(*start_routine) (void *), void *data, int priority=0) {
#endif
#ifdef USE_GLIB_THREADS
        thread = g_thread_create((GThreadFunc)start_routine, (gpointer) data, TRUE, &threadErrorPtr);
        if (thread==NULL)
          return ThreadDebug().evaluateError(THREAD_CREATE_ERROR);
#else
        pthread_attr_t attr; // by default assumed PTHREAD_EXPLICIT_SCHED
        pthread_attr_init(&attr);
        int threadResp=0;

        if (priority>0)
          if (threadResp=setPriority(&attr, priority))
            return threadResp;

        if (threadResp=pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE))
          return ThreadDebug().evaluateError(threadResp, "Thread::run : Thread setting detatch state failed.");
//    int pthread_setcanceltype(int type, int *oldtype);

        if (threadResp=pthread_create(&thread, &attr, start_routine, data)){
          if (priority>0)
            return ThreadDebug().evaluateError(threadResp, "Thread::run : Thread creation failed. Check you have su or user permissions to create a thread with a specific priority.\n");
          else
            return ThreadDebug().evaluateError(threadResp, "Thread::run : Thread creation failed.\n");
        }

        pthread_attr_destroy(&attr);

        if (threadResp!=0)
            return ThreadDebug().evaluateError(THREAD_CREATE_ERROR);
#endif
        return 0;
    }

#ifndef USE_GLIB_THREADS
  /** Method to find the p_thread policy
  \return the policy (e.g. SCHED_FIFO)
  */
  int getPolicy(){return policy;}

  /** Method to get the maximum scheduling priority for this thread's policy
  \return the maximum priority or a negative vlue on error
  */
  int getMaxPriority(){return sched_get_priority_max(getPolicy());}

  /** Method to get the minimum scheduling priority for this thread's policy
  \return the minimum priority or a negative vlue on error
  */
  int getMinPriority(){return sched_get_priority_min(getPolicy());}

  int setPriority(pthread_attr_t *attributes, int priority){
    int res=0;
    if ((res = pthread_attr_setinheritsched(attributes, PTHREAD_EXPLICIT_SCHED)))
        return ThreadDebug().evaluateError(res, "Cannot request explicit scheduling for thread");

    if ((res = pthread_attr_setscope(attributes, PTHREAD_SCOPE_SYSTEM)))
      return ThreadDebug().evaluateError(res, "Cannot set scheduling scope for thread");

    if ((res = pthread_attr_setschedpolicy(attributes, getPolicy())))
      return ThreadDebug().evaluateError(res, "Cannot request schedule policy for thread");

    struct sched_param rt_param;
    memset(&rt_param, 0, sizeof(rt_param));
    rt_param.sched_priority = priority;

    if ((res = pthread_attr_setschedparam(attributes, &rt_param)))
      return ThreadDebug().evaluateError(res, "Cannot set scheduling priority for thread");

    return res;
  }

  int getPriority(){
    int res;
    struct sched_param rt_param;
    memset(&rt_param, 0, sizeof(rt_param));
    int policyLocal;
    if ((res=pthread_getschedparam(thread, &policyLocal, &rt_param)) != 0)
      return ThreadDebug().evaluateError(res," Getting sched param.");
    printf("Thread policy is %d priority is %d\n",policyLocal, rt_param.sched_priority);
    if (policyLocal != SCHED_FIFO)
      return ThreadDebug().evaluateError(THREAD_SCHED_ERROR, "Scheduling is NOT SCHED_FIFO!\n");
    return rt_param.sched_priority;
  }
#endif

    /** Wait for the thread to end
    \return If using USE_GLIB_THREADS then NULL, if using pthreads, then the argument passed to Thread::exit when called by the exiting thread.
    */
    void *meetThread(void) {
        if (thread) {
#ifdef USE_GLIB_THREADS
            g_thread_join(thread);
            clearThread();
#else
            void *retVal;
            int threadResp=pthread_join(thread, &retVal);
            if (threadResp!=0) {
                errno=threadResp;
                perror("Thread::meetThread : Couldn't meet the thread");
            }
            clearThread();
            return retVal;
#endif
        }
        return NULL;
    }

    /** Called to exit the thread passes retVal to the one who calls meetThread
    \param retVal The pointer to pass to the meetThread caller
    */
    void exit(void *retVal) {
#ifndef USE_GLIB_THREADS
        pthread_exit(retVal);
#endif
    }

    /** Find out if the thread is running.
    \return true if running, false if stopped.
    */
    bool running() {
#ifndef USE_GLIB_THREADS
        return thread!=(int)NULL;
#else
        return thread!=0;
#endif
    }
};

/** For the threading of a single method.
*/
class ThreadedMethod : public Thread {
#ifdef USE_GLIB_THREADS
    /** The static method which is called to begin the thread.
    */
    static void threadMainStatic(void *data) {
        static_cast<ThreadedMethod*>(data)->threadMainLocal();
    }
#else
    /** The static method which is called to begin the thread.
    */
    static void *threadMainStatic(void *data) {
      void *ret = static_cast<ThreadedMethod*>(data)->threadMainLocal();
      return ret;
    }
#endif

  /** Call the virtual threadMain and then clear the thread so people can test running.
  \returns void *
  */
  void *threadMainLocal(void){
    void *ret = threadMain();
    clearThread();
    return ret;
  }

public:
    /** The inheriting class implements the thread's main method.
    \return A pointer to a return variable (only used if USE_GLIB_THREADS is not defined = pthread )
    */
    virtual void *threadMain(void)=0;

    /** Start the threadMain in a new thread.
    \param prioirity Set the priority for the thread e.g. sched_get_priority_max(SCHED_FIFO)
    \return NO_ERROR on success, or a suitable error on failure : THREAD_CREATE_ERROR
    */
    virtual int run(int priority=0) {
        return Thread::run(threadMainStatic, static_cast<void*>(this), priority);
    }
};

#ifndef USE_USE_GLIB_THREADS
/** Class to protext mutual exclusion zones.
*/
class Mutex {
protected:
    pthread_mutex_t mut; ///< The POSIX mutex semaphore.
public:
    /** Constructor.
    Initialises the mutual exclusion device.
    */
    Mutex() {
        pthread_mutex_init(&mut, NULL); //defaults to a fast mutex PTHREAD_MUTEX_INITIALIZER
    }

    /** Destroys the mutual exclusion device.
    */
    virtual ~Mutex() {
        pthread_mutex_destroy(&mut);
//        int retVal=pthread_mutex_destroy(&mut);
//        if (retVal!=0) // returned with error
//            if (retVal==EBUSY)
//                ThreadDebug().evaluateError(THREAD_MUTEX_DESTROY_LOCKED_ERROR);
    }

    /** Lock the mutual exclusion zone.
    \return NO_ERROR on success, the suitable error code otherwise (EINVAL, EDEADLK).
    */
    int lock() {
        int retVal=pthread_mutex_lock(&mut);
        if (retVal!=0) { // returned with error
            if (retVal==EINVAL)
                return ThreadDebug().evaluateError(THREAD_MUTEX_UNINIT_ERROR);
            if (retVal==EDEADLK)
                return ThreadDebug().evaluateError(THREAD_MUTEX_DEADLK_ERROR);

        }
        return NO_ERROR;
    }

    /** Try to lock the mutual exclusion zone.
    \return NO_ERROR on success, the suitable error code otherwise (EINVAL, EBUSY).
    */
    int tryLock() {
        int retVal=pthread_mutex_trylock(&mut);
        if (retVal!=0) { // returned with error
            if (retVal==EINVAL)
                return ThreadDebug().evaluateError(THREAD_MUTEX_UNINIT_ERROR);
            if (retVal==EBUSY)
                return ThreadDebug().evaluateError(THREAD_MUTEX_LOCKBUSY_WARNING);

        }
        return NO_ERROR;
    }

    /** Try to lock the mutual exclusion zone.
    \return NO_ERROR on success, the suitable error code otherwise (EINVAL, EPERM when using a error checking mutex).
    */
    int unLock() {
        int retVal=pthread_mutex_unlock(&mut);
        if (retVal!=0) { // returned with error
            if (retVal==EINVAL)
                return ThreadDebug().evaluateError(THREAD_MUTEX_UNINIT_ERROR);
            if (retVal==EPERM) // this result is returned from error checking mutexes only
                return ThreadDebug().evaluateError(THREAD_MUTEX_DONTOWN_ERROR);

        }
        return NO_ERROR;
    }
};

/** Class which waits for a signal from another thread.
*/
class Cond : public Mutex {
    pthread_cond_t cond; ///< The POSIX condition variable
public:
    /// Constructor
    Cond(){
        // pthread_condattr_t cond_attr; // currently not used
        pthread_cond_init(&cond, NULL);
    }

    /// Destructor
    virtual ~Cond(){
        // pthread_condattr_t cond_attr; // currently not used
        int retVal = pthread_cond_destroy(&cond);
        if (retVal==EBUSY)
            ThreadDebug().evaluateError(THREAD_COND_WAITBUSY_ERROR);
    }

    /** Wait for the signal or broadcast.
    Assumes that the inherited Mutex::lock() method has already been called.
    Returns with Cond::Mutex in a locked state.
    */
    void wait(){
       pthread_cond_wait(&cond, &mut);
    }

    /** Signal a single waiting thread.
    Assumes that the inherited Mutex::lock() method has already been called.
    Returns with Cond::Mutex in a locked state.
    */
    void signal(){
        pthread_cond_signal(&cond);
    }

    /** Signal all waiting threads.
    Assumes that the inherited Mutex::lock() method has already been called.
    Returns with Cond::Mutex in a locked state.
    */
    void boroadcast(){
        pthread_cond_broadcast(&cond);
    }
};

/** Class for inter thread signaling and synchronisation.

Synchronisation is performed in the following manner :
\code
class WaitingThreadTest : public WaitingThread {
public:
    void *threadMain(void){ // This is the method to execute in the thread
        cond.lock();
        cond.wait();
        cout<<"WaitingThreadTest : I have been signalled "<<endl;
        cond.unLock();
        return NULL;
    }
};

// This is an example from the main thread ... shows you how to signal the waiting thread.
WaitingThreadTest waitingThread;
waitingThread.run(); // start the waiting thread - and give a second to start
sleep(1);
waitingThread.cond.lock(); // lock the mutex, indicate the condition and wake the thread.
waitingThread.cond.signal(); // Wake the WaitingThread
waitingThread.cond.unLock(); // Unlock so the WaitingThread can continue.
\endcode

If you need a Boolean test, then construct like so :
\code
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

// in your main thread
WaitingThreadTest waitingThread;
waitingThread.run(); // start the waiting thread - and give a second to start
sleep(1);
waitingThread.cond.lock(); // lock the mutex, indicate the condition and wake the thread.
waitingThread.ready=true;
waitingThread.cond.signal(); // Wake the WaitingThread
waitingThread.cond.unLock(); // Unlock so the WaitingThread can continue.
\endcode
*/
class WaitingThread : public ThreadedMethod {
public:
    Cond cond;

    virtual ~WaitingThread(void){
        cond.lock(); // signal the waiting thread so it isn't waiting and it is possible to shutdown.
        cond.signal();
        // don't unlock so that the waiting thread can't enter again during shutdown ... don't call here cond.unLock();
    }
};
#endif // ifndef USE_USE_GLIB_THREADS

#endif // THREAD_H_
