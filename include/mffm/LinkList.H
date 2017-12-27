/* Copyright 1998-2018 Matt Flax <flatmax@flatmax.org>
   This file is now part of MFFM VectorBass.

   MFFM VectorBass is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   MFFM VectorBass is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with MFFM VectorBass
 */

#ifndef LINKLIST_H
#define LINKLIST_H

#include <sys/types.h>
#include <iostream>

//#define LINKLIST_DEBUG

/** \brief Defines a lug in a linked list
The concept is simple, this Lug points to a next or previous Lug. You can ask for the next or prev.
If the constructor is called with the old lug, it is linked as the prvious.

\tparam TYPE is the class or vairable type maintained by the linked list.
*/
template <class TYPE>
class Lug {

protected:

public:

    /// Links to the next and prev elements in the list
    Lug<TYPE> * next, * prev;

    TYPE ptr; // The element to point to

    /// This is for the first element - links to itself
    Lug(void) {
        next = this;
        prev = this;
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList: Adding first "<<this<<" with previous "<< prev << " next " << next <<std::endl;
#endif
    }
    /// Links to previous element and next element
    Lug(Lug<TYPE> * oldOne) {
        next = (*oldOne).getNext();
        prev = oldOne;
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList: Adding "<<this<<" with previous "<< prev << " next " << next <<std::endl;
#endif
        (*oldOne).getNext()->prev = this;
        (*oldOne).next = this;
    }

    //  ~Lug(void){}

    /// Return a pointer to the next element
    Lug *
    getNext(void) {
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList: getNext =  "<< next <<std::endl;
#endif
        return next;
    }

    /// Return a pointer to the previous element
    Lug *
    getPrev(void) {
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList: getPrev =  "<< prev <<std::endl;
#endif
        return prev;
    }
};

template <class TYPE>
class LinkList {

    Lug<TYPE>* lug; ///< The current Lug we are pointing to
    Lug<TYPE>* startLug; ///< The first Lug in the looped list
    long count; ///< The number in the list
public:
    // Direction ...
    typedef enum {FWD, REV} direction; ///< The possible directions we are travelling when next and prev are called.
    direction dir; ///< The current direction

    /** \brief Initialiser
    This is for the first element - links to itself
    */
    LinkList(void) {
        lug = (Lug<TYPE>*)NULL;
        dir = FWD;
        count =0;
    }

    /// Small cleanup ...
    virtual ~LinkList(void) {
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList:~LinkList  removing "<<std::endl;
#endif

        while (lug != (Lug<TYPE>*)NULL)
            remove();
    }


    /** \brief Adds as the current element ...
    Added to the next of the current lug.
    \param newElement The element to add
    */
    void add(TYPE newElement) { // Add the ptr
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList:add "<< newElement <<std::endl;
#endif
        if (lug == (Lug<TYPE>*)NULL) { // create the first ...
            lug = new Lug<TYPE>;
            startLug = lug;
        } else // Add infront of the current lug
            lug = new Lug<TYPE>(lug);
        lug->ptr = newElement;
        count++;
    }

    /** \brief exchange lugs
    The current lug is replaced with this lug.
    \param changeElement The element to swap in
    \return the element swapped out.
    */
    TYPE change(TYPE changeElement) { // exchange ptr's
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList:change "<< changeElement <<std::endl;
#endif
        if (lug != (Lug<TYPE>*)NULL) {
            TYPE tempPtr = lug->ptr;
            lug->ptr = changeElement;
            return tempPtr;
        } else
            return (TYPE)NULL;
    }

    /** \brief remove the current lug from the LinkList
    The current lug is removed form the list and passed back to the caller.
    \return The removed lug.
    */
    TYPE remove(void) { // Remove the current lug and return the ptr
        if (lug != (Lug<TYPE>*)NULL) {
            if (lug == startLug)
                startLug = lug->getNext();
            Lug<TYPE>* tempLug = lug;
            TYPE tempPtr = lug->ptr;

            if (lug->getNext() == lug) { // Only one lug exists !
                lug = (Lug<TYPE>*)NULL;
            } else {
                lug->getPrev()->next = lug->getNext();
                lug->getNext()->prev = lug->getPrev();
                if (dir == FWD)
                    lug = lug->getNext();
                else
                    lug = lug->getPrev();
            }
            count--;
            delete tempLug;
            return tempPtr;
        } else {
            return (TYPE)0;//NULL;
        }
    }

    /* BAD IDEA ! - what if the lug->ptr is not a ptr !
    void delAll(void){
      while (getCount())
        del();
    }

    void del(void){ // Delete the current lug
      if (lug != (Lug<TYPE>*)NULL){
        if (lug == startLug)
    startLug = lug->getNext();
        Lug<TYPE>* tempLug = lug;
        TYPE tempPtr = lug->ptr;

        if (lug->getNext() == lug){ // Only one lug exists !
    lug = (Lug<TYPE>*)NULL;
        } else {
    lug->getPrev()->next = lug->getNext();
    lug->getNext()->prev = lug->getPrev();
    if (dir == FWD)
      lug = lug->getNext();
    else
      lug = lug->getPrev();
        }
        count--;
        delete tempLug;
        delete tempPtr;
        //      return tempPtr;
      }
    }
    */

    /** \brief return a pointer to the current lug
    \return the current lug
    */
    TYPE current(void) { // Return the current ptr
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList:current " <<std::endl;
#endif
        if (lug != (Lug<TYPE>*)NULL)
            return lug->ptr;
        else
            return (TYPE)NULL;
    }

    /** \brief Return a pointer to the next lug and move on to that lug
    Increments according to dir (either forward or backwards).
    \return The next lug in the chain
    */
    TYPE next(void) { // return the next ptr
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList:next "<<std::endl;
#endif
        if (lug != (Lug<TYPE>*)NULL) {
            if (dir == FWD)
                lug = lug->getNext();
            else
                lug = lug->getPrev();
            return lug->ptr;
        } else
            return (TYPE)0;//NULL;
    }

    /** \brief Return a pointer to the previous lug and move to that lug
    Increments against dir (either forward or backwards).
    \return The prev lug in the chain
    */
    TYPE prev(void) { // return the previous ptr
        if (lug != (Lug<TYPE>*)NULL) {
            if (dir == FWD)
                lug = lug->getPrev();
            else
                lug = lug->getNext();
            return lug->ptr;
        } else
            return (TYPE)0;//NULL;
    }

    /** \brief returns the i'th lug in the chain irrespective of direction
    \param i The lug to extract
    \return The lug at index i OR NULL if it doesn't exist
    */
    TYPE grab(int i) { // return the i'th lug in the chain
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList:grab "<< i <<std::endl;
#endif
        if (i<=0) {
            std::cerr<<"LinkList: grab: index is less then or equal to zero"<<std::endl;
            return (TYPE)0;// NULL;
        } else if (i>count) {
            std::cerr<<"LinkList: grab: index exceeds count"<<std::endl;
            return (TYPE)0;// NULL;
        } else {
            lug = startLug;

            while (--i)
                lug = lug->getNext();
            return lug->ptr;
        }
    }

    int getCount(void) {
#ifdef LINKLIST_DEBUG
        std::cout <<"LinkList:getCount "<< count <<std::endl;
#endif
        return count;    ///< Returns the number of Lug s in the chain
    }

    /** \brief Runs the ostream operator on each element of the list
    \param o the ostream
    \param l A pointer to the linked list
    \return The ostream
    */
    friend std::ostream& operator <<(std::ostream& o, LinkList* l) {
        if (l->getCount()==0) return o;
        l->grab(1);
        l->prev();
        for (int j=0; j<l->getCount(); j++)
            o<<l->next()<<' ';
        o<<std::endl;
        return o;
    }
    /** \brief Runs the ostream operator on each element of the list
      \param o the ostream
      \param l The linked list
      \return The ostream
      */
    friend std::ostream& operator <<(std::ostream& o, LinkList& l) {
        if (l.getCount()==0) return o;
        l.grab(1);
        l.prev();
        for (int j=0; j<l.getCount(); j++)
            o<<l.next()<<' ';
        o<<std::endl;
        return o;
    }

    /** Method to get the last element in the list.
    \return The lug at index i OR NULL if it doesn't exist
    */
    TYPE getEnd(){
        grab(1); // get the head.
        return prev(); // get the end element
    }
};
#endif // LINKLIST_H
