// Signature must be retained. Implementation must be added.

#ifndef SEARCH_H
#define SEARCH_H

#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"
#include "iostream"

// returns FIRST array location matching the given item (based on the comparator)
template <typename T>
long long binarySearch(const T& item, const TemplatedArray<T>& array, const Comparator<T>& comparator) {
  int low = 0;                                                                  //left side of the array
  int mid = 0;                                                                  //midpoint
  int high = static_cast<int>(array.getSize())-1;                               //right side of the array
  T item2;                                                                      //item to compare
  int c;                                                                        //result of comparator

  while(high >= low)
  {
    mid = (high + low) / 2;                                                     //calculate midpoint
    item2 = array[mid];                                                         //current item being compared
    c = comparator.compare(item, item2);                                        //comparison

    if (c == 1)                                                                 //item > item2
    {
        low = mid + 1;                                                          //look at the upper half
     }
     else if (c == -1)                                                          //item < item2
     {
        high = mid - 1;                                                         //look at the lower half
     }
     else if (c == 0)                                                           //item = item2
     {
        while(c == 0 && mid > 0)                                                //loop to ensure return of first instance
        {
          mid--;                                                                //step back
          item2 = array[mid];
          c = comparator.compare(item, item2);                                  //comparison
          if(c != 0)
          {
            mid++;                                                              //reverse last step when first non-match is found
          }
        }
        return mid;                                                             //return first instance of match
     }
  }
  return -1;                                                                    //no match found
}

#endif
