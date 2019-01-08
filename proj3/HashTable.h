#ifndef HASH_TABLE
#define HASH_TABLE

#include "Exceptions.h"
#include "Comparator.h"
#include "OULinkedList.h"
#include "OULinkedListEnumerator.h"
#include "Hasher.h"
#include <iostream>
#include <chrono>
#include <thread>

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
    Comparator<T>* comparator = nullptr;                        // used to determine item equality
    Hasher<T>* hasher = nullptr;                                // used to compute hash value
    unsigned long size = 0;                                    // actual number of items currently in hash table
    float maxLoadFactor = DEFAULT_MAX_LOAD_FACTOR;            // the load factor used to determine when to increase the table size
    float minLoadFactor = DEFAULT_MIN_LOAD_FACTOR;            // the load factor used to determine when to decrease the table size
    unsigned int scheduleIndex = DEFAULT_SCHEDULE_INDEX;    // the index of current location in the size schedule
    unsigned long baseCapacity = DEFAULT_BASE_CAPACITY;        // the size of the array
    unsigned long totalCapacity = baseCapacity;                // the size of the array plus chains of more than one link
    unsigned long cap = DEFAULT_BASE_CAPACITY;
    OULinkedList<T>** table = nullptr;                            // table will be an array of pointers to OULinkedLists of type T
    void setCapacity(unsigned long neededSize);
    OULinkedList<T>** makeTable();
    void add(const T* item, OULinkedList<T>** t);
    bool has(const T* item);
    unsigned long encode(const T* item) const;
    bool resize(float load);
    void rehash();
    
    std::string format(float num) const;
    std::string getStats() const;

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
        OULinkedListEnumerator<T> e = temp->enumerator();
        T* ptr = e.currentData();
        if(ptr == nullptr)
        {
            i++;
        }
        else if(e.hasNext())
        {
            os << i << ": " << *ptr << std::endl << '\n';       //print first
            e.next();
            while(e.hasNext())
            {
                T item = *e.currentData();
                os << "OVERFLOW: " << item << std::endl;
                e.next();
            }
            T item = *e.currentData();                          //print last
            os << "OVERFLOW: " << item << std::endl << '\n';
            i++;
        }
        else
        {
            os << i << ": " << *ptr << std::endl << '\n';       //print only item
            i++;
        }
    }
    os << ht.getStats();
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
    this->maxLoadFactor = maxLoadFactor;
    this->minLoadFactor = minLoadFactor;
    this->setCapacity(size);
    this->table = this->makeTable();
}

template <typename T> HashTable<T>::~HashTable()
{
    delete[] this->table;
}

template <typename T> void HashTable<T>::setCapacity(unsigned long neededSize)
{
    if(neededSize==0)
    {
        this->scheduleIndex = 0;
        this->baseCapacity = SCHEDULE[this->scheduleIndex];
        this->totalCapacity = this->baseCapacity;
    }
    else
    {
        unsigned int i = 0;
        while(i<SCHEDULE_SIZE)
        {
            if(neededSize>=SCHEDULE[i])
            {
                i++;
            }
            else
            {
                float load = (float)neededSize/(float)SCHEDULE[i];
                while(this->resize(load))
                {
                    i++;
                    load = (float)neededSize/(float)SCHEDULE[i];
                }
                this->scheduleIndex = i;
                break;
            }
        }
        this->baseCapacity = SCHEDULE[this->scheduleIndex];
        this->totalCapacity = this->baseCapacity;
    }
}

template <typename T> OULinkedList<T>** HashTable<T>::makeTable()
{
    OULinkedList<T>** tb = new OULinkedList<T>*[this->baseCapacity];
    unsigned long i = 0;
    
    while(i<this->baseCapacity)
    {
        OULinkedList<T>* neu = new OULinkedList<T>(this->comparator);
        tb[i] = neu;
        i++;
    }
    return tb;
}

template <typename T> void HashTable<T>::add(const T* item, OULinkedList<T>** t)
{
    unsigned long index = this->encode(item);
    OULinkedList<T>* temp = t[index];
    OULinkedListEnumerator<T> e = temp->enumerator();
    T* ptr = e.currentData();
    if(temp->insert(item) && ptr != nullptr)
    {
        this->totalCapacity++;
    }
    return;
}

template <typename T> bool HashTable<T>::has(const T* item)
{
    bool found = false;
    unsigned long index = this->encode(item);
    OULinkedList<T>* temp = this->table[index];
    OULinkedListEnumerator<T> e = temp->enumerator();
    while(e.hasNext())
    {
        T item2 = *e.currentData();
        int c = this->comparator->compare(*item, item2);
        if(c == 0)
        {
            found = true;
            return found;
        }
        e.next();
    }
    T* ptr = e.currentData();
    if(ptr == nullptr)
    {
        return found;
    }
    T item2 = *ptr;                      //check last
    int c = this->comparator->compare(*item, item2);
    if(c == 0)
    {
        found = true;
        return found;
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

template <typename T> bool HashTable<T>::resize(float load)
{
    if(load > this->maxLoadFactor)
    {
        this->scheduleIndex++;
        this->cap = this->baseCapacity;
        this->baseCapacity = SCHEDULE[this->scheduleIndex];
        this->totalCapacity = this->baseCapacity;
        return true;
    }
    else if(load < this->minLoadFactor)
    {
        this->scheduleIndex--;
        this->cap = this->baseCapacity;
        this->baseCapacity = SCHEDULE[this->scheduleIndex];
        this->totalCapacity = this->baseCapacity;
        return true;
    }
    return false;
}

template <typename T> void HashTable<T>::rehash()
{
    OULinkedList<T>** old = this->table;
    unsigned long oldCap = this->cap;
    OULinkedList<T>** neu = makeTable();
    
    unsigned long i = 0;

    while(i<oldCap)
    {
        OULinkedList<T>* temp = old[i];
        OULinkedListEnumerator<T> e = temp->enumerator();
        while(e.hasNext())
        {
            T* item = new T(*e.currentData());
            add(item, neu);
            e.next();
        }
        T* item = e.currentData();
        if(item != nullptr)
        {
            add(item, neu);
        }
        i++;
    }
    this->table = neu;
}

template <typename T> string HashTable<T>::format(float num) const
{
    string str = to_string(num);
    while(str[str.size()-1] == '0')
    {
        str.pop_back();
    }
    return str;
}

template <typename T> string HashTable<T>::getStats() const
{
    
    string str = "Base Capacity: " + to_string(this->baseCapacity) + "; Total Capacity: " + to_string(this->totalCapacity) + "; Load Factor: " + format(getLoadFactor());
    return str;
}

template <typename T> bool HashTable<T>::insert(const T* item)
{
    if(this->has(item))
    {
        return false;
    }
    this->add(item, this->table);
    this->size++;
    if(this->totalCapacity > this->baseCapacity || getLoadFactor() > this->maxLoadFactor)
    {
        if(this->resize(this->getLoadFactor()))
        {
            this->rehash();
        }
    }
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
    if(this->resize(this->getLoadFactor()))
    {
        this->rehash();
    }
    unsigned long index = this->encode(item);
    OULinkedList<T>* temp = this->table[index];
    unsigned long s1 = temp->getSize();
    temp->remove(item);
    if(s1!=1 && temp->getSize()<s1)
    {
        this->totalCapacity--;
    }
    this->size--;
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
    float loadFactor = ((float)this->size / (float)this->totalCapacity);
    return loadFactor;
}

template <typename T> unsigned long HashTable<T>::getBucketNumber(const T* item) const
{
    return this->encode(item);
}


#endif // !HASH_TABLE
