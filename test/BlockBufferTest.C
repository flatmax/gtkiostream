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

#include "BlockBuffer.H"
#include <iostream>

int main(int argc, char *argv[]) {
    BlockBuffer bb;
    bb.resizeBuffers(3,5);

    Eigen::Array<short unsigned, Eigen::Dynamic, Eigen::Dynamic> *b1=bb.getEmptyBuffer(); // get an full buffer
    std::cout<<"get empty buffer "<<std::endl;
    Eigen::Array<short unsigned, Eigen::Dynamic, Eigen::Dynamic> *b2=bb.getEmptyBuffer(); // get an full buffer
    std::cout<<"get empty buffer "<<std::endl;
    for (int i=0;i<b1->rows();i++){
        for (int j=0;j<b1->cols();j++){
                std::cout<<"i,j = "<<i<<','<<j<<'\t';
            (*b1)(i,j)=1;
            (*b2)(i,j)=2;
        }
        std::cout<<std::endl;
    }
    std::cout<<"set constants  "<<std::endl;

    bb.putFullBuffer(b2);
    bb.putFullBuffer(b1);
    std::cout<<"put full buffers"<<std::endl;

    b1=bb.getFullBuffer();
    std::cout<<(*b1)<<std::endl;
    bb.putEmptyBuffer(b1);

    b1=bb.getFullBuffer();
    std::cout<<*b1<<std::endl;
    bb.putEmptyBuffer(b1);

    b1=bb.getEmptyBuffer();
    std::cout<<*b1<<std::endl;
    bb.putEmptyBuffer(b1);

    b1=bb.getEmptyBuffer();
    std::cout<<*b1<<std::endl;
    bb.putEmptyBuffer(b1);

    b1=bb.getEmptyBuffer();
    std::cout<<*b1<<std::endl;
    bb.putEmptyBuffer(b1);

    std::cout<<" resizing"<<std::endl;
    bb.resize(4);
    b1=bb.getEmptyBuffer();
    std::cout<<*b1<<std::endl;
    b1=bb.getEmptyBuffer();
    std::cout<<*b1<<std::endl;
    b1=bb.getEmptyBuffer();
    std::cout<<*b1<<std::endl;
    b1=bb.getEmptyBuffer();
    std::cout<<*b1<<std::endl;
    return 0;
}
