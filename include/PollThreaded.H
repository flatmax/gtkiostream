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
#ifndef POLL_H_
#define POLL_H_

#include <Thread.H>
#include <poll.h>

/** Class to handle polling on file descriptors
Usage :
\code

\endcode
*/
class PollThreaded : public ThreadedMethod {

  virtual void *threadMain(void){
    while (1) {
      int err=poll(&fds, 1, -1);
      if (err==0)
        printf("PollThreaded::threadMain : timeout, no events found");
      if (err>0){
        // printf("PollThreaded::threadMain : %d structures had events", err);
        err=processPollEvents();
        if (err<0)
          break;
      }
      if (err<0){
        Debug().evaluateError(errno);
        break;
      }
    }
    printf("PollThread::threadMain : returning and ending the thread.\n");
    return NULL;
  }

  /** Poll found events, overload this method to handle events.
  \return <0 to stop the thread processing and let it exit.
  */
  virtual int processPollEvents()=0;

protected:
  struct pollfd fds; ///< The file descriptors to watch

public:
  /// Constructor
  PollThreaded(){
  }

  /// Destructor
  virtual ~PollThreaded(){
  }
};
#endif // POLL_H_
