//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses R2VB technique to compute skinning animation in the pixel 
// shader. Using the R2VB we can compute 2048-4096 (depending on hardware)
// different animations at one time and render 2048-4096 models with one
// DrawPrimitive() call. 
//-----------------------------------------------------------------------------

#include "App.h"
#include "../../Framework/Direct3D/atir2vb.h"
#include <D3dx9math.h>

#define SCALE							0.02f

#define __USE_HALF__

#ifdef __USE_HALF__
	#define VERTEX_FORMAT					D3DFMT_A16B16G16R16F
	#define BONE_INDEX_FORMAT				D3DFMT_A16B16G16R16F
	#define VERTEX_WEIGHT_FORMAT			D3DFMT_A16B16G16R16F
	#define BONE_MATRIX_FORMAT				D3DFMT_A16B16G16R16F
	#define FRAMETIME_MATRIX_FORMAT			D3DFMT_A16B16G16R16F
	#define R2VB_VBO_FORMAT					D3DFMT_A32B32G32R32F
	#define R2VB_TEXCOORD_FORMAT			FORMAT_FLOAT
	
	#define VERTEX_VALUE_FORMAT				half
	#define BONE_INDEX_VALUE_FORMAT			half
	#define VERTEX_WEIGHT_VALUE_FORMAT		half
	#define BONE_MATRIX_VALUE_FORMAT		half
	#define FRAMETIME_MATRIX_VALUE			half
	#define R2VB_VERTEX_FORMAT				FORMAT_FLOAT
	#define R2VB_VERTEX_VALUE_FORMAT		float
	#define R2VB_TEXCOORD_VALUE_FORMAT		float
#else
	#define VERTEX_FORMAT					D3DFMT_A32B32G32R32F
	#define BONE_INDEX_FORMAT				D3DFMT_A32B32G32R32F
	#define VERTEX_WEIGHT_FORMAT			D3DFMT_A32B32G32R32F
	#define BONE_MATRIX_FORMAT				D3DFMT_A32B32G32R32F
	#define FRAMETIME_MATRIX_FORMAT			D3DFMT_A32B32G32R32F
	#define R2VB_VBO_FORMAT					D3DFMT_A32B32G32R32F
	#define R2VB_TEXCOORD_FORMAT			FORMAT_FLOAT

	#define VERTEX_VALUE_FORMAT				float
	#define BONE_INDEX_VALUE_FORMAT			float
	#define VERTEX_WEIGHT_VALUE_FORMAT		float
	#define BONE_MATRIX_VALUE_FORMAT		float
	#define FRAMETIME_MATRIX_VALUE			float
	#define R2VB_VERTEX_FORMAT				FORMAT_FLOAT
	#define R2VB_VERTEX_VALUE_FORMAT		float
	#define R2VB_TEXCOORD_VALUE_FORMAT		float
#endif

#define MODELNAME						"npc/npc_01"
#define MODELNAME_2						"ruby/ruby"

BaseApp *app = new App();

//-----------------------------------------------------------------------------------------
// Convert quaterion to float4X4 matrix
//-----------------------------------------------------------------------------------------
void Quaternion2float4x4(float4x4& m, float4& q)
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0f;

	xs = q.x*s;        ys = q.y*s;        zs = q.z*s;
	wx = q.w*xs;       wy = q.w*ys;       wz = q.w*zs;
	xx = q.x*xs;       xy = q.x*ys;       xz = q.x*zs;
	yy = q.y*ys;       yz = q.y*zs;       zz = q.z*zs;

	m.elem[0][0] = 1.0f - (yy+zz);
	m.elem[0][1] = xy + wz;
	m.elem[0][2] = xz - wy;

	m.elem[1][0] = xy - wz;
	m.elem[1][1] = 1.0f - (xx+zz);
	m.elem[1][2] = yz + wx;
	
	m.elem[2][0] = xz + wy;
	m.elem[2][1] = yz - wx;
	m.elem[2][2] = 1.0f - (xx + yy);

	m.elem[3][0] = m.elem[3][1] = m.elem[3][2] = 0.0f;
	m.elem[3][3] = 1.0f;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::setup(char *args[], int nArguments)
{
	width  = 800;
	height = 600;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::resetCamera()
{
	camPos = float3(0, 1500*SCALE, -1500*SCALE);
	wx = 0.7f;
	wy = 0.0f;
	speed = 1000.0f*SCALE;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::initGUI()
{
	menu = mainMenu->addMenu("Method");
	vsAnimation = menu->addItem("Vertex shader skinning",  MENU_CHECKABLE);
	psAnimation = menu->addItem("Pixel shader skinning", MENU_CHECKABLE);
	menu->setRadioButtonGroup(vsAnimation, psAnimation, 1);
	menu->toggleItemCheck(psAnimation);

	animationMenu = mainMenu->addMenu("Animation");
	sameAnimation = animationMenu->addItem("Same animation everyone",  MENU_CHECKABLE);
	differentAnimation = animationMenu->addItem("Different animation everyone",  MENU_CHECKABLE);
	animationMenu->setRadioButtonGroup(sameAnimation, differentAnimation, 1);
	animationMenu->toggleItemCheck(differentAnimation);

	AIMenu = mainMenu->addMenu("AI");
	noAI = AIMenu->addItem("No AI",  MENU_CHECKABLE);
	respawn = AIMenu->addItem("Respawn",  MENU_CHECKABLE);
	bounce = AIMenu->addItem("Bounce", MENU_CHECKABLE);
	AIMenu->setRadioButtonGroup(noAI, bounce, 1);
	AIMenu->toggleItemCheck(bounce);
	lastAI = bounce;

	addWidget(numModelSlider = new Slider(760, 10, 26, 580, false, 1, MAX_NUM_MODELS/4, 2500));

	D3D9App::initGUI();
}
//-----------------------------------------------------------------------------------------
// Load skinning vertex data into texture. It includes vertex position, bone index and 
// weight value.
//-----------------------------------------------------------------------------------------
bool App::loadSkinningVertexMap(Model *m, char* name)
{
	FILE	*fp;
	DWORD	NumVertices, NumBones;
	float4	*vertexOffset;
	float4	*vertexNormal;

	fp = fopen(name, "rb");
	if (!fp)
		return false;

	fread(&NumBones, sizeof(DWORD), 1, fp);
	fread(&NumVertices, sizeof(DWORD), 1, fp);

	vertexOffset = new float4[NumVertices];
	vertexNormal = new float4[NumVertices];

	for (uint i=0; i<NumVertices; i++)
	{
		fread(&vertexOffset[i], sizeof(float4), 1, fp);
		fseek(fp, sizeof(float4)*3, SEEK_CUR);
		fread(&vertexNormal[i], sizeof(float4), 1, fp);
		vertexNormal[i].w = 0.0f;
	}

	LPDIRECT3DTEXTURE9 tex;
	D3DLOCKED_RECT rect;

		// 1 pixel can store 1 vertex position and 1 pixel can store 1 vertex normal, so one skinning vertex == 2 pixels.
		if (dev->CreateTexture(m->vertexTextureWidth*2, m->vertexTextureHeight, 1, 0, VERTEX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
		{
			ERRORMSG("Couldn't create texture");
			return false;
		}		
		if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
		{
			VERTEX_VALUE_FORMAT* lpC = (VERTEX_VALUE_FORMAT *)rect.pBits;

			for (uint j=0; j<NumVertices; j++)
			{
				*lpC++ = VERTEX_VALUE_FORMAT(vertexOffset[j].x);
				*lpC++ = VERTEX_VALUE_FORMAT(vertexOffset[j].y);
				*lpC++ = VERTEX_VALUE_FORMAT(vertexOffset[j].z);
				*lpC++ = VERTEX_VALUE_FORMAT(vertexOffset[j].w);
				*lpC++ = VERTEX_VALUE_FORMAT(vertexNormal[j].x);
				*lpC++ = VERTEX_VALUE_FORMAT(vertexNormal[j].y);
				*lpC++ = VERTEX_VALUE_FORMAT(vertexNormal[j].z);
				*lpC++ = VERTEX_VALUE_FORMAT(vertexNormal[j].w);
			}
			tex->UnlockRect(0);
		}
		if ((m->skinningVertex = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	// 1 pixel can store 4 bone indices, so one skinning vertex bone index data == 1 pixel.
	if (dev->CreateTexture(m->vertexTextureWidth, m->vertexTextureHeight, 1, 0, BONE_INDEX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}
	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		BONE_INDEX_VALUE_FORMAT* lpC = (BONE_INDEX_VALUE_FORMAT *)rect.pBits;
		float V[4];

		for (uint i=0; i<NumVertices; i++)
		{
			fread(V, sizeof(float), 4, fp);
			for (int j=0; j<4; j++)
			{
				V[j] /= ((float)NumBones);
				*lpC++ = BONE_INDEX_VALUE_FORMAT(V[0]);
			}
		}
		tex->UnlockRect(0);
	}
	if ((m->vertexBoneIndex = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	// 1 pixel can store 4 weight values, so one skinning vertex weight value == 1 pixel.
	if (dev->CreateTexture(m->vertexTextureWidth, m->vertexTextureHeight, 1, 0, VERTEX_WEIGHT_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}
	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		VERTEX_WEIGHT_VALUE_FORMAT* lpC = (VERTEX_WEIGHT_VALUE_FORMAT *)rect.pBits;

		for (uint i=0; i<NumVertices; i++)
		{
			float fV[4];
			fread(&fV[0], sizeof(float), 1, fp);
			*lpC++ = VERTEX_WEIGHT_VALUE_FORMAT(fV[0]);
			fread(&fV[1], sizeof(float), 1, fp);
			*lpC++ = VERTEX_WEIGHT_VALUE_FORMAT(fV[1]);
			fread(&fV[2], sizeof(float), 1, fp);
			*lpC++ = VERTEX_WEIGHT_VALUE_FORMAT(fV[2]);
			fread(&fV[3], sizeof(float), 1, fp);
			*lpC++ = VERTEX_WEIGHT_VALUE_FORMAT(fV[3]);
		}
		tex->UnlockRect(0);
	}
	if ((m->vertexWeight = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	fclose(fp);
	
	delete [] vertexNormal;
	delete [] vertexOffset;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load bone animation matrixes into texture.
//-----------------------------------------------------------------------------------------
bool App::loadBoneAnimationMap(Model *m, Batch *b, char* name)
{
	FILE	*fp;

	fp = fopen(name, "rb");
	if (!fp)
		return false;

	fread(&m->numBones, sizeof(DWORD), 1, fp);
	fread(&m->numFrames, sizeof(DWORD), 1, fp);
	for (uint i=0; i<m->numFrames; i++)
		fread(m->bonesMatrix[i], sizeof(float4x4), m->numBones, fp);

	fclose(fp);

	LPDIRECT3DTEXTURE9 tex;
	if (dev->CreateTexture(4*m->numBones, m->numFrames, 1, 0, BONE_MATRIX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}
	D3DLOCKED_RECT rect;
	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		BONE_MATRIX_VALUE_FORMAT* lpC = (BONE_MATRIX_VALUE_FORMAT *)rect.pBits;
		for (uint f=0; f<m->numFrames; f++)
			for (DWORD i=0; i<m->numBones; i++)
			{
				float *lpM = &m->bonesMatrix[f][i].elem[0][0];
				for (int i=0; i<16; i++)
					(*lpC++) = BONE_MATRIX_VALUE_FORMAT(lpM[i]);
			}
		tex->UnlockRect(0);
	}
	if ((m->boneAnimation = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	LPDIRECT3DTEXTURE9 d3drt;
	// Create render target for storing bone's transform matrices.
	// 4 pixels == 1 matrix4x4, so (4*model[0].numBones)x1 floating-point texture can store (model[0].numBones) bone matrices.
	if (dev->CreateTexture(4*m->numBones, 1, 1, D3DUSAGE_RENDERTARGET, BONE_MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((m->boneMatrix = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	// Create render target for storing bones' transform matrices of all models.
	// 4 pixels == 1 matrix4x4, so (4*model[0].numBones)x(maxBatchOnce) floating-point texture can store (model[0].numBones) bone matrices for (maxBatchOnce) models.
	if (dev->CreateTexture(4*m->numBones, maxBatchOnce, 1, D3DUSAGE_RENDERTARGET, BONE_MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((b->boneMatrix = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load model triangle and vertex data, that will be used on vertex shader animation.
// We also create R2VB vertex buffer here.
//-----------------------------------------------------------------------------------------
bool App::loadModel(Model *m, Batch *b, char* name)
{
	FILE	*fp;
	WORD	*lpTriangleList;
	float2	*lpUVList;

	fp = fopen(name, "rb");
	if (!fp)
		return false;

	fread(&m->numVertices, sizeof(DWORD), 1, fp);
	if (m->numVertices%2)
		m->vertexTextureWidth =  m->numVertices+1;
	else
		m->vertexTextureWidth =  m->numVertices;
	m->vertexTextureHeight = 1;
	b->vertexTextureWidth = m->vertexTextureWidth;
	b->vertexTextureHeight = m->vertexTextureHeight * maxBatchOnce;

	lpUVList = new float2[m->numVertices];

	// Create vertex declarations
	StreamVertexAttribute modelVertexAttribs[] = {
		1, TYPE_VERTEX,   R2VB_VERTEX_FORMAT, 4,
		1, TYPE_NORMAL,   R2VB_VERTEX_FORMAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
	if ((m->vertexFormat = renderer->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs))) == VF_NONE) return false;

	StreamVertexAttribute vsModelVertexAttribs[] = {
		1, TYPE_VERTEX,   FORMAT_FLOAT, 4,		
		1, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		1, TYPE_NORMAL,	  FORMAT_FLOAT, 4,
		1, TYPE_TANGENT,  FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};

	if ((m->vsVertexFormat = renderer->addVertexFormat(vsModelVertexAttribs, elementsOf(vsModelVertexAttribs))) == VF_NONE) return false;
	// Create the model vertex buffer for vertex shader animation
	if ((m->vsVertexBuffer = renderer->addVertexBuffer(m->numVertices * 4 *(sizeof(float4)))) == VB_NONE) return false;
	float4 *dest = (float4 *) renderer->lockVertexBuffer(m->vsVertexBuffer);
	for (DWORD i=0; i<m->numVertices; i++)
	{
		fread(dest, sizeof(float4), 1, fp);
		dest++;

		fseek(fp, sizeof(float4)*3, SEEK_CUR);

		fread(dest, sizeof(float4), 1, fp);
		dest++;

		fread(dest, sizeof(float4), 1, fp);
		dest++;

		fread(dest, sizeof(float4), 1, fp);
		dest++;
	}
	renderer->unlockVertexBuffer(m->vsVertexBuffer);
	// Create the texture coord of model
	if ((m->UVBuffer = renderer->addVertexBuffer(m->numVertices * (sizeof(R2VB_TEXCOORD_VALUE_FORMAT)*2))) == VB_NONE) return false;
	R2VB_TEXCOORD_VALUE_FORMAT *uvdest = (R2VB_TEXCOORD_VALUE_FORMAT *) renderer->lockVertexBuffer(m->UVBuffer);
		fread(lpUVList, sizeof(float2), m->numVertices, fp);
		for (uint j=0; j<m->numVertices; j++)
		{
			*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].x);
			*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].y);
		}
	renderer->unlockVertexBuffer(m->UVBuffer);

	// Create index buffer of model
	fread(&m->numTriangles, sizeof(DWORD), 1, fp);
	lpTriangleList = new WORD[m->numTriangles*3];

	// Create the triangle index buffer
	if ((m->indexBuffer = renderer->addIndexBuffer(m->numTriangles*3, sizeof(WORD))) == IB_NONE) return false;
	WORD *iDest = (WORD *) renderer->lockIndexBuffer(m->indexBuffer);
		fread(lpTriangleList, sizeof(WORD), m->numTriangles*3, fp);
		stx_memcpy(iDest, lpTriangleList, sizeof(WORD)*m->numTriangles*3);
	renderer->unlockIndexBuffer(m->indexBuffer);

	/*
		Create render target for the model animation.
		Since these render targets will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	LPDIRECT3DTEXTURE9 d3drt0;
	if (dev->CreateTexture(m->vertexTextureWidth*2, m->vertexTextureHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, R2VB_VBO_FORMAT, D3DPOOL_DEFAULT, &d3drt0, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((m->R2VB_VBO = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt0, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size;
	if (debugRuntime)
		size = (m->vertexTextureWidth*2) * m->vertexTextureHeight * 4 * sizeof(R2VB_VERTEX_VALUE_FORMAT);
	else
		size = 2;
	if ((m->R2VB_DummyVBO = renderer->addVertexBuffer(size)) == VB_NONE) return false;
	
	fclose(fp);

	// Setup batch data -----------------------------------------------------------------------------------------------------------------------------------------------------------------
	b->numVertices = m->vertexTextureWidth*m->vertexTextureHeight*maxBatchOnce;
	b->numTriangles = m->numTriangles*maxBatchOnce;
	if ((b->vertexFormat = renderer->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs))) == VF_NONE) return false;

	// Create the vertex texture coordinate buffer of models for batch rendering
	if ((b->UVBuffer = renderer->addVertexBuffer(b->vertexTextureWidth * b->vertexTextureHeight * (sizeof(R2VB_TEXCOORD_VALUE_FORMAT)*2))) == VB_NONE) return false;
	uvdest = (R2VB_TEXCOORD_VALUE_FORMAT *) renderer->lockVertexBuffer(b->UVBuffer);
	R2VB_TEXCOORD_VALUE_FORMAT *lpCUV = uvdest;
		for (uint i=0; i<maxBatchOnce; i++)
		{			
			float OffsetX = (rand()%4)*0.25f;
			float OffsetY = (rand()%4)*0.25f;
			for (uint j=0; j<m->numVertices; j++)
			{
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].x+OffsetX);
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].y+OffsetY);
			}
			lpCUV += (m->vertexTextureWidth*m->vertexTextureHeight*2);
			uvdest = lpCUV;
		}
	renderer->unlockVertexBuffer(b->UVBuffer);
	/*
		Create render target for the model animation of batch rendering.
		Since these render targets will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	if (dev->CreateTexture(b->vertexTextureWidth*2, b->vertexTextureHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, R2VB_VBO_FORMAT, D3DPOOL_DEFAULT, &d3drt0, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((b->R2VB_VBO = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt0, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	if (debugRuntime)
		size = (b->vertexTextureWidth*2) * b->vertexTextureHeight * 4 * sizeof(R2VB_VERTEX_VALUE_FORMAT);
	else
		size = 2;
	if ((m->R2VB_OnceDummyVBO = renderer->addVertexBuffer(size)) == VB_NONE) return false;


	// Create index buffer for batch rendering
	if ((b->indexBuffer = renderer->addIndexBuffer(b->numTriangles*3, sizeof(DWORD))) == IB_NONE) return false;
	DWORD *dDest = (DWORD *) renderer->lockIndexBuffer(b->indexBuffer);
	for (DWORD i=0; i<maxBatchOnce; i++)
	{
		for (DWORD t=0; t<m->numTriangles*3; t++)
		{
			*dDest++ = (DWORD)lpTriangleList[t] + i*m->vertexTextureWidth*m->vertexTextureHeight;
		}
	}
	renderer->unlockIndexBuffer(b->indexBuffer);
	
	// Create a texture for storing constants for the batch rendering. We store frame time,
	// interpolation values and transform matrix here. We create 3 textures here for ping-ponging rendering.
	LPDIRECT3DTEXTURE9 tex;
	if (dev->CreateTexture(4, maxBatchOnce, 1, D3DUSAGE_DYNAMIC, FRAMETIME_MATRIX_FORMAT, D3DPOOL_DEFAULT , &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}	
	if ((b->FrameTime[0] = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	if (dev->CreateTexture(4, maxBatchOnce, 1, D3DUSAGE_DYNAMIC, FRAMETIME_MATRIX_FORMAT, D3DPOOL_DEFAULT , &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}
	if ((b->FrameTime[1] = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	if (dev->CreateTexture(4, maxBatchOnce, 1, D3DUSAGE_DYNAMIC, FRAMETIME_MATRIX_FORMAT, D3DPOOL_DEFAULT , &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}
	if ((b->FrameTime[2] = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	b->currentFrameTimeTexture = 0;

	delete [] lpUVList;
	delete [] lpTriangleList;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::load()
{
	// Check if the hardware supports R2VB or not.
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK)
	{
		ERRORMSG("No R2VB support");
		return false;
	}
	// The maximum number of models that can be rendered at one time depends on the maximum size of the texture.
	maxBatchOnce = caps.MaxTextureHeight;

	// Load textures	
	if ((model[0].diffuse = renderer->addTexture(TEX_PATH "boy.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((model[1].diffuse = renderer->addTexture(TEX_PATH "ruby.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	
	// Load shaders
	if ((animationSH = renderer->addShader("animation.shd")) == SHADER_NONE) return false;
	if ((skinningSH = renderer->addShader("skinning.shd")) == SHADER_NONE) return false;
	if ((modelSH = renderer->addShader("model.shd")) == SHADER_NONE) return false;
	if ((batchAnimationSH = renderer->addShader("batchAnimation.shd")) == SHADER_NONE) return false;
	if ((batchSkinningSH = renderer->addShader("batchSkinning.shd")) == SHADER_NONE) return false;
	if ((batchModelSH = renderer->addShader("batchModel.shd")) == SHADER_NONE) return false;
	if ((vsModelSH = renderer->addShader("vsModel.shd")) == SHADER_NONE) return false;
	
	StreamVertexAttribute quadAttribs[] = { 
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2	};
	if ((QuadVF = renderer->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == VF_NONE) return false;

	float quadV[4][6] = {
		-1,  1, 0, 1, 0, 0,
		 1,  1, 0, 1, 1, 0,
		 1, -1, 0, 1, 1, 1,
		-1, -1, 0, 1, 0, 1,
	};
	// Create the quad vertex buffer	
	if ((QuadVB = renderer->addVertexBuffer(4 * 6 * (sizeof(float)))) == VB_NONE) return false;
	float *dest = (float *) renderer->lockVertexBuffer(QuadVB);
		stx_memcpy(dest, quadV, sizeof(float)*6*4);
	renderer->unlockVertexBuffer(QuadVB);

	StreamVertexAttribute batchQuadAttribs[] = { 
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2};
	if ((batch_QuadVF = renderer->addVertexFormat(batchQuadAttribs, elementsOf(batchQuadAttribs))) == VF_NONE) return false;

	float batchQuadV[4][6] = {
		-1,  1, 0, 1, 0, 0,
		 1,  1, 0, 1, 1, 0,
		 1, -1, 0, 1, 1, 1,
		-1, -1, 0, 1, 0, 1,
	};
	// Create the quad vertex buffer for batch rendering
	if ((batch_QuadVB = renderer->addVertexBuffer(4 * 6 * (sizeof(float)))) == VB_NONE) return false;
	dest = (float *) renderer->lockVertexBuffer(batch_QuadVB);
		stx_memcpy(dest, batchQuadV, sizeof(float)*4*6);
	renderer->unlockVertexBuffer(batch_QuadVB);

	// Load model data
	if (!loadModel(&model[0], &batch[0], MODEL_PATH MODELNAME ".mdl")) return false;
	// Load skinning vertex maps
	if (!loadSkinningVertexMap(&model[0], MODEL_PATH MODELNAME ".skn")) return false;
	// Load animation maps
	if (!loadBoneAnimationMap(&model[0], &batch[0], MODEL_PATH MODELNAME ".ani")) return false;

	// Load model data
	if (!loadModel(&model[1], &batch[1], MODEL_PATH MODELNAME_2 ".mdl")) return false;
	// Load skinning vertex maps
	if (!loadSkinningVertexMap(&model[1], MODEL_PATH MODELNAME_2 ".skn")) return false;
	// Load animation maps
	if (!loadBoneAnimationMap(&model[1], &batch[1], MODEL_PATH MODELNAME_2 ".ani")) return false;

	resetModels();

	time = 1;
	interpolate = 0.0f;
	currentModel = 0;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::onSize(const int w, const int h)
{
	D3D9App::onSize(w, h);
	numModelSlider->setPosition(w - 40.0f, 10);
	//numModelSlider->setSize(26, h - 20.0f);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::onReset()
{
	return true;
}
//-----------------------------------------------------------------------------------------
// Reset model's AI data.
//-----------------------------------------------------------------------------------------
void App::resetModels()
{
	for (int x=0; x<MAX_NUM_MODELS; x++)
	{
		modelAI[x].bBounce = false;
		modelAI[x].bInSafeZone = true;
		modelAI[x].Deg = (float)(rand()%360);
		modelAI[x].Speed = (1.5f + rand()%50/10.0f)*SCALE;
		Quat.x = sinf(modelAI[x].Deg*D2R/2) * 0.0f;
		Quat.y = sinf(modelAI[x].Deg*D2R/2) * 1.0f;
		Quat.z = sinf(modelAI[x].Deg*D2R/2) * 0.0f;
		Quat.w = cosf(modelAI[x].Deg*D2R/2);
		modelAI[x].Trans = identity4();
		Quaternion2float4x4(modelAI[x].Trans, Quat);
		modelAI[x].Trans.elem[0][3] = (float)(rand()%3000-1500)*SCALE;
		modelAI[x].Trans.elem[1][3] = 0.0f*SCALE;
		modelAI[x].Trans.elem[2][3] = (float)(rand()%3000-1500)*SCALE;

		modelAI[x].time = (float)(rand()%((int)RUN_T1))+1.0f;
		modelAI[x].interpolate = (rand()%10)/10.0f;

		modelAI[x].bBounceByGround = false;
		modelAI[x].Scale.y = (rand()%60)/100.0f + 0.8f;

		modelAI[x].currentState = RUN;
	}
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to calculate new bone matrices.
//-----------------------------------------------------------------------------------------
void App::psCalculateAnimation(float t, float interp)
{
	renderer->changeRenderTarget(model[currentModel].boneMatrix);

		renderer->setVertexFormat(QuadVF);
		renderer->setVertexBuffer(QuadVB, 0);renderer->setShader(animationSH);
		renderer->setTexture("boneAnimation", model[currentModel].boneAnimation);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->setCullFace(BACK);
		renderer->setShaderConstant4f("time_interp", float4(t-1, t, interp, 0.0f));
		renderer->setShaderConstant1f("XBias", 0.1f/(model[currentModel].numBones*4.0f));
		renderer->setShaderConstant1f("iBoneAnimationHeight", 1.0f/model[currentModel].numFrames);
		renderer->apply();	
		
		dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to skin the vertices of the model[currentModel].
//-----------------------------------------------------------------------------------------
void App::psSkinning()
{
	renderer->changeRenderTarget(model[currentModel].R2VB_VBO);

		renderer->setVertexFormat(QuadVF);
		renderer->setVertexBuffer(QuadVB, 0);
		renderer->setShader(skinningSH);
		renderer->setTexture("skinningVertex", model[currentModel].skinningVertex);
		renderer->setTexture("vertexBoneIndex", model[currentModel].vertexBoneIndex);
		renderer->setTexture("vertexWeight", model[currentModel].vertexWeight);
		renderer->setTexture("boneMatrix", model[currentModel].boneMatrix);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->setShaderConstant1f("xBias", 0.1f/(model[currentModel].vertexTextureWidth*2.0f));
		renderer->setShaderConstant4f("bias", float4(0.1f/(model[currentModel].numBones*4.0f), 1.1f/(model[currentModel].numBones*4.0f), 2.1f/(model[currentModel].numBones*4.0f), 3.1f/(model[currentModel].numBones*4.0f)));
		renderer->apply();

		dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}
//-----------------------------------------------------------------------------------------
// Render model using R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void App::psDrawModel(int mi)
{
	renderer->setShader(modelSH);
	renderer->setVertexFormat(model[currentModel].vertexFormat);
	renderer->setVertexBuffer(model[currentModel].UVBuffer, 0);
	renderer->setVertexBuffer(model[currentModel].R2VB_DummyVBO, 1);
	renderer->setIndexBuffer(model[currentModel].indexBuffer);	
	renderer->setTexture("diffuseMap", model[currentModel].diffuse);	
	renderer->setDepthFunc(LESS);
	renderer->setShaderConstant4x4f("mvp", MVP);
	renderer->setShaderConstant4x4f("trans", modelAI[mi].Trans);
	renderer->apply();	

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, model[currentModel].numVertices, 0, model[currentModel].numTriangles);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to calculate new bone matrices of multiple models.
//-----------------------------------------------------------------------------------------
void App::psBatchCalculateAnimation(int num)
{
	renderer->changeRenderTarget(batch[currentModel].boneMatrix);

		renderer->setVertexFormat(QuadVF);
		renderer->setShader(batchAnimationSH);
		renderer->setTexture("boneAnimation", model[currentModel].boneAnimation);
		renderer->setTexture("trans", batch[currentModel].FrameTime[batch[currentModel].currentFrameTimeTexture]);	
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->setCullFace(BACK);
		renderer->setShaderConstant1f("XBias", 0.1f/(model[currentModel].numBones*4.0f));
		renderer->setShaderConstant1f("iBoneAnimationHeight", 1.0f/model[currentModel].numFrames);
		renderer->apply();

		static float batchQuadV[4][6] = {
			-1, 1, 0, 1, 0, 0,
			1,  1, 0, 1, 1, 0,
			1, -1, 0, 1, 1, 1,
			-1, -1, 0, 1, 0, 1, 
		};

		float bias = (float)num/(float)(maxBatchOnce);
		// Preventing from wasting fillrate, we don't need to render
		// the whole quad when the number of models is not the maximum value.
		// Adjusting texture coordinate and position to render only a portion of the quad.
		batchQuadV[2][1] = 1.0f - bias*2.0f;
		batchQuadV[3][1] = 1.0f - bias*2.0f;
		batchQuadV[2][5] = bias;
		batchQuadV[3][5] = bias;

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, batchQuadV, sizeof(float)*6);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to transform vertices of multiple models.
//-----------------------------------------------------------------------------------------
void App::psBatchSkinning(int num)
{
	renderer->changeRenderTarget(batch[currentModel].R2VB_VBO);

		renderer->setVertexFormat(QuadVF);
		renderer->setShader(batchSkinningSH);
		renderer->setTexture("skinningVertex", model[currentModel].skinningVertex);
		renderer->setTexture("vertexBoneIndex", model[currentModel].vertexBoneIndex);
		renderer->setTexture("vertexWeight", model[currentModel].vertexWeight);
		renderer->setTexture("boneMatrix", batch[currentModel].boneMatrix);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->setShaderConstant1f("xBias", 0.1f/(model[currentModel].vertexTextureWidth*2.0f));
		renderer->setShaderConstant1f("yBias", 0.1f/(float)maxBatchOnce);
		renderer->setShaderConstant1f("numBatch", (float)maxBatchOnce);
		renderer->setShaderConstant4f("bias", float4(0.1f/(model[currentModel].numBones*4.0f), 1.1f/(model[currentModel].numBones*4.0f), 2.1f/(model[currentModel].numBones*4.0f), 3.1f/(model[currentModel].numBones*4.0f)));
		renderer->apply();

		static float batchQuadV[4][6] = {
			-1, 1, 0, 1, 0, 0,
			1,  1, 0, 1, 1, 0,
			1, -1, 0, 1, 1, 1,
			-1, -1, 0, 1, 0, 1, 
		};

		float bias = (float)num/(float)maxBatchOnce;
		// Preventing from wasting fillrate, we don't need to render
		// the whole quad when the number of models is not the maximum value.
		// Adjusting texture coordinate and position to render only a portion of the quad.
		batchQuadV[2][1] = 1.0f - bias*2.0f;
		batchQuadV[3][1] = 1.0f - bias*2.0f;
		batchQuadV[2][5] = bias;
		batchQuadV[3][5] = bias;

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, batchQuadV, sizeof(float)*6);
}
//-----------------------------------------------------------------------------------------
// Render multiple models using one R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void App::psBatchDrawModel(int num)
{
	renderer->setShader(batchModelSH);
	renderer->setVertexFormat(batch[currentModel].vertexFormat);
	renderer->setVertexBuffer(batch[currentModel].UVBuffer, 0);
	renderer->setVertexBuffer(model[currentModel].R2VB_OnceDummyVBO, 1);
	renderer->setIndexBuffer(batch[currentModel].indexBuffer);	
	renderer->setTexture("diffuseMap", model[currentModel].diffuse);	
	renderer->setDepthFunc(LESS);
	renderer->setShaderConstant4x4f("mvp", MVP);
	renderer->apply();	
	// Render multiple models at one time.
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, num*model[currentModel].vertexTextureWidth*model[currentModel].vertexTextureHeight, 0, num*model[currentModel].numTriangles);
}
//-----------------------------------------------------------------------------------------
// Pixel shader animation 
//-----------------------------------------------------------------------------------------
void App::drawPixelShaderAnimation()
{
	// Enable render to vertex buffer extension
    dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

	if (sameAnimation == animationMenu->getCheckedRadioButton(1))
	{
		psCalculateAnimation(time, interpolate);
		psSkinning();			
		renderer->resetRenderTarget();

		// Bind the render target to the DMAP sampler
		dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(model[currentModel].R2VB_VBO));
		// Tell the driver that stream 1 is to be fetched from the DMAP texture
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));	

		uint Counter = 0;
		for (int x=0; x<numModels; x++)
		{
			currentModel = 1;//x%2;
			psDrawModel(x);
			Counter ++;
			if (Counter > maxBatchOnce)
			{
				Counter = 0;
			}
		}
		// Stream 1 restored to regular vertex buffer mode
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
		// Unbind the DMAP texture
		dev->SetTexture(D3DDMAPSAMPLER, NULL);
	}
	else if (differentAnimation == animationMenu->getCheckedRadioButton(1))
	{
		// Tell the driver that stream 1 is to be fetched from the DMAP texture
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));	

		static bool bInit = false;

		int Count = 0;
		int NumData;
		int modelIndex = 0;
		while (Count < numModels)
		{
			modelIndex++;
			currentModel = modelIndex%2;
			uint Num = (numModels - Count);
			if (Num >= maxBatchOnce)
			{
				Num = maxBatchOnce;
				NumData = Num;
			}
			else
			{
				NumData = Num+1;
			}

			// upload the data of each character to the texture
			D3DLOCKED_RECT rect;
			LPDIRECT3DTEXTURE9 lpTex = (LPDIRECT3DTEXTURE9)(((D3D9Renderer *) renderer)->getD3DTexture(batch[currentModel].FrameTime[batch[currentModel].currentFrameTimeTexture]));
			if (lpTex->LockRect(0, &rect, NULL, D3DLOCK_DISCARD) == D3D_OK)
			{
				FRAMETIME_MATRIX_VALUE* lpC = (FRAMETIME_MATRIX_VALUE*)rect.pBits;
				for (int i=0; i<NumData; i++)
				{
					float *lpV = (float*)(&(modelAI[Count+i].Trans.elem[0][0]));
					for (int j=0; j<16; j++)
					{
						*lpC++ = FRAMETIME_MATRIX_VALUE(lpV[j]);
					}
				}
				lpTex->UnlockRect(0);
			}
			psBatchCalculateAnimation(Num);
			psBatchSkinning(Num);
			renderer->resetRenderTarget();

			// Bind the render target to the DMAP sampler
			dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(batch[currentModel].R2VB_VBO));
			
			psBatchDrawModel(Num);

			// Unbind the DMAP texture
			dev->SetTexture(D3DDMAPSAMPLER, NULL);

			Count += Num;
			batch[currentModel].currentFrameTimeTexture ++;
			if (batch[currentModel].currentFrameTimeTexture >= 3)
				batch[currentModel].currentFrameTimeTexture = 0;
		}
		// Stream 1 restored to regular vertex buffer mode
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	}

	// Disable render to vertex buffer extension
    dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));	
}
//-----------------------------------------------------------------------------------------
// Use vertex shader to calculate new bone matrices and send constants to shader. 
//-----------------------------------------------------------------------------------------
void App::vsCalculateAnimation(float t, float interp, float scale)
{
	static float4x4	M[48];

	renderer->setShader(vsModelSH);
	renderer->setVertexFormat(model[currentModel].vsVertexFormat);
	renderer->setVertexBuffer(model[currentModel].UVBuffer, 0);
	renderer->setVertexBuffer(model[currentModel].vsVertexBuffer, 1);
	renderer->setIndexBuffer(model[currentModel].indexBuffer);	
	renderer->setDepthFunc(LESS);
	renderer->setCullFace(BACK);	
	renderer->apply();	

	for (DWORD i=0; i<model[currentModel].numBones; i++)
	{
		// compute bone matrices of the current frame
		for (int x= 0; x<4; x++)
			for (int y= 0; y<4; y++)
				M[i].elem[x][y] = lerp(model[currentModel].bonesMatrix[(int)(t)][i].elem[x][y], model[currentModel].bonesMatrix[(int)(t+1)][i].elem[x][y], interp);

		// do some scale
		M[i].elem[0][0] *= scale; M[i].elem[0][1] *= scale; M[i].elem[0][2] *= scale;
		M[i].elem[1][0] *= scale; M[i].elem[1][1] *= scale; M[i].elem[1][2] *= scale;
		M[i].elem[2][0] *= scale; M[i].elem[2][1] *= scale; M[i].elem[2][2] *= scale;
	}
    renderer->changeShaderConstantArray4x4f("boneMatrix", M, model[currentModel].numBones);
}
//-----------------------------------------------------------------------------------------
// Render model using regular vertex buffer.
//-----------------------------------------------------------------------------------------
void App::vsDrawModel(int mi)
{
	renderer->setShader(vsModelSH);
	renderer->setVertexFormat(model[currentModel].vsVertexFormat);
	renderer->setVertexBuffer(model[currentModel].UVBuffer, 0);
	renderer->setVertexBuffer(model[currentModel].vsVertexBuffer, 1);
	renderer->setIndexBuffer(model[currentModel].indexBuffer);	
	renderer->setDepthFunc(LESS);
	renderer->setCullFace(BACK);
	renderer->setTexture("diffuseMap", model[currentModel].diffuse);	
	renderer->setShaderConstant4x4f("mvp", MVP);	
	renderer->setShaderConstant4x4f("trans", modelAI[mi].Trans);
	renderer->apply();

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, model[currentModel].numVertices, 0, model[currentModel].numTriangles);
}
//-----------------------------------------------------------------------------------------
// Vertex shader animation.
//-----------------------------------------------------------------------------------------
void App::drawVertexShaderAnimation()
{
	if (sameAnimation == animationMenu->getCheckedRadioButton(1))
	{
        vsCalculateAnimation(time, interpolate);
		uint Counter = 0;
		for (int x=0; x<numModels; x++)
		{
			currentModel = 1;//x%2;
			vsDrawModel(x);
			Counter++;
			if (Counter > maxBatchOnce)
			{
				Counter = 0;
			}
		}
	}
	else if (differentAnimation == animationMenu->getCheckedRadioButton(1))
	{
		uint Counter = 0;
		for (int x=0; x<numModels; x++)
		{
			currentModel = x%2;
			vsCalculateAnimation(modelAI[x].time, modelAI[x].interpolate, modelAI[x].Scale.y);			
			vsDrawModel(x);
			Counter++;
			if (Counter > maxBatchOnce)
			{
				Counter = 0;
			}
		}
	}
}
//-----------------------------------------------------------------------------------------
// AI loop, control the movement of every character in the scene.
//-----------------------------------------------------------------------------------------
void App::AILoop()
{
	static float Grav = -0.7f*SCALE;

	if (lastAI != AIMenu->getCheckedRadioButton(1))
		resetModels();

	if (differentAnimation == animationMenu->getCheckedRadioButton(1)) // every character has different animation
	{
		for (int x=0; x<numModels; x++)
		{
			vec3		Dist = vec3(modelAI[x].Trans.elem[0][3], modelAI[x].Trans.elem[1][3], modelAI[x].Trans.elem[2][3]) - camPos;
			float		Len;	

			Len = length(Dist);
			// if the character collide the camera 
			if (Len < 150.0f*SCALE)
			{
				if (modelAI[x].currentState != DYING)
				{
					modelAI[x].currentState = DYING;
					modelAI[x].time = DIE_T0;
					modelAI[x].interpolate = 0.0f;
					modelAI[x].upSpeed = (8.0f + ((rand()%800)/100.0f))*SCALE;
					modelAI[x].flyDir = Dist/Len;
					modelAI[x].bDie = false;
					modelAI[x].bBounceByGround = false;
				}
			}
			else
			{
				if (!(rand()%600)) // Sometimes they want dancing
				{
					if (modelAI[x].currentState == RUN)
					{
						modelAI[x].currentState = DANCE;
						modelAI[x].time = DANCE_T0;
						modelAI[x].interpolate = 0.0f;
					}
				}
				else if (!(rand()%600)) // Sometimes they want kicking
				{
					if (modelAI[x].currentState == RUN)
					{
						modelAI[x].currentState = KICK;
						modelAI[x].time = KICK_T0;
						modelAI[x].interpolate = 0.0f;
					}
				}
			}
		}
	}

	// AI for bounce
	if (bounce == AIMenu->getCheckedRadioButton(1))
	{
		for (int x=0; x<numModels; x++)
		{
			if (modelAI[x].Deg > 360.0f)
			{
				modelAI[x].Deg -= 360.0f;
			}
			else if (modelAI[x].Deg < -360.0f)
			{
				modelAI[x].Deg += 360.0f;
			}

			switch (modelAI[x].currentState)
			{
				// if the character is running
				case RUN:
					// if the character is out of the boundary
					if (modelAI[x].bBounce)
					{
						// turn the character until he face to the origin point
						modelAI[x].Deg += 3.0f;
						float3    v1, v2;
						float     d;
						v1 = float3(modelAI[x].Trans.elem[0][3], 0.0f, modelAI[x].Trans.elem[2][3]);
						v2 = float3(modelAI[x].Trans.elem[0][2], 0.0f, modelAI[x].Trans.elem[2][2]);
						v1 = normalize(v1);
						d = dot(v1, v2);
						if (d > 0.9f)
							modelAI[x].bBounce = false;

						Quat.y = sinf(modelAI[x].Deg*D2R/2.0f);
						Quat.w = cosf(modelAI[x].Deg*D2R/2.0f);
						Quaternion2float4x4(modelAI[x].Trans, Quat);
					}
					else // if the character is inside the boundary
					{
						// turn a degree randomly 
						if (!(rand()%20))
						{
							modelAI[x].Deg += 3.0f;
							Quat.y = sinf(modelAI[x].Deg*D2R/2.0f);
							Quat.w = cosf(modelAI[x].Deg*D2R/2.0f);
							Quaternion2float4x4(modelAI[x].Trans, Quat);
						}
					}	
					// if the character is running and not dead, move the character
					modelAI[x].Trans.elem[0][3] -= modelAI[x].Trans.elem[0][2]*modelAI[x].Speed;
					modelAI[x].Trans.elem[1][3] -= modelAI[x].Trans.elem[1][2]*modelAI[x].Speed;
					modelAI[x].Trans.elem[2][3] -= modelAI[x].Trans.elem[2][2]*modelAI[x].Speed;
					break;

				// if the caharcter is dying
				case DYING: 
					if (!modelAI[x].bDie)
					{
						// charcater fly away and do some physical computing
						modelAI[x].Trans.elem[0][3] += modelAI[x].flyDir.x*10.0f*SCALE;					
						modelAI[x].Trans.elem[2][3] += modelAI[x].flyDir.z*10.0f*SCALE;					
						modelAI[x].Trans.elem[1][3] += modelAI[x].upSpeed;	
						modelAI[x].upSpeed += Grav;
						if (modelAI[x].upSpeed < 0.0f)
						{
							if (modelAI[x].Trans.elem[1][3] <= 0.0f)
							{
								modelAI[x].Trans.elem[1][3] = 0.0f;
								if (modelAI[x].upSpeed < -6.0f*SCALE)
								{
									modelAI[x].upSpeed *= -0.6f;
									modelAI[x].bBounceByGround = true;
									modelAI[x].time = DIE_T1;
								}
								else
									modelAI[x].bDie = true;
							}
						}
					}
					break;
			}
			
			if (modelAI[x].bInSafeZone) //character in safe zone
			{
				if (modelAI[x].Trans.elem[0][3] > 2500.0f*SCALE || modelAI[x].Trans.elem[0][3] < -2500.0f*SCALE ||
					modelAI[x].Trans.elem[1][3] > 2500.0f*SCALE || modelAI[x].Trans.elem[1][3] < -2500.0f*SCALE ||
					modelAI[x].Trans.elem[2][3] > 2500.0f*SCALE || modelAI[x].Trans.elem[2][3] < -2500.0f*SCALE)
				{
					// charcater out of safe zone, then need to turn him back into the boundary
					modelAI[x].bBounce = true;
					modelAI[x].bInSafeZone = false;
				}
			}
			else 
			{
				if (modelAI[x].Trans.elem[0][3] < 2000.0f*SCALE && modelAI[x].Trans.elem[0][3] > -2000.0f*SCALE &&
					modelAI[x].Trans.elem[1][3] < 2000.0f*SCALE && modelAI[x].Trans.elem[1][3] > -2000.0f*SCALE &&
					modelAI[x].Trans.elem[2][3] < 2000.0f*SCALE && modelAI[x].Trans.elem[2][3] > -2000.0f*SCALE)
				{
					// charcater out of the boundary, go into safe zone
					modelAI[x].bInSafeZone = true;
				}
			}
		}
	}
	else if (respawn == AIMenu->getCheckedRadioButton(1)) // AI for respawn
	{
		for (int x=0; x<numModels; x++)
		{
			// turn a degree randomly 
			if (!(rand()%20))
			{
				modelAI[x].Deg += 3.0f*D2R;
				Quat.y = sinf(modelAI[x].Deg/2) * 1.0f;
				Quat.w = cosf(modelAI[x].Deg/2.0f);
				Quaternion2float4x4(modelAI[x].Trans, Quat);
			}

			switch (modelAI[x].currentState)
			{
				// if the character is running
				case RUN:
					// if the character is running and not dead, move the character
					modelAI[x].Trans.elem[0][3] -= modelAI[x].Trans.elem[0][2]*modelAI[x].Speed;
					modelAI[x].Trans.elem[1][3] -= modelAI[x].Trans.elem[1][2]*modelAI[x].Speed;
					modelAI[x].Trans.elem[2][3] -= modelAI[x].Trans.elem[2][2]*modelAI[x].Speed;
					break;

				// if the caharcter is dying
				case DYING:
					if (!modelAI[x].bDie)
					{
						// charcater fly away and do some physical computing
						modelAI[x].Trans.elem[0][3] += modelAI[x].flyDir.x*10.0f*SCALE;					
						modelAI[x].Trans.elem[2][3] += modelAI[x].flyDir.z*10.0f*SCALE;					
						modelAI[x].Trans.elem[1][3] += modelAI[x].upSpeed;	
						modelAI[x].upSpeed += Grav;
						if (modelAI[x].upSpeed < 0.0f)
						{
							if (modelAI[x].Trans.elem[1][3] <= 0.0f)
							{
								modelAI[x].Trans.elem[1][3] = 0.0f;
								if (modelAI[x].upSpeed < -6.0f*SCALE)
								{
									modelAI[x].upSpeed *= -0.6f;
									modelAI[x].bBounceByGround = true;
									modelAI[x].time = DIE_T1;
								}
								else
									modelAI[x].bDie = true;
							}
						}
					}
					break;				
			}
			
			// if the character is out of the boundary
			if (modelAI[x].Trans.elem[0][3] > 2500.0f*SCALE || modelAI[x].Trans.elem[0][3] < -2500.0f*SCALE ||
				modelAI[x].Trans.elem[1][3] > 2500.0f*SCALE || modelAI[x].Trans.elem[1][3] < -2500.0f*SCALE ||
				modelAI[x].Trans.elem[2][3] > 2500.0f*SCALE || modelAI[x].Trans.elem[2][3] < -2500.0f*SCALE)
			{
				// respawn the character inside the boundary
				modelAI[x].Trans.elem[0][3] = (float)(rand()%3000-1500)*SCALE;
				modelAI[x].Trans.elem[1][3] = 0.0f;
				modelAI[x].Trans.elem[2][3] = (float)(rand()%3000-1500)*SCALE;
			}
		}
	}	

	// play the animation
	if (sameAnimation == animationMenu->getCheckedRadioButton(1)) // every character has same animation
	{
		interpolate += 1.0f;
		if (interpolate >= 1.0f)
		{
			interpolate -= 1.0f;
			time += 1.0f;

			if (time >= RUN_T1)
				time = RUN_T0;
		}
	}
	else // every character has different animation
	{		
		for (int x=0; x<numModels; x++)
		{
			switch (modelAI[x].currentState)
			{
				case RUN:
					modelAI[x].interpolate += 1.0f;
					if (modelAI[x].interpolate >= 1.0f)
					{
						modelAI[x].interpolate -= 1.0f;
						modelAI[x].time += 1.0f;
						if (modelAI[x].time >= RUN_T1)
							modelAI[x].time = RUN_T0;
					}
					break;

				case DYING:
					if (!modelAI[x].bDie)
					{
						modelAI[x].interpolate += 1.0f;
						if (modelAI[x].interpolate >= 1.0f)
						{
							modelAI[x].interpolate -= 1.0f;
							modelAI[x].time += 1.0f;
							if (modelAI[x].bBounceByGround)
							{
								if (modelAI[x].time >= DIE_T2)
								{
									modelAI[x].time = DIE_T2;
									modelAI[x].interpolate = 0.0f;
								}
							}
							else // if the character is dead, stop the animation at the last frame
							{
								if (modelAI[x].time >= DIE_T1)
								{
									modelAI[x].time = DIE_T1;
									modelAI[x].interpolate = 0.0f;
								}
							}
						}
					}
					else
					{
						modelAI[x].currentState = RESPAWN;
						modelAI[x].time = RESPAWN_T0;
						modelAI[x].interpolate = 0.0f;
						modelAI[x].bDie = false;

					}
					break;

				// if the character is dancing
				case DANCE:
					modelAI[x].interpolate += 1.0f;
					if (modelAI[x].interpolate >= 1.0f)
					{
						modelAI[x].interpolate -= 1.0f;
						modelAI[x].time += 1.0f;
						if (modelAI[x].time >= DANCE_T1)
						{
							// Run again.
							modelAI[x].currentState = RUN;
							modelAI[x].time = RUN_T0;
							modelAI[x].interpolate = 0.0f;
						}
					}
					break;

					// if the character is kicking
				case KICK:
					modelAI[x].interpolate += 1.0f;
					if (modelAI[x].interpolate >= 1.0f)
					{
						modelAI[x].interpolate -= 1.0f;
						modelAI[x].time += 1.0f;
						if (modelAI[x].time >= KICK_T1)
						{
							// Run again.
							modelAI[x].currentState = RUN;
							modelAI[x].time = RUN_T0;
							modelAI[x].interpolate = 0.0f;
						}
					}
					break;

				// if the character is respawning
				case RESPAWN:
					modelAI[x].interpolate += 1.0f;
					if (modelAI[x].interpolate >= 1.0f)
					{
						modelAI[x].interpolate -= 1.0f;
						modelAI[x].time += 1.0f;
						if (modelAI[x].time >= RESPAWN_T1)
						{
							// Run again.
							modelAI[x].currentState = RUN;
							modelAI[x].time = RUN_T0;
							modelAI[x].interpolate = 0.0f;
						}
					}
					break;
			}
		}
	}
	

	if (differentAnimation == animationMenu->getCheckedRadioButton(1) && psAnimation == menu->getCheckedRadioButton(1))
	{
		// update the data of each character
		for (int x=0; x<numModels; x++)
		{
			modelAI[x].Trans.elem[3][0] = modelAI[x].time-1;
			modelAI[x].Trans.elem[3][1] = modelAI[x].time;
			modelAI[x].Trans.elem[3][2] = modelAI[x].interpolate;
			modelAI[x].Trans.elem[3][3] = modelAI[x].Scale.y;
		}
	}
	else
	{
		// reset the data of each character
		for (int x=0; x<numModels; x++)
		{
			modelAI[x].Trans.elem[3][0] = 0.0f;
			modelAI[x].Trans.elem[3][1] = 0.0f;
			modelAI[x].Trans.elem[3][2] = 0.0f;
			modelAI[x].Trans.elem[3][3] = 1.0f;
		}
	}
}
//-----------------------------------------------------------------------------------------
// Render the frame
//-----------------------------------------------------------------------------------------
void App::drawFrame()
{
	float4x4 rootM;
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, SCALE, 20000*SCALE, true);

	float4x4 modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	MVP = projection * modelview;

	numModels = (int)numModelSlider->getValue()*4;

	AILoop();

	renderer->clearBuffers(COLOR_BUFFER | DEPTH_BUFFER, float4(0.5, 0.5, 0.5, 0));
	if (vsAnimation == menu->getCheckedRadioButton(1))
		drawVertexShaderAnimation();
	else
		drawPixelShaderAnimation();

	lastAI = AIMenu->getCheckedRadioButton(1);

	char  str[256];
	sprintf(str, "%d models", numModels);

	renderer->setCullFace(BACK);
	renderer->setBlending(CONSTANT_COLOR, ZERO);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->apply();
	renderer->changeBlendConstantColor(vec4(1, 0.6f, 0.3f, 0));
	mainFont.drawText(renderer, str, 120, 7, 32, 40, 0.17f, -0.12f, HA_LEFT, VA_TOP);
}
