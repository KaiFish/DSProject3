#ifndef HASH_TABLE_ENUMERATOR
#define HASH_TABLE_ENUMERATOR

#include "Enumerator.h"
#include "Exceptions.h"
#include "HashTable.h"
#include "TemplatedArray.h"

template <typename T>
class HashTableEnumerator : public Enumerator<T>
{
private:
    unsigned long bucket = 0;                                // the current bucket during the enumeration process
    OULinkedListEnumerator<T>* chainEnumerator = NULL;        // used to move through the linked list of the current bucket
    HashTable<T>* hashTable = NULL;                            // pointer to the HashTable being enumerated
    void* ptr = nullptr;
    
    unsigned long numBuckets = 0;
    unsigned long index = 0;
    void fill();
    bool checkBucket();
    bool advanceBucket();
    // you may add additional member variables and functions here to support the operation of your code
public:
    HashTableEnumerator(HashTable<T>* hashTable);            // constructor needs a pointer to the HashTable to be enumerated
    virtual ~HashTableEnumerator();
    bool hasNext() const;                                    // true if there are elements that have not yet been returned via next()
    T next();                                                // throws ExceptionEnumerationBeyondEnd if no next item is available
    T peek() const;                                            // throws ExceptionEnumerationBeyondEnd if no next item is available
};

// put implementation for HashTableEnumerator here

template <typename T> HashTableEnumerator<T>::HashTableEnumerator(HashTable<T>* hashTable)
{
    this->hashTable = hashTable;
    this->numBuckets = this->hashTable->baseCapacity;
    OULinkedListEnumerator<T> e = this->hashTable->table[this->bucket]->enumerator();
    this->chainEnumerator = &e;
    fill();
    this->index = 0;
    if(sizeof(this->ptr) == 0)
    {
        throw new ExceptionHashTableAccess();
    }
}

template <typename T> HashTableEnumerator<T>::~HashTableEnumerator()
{
    bucket = 0;
}

template <typename T> void HashTableEnumerator<T>::fill()
{
    unsigned long size = this->hashTable->size;
    T* arr = new T[size];
    bool notEnd = true;
    while(checkBucket() && notEnd)
    {
        arr[this->index] = *(this->chainEnumerator->currentData());
        index++;
        if(this->chainEnumerator->hasNext())
        {
            this->chainEnumerator->next();
        }
        else
        {
            notEnd = advanceBucket();
        }
    }
    this->ptr = &arr;
}

template <typename T> bool HashTableEnumerator<T>::checkBucket()
{
    bool notEnd = true;
    while(this->chainEnumerator->currentData() == nullptr && notEnd)
    {
        notEnd = advanceBucket();
    }
    return notEnd;
}

template <typename T> bool HashTableEnumerator<T>::advanceBucket()
{
    this->bucket++;
    if(this->bucket < this->numBuckets)
    {
        OULinkedListEnumerator<T> e = this->hashTable->table[this->bucket]->enumerator();
        this->chainEnumerator = &e;
        return true;
    }
    return false;
}


template <typename T> bool HashTableEnumerator<T>::hasNext() const
{
    T* arr = static_cast<T*>(this->ptr);
    unsigned long nextI = this->index + 1;
    if(arr[this->index] == nullptr || arr[nextI] == nullptr)
    {
        return false;
    }
    return true;
}

template <typename T> T HashTableEnumerator<T>::next()
{
    T* arr = static_cast<T*>(this->ptr);
    if(!hasNext())
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    T temp1 = *arr[this->index];
    T* temp2 = new T(temp1);
    this->index++;
    return *temp2;
}

template <typename T> T HashTableEnumerator<T>::peek() const
{
    T* arr = static_cast<T*>(this->ptr);
    if(!hasNext())
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    T temp1 = *arr[this->index];
    T* temp2 = new T(temp1);
    return *temp2;
}

#endif // !HASH_TABLE_ENUMERATOR
