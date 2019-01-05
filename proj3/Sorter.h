// Signature must be retained. Implementation must be added.
#ifndef SORT_H
#define SORT_H

#include "Exceptions.h"
#include "TemplatedArray.h"
#include "Comparator.h"
#include "iostream"


template <typename T>
class Sorter {
// private:
public:
	static void swap(TemplatedArray<T>& a, int x, int y);																			//swap objects at a[x] and a[y]
	static int partition(TemplatedArray<T>& a, const Comparator<T>& c, int low, int high);		//partition each section of array
	static void quickSort(TemplatedArray<T>& a, const Comparator<T>& c, int low, int high);		//recursive call to sort
// public:
	static void sort(TemplatedArray<T>& array, const Comparator<T>& comparator);							//public call for quickSort
};

template <typename T> void Sorter<T>::swap(TemplatedArray<T>& a, int x, int y)	//swap objects at a[x] and a[y]
{
  T t = a.get(static_cast<unsigned long>(x));																		//pointers are stupid, this is the only way I could get them to work
	T b = a.get(static_cast<unsigned long>(y));
  a.replaceAt(&b, static_cast<unsigned long>(x));
  a.replaceAt(&t, static_cast<unsigned long>(y));
  return;
}


template <typename T> int Sorter<T>::partition(TemplatedArray<T>& a, const Comparator<T>& c, int low, int high)
{
	int midpoint = (high-low)/2 +low;																							//calculate midpoint
  T pivot = a[midpoint];    																										//pivot at midpoint
  int i = low;
  int j = high;
  bool done = false;
  while(!done)
  {
    while(c.compare(a[i], pivot) == -1)																					// a[i] < pivot
    {
      ++i;
    }
    while(c.compare(a[j], pivot) == 1)																					//a[j] > pivot
    {
      --j;
    }
    if(i>=j)
    {
      done = true;
    }
    else
    {
      swap(a, i, j);																														//swap a[i] and a[j]
      ++i;
      --j;
    }
  }
  return j;
}


template <typename T> void Sorter<T>::quickSort(TemplatedArray<T>& a, const Comparator<T>& c, int low, int high)
{
  int j = 0;
  if(low >= high)																																//sorting done
  {
    return;
  }
  j = partition(a, c, low, high);																								//partition the array

  quickSort(a, c, low, j);																											//sort lower half
  quickSort(a, c, j+1, high);																										//sort upper half
}


template <typename T> void Sorter<T>::sort(TemplatedArray<T>& array, const Comparator<T>& comparator)
{
	int high = static_cast<int>(array.getSize() - 1);															//upper bound
	int low = 0;																																	//lower bound
	quickSort(array, comparator, low, high);																			//initial QuickSort call
	return;
}

#endif
