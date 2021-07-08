/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\Queue.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _QUEUE_H_
#define _QUEUE_H_

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
		first = last = curr = NULL;
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
		if (curr != NULL){
			QueueNode <ARG_TYPE> *newCurr = (moveForward? curr->next : curr->prev);
			releaseNode(curr);
			delete curr;
			curr = newCurr;
			count--;
		}
		return (curr != NULL);
	}

	bool goToFirst(){ return (curr = first     ) != NULL; }
	bool goToLast (){ return (curr = last      ) != NULL; }
	bool goToPrev (){ return (curr = curr->prev) != NULL; }
	bool goToNext (){ return (curr = curr->next) != NULL; }
	bool goToObject(const ARG_TYPE object){
		curr = first;
		while (curr != NULL && object != curr->object){
			curr = curr->next;
		}
		return (curr != NULL);
	}

	ARG_TYPE getCurrent() const { return curr->object; }    	

	void clear(){
		while (first){
            curr = first;
			first = first->next;
			delete curr;
		}
		last = curr = NULL;
		count = 0;
	}

	void moveCurrentToTop(){
		if (curr != NULL){
			releaseNode(curr);
			insertNodeFirst(curr);
		}
	}

protected:
	void insertNodeFirst(QueueNode <ARG_TYPE> *node){
		if (first != NULL){
            first->prev = node;
		} else {
			last = node;
		}
		node->next = first;
		node->prev = NULL;

		first = node;
	}

	void releaseNode(const QueueNode <ARG_TYPE> *node){
		if (node->prev == NULL){
			first = node->next;			
		} else {
			node->prev->next = node->next;
		}
		if (node->next == NULL){
			last = node->prev;
		} else {
			node->next->prev = node->prev;
		}
	}

	unsigned int count;
	QueueNode <ARG_TYPE> *first, *last, *curr;
};

#endif // _QUEUE_H_
