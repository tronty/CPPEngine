/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\BSP.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _BSP_H_
#define _BSP_H_

#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include "VArray.h"

struct BTri {
	void setup();
	void split(BTri *dest, int &nPos, int &nNeg, const D3DXFROMWINEVECTOR3 &normal, const float offset, const float epsilon = 0) const;

	bool intersects(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1) const;
	bool isAbove(const D3DXFROMWINEVECTOR3 &pos) const;

	D3DXFROMWINEVECTOR3 v[3];
	D3DXFROMWINEVECTOR3 edgeNormals[3];
	float edgeOffsets[3];
	D3DXFROMWINEVECTOR3 normal;
	float offset;
	void *data;
};

struct BNode {
	~BNode();

	int getSize() const;

	void build(VArray_ <BTri> &tris, const int cutWeight, const int unbalanceWeight);
	void read(FILE *file);
	void write(FILE *file) const;

	bool intersects(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, const D3DXFROMWINEVECTOR3 &dir, D3DXFROMWINEVECTOR3 *point, const BTri **triangle) const;
	bool pushSphere(D3DXFROMWINEVECTOR3 &pos, const float radius) const;

	BNode *back;
	BNode *front;

	BTri tri;
};

class BSP {
public:
	BSP(){
		top = NULL;
	}
	~BSP(){
		delete top;
	}

	int getSize() const;

	void addTriangle(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, const D3DXFROMWINEVECTOR3 &v2, void *data = NULL);
	void build(const int cutWeight = 3, const int unbalanceWeight = 1);

	bool load(const char *fileName);
	bool save(const char *fileName) const;

	bool intersects(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, D3DXFROMWINEVECTOR3 *point = NULL, const BTri **triangle = NULL) const;
	bool pushSphere(D3DXFROMWINEVECTOR3 &pos, const float radius) const;

protected:
	VArray_ <BTri> tris;
	BNode *top;
};

#endif // _BSP_H_
