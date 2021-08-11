#include "SnowSystem.h"
#include "../../Framework/Math/Perlin.h"

bool SnowSystem::load(LPDIRECT3DDEVICE9 lpdev, Renderer *lprenderer, DWORD maxtexwidth, DWORD maxtexheight, bool debugruntime, bool supportsHalf){
	dev = lpdev;
	renderer = lprenderer;

	rtWidth  = min(int(maxtexwidth) / 4, MAX_WIDTH);
	rtHeight = min(1048576 / rtWidth, int(maxtexheight));

	if ((rtPos[0] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((rtPos[1] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((rtDir[0] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	if ((rtDir[1] = renderer->addRenderTarget(rtWidth, rtHeight, FORMAT_RGBA16f, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	/*
		Since this render target will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	LPDIRECT3DTEXTURE9 d3drt;
	if (dev->CreateTexture(4 * rtWidth, rtHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, supportsHalf? D3DFMT_A16B16G16R16F : D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((r2vbRT = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;



	// Load shaders
	if ((particleShader = renderer->addShader("particle.shd"   )) == SHADER_NONE) return false;
	if ((physics        = renderer->addShader("physics.shd"    )) == SHADER_NONE) return false;
	if ((initPhysics    = renderer->addShader("initPhysics.shd")) == SHADER_NONE) return false;
	if ((makeQuads      = renderer->addShader("makeQuads.shd"  )) == SHADER_NONE) return false;


	// Load textures
	if ((particle = renderer->addTexture(TEX_PATH "Particle.dds", FILTER_BILINEAR, CLAMP)) == TEXTURE_NONE) return false;

	StreamVertexAttribute particleAttribs[] = {
		0, TYPE_VERTEX,   supportsHalf? FORMAT_HALF : FORMAT_FLOAT, 4,
		1, TYPE_TEXCOORD, FORMAT_USHORT, 2,
	};
	if ((particleVF = renderer->addVertexFormat(particleAttribs, elementsOf(particleAttribs))) == TEXTURE_NONE) return false;

	VertexAttribute physicsAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((physicsVF = renderer->addVertexFormat(physicsAttribs, elementsOf(physicsAttribs))) == TEXTURE_NONE) return false;

	if ((particleIndices = renderer->addIndexBuffer(6 * rtWidth * ROWS_PER_DIP, sizeof(INDEX_TYPE))) == IB_NONE) return false;
	INDEX_TYPE *indices = (INDEX_TYPE *) renderer->lockIndexBuffer(particleIndices);
	if (indices == NULL) return false;
	for (int i = 0; i < rtWidth * ROWS_PER_DIP; i++){
		indices[6 * i    ] = 4 * i;
		indices[6 * i + 1] = 4 * i + 1;
		indices[6 * i + 2] = 4 * i + 2;
		indices[6 * i + 3] = 4 * i;
		indices[6 * i + 4] = 4 * i + 2;
		indices[6 * i + 5] = 4 * i + 3;
	}
	renderer->unlockIndexBuffer(particleIndices);


	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size = rtWidth * rtHeight * 4 * (debugruntime? 4 * (supportsHalf? sizeof(half) : sizeof(float)) : sizeof(ushort) * 2);

	if ((quadCoordVB = renderer->addVertexBuffer(size)) == VB_NONE) return false;
	ushort *quadCoord = (ushort *) renderer->lockVertexBuffer(quadCoordVB);
	if (quadCoord == NULL) return false;
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
	renderer->unlockVertexBuffer(quadCoordVB);

	// RAND_MAX = 32767 in MSVC
#define rand16() (rand() ^ (rand() << 1))

	Image img;
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

	if ((randTex = renderer->addTexture(img, FILTER_POINT)) == TEXTURE_NONE) return false;

	reset();

	return true;
}

bool SnowSystem::reset(){
	if (dev)
	{
		dev->BeginScene();

		renderer->setShader(initPhysics);
		renderer->setTexture("Rand", randTex);
		renderer->setShaderConstant2f("halfPixel", float2(0.5f / rtWidth, 0.5f / rtHeight));
		renderer->setVertexFormat(physicsVF);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		const float2 vertices[] = { float2(-1, -1), float2( 1, -1), float2( 1,  1), float2(-1,  1) };

		// Initialize the render targets
		TextureID rt0[] = { rtPos[0], rtDir[0] };
		renderer->changeRenderTargets(elementsOf(rt0), rt0);
			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

		TextureID rt1[] = { rtPos[1], rtDir[1] };
		renderer->changeRenderTargets(elementsOf(rt1), rt1);
			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

		renderer->resetRenderTarget();

		currRT  = 0;
		otherRT = 1;

		dev->EndScene();
	}

	return true;
}

void SnowSystem::draw(const float4x4 &mvp, float4x4 &mv, float frametime){
	int activeRows = (int) (0.02f * rtHeight + 0.5f);
	float b = 1.0f - 2.0f * activeRows / rtHeight;

	const float2 vertices[] = { float2(-1, b), float2(1, b), float2( 1,  1), float2(-1,  1) };

	TextureID rt[] = { rtPos[currRT], rtDir[currRT] };

	renderer->changeRenderTargets(elementsOf(rt), rt);

		renderer->setShader(physics);
		renderer->setTexture("Pos", rtPos[otherRT]);
		renderer->setTexture("Dir", rtDir[otherRT]);
		renderer->setTexture("Rand", randTex);

		static float t = -2;
		float ft = min(frametime, 0.05f);
		t += 2.0f * ft;

		renderer->setShaderConstant2f("halfPixel", float2(0.5f / rtWidth, 0.5f / rtHeight));
		renderer->setShaderConstant1f("frameTime", ft);
		renderer->setShaderConstant2f("randValue", float2((rand()%137)/100.0f, (rand()%531)/100.0f));
		renderer->setVertexFormat(physicsVF);
		renderer->apply();

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

	renderer->changeRenderTarget(r2vbRT);

		renderer->setShader(makeQuads);
		renderer->setTexture("Pos", rtPos[currRT]);
		renderer->setShaderConstant2f("halfPixel", float2(0.5f / (4 * rtWidth), 0.5f / rtHeight));
		renderer->setVertexFormat(physicsVF);
		renderer->apply();

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

	renderer->resetRenderTarget();

	dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// Enable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));


	renderer->setShader(particleShader);
	renderer->setTexture("Particle", particle);
	renderer->setShaderConstant4x4f("mvp", mvp);
	renderer->setShaderConstant3f("dx", 12.5f * mv.getRightVec());
	renderer->setShaderConstant3f("dy", 12.5f * mv.getUpVec());
	renderer->setVertexFormat(particleVF);
	renderer->setIndexBuffer(particleIndices);
	renderer->setVertexBuffer(quadCoordVB, 0); // Dummy vertex buffer
	renderer->setVertexBuffer(quadCoordVB, 1);
	renderer->setBlending(ONE, ONE);
	renderer->setMask(COLOR);
	renderer->apply();


	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(r2vbRT));
	// Tell the driver that stream 0 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DMAP));

	/*
		We could make make one big DIP call, but some cards have a lower primitive count limitation.
		So we split it into a number of draw calls to be friendly with those cards; we're not going
		to be limited by drawcalls anyway. This also allows us to use a smaller index buffer.
	*/
	for (int i = 0; i < activeRows; i += ROWS_PER_DIP){
		int nRows = min(activeRows - i, ROWS_PER_DIP);
		int nQuads = nRows * rtWidth;

		dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 4 * i * rtWidth, 0, 4 * nQuads, 0, 2 * nQuads);
	}

	// Stream 0 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);

	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

	// Flip render targets
	currRT = otherRT;
	otherRT = 1 - currRT;

	int cc = activeRows * rtWidth;
}
