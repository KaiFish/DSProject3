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
    
    unsigned long numBuckets = 0;
    void go2First();
    // you may add additional member variables and functions here to support the operation of your code
public:
    HashTableEnumerator(HashTable<T>* hashTable);            // constructor needs a pointer to the HashTable to be enumerated
    virtual ~HashTableEnumerator();
    bool hasNext();                                    // true if there are elements that have not yet been returned via next()
    T next();                                                // throws ExceptionEnumerationBeyondEnd if no next item is available
    T peek();                                            // throws ExceptionEnumerationBeyondEnd if no next item is available
};

// put implementation for HashTableEnumerator here

template <typename T> HashTableEnumerator<T>::HashTableEnumerator(HashTable<T>* hashTable)
{
    this->hashTable = hashTable;
    this->numBuckets = this->hashTable->baseCapacity;
    OULinkedListEnumerator<T> e = this->hashTable->table[this->bucket]->enumerator();
    this->chainEnumerator = &e;
    
    if(this->hashTable->getSize() == 0)
    {
        throw new ExceptionHashTableAccess();
    }
    
    //this->go2First();
}

template <typename T> HashTableEnumerator<T>::~HashTableEnumerator()
{
    bucket = 0;
}

template <typename T> void HashTableEnumerator<T>::go2First()
{
    OULinkedListEnumerator<T> e = this->hashTable->table[this->bucket]->enumerator();
    if(e.currentData() == nullptr)
    {
        this->bucket++;
        OULinkedListEnumerator<T> e = this->hashTable->table[this->bucket]->enumerator();
        while(e.currentData() == nullptr)
        {
            this->bucket++;
            e = this->hashTable->table[this->bucket]->enumerator();
        }
        this->chainEnumerator = &e;
    }
    
    if(this->chainEnumerator->currentData() == nullptr)
    {
        throw ExceptionHashTableAccess();
    }
}

template <typename T> bool HashTableEnumerator<T>::hasNext()
{
    bool temp = this->chainEnumerator->hasNext();
    if(!temp)
    {
        unsigned long b = this->bucket;
        while(!temp && b < this->numBuckets)
        {
            b++;
            OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
            temp = e.hasNext();
        }
        if(!temp)
        {
            return false;
        }
    }
    return true;
}

template <typename T> T HashTableEnumerator<T>::next()
{
    T* temp = this->chainEnumerator->currentData();
    if(temp == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    else
    {
        if(this->chainEnumerator->hasNext())
        {
            this->chainEnumerator->next();
        }
        else
        {
            unsigned long b = this->bucket + 1;
            OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
            while(e.currentData() == nullptr && b < this->numBuckets)
            {
                OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
                b++;
                
            }
            if(b > this->numBuckets)
            {
                throw new ExceptionEnumerationBeyondEnd();
            }
            else
            {
                this->chainEnumerator = &e;
            }
        }
    }
    return *temp;
}

template <typename T> T HashTableEnumerator<T>::peek()
{
    T* temp = this->chainEnumerator->currentData();
    if(temp == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    return *temp;
}

#endif // !HASH_TABLE_ENUMERATOR


//if(hasNext())
//{
//    try
//    {
//        this->chainEnumerator->next();
//    }
//    catch(ExceptionEnumerationBeyondEnd*)
//    {
//        bool nextHas = true;
//        unsigned long b = this->bucket;
//        while(!nextHas && b < this->numBuckets)
//        {
//            b++;
//            OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
//            nextHas = e.hasNext();
//        }
//        if(!nextHas)
//        {
//            throw new ExceptionEnumerationBeyondEnd();
//        }
//        else
//        {
//            OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
//            this->chainEnumerator = &e;
//        }
//    }
//}
//else
//{
//    throw new ExceptionEnumerationBeyondEnd();
//    }
