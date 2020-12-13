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

 #include <iostream>
 using namespace std;
#include "DirectoryScanner.H"

int main(int argc, char *argv[]){
    if (argc<2){
        std::cout<<"usage : "<<argv[0]<<" directoryName"<<std::endl;
        return -1;
    }
    string homeDir=DirectoryScanner().getHomeDir();
    cout<<"user's home directory = "<<homeDir<<endl;


    int ret=DirectoryScanner().mkDir(argv[1], S_IRWXU);
    cout<<"mkdir returns "<<ret<<endl;

    cout<<"now run :\nDirectoryScannerTest "<<argv[1]<<endl;
    return 0;
}
