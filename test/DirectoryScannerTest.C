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

#include "DirectoryScanner.H"
#include <iostream>

int main(int argc, char *argv[]){
    if (argc<2){
        std::cout<<"usage : "<<argv[0]<<" directoryName"<<std::endl;
        return -1;
    }
    DirectoryScanner ds(argv[1]);
    ds.findAll(std::vector<std::string>());
    std::cout<<"Found the following in the directory "<<ds.getPath()<<std::endl;
    for (int i=0; i<ds.size(); i++)
        std::cout<<ds[i]<<std::endl;

    std::vector<std::string> excluded;
    excluded.push_back(".");
    excluded.push_back("..");

    ds.resize(0); // clear the known elements in the directory.
    ds.findAll(excluded);
    std::cout<<"Found the following in the directory (excluding . and ..) "<<ds.getPath()<<std::endl;
    for (int i=0; i<ds.size(); i++)
        std::cout<<ds[i]<<std::endl;

    return 0;
}
