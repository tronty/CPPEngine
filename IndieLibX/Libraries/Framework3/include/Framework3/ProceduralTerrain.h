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

#ifndef _PROCEDURALTERRAIN_H_
#define _PROCEDURALTERRAIN_H_

#include <set>
#include <STX/STX.h>
#include <STX/d3dx9mathFromWine.h>
#include <Framework3/IRenderer.h>

//typedef float (* ProceduralTerrainFunc)(const float x, const float z);

struct Block;

typedef Vertex332N TerrVertex;

class ProceduralTerrain {
protected:
	TextureID ground;
	//ProceduralTerrainFunc terrainFunc;
	//OrderedSet <Block> blocks;
	std::vector<Block*> blocks;
	//Fifo <Block> blockQueue;
	unsigned int maxBlocks;
	std::vector<unsigned int> indices;
	std::vector<IndexBufferID> indexBuffers;
	float blockSize;
	int blockDensity;
	int texRepeat;

	void setBlock(Block *block);
	void freeBlock(Block *block);

public:
	ProceduralTerrain(/* ProceduralTerrainFunc terrFunc, */ float bSize, int bDensity, int blockTexRepeat, int mBlocks);
	~ProceduralTerrain();

	float blockDistance(const int x, const int z, const Vertex332N &pos);
	//int draw(const Vertex332N &pos, float viewDistance, float lodFactor);
	int draw(const D3DXFROMWINEVECTOR3& position, int viewDistance, float lodFactor);

};

#endif // _PROCEDURALTERRAIN_H_

