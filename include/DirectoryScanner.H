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
#ifndef DIRECTORYSCANNER_H_
#define DIRECTORYSCANNER_H_

#include <errno.h>

#include <Debug.H>
#define DIRSCAN_OPENDIR_ERROR DIRSCAN_ERROR_OFFSET-1 ///< Error when the specified directory can't be found.
#define DIRSCAN_CLOSEDIR_ERROR DIRSCAN_ERROR_OFFSET-2 ///< Error when the specified dir doesn't close successfully
#define DIRSCAN_NOTOPEN_ERROR DIRSCAN_ERROR_OFFSET-3 ///< Error when the directory hasn't been open yet.
#define DIRSCAN_CREATEDIR_ERROR DIRSCAN_ERROR_OFFSET-4 ///< Error when the directory hasn't been open yet.
#define DIRSCAN_DIRDOESNTEXIST_ERROR DIRSCAN_ERROR_OFFSET-5 ///< Error when the directory hasn't been open yet.

#include <vector>

#if defined(_MSC_VER) || defined(__MINGW32__)
#include "posixForMicrosoft/dirent.h"
#include <direct.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h> // for getHomeDir
#include <unistd.h> // for getHomeDir
#endif
#include <string.h>

/** Debug class for the DirectoryScanner
*/
class DirectoryScannerDebug : public Debug {
public:
    /** Constructor defining all debug std::strings which match the debug defined variables
    */
    DirectoryScannerDebug() {
#ifndef NDEBUG
        errors[DIRSCAN_OPENDIR_ERROR]=std::string("Error when trying to open the directory ");
        errors[DIRSCAN_CLOSEDIR_ERROR]=std::string("Error when trying to close the directory. ");
        errors[DIRSCAN_NOTOPEN_ERROR]=std::string("You must open the directory first. ");
        errors[DIRSCAN_CREATEDIR_ERROR]=std::string("Could not create the directory named : ");
        errors[DIRSCAN_DIRDOESNTEXIST_ERROR]=std::string("Could not find that directory : ");

#endif
    }
};

/** Scan a directory for contents, either directories or other files.
The found names are in the inherited std::vector of std::strings.
*/
class DirectoryScanner : public std::vector<std::string> {
    DIR *dir;
    std::string path;
public:
  /** Empty constructor call open to operate.
  */
  DirectoryScanner() {
      dir=NULL;
  }

  /** Can the directory at path. The directoryItems are filled.
  \param path The path to scan for files/directories
  */
  DirectoryScanner(std::string path) {
      dir=NULL;
      open(path);
  }

    virtual ~DirectoryScanner() {
      if (dir)
        close();
    }

    /** Open the directory at path.
    \param path The path to scan for files/directories
    \return NO_ERROR or the appropriate error on failure.
    */
    int open(std::string path) {
        if (dir)
            close();
        dir = opendir(path.c_str()); // open the directory
        if (!dir) {
            char *errStr=strerror(errno);
            return DirectoryScannerDebug().evaluateError(DIRSCAN_OPENDIR_ERROR, path+". "+*errStr);
        }
        return NO_ERROR;
    }

    /** Close the directory
    \return NO_ERROR or the appropriate error on failure.
    */
    int close(void) {
        int ret=closedir(dir); // close the directory
        if (ret==-1) {
            char *errStr=strerror(errno);
            return DirectoryScannerDebug().evaluateError(DIRSCAN_CLOSEDIR_ERROR, std::string(errStr));
        }
        resize(0); // remove any previous entries
        path="";
        return NO_ERROR;
    }

    /** The directoryItems are filled in the directory represented by path.
    \param dontInclude The directories or files not to include
    \return NO_ERROR or the appropriate error on failure.
    */
    int findAll(const std::vector<std::string> &dontInclude) {
        if (!dir)
            return DirectoryScannerDebug().evaluateError(DIRSCAN_NOTOPEN_ERROR);
        const struct dirent *ent;
        rewinddir(dir); // go to the beginning
        while (ent = readdir(dir), ent != NULL) {
            bool excluded=false;
            //cout<<ent->d_name<<endl;
            for (unsigned int i=0; i<dontInclude.size(); i++){
                //cout<<"\t\t"<<dontInclude[i]<<'\t'<<strcmp(dontInclude[i].c_str(), ent->d_name)<<endl;
                if (strcmp(dontInclude[i].c_str(), ent->d_name)==0) {
                    excluded=true;
                    break;
                }
            }
            if (!excluded) // if not excluded then add to the list
                push_back(std::string(ent->d_name));
        }
        /* discern for the following on d_type in future - although not supported by all systems
        DT_BLK      This is a block device.
        DT_CHR      This is a character device.
        DT_DIR      This is a directory.
        DT_FIFO     This is a named pipe (FIFO).
        DT_LNK      This is a symbolic link.
        DT_REG      This is a regular file.
        DT_SOCK     This is a UNIX domain socket.
        DT_UNKNOWN  The file type is unknown.
        */
        return NO_ERROR;
    }

    /** Keep any item which has pattern in the path/name.
    \param pattern The pattern to find in the path/name
    */
    void keepWithPattern(const std::string pattern){
        for (int i=size()-1; i>=0; i--) // remove any elements which don't match the pattern
            if (operator[](i).find(pattern)==std::string::npos)
                erase(begin()+i);
    }

    /** Find the current path.
    \return The current path.
    */
    std::string getPath(){
        return path;
    }

    #if !defined(_MSC_VER) && !defined(__MINGW32__)
  /** Get the user's home directory
  \return The user's home directory if successful, empty string otherwise
  */
  static std::string getHomeDir(){
    struct passwd *pw = getpwuid(getuid());
    if (pw)
      return std::string(pw->pw_dir);
    else
      return std::string();
  }
#endif
	/** Make a directory.
	\param pathname The directory to make
	*/
#if !defined(_MSC_VER) && !defined(__MINGW32__)
	static int mkDir(const std::string &pathname, mode_t mode){
    printf("hi2 \n");
    std::string cmd("mkdir -p ");
    cmd+=pathname;
    printf("%s\n",cmd.c_str());
    int ret=system(cmd.c_str());
    printf("%d\n",ret);
    if (ret!=0) { // onfailure try with mkdir
      int res=mkdir(pathname.c_str(), mode);
      if (res==EDQUOT)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". The user's quota of disk blocks or inodes on the filesystem has been exhausted.");
      if (res==EFAULT)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+" points outside your accessible address space.");
      if (res==ELOOP)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". Too many symbolic links were encountered in resolving pathname.");
      if (res==ELOOP)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". Too many symbolic links were encountered in resolving pathname.");
      if (res==EMLINK)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". The number of links to the parent directory would exceed LINK_MAX.");
      if (res==ENAMETOOLONG)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+" was too long.");
      if (res==ENOENT)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". A directory component in pathname does not exist or is a dangling symbolic link.");
      if (res==ENOMEM)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". Insufficient kernel memory was available.");
      if (res==ENOSPC)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". The device containing pathname has no room for  the  new  directory.");

      if (res==ENOSPC)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". The  new  directory  cannot  be  created because the user's disk quota is exhausted.");
      if (res==ENOTDIR)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". A component used as a directory in pathname is not, in fact, a directory.");
      if (res==EPERM )
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". The filesystem containing pathname does not support the creation of directories.");
      if (res==EROFS)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+" refers to a file on a read-only filesystem.");
      if (res==EACCES)
       return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+". The  parent  directory  does  not  allow write permission to the process, or one of the directories in  pathname  did  not  allow search permission.");
      if (res==EEXIST)
        return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+" already exists");
    }
#else
	static int mkDir(const std::string &pathname) {
    printf("hi1 \n");
    int res=_mkdir(pathname.c_str());
    if (res==ENOENT)
      return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+" the path couldn't be found");
    if (res==EEXIST)
      return DirectoryScannerDebug().evaluateError(DIRSCAN_CREATEDIR_ERROR, pathname+" already exists");
#endif
	 return NO_ERROR;
	}
};

#endif // DIRECTORYSCANNER_H_
