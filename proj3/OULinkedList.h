#ifndef OU_LINKED_LIST
#define OU_LINKED_LIST

#include "OULink.h"
#include "Comparator.h"
#include "OULinkedListEnumerator.h"
#include "iostream"
#include "Exceptions.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

// OULinkedList stands for Ordered, Unique Linked List. It is a linked list that is always maintained in
// order (based on the comparator provided to it when the list is created) and that only contains unique
// items (that is, duplicates are not allowed)
template <typename T>
class OULinkedList {
    template <typename R>  // not necessary
    //friend class OULinkedListEnumerator;  // not necessary
    friend std::ostream& operator<<(std::ostream& os, const OULinkedList<T>& ll);
private:
    Comparator<T>* comparator = nullptr;                // used to determine list order and item equality
    unsigned long size = 0;                                            // actual number of items currently in list
    OULink<T>* first = nullptr;                                    // pointer to first link in list
    OULink<T>* last = nullptr;                                    // pointer to last link in list
public:
    OULinkedList(Comparator<T>* comparator);        // creates empty linked list with comparator
    virtual ~OULinkedList();                                        // deletes all links and their data items
    
    // if an equivalent item is not already present, insert item in order and return true
    // if an equivalent item is already present, leave list unchanged and return false
    bool insert(const T* item);
    
    // if item is greater than item at last, append item at end and return true
    // if item is less than or equal to item at last, leave list unchanged and return false
    bool append(const T* item);  // now const
    
    // if an equivalent item is already present, replace item and return true
    // if an equivalent item is not already present, leave list unchanged and return false
    bool replace(const T* item);  // now const
    
    // if an equivalent item is already present, remove item and return true
    // if an equivalent item is not already present, leave list unchanged and return false
    bool remove(const T* item);  // now const
    
    // if any items are present, return a copy of the first item
    // if no items are present, throw new ExceptionLinkedListAccess
    T get() const;
    
    // if an equivalent item is present, return a copy of the first such item
    // if an equivalent item is not present, throw a new ExceptionLinkedListAccess
    T find(const T* item) const;
    
    unsigned long getSize() const;                    // returns the current number of items in the list
    
    OULinkedListEnumerator<T> enumerator() const;    // create an enumerator for this linked list
    
    OULinkedListEnumerator<T>* eNum() const;
};

template <typename T> std::ostream& operator<<(std::ostream& os, const OULinkedList<T>& ll)
{
    OULinkedListEnumerator<T> e = ll.enumerator();
    while(e.hasNext())
    {
        T item = e.next();
        os << item << endl;
    }
    return os;
}
template <typename T> OULinkedList<T>::OULinkedList(Comparator<T>* comparator)
{
    this->comparator = comparator;
}

template <typename T> OULinkedList<T>::~OULinkedList()
{
    delete first;
}

template <typename T> bool OULinkedList<T>::insert(const T* item)
{
    if(this->first == nullptr)
    {
        this->append(item);
        return true;
    }
    else
    {
        OULink<T>* curr = this->first;
        OULink<T>* prev = nullptr;
        
        T* item2 = curr->data;
        int c = comparator->compare(*item, *item2);
        
        while(c == 1)
        {
            if(curr->next == nullptr)
            {
                break;
            }
            else
            {
                prev = curr;
                curr = curr->next;
                item2 = curr->data;
                c = comparator->compare(*item, *item2);
            }
        }
        if(c==0)
        {
            return false;
        }
        else if(c==-1)
        {
            OULink<T>* neu = new OULink<T>(item);
            if(prev==nullptr)
            {
                this->first = neu;
                neu->next = curr;
            }
            else
            {
                prev->next = neu;
                neu->next = curr;
            }
            this->size++;
            return true;
        }
        else if(c==1)
        {
            this->append(item);
            return true;
        }
    }
    return false;
}

template <typename T> bool OULinkedList<T>::append(const T* item)
{
    if(first==nullptr)
    {
        OULink<T>* neu = new OULink<T>(item);
        first = neu;
        last = neu;
        this->size++;
        return true;
    }
    
    T* last_item = this->last->data;
    int c = comparator->compare(*item, *last_item);
    
    if (c == 1)
    {
        OULink<T>* neu = new OULink<T>(item);
        this->last->next = neu;
        this->last = neu;
        this->last->next = nullptr;
        this->size++;
        return true;
    }
    return false;
}

template <typename T> bool OULinkedList<T>::replace(const T* item)
{
    OULink<T>* curr = this->first;
    OULink<T>* prev = nullptr;
    
    T* item2 = curr->data;
    int c = comparator->compare(*item, *item2);
    while(c!=0)
    {
        if(curr->next == nullptr)
        {
            break;
        }
        else
        {
            prev = curr;
            curr = curr->next;
            item2 = curr->data;
            c = comparator->compare(*item, *item2);
        }
    }
    if(c==0)
    {
        OULink<T>* neu = new OULink<T>(item);
        if(prev==nullptr)
        {
            this->first = neu;
        }
        else
        {
            prev->next = neu;
        }
        
        if(curr->next==nullptr)
        {
            this->last = neu;
        }
        else
        {
            neu->next = curr->next;
        }
        
        
        return true;
    }
    return false;
}

template <typename T> bool OULinkedList<T>::remove(const T* item)
{
    OULink<T>* curr = this->first;
    OULink<T>* prev = nullptr;
    
    T* item2 = curr->data;
    int c = comparator->compare(*item, *item2);
    while(c!=0)
    {
        if(curr->next == nullptr)
        {
            break;
        }
        else
        {
            prev = curr;
            curr = curr->next;
            item2 = curr->data;
            c = comparator->compare(*item, *item2);
        }
    }
    if(c==0)
    {
        if(prev == nullptr)
        {
            this->first = curr->next;
        }
        else if(curr->next == nullptr)
        {
            prev->next = nullptr;
        }
        else
        {
            prev->next = curr->next;
        }
        size--;
        return true;
    }
    return false;
}

template <typename T> T OULinkedList<T>::get() const
{
    if(first == nullptr)
    {
        throw new ExceptionLinkedListAccess();
    }
    T* temp = this->first->data;
    return *temp;
}

template <typename T> T OULinkedList<T>::find(const T* item) const
{
    if(this->first==nullptr)
    {
        throw new ExceptionLinkedListAccess();
    }
    else
    {
        OULink<T>* curr = this->first;
        
        T* item2 = curr->data;
        int c = comparator->compare(*item, *item2);
        while(c!=0)
        {
            if(curr->next == nullptr)
            {
                break;
            }
            else
            {
                curr = curr->next;
                item2 = curr->data;
                c = comparator->compare(*item, *item2);
            }
        }
        if(c == 0)                                                                                                    //check last item
        {
            return *item2;
        }
        else
        {
            throw new ExceptionLinkedListAccess();
        }
    }
}

template <typename T> unsigned long OULinkedList<T>::getSize() const
{
    return this->size;
}

template <typename T> OULinkedListEnumerator<T> OULinkedList<T>::enumerator() const
{
    OULinkedListEnumerator<T>* e = new OULinkedListEnumerator<T>(first);
    return *e;
}

template <typename T> OULinkedListEnumerator<T>* OULinkedList<T>::eNum() const
{
    return new OULinkedListEnumerator<T>(first);
}

#endif // !OU_LINKED_LIST
