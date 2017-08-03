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
#include "HeapTreeType.H"
#include "HeapTree.H"
#include <math.h>
#include <time.h>

// random string generation function
string randomStrGen(int length) {
    static string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    string result;
    result.resize(length);

    for (int i = 0; i < length; i++)
        result[i] = charset[rand() % charset.length()];

    return result;
}

// random number generation function
int findRand(int cnt){
    int r=(int)round((float)(rand()-RAND_MAX/2)*2.0/(float)RAND_MAX*(float)cnt);
    return r;
}

int main(int argc, char *argv[]){

    int cnt=12; // the number of elements to sort

    HeapTreeType<int> ht; // the heap tree which will sort int types
    ht.resize(cnt); // resize the heaptree (not necessary but more efficient)

    srand(time(NULL)); // generate a random seed

    // generate random numbers to sort
    cout<<'\n'<<endl;
    cout<<"sorting ints example\ninput : ";
    for (int i=0;i<cnt;i++)
        ht.add(findRand(cnt));
    ht.dump(); // display the numbers
    cout<<endl;

    ht.sort();  // sort the numbers

    cout<<"output : "; // display the sorted numbers
    ht.dump();

    cout<<'\n'<<endl;
    cout<<"sorting strings example\ninput : ";
    HeapTree<string> hts; // create the HeapTree for string type
    for (int i=0;i<cnt;i++){ // generate some random strings
        string *tempS=new string(randomStrGen(4));
        cout<<*tempS<<'\t';
        hts.add(tempS, &string::compare); // add the string to the HeapTree
    }
    cout<<endl;

    hts.sort(&string::compare); // do the sorting

    cout<<"output : "; // dump the sorted strings to std output
    hts.dumpDereference();
    cout<<'\n'<<endl;

    hts.deleteElements();//cleanup

    // an example sorting a LinkList.
    cout<<"sorting strings LinkList example\ninput : ";
    LinkList<string *> ll;
    for (int i=0;i<cnt;i++){ // generate some random strings
        string *tempS=new string(randomStrGen(4));
        cout<<*tempS<<'\t';
        ll.add(tempS); // add the string to the LinkList.
    }
    cout<<endl;

    hts.sort(ll, &string::compare);

    cout<<"output : "; // dump the sorted strings to std output
    ll.grab(1); ll.prev();
    for (int i=0;i<ll.getCount(); i++)
        cout<<*ll.next()<<'\t';
    cout<<endl;

    //cleanup
    while (ll.getCount())
        delete ll.remove();

    return 0;
}
