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
#include "BST.H"
#include <math.h>
#include <time.h>

// function to generate a random string
string randomStrGen(int length) {
    static string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    string result;
    result.resize(length);

    for (int i = 0; i < length; i++)
        result[i] = charset[rand() % charset.length()];

    return result;
}

// function to generate a random int
int findRand(int cnt){
    int r=(int)round((float)(rand()-RAND_MAX/2)*2.0/(float)RAND_MAX*(float)cnt);
    return r;
}

int main(int argc, char *argv[]){
    srand(time(NULL)); // set the random seed

    int cnt=10; // the number of items to generate

    cout<<"want to sort the following :"<<endl;

    // add a bunch of strings to the link list - could be done with ints as well
    LinkList<string *> linkList;
    for (int i=0;i<cnt;i++){
        string *tempS=new string(randomStrGen(4));
        linkList.add(tempS);
        cout<<*tempS<<'\t';
    }
    cout<<endl;

    BST<string> bst; // create the Binary Search Tree
    bst.sort(&linkList, &string::compare); // sort the LinkList

    cout<<"The result is :"<<endl; // display the result
    linkList.grab(1); linkList.prev();
    for (int i=1; i<=linkList.getCount(); i++)
        cout<<(*linkList.next())<<'\t';
    cout<<endl;

    while (linkList.getCount()) // empty the LinkList
        delete linkList.remove();
    return 0;
}
