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
public:
    OULinkedListEnumerator(OULink<T>* first);
    virtual ~OULinkedListEnumerator();
    bool hasNext();
    T next();
    T peek();
    OULink<T>* curr();
    T* currentData();
};
template <typename T> OULinkedListEnumerator<T>::OULinkedListEnumerator(OULink<T>* first)
{
    this->current = first;
}

template <typename T> OULinkedListEnumerator<T>::~OULinkedListEnumerator()
{
    this->current = nullptr;
}

template <typename T> bool OULinkedListEnumerator<T>::hasNext()
{
    if(this->current == nullptr || this->current->next == nullptr)
    {
        return false;
    }
    return true;
}

template <typename T> T OULinkedListEnumerator<T>::next()
{
    if(this->current == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    T* temp = this->current->data;
    this->current = this->current->next;
    return *temp;
}

template <typename T> T OULinkedListEnumerator<T>::peek()
{
    if(this->current == nullptr)
    {
        throw new ExceptionEnumerationBeyondEnd();
    }
    OULink<T>* t = this->current;
    T* temp = t->data;
    return *temp;
}

template <typename T> OULink<T>* OULinkedListEnumerator<T>::curr()
{
    return this->current;
}

template <typename T> T* OULinkedListEnumerator<T>::currentData()
{
    OULink<T>* curr = this->current;
    if(curr == nullptr)
    {
        return nullptr;
    }
    return curr->data;
}

#endif // !OU_LINKED_LIST_ENUMERATOR
