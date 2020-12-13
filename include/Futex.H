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
#ifndef FUTEX_H_
#define FUTEX_H_

#include <linux/futex.h>
#include <cstddef>
#include <limits.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "Debug.H"

/** Class to implement Futex signalling.
*/
class Futex {
  #define DEFAULT_START_VAL 0
  int f; ///< The futex variable
public:
  Futex(){
    f=DEFAULT_START_VAL; // start with default wait value
  }

  /** Wait on the wake signal (using the DEFAULT_START_VAL)
  \return 0 on success, or <0 on failure
  */
  int wait(){
    return waitVal(DEFAULT_START_VAL);
  }

  /** Wait on the wake signal or if val hasn't changed.
  \param val The waiting value for f : if still this value, then wait
  \return 0 on success, or <0 on failure
  */
  int waitVal(int val){
    // if (__sync_bool_compare_and_swap(&f, 1, 0))
    //    return 0;
    int ret = syscall(SYS_futex, &f, FUTEX_WAIT, val, NULL, NULL, 0);
    if (ret<0)
      return Debug().evaluateError(ret);
    return ret;
  }

  /** Wakes up threads in the wait method.
  \param howMany INT_MAX for all, otherwise <INT_MAX for that many.
  \returns the number of waiters woken up or <0 on error
  */
  int wake(int howMany){
    // __sync_bool_compare_and_swap(&f, 0, 1);
    howMany = syscall(SYS_futex, &f, FUTEX_WAKE, howMany, NULL, NULL, 0);
    if (howMany<0) // do we have an error ? Force system strerror printing if so ,,,
      return Debug().evaluateError(howMany);
    return howMany; // howMany woken up, report back
  }

  /** Wake all waiting threads.
  \returns the number of waiters woken up or <0 on error
  */
  int wakeAll(){
    return wake(INT_MAX);
  }
};

#endif //FUTEX_H_
