
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "../Platform.h"
#include <stdlib.h>
//#include <string.h>
#include <new.h>

template <class TYPE>
class Array
{
public:
	Array()
	{
		m_Count = m_Capacity = 0;
		m_List = nullptr;
	}

	Array(const uint count, const uint capacity)
	{
		ASSERT(count <= capacity);

		m_Count = count;
		m_Capacity = capacity;
		m_List = (TYPE *) malloc(capacity * sizeof(TYPE));
	}
	
	~Array()
	{
		TYPE* curr = m_List + m_Count;

		// Call destructor for remaining items
		while (curr != m_List)
		{
			--curr;
			curr->~TYPE();
		}
		free(m_List);
	}

	TYPE* GetArray() const { return m_List; }

	TYPE* AbandonArray()
	{
		TYPE* rList = m_List;
		m_List = nullptr;
		m_Count = 0;
		m_Capacity = 0;
		return rList;
	}

	TYPE& operator [] (const uint index) const
	{
		ASSERT(index < m_Count);
		return m_List[index];
	}
	uint GetCount() const { return m_Count; }

	void SetCount(const uint count)
	{
		if (count > m_Capacity)
			SetCapacity(count);


		TYPE* curr = m_List + m_Count;
		TYPE* end = m_List + count;

		if (curr < end)
		{
			// Call constructor on in-place memory locations for all new items.
			do
			{
				new (curr) TYPE();
				++curr;
			}
			while (curr != end);
		}
		else if (curr > end)
		{
			// Call destructor for any deleted items
			do
			{
				--curr;
				curr->~TYPE();
			}
			while (curr != end);
		}


		// Call constructors on in-place memory locations for all new items.
// 		for (uint i = m_Count; i < count; i++)
// 		{
// 			new (m_List + i) TYPE();
// 		}

// 		for (uint i = m_Count; i >= count; i--)
// 		{
// 			m_List[i].~TYPE();
// 		}

		m_Count = count;
	}

	void SetCapacity(const uint capacity)
	{
		ASSERT(m_Count <= capacity);

		m_Capacity = capacity;
		m_List = (TYPE *) realloc(m_List, capacity * sizeof(TYPE));
	}

	uint Add(const TYPE object)
	{
		if (m_Count == m_Capacity)
		{
			if (m_Capacity)
				m_Capacity += m_Capacity;
			else
				m_Capacity = 8;

			m_List = (TYPE *) realloc(m_List, m_Capacity * sizeof(TYPE));
		}

		m_List[m_Count] = object;
		return m_Count++;
	}

	void FastRemove(const uint index)
	{
		ASSERT(index < m_Count);

		m_Count--;
		m_List[index] = m_List[m_Count];
	}

	void OrderedRemove(const uint index)
	{
		ASSERT(index < m_Count);

		m_Count--;
		memmove(m_List + index, m_List + index + 1, (m_Count - index) * sizeof(TYPE));
	}

	int Find(const TYPE object) const
	{
		for (uint i = 0; i < m_Count; i++)
		{
			if (object == m_List[i])
				return i;
		}

		return -1;
	}

	void Clear()
	{
		m_Count = 0;
	}

	void Reset()
	{
		free(m_List);
		m_List = nullptr;
		m_Count = m_Capacity = 0;
	}

	/*void Sort()
	{
		std::sort(m_List, m_List + m_Count);
	}*/

protected:
	TYPE* m_List;
	uint m_Count;
	uint m_Capacity;
};


/************************************************************************/


template <class TYPE>
class StaticArray
{
public:
	StaticArray()
	{
		m_Count = 0;
		m_List = nullptr;
	}

	StaticArray(const uint capacity)
	{
		m_Count = capacity;
		m_List = (TYPE *) malloc(capacity * sizeof(TYPE));
	}
	
	~StaticArray()
	{
		free(m_List);
	}

	TYPE* GetArray() const { return m_List; }

	TYPE* AbandonArray()
	{
		TYPE* rList = m_List;
		m_List = nullptr;
		m_Count = 0;
		return rList;
	}

	TYPE& operator [] (const uint index) const
	{
		ASSERT(index < m_Count);
		return m_List[index];
	}
	uint GetCount() const { return m_Count; }

	void SetCapacity(const uint capacity)
	{
		m_Count = capacity;
		m_List = (TYPE *) realloc(m_List, capacity * sizeof(TYPE));
	}

	int Find(const TYPE object) const
	{
		for (uint i = 0; i < m_Count; i++)
		{
			if (object == m_List[i])
				return i;
		}

		return -1;
	}

	void Reset()
	{
		free(m_List);
		m_List = nullptr;
		m_Count = m_Capacity = 0;
	}

	/*void Sort()
	{
		std::sort(m_List, m_List + m_Count);
	}*/

protected:
	TYPE* m_List;
	uint m_Count;
};


/************************************************************************/

/*
#include <malloc.h>

template <class TYPE>
class StackArray
{
public:
	StackArray(uint count)
	{
		m_List = (TYPE *) alloca(count * sizeof(TYPE));
		//m_List = (TYPE *) malloc(count * sizeof(TYPE));
		m_Count = count;
	}

	~StackArray()
	{
		//free(m_List);
	}

	TYPE *GetArray() const { return m_List; }

	TYPE &operator [] (const uint index) const { return m_List[index]; }
	uint GetCount() const { return m_Count; }

	int Find(const TYPE object)
	{
		for (uint i = 0; i < m_Count; i++)
		{
			if (object == m_List[i])
				return i;
		}

		return -1;
	}

	void Sort()
	{
		std::sort(m_List, m_List + m_Count);
	}

protected:
	TYPE *m_List;
	uint m_Count;
};
*/

#endif // _ARRAY_H_
