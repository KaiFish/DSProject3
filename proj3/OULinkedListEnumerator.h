#ifndef OU_LINKED_LIST_ENUMERATOR
#define OU_LINKED_LIST_ENUMERATOR

#include "Enumerator.h"
#include "Exceptions.h"
#include "OULink.h"
#include "iostream"

template <typename T>
class OULinkedListEnumerator : public Enumerator<T>
{
private:
    OULink<T>* current;
    bool first = true;
    
public:
    OULinkedListEnumerator(OULink<T>* first);
    virtual ~OULinkedListEnumerator();
    bool hasNext() const;
    T next();
    T peek() const;
};

template <typename T> OULinkedListEnumerator<T>::OULinkedListEnumerator(OULink<T>* first)
{
    this->current = first;
}

template <typename T> OULinkedListEnumerator<T>::~OULinkedListEnumerator()
{
    this->current = nullptr;
}

template <typename T> bool OULinkedListEnumerator<T>::hasNext() const
{
    if(first) return this->current != nullptr;
    else return this->current->next != nullptr;
    
}

template <typename T> T OULinkedListEnumerator<T>::next()
{
    if(this->current == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    
    if(this->first)
    {
        T* temp = this->current->data;
        this->first = false;
        return *temp;
    }
    
    this->current = this->current->next;
    
    if(this->current == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    
    T* temp = this->current->data;
    return *temp;
}

template <typename T> T OULinkedListEnumerator<T>::peek() const
{
    if(this->current == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    
    if(this->first)
    {
        T* temp = this->current->data;
        return *temp;
    }
    
    if(this->current->next == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    
    T* temp =  this->current->next->data;
    return *temp;
}


#endif // !OU_LINKED_LIST_ENUMERATOR
