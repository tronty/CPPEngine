
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

#pragma once

#include "../Platform.h"

template <class TYPE>
struct KdNode
{
	KdNode <TYPE> *lower, *upper;
	uint index;
	TYPE *point;
};

template <class TYPE>
class KdTree
{
public:
	KdTree(const uint nComp, const uint capasity)
	{
		m_Top = NULL;
		m_Dimensions = nComp;
		m_Count = 0;

		m_Curr = m_Mem = (ubyte *) malloc(capasity * (sizeof(KdNode <TYPE>) + nComp * sizeof(TYPE)));
	}

	~KdTree()
	{
		free(m_Mem);
	}

	uint AddUnique(const TYPE *point)
	{
		if (m_Top != NULL)
		{
			return AddUniqueToNode(m_Top, point);
		}
		else
		{
			m_Top = NewNode(point);
			return 0;
		}
	}

	void Clear()
	{
		m_Curr = m_Mem;

		m_Top = NULL;
		m_Count = 0;
	}

	uint GetCount() const { return m_Count; }

private:
	uint AddUniqueToNode(KdNode <TYPE> *node, const TYPE *point)
	{
		uint comp = 0;

		for (;;)
		{
			if (point[comp] < node->point[comp])
			{
				if (node->lower)
				{
					node = node->lower;
				}
				else
				{
					node->lower = NewNode(point);
					return node->lower->index;
				}
			}
			else if (point[comp] > node->point[comp])
			{
				if (node->upper)
				{
					node = node->upper;
				}
				else
				{
					node->upper = NewNode(point);
					return node->upper->index;
				}
			}
			else if (IsEqualToNode(node, point))
			{
				return node->index;
			}
			else
			{
				if (node->upper)
				{
					node = node->upper;
				}
				else
				{
					node->upper = NewNode(point);
					return node->upper->index;
				}
			}

			++comp;
			if (comp == m_Dimensions)
				comp = 0;

		}
	}

	KdNode <TYPE> *NewNode(const TYPE *point)
	{
		KdNode <TYPE> *node = (KdNode <TYPE> *) NewMem(sizeof(KdNode <TYPE>));
		node->point = (TYPE *) NewMem(m_Dimensions * sizeof(TYPE));

		memcpy(node->point, point, m_Dimensions * sizeof(TYPE));

		node->lower = NULL;
		node->upper = NULL;
		node->index = m_Count;

		m_Count++;

		return node;
	}

	bool IsEqualToNode(const KdNode <TYPE> *node, const TYPE *point) const
	{
		uint i = 0;
		do
		{
			//if (fabsf(node->point[i] - point[i]) > 0.001f)
			if (node->point[i] != point[i])
				return false;
			i++;
		} while (i < m_Dimensions);

		return true;
	}

	void *NewMem(const uint size)
	{
		ubyte *rmem = m_Curr;
		m_Curr += size;
		return rmem;
	}

	KdNode <TYPE>* m_Top;
	uint m_Dimensions;
	uint m_Count;

	ubyte* m_Mem;
	ubyte* m_Curr;
};
