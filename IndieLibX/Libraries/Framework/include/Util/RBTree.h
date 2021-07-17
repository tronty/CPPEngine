/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\RBTree.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _RBTREE_H_
#define _RBTREE_H_

/* 
	This class implements a so called red-black binary tree, which is a binary tree
	which is guaranteed to be approximatly balanced such that no path from the root
	to a leaf is longer than twice the length of the shortest path. The inserting,
	deleting and searching operations all execute in O(log(n)) time.
*/


enum eCOLOR { RED, BLACK };

template <class TYPE>
class Node {
public:
	Node *parent;
	Node *left;
	Node *right;

	TYPE object;
	eCOLOR color;
};


template <class TYPE>
class RBTree {
public:
	RBTree(){
		// Create a "NULL" node. This simplifies things over having NULL pointers.
		null = new Node <TYPE>;
		null->left = NULL;
		null->right = NULL;
		null->parent = NULL;
		null->color = BLACK;
		curr = root = null;
	}
	
	~RBTree(){
		clearNode(root);
		delete null;
	}

	void clear(){
		clearNode(root);
		curr = root = null;
	}

	TYPE &getCurrent() const { return curr->object; }
	bool isInSet(const TYPE &object){ return findNode(object) != null; }

	bool goToFirst(){
		if (root == null) return false;
		curr = minNode(root);
		return true;
	}

	bool goToLast(){
		if (root == null) return false;
		curr = maxNode(root);
		return true;
	}

	bool goToObject(const TYPE &object){
		return (curr = findNode(object)) != null;
	}

	bool goToNext(){
		if (curr == null) return false;
		return (curr = nextNode(curr)) != null;
	}

	bool goToPrev(){
		if (curr == null) return false;
		return (curr = prevNode(curr)) != null;
	}

	void insert(const TYPE &object);
	bool remove(const TYPE &object);

protected:
	Node <TYPE> *root, *curr, *null;

	Node <TYPE> *treeInsert(const TYPE &object);
	void treeRemove(Node <TYPE> *node);
	void treeRemoveFixColors(Node <TYPE> *node);
	void leftRotate(Node <TYPE> *node);
	void rightRotate(Node <TYPE> *node);
	void clearNode(Node <TYPE> *node);

	Node <TYPE> *minNode(Node <TYPE> *node){
		while (node->left != null) node = node->left;
		return node;
	}

	Node <TYPE> *maxNode(Node <TYPE> *node){
		while (node->right != null) node = node->right;
		return node;
	}

	Node <TYPE> *nextNode(Node <TYPE> *node){
		if (node->right != null) return minNode(node->right);

		Node <TYPE> *pNode = node->parent;
		while (pNode != null && node == pNode->right){
			node  = pNode;
			pNode = pNode->parent;
		}

		return pNode;
	}

	Node <TYPE> *prevNode(Node <TYPE> *node){
		if (node->left != null) return maxNode(node->left);

		Node <TYPE> *pNode = node->parent;
		while (pNode != null && node == pNode->left){
			node  = pNode;
			pNode = pNode->parent;
		}

		return pNode;
	}

	Node <TYPE> *findNode(const TYPE &object){
		Node <TYPE> *node = root;
		while (node != null && object != node->object){
			node = (object < node->object)? node->left : node->right;
		}
		return node;
	}
};

template <class TYPE>
void RBTree <TYPE>::clearNode(Node <TYPE> *node){
	if (node != null){
		clearNode(node->left);
		clearNode(node->right);
		delete node;
	}
}

template <class TYPE>
void RBTree <TYPE>::insert(const TYPE &object){
	Node <TYPE> *x = treeInsert(object);
	x->color = RED;

	while (x != root && x->parent->color == RED){
		if (x->parent == x->parent->parent->left){
			Node <TYPE> *y = x->parent->parent->right;
			if (y->color == RED){
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->right){
					x = x->parent;
					leftRotate(x);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				rightRotate(x->parent->parent);
			}
		} else {
			Node <TYPE> *y = x->parent->parent->left;
			if (y->color == RED){
				x->parent->color = BLACK;
				y->color = BLACK;
				x->parent->parent->color = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->left){
					x = x->parent;
					rightRotate(x);
				}
				x->parent->color = BLACK;
				x->parent->parent->color = RED;
				leftRotate(x->parent->parent);
			}
		}
	}

	root->color = BLACK;
}

template <class TYPE>
bool RBTree <TYPE>::remove(const TYPE &object){
	Node <TYPE> *node = findNode(object);
	if (node == null) return false;
	treeRemove(node);
	return true;
}

template <class TYPE>
Node <TYPE> *RBTree <TYPE>::treeInsert(const TYPE &object){
	Node <TYPE> *currNode, *lastNode, *newNode;

	lastNode = null;
	currNode = root;

	while (currNode != null){
		lastNode = currNode;
		currNode = (object < currNode->object)? currNode->left : currNode->right;
	}

	newNode = new Node <TYPE>;
	newNode->left  = null;
	newNode->right = null;
	newNode->object = object;

	newNode->parent = lastNode;
	if (lastNode == null){
		root = newNode;
	} else {
		if (newNode->object < lastNode->object){
			lastNode->left = newNode;
		} else {
			lastNode->right = newNode;
		}
	}

	return newNode;
}

template <class TYPE>
void RBTree <TYPE>::treeRemove(Node <TYPE> *node){
	Node <TYPE> *x, *y;

	y = (node->left == null || node->right == null)? node : nextNode(node);

	x = (y->left != null)? y->left : y->right;

	x->parent = y->parent;
	if (y->parent == null){
		root = x;
	} else {
		if (y == y->parent->left){
			y->parent->left = x;
		} else {
			y->parent->right = x;
		}
	}
	if (y != node) node->object = y->object;
	if (y->color == BLACK) treeRemoveFixColors(x);

	delete y;
}

template <class TYPE>
void RBTree <TYPE>::treeRemoveFixColors(Node <TYPE> *node){
	Node <TYPE> *w;

	while (node != root && node->color == BLACK){
		if (node == node->parent->left){
			w = node->parent->right;
			if (w->color == RED){
				w->color = BLACK;
				node->parent->color = RED;
				leftRotate(node->parent);
				w = node->parent->right;
			}
			if (w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				node = node->parent;
			} else {
				if (w->right->color == BLACK){
					w->left->color = BLACK;
					w->color = RED;
					rightRotate(w);
					w = node->parent->right;
				}
				w->color = node->parent->color;
				node->parent->color = BLACK;
				w->right->color = BLACK;
				leftRotate(node->parent);
				node = root;
			}
		} else {
			w = node->parent->left;
			if (w->color == RED){
				w->color = BLACK;
				node->parent->color = RED;
				rightRotate(node->parent);
				w = node->parent->left;
			}
			if (w->left->color == BLACK && w->right->color == BLACK){
				w->color = RED;
				node = node->parent;
			} else {
				if (w->left->color == BLACK){
					w->right->color = BLACK;
					w->color = RED;
					leftRotate(w);
					w = node->parent->left;
				}
				w->color = node->parent->color;
				node->parent->color = BLACK;
				w->left->color = BLACK;
				rightRotate(node->parent);
				node = root;
			}
		}
	}
	node->color = BLACK;
}


template <class TYPE>
void RBTree <TYPE>::leftRotate(Node <TYPE> *node){
	Node <TYPE> *y = node->right;

	node->right = y->left;
	if (y->left != null) y->left->parent = node;

	y->parent = node->parent;
	if (node->parent == null){
		root = y;
	} else {
		if (node == node->parent->left){
			node->parent->left = y;
		} else {
			node->parent->right = y;
		}
	}
	y->left = node;
	node->parent = y;
}

template <class TYPE>
void RBTree <TYPE>::rightRotate(Node <TYPE> *node){
	Node <TYPE> *y = node->left;

	node->left = y->right;
	if (y->right != null) y->right->parent = node;

	y->parent = node->parent;
	if (node->parent == null){
		root = y;
	} else {
		if (node == node->parent->right){
			node->parent->right = y;
		} else {
			node->parent->left = y;
		}
	}
	y->right = node;
	node->parent = y;
}

#endif // _RBTREE_H_
