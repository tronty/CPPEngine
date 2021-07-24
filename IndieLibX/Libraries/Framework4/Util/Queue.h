
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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "../Platform.h"

template <class TYPE>
struct QueueNode
{
	QueueNode <TYPE> *prev;
	QueueNode <TYPE> *next;
	TYPE object;
};

template <class TYPE>
class Queue
{
public:
	Queue()
	{
		m_Count = 0;
		m_First = NULL;
		m_Last  = NULL;
		m_Curr  = NULL;
		m_Del   = NULL;
	}

	~Queue()
	{
		Clear();
	}

	uint GetCount() const { return m_Count; }

	void AddFirst(const TYPE object)
	{
		QueueNode <TYPE> *node = new QueueNode <TYPE>;
		node->object = object;
		InsertNodeFirst(node);
		m_Count++;
	}

	void AddLast(const TYPE object)
	{
		QueueNode <TYPE> *node = new QueueNode <TYPE>;
		node->object = object;
		InsertNodeLast(node);
		m_Count++;
	}

	void InsertBeforeCurrent(const TYPE object)
	{
		QueueNode <TYPE> *node = new QueueNode <TYPE>;
		node->object = object;
		InsertNodeBefore(m_Curr, node);
		m_Count++;
	}

	void InsertAfterCurrent(const TYPE object)
	{
		QueueNode <TYPE> *node = new QueueNode <TYPE>;
		node->object = object;
		InsertNodeAfter(m_Curr, node);
		m_Count++;
	}

	bool RemoveCurrent()
	{
		if (m_Curr != NULL)
		{
			ReleaseNode(m_Curr);
			if (m_Del)
				delete m_Del;
			m_Del = m_Curr;
			m_Count--;
		}
		return (m_Curr != NULL);
	}

	bool GoToFirst() { return (m_Curr = m_First) != NULL; }
	bool GoToLast () { return (m_Curr = m_Last ) != NULL; }
	bool GoToPrev () { return (m_Curr = m_Curr->prev) != NULL; }
	bool GoToNext () { return (m_Curr = m_Curr->next) != NULL; }
	bool GoToObject(const TYPE object)
	{
		m_Curr = m_First;
		while (m_Curr != NULL)
		{
			if (object == m_Curr->object)
				return true;
			m_Curr = m_Curr->next;
		}
		return false;
	}

	TYPE GetCurrent() const { return m_Curr->object; }
	void SetCurrent(const TYPE object) { m_Curr->object = object; }

	TYPE GetPrev() const { return m_Curr->prev->object; }
	TYPE GetNext() const { return m_Curr->next->object; }
	TYPE GetPrevWrap() const { return ((m_Curr->prev != NULL)? m_Curr->prev : m_Last)->object; }
	TYPE GetNextWrap() const { return ((m_Curr->next != NULL)? m_Curr->next : m_First)->object; }

	void Clear()
	{
		delete m_Del;
		m_Del = NULL;
		while (m_First)
		{
			m_Curr = m_First;
			m_First = m_First->next;
			delete m_Curr;
		}
		m_Last = m_Curr = NULL;
		m_Count = 0;
	}

	void MoveCurrentToTop()
	{
		if (m_Curr != NULL)
		{
			ReleaseNode(m_Curr);
			InsertNodeFirst(m_Curr);
		}
	}

protected:
	void InsertNodeFirst(QueueNode <TYPE> *node)
	{
		if (m_First != NULL)
			m_First->prev = node;
		else
			m_Last = node;

		node->next = m_First;
		node->prev = NULL;

		m_First = node;
	}

	void InsertNodeLast(QueueNode <TYPE> *node)
	{
		if (m_Last != NULL)
			m_Last->next = node;
		else
			m_First = node;

		node->prev = m_Last;
		node->next = NULL;

		m_Last = node;
	}

	void InsertNodeBefore(QueueNode <TYPE> *at, QueueNode <TYPE> *node)
	{
		QueueNode <TYPE> *prev = at->prev;
		at->prev = node;
		if (prev)
			prev->next = node;
		else
			m_First = node;

		node->next = at;
		node->prev = prev;
	}

	void InsertNodeAfter(QueueNode <TYPE> *at, QueueNode <TYPE> *node)
	{
		QueueNode <TYPE> *next = at->next;
		at->next = node;
		if (next)
			next->prev = node;
		else
			m_Last = node;

		node->prev = at;
		node->next = next;
	}

	void ReleaseNode(const QueueNode <TYPE> *node)
	{
		if (node->prev == NULL)
			m_First = node->next;
		else
			node->prev->next = node->next;

		if (node->next == NULL)
			m_Last = node->prev;
		else
			node->next->prev = node->prev;
	}

	QueueNode <TYPE> *m_First, *m_Last, *m_Curr, *m_Del;
	uint m_Count;
};

#endif // _QUEUE_H_
