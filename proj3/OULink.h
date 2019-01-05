#ifndef OU_LINK_H
#define OU_LINK_H

template <typename T>
class OULink {
	template <typename F>
	friend class OULinkedList;
	template <typename F>
	friend class OULinkedListEnumerator;
private:
	T* data = nullptr;										// pointer to data item of any type
	OULink* next = nullptr;								// pointer to next link
public:
	OULink(const T* item);
	virtual ~OULink();
};

template<typename T> OULink<T>::OULink(const T* item)
{
	data = const_cast<T*>(item);
}

template<typename T> OULink<T>::~OULink()
{
	delete data;
	if(next !=  nullptr)
	{
		delete next;
	}
  data = nullptr;
	next = nullptr;
}

#endif // !OU_LINK_H
