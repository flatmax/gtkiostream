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
#ifndef HEAP_TREE_TYPE_H_
#define HEAP_TREE_TYPE_H_

#include <stdlib.h>
#include <iostream>
#include <math.h>

//#include "Array.H"
#include <vector>

using namespace std;

#include <assert.h>

/** A class to manage the tree co-ordinates.
The tree co-ordinates are the tree level and index in the level.

*/
class HeapTreeCell{
public:
    unsigned int level; ///< The level of the tree which is pointed to, from the top.
    unsigned int child; ///< The numeric count of the child in the tree from the top.

//    /** Overloads the output operator.
//    This is usefull for debugging.
//    \param c The ostream to output to
//    \param cell The cell to put into the ostream
//    \return A reference to the ostream for further operations.
//    */
//    friend ostream& operator<<(ostream& c, const HeapTreeCell& cell);
};
//ostream& operator<<(ostream& c, const HeapTreeCell& cell){
//    c<<"("<<cell.level<<", "<<cell.child<<")";
//}

/** HeapTreeType for constant variables
Implementation of a min. HeapTreeType with an included sort method.
It obeys the following properties :
shape property: the tree is a complete binary tree; that is, all levels of the tree, except possibly the last one (deepest) are fully filled, and, if the last level of the tree is not complete, the nodes of that level are filled from left to right.
heap property: each node is greater than or equal to each of its children according to a comparison predicate.

Whilst this HeapTreeType inherits from vector, certain operators can not be exposed to the user, consequently inheritance is protected.

A concept for expansion to non-binary heap trees is sought in the future, it is currently not supported.

A simplistic '<' is used for comparison and it is assumed that the HT_TYPE is capable of being compared in this way, e.g. when HT_TYPE in int.

\tparam HT_TYPE the type of the HeapTreeType
*/
template<class HT_TYPE>
class HeapTreeType : protected vector<HT_TYPE> {
    float base;

    /** Swaps the parent with it's children.
    If the parent evaluates to be bigger then it's child, then swap the parent and child.
    If both children are smaller, find the smallest child and swap with the parent.
    \param index The index of the parent for evaluation.
    */
    void swapIfBigger(unsigned int index){
        //this-> dump(); cout<<endl;
        HeapTreeCell childL, childR;
        findChildren(index, childL, childR);
        unsigned int indexL, indexR;
        indexL=findIndex(childL); indexR=findIndex(childR);
        int useIndexL=0, useIndexR=0;
        int res;
        if (indexL<unsortedCount)
                if ((*this)[index]<(*this)[indexL])
                    useIndexL=1;
        if (indexR<unsortedCount)
                if ((*this)[index]<(*this)[indexR])
                    useIndexR=1;
        if (useIndexR && useIndexL)
                if ((*this)[indexL]<(*this)[indexR])
                    useIndexL=0;
                else
                    useIndexR=0;
        if (useIndexL){
            swap(index,indexL);
            swapIfBigger(indexL);
        }
        if (useIndexR){
            swap(index,indexR);
            swapIfBigger(indexR);
        }
    }
protected:
    int unsortedCount;

    /** Swap two elements in the heap tree.
    \param one The index of the one to swap with two.
    \param two The index of the two to swap with one.
    */
    void swap(unsigned int one, unsigned int two){
        HT_TYPE temp=(*this)[one];
        (*this)[one]=(*this)[two];
        (*this)[two]=temp;
    }

public:
    /// Constructor - currrently only supports baseIn=2 : to be expanded in the future
    HeapTreeType(int baseIn=2) {
        assert(baseIn==2);
        base=(float)baseIn; // binary tree
        deleteElements();
    }

    /** HeapTreeType add, which ensures that both shape and heap properties are satisfied.
    \param value The variable to add.
    */
    void add(HT_TYPE value){
        unsortedCount+=1; // add
        if (unsortedCount+1>vector<HT_TYPE>::size())
            vector<HT_TYPE>::resize(unsortedCount+1);
        (*this)[unsortedCount]=value;
        // bubble up
        int index=unsortedCount;
            //this->dump(); cout<<endl;
        while (index>0){
            int parent=findParent(index);
            if ((*this)[parent]<(*this)[index]){
                HeapTreeType<HT_TYPE>::swap(parent, index);
                index=parent;
            //this->dump(); cout<<endl;
            } else
                break;
        }
            //this->dump(); cout<<endl;
    }

    /** Completes a cell, which instructs the layer and location within the layer of the HT_TYPE element at index.
    \param index [in] The HT_TYPE element vector index to find the HeapTreeCell representation of.
    \param cell [out] The resulting 'tree co-ordinates' of the element at vectorial index.
    */
    void findCell(unsigned int index, HeapTreeCell &cell){
        findCell(&(*this)[index], cell);
    }

    /** Given an element, find the 'tree co-ordinate' location and store in cell
    \param[in] index The element to located the tree co-ordinates of
    \param[out] cell The resulting 'tree co-ordinates' of the element at vectorial index.
    */
    void findCell(HT_TYPE* index, HeapTreeCell &cell){
        HT_TYPE* root=&(*this)[0];
        //cout<<"index="<<index<<'\t'<<"root="<<root<<"\tsizeof(HT_TYPE)="<<sizeof(HT_TYPE)<<"index-root"<<index-root<<endl;
        int location = (int)(index-root); // no need to divide by sizeof here
        cell.level=0;
//        cout<<"pow = "<<base<<"^"<<cell.level<<" = "<<pow(base,(float)cell.level)<<endl;
        while ((location-=(int)pow(base,(float)cell.level))>=0)
            cell.level++;
        location+=(int)pow(base,(float)cell.level);
        cell.child=location;
        return;
    }

    /** Given 'tree co-ordinates', return the vector index.
    \param cell Tree co-ordinates. The level and level index of the tree element.
    \return The index in the vector matching the tree co-ordinates.
    */
    unsigned int findIndex(HeapTreeCell &cell){
        if (!cell.level) return 0;
        return (unsigned int)(pow(base,(float)cell.level)+cell.child-base+1.0);
    }

    /** Given a child's index, return the parent's index.
    \param index The child's index
    \return The parent's index
    */
    unsigned int findParent(unsigned int index){
        if (!index){
            cerr<<"error: the root parent has no parent"<<endl;
            exit(-1);
        }
        HeapTreeCell child, parent;
        findCell(index, child); // find the cell of the child
        parent.level=child.level-1;
        parent.child=(unsigned int)floor((float)child.child/base);
        return findIndex(parent);
    }

    /** Given a parent's vector index, return the children't 'tree co-ordinatees'
    \param parent The parent's vector index.
    \param childL The left child's level and level index.
    \param childR The right child's level and level index.
    */
    void findChildren(unsigned int parent, HeapTreeCell& childL, HeapTreeCell& childR){
//        cout<<"findChildren"<<parent<<endl;
        HeapTreeCell cell;
        findCell(parent, cell);
        childL.level=cell.level+1; childR.level=cell.level+1;
        childL.child=cell.child*2; childR.child=cell.child*2+1;
    }

//    void parent(HT_TYPE* index, HeapTreeCell& cell){
//        HT_TYPE* root=&(*this)[0];
//        //cout<<"index="<<index<<'\t'<<"root="<<root<<"\tsizeof(HT_TYPE)="<<sizeof(HT_TYPE)<<"index-root"<<index-root<<endl;
//        unsigned int location = (int)(index-root); // no need to divide by sizeof here
//    }

    /** Run through the vector and output (to cout) the HT_TYPE tree element.
    */
    void dump(){
        for (int i=0;i<vector<HT_TYPE>::size();i++)
            cout<<(*this)[i]<<'\t';
    }

    /** Run through the vector and output (to cout) the HT_TYPE tree element.
    */
    void dumpDereference(){
        for (int i=0;i<vector<HT_TYPE>::size();i++)
            cout<<*((*this)[i])<<'\t';
    }

    /** Given a cell, dump to cout the children.
    */
    void dumpLPR(HeapTreeCell& cell){
        HeapTreeCell childL, childR;
        unsigned int myIndex=findIndex(cell);
        findChildren(myIndex, childL, childR);
        if (findIndex(childL)<=unsortedCount)
            dumpLPR(childL);
        cout<<(*this)[myIndex]<<'\t';
        if (findIndex(childR)<=unsortedCount)
            dumpLPR(childR);
    }

    /** Sort a HeapTreeType using the sort algorithm.
    */
    void sort(void){
                // store first and bubble up
        unsigned int unsortedCountOrig=unsortedCount;
        while (unsortedCount>0){
            unsigned int index=0;
            swap(index,unsortedCount);
            swapIfBigger(index);
            unsortedCount-=1;
            //this->dump();cout<<"\tsize="<<unsortedCount<<endl;
        }
    }

    /** Calls the vector<HT_TYPE>::resize method.
    */
    void resize(size_t s, HT_TYPE c=HT_TYPE()){
        vector<HT_TYPE>::resize(s,c);
    }

    /** Delete reset the unsortedCount
    */
    virtual void deleteElements(void){
        this->resize(0);
        unsortedCount=-1;
    }


////    /** Return the variable in the vector.
////    NOTE: Do not use this to add new variables to the HeapTreeType. Use the 'add' method to add new variables, this will res
////    */
//    HeapTreeType &operator<<=(HT_TYPE var){
////        if (unsortedCount+1!=i){
////            cerr<<"ERROR: Current add location = "<<unsortedCount+t<<" you specified "<<i<<endl;
////            assert(-1);
////        }
//        add(var);
//        return *this;
//    }
//

//    HT_TYPE operator[](int i){
//        return vector<HT_TYPE>::operator[](i);
//    }
//
//    HT_TYPE operator[](int i){
//        if (i>unsortedCount){
//            cerr<<"ERROR: Current tree unsortedCount = "<<unsortedCount<<" you requested index = "<<i<<endl;
//            assert(-1);
//        }
//        return (*this)[i];
//    }
};
#endif //HEAP_TREE_TYPE_H_
