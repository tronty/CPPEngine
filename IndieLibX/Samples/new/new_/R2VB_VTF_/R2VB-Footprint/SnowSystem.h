#ifndef _SNOWSYSTEM_H_
#define _SNOWSYSTEM_H_

#include "../../Framework/Util/ProgrammerArt.h"
#include "../../Framework/Direct3D/atir2vb.h"
#include "../../Framework/Direct3D/D3D9App.h"

#define ROWS_PER_DIP 64
#define MAX_WIDTH 1024

#if (ROWS_PER_DIP * MAX_WIDTH <= 65536)
#define INDEX_TYPE ushort
#else
#define INDEX_TYPE uint
#endif


class SnowSystem {
public:
	SnowSystem() {dev = NULL; renderer = NULL; };
	bool load(LPDIRECT3DDEVICE9, Renderer*, DWORD maxtexwidth, DWORD maxtexheight, bool debugruntime, bool supportsHalf);
	bool reset();

	void draw(const float4x4 &mvp, float4x4 &mv, float frametime);

private:
	LPDIRECT3DDEVICE9 dev;
	Renderer *renderer;

	ShaderID particleShader, physics, initPhysics, makeQuads;
	TextureID randTex, particle, palette;

	VertexFormatID particleVF, physicsVF;
	VertexBufferID quadCoordVB;
	IndexBufferID particleIndices;

	TextureID rtPos[2], rtDir[2], r2vbRT;
	int currRT, otherRT;
	int rtWidth, rtHeight;
};

#endif
