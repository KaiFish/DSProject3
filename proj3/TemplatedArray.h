#ifndef TEMPLATED_ARRAY_H
#define TEMPLATED_ARRAY_H

#include "Exceptions.h"

const unsigned long DEFAULT_ARRAY_CAPACITY = 10;								    //capacity used in no arg constructor
template<typename T> class TemplatedArray {
    friend std::ostream& operator<<(std::ostream& os, const NvraRecord& record);

private:
    unsigned long capacity = DEFAULT_ARRAY_CAPACITY;								//maximum capacity, in records
    unsigned long size = 0;															//actual number of items currently in array
    T* data = nullptr;																//pointer to array of records
    void checkSize();
    void setData(T* newData, unsigned long newSize);
public:
    TemplatedArray();																//constructs array with default capacity
    TemplatedArray(unsigned long capacity);											//constructs array with specified capacity
    virtual ~TemplatedArray();														//frees array space as object is deleted
    void add(T* record);															//adds record, increments size, doubles capacity as necessary
    void addAt(T* record, unsigned long i);
    void replaceAt(T* record, unsigned long i);
    void removeAt(unsigned long i);
    T get(unsigned long i) const;													//returns (copy of) record at index
    unsigned long getCapacity() const;				    							//returns the current capacity of the array
    unsigned long getSize() const;					    							//returns the current number of items in the array
    T operator[](unsigned long index) const;		    							//returns (reference to) item at index
    T* getPtr(unsigned long i) const;
};

template <typename T> std::ostream& operator<<(std::ostream& os, const TemplatedArray<T>& ta)
{
    unsigned long i = 0;
    while(i<ta.getSize())
    {
        os << ta.get(i) << std::endl;
        i++;
    }
    return os;
}

template<class T> void TemplatedArray<T>::checkSize()	//checks if the array needs to be resized and then resizes if necessary
{
    bool resize = false;						//this is not strictly necessary, but added rather than doubling the code in the if(resize) block
    if (size==capacity)							//checks if it needs to double
    {
        capacity = capacity * 2;
        resize = true;
    }
    else if (size==((capacity/2) - 1))		    //checks if it needs to half
    {
        capacity = capacity / 2;
		resize = true;
	}

	if(resize)									//resizes the array
	{
        T* newData = new T[capacity];
		unsigned long i=0;
		while(i < size)
		{
			newData[i] = data[i];
			i++;
		}
		delete[] data;
		data = newData;
	}
}

template<class T> void TemplatedArray<T>::setData(T* newData, unsigned long newSize)
{
	data = newData;
	size = newSize;
}

template<class T> TemplatedArray<T>::TemplatedArray()   //constructs array with default capacity
{
    this->capacity = DEFAULT_ARRAY_CAPACITY;        //set capacity at 10, per the lab specs
    data = new T[this->capacity];            	    //create enough space for 10 records
}

template<class T> TemplatedArray<T>::TemplatedArray(unsigned long capacity)     //constructs array with specified capacity
{
    this->capacity = capacity;             	//set capacity as given by parameter
    data = new T[capacity];            		//create enough space for specified capacity
}

template<class T> TemplatedArray<T>::~TemplatedArray()      //frees array space as object is deleted
{
    delete[] data;
    data = nullptr;
}

template<class T> void TemplatedArray<T>::add(T* record)	//adds record to first free position
{
    checkSize();
    data[size] = *record;
    size++;
    checkSize();
}

template<class T> void TemplatedArray<T>::addAt(T* record, unsigned long i)     //adds record where specified, shifts other data
{
  checkSize();

  if (i > size)
  {
    throw new ExceptionIndexOutOfRange();
  }

  unsigned long j = size;
  while(j >= i)
  {
    data[j + 1] = data[j];
		j--;
  }
  data[i] = *record;
  size++;

	checkSize();
}

template<class T> void TemplatedArray<T>::replaceAt(T* record, unsigned long i)     //replaces record at index specified
{
  if (size == 0 || i > size - 1)
  {
    throw new ExceptionIndexOutOfRange();
  }

  data[i] = *record;
}

template<class T> void TemplatedArray<T>::removeAt(unsigned long i)     //removes record where specified
{
  if (size == 0 || i > size - 1)
  {
    throw new ExceptionIndexOutOfRange();
  }
  unsigned long j = i;
  while(j < size-1)
  {
    data[j] = data[j+1];
		j++;
  }
  size--;
  checkSize();
}

template<class T> T TemplatedArray<T>::get(unsigned long i) const       //returns (copy of) record at index
{
  if(size == 0 || i > size - 1)
  {
    throw new ExceptionIndexOutOfRange();
  }
  return T(data[i]);
}

template<class T> unsigned long TemplatedArray<T>::getCapacity() const      //returns the current capacity of the array
{
	return capacity;
}

template<class T> unsigned long TemplatedArray<T>::getSize() const      //returns the current number of items in the array
{
	return size;
}

template<class T> T TemplatedArray<T>::operator[](unsigned long index) const    //returns (reference to) item at index
{
	if(index >= size)
	{
		throw new ExceptionIndexOutOfRange();
	}
	return data[index];
}

template<class T> T* TemplatedArray<T>::getPtr(unsigned long i) const       //returns pointer to record at index
{
    if(size == 0 || i > size - 1)
    {
        return nullptr;
    }
    return new T(data[i]);
}

#endif
