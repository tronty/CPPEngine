//-----------------------------------------------------------------------------
// File: Octree.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
#define D3D_OVERLOADS

//include <tchar.h>
#include <stdio.h>
#include "Helper.h"
#include "Octree.h"
#include "DXErrors.h"

COctreeNode::COctreeNode(FILE *fp)
{
	ReadTree(fp);
}

COctreeNode::~COctreeNode()
{
	if (m_pKids[0] != 0)
	{
		for(__DWORD__ i = 0; i < 8; i++)
			delete m_pKids[i];
	}
	else
	{
		//this node is a leaf so just delete its poly lists
		delete [] m_pTriangleLists;
	}
}

void COctreeNode::ReadTree(FILE *fp)
{
	CHAR type[5];

	fread(type, sizeof(CHAR), 4, fp);

	type[4]='\0';

	fread(&m_vCenter, sizeof(D3DXFROMWINEVECTOR3), 1, fp);
	fread(&m_fWidth, sizeof(float), 1, fp);
	fread(&m_fHeight, sizeof(float), 1, fp);
	fread(&m_fDepth, sizeof(float), 1, fp);
	fread(&m_dwListCount, sizeof(__DWORD__), 1, fp);

	__DWORD__ i;
	float wd2 = m_fWidth / 2.0f;
	float hd2 = m_fHeight / 2.0f;
	float dd2 = m_fDepth / 2.0f;
	for(i = 0; i < 8; i++)
	{
		//compute the location of the current corner
		m_vCorners[i].x = m_vCenter.x + ((i & 0x1) ? wd2 : -wd2);
		m_vCorners[i].y = m_vCenter.y + ((i & 0x2) ? hd2 : -hd2);
		m_vCorners[i].z = m_vCenter.z + ((i & 0x4) ? dd2 : -dd2);
	}

	if (!strcmp(type, "LEAF"))
	{ 
		//node is a leaf in the tree read its polygon
		//lists and set its children to 0
		m_pTriangleLists = new CTriangleList[m_dwListCount];

		for(i = 0; i < m_dwListCount; i++)
			m_pTriangleLists[i].Load(fp);

		for(i = 0; i < 8; i++)
		{
			m_pKids[i] = 0;
		}
	}
	else if (!strcmp(type, "NODE"))
	{
		//the node is non-leaf, so read its children
		for(i = 0; i < 8; i++)
		{
			m_pKids[i] = new COctreeNode(fp);
		}
	}
}

void COctreeNode::Cull(CRenderQueue *pRQ, CLIPVOLUME& cv, D3DXFROMWINEVECTOR3& pos)
{
	__DWORD__ zones[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	float x, y, z;
	__DWORD__ i;

	for(i = 0; i < 8; i++)
	{
		x = m_vCorners[i].x;
		y = m_vCorners[i].y;
		z = m_vCorners[i].z;

		if ((cv.pNear.a * x + cv.pNear.b * y + cv.pNear.c * z + cv.pNear.d) > -0.01f)
			zones[i] |= 0x01;
		else if ((cv.pFar.a * x + cv.pFar.b * y + cv.pFar.c * z + cv.pFar.d) > -0.01f)
			zones[i] |= 0x02;

		if ((cv.pLeft.a * x + cv.pLeft.b * y + cv.pLeft.c * z + cv.pLeft.d) > -0.01f)
			zones[i] |= 0x04;
		else if ((cv.pRight.a * x + cv.pRight.b * y + cv.pRight.c * z + cv.pRight.d) > -0.01f)
			zones[i] |= 0x08;

		if ((cv.pTop.a * x + cv.pTop.b * y + cv.pTop.c * z + cv.pTop.d) > -0.01f)
			zones[i] |= 0x10;
		else if ((cv.pBottom.a * x + cv.pBottom.b * y + cv.pBottom.c * z + cv.pBottom.d) > -0.01f)
			zones[i] |= 0x20;
	}

	//if all of the corners are outside of the boundaries
	// this node is excluded, so stop traversing
	if (zones[0] & zones[1] & zones[2] & zones[3] & zones[4] & zones[5] & zones[6] & zones[7])
		return;

	// if this is a leaf add the triangle lists to the render queue
	if (m_pKids[0] == 0)
	{
		for(i = 0; i < m_dwListCount; i++)
			pRQ->AddToQueue(&m_pTriangleLists[i]);
	}
	else 
	{
		//this is not a leaf traverse deeper
		for(i = 0; i < 8; i++)
			m_pKids[i]->Cull(pRQ, cv, pos);
	}
}
