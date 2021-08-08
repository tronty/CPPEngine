/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

#include "SnowSystem.h"
#include <Math/Perlin.h>

bool SnowSystem::load(__DWORD__ maxtexwidth,__DWORD__ maxtexheight, bool debugruntime, bool supportsHalf){

	rtWidth  = min(int(maxtexwidth) / 4, MAX_WIDTH);
	rtHeight = min(1048576 / rtWidth, int(maxtexheight));

	rtPos[0] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	rtPos[1] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	rtDir[0] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	rtDir[1] = IRenderer::GetRendererInstance()->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());

	/*
		Since this render target will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	r2vbRT = IRenderer::GetRendererInstance()->addRenderTarget(4*rtWidth, rtHeight, FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Load shaders
	if ((particleShader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/particle.shd", "main", "main"   )) == SHADER_NONE) return false;
	if ((physics        = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/physics.shd", "main", "main"    )) == SHADER_NONE) return false;
	if ((initPhysics    = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/initPhysics.shd", "main", "main")) == SHADER_NONE) return false;
	if ((makeQuads      = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Footprint/makeQuads.shd", "main", "main"  )) == SHADER_NONE) return false;

	// Load textures
	particle = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Footprint/Particle.dds", false, IRenderer::GetRendererInstance()->GetbilinearClamp());

	{FormatDesc particleAttribs[] = {
		0, TYPE_VERTEX,   supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 2,
	};
	if ((particleVF = IRenderer::GetRendererInstance()->addVertexFormat(particleAttribs, elementsOf(particleAttribs), particleShader)) == TEXTURE_NONE) return false;}

	{FormatDesc physicsAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((physicsVF = IRenderer::GetRendererInstance()->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs), physics)) == TEXTURE_NONE) return false;
	if ((initPhysicsVF = IRenderer::GetRendererInstance()->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs), initPhysics)) == TEXTURE_NONE) return false;
	if ((makeQuadsVF = IRenderer::GetRendererInstance()->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs), makeQuads)) == TEXTURE_NONE) return false;}

	if ((particleIndices = IRenderer::GetRendererInstance()->addIndexBuffer(6 * rtWidth * ROWS_PER_DIP, sizeof(INDEX_TYPE))) == IB_NONE) return false;
	INDEX_TYPE *indices = (INDEX_TYPE *) IRenderer::GetRendererInstance()->lockIndexBuffer(particleIndices);
	if (indices == 0) return false;
	for (int i = 0; i < rtWidth * ROWS_PER_DIP; i++){
		indices[6 * i    ] = 4 * i;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;
		indices[6 * i + 3] = 4 * i;
		indices[6 * i + 4] = 4 * i + 2;
		indices[6 * i + 5] = 4 * i + 3;
	}
	IRenderer::GetRendererInstance()->unlockIndexBuffer(particleIndices);


	/*
		We need a dummy vertex buffer when we specify stride and offset when calling setVertexBuffer()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size = rtWidth * rtHeight * 4 * (debugruntime? 4 * (supportsHalf? sizeof(half) : sizeof(float)) : sizeof(ushort) * 2);

	if ((quadCoordVB = IRenderer::GetRendererInstance()->addVertexBuffer(size)) == VB_NONE) return false;
	ushort *quadCoord = (ushort *) IRenderer::GetRendererInstance()->lockVertexBuffer(quadCoordVB);
	if (quadCoord == 0) return false;
	for (int i = 0; i < rtWidth * rtHeight; i++){
		quadCoord[8 * i + 0] = 0;
		quadCoord[8 * i + 1] = 0;
		quadCoord[8 * i + 2] = 65535;
		quadCoord[8 * i + 3] = 0;
		quadCoord[8 * i + 4] = 65535;
		quadCoord[8 * i + 5] = 65535;
		quadCoord[8 * i + 6] = 0;
		quadCoord[8 * i + 7] = 65535;
	}
	IRenderer::GetRendererInstance()->unlockVertexBuffer(quadCoordVB);

	// RAND_MAX = 32767 in MSVC
#define rand16() (rand() ^ (rand() << 1))

	Image3 img;
	ushort *pixels = (ushort *) img.create(FORMAT_RGBA16, 128, 128, 1, 1);
	for (int i = 0; i < 128 * 128; i++){
		ushort x, y, z;
		float d;
		do {
			x = rand16();
			y = rand16();
			z = rand16();

			float dx = (x / 65535.0f) * 2 - 1;
			float dy = (y / 65535.0f) * 2 - 1;
			float dz = (z / 65535.0f) * 2 - 1;

			d = dx * dx + dy * dy + dz * dz;
		} while (d > 1.0f);

		pixels[4 * i + 0] = x;
		pixels[4 * i + 1] = y;
		pixels[4 * i + 2] = z;
		pixels[4 * i + 3] = rand16();
	}

	randTex = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	reset();

	return true;
}

bool SnowSystem::reset(){
	if (1)
	{
		IRenderer::GetRendererInstance()->BeginScene();

		IRenderer::GetRendererInstance()->setShader(initPhysics);
		IRenderer::GetRendererInstance()->setVertexFormat(initPhysicsVF);
		IRenderer::GetRendererInstance()->setTexture("Rand", randTex);
		IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / rtWidth, 0.5f / rtHeight));
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

		const D3DXFROMWINEVECTOR2 vertices[] = { D3DXFROMWINEVECTOR2(-1, -1), D3DXFROMWINEVECTOR2( 1, -1), D3DXFROMWINEVECTOR2( 1,  1), D3DXFROMWINEVECTOR2(-1,  1) };

		// Initialize the render targets
		TextureID rt0[] = { rtPos[0], rtDir[0] };
		IRenderer::GetRendererInstance()->changeRenderTargets(rt0, elementsOf(rt0));
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

		TextureID rt1[] = { rtPos[1], rtDir[1] };
		IRenderer::GetRendererInstance()->changeRenderTargets(rt1, elementsOf(rt1));
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

		IRenderer::GetRendererInstance()->changeToMainFramebuffer();

		currRT  = 0;
		otherRT = 1;

		IRenderer::GetRendererInstance()->EndScene();
	}

	return true;
}

void SnowSystem::draw(const D3DXFROMWINEMATRIX &mvp, D3DXFROMWINEMATRIX &mv, float frametime){
	int activeRows = (int) (0.02f * rtHeight + 0.5f);
	float b = 1.0f - 2.0f * activeRows / rtHeight;

	const D3DXFROMWINEVECTOR2 vertices[] = { D3DXFROMWINEVECTOR2(-1, b), D3DXFROMWINEVECTOR2(1, b), D3DXFROMWINEVECTOR2( 1,  1), D3DXFROMWINEVECTOR2(-1,  1) };

	TextureID rt[] = { rtPos[currRT], rtDir[currRT] };

	IRenderer::GetRendererInstance()->changeRenderTargets(rt, elementsOf(rt));

		IRenderer::GetRendererInstance()->setShader(physics);
		IRenderer::GetRendererInstance()->setVertexFormat(physicsVF);
		IRenderer::GetRendererInstance()->setTexture("Pos", rtPos[otherRT]);
		IRenderer::GetRendererInstance()->setTexture("Dir", rtDir[otherRT]);
		IRenderer::GetRendererInstance()->setTexture("Rand", randTex);

		static float t = -2;
		float ft = min(frametime, 0.05f);
		t += 2.0f * ft;

		IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / rtWidth, 0.5f / rtHeight));
		IRenderer::GetRendererInstance()->setShaderConstant1f("1000.0f*timeGetTime()", ft);
		IRenderer::GetRendererInstance()->setShaderConstant2f("randValue", D3DXFROMWINEVECTOR2((rand()%137)/100.0f, (rand()%531)/100.0f));

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

	IRenderer::GetRendererInstance()->changeRenderTarget(r2vbRT);

		IRenderer::GetRendererInstance()->setShader(makeQuads);
		IRenderer::GetRendererInstance()->setVertexFormat(makeQuadsVF);
		IRenderer::GetRendererInstance()->setTexture("Pos", rtPos[currRT]);
		IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / (4 * rtWidth), 0.5f / rtHeight));

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

	IRenderer::GetRendererInstance()->Clear(false, true, stx_GetRGBA(0x00000000));

	// Enable render to vertex buffer extension
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));


	IRenderer::GetRendererInstance()->setShader(particleShader);
	IRenderer::GetRendererInstance()->setVertexFormat(particleVF);
	IRenderer::GetRendererInstance()->setTexture("Particle", particle);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setShaderConstant3f("dx", 12.5f * mv.getRightVec());
	IRenderer::GetRendererInstance()->setShaderConstant3f("dy", 12.5f * mv.getUpVec());
	IRenderer::GetRendererInstance()->setIndexBuffer(particleIndices);
	//IRenderer::GetRendererInstance()->setVertexBuffer(0, quadCoordVB); // Dummy vertex buffer
	IRenderer::GetRendererInstance()->setVertexBuffer(0, quadCoordVB);
	IRenderer::GetRendererInstance()->setBlending(eONE, eONE);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);

	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", r2vbRT);
	// Tell the driver that stream 0 is to be fetched from the DMAP texture
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

	/*
		We could make make one big DIP call, but some cards have a lower primitive count limitation.
		So we split it into a number of draw calls to be friendly with those cards; we're not going
		to be limited by drawcalls anyway. This also allows us to use a smaller index buffer.
	*/
	for (int i = 0; i < activeRows; i += ROWS_PER_DIP){
		int nRows = min(activeRows - i, ROWS_PER_DIP);
		int nQuads = nRows * rtWidth;

		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 4 * i * rtWidth, 0, 4 * nQuads, 0, 2 * nQuads);
	}

	// Stream 0 restored to regular vertex buffer mode
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	

	// Disable render to vertex buffer extension
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

	// Flip render targets
	currRT = otherRT;
	otherRT = 1 - currRT;

	int cc = activeRows * rtWidth;
}

