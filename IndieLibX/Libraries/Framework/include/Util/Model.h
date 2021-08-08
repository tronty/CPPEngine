/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
//-----------------------------------------------------------------------------
// File: Framework\Util\Model.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _MODEL_H_
#define _MODEL_H_

#include <Framework3/IRenderer.h>
#include "VArray.h"

struct Batch {
	uint firstIndex;
	uint nIndices;
	uint firstVertex;
	uint nVertices;
};

class Model {
public:
	Model();
	~Model();

	bool loadHmdl(const char *fileName);
	bool load(const char *fileName);
	bool save(const char *fileName) const;

	bool makeDrawable();
	void clean();

	void setIndexSize(const unsigned int size){ indexSize = size; }

	void addBatch(const unsigned int firstIndex, const unsigned int nIndices, const unsigned int minVertex = 0, const unsigned int maxVertex = 0);
	void setBatch(const unsigned int batch, const unsigned int firstIndex, const unsigned int nIndices, const unsigned int minVertex = 0, const unsigned int maxVertex = 0);
	bool removeBatch(const unsigned int batch);
	void appendBatch(const void *verts, const unsigned int nVerts, const unsigned short *inds = NULL, const unsigned int nInds = 0);

	void addFormat(const AttributeType attType, const unsigned int size){
#if 0 // ???
		if (nAttribs == 0) vertexSize = 0;
		unsigned int index = nAttribs;
		nAttribs++;
		attribs = (VertexAttribute *) realloc(attribs, nAttribs * sizeof(VertexAttribute));

		attribs[index].type = attType;
		attribs[index].size = size;
		attribs[index].format = FORMAT_FLOAT;

		vertexSize += size * sizeof(float);
#endif
	}
	void appendFormat(const AttributeType attType, const unsigned int size);

	void setVBData(const void *data, const unsigned int nVerts){
		vertices = (unsigned char *) data;
		nVertices = nVerts;
	}
	void setIBData(const void *data, const unsigned int nInds, const unsigned int indSize){
		indices = (unsigned char *) data;
		nIndices = nInds;
		indexSize = indSize;
	}
	void addNormals();

	unsigned int getBatchCount() const { return batches.getCount(); }
	unsigned int getVertexSize() const { return vertexSize; }
	unsigned int getIndexSize() const { return indexSize; }
	unsigned int getVertexCount() const { return nVertices; }
	unsigned int getIndexCount() const { return nIndices; }

	Batch &getBatch(unsigned int index){ return batches[index]; }

	unsigned char *getVertexArray() const { return vertices; }
	unsigned char *getIndexArray() const { return indices; }

	void draw(const Primitives primitives = PRIM_TRIANGLES);
	void drawBatch(const unsigned int batch, const Primitives primitives = PRIM_TRIANGLES);

private:
	void computeRange(Batch &batch);

	VArray_ <Batch> batches;

	unsigned char *vertices;
	unsigned char *indices;
	unsigned int vertexSize;
	unsigned int indexSize;
	unsigned int nVertices;
	unsigned int nIndices;

	//VertexAttribute *attribs;
	FormatDesc *attribs;
	unsigned int nAttribs;

	VertexFormatID vertexFormat;
	VertexBufferID vertexBuffer;
	IndexBufferID indexBuffer;

};

#endif // _MODEL_H_

