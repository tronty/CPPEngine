/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	//-----------------------------------------------------------------------------
// File: Framework\Util\ATIModel.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef _ATIMODEL_H_
#define _ATIMODEL_H_

#include <Framework3/IRenderer.h>
#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include "Mesh.h"
#include "Bone.h"

enum AnimationType{
	STATIC_GEOMETRY = 0,
	SKINNING_ANIMATION,
	SHAPE_ANIMATION,
	HIERARCHY_ANIMATION,
};

struct Material{
	char diffuseMap[64];
};

struct BlendData{
	__DWORD__ bone;
	D3DXFROMWINEVECTOR3 offset;
	float weight;
};

struct SkinningData{
	__DWORD__ numBones;
	BlendData* pBlendData;
};

struct Triangle{
	__DWORD__ v[3];
	D3DXFROMWINEVECTOR3 normal;
};

struct OffsetTable{
	__DWORD__ materialChunk;
	__DWORD__ vertexChunk;
	__DWORD__ triangleChunk;
	__DWORD__ meshChunk;
	__DWORD__ boneChunk;
};

class ATIModel : public Transform {
protected:
	int animationType;
	__DWORD__ numVertices;
	D3DXFROMWINEVECTOR3* pVPosition;
	D3DXFROMWINEVECTOR3* pVNormal;
	D3DXFROMWINEVECTOR3* pVTangent;
	D3DXFROMWINEVECTOR3* pVBinormal;
	D3DXFROMWINEVECTOR2* pVTexcoord;
	D3DXFROMWINEVECTOR4* pVColor;
	SkinningData* pVSkinningData;
	__DWORD__ numTriangles;
	Triangle* pTriangle;

	__DWORD__ numMaterials;
	Material* pMaterial;
	__DWORD__ numMeshs;
	Mesh* pMesh;
	Transform** ppMeshHie;
	__DWORD__ numBones;
	Bone* pBone;
	Transform** ppBoneHie;
	__DWORD__ traverseIndex;

public:
	ATIModel();
	~ATIModel();

	bool load(const char *name);
	void recursionMeshTraverse(Transform** lphie, Transform *lpct);
	void recursionBoneTraverse(Transform** lphie, Transform *lpct);
	void buildMeshHie();
	void buildBoneHie();
	bool play(float step = 1.0f, bool bloop = true);
	void updateVertices(D3DXFROMWINEVECTOR4 *pv);
	void clean();
	void setRange(float s, float e);
	void setTime(float t);

	Mesh &getMesh(const int index) const { return pMesh[index]; }
	__DWORD__ getVertexCount() const { return numVertices; };
	__DWORD__ getTriangleCount() const { return numTriangles; };
};

#endif // _ATIMODEL_H_
