/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#ifndef _SNOWSYSTEM_H_
#define _SNOWSYSTEM_H_

#include <Util/ProgrammerArt.h>

#define ROWS_PER_DIP 64
#define MAX_WIDTH 1024

#if (ROWS_PER_DIP * MAX_WIDTH <= 65536)
#define INDEX_TYPE ushort
#else
#define INDEX_TYPE uint
#endif


class SnowSystem {
public:
	SnowSystem(){}
	bool load(__DWORD__ maxtexwidth, __DWORD__ maxtexheight, bool debugruntime, bool supportsHalf);
	bool reset();

	void draw(const D3DXFROMWINEMATRIX &mvp, D3DXFROMWINEMATRIX &mv, float frametime);

private:
	ShaderID particleShader, physics, initPhysics, makeQuads;
	TextureID randTex, particle, palette;

	VertexFormatID particleVF, physicsVF, initPhysicsVF, makeQuadsVF;
	VertexBufferID quadCoordVB;
	IndexBufferID particleIndices;

	TextureID rtPos[2], rtDir[2], r2vbRT;
	int currRT, otherRT;
	int rtWidth, rtHeight;
};
#endif

