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
#ifndef HEAP_TREE_H_
#define HEAP_TREE_H_

#include "HeapTreeType.H"
#include "LinkList.H"

/** HeapTree
Implementation of a min. HeapTree with an included sort method.
It obeys the following properties :
shape property: the tree is a complete binary tree; that is, all levels of the tree, except possibly the last one (deepest) are fully filled, and, if the last level of the tree is not complete, the nodes of that level are filled from left to right.
heap property: each node is greater than or equal to each of its children according to a comparison predicate 'compare' defined for the data structure.
In this case, the 'compare' predicate defines a minimum heap tree (smallest at the top).

Whilst this HeapTree inherits from vector, certain operators can not be exposed to the user, consequently inheritance is protected.

A concept for expansion to non-binary heap trees is sought in the future, it is currently not supported.
\tparam HT_TYPE the type of the HeapTree
*/
template<class HT_TYPE>
class HeapTree : public HeapTreeType<HT_TYPE*> {
    /** The comparison typedefinition
        Assumes HT_TYPE is a pointer type.
    */
    typedef int(HT_TYPE::*HTCompareMethod)(const HT_TYPE&)const;

    /** Swaps the parent with it's children.
    If the parent evaluates to be bigger then it's child, then swap the parent and child.
    If both children are smaller, find the smallest child and swap with the parent.
    \param index The index of the parent for evaluation.
    \param compare The comparison predicate. If NULL, use the '<' logical operator. If compare!=NULL, assume HT_TYPE is a pointer type and evaluate (*parent).compare(*child)
    */
    void swapIfBigger(unsigned int index, HTCompareMethod compare){
        HeapTreeCell childL, childR;
        HeapTreeType<HT_TYPE*>::findChildren(index, childL, childR);
        unsigned int indexL=HeapTreeType<HT_TYPE*>::findIndex(childL);
        unsigned int indexR=HeapTreeType<HT_TYPE*>::findIndex(childR);
        //cout<<"indexL "<<indexL<<"indexR "<<indexR<<endl;
        int useIndexL=0, useIndexR=0;
        int res;
        if (indexL<HeapTreeType<HT_TYPE*>::unsortedCount){
            //    cout<<"indexL "<<(*(*this)[index])<<'\t'<<(*(*this)[indexL])<<'\t'<<(*(*this)[index].*compare)(*(*this)[indexL])<<endl;
                if (((*(*this)[index]).*compare)(*(*this)[indexL])<0)
                    useIndexL=1;
        }
        if (indexR<HeapTreeType<HT_TYPE*>::unsortedCount){
          //      cout<<"indexR "<<*(*this)[index]<<'\t'<<*(*this)[indexR]<<'\t'<<(*(*this)[index].*compare)(*(*this)[indexR])<<endl;
                if (((*(*this)[index]).*compare)(*(*this)[indexR])<0)
                    useIndexR=1;
        }
        if (useIndexR && useIndexL){
        //        cout<<"indexLR "<<*(*this)[indexL]<<'\t'<<*(*this)[indexR]<<'\t'<<(*(*this)[indexL].*compare)(*(*this)[indexR])<<endl;
                if (((*(*this)[indexL]).*compare)(*(*this)[indexR])<0)
                    useIndexL=0;
                else
                    useIndexR=0;
        }
        if (useIndexL){
//                cout<<"before index indexL "<<*(*this)[index]<<'\t'<<*(*this)[indexL]<<'\t'<<(*(*this)[index].*compare)(*(*this)[indexL])<<endl;
            HeapTreeType<HT_TYPE*>::swap(index,indexL);
  //              cout<<"after index indexL "<<*(*this)[index]<<'\t'<<*(*this)[indexL]<<'\t'<<(*(*this)[index].*compare)(*(*this)[indexL])<<endl;
            swapIfBigger(indexL, compare);
        }
        if (useIndexR){
    //            cout<<"before index indexR "<<*(*this)[index]<<'\t'<<*(*this)[indexR]<<'\t'<<(*(*this)[index].*compare)(*(*this)[indexR])<<endl;
            HeapTreeType<HT_TYPE*>::swap(index,indexR);
      //          cout<<"after index indexR "<<*(*this)[index]<<'\t'<<*(*this)[indexR]<<'\t'<<(*(*this)[index].*compare)(*(*this)[indexR])<<endl;
            swapIfBigger(indexR, compare);
        }
    }
public:

    /// Constructor - currrently only supports baseIn=2 : to be expanded in the future
    HeapTree(int baseIn=2) : HeapTreeType<HT_TYPE*>(baseIn) {
    }


    /** HeapTree add, which ensures that both shape and heap properties are satisfied.
    \param value The variable to add.
    \param compare The comparison predicate. If NULL, use the '<' logical operator. If compare!=NULL, assume HT_TYPE is a pointer type and evaluate (*parent).compare(*child)
    */
    void add(HT_TYPE *value, HTCompareMethod compare){
        HeapTreeType<HT_TYPE*>::unsortedCount+=1; // add
        if (HeapTreeType<HT_TYPE*>::unsortedCount+1>vector<HT_TYPE*>::size())
            vector<HT_TYPE*>::resize(HeapTreeType<HT_TYPE*>::unsortedCount+1);
        (*this)[HeapTreeType<HT_TYPE*>::unsortedCount]=value;
        // bubble up
        int index=HeapTreeType<HT_TYPE*>::unsortedCount;
            //this->dump(); cout<<endl;
        while (index>0){
            int parent=HeapTreeType<HT_TYPE*>::findParent(index);
            if ((*(*this)[parent].*compare)(*(*this)[index])<0){
                HeapTreeType<HT_TYPE*>::swap(parent, index);
                index=parent;
            //this->dump(); cout<<endl;
            } else
                break;
        }
            //this->dump(); cout<<endl;
    }

    /** Sort a HeapTree using the bubble sort algorithm.
    \param compare The comparison predicate. If NULL, use the '<' logical operator. If compare!=NULL, assume HT_TYPE is a pointer type and evaluate (*parent).compare(*child)
    */
    void sort(HTCompareMethod compare){
                // store first and bubble up
        unsigned int sizeOrig=HeapTreeType<HT_TYPE*>::unsortedCount;
        while (HeapTreeType<HT_TYPE*>::unsortedCount>0){
            unsigned int index=0;
            HeapTreeType<HT_TYPE*>::swap(index,HeapTreeType<HT_TYPE*>::unsortedCount);
            swapIfBigger(index, compare);
            HeapTreeType<HT_TYPE*>::unsortedCount-=1;
            //this->dump();
//            cout<<"\tunsortedCount="<<HeapTreeType<HT_TYPE*>::unsortedCount<<endl;
        }
    }

    /** convenience method to sort a LinkList
    The HT_TYPE::compare method must exist and be valid.
    Returns the same ll, with members sorted.
    \param ll A LinkList with members to sort.
    \param compare The comparison method to execute during evaluation.
    */
    void sort(LinkList<HT_TYPE *> &ll, HTCompareMethod compare){
        if (vector<HT_TYPE*>::size()!=ll.getCount()) // ensure the vector size matches
            resize(ll.getCount());
        while (ll.getCount()) // load the vector
            add(ll.remove(),compare);
        sort(compare); // sort
        for (int i=0; i<vector<HT_TYPE*>::size(); i++) // unload the vector
            ll.add((*this)[i]);
        HeapTreeType<HT_TYPE*>::deleteElements();
    }

    /** Delete the pointers in the vector
    */
    void deleteElements(void){
        for (int i=0;i<HeapTreeType<HT_TYPE*>::unsortedCount; i++){
            delete (*this)[i];
            (*this)[i]=NULL;
        }
        HeapTreeType<HT_TYPE*>::deleteElements();
    }
//    /** Run through the vector and output (to cout) the HT_TYPE tree element.
//    */
//    void dump(){
//        for (int i=0;i<(HeapTreeType<HT_TYPE*>::vector<HT_TYPE>::size());i++)
//            cout<<*(HeapTreeType<HT_TYPE*>::vector<HT_TYPE>::operator[](i))<<'\t';
//    }

//    HT_TYPE operator[](int i){
//        return HeapTreeType<HT_TYPE*>::operator[](i);
//    }
};
#endif //HEAP_TREE_H_

/** \example HeapTreeSort.C
 * This is an example of how to use the HeapTree class.
 * It will sort both int types using the HeapTreeType and strings using the HeapTree
 */
