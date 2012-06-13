#ifndef LINKED_LIST_H
#define LINKED_LIST_H


//-----------------------------------------------------------------------------
// Linked List Class
//-----------------------------------------------------------------------------
template<class T> 
class LinkedList
{
public:	
	struct Element
	{
		T*	 data; // Pointer to the data held in the element.
		Element* next; // Pointer to the next element in the list.
		Element* prev; // Pointer to the previous element in the list.
		
		Element( T *element )
		{
			data = element;
			next = prev = NULL;
		}

		~Element()
		{
			SAFE_DELETE( data );
			if( next )
				next->prev = prev;
			if( prev )
				prev->next = next;
		}
	};

private:
	Element*      m_first;	 	// First element in the linked list.
	Element*      m_last;	 	// Last element in the linked list.
	Element*      m_iterate; 	// Used for iterating the linked list.
	Element*      m_temp;	 	// Used for temporary storage in various operations.
	unsigned long m_totalElements;  // Total number of elements in the linked list.

public:
	LinkedList();
	~LinkedList();
	T*    Add( T* element );	
	T*    InsertBefore( T *element, Element* nextElement );
	void  Remove( T** element );
	void  Empty();	
	void  ClearPointers();	
	void  ClearPointer( T** element );	
	T*    Iterate( bool restart = false );
	T*    GetCurrent();	
	T*    GetFirst();	
	T*    GetLast();	
	T*    GetNext( T* element );
	T*    GetRandom();	
	
	unsigned long GetTotalElements();
	Element* GetCompleteElement( T* element )
	{
		m_temp = m_first;
		while( m_temp != NULL )
		{
			if( m_temp->data == element )
				return m_temp;

			m_temp = m_temp->next;
		}
		return NULL;
	}
};


template <class T>
LinkedList<T>::LinkedList()
{
	m_first = m_last = m_iterate = m_temp = NULL;
	m_totalElements = 0;
}


template <class T>
LinkedList<T>::~LinkedList()
{
	Empty();
}


template <class T>
T* LinkedList<T>::Add( T *element )
{
	if( element == NULL )
		return NULL;

	if( m_first == NULL )
	{
		m_first = new Element( element );
		m_last = m_first;
	}
	else
	{
		m_last->next = new Element( element );
		m_last->next->prev = m_last;
		m_last = m_last->next;
	}
	m_totalElements++;

	return m_last->data;
}


template <class T>
T* LinkedList<T>::InsertBefore( T* element, Element* nextElement )
{
	m_temp = nextElement->prev;

	m_totalElements++;

	if( m_temp == NULL )
	{
		m_first = new Element( element );
		m_first->next = nextElement;
		nextElement->prev = m_first;
		return m_first->data;
	}
	else
	{
		m_temp->next = new Element( element );
		m_temp->next->prev = m_temp;
		m_temp->next->next = nextElement;
		nextElement->prev = m_temp->next;
		return m_temp->next->data;
	}
}


template <class T>
void LinkedList<T>::Remove( T **element )
{
	m_temp = m_first;
	while( m_temp != NULL )
	{
		if( m_temp->data == *element )
		{
			if( m_temp == m_first )
			{
				m_first = m_first->next;
				if( m_first )
					m_first->prev = NULL;
			}
			if( m_temp == m_last )
			{
				m_last = m_last->prev;
				if( m_last )
					m_last->next = NULL;
			}

			SAFE_DELETE( m_temp );
			*element = NULL;
			m_totalElements--;
			return;
		}
		m_temp = m_temp->next;
	}
}


template <class T>
void LinkedList<T>::Empty()
{
	while( m_last != NULL )
	{
		m_temp = m_last;
		m_last = m_last->prev;
		SAFE_DELETE( m_temp );
	}
	m_first = m_last = m_iterate = m_temp = NULL;
	m_totalElements = 0;
}


template <class T>
void LinkedList<T>::ClearPointers()
{
	while( m_last != NULL )
	{
		m_temp = m_last;
		m_temp->data = NULL;
		m_last = m_last->prev;
		SAFE_DELETE( m_temp );
	}
	m_first = m_last = m_iterate = m_temp = NULL;
	m_totalElements = 0;
}


template <class T>
void LinkedList<T>::ClearPointer( T **element )
{
	m_temp = m_first;
	while( m_temp != NULL )
	{
		if( m_temp->data == *element )
		{
			if( m_temp == m_first )
			{
				m_first = m_first->next;
				if( m_first )
					m_first->prev = NULL;
			}
			if( m_temp == m_last )
			{
				m_last = m_last->prev;
				if( m_last )
					m_last->next = NULL;
			}

			m_temp->data = NULL;
			SAFE_DELETE( m_temp );
			*element = NULL;			
			m_totalElements--;
			return;
		}
		m_temp = m_temp->next;
	}
}


template <class T>
T* LinkedList<T>::Iterate( bool restart = false )
{
	if( restart )
		m_iterate = NULL;
	else
	{
		if( m_iterate == NULL )
			m_iterate = m_first;
		else
			m_iterate = m_iterate->next;
	}

	if( m_iterate == NULL )
		return NULL;
	else
		return m_iterate->data;
}


template <class T>
T* LinkedList<T>::GetCurrent()
{
	if( m_iterate )
		return m_iterate->data;
	else
		return NULL;
}


template <class T>
T* LinkedList<T>::GetFirst()
{
	if( m_first )
		return m_first->data;
	else
		return NULL;
}


template <class T>
T* LinkedList<T>::GetLast()
{
	if( m_last )
		return m_last->data;
	else
		return NULL;
}


template <class T>
T* LinkedList<T>::GetNext( T* element )
{
	m_temp = m_first;
	while( m_temp != NULL )
	{
		if( m_temp->data == element )
		{
			if( m_temp->next == NULL )
				return NULL;
			else
				return m_temp->next->data;
		}
		m_temp = m_temp->next;
	}
	return NULL;
}


template <class T>
T* LinkedList<T>::GetRandom()
{
	if( m_totalElements == 0 )
		return NULL;
	else if( m_totalElements == 1 )
		return m_first->data;

	unsigned long element = rand() % m_totalElements;

	m_temp = m_first;
	for( unsigned long e = 0; e < element; e++ )
		m_temp = m_temp->next;

	return m_temp->data;
}


template <class T>
unsigned long LinkedList<T>::GetTotalElements()
{
	return m_totalElements;
}


#endif