/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\Model.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/Model.h>

#undef LOG_FNLN
#undef LOG_PRINT

Model::Model(){
	vertices = NULL;
	indices = NULL;
	vertexSize = indexSize = 0;
	nVertices = nIndices = 0;
	vertexBuffer = VB_NONE;
	indexBuffer  = IB_NONE;

	attribs = NULL;
	nAttribs = 0;
}

Model::~Model(){
	clean();
}

struct Header {
	uint32 version;
	uint32 nBatches;
	uint32 nVertices;
	uint32 nIndices;
	uint32 vertexSize;
	uint32 indexSize;
};

bool Model::loadHmdl(const char *fileName){

//LOG_FNLN;
//LOG_PRINT("fopen:%s\n", fileName);
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) return false;

	unsigned int version, nBatches;
	fread(&version,  sizeof(version),  1, file);
	fread(&nBatches, sizeof(nBatches), 1, file);

	for (unsigned int i = 0; i < nBatches; i++){
		unsigned int nVertices, nIndices;

		fread(&nVertices,  sizeof(nVertices),  1, file);
		fread(&nIndices,   sizeof(nIndices),   1, file);
		fread(&vertexSize, sizeof(vertexSize), 1, file);
		fread(&indexSize,  sizeof(indexSize),  1, file);

		// Skip primitive and format data
		fseek(file, 4, SEEK_CUR);
		unsigned int nFormats;
		fread(&nFormats, sizeof(nFormats), 1, file);
		fseek(file, nFormats * 20, SEEK_CUR);

		unsigned char *vertices = new unsigned char[nVertices * vertexSize];
		fread(vertices, nVertices * vertexSize, 1, file);

		if (nIndices > 0){
			unsigned short *indices = new unsigned short[nIndices];
			fread(indices, nIndices * indexSize, 1, file);

			appendBatch(vertices, nVertices, indices, nIndices);

			delete indices;
		} else {
			appendBatch(vertices, nVertices);
		}
		delete vertices;
	}

	fclose(file);

	return true;
}

bool Model::load(const char *fileName){
//LOG_FNLN;
//LOG_PRINT("fopen:%s\n", fileName);
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) return false;

	Header header;
	fread(&header, sizeof(header), 1, file);
	if (header.version != 0){
		fclose(file);
		return false;
	}

	nVertices = header.nVertices;
	nIndices  = header.nIndices;
	vertexSize = header.vertexSize;
	indexSize  = header.indexSize;

	batches.setCount(header.nBatches);
	fread(batches.getArray(), sizeof(Batch), header.nBatches, file);

	vertices = new unsigned char[vertexSize * nVertices];
	fread(vertices, vertexSize, nVertices, file);
	if (nIndices > 0){
		indices = new unsigned char[indexSize * nIndices];
		fread(indices, indexSize, nIndices, file);
	}
	fclose(file);

	return true;
}

bool Model::save(const char *fileName) const {
//LOG_FNLN;
//LOG_PRINT("fopen:%s\n", fileName);
	FILE *file = fopen(fileName, "wb");
	if (file == NULL) return false;

	Header header;
	header.version = 0;
	header.nBatches = batches.getCount();
	header.nVertices = nVertices;
	header.nIndices  = nIndices;
	header.vertexSize = vertexSize;
	header.indexSize  = indexSize;
	fwrite(&header, sizeof(header), 1, file);

	for (unsigned int i = 0; i < batches.getCount(); i++){
		fwrite(&batches[i], sizeof(Batch), 1, file);
	}

	fwrite(vertices, vertexSize, nVertices, file);
	fwrite(indices, indexSize, nIndices, file);
	fclose(file);

	return true;
}

bool Model::makeDrawable(){
	if ((vertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(nVertices * vertexSize, STATIC, vertices)) == VB_NONE) return false;
	if (nIndices > 0){
		if ((indexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(nIndices, indexSize, STATIC, indices)) == IB_NONE) return false;
	}

	// ??? if ((vertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(attribs, nAttribs, -1 /* ??? */ )) == VF_NONE) return false;

	return true;
}

void Model::clean(){
	delete attribs;
	delete vertices;
	delete indices;
}

void Model::addBatch(const unsigned int firstIndex, const unsigned int nIndices, const unsigned int minVertex, const unsigned int maxVertex){
	Batch batch;
	batch.firstIndex = firstIndex;
	batch.nIndices = nIndices;

	if (minVertex == 0 && maxVertex == 0){
		computeRange(batch);
	} else {
		batch.firstVertex = minVertex;
		batch.nVertices = maxVertex - minVertex + 1;
	}

	batches.add(batch);
}

void Model::setBatch(const unsigned int batch, const unsigned int firstIndex, const unsigned int nIndices, const unsigned int minVertex, const unsigned int maxVertex){
	batches[batch].firstIndex = firstIndex;
	batches[batch].nIndices = nIndices;

	if (minVertex == 0 && maxVertex == 0){
		computeRange(batches[batch]);
	} else {
		batches[batch].firstVertex;
		batches[batch].nVertices = maxVertex - minVertex + 1;
	}
}

bool Model::removeBatch(const unsigned int batch){
	if (batch >= batches.getCount()) return false;

	batches.orderedRemove(batch);

	return true;
}

void Model::appendBatch(const void *verts, const unsigned int nVerts, const unsigned short *inds, const unsigned int nInds){
	vertices = (unsigned char *) realloc(vertices, (nVertices + nVerts) * vertexSize);
	stx_memcpy(vertices + nVertices * vertexSize, verts, nVerts * vertexSize);

	if (indexSize > 0){
		indices = (unsigned char *) realloc(indices, (nIndices + nInds) * indexSize);
		if (indexSize == 2){
			unsigned short *dest = ((unsigned short *) indices) + nIndices;
			for (unsigned int i = 0; i < nInds; i++){
				dest[i] = nVertices + inds[i];
			}
		} else {
			unsigned int *dest = ((unsigned int *) indices) + nIndices;
			for (unsigned int i = 0; i < nInds; i++){
				dest[i] = nVertices + inds[i];
			}
		}

		addBatch(nIndices, nInds);
		nIndices += nInds;
	} else {
		Batch batch;
		batch.firstIndex = 0;
		batch.nIndices = 0;
		batch.firstVertex = nVertices;
		batch.nVertices = nVerts;

		batches.add(batch);
	}

	nVertices += nVerts;
}

void Model::appendFormat(const AttributeType attType, const unsigned int size){
	unsigned int newVertexSize = vertexSize + size * sizeof(float);
	unsigned char *newVertices = new unsigned char[newVertexSize * nVertices];

	for (unsigned int i = 0; i < nVertices; i++){
		stx_memcpy(newVertices + i * newVertexSize, vertices + i * vertexSize, vertexSize);
		stx_memset(newVertices + i * newVertexSize + vertexSize, 0, size * sizeof(float));
	}

	delete vertices;
	vertices = newVertices;
	addFormat(attType, size);
}

void Model::addNormals(){
	unsigned int newVertexSize = vertexSize + sizeof(D3DXFROMWINEVECTOR3);
	unsigned char *newVertices = new unsigned char[nVertices * newVertexSize];
	stx_memset(newVertices, 0, nVertices * newVertexSize);

	unsigned int vIndex = 0;
	for (unsigned int i = 0; i < nAttribs; i++){
		if (attribs[i].type == TYPE_VERTEX) break;
		vIndex += attribs[i].size * sizeof(float);
	}

	if (indexSize == 2){
		unsigned short *inds = (unsigned short *) indices;

		for (unsigned int i = 0; i < nIndices; i += 3){
			D3DXFROMWINEVECTOR3 v0 = *(D3DXFROMWINEVECTOR3 *) (vertices + vertexSize * inds[i    ] + vIndex);
			D3DXFROMWINEVECTOR3 v1 = *(D3DXFROMWINEVECTOR3 *) (vertices + vertexSize * inds[i + 1] + vIndex);
			D3DXFROMWINEVECTOR3 v2 = *(D3DXFROMWINEVECTOR3 *) (vertices + vertexSize * inds[i + 2] + vIndex);

			D3DXFROMWINEVECTOR3 normal = normalize(cross(v1 - v0, v2 - v0));

			*(D3DXFROMWINEVECTOR3 *) (newVertices + newVertexSize * inds[i    ] + vertexSize) += normal;
			*(D3DXFROMWINEVECTOR3 *) (newVertices + newVertexSize * inds[i + 1] + vertexSize) += normal;
			*(D3DXFROMWINEVECTOR3 *) (newVertices + newVertexSize * inds[i + 2] + vertexSize) += normal;
		}
	} else {
		unsigned int *inds = (unsigned int *) indices;

		for (unsigned int i = 0; i < nIndices; i += 3){
			D3DXFROMWINEVECTOR3 v0 = *(D3DXFROMWINEVECTOR3 *) (vertices + vertexSize * inds[i    ] + vIndex);
			D3DXFROMWINEVECTOR3 v1 = *(D3DXFROMWINEVECTOR3 *) (vertices + vertexSize * inds[i + 1] + vIndex);
			D3DXFROMWINEVECTOR3 v2 = *(D3DXFROMWINEVECTOR3 *) (vertices + vertexSize * inds[i + 2] + vIndex);

			D3DXFROMWINEVECTOR3 normal = normalize(cross(v1 - v0, v2 - v0));

			*(D3DXFROMWINEVECTOR3 *) (newVertices + newVertexSize * inds[i    ] + vertexSize) += normal;
			*(D3DXFROMWINEVECTOR3 *) (newVertices + newVertexSize * inds[i + 1] + vertexSize) += normal;
			*(D3DXFROMWINEVECTOR3 *) (newVertices + newVertexSize * inds[i + 2] + vertexSize) += normal;
		}
	}

	for (unsigned int i = 0; i < nVertices; i++){
		stx_memcpy(newVertices + i * newVertexSize, vertices + i * vertexSize, vertexSize);
		D3DXFROMWINEVECTOR3 *norm = (D3DXFROMWINEVECTOR3 *) (newVertices + newVertexSize * i + vertexSize);
		*norm = normalize(*norm);
	}

	delete vertices;
	vertices = newVertices;

	addFormat(TYPE_NORMAL, 3);
}

void Model::computeRange(Batch &batch){
	uint minVertex = 0xFFFFFFFF;
	uint maxVertex = 0;

	uint last = batch.firstIndex + batch.nIndices;
	for (uint i = batch.firstIndex; i < last; i++){
		uint index;
		if (indexSize == 2){
			index = ((ushort *) indices)[i];
		} else {
			index = ((uint *) indices)[i];
		}
		if (index < minVertex) minVertex = index;
		if (index > maxVertex) maxVertex = index;
	}

	batch.firstVertex = minVertex;
	batch.nVertices = maxVertex - minVertex + 1;
}

void Model::draw(const Primitives primitives){
	for (unsigned int i = 0; i < batches.getCount(); i++){
		drawBatch(i, primitives);
	}
}

void Model::drawBatch(const unsigned int batch, const Primitives primitives){
	IRenderer::GetRendererInstance()->setVertexFormat(vertexFormat);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, vertexBuffer);
	if (batches[batch].nIndices > 0){
		IRenderer::GetRendererInstance()->setIndexBuffer(indexBuffer);

		IRenderer::GetRendererInstance()->drawElements(primitives, batches[batch].firstIndex, batches[batch].nIndices, batches[batch].firstVertex, batches[batch].nVertices);
	} else {
		IRenderer::GetRendererInstance()->drawArrays(primitives, batches[batch].firstVertex, batches[batch].nVertices);
	}
}

