/* Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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
#ifndef BST_H
#define BST_H

#include <stdlib.h>
#include <iostream>
using namespace std;

#include "LinkList.H"

#define BST_OK 0
#define BST_MALLOC_ERROR -1

/** Binary Search Tree
This class is useful for generating an ordered Binary Tree.
This is simple and fast for sorting your data.
\tparam BST_TYPE the type of the variables (values) to be stored/sorted.
*/
template<class BST_TYPE>
class BST{
    BST *childL, *childR; ///< Left and right children nodes.
    BST_TYPE *value; ///< The pointer to the variable
public:

    /// The comparison typedefinition
    typedef int(BST_TYPE::*BSTCompareMethod)(const BST_TYPE&)const;

    /** Constructor
    \param v a supplied value to store locally. Defaults to NULL
    \param cL left child value. Defaults to NULL
    \param cR right child value. Defaults to NULL
    */
    BST(BST_TYPE *v=NULL, BST* cL=NULL, BST* cR=NULL){
        childL=cL;
        childR=cR;
        value=v;
    }

    ///Destructor - any resident variable or children are deleted
    ~BST(void){
        if (childL) delete childL;
        childL=NULL;
        if (childR) delete childR;
        childR=NULL;
        if (value) delete value;
        value=NULL;
    }

    /** Add a new variable to the BST.
    This method is a recursive evaluate and store method.
    \param v the new variable to add
    \param compare the comparison method to use (should be true if v>value), if NULL then check using v>value
    \return BST_OK if OK, otherwise, BST_MALLOC_ERROR
    */
    int add(BST_TYPE *v, BSTCompareMethod compare=NULL){
        if (value==NULL){ // handle the root node
            value=v;
            return BST_OK;
        }
        BST** child;
        int comparison; // evaluate the comparison between v and value
        if (compare!=NULL) // if supplied, then utilise
            comparison=(*value.*compare)(*v);
        else // otherwise default to v>value
            comparison=(*v>*value);
        if (comparison<0) // if *v>*value comparison will be negative
            child=&childR;
        else
            child=&childL;
        if (*child==NULL){ // if there is no child, then create one
            *child=new BST<BST_TYPE>(v);
            if (!*child){
                cerr<<"couldn't create new BST child"<<endl;
                return BST_MALLOC_ERROR;
            }
        } else // if there is a child, then ask the child to evaluate and store
            return (*child)->add(v, compare);
        return BST_OK;
    }

    /** Take a LinkList<BST_TYPE *> and sort it.
    \param linkList a pointer to the LinkList<BST_TYPE> to sort.
    \param compare the method for comparing two values, see add.
    \return BST_OK on success. On failure, returns <0 AND the original contents of the linkList may be partially in both the BST and the LinkList<BST_TYPE *>
    */
    int sort(LinkList<BST_TYPE *> *linkList, BSTCompareMethod compare=NULL){
        if (!linkList->getCount()) // return on the empty list
            return BST_OK;
        int res=BST_OK;
        while (linkList->getCount()) // put each link into the BST
            if ((res=add(linkList->remove(), compare))<0)
                return res;
        removeSorted(linkList); // remove in a sorted fashion
    }

    /** Remove from the tree adding to the LinkList<BST_TYPE *> is a sorted manner.
    This yields a sorted linkList.
    \param linkList the pointer to the LinkList<BST_TYPE *> to add the value to.
    */
    void removeSorted(LinkList<BST_TYPE *> *linkList){
        if (childL){         // recursive removal of the left child.
            childL->removeSorted(linkList); // remove the child and set to NULL
            delete childL; childL=NULL;
        }

        linkList->add(value); // add to the linked list
        value=NULL;

        if (childR){         // recursive removal of the right child.
            childR->removeSorted(linkList); // remove the child and set to NULL
            delete childR; childR=NULL;
        }

    }

//    BST* operator[](char which){
//        if (which=='p')
//            return parent;
//        else if (which=='l')
//            return childL;
//        else if (which=='r')
//            return childR;
//        else {
//            cerr<<"your algorithm must index either 'p' for parent, 'l' for left child, 'r' for right child"<<endl;
//            exit(-1);
//        }
//    }

//    void dumpSorted(void){
//        if (childL)
//            childL->dumpSorted();
//        cout<<value<<'\t';
//        if (childR)
//            childR->dumpSorted();
//    }
//
//    void dumpDepthFirst(void){
//        cout<<value<<'\t';
//        if (childL)
//            childL->dumpDepthFirst();
//        if (childR)
//            childR->dumpDepthFirst();
//    }
//
//    void dumpPostOrder(void){
//        if (childL)
//            childL->dumpPostOrder();
//        if (childR)
//            childR->dumpPostOrder();
//        cout<<value<<'\t';
//    }
//

};
#endif //BST_H
