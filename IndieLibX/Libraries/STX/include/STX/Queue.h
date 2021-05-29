//-----------------------------------------------------------------------------
// File: Framework\Util\Queue.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>
#include <math.h>

template <class ARG_TYPE>
struct QueueNode {
	ARG_TYPE object;

    QueueNode <ARG_TYPE> *prev;
    QueueNode <ARG_TYPE> *next;
};

template <class ARG_TYPE>
class Queue {
public:
	Queue(){
		count = 0;
		first = last = curr = 0;
	}

	~Queue(){
		clear();
	}

	unsigned int getCount() const { return count; }

	void addFirst(const ARG_TYPE object){
		QueueNode <ARG_TYPE> *node = new QueueNode <ARG_TYPE>;
		node->object = object;
		insertNodeFirst(node);
		count++;
	}

	bool removeCurrent(bool moveForward = true){
		if (curr != 0){
			QueueNode <ARG_TYPE> *newCurr = (moveForward? curr->next : curr->prev);
			releaseNode(curr);
			delete curr;
			curr = newCurr;
			count--;
		}
		return (curr != 0);
	}

	bool goToFirst(){ return (curr = first     ) != 0; }
	bool goToLast (){ return (curr = last      ) != 0; }
	bool goToPrev (){ return (curr = curr->prev) != 0; }
	bool goToNext (){ return (curr = curr->next) != 0; }
	bool goToObject(const ARG_TYPE object){
		curr = first;
		while (curr != 0 && object != curr->object){
			curr = curr->next;
		}
		return (curr != 0);
	}

	ARG_TYPE getCurrent() const { return curr->object; }    	

	void clear(){
		while (first){
            curr = first;
			first = first->next;
			delete curr;
		}
		last = curr = 0;
		count = 0;
	}

	void moveCurrentToTop(){
		if (curr != 0){
			releaseNode(curr);
			insertNodeFirst(curr);
		}
	}

protected:
	void insertNodeFirst(QueueNode <ARG_TYPE> *node){
		if (first != 0){
            first->prev = node;
		} else {
			last = node;
		}
		node->next = first;
		node->prev = 0;

		first = node;
	}

	void releaseNode(const QueueNode <ARG_TYPE> *node){
		if (node->prev == 0){
			first = node->next;			
		} else {
			node->prev->next = node->next;
		}
		if (node->next == 0){
			last = node->prev;
		} else {
			node->next->prev = node->prev;
		}
	}

	unsigned int count;
	QueueNode <ARG_TYPE> *first, *last, *curr;
};

#endif // _QUEUE_H_
