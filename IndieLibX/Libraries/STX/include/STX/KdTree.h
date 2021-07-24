//-----------------------------------------------------------------------------
// File: Framework\Direct3D\KDTree.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <stdlib.h>
#include <math.h>

template <class TYPE, int COMP_COUNT>
struct KDNode {
	~KDNode(){
		delete left;
		delete right;
	}

	TYPE comp[COMP_COUNT];
	KDNode *left, *right;
};


template <class TYPE, int COMP_COUNT>
class KDTree {
public:
	KDTree(){
		top = NULL;
	}

	~KDTree(){
		delete top;
	}

	void clear(){
        delete top;
        top = NULL;
	}

	void add(const TYPE *item){
		KDNode <TYPE, COMP_COUNT> *newNode = new KDNode<TYPE, COMP_COUNT>;
		stx_memcpy(newNode->comp, item, sizeof(newNode->comp));
		newNode->left  = NULL;
		newNode->right = NULL;

		int n = 0;

		if (top){
			KDNode <TYPE, COMP_COUNT> *node = top;

			while (true){
				if (item[n] < node->comp[n]){
					if (node->left){
						node = node->left;
					} else {
						node->left = newNode;
						return;
					}
				} else {
					if (node->right){
						node = node->right;
					} else {
						node->right = newNode;
						return;
					}
				}

				n++;
				if (n >= COMP_COUNT) n = 0;
			}

		} else {
			top = newNode;
		}
	}

	bool hasPointWithin(const TYPE *item, const TYPE radius){
		if (top){
			return isWithin(top, item, radius, 0);
		} else {
			return false;
		}
	}

protected:
	bool isWithin(const KDNode <TYPE, COMP_COUNT> *node, const TYPE *item, const TYPE radius, int n){
		TYPE dist = distSquared(node->comp, item);
		if (dist < radius * radius) return true;

		TYPE d = item[n] - node->comp[n];

		n++;
		if (n >= COMP_COUNT) n = 0;

		if (node->left && d < radius){
            if (isWithin(node->left, item, radius, n)) return true;
		}
		if (node->right && -d < radius){
            if (isWithin(node->right, item, radius, n)) return true;
		}

		return false;
	}

	TYPE distSquared(const TYPE *i0, const TYPE *i1){
		TYPE sum = 0;
		for (int i = 0; i < COMP_COUNT; i++){
			TYPE d = i0[i] - i1[i];
            sum += d * d;
		}
		return sum;
	}


	KDNode <TYPE, COMP_COUNT> *top;
    const KDNode <TYPE, COMP_COUNT> *minNode;
	TYPE minDist;
};

#endif // _KDTREE_H_
