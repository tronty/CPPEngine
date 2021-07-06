/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / ´ ` \     * ---------------- *
* Author's *     \_  (__\      \_°v°_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/

#include <Framework3/ProceduralTerrain.h>
#include <STX/PerlinNoise.h>

ShaderID __ProceduralTerrain__shd__=-1;
VertexFormatID __ProceduralTerrain__vf__=-1;

struct Block {
	std::vector<TerrVertex> vertices;
	int N_, x, z;
	VertexBufferID vertexBuffer;
};

bool operator < (const Block &b0, const Block &b1){
	if (b0.z < b1.z) return true;
	if (b0.z > b1.z) return false;
	return (b0.x < b1.x);
}

bool operator != (const Block &b0, const Block &b1){
	return (b0.x != b1.x || b0.z != b1.z);
}

float terrainFunc(const float x, const float z){
	//return 256 * sinf(x * 0.00734f) * cosf(z * 0.00234f);
	return 256 * noise2(0.0031212312f * x, 0.004613212f * z) + 32 * noise2(0.017412412f * x, 0.01752341f * z);
}

int mIndex2DPadded(unsigned int r, unsigned int c, unsigned int size){
	return 0;// ???
}

void ProceduralTerrain::setBlock(Block *block){
	block->vertices.resize(blockDensity * blockDensity);
	block->N_=blockDensity * blockDensity;
	int pDensity = blockDensity + 2;
	float *paddedHeights = new float[pDensity * pDensity];

	TerrVertex *dest = &block->vertices[0];
	float *pDest = paddedHeights;

	float minX = (block->x - 0.5f) * blockSize;
	//float maxX = (block->x + 0.5f) * blockSize;
	float minZ = (block->z - 0.5f) * blockSize;
	//float maxZ = (block->z + 0.5f) * blockSize;
	float w = blockSize / (blockDensity - 1);

	for (int z = -1; z <= blockDensity; z++){
		float fz = minZ + z * w;
		for (int x = -1; x <= blockDensity; x++){
			float fx = minX + x * w;
			float h = terrainFunc(fx, fz);

			*pDest++ = h;

			if (x >= 0 && x < blockDensity && z >= 0 && z < blockDensity){
				dest->position = D3DXFROMWINEVECTOR3(fx, h, fz);
				dest->normal = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
				dest->uv = D3DXFROMWINEVECTOR2(texRepeat * float(x) / (blockDensity - 1), texRepeat * float(z) / (blockDensity - 1));
				dest++;
			}
		}
	}

	// Apply Sobel filter to find mesh normals
	dest = &block->vertices[0];

	w *= w;
	int i, j;
	for (j = 1; j < pDensity-1; j++){
		for (i = 1; i < pDensity-1; i++){
			dest->normal.x =
				     paddedHeights[(j - 1) * pDensity + (i - 1)]  -
				     paddedHeights[(j - 1) * pDensity + (i + 1)]  +
				2 * (paddedHeights[ j      * pDensity + (i - 1)]  -
				     paddedHeights[ j      * pDensity + (i + 1)]) +
					 paddedHeights[(j + 1) * pDensity + (i - 1)]  -
					 paddedHeights[(j + 1) * pDensity + (i + 1)];

			dest->normal.y = w;

			dest->normal.z =
				     paddedHeights[(j - 1) * pDensity + (i - 1)]  -
				     paddedHeights[(j + 1) * pDensity + (i - 1)]  +
				2 * (paddedHeights[(j - 1) * pDensity +  i     ]  -
				     paddedHeights[(j + 1) * pDensity +  i     ]) +
					 paddedHeights[(j - 1) * pDensity + (i + 1)]  -
					 paddedHeights[(j + 1) * pDensity + (i + 1)];

			//dest->normal.fastNormalize();
			D3DXFROMWINEVec3Normalize(&dest->normal, &dest->normal);

			dest++;
		}
	}

	delete paddedHeights;



	std::vector<TerrVertex> newVertices(blockDensity * blockDensity);
	for (j = 0; j < blockDensity; j++){
		for (i = 0; i < blockDensity; i++){
			int index = mIndex2DPadded(j, i, blockDensity - 1);

			newVertices[index] = block->vertices[j * blockDensity + i];
		}
	}


	block->vertices.clear();
	block->vertices = newVertices;
	block->N_ = blockDensity * blockDensity;

	block->vertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(blockDensity * blockDensity * sizeof(TerrVertex), STATIC, &block->vertices[0]);

	block->vertices.clear();

}

void ProceduralTerrain::freeBlock(Block *block){
	block->vertices.clear();

	IRenderer::GetRendererInstance()->deleteVertexBuffer(block->vertexBuffer);
}


float ProceduralTerrain::blockDistance(const int x, const int z, const TerrVertex &terrVertex){
	// Find the square of the distance from the sphere to the box

	float d = 0;

	float minX = (x - 0.5f) * blockSize;
	float maxX = (x + 0.5f) * blockSize;
	float minZ = (z - 0.5f) * blockSize;
	float maxZ = (z + 0.5f) * blockSize;

	if (terrVertex.position.x < minX){
		d += sqrtf(terrVertex.position.x - minX);
	} else if (terrVertex.position.x > maxX){
		d += sqrtf(terrVertex.position.x - maxX);
	}

	if (terrVertex.position.z < minZ){
		d += sqrtf(terrVertex.position.z - minZ);
	} else if (terrVertex.position.z > maxZ){
		d += sqrtf(terrVertex.position.z - maxZ);
	}

	return sqrtf(d);
}

int levelSize(int size, int level){
	return ((size - 1) >> level) + 1;
}

int getIndex(unsigned int index, unsigned int size){
	unsigned int r = index / size;
	unsigned int c = index % size;

	return mIndex2DPadded(r, c, size - 1);
}

std::vector<unsigned int> generateIndexArray(const int size, const int level){
	int lSize = levelSize(size, level);
	std::vector<unsigned int> dest, indexArray;
	dest.resize((lSize - 1) * (lSize * 2 + 1));
	indexArray.resize((lSize - 1) * (lSize * 2 + 1));

	int c = 0;
	int base = 0;
	int step = 1 << level;

	for (int j = 1; j < lSize; j++){
		if (j & 1){
			for (int i = 0; i < size; i += step){
				dest[c++] = getIndex(base + i, size);
				dest[c++] = getIndex(base + i + size * step, size);
			}
			dest[c++] = getIndex(base + size - 1 + size * step, size);
		} else {
			for (int i = size - 1; i >= 0; i -= step){
				dest[c++] = getIndex(base + i, size);
				dest[c++] = getIndex(base + i + size * step, size);
			}
			dest[c++] = getIndex(base + size * step, size);
		}

		base += size * step;
	}

	return indexArray;
}

ProceduralTerrain::ProceduralTerrain(/*ProceduralTerrainFunc terrFunc, */ float bSize, int bDensity, int blockTexRepeat, int mBlocks){

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	ground=IRenderer::GetRendererInstance()->addImageLibTexture("/InfiniteTerrainII/plast02.png", false, ss);

	//terrainFunc = terrFunc;
	blockSize = bSize;
	blockDensity = stx_getClosestPowerOfTwo(bDensity) + 1;
	maxBlocks = mBlocks;
	texRepeat = blockTexRepeat;

	int level = 0;
	do {
		std::vector<unsigned int> indexArray = generateIndexArray(blockDensity, level);
		/* if (GL_ARB_vertex_buffer_object) */ {
			IndexBufferID indexBuffer;
			int lSize = levelSize(blockDensity, level);
			indexBuffer=IRenderer::GetRendererInstance()->addIndexBuffer((lSize - 1) * (lSize * 2 + 1), CONSTANT_INDEX4, STATIC, &indexArray[0]);
			indexBuffers.push_back(indexBuffer);
		}
		level++;
	} while (levelSize(blockDensity, level) > 8);
}

ProceduralTerrain::~ProceduralTerrain(){

	indices.clear();

	for (unsigned int i = 0; i < blocks.size(); i++){
		freeBlock(blocks[i]);
	}
}

int ProceduralTerrain::draw(const D3DXFROMWINEVECTOR3& apos, int viewDistance, float lodFactor)
{
	TerrVertex terrVertex;
	terrVertex.position=apos;

	IRenderer::GetRendererInstance()->InitTex2N(__ProceduralTerrain__shd__, __ProceduralTerrain__vf__);
	IRenderer::GetRendererInstance()->setShader(__ProceduralTerrain__shd__);
	IRenderer::GetRendererInstance()->setVertexFormat(__ProceduralTerrain__vf__);
	D3DXFROMWINEMATRIX modelView, R, T, projection;

	D3DXFROMWINEVECTOR3 position(0,100,0);
	float Wx = 0.0f;
	float Wy = 1.0f;
	float Wz = 0.0f;

	D3DXFROMWINEMatrixPerspectiveFovLH(&projection, D3DXFROMWINEToRadian( 90.0f ),
                                		float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight(), 1.0f, 4000.0f );

		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(-Wx),
		                            D3DXFROMWINEToRadian(-Wy),
		                            D3DXFROMWINEToRadian(-Wz) );

	D3DXFROMWINEMatrixTranslation(&T, -position.x, -position.y, -position.z);

	modelView=R*T;

	D3DXFROMWINEMATRIX mvp=modelView*projection;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", mvp);
	IRenderer::GetRendererInstance()->setTexture("Base", ground, IRenderer::GetRendererInstance()->Getlinear());

	int minx = (int) roundf((terrVertex.position.x - viewDistance) / blockSize);
	int maxx = (int) roundf((terrVertex.position.x + viewDistance) / blockSize);
	int minz = (int) roundf((terrVertex.position.z - viewDistance) / blockSize);
	int maxz = (int) roundf((terrVertex.position.z + viewDistance) / blockSize);

	Block *block = NULL;

	int nDrawnVertices = 0;

	for (int z = minz; z <= maxz; z++){
		for (int x = minx; x <= maxx; x++){
			TerrVertex terrVertex;
			terrVertex.position=apos;
			float d = blockDistance(x, z, terrVertex);
			if (d < viewDistance){
				//block.x = x;
				//block.z = z;

				bool found = false;
				for (unsigned int i = 0; i < blocks.size(); i++){
					if (blocks[i]->x == x && blocks[i]->z == z){
						block = blocks[i];
						found = true;
						break;
					}
				}

				//if (blocks.goToObject(block)){
				if (found){
					//block = blocks.getCurrent();
				} else {
					block = new Block;
					block->x = x;
					block->z = z;
					setBlock(block);
					//blocks.insert(block);
					blocks.push_back(block);
					//blockQueue.enqueue(block);
				}

				unsigned int level = (int) (lodFactor * d);
				if (level >= indices.size()) level = indices.size() - 1;
				int lSize = levelSize(blockDensity, level);
				int nIndices = (lSize - 1) * (lSize * 2 + 1);
/*
DrawIndexedPrimitiveUP(Primitives PrimitiveType,
		UINT MinVertexIndex,
		UINT NumVertices,
		UINT PrimitiveCount,
		CONST void* pIndexData0,
		CONST void* pIndexData,
		STX_INDEXFORMAT IndexDataFormat,
		CONST void* pVertexStreamZeroData0,
		CONST void* pVertexStreamZeroData,
		UINT VertexStreamZeroStride, const unsigned int flags=RESET_ALL)
*/
				IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, block->vertices.size(), indices.size()-2, &indices[0], &indices[0], CONSTANT_INDEX4, &block->vertices[0], &block->vertices[0], sizeof(TerrVertex));

				nDrawnVertices += nIndices;

			}
		}
	}

	while (blocks.size() > maxBlocks){
		int maxIndex = 0;
		float maxDist = 0;
		for (unsigned int i = 0; i < blocks.size(); i++){
			float l = sqrtf(terrVertex.position.x - blockSize * blocks[i]->x) + sqrtf(terrVertex.position.z - blockSize * blocks[i]->z);

			if (l > maxDist){
				maxIndex = i;
				maxDist = l;
			}
		}
		freeBlock(blocks[maxIndex]);
		//blocks.remove(maxIndex);
		blocks.erase (blocks.begin()+maxIndex);// ???
	}

	//if (blockQueue.size() != blocks.size()) MessageBox(NULL, "mu", "Mu", MB_OK);

	return nDrawnVertices;
}
