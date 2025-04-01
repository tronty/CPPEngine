/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\ATIModel.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/ATIModel.h>

#ifdef LINUX
#define LOG_PRINT(...) STX_PRINT(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#else
#define LOG_PRINT(...)
#define LOG_FNLN
#endif

ATIModel::ATIModel(){	
	animationType = STATIC_GEOMETRY;
	numVertices = 0;
	pVPosition = NULL;
	pVNormal = NULL;
	pVTangent = NULL;
	pVBinormal = NULL;
	pVTexcoord = NULL;
	pVColor = NULL;
	pVSkinningData = NULL;
	numTriangles = 0;
	pTriangle = NULL;

	numMaterials = 0;
	numMeshs = 0;
	numBones = 0;
	pMaterial = NULL;
	pMesh = NULL;
	ppMeshHie = NULL;
	pBone = NULL;
	ppBoneHie = NULL;
}

ATIModel::~ATIModel(){
	clean();
}

void ATIModel::clean(){
	delete [] pVPosition;
	delete [] pVNormal;
	delete [] pVTangent; 
	delete [] pVBinormal;
	delete [] pVTexcoord;
	delete [] pVColor;
	for (__DWORD__ v=0; v<numVertices; v++)
	{
		delete [] pVSkinningData[v].pBlendData;
	}
	delete [] pVSkinningData;
	delete [] pTriangle; 

	delete [] pMaterial;
	delete [] pMesh;
	delete [] pBone;
	delete [] ppMeshHie;
	delete [] ppBoneHie;
}

bool ATIModel::load(const char *name) {
	FILE *fp;
	__DWORD__ Version;
	OffsetTable offsetTable;

LOG_FNLN;
LOG_PRINT("fopen:%s\n", name);

	fp = fopen(name, "rb");
	if (!fp)
		return false;

	char Header[9] = "ATIMODEL";
	char ReadMark[9];

	fread(ReadMark, 1, 8, fp); 
	ReadMark[8] = '\0';
	if (stricmp(ReadMark, Header))
		goto ErrorExit;

	fread(&animationType, sizeof(int), 1, fp);
	fread(&Version, sizeof(__DWORD__), 1, fp);

	fread(&offsetTable, sizeof(OffsetTable), 1, fp);

	//fseek(fp, offsetTable.materialChunk, SEEK_SET);
	fread(&numMaterials, sizeof(__DWORD__), 1, fp);
	pMaterial = new Material[numMaterials];
	for (__DWORD__ i=0; i<numMaterials; i++)
	{
		fseek(fp, 64, SEEK_CUR);
		fseek(fp, sizeof(D3DXFROMWINEVECTOR4), SEEK_CUR);
		fseek(fp, sizeof(D3DXFROMWINEVECTOR4), SEEK_CUR);
		fseek(fp, sizeof(float), SEEK_CUR);
		fread(pMaterial[i].diffuseMap, 1, 64, fp);
		fseek(fp, 64, SEEK_CUR);
		fseek(fp, 64, SEEK_CUR);
		fseek(fp, 64, SEEK_CUR);
		fseek(fp, 64, SEEK_CUR);
	}

	//fseek(fp, offsetTable.vertexChunk, SEEK_SET);
	fread(&numVertices, sizeof(__DWORD__), 1, fp);
	pVPosition = new D3DXFROMWINEVECTOR3[numVertices];
	pVNormal = new D3DXFROMWINEVECTOR3[numVertices];
	pVTangent = new D3DXFROMWINEVECTOR3[numVertices];
	pVBinormal = new D3DXFROMWINEVECTOR3[numVertices];
	pVTexcoord = new D3DXFROMWINEVECTOR2[numVertices];
	pVColor = new D3DXFROMWINEVECTOR4[numVertices];
	pVSkinningData = new SkinningData[numVertices];
	for (__DWORD__ i=0; i<numVertices; i++)
	{
		pVSkinningData[i].pBlendData = NULL;
		pVSkinningData[i].numBones = 0;
	}
	fread(pVPosition, sizeof(D3DXFROMWINEVECTOR3), numVertices, fp);
	fread(pVNormal, sizeof(D3DXFROMWINEVECTOR3), numVertices, fp);
	fread(pVTangent, sizeof(D3DXFROMWINEVECTOR3), numVertices, fp);
	fread(pVBinormal, sizeof(D3DXFROMWINEVECTOR3), numVertices, fp);
	fread(pVTexcoord, sizeof(D3DXFROMWINEVECTOR2), numVertices, fp);
	fread(pVColor, sizeof(D3DXFROMWINEVECTOR4), numVertices, fp);
	for (__DWORD__ i=0; i<numVertices; i++)
	{
		fread(&pVSkinningData[i].numBones, sizeof(__DWORD__), 1, fp);
		if (pVSkinningData[i].numBones)
		{
			pVSkinningData[i].pBlendData = new BlendData[pVSkinningData[i].numBones];
			for (__DWORD__ b=0; b<pVSkinningData[i].numBones; b++)
			{
				fread(&pVSkinningData[i].pBlendData[b].bone, sizeof(__DWORD__), 1, fp);
				fread(&pVSkinningData[i].pBlendData[b].offset, sizeof(D3DXFROMWINEVECTOR3), 1, fp);
				fread(&pVSkinningData[i].pBlendData[b].weight, sizeof(float), 1, fp);
			}
		}
	}

	//fseek(fp, offsetTable.triangleChunk, SEEK_SET);
	fread(&numTriangles, sizeof(__DWORD__), 1, fp);
	pTriangle = new Triangle[numTriangles];
	for (__DWORD__ i=0; i<numTriangles; i++)
	{
		fread(&pTriangle[i].v[0], sizeof(__DWORD__), 1, fp);
		fread(&pTriangle[i].v[1], sizeof(__DWORD__), 1, fp);
		fread(&pTriangle[i].v[2], sizeof(__DWORD__), 1, fp);
		fread(&pTriangle[i].normal, sizeof(D3DXFROMWINEVECTOR3), 1, fp);
	}

	//fseek(fp, offsetTable.meshChunk, SEEK_SET);
	fread(&numMeshs, sizeof(__DWORD__), 1, fp);
	pMesh = new Mesh[numMeshs];
	for (__DWORD__ i=0; i<numMeshs; i++)
	{
		fseek(fp, 64, SEEK_CUR);
		fread(&pMesh[i].materialIndex, sizeof(int), 1, fp);
		fread(&pMesh[i].baseVertex, sizeof(__DWORD__), 1, fp);
		fread(&pMesh[i].numVertices, sizeof(__DWORD__), 1, fp);
		fread(&pMesh[i].baseTriangle, sizeof(__DWORD__), 1, fp);
		fread(&pMesh[i].numTriangles, sizeof(__DWORD__), 1, fp);

		fread(&pMesh[i].numBones, sizeof(__DWORD__), 1, fp);
		pMesh[i].pBone = new __DWORD__[pMesh[i].numBones];
		fread(pMesh[i].pBone, sizeof(__DWORD__), pMesh[i].numBones, fp);

		fread(&pMesh[i].parent, sizeof(int),1, fp);
		fread(&pMesh[i].numChildren, sizeof(__DWORD__), 1, fp);
		pMesh[i].pChildren = new __DWORD__[pMesh[i].numChildren];
		fread(pMesh[i].pChildren, sizeof(__DWORD__), pMesh[i].numChildren, fp);

		fread(&pMesh[i].numPrimitives, sizeof(__DWORD__), 1, fp);
		pMesh[i].pPrimitive = new Primitive[pMesh[i].numPrimitives];
		for (__DWORD__ p=0; p<pMesh[i].numPrimitives; p++)
		{
			fread(&pMesh[i].pPrimitive[p].Type, sizeof(int), 1, fp);
			fread(&pMesh[i].pPrimitive[p].numIndices, sizeof(__DWORD__), 1, fp);
			pMesh[i].pPrimitive[p].pIndices = new __DWORD__[pMesh[i].pPrimitive[p].numIndices];
			fread(pMesh[i].pPrimitive[p].pIndices, sizeof(__DWORD__), pMesh[i].pPrimitive[p].numIndices, fp);
		}

		fread(&pMesh[i].animation.positionKeys.numKeys, sizeof(__DWORD__), 1, fp);
		pMesh[i].animation.positionKeys.pKey = new AnimationKey[pMesh[i].animation.positionKeys.numKeys];
		for (__DWORD__ p=0; p<pMesh[i].animation.positionKeys.numKeys; p++)
		{
			fread(&pMesh[i].animation.positionKeys.pKey[p].startTime, sizeof(int), 1, fp);
			fread(&pMesh[i].animation.positionKeys.pKey[p].endTime, sizeof(int), 1, fp);
			fread(pMesh[i].animation.positionKeys.pKey[p].value, sizeof(float), 3, fp);
		}
		fread(&pMesh[i].animation.rotationKeys.numKeys, sizeof(__DWORD__), 1, fp);
		pMesh[i].animation.rotationKeys.pKey = new AnimationKey[pMesh[i].animation.rotationKeys.numKeys];
		for (__DWORD__ p=0; p<pMesh[i].animation.rotationKeys.numKeys; p++)
		{
			fread(&pMesh[i].animation.rotationKeys.pKey[p].startTime, sizeof(int), 1, fp);
			fread(&pMesh[i].animation.rotationKeys.pKey[p].endTime, sizeof(int), 1, fp);
			fread(pMesh[i].animation.rotationKeys.pKey[p].value, sizeof(float), 4, fp);
		}
		fread(&pMesh[i].animation.scaleKeys.numKeys, sizeof(__DWORD__), 1, fp);
		pMesh[i].animation.scaleKeys.pKey = new AnimationKey[pMesh[i].animation.scaleKeys.numKeys];
		for (__DWORD__ p=0; p<pMesh[i].animation.scaleKeys.numKeys; p++)
		{
			fread(&pMesh[i].animation.scaleKeys.pKey[p].startTime, sizeof(int), 1, fp);
			fread(&pMesh[i].animation.scaleKeys.pKey[p].endTime, sizeof(int), 1, fp);
			fread(pMesh[i].animation.scaleKeys.pKey[p].value, sizeof(float), 3, fp);
		}
	}

	fread(&numBones, sizeof(__DWORD__), 1, fp);
	pBone = new Bone[numBones];
	for (__DWORD__ i=0; i<numBones; i++)
	{
		fseek(fp, 64, SEEK_CUR);
		fread(&pBone[i].parent, sizeof(int),1, fp);
		fread(&pBone[i].numChildren, sizeof(__DWORD__), 1, fp);
		if (pBone[i].numChildren)
		{
			pBone[i].pChildren = new __DWORD__[pBone[i].numChildren];
			fread(pBone[i].pChildren, sizeof(__DWORD__), pBone[i].numChildren, fp);
		}
		fread(&pBone[i].animation.positionKeys.numKeys, sizeof(__DWORD__), 1, fp);
		pBone[i].animation.positionKeys.pKey = new AnimationKey[pBone[i].animation.positionKeys.numKeys];
		for (__DWORD__ p=0; p<pBone[i].animation.positionKeys.numKeys; p++)
		{
			fread(&pBone[i].animation.positionKeys.pKey[p].startTime, sizeof(int), 1, fp);
			fread(&pBone[i].animation.positionKeys.pKey[p].endTime, sizeof(int), 1, fp);
			fread(pBone[i].animation.positionKeys.pKey[p].value, sizeof(float), 3, fp);
		}
		fread(&pBone[i].animation.rotationKeys.numKeys, sizeof(__DWORD__), 1, fp);
		pBone[i].animation.rotationKeys.pKey = new AnimationKey[pBone[i].animation.rotationKeys.numKeys];
		for (__DWORD__ p=0; p<pBone[i].animation.rotationKeys.numKeys; p++)
		{
			fread(&pBone[i].animation.rotationKeys.pKey[p].startTime, sizeof(int), 1, fp);
			fread(&pBone[i].animation.rotationKeys.pKey[p].endTime, sizeof(int), 1, fp);
			fread(pBone[i].animation.rotationKeys.pKey[p].value, sizeof(float), 4, fp);
		}
		fread(&pBone[i].animation.scaleKeys.numKeys, sizeof(__DWORD__), 1, fp);
		pBone[i].animation.scaleKeys.pKey = new AnimationKey[pBone[i].animation.scaleKeys.numKeys];
		for (__DWORD__ p=0; p<pBone[i].animation.scaleKeys.numKeys; p++)
		{
			fread(&pBone[i].animation.scaleKeys.pKey[p].startTime, sizeof(int), 1, fp);
			fread(&pBone[i].animation.scaleKeys.pKey[p].endTime, sizeof(int), 1, fp);
			fread(pBone[i].animation.scaleKeys.pKey[p].value, sizeof(float), 3, fp);
		}
	}

	fclose(fp);

	if (numMeshs)
	{
		ppMeshHie = (Transform**)malloc(sizeof(Transform*)*numMeshs);
		buildMeshHie();
	}

	if (numBones)
	{
		ppBoneHie = (Transform**)malloc(sizeof(Transform*)*numBones);
		buildBoneHie();
	}

	return true;

ErrorExit:
	fclose(fp);
	return false;
}

void ATIModel::recursionMeshTraverse(Transform** lphie, Transform *lpct) {
	lphie[traverseIndex++] = lpct;
	for (__DWORD__ i=0; i<lpct->numChildren; i++)
	{
		recursionMeshTraverse(lphie, &pMesh[lpct->pChildren[i]]);
	}	
}

void ATIModel::buildMeshHie() {
	__DWORD__ i;
	for (i=0; i<numMeshs; i++)
	{
		if (pMesh[i].parent == -1)
			break;
	}
	traverseIndex = 0;
	recursionMeshTraverse(ppMeshHie, &pMesh[i]);
}

void ATIModel::recursionBoneTraverse(Transform** lphie, Transform *lpct) {
	lphie[traverseIndex++] = lpct;
	for (__DWORD__ i=0; i<lpct->numChildren; i++)
	{
		recursionBoneTraverse(lphie, &pBone[lpct->pChildren[i]]);
	}	
}

void ATIModel::buildBoneHie() {
	__DWORD__ i;
	if (numBones)
	{
		for (i=0; i<numBones; i++)
		{
			if (pBone[i].parent == -1)
				break;
		}
		traverseIndex = 0;
		recursionBoneTraverse(ppBoneHie, &pBone[i]);
	}
}

bool ATIModel::play(float step, bool bloop) {
	Transform::play(step, bloop);
	switch (animationType)
	{
		case STATIC_GEOMETRY:
			break;

		case SKINNING_ANIMATION:
			for (__DWORD__ i=0; i<numBones; i++)
			{
				ppBoneHie[i]->play(step, bloop);
				if (ppBoneHie[i]->parent == -1)
					ppBoneHie[i]->setTransformMatrix(ppBoneHie[i]->animation.getTransformMatrix() * transformMatrix);			
				else
					ppBoneHie[i]->setTransformMatrix(ppBoneHie[i]->animation.getTransformMatrix() * pBone[ppBoneHie[i]->parent].getTransformMatrix());
			}
			break;

		case HIERARCHY_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				ppMeshHie[i]->play(step, bloop);
				if (ppMeshHie[i]->parent == -1)
					ppMeshHie[i]->setTransformMatrix(ppMeshHie[i]->animation.getTransformMatrix() * transformMatrix);			
				else
					ppMeshHie[i]->setTransformMatrix(ppMeshHie[i]->animation.getTransformMatrix() * pMesh[ppMeshHie[i]->parent].getTransformMatrix());
			}
			break;

		case SHAPE_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				ppMeshHie[i]->play(step, bloop);
				if (ppMeshHie[i]->parent == -1)
					ppMeshHie[i]->setTransformMatrix(ppMeshHie[i]->animation.getTransformMatrix() * transformMatrix);			
				else
					ppMeshHie[i]->setTransformMatrix(ppMeshHie[i]->animation.getTransformMatrix() * pMesh[ppMeshHie[i]->parent].getTransformMatrix());
			}
			break;
	}
	return true;
}

void ATIModel::setRange(float s, float e) {
	Transform::setRange(s, e);
	switch (animationType)
	{
		case STATIC_GEOMETRY:
			break;

		case SKINNING_ANIMATION:
			for (__DWORD__ i=0; i<numBones; i++)
			{
				pBone[i].setRange(s, e);
			}
			break;

		case HIERARCHY_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				pMesh[i].setRange(s, e);
			}
			break;

		case SHAPE_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				pMesh[i].setRange(s, e);
			}
			break;
	}
}

void ATIModel::setTime(float t) {
	Transform::setTime(t);
	switch (animationType)
	{
		case STATIC_GEOMETRY:
			break;

		case SKINNING_ANIMATION:
			for (__DWORD__ i=0; i<numBones; i++)
			{
				pBone[i].setTime(t);
			}
			break;

		case HIERARCHY_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				pMesh[i].setTime(t);
			}
			break;

		case SHAPE_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				pMesh[i].setTime(t);
			}
			break;
	}
}

void ATIModel::updateVertices(D3DXFROMWINEVECTOR4 *pv) {
	D3DXFROMWINEVECTOR4 P;
	switch (animationType)
	{
		case STATIC_GEOMETRY:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				for (__DWORD__ v=pMesh[i].baseVertex; v<pMesh[i].baseVertex+pMesh[i].numVertices; v++)
				{
					pv[v].x = pVPosition[v].x;
					pv[v].y = pVPosition[v].y;
					pv[v].z = pVPosition[v].z;
					pv[i].w = 1.0f;
				}
			}
			break;

		case SKINNING_ANIMATION:
			for (__DWORD__ i=0; i<numVertices; i++)
			{
				if (pVSkinningData[i].numBones)
				{
					if (pVSkinningData[i].numBones > 1)
					{
						pv[i].x = pv[i].y = pv[i].z = 0.0f;
						for (__DWORD__ j=0; j<pVSkinningData[i].numBones; j++)
						{
							D3DXFROMWINEVECTOR4 P(pVSkinningData[i].pBlendData[j].offset, 1.0f);
#if 0
							P = transpose(pBone[pVSkinningData[i].pBlendData[j].bone].getTransformMatrix()) * P;
#else
D3DXFROMWINEMATRIX M=transpose(pBone[pVSkinningData[i].pBlendData[j].bone].getTransformMatrix());
D3DXFROMWINEVec4TransformCoord(&P, &P, &M);
#endif

							P *= pVSkinningData[i].pBlendData[j].weight;
							pv[i] += P;
						}
						pv[i].w = 1.0f;
					}
					else
					{
						D3DXFROMWINEVECTOR4 P(pVSkinningData[i].pBlendData[0].offset, 1.0f);
#if 0
						pv[i] = transpose(pBone[pVSkinningData[i].pBlendData[0].bone].getTransformMatrix()) * P;
#else
D3DXFROMWINEMATRIX M=transpose(pBone[pVSkinningData[i].pBlendData[0].bone].getTransformMatrix());
D3DXFROMWINEVec4TransformCoord(&pv[i], &P, &M);
#endif
						pv[i].w = 1.0f;
					}
				}
			}
			break;

		case HIERARCHY_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				for (__DWORD__ v=pMesh[i].baseVertex; v<pMesh[i].baseVertex+pMesh[i].numVertices; v++)
				{
					D3DXFROMWINEVECTOR4 P(pVPosition[v], 1.0f);
#if 0
					pv[v] = transpose(pMesh[i].getTransformMatrix()) * P;
#else
D3DXFROMWINEMATRIX M=transpose(pMesh[i].getTransformMatrix());
D3DXFROMWINEVec4TransformCoord(&pv[v], &P, &M);
#endif
					pv[i].w = 1.0f;
				}
			}
			break;

		case SHAPE_ANIMATION:
			for (__DWORD__ i=0; i<numMeshs; i++)
			{
				for (__DWORD__ v=pMesh[i].baseVertex; v<pMesh[i].baseVertex+pMesh[i].numVertices; v++)
				{
					D3DXFROMWINEVECTOR4 P(pVPosition[v], 1.0f);
#if 0
					pv[v] = transpose(pMesh[i].getTransformMatrix()) * P;
#else
D3DXFROMWINEMATRIX M=transpose(pMesh[i].getTransformMatrix());
D3DXFROMWINEVec4TransformCoord(&pv[v], &P, &M);
#endif
					pv[i].w = 1.0f;
				}
			}
			break;
	}
}
