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
#ifndef FILEWATCHTHREAD_H_
#define FILEWATCHTHREAD_H_

#include <Thread.H>
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <stdexcept>

/** Class to watch a file and execute on state change - based on inotify
Usage is like so :
\code{.cpp}
  FileWatchThreaded fwt;
  fwt.add(fileOrDirName); // add the file system thing to watch
  fwt.run();
\endcode
At this point, a thread has started and it is watching the directorys or files you specified. If they change, then
you can overload modified or closeWrite below to catch those events.
Here is an example of a class which overloads those methods :
\code{.cpp}
  class Watcher : public FileWatchThread {
    virtual void modified(char *name){
    printf("modified occured \n");
    }
    virtual void closeWrite(char *name){
      printf("close write occured \n");
    }
  };
\endcode
*/
class FileWatchThreaded : public ThreadedMethod {
    int fd; ///< The inotify file descriptor
    #define MAX_EVENTS 10 ///< Max number of events to read at one time
    #define BUF_SIZE MAX_EVENTS*(sizeof(struct inotify_event) + NAME_MAX + 1)
    char eventBuf[BUF_SIZE] __attribute__ ((aligned(__alignof__(struct inotify_event)))); ///< The buffered events

    std::map<std::string, int> pathFds; ///< This will contain a map between watch file descriptors and file/dir names

    /** Overload this function to deal with IN_MODIFY events
    \param name When watching a directory, the file name modified is given here
    */
    virtual void modified(char *name){}

    /** Overload this function to deal with IN_CLOSE_WRITE events
    \param name When watching a directory, the file name modified is given here
    */
    virtual void closeWrite(char *name){
      // printf("FileWatchThreaded::close write\n");
    }

    /** Overload this function to deal with IN_ATTRIB events
    \param name When watching a directory, the file name modified is given here
    \param wd The watch descriptor
    */
    virtual void attribChange(char *name, int wd){}

    /** Overload this function to deal with IN_ATTRIB events
    \param name When watching a directory, the file name modified is given here
    */
    virtual void deleteSelf(char *name){}

protected:
    /** The file watching thread.
    */
    virtual void *threadMain(void){
      while (int numRead = read(fd, eventBuf, BUF_SIZE)) {
        if (numRead == 0 | numRead == -1){
          printf("FileWatchThreaded::threadMain : read() from inotify fd returned %d !\n",numRead);
          Debug().evaluateError(numRead); // print strerror
          break;
        }
        for (char *p = (char*)eventBuf; p < eventBuf + numRead; p += sizeof(struct inotify_event) + reinterpret_cast<struct inotify_event *>(p)->len) {
          // printf("event %s\n",reinterpret_cast<struct inotify_event *>(p)->name);
          if (reinterpret_cast<struct inotify_event *>(p)->mask & IN_MODIFY)
            modified(reinterpret_cast<struct inotify_event *>(p)->name);
          if (reinterpret_cast<struct inotify_event *>(p)->mask & IN_CLOSE_WRITE)
            closeWrite(reinterpret_cast<struct inotify_event *>(p)->name);
          if (reinterpret_cast<struct inotify_event *>(p)->mask & IN_ATTRIB)
            attribChange(reinterpret_cast<struct inotify_event *>(p)->name, reinterpret_cast<struct inotify_event *>(p)->wd);
          if (reinterpret_cast<struct inotify_event *>(p)->mask & IN_DELETE_SELF)
            deleteSelf(reinterpret_cast<struct inotify_event *>(p)->name);
            // Other unhandled events which can be added ...
            // IN_ACCESS, IN_CLOSE_NOWRITE, IN_CREATE, IN_DELETE, IN_DELETE_SELF,IN_IGNORED
            // IN_ISDIR, IN_MOVE_SELF, IN_MOVED_FROM, IN_MOVED_TO, IN_OPEN, IN_Q_OVERFLOW, IN_UNMOUNT
        }
      }
      return NULL;
    }
public:
  /// Constructor
  FileWatchThreaded(){
    fd = inotify_init();
    if (fd == -1){
        Debug().evaluateError(fd, " FileWatchThreaded : When calling inotify_init.");
        assert(0); // exit on failure
    }
  }

  /// Destructor
  virtual ~FileWatchThreaded(){
  }

  /** Add a file or directory to watch here
  \param path The directory or file to watch
  \return 0 on success
  */
  virtual int add(std::string path){
    int ret = inotify_add_watch(fd, path.c_str(), IN_ALL_EVENTS);
    if (ret == -1)
      return Debug().evaluateError(fd, " FileWatchThreaded : When calling inotify_add_watch.");
    pathFds[path]=ret; // store the path fd
    return 0;
  }

  /** Remove a file descriptor from the inotify watch system.
  \param fd The file descriptor to remove
  \return 0 on success
  */
  virtual int rm(int fdw){
    int ret=inotify_rm_watch(fd, fdw);
    if (ret!=0)
      Debug().evaluateError(fd, "couldn't remove the fd from the inotify subsystem\n");
    for (std::map<std::string, int>::iterator it=pathFds.begin(); it!=pathFds.end(); ++it)
      if (it->second == fdw){
        pathFds.erase(it->first);
        break;
      }
    return ret;
  }

  /** Get the file descriptor for a particular path if it is in the list of watched file descriptors.
  \return The fd on success. Returns ENOENT is not in existance.
  */
  int getFD(std::string path){
    int fdw;
    try {
      fdw=pathFds.at(path);
    } catch (const std::out_of_range& oor){
      return Debug().evaluateError(-ENOENT, " FileWatchThreaded : no fd for that path is currently being watched.");
    }
    return fdw;
  }

  /** Add a file or directory to watch here, if it doesn't exist, then create
  \param path The directory or file to watch
  \return 0 on success
  */
  virtual int addFile(std::string path){
    int ret=add(path);
    if (ret==ESRCH) { // file doesn't exist, attempt to create
      FILE *fid=fopen(path.c_str(), "a");
      if (fid==NULL) // check for failure
        ret=-EIO;
      else {
        fclose(fid);
        ret=add(path); // now try to add the file a second time
      }
    }
    return ret;
  }

  void dumpContents(){
    for (std::map<std::string, int>::iterator it=pathFds.begin(); it!=pathFds.end(); ++it)
      printf("%s => %d\n",it->first.c_str(), it->second);
  }
};

#endif // FILEWATCHTHREAD_H_
