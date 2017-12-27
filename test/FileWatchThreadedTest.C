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

#include "FileWatchThreaded.H"
#include <iostream>

int main(int argc, char *argv[]){
    if (argc<2){
        std::cout<<"usage : "<<argv[0]<<" fileName or directoryName"<<std::endl;
        return -1;
    }
    FileWatchThreaded fwt;
    fwt.add(argv[1]); // add the file system thing to watch
    std::cout<<"Please modify "<<argv[1]<<std::endl;
    fwt.run();
    
    fwt.meetThread(); // wait for the thread to exit

    return 0;
}
