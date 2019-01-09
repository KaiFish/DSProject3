#ifndef HASH_TABLE_ENUMERATOR
#define HASH_TABLE_ENUMERATOR

#include "Enumerator.h"
#include "Exceptions.h"
#include "HashTable.h"

template <typename T>
class HashTableEnumerator : public Enumerator<T>
{
private:
    unsigned long bucket = 0;                                // the current bucket during the enumeration process
    OULinkedListEnumerator<T>* chainEnumerator = NULL;        // used to move through the linked list of the current bucket
    HashTable<T>* hashTable = NULL;                            // pointer to the HashTable being enumerated
    
    unsigned long numBuckets = 0;
    
    bool checkNext(unsigned long b) const;
    bool advanceNext(unsigned long b);
    T peekNext(unsigned long b) const;
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
    
    if(this->hashTable->getSize() == 0)
    {
        throw new ExceptionHashTableAccess();
    }
    
    this->numBuckets = this->hashTable->baseCapacity;
    this->chainEnumerator = this->hashTable->table[this->bucket]->eNum();
    if(!(this->chainEnumerator->hasNext()))
    {
        this->advanceNext(this->bucket + 1);
    }
}

template <typename T> HashTableEnumerator<T>::~HashTableEnumerator()
{
    bucket = 0;
}

template <typename T> bool HashTableEnumerator<T>::checkNext(unsigned long b) const
{
    if(b == this->numBuckets)
    {
        return false;
    }
    
    OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
    
    if(!e.hasNext())
    {
        return this->checkNext(b+1);
    }
    else
    {
        return true;
    }
    
    return false;
}

template <typename T> bool HashTableEnumerator<T>::advanceNext(unsigned long b)
{
    if(this->chainEnumerator->hasNext())
    {
        return true;
    }
    
    if(b == this->numBuckets)
    {
        return false;
    }
    
    OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
    
    if(!e.hasNext())
    {
        return this->advanceNext(b+1);
        
    }
    else
    {
        this->bucket = b;
        this->chainEnumerator = this->hashTable->table[b]->eNum();
        return true;
    }
    
    return false;
}

template <typename T> T HashTableEnumerator<T>::peekNext(unsigned long b) const
{
    if(b == this->numBuckets)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    
    OULinkedListEnumerator<T> e = this->hashTable->table[b]->enumerator();
    
    if(!e.hasNext())
    {
        return this->peekNext(b+1);
        
    }
    else
    {
        return e.peek();
    }
}


template <typename T> bool HashTableEnumerator<T>::hasNext() const
{
    if(this->chainEnumerator->hasNext() || this->checkNext(this->bucket + 1))
    {
        return true;
    }
    return false;
}

template <typename T> T HashTableEnumerator<T>::next()
{
    if(this->chainEnumerator->hasNext())
    {
        return this->chainEnumerator->next();
    }
    
    if(this->checkNext(this->bucket + 1))
    {
        bool move = this->advanceNext(this->bucket + 1);
        if(!move)
        {
            throw new ExceptionEnumerationBeyondEnd();
        }
    }
    else
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    return this->chainEnumerator->next();
}

template <typename T> T HashTableEnumerator<T>::peek() const
{
    if(this->chainEnumerator->hasNext())
    {
        return this->chainEnumerator->peek();
    }
    return this->peekNext(this->bucket + 1);
}

#endif // !HASH_TABLE_ENUMERATOR
