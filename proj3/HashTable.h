#ifndef HASH_TABLE
#define HASH_TABLE

#include "Exceptions.h"
#include "Comparator.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"
#include "Hasher.h"
#include <iostream>

const unsigned int SCHEDULE_SIZE = 25;            // the number of items in the size schedule
const unsigned int SCHEDULE[SCHEDULE_SIZE] = { 1, 2, 5, 11, 23, 53, 107, 223, 449, 907, 1823, 3659, 7309, 14621, 29243, 58511, 117023, 234067, 468157, 936319, 1872667, 3745283, 7490573, 14981147, 29962343 };        // the size schedule (all primes)
const unsigned int DEFAULT_SCHEDULE_INDEX = 3;    // the default position in the size schedule
const unsigned long DEFAULT_BASE_CAPACITY = SCHEDULE[DEFAULT_SCHEDULE_INDEX];     // the default size of the array
const float DEFAULT_MAX_LOAD_FACTOR = 0.9f;        // the default load factor used to determine when to increase the table size
const float DEFAULT_MIN_LOAD_FACTOR = 0.4f;        // the default load factor used to determine when to decrease the table size

template <typename T>
class HashTable {
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const HashTable<U>& ht);
    template <typename R>
    friend class HashTableEnumerator;                        // necessary to allow the enumerator to access the table's private data

private:
    Comparator<T>* comparator = NULL;                        // used to determine item equality
    Hasher<T>* hasher = NULL;                                // used to compute hash value
    unsigned long size = 0;                                    // actual number of items currently in hash table
    float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR;            // the load factor used to determine when to increase the table size
    float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR;            // the load factor used to determine when to decrease the table size
    unsigned int scheduleIndex = DEFAULT_SCHEDULE_INDEX;    // the index of current location in the size schedule
    unsigned long baseCapacity = DEFAULT_BASE_CAPACITY;        // the size of the array
    unsigned long totalCapacity = baseCapacity;                // the size of the array plus chains of more than one link
    OULinkedList<T>** table = NULL;                            // table will be an array of pointers to OULinkedLists of type T
    unsigned int capacity();
    OULinkedList<T>** makeTable();
    void add(const T* item, OULinkedList<T>** t);
    bool has(const T* item);
    unsigned long encode(const T* item) const;
    bool resize();
    void rehash();
    // you may add additional member variables and functions here to support the operation of your code
public:
    HashTable(Comparator<T>* comparator, Hasher<T>* hasher);            // creates an empty table of DEFAULT_BASE_CAPACITY
    HashTable(Comparator<T>* comparator, Hasher<T>* hasher,
              // if size given, creates empty table with size from schedule of sufficient capacity (considering maxLoadFactor)
              unsigned long size,
              float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR,
              float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR);
    virtual ~HashTable();
    
    // if an equivalent item is not already present, insert item at proper location and return true
    // if an equivalent item is already present, leave table unchanged and return false
    bool insert(const T* item);
    
    // if an equivalent item is already present, replace item and return true
    // if an equivalent item is not already present, leave table unchanged and return false
    bool replace(const T* item);
    
    // if an equivalent item is already present, remove item and return true
    // if an equivalent item is not already present, leave table unchanged and return false
    bool remove(const T* item);
    
    // if an equivalent item is present, return a copy of the item
    // if an equivalent item is not present, throw a new ExceptionHashTableAccess
    T find(const T* item) const;
    
    unsigned long getSize() const;                        // returns the current number of items in the table
    unsigned long getBaseCapacity() const;                // returns the current base capacity of the table
    unsigned long getTotalCapacity() const;                // returns the current total capacity of the table
    float getLoadFactor() const;                        // returns the current load factor of the table
    unsigned long getBucketNumber(const T* item) const;    // returns the bucket number for an item using its hash function mod array size
    
};

template <typename T> std::ostream& operator<<(std::ostream& os, const HashTable<T>& ht)
{
    unsigned long i = 0;
    
    while(i<ht.baseCapacity)
    {
        OULinkedList<T>* temp = ht.table[i];
        if(temp->first == nullptr)
        {
            i++;
        }
        else if(temp->first == temp->last)
        {
            os << i << ": " << temp->get() << std::endl << '\n';
        }
        else
        {
            OULinkedListEnumerator<T> e = temp->enumerator();
            os << i << ": " << temp->get() << std::endl << '\n';
            e.next();
            while(e.hasNext())
            {
                T item = e.currentData();
                os << "OVERFLOW: " << item << std::endl << '\n';
                e.next();
            }
            T item = e.currentData();
            os << "OVERFLOW: " << item << std::endl << '\n';
            i++;
        }
    }
    return os;
}

template <typename T> HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher)
{
    this->comparator = comparator;
    this->hasher = hasher;
    this->table = this->makeTable();
}

template <typename T> HashTable<T>::HashTable(Comparator<T>* comparator, Hasher<T>* hasher, unsigned long size,
    float maxLoadFactor, float minLoadFactor)
{
    this->comparator = comparator;
    this->hasher = hasher;
    this->table = this->makeTable();
    this->size = size;
    this->maxLoadFactor = maxLoadFactor;
    this->minLoadFactor = minLoadFactor;
}

template <typename T> HashTable<T>::~HashTable()
{
    delete[] this->table;
}

template <typename T> unsigned int HashTable<T>::capacity()
{
    unsigned int cap;
    if(this->size==0)
    {
        cap = static_cast<unsigned int>(DEFAULT_BASE_CAPACITY);
    }
    else
    {
        unsigned int i = 0;
        while(i<SCHEDULE_SIZE)
        {
            if (this->size>=SCHEDULE[i])
            {
                i++;
            }
            else
            {
                this->scheduleIndex = i;
                break;
            }
        }
        cap = SCHEDULE[this->scheduleIndex];
    }
    return cap;
}

template <typename T> OULinkedList<T>** HashTable<T>::makeTable()
{
    this->baseCapacity = this->capacity();
    OULinkedList<T>** arr = new OULinkedList<T>*[this->baseCapacity];
    unsigned long i = 0;
    while(i<this->baseCapacity)
    {
        OULinkedList<T>* neu = new OULinkedList<T>(this->comparator);
        arr[i] = neu;
        i++;
    }
    return arr;
}

template <typename T> void HashTable<T>::add(const T* item, OULinkedList<T>** t)
{
    unsigned long index = this->encode(item);
    OULinkedList<T>* temp = t[index];
    temp->insert(item);
    return;
}

template <typename T> bool HashTable<T>::has(const T* item)
{
    bool found = true;
    if (this->size < 1)
    {
        found = false;
    }
    else
    {
        try
        {
            find(item);
        }
        catch (ExceptionHashTableAccess& e)
        {
            found = false;
        }
    }
    return found;
}

template <typename T> unsigned long HashTable<T>::encode(const T* item) const
{
    T t = *item;
    unsigned long key = this->hasher->hash(t);
    unsigned long mod = this->baseCapacity;
    unsigned long index = key%mod;
    return index;
}

template <typename T> bool HashTable<T>::resize()
{
    float loadFactor = this->getLoadFactor();
    if(loadFactor >= this->maxLoadFactor || loadFactor <= minLoadFactor)
    {
        return true;
    }
    return false;
}

template <typename T> void HashTable<T>::rehash()
{
    OULinkedList<T>** old = this->table;
    unsigned long oldCap = this->baseCapacity;
    OULinkedList<T>** neu = makeTable();
    unsigned long i = 0;
    
    while(i<oldCap)
    {
        OULinkedList<T>* temp = old[i];
        T first = temp->get();
        T* firstPtr = &first;
        if(firstPtr == nullptr)
        {
            i++;
        }
        else
        {
            OULinkedListEnumerator<T> e = temp->enumerator();
            while(e.hasNext())
            {
                T item = e.currentData();
                T* ptr = &item;
                this->add(ptr, neu);
                e.next();
            }
            T item = e.currentData();
            T* ptr = &item;
            this->add(ptr, neu);
            i++;
        }
    }
    this->table = neu;
}

template <typename T> bool HashTable<T>::insert(const T* item)
{
    if(this->has(item))
    {
        return false;
    }
    if(this->resize())
    {
        this->rehash();
    }
    this->add(item, this->table);
    this->size++;
    return true;
}

template <typename T> bool HashTable<T>::replace(const T* item)
{
    if(!this->has(item))
    {
        return false;
    }
    unsigned long index = this->encode(item);
    OULinkedList<T>* temp = this->table[index];
    temp->replace(item);
    return true;
}

template <typename T> bool HashTable<T>::remove(const T* item)
{
    if(!this->has(item))
    {
        return false;
    }
    unsigned long index = this->encode(item);
    OULinkedList<T>* temp = this->table[index];
    temp->remove(item);
    this->size--;
    if(this->resize())
    {
        this->rehash();
    }
    return true;
}

template <typename T> T HashTable<T>::find(const T* item) const
{
    unsigned long index = this->encode(item);
    OULinkedList<T>* temp = this->table[index];
    T t = T();
    try
    {
        t = temp->find(item);
    }
    catch (ExceptionLinkedListAccess& e)
    {
        throw ExceptionHashTableAccess();
    }
    return t;
}

template <typename T> unsigned long HashTable<T>::getSize() const
{
    return this->size;
}

template <typename T> unsigned long HashTable<T>::getBaseCapacity() const
{
    return this->baseCapacity;
}

template <typename T> unsigned long HashTable<T>::getTotalCapacity() const
{
    return this->totalCapacity;
}

template <typename T> float HashTable<T>::getLoadFactor() const
{
    float loadFactor = static_cast<float>(this->size/this->baseCapacity);
    return loadFactor;
}

template <typename T> unsigned long HashTable<T>::getBucketNumber(const T* item) const
{
    return this->encode(item);
}

#endif // !HASH_TABLE
