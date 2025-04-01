/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: ProgrammerArt.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/ProgrammerArt.h>

Model *createCube(const float sizeX, const float sizeY, const float sizeZ, const uint flags){
	static const D3DXFROMWINEVECTOR3 verts[] = {
		D3DXFROMWINEVECTOR3(-sizeX, -sizeY, -sizeZ), D3DXFROMWINEVECTOR3(-sizeX, -sizeY, sizeZ), D3DXFROMWINEVECTOR3(-sizeX, sizeY, -sizeZ), D3DXFROMWINEVECTOR3(-sizeX, sizeY, sizeZ),
		D3DXFROMWINEVECTOR3( sizeX, -sizeY, -sizeZ), D3DXFROMWINEVECTOR3( sizeX, -sizeY, sizeZ), D3DXFROMWINEVECTOR3( sizeX, sizeY, -sizeZ), D3DXFROMWINEVECTOR3( sizeX, sizeY, sizeZ),
	};
	static const D3DXFROMWINEVECTOR3 norms[] = { D3DXFROMWINEVECTOR3(-1, 0, 0), D3DXFROMWINEVECTOR3(0, 0, 1), D3DXFROMWINEVECTOR3(1, 0, 0), D3DXFROMWINEVECTOR3(0, 0, -1), D3DXFROMWINEVECTOR3(0, 1, 0), D3DXFROMWINEVECTOR3(0, -1, 0) };
	static const uint indices[] = {
		0, 1, 3, 2,
		1, 5, 7, 3,
		5, 4, 6, 7,
		4, 0, 2, 6,
		3, 7, 6, 2,
		5, 1, 0, 4,
	};

	int vertexSize = 3;
	if (flags & NORMALS)   vertexSize += 3;
	if (flags & TEXCOORDS) vertexSize += 2;

	float *vertices = new float[36 * vertexSize];
	float *dest = vertices;
	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 6; j++){
			static int ind[] = { 0, 1, 2, 0, 2, 3 };
			int k = ind[j];

			*((D3DXFROMWINEVECTOR3 *) dest) = verts[indices[4 * i + k]];
			dest += 3;
			if (flags & NORMALS){
				*((D3DXFROMWINEVECTOR3 *) dest) = norms[i];
				dest += 3;
			}
			if (flags & TEXCOORDS){
				*((D3DXFROMWINEVECTOR2 *) dest) = D3DXFROMWINEVECTOR2((k >= 1) && (k <= 2), (k > 1));
				dest += 2;
			}
		}
	}


	Model *model = new Model();
	model->setVBData(vertices, 36);
	model->addFormat(TYPE_VERTEX, 3);
	if (flags & NORMALS)   model->addFormat(TYPE_NORMAL,   3);
	if (flags & TEXCOORDS) model->addFormat(TYPE_TEXCOORD, 2);
	model->addBatch(0, 0, 0, 36);

	return model;
}


D3DXFROMWINEVECTOR3 *dest;

void subDivide(const D3DXFROMWINEVECTOR3 &v0, const D3DXFROMWINEVECTOR3 &v1, const D3DXFROMWINEVECTOR3 &v2, int level){
	if (level){
		level--;
		D3DXFROMWINEVECTOR3 v3 = normalize(v0 + v1);
		D3DXFROMWINEVECTOR3 v4 = normalize(v1 + v2);
		D3DXFROMWINEVECTOR3 v5 = normalize(v2 + v0);

		subDivide(v0, v3, v5, level);
		subDivide(v3, v4, v5, level);
		subDivide(v3, v1, v4, level);
		subDivide(v5, v4, v2, level);
	} else {
		*dest++ = v0;
		*dest++ = v1;
		*dest++ = v2;
	}
}

Model *createSphere(const int subDivLevel, const float radius){
	int nVertices = 4 * 3 * (1 << (2 * subDivLevel));

	D3DXFROMWINEVECTOR3 *vertices = new D3DXFROMWINEVECTOR3[nVertices];

	float e = 1.0f / (1 + sqrtf(2));
	float a = sqrtf(1 - e * e);
	float b = 1 + e - a;
	float d = sqrtf(1 - e * e - b * b);


	D3DXFROMWINEVECTOR3 y = D3DXFROMWINEVECTOR3(0, 1, 0);
	D3DXFROMWINEVECTOR3 x = D3DXFROMWINEVECTOR3(a, -e, 0);
	D3DXFROMWINEVECTOR3 z0 = D3DXFROMWINEVECTOR3(-b, -e,  d);
	D3DXFROMWINEVECTOR3 z1 = D3DXFROMWINEVECTOR3(-b, -e, -d);

	dest = vertices;
	subDivide(y, z0, x,  subDivLevel);
	subDivide(y, z1, z0, subDivLevel);
	subDivide(y, x,  z1, subDivLevel);
	subDivide(x, z0, z1, subDivLevel);

	if (radius != 1){
		for (int i = 0; i < nVertices; i++){
			vertices[i] *= radius;
		}
	}

	Model *model = new Model();
	model->setVBData(vertices, nVertices);
	model->addFormat(TYPE_VERTEX, 3);
	model->addBatch(0, 0, 0, nVertices);

	return model;
}

Model *createDisc(const int nSlices, const int nRings, const float radius, uint flags, ZFunc zFunc){
	int centerSample = (flags & CENTER_SAMPLE);
	int nComponents = (flags & TWO_COMPONENTS)? 2 : 3;

	static const int index[] = { 2, 1, 0, 2, 1 };
	int ind = ((flags >> 2) & 0x3);
	int xIndex = index[ind + 2];
	int yIndex = index[ind + 1];
	int zIndex = index[ind];

	int nVertices = centerSample + nRings * nSlices;
	uint nIndices = nRings * 2 * (nSlices + 1);

	float *vertices = new float[nVertices * nComponents];
	float *vDest = vertices;
	if (centerSample){
		// Center vertex
		vDest[xIndex] = 0;
		vDest[yIndex] = 0;
		if (nComponents > 2){
			if (zFunc) vDest[zIndex] = zFunc(0, 0); else vDest[zIndex] = 0;
		}
		vDest += nComponents;
	}

	uint *indices = new uint[nIndices];
	uint *iDest = indices;

	for (int ring = 0; ring < nRings; ring++){
		float r = (ring + 1) * radius / nRings;
		for (int slice = 0; slice < nSlices; slice++){
			float a = slice * 2 * D3DXFROMWINE_PI / nSlices;

			float x = r * cosf(a);
			float y = r * sinf(a);

			vDest[xIndex] = x;
			vDest[yIndex] = y;
			if (nComponents > 2){
				if (zFunc) vDest[zIndex] = zFunc(x, y); else vDest[zIndex] = 0;
			}
			vDest += nComponents;

			*iDest++ = (ring > 0)? nSlices * (ring - 1) + slice + centerSample : 0;
			*iDest++ = nSlices * ring + slice + centerSample ;
		}
		*iDest++ = (ring > 0)? nSlices * (ring - 1) + centerSample : 0;
		*iDest++ = nSlices * ring + centerSample;
	}

	Model *model = new Model();
	model->setVBData(vertices, nVertices);
	model->setIBData(indices, nIndices, sizeof(uint));
	model->addFormat(TYPE_VERTEX, nComponents);
	model->addBatch(0, nIndices, 0, nVertices - 1);

	return model;
}

template <typename INT_TYPE>
void fillRectangleIB(INT_TYPE *dest, const int width, const int height){
	for (int y = 0; y < height - 1; y++){
		if (y & 1){
			*dest++ = y * width + width - 1;
			for (int x = width - 1; x >= 0; x--){
				*dest++ = y * width + x;
				*dest++ = (y + 1) * width + x;
			}
		} else {
			*dest++ = y * width;
			for (int x = 0; x < width; x++){
				*dest++ = y * width + x;
				*dest++ = (y + 1) * width + x;
			}
		}
	}
}

// Instantiate for ushort and uint
template void fillRectangleIB(unsigned short *dest, const int width, const int height);
template void fillRectangleIB(unsigned int *dest, const int width, const int height);
