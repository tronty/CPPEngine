//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses an R2VB technique to compute the per-face normal of a skeletal
// animation model on the GPU, then uses the computed per-face normal for shadow
// volume extrusion.
//-----------------------------------------------------------------------------

#include "App.h"
#include "../../Framework/Direct3D/atir2vb.h"

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

BaseApp *app = new App();
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::setup(char *args[], int nArguments)
{
	width  = 800;
	height = 600;
	speed = 10;
	stencilBits = 8;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::resetCamera()
{
	camPos = float3(-10, 20, -20);
	wx = 0.5f;
	wy = -0.5f;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::initGUI()
{
	lightMenu  = mainMenu->addMenu("Light");
	oneLight   = lightMenu->addItem("1 Light",  MENU_CHECKABLE);
	twoLight   = lightMenu->addItem("2 Lights", MENU_CHECKABLE);
	threeLight = lightMenu->addItem("3 Lights", MENU_CHECKABLE);
	fourLight  = lightMenu->addItem("4 Lights", MENU_CHECKABLE);
	lightMenu->setRadioButtonGroup(oneLight, fourLight, 1);
	lightMenu->toggleItemCheck(twoLight);

	D3D9App::initGUI();
}
//-----------------------------------------------------------------------------------------
// Load skinning vertex data into textures.
// It includes vertex position, bone index and weight value.
//-----------------------------------------------------------------------------------------
bool App::loadSkinningVertexMap(char* name)
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

	model.pVertex = new float3[NumVertices];
	vertexOffset = new float4[NumVertices];
	vertexNormal = new float4[NumVertices];

	for (uint i=0; i<NumVertices; i++)
	{
		fread(&vertexOffset[i], sizeof(float4), 1, fp);
		fseek(fp, sizeof(float4)*3, SEEK_CUR);
		fread(&vertexNormal[i], sizeof(float4), 1, fp);
		vertexNormal[i].w = 0.0f;
		model.pVertex[i].x = vertexOffset[i].x;
		model.pVertex[i].y = vertexOffset[i].y;
		model.pVertex[i].z = vertexOffset[i].z;
	}

	LPDIRECT3DTEXTURE9 tex;
	D3DLOCKED_RECT rect;

	// 1 pixel can store 1 vertex positions, 1 pixel can store 1 vertex normal, so one skinning vertex == 2 pixels.
	if (dev->CreateTexture(model.vertexTextureWidth*2, model.vertexTextureHeight, 1, 0, VERTEX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
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
	if ((model.skinningVertex = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	// 1 pixel can store 4 bone indices, so one vertex skinning bone == 1 pixel.
	if (dev->CreateTexture(model.vertexTextureWidth, model.vertexTextureHeight, 1, 0, BONE_INDEX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
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
			for (int j=0; j<4; j++)
			{
				fread(&V[j], sizeof(float), 1, fp);
				V[j] *= 4.0f;
				V[j] /= ((float)NumBones*4.0f);
				*lpC++ = BONE_INDEX_VALUE_FORMAT(V[j]);
			}
		}
		tex->UnlockRect(0);
	}
	if ((model.vertexBoneIndex = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	// 1 pixel can store 4 weight values, so one skinning vertex weight value == 1 pixel.
	if (dev->CreateTexture(model.vertexTextureWidth, model.vertexTextureHeight, 1, 0, VERTEX_WEIGHT_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
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
	if ((model.vertexWeight = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	fclose(fp);

	delete [] vertexOffset;
	delete [] vertexNormal;
	
	return true;
}
//-----------------------------------------------------------------------------------------
// Load bone animation matrices into texture.
//-----------------------------------------------------------------------------------------
bool App::loadBoneAnimationMap(char* name)
{
	FILE	*fp;

	fp = fopen(name, "rb");
	if (!fp)
		return false;

	fread(&model.numBones, sizeof(DWORD), 1, fp);
	fread(&model.numFrames, sizeof(DWORD), 1, fp);
	for (uint i=0; i<model.numFrames; i++)
		fread(model.bonesMatrix[i], sizeof(float4x4), model.numBones, fp);

	fclose(fp);

	LPDIRECT3DTEXTURE9 tex;
	if (dev->CreateTexture(4*model.numBones, model.numFrames, 1, 0, BONE_MATRIX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}
	D3DLOCKED_RECT rect;
	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		BONE_MATRIX_VALUE_FORMAT* lpC = (BONE_MATRIX_VALUE_FORMAT *)rect.pBits;
		for (uint f=0; f<model.numFrames; f++)
			for (DWORD i=0; i<model.numBones; i++)
			{
				float *lpM = &model.bonesMatrix[f][i].elem[0][0];
				for (int i=0; i<16; i++)
					*lpC++ = BONE_MATRIX_VALUE_FORMAT(lpM[i]);
			}
		tex->UnlockRect(0);
	}
	if ((model.boneAnimation = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	LPDIRECT3DTEXTURE9 d3drt0;
	// Create render target for storage of bones' transform matrices.
	// 4 pixels == 1 4x4 matrix, so a (4*model.numBones)x1 floating-point texture can store (model.numBones) bone matrices.
	if (dev->CreateTexture(4*model.numBones, 1, 1, D3DUSAGE_RENDERTARGET, BONE_MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt0, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((model.boneMatrix = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt0, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load model triangle and vertex data, that will be used for vertex shader animation.
// We also create the R2VB vertex buffer here.
//-----------------------------------------------------------------------------------------
bool App::createPlayer(char *modelname, char *skinname, char *animation)
{
	FILE	*fp;
	float2	*lpUVList;
	char Path[256];

	sprintf(Path, "%s%s", MODEL_PATH, modelname);
	fp = fopen(Path, "rb");
	if (!fp)
		return false;

	fread(&model.numVertices, sizeof(DWORD), 1, fp);
	model.vertexTextureWidth = model.numVertices;
	model.vertexTextureHeight = 1;
	if (model.vertexTextureWidth > (caps.MaxTextureWidth/2))
	{
		model.vertexTextureHeight = (model.vertexTextureWidth/(caps.MaxTextureWidth/2));
		if (model.vertexTextureWidth%(caps.MaxTextureWidth/2))
			model.vertexTextureHeight++;
		if (model.vertexTextureHeight%2)
			model.vertexTextureHeight++;
		model.vertexTextureWidth = (caps.MaxTextureWidth/2);
	}

	lpUVList = new float2[model.numVertices];

	// Create vertex declarations
	StreamVertexAttribute modelVertexAttribs[] = {
		1, TYPE_VERTEX,   R2VB_VERTEX_FORMAT, 4,
		1, TYPE_NORMAL,   R2VB_VERTEX_FORMAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
	if ((model.VF = renderer->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs))) == VF_NONE) return false;

	for (DWORD i=0; i<model.numVertices; i++) 
	{
		fseek(fp, sizeof(float4)*7, SEEK_CUR);
	}
	// Create the texture coordinates of the model
	if ((model.UVBuffer = renderer->addVertexBuffer(model.numVertices * (sizeof(R2VB_TEXCOORD_VALUE_FORMAT)*2))) == VB_NONE) return false;
	R2VB_TEXCOORD_VALUE_FORMAT *uvdest = (R2VB_TEXCOORD_VALUE_FORMAT *) renderer->lockVertexBuffer(model.UVBuffer);
		fread(lpUVList, sizeof(float2), model.numVertices, fp);
		for (uint j=0; j<model.numVertices; j++)
		{
			*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].x);
			*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].y);
		}
	renderer->unlockVertexBuffer(model.UVBuffer);

	fread(&model.numTriangles, sizeof(DWORD), 1, fp);
	model.pTriangleList = new WORD[model.numTriangles*3];

	// Create index buffer of the model
	if ((model.IB = renderer->addIndexBuffer(model.numTriangles*3, sizeof(WORD))) == IB_NONE) return false;
	WORD *iDest = (WORD *) renderer->lockIndexBuffer(model.IB);
		fread(model.pTriangleList, sizeof(WORD), model.numTriangles*3, fp);
		stx_memcpy(iDest, model.pTriangleList, sizeof(WORD)*model.numTriangles*3);
	renderer->unlockIndexBuffer(model.IB);

	/*
		Create render target for the model animation.
		Since this render target will be used as a vertex buffer we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	LPDIRECT3DTEXTURE9 d3drt;
	if (dev->CreateTexture(model.vertexTextureWidth*2, model.vertexTextureHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, R2VB_VBO_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((model.R2VB_VBO = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size;

	if (debugRuntime)
		size = (model.vertexTextureWidth*2) * model.vertexTextureHeight * 4 * sizeof(R2VB_VERTEX_VALUE_FORMAT);
	else
		size = 2;

	if ((model.R2VB_DummyVBO = renderer->addVertexBuffer(size)) == VB_NONE) return false;

	fclose(fp);

	delete [] lpUVList;
	
	sprintf(Path, "%s%s", MODEL_PATH, skinname);
	loadSkinningVertexMap(Path);
	sprintf(Path, "%s%s", MODEL_PATH, animation);
	loadBoneAnimationMap(Path);

	// Load textures
	if ((model.diffuseMap = renderer->addTexture(TEX_PATH "boy03.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	// Load shaders
	if ((model.animationSH = renderer->addShader("animation.hlsl")) == SHADER_NONE) return false;
	if ((model.skinningSH = renderer->addShader("skinning.hlsl")) == SHADER_NONE) return false;
	if ((model.modelSH = renderer->addShader("model.hlsl")) == SHADER_NONE) return false;
	if ((model.ambientModelSH = renderer->addShader("Model_Ambient.hlsl")) == SHADER_NONE) return false;
	if ((model.calculateNormalSH = renderer->addShader("normal.hlsl")) == SHADER_NONE) return false;
	if ((model.shadowVolumeSH = renderer->addShader("ShadowVolume.hlsl")) == SHADER_NONE) return false;

	StreamVertexAttribute quadAttribs[] = { 
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	if ((model.QuadVF = renderer->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == VF_NONE) return false;

	float quadV[4][6] = {
		-1,  1, 0, 1, 0, 0,
		 1,  1, 0, 1, 1, 0,
		 1, -1, 0, 1, 1, 1,
		-1, -1, 0, 1, 0, 1,
	};
	// Create the quad vertex buffer
	if ((model.QuadVB = renderer->addVertexBuffer(4 * 6 * (sizeof(float)))) == VB_NONE) return false;
	float *dest = (float *) renderer->lockVertexBuffer(model.QuadVB);
		stx_memcpy(dest, quadV, sizeof(float)*6*4);
	renderer->unlockVertexBuffer(model.QuadVB);

	model.transformMatrix = identity4();
	model.time = 1.0f;
	model.interpolate = 0.0f;

	return true;
}

struct EdgeData{
	bool bGenerated;
	WORD v0, v1, v2;
	vec3 p0, p1;
};
//-----------------------------------------------------------------------------------------
// Generate shadow volume mesh from the model's triangles. We generate a quad for every two
// adjacent triangles. The model must be closed for us to be able to generate a correct
// shadow volume mesh.
//-----------------------------------------------------------------------------------------
bool App::generateShadowVolume() 
{
	LPDIRECT3DTEXTURE9 tex;
	D3DLOCKED_RECT rect;
	EdgeData*lpEdgeData; 
	WORD *pVertexIndexMap;
	DWORD shadowVolumeTexWidth, shadowVolumeTexHeight;
	DWORD numIndex;
	DWORD numEdge;

	lpEdgeData = new EdgeData[model.numTriangles*3];
	pVertexIndexMap = new WORD[model.numTriangles*3*4*3];

	for (DWORD t=0; t<model.numTriangles; t++)
	{
		lpEdgeData[t*3+0].bGenerated = false;
		lpEdgeData[t*3+0].v0 = model.pTriangleList[t*3+0];
		lpEdgeData[t*3+0].p0 = model.pVertex[lpEdgeData[t*3+0].v0];
		lpEdgeData[t*3+0].v1 = model.pTriangleList[t*3+1];
		lpEdgeData[t*3+0].p1 = model.pVertex[lpEdgeData[t*3+0].v1];
		lpEdgeData[t*3+0].v2 = model.pTriangleList[t*3+2];

		lpEdgeData[t*3+1].bGenerated = false;
		lpEdgeData[t*3+1].v0 = model.pTriangleList[t*3+1];
		lpEdgeData[t*3+1].p0 = model.pVertex[lpEdgeData[t*3+1].v0];
		lpEdgeData[t*3+1].v1 = model.pTriangleList[t*3+2];
		lpEdgeData[t*3+1].p1 = model.pVertex[lpEdgeData[t*3+1].v1];
		lpEdgeData[t*3+1].v2 = model.pTriangleList[t*3+0];

		lpEdgeData[t*3+2].bGenerated = false;
		lpEdgeData[t*3+2].v0 = model.pTriangleList[t*3+2];
		lpEdgeData[t*3+2].p0 = model.pVertex[lpEdgeData[t*3+2].v0];
		lpEdgeData[t*3+2].v1 = model.pTriangleList[t*3+0];
		lpEdgeData[t*3+2].p1 = model.pVertex[lpEdgeData[t*3+2].v1];
		lpEdgeData[t*3+2].v2 = model.pTriangleList[t*3+1];
	}

	//find edge of every adjacent triangles
	numIndex = 0;
	numEdge = 0;
	for (DWORD t=0; t<model.numTriangles*3; t++)
	{
		if (!lpEdgeData[t].bGenerated)
		{
			pVertexIndexMap[numIndex++] = lpEdgeData[t].v0;
			pVertexIndexMap[numIndex++] = lpEdgeData[t].v1;
			pVertexIndexMap[numIndex++] = lpEdgeData[t].v2;

			pVertexIndexMap[numIndex++] = lpEdgeData[t].v1;
			pVertexIndexMap[numIndex++] = lpEdgeData[t].v2;
			pVertexIndexMap[numIndex++] = lpEdgeData[t].v0;

			lpEdgeData[t].bGenerated = true;
			bool bFind = false;
			for (DWORD j=0; j<model.numTriangles*3; j++)
			{
				if (j != t)
				{
					if (!lpEdgeData[j].bGenerated)
					{
						float Dist0 = distance(lpEdgeData[j].p0, lpEdgeData[t].p1);
						float Dist1 = distance(lpEdgeData[j].p1, lpEdgeData[t].p0);
						if (Dist0 <= 0.001f && Dist1 <= 0.001f)
						{
							// Record the vertex indices for two edge points
							lpEdgeData[j].bGenerated = true;

							pVertexIndexMap[numIndex++] = lpEdgeData[j].v1;
							pVertexIndexMap[numIndex++] = lpEdgeData[j].v2;
							pVertexIndexMap[numIndex++] = lpEdgeData[j].v0;

							pVertexIndexMap[numIndex++] = lpEdgeData[j].v0;
							pVertexIndexMap[numIndex++] = lpEdgeData[j].v1;
							pVertexIndexMap[numIndex++] = lpEdgeData[j].v2;

							bFind = true;
							break;
						}
					}
				}
			}

			if (bFind)
			{
				numEdge++;
			}
			else
			{
				// Mesh is not closed
				ERRORMSG("Couldn't create shadow volume because mesh is not closed");
			}
		}
	}

	// Create vertex declarations
	StreamVertexAttribute VertexAttribs[] = {
		1, TYPE_VERTEX, FORMAT_FLOAT, 4,
		1, TYPE_NORMAL, FORMAT_FLOAT, 4,
	};
	if ((model.shadowVolumeVF = renderer->addVertexFormat(VertexAttribs, elementsOf(VertexAttribs))) == VF_NONE) return false;

	model.numVerticesShadowVolume = numEdge*4;

	// Try to find a texture size that's roughly square
	ubyte shift = 0;
	while ((2U << (2 * shift)) < model.numVerticesShadowVolume) shift++;
	shadowVolumeTexWidth = 1 << shift;
	shadowVolumeTexHeight = (model.numVerticesShadowVolume + shadowVolumeTexWidth - 1) >> shift;

	// Create a texture to store triangle vertex indices of every vertex of the shadow volume mesh
	if (dev->CreateTexture(shadowVolumeTexWidth*2, shadowVolumeTexHeight, 1, 0, D3DFMT_A16B16G16R16, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}
	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		short *lpC = (short *)rect.pBits;

		for (uint i=0; i<numEdge; i++)
		{
			*lpC++ = short((float)pVertexIndexMap[i*12+0]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+1]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+2]*2.0f);
			*lpC++ = short(0.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+0]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+1]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+2]*2.0f);
			*lpC++ = short(1.0f);

			*lpC++ = short((float)pVertexIndexMap[i*12+3]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+4]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+5]*2.0f);
			*lpC++ = short(0.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+3]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+4]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+5]*2.0f);
			*lpC++ = short(1.0f);

			*lpC++ = short((float)pVertexIndexMap[i*12+6]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+7]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+8]*2.0f);
			*lpC++ = short(0.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+6]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+7]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+8]*2.0f);
			*lpC++ = short(1.0f);

			*lpC++ = short((float)pVertexIndexMap[i*12+9]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+10]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+11]*2.0f);
			*lpC++ = short(0.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+9]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+10]*2.0f);
			*lpC++ = short((float)pVertexIndexMap[i*12+11]*2.0f);
			*lpC++ = short(1.0f);
		}
		tex->UnlockRect(0);
	}
	if ((model.shadowVolumeVertexIndexMap = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	model.numTrianglesShadowVolume = numEdge*2;

	// Create the triangle index buffer of the shadow volume
	if ((model.shadowVolumeIB = renderer->addIndexBuffer(model.numTrianglesShadowVolume*3, sizeof(WORD))) == IB_NONE) return false;
	WORD *iDest = (WORD *) renderer->lockIndexBuffer(model.shadowVolumeIB);
	for (uint i=0; i<numEdge; i++)
	{
		*iDest++ = i*4 + 0;
		*iDest++ = i*4 + 1;
		*iDest++ = i*4 + 2;
		*iDest++ = i*4 + 2;
		*iDest++ = i*4 + 1;
		*iDest++ = i*4 + 3;
	}
	renderer->unlockIndexBuffer(model.shadowVolumeIB);

	LPDIRECT3DTEXTURE9 d3drt;
	// Create R2VB vertex buffer. Since the render target will be used as vertex buffer
	// we need to signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	if (dev->CreateTexture(shadowVolumeTexWidth*2, shadowVolumeTexHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((model.R2VB_ShadowVolumeNormal_VBO = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;



	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size;
	if (debugRuntime)
		size = (shadowVolumeTexWidth*2) * shadowVolumeTexHeight * 4 * sizeof(float);
	else
		size = 2;
	if ((model.R2VB_ShadowVolumeNormal_DummyVBO = renderer->addVertexBuffer(size)) == VB_NONE) return false;

	delete [] lpEdgeData;
	delete [] pVertexIndexMap;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load a floor model
//-----------------------------------------------------------------------------------------
bool Floor::load(char *name, Renderer* renderer)
{
	ATIModel::load(name);
	StreamVertexAttribute modelVertexAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};

	if ((VertexFormat = renderer->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs))) == VF_NONE) return false;
	// Create the vertex buffer for this model
	if ((VertexBuffer = renderer->addVertexBuffer(numVertices * (sizeof(float3) + sizeof(float2)))) == VB_NONE) return false;
	float *dest = (float *) renderer->lockVertexBuffer(VertexBuffer);
	for (DWORD i=0; i<numVertices; i++)
	{
		(*dest++) = pVPosition[i].x;
		(*dest++) = pVPosition[i].y;
		(*dest++) = pVPosition[i].z;
		(*dest++) = pVTexcoord[i].x;
		(*dest++) = pVTexcoord[i].y;
	}
	renderer->unlockVertexBuffer(VertexBuffer);
	// Create the index buffer for this model
	if ((IndexBuffer = renderer->addIndexBuffer(numTriangles*3, sizeof(DWORD))) == IB_NONE) return false;
	DWORD *iDest = (DWORD *) renderer->lockIndexBuffer(IndexBuffer);
		for (DWORD i=0; i<numTriangles; i++)
		{
			(*iDest++) = pTriangle[i].v[0];
			(*iDest++) = pTriangle[i].v[1];
			(*iDest++) = pTriangle[i].v[2];
		}
	renderer->unlockIndexBuffer(IndexBuffer);
	// load shaders for this model
	if ((Shader = renderer->addShader("floor.hlsl")) == SHADER_NONE) return false;
	if ((ambientShader = renderer->addShader("floor_ambient.hlsl")) == SHADER_NONE) return false;
	// load diffuse map for this model
	char FullName[256];
	sprintf(FullName, "%s%s", TEX_PATH, pMaterial[0].diffuseMap);
	if ((DiffuseMap = renderer->addTexture(FullName, FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((BumpMap = renderer->addTexture(TEX_PATH "layingrockBump.png", FILTER_TRILINEAR_ANISO, NORMALMAP)) == TEXTURE_NONE) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load all the resources the sample needs
//-----------------------------------------------------------------------------------------
bool App::load()
{
	// check if the hardware support R2VB?
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK)
	{
		ERRORMSG("No R2VB support");
		return false;
	}
	// check if the hardware support two sided stencil buffer?
	if ((caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED) == 0)
	{
		ERRORMSG("Two-sided stencil is not supported");
		return false;
	}

	// Load shaders
	if ((skybox = renderer->addShader("skybox.hlsl")) == SHADER_NONE) return false;

	// Load textures
	if ((env = renderer->addCubemap(
		TEX_PATH "Mountains/posx.jpg", TEX_PATH "Mountains/negx.jpg",
		TEX_PATH "Mountains/posy.jpg", TEX_PATH "Mountains/negy.jpg",
		TEX_PATH "Mountains/posz.jpg", TEX_PATH "Mountains/negz.jpg", FILTER_TRILINEAR)) == TEXTURE_NONE) return false;

	StreamVertexAttribute skyboxAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = renderer->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs))) == VF_NONE) return false;

	if (!floor.load(MODEL_PATH "floor/floor.am", renderer)) return false;

	if (!createPlayer("boy03/boy03.mdl", "boy03/boy03.skn", "boy03/boy03.ani")) return false;
	if (!generateShadowVolume()) return false;

	delete [] model.pVertex;
	delete [] model.pTriangleList;

	ambient.color = float4(0, 0, 0.3f, 1.0f);

	light[0].position.x = cosf(-60.0f*PI/180.0f)*20.0f;
	light[0].position.z = sinf(-60.0f*PI/180.0f)*20.0f;
	light[0].position.y = 20.0f;
	light[0].color = float4(0.45f, 0.45f, 0.45f, 1);

	light[1].position.x = cosf(90.0f*PI/180.0f)*30.0f;
	light[1].position.z = sinf(90.0f*PI/180.0f)*30.0f;
	light[1].position.y = 20.0f;
	light[1].color = float4(0.45f, 0.45f, 0.45f, 1);

	light[2].position.x = cosf(180.0f*PI/180.0f)*30.0f;
	light[2].position.z = sinf(180.0f*PI/180.0f)*30.0f;
	light[2].position.y = 40.0f;
	light[2].color = float4(0.45f, 0.45f, 0.45f, 1);

	light[3].position.x = cosf(270.0f*PI/180.0f)*30.0f;
	light[3].position.z = sinf(270.0f*PI/180.0f)*30.0f;
	light[3].position.y = 30.0f;
	light[3].color = float4(0.45f, 0.45f, 0.45f, 1);

	numLights = 2;

	onReset();
	return true;
}
//-----------------------------------------------------------------------------------------
// Render the skybox
//-----------------------------------------------------------------------------------------
void App::drawSkybox(const float4x4 &mvp)
{
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const ushort inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	renderer->setShader(skybox);
	renderer->setVertexFormat(skyboxVF);
	renderer->setTexture("Skybox", env);
	renderer->setMask(COLOR);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", mvp);

	dev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 12, inds, D3DFMT_INDEX16, verts, 3 * sizeof(float));
}
//-----------------------------------------------------------------------------------------
// Render the floor
//-----------------------------------------------------------------------------------------
void App::drawFloor(Light &lit, bool ambient)
{
	if (ambient) //ambient pass
	{
		renderer->setShader(floor.ambientShader);
		renderer->setMask(COLOR | DEPTH);
		renderer->setDepthFunc(LESS);
	}
	else //lighting pass
	{
		renderer->setShader(floor.Shader);
		renderer->setBlending(ONE, ONE);
		renderer->setMask(COLOR);
		renderer->setDepthFunc(LEQUAL);
	}
	renderer->setVertexFormat(floor.VertexFormat);
	renderer->setVertexBuffer(floor.VertexBuffer, 0);
	renderer->setIndexBuffer(floor.IndexBuffer);
	renderer->setTexture("DiffuseMap", floor.DiffuseMap);
	if (!ambient)
		renderer->setTexture("BumpMap", floor.BumpMap);
	renderer->setCullFace(BACK);	
	renderer->apply();
	renderer->changeShaderConstant4x4f("mvp", MVP);
	if (!ambient)
	{
		renderer->changeShaderConstant4f("LightPos", lit.position);
		renderer->changeShaderConstant4f("CameraPos", vec4(camPos, 1.0f));
	}
	renderer->changeShaderConstant4f("LightColor", lit.color);

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, floor.getVertexCount(), 0, floor.getTriangleCount());
}
//-----------------------------------------------------------------------------------------
// Play the animation of player.
//-----------------------------------------------------------------------------------------
void App::playPlayer()
{
	model.interpolate += frameTime*30.0f;
	if (model.interpolate >= 1.0f)
	{
		model.interpolate = 0.0f;
		model.time++;
		if (model.time >= 244.0f)
			model.time = 1.0f;
	}
	psCalculateAnimation(model.time, model.interpolate);
	psSkinning();
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to calculate new bone matrixes.
//-----------------------------------------------------------------------------------------
void App::psCalculateAnimation(float t, float interp)
{
	renderer->changeRenderTarget(model.boneMatrix);

		renderer->setVertexFormat(model.QuadVF);
		renderer->setVertexBuffer(model.QuadVB, 0);
		renderer->setShader(model.animationSH);
		renderer->setTexture("boneAnimation", model.boneAnimation);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		renderer->changeShaderConstant4f("time_interp", float4(t-1, t, interp, 0.0f));
		renderer->changeShaderConstant1f("XBias", 0.1f/(model.numBones*4.0f));
		renderer->changeShaderConstant1f("iBoneAnimationHeight", 1.0f/model.numFrames);
		
		dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to transform the model's vertices.
//-----------------------------------------------------------------------------------------
void App::psSkinning()
{
	renderer->changeRenderTarget(model.R2VB_VBO);

		renderer->setVertexFormat(model.QuadVF);
		renderer->setVertexBuffer(model.QuadVB, 0);
		renderer->setShader(model.skinningSH);
		renderer->setTexture("skinningVertex", model.skinningVertex);
		renderer->setTexture("vertexBoneIndex", model.vertexBoneIndex);
		renderer->setTexture("vertexWeight", model.vertexWeight);
		renderer->setTexture("boneMatrix", model.boneMatrix);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		renderer->changeShaderConstant1f("xBias", 0.1f/(model.vertexTextureWidth));
		renderer->changeShaderConstant4f("bias", float4(0.1f/(model.numBones*4.0f), 1.1f/(model.numBones*4.0f), 2.1f/(model.numBones*4.0f), 3.1f/(model.numBones*4.0f)));

		dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	renderer->resetRenderTarget();
}
//-----------------------------------------------------------------------------------------
// Render model using R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void App::psDrawModel(Light &lit, bool ambient)
{
	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(model.R2VB_VBO));

	if (ambient) //ambient pass
	{
		renderer->setShader(model.ambientModelSH);
		renderer->setMask(COLOR | DEPTH);
		renderer->setDepthFunc(LESS);
	}
	else  //lighting pass
	{
		renderer->setShader(model.modelSH);
		renderer->setBlending(ONE, ONE);
		renderer->setMask(COLOR);
		renderer->setDepthFunc(LEQUAL);
	}
	renderer->setVertexFormat(model.VF);
	renderer->setVertexBuffer(model.R2VB_DummyVBO, 1);
	renderer->setVertexBuffer(model.UVBuffer, 0);
	renderer->setIndexBuffer(model.IB);
	renderer->setTexture("diffuseMap", model.diffuseMap);
	renderer->apply();

	renderer->changeShaderConstant4x4f("mvp", MVP);
	renderer->changeShaderConstant4x4f("trans", model.transformMatrix);
	if (!ambient)
		renderer->changeShaderConstant4f("LightPos", lit.position);
	renderer->changeShaderConstant4f("LightColor", lit.color);

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, model.numVertices, 0, model.numTriangles);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::drawPlayer(Light& lit, bool ambient)
{
	psDrawModel(lit, ambient);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to re-compute per-face normals of the model.
// They will be rendered to an R2VB vertex buffer of the shadow volume mesh.
//-----------------------------------------------------------------------------------------
void App::psCalculateShadowVolumeNormals()
{
	renderer->changeRenderTarget(model.R2VB_ShadowVolumeNormal_VBO);

		renderer->setVertexFormat(model.QuadVF);
		renderer->setVertexBuffer(model.QuadVB, 0);
		renderer->setShader(model.calculateNormalSH);
		renderer->setTexture("vertexPosition", model.R2VB_VBO);
		renderer->setTexture("vertexIndexMap", model.shadowVolumeVertexIndexMap);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		renderer->changeShaderConstant1f("iVertexTextureWidth", 1.0f/(float)(model.vertexTextureWidth*2.0f));

		dev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

	renderer->resetRenderTarget();
}
//-----------------------------------------------------------------------------------------
// Render shadow volume mesh to mark shadowed area in the stencil buffer.
// We use two sided stencil buffer for marking the shadowing area.
//-----------------------------------------------------------------------------------------
void App::drawShadowVolume(Light &lit)
{
	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(model.R2VB_ShadowVolumeNormal_VBO));

	dev->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	dev->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0x10);

	dev->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFF);
	dev->SetRenderState(D3DRS_STENCILREF,  0x10);
	dev->SetRenderState(D3DRS_STENCILMASK, 0xFF);
	// Use double-sided stencil
	dev->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
	// Setup stencil functions for CW faces
	dev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	dev->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
	dev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	dev->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_INCR);
	// Setup stencil functions for CCW faces
	dev->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
	dev->SetRenderState(D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP);
	dev->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
	dev->SetRenderState(D3DRS_CCW_STENCILPASS,  D3DSTENCILOP_DECR);
	//enable clip plane
	dev->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	float4x4 mat = !transpose(MVP);
	dev->SetClipPlane(0, mat * float4(0, 1, 0, 0));
	// Use a depth bias because clip planes create new geometry
	float bias = -0.00004f;
	float slopeBias = -0.4f;
	dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD *) &bias);
	dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *) &slopeBias);

	renderer->setVertexFormat(model.shadowVolumeVF);
	renderer->setVertexBuffer(model.R2VB_ShadowVolumeNormal_DummyVBO, 1);
	renderer->setVertexBuffer(model.R2VB_ShadowVolumeNormal_DummyVBO, 2);
	renderer->setIndexBuffer(model.shadowVolumeIB);
	renderer->setShader(model.shadowVolumeSH);
	renderer->setDepthFunc(LESS);
	renderer->setCullFace(CULL_NONE);
	renderer->setMask(NONE);
	renderer->setShaderConstant4f("LightPos", lit.position);
	renderer->setShaderConstant4x4f("mvp", MVP);
	renderer->apply();
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, model.numVerticesShadowVolume, 0, model.numTrianglesShadowVolume);

	dev->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	
	//disable clip plane
	dev->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	dev->SetRenderState(D3DRS_DEPTHBIAS, 0);
	dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
}
//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------
void App::ambientPass()
{
	drawFloor(ambient, true);
	drawPlayer(ambient, true);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::lightingPass(Light lit)
{
	drawShadowVolume(lit);

	// Setup stencil function for the final pass, 
	// we only render the part of the scene not covered by the shadow volume
	dev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
	dev->SetRenderState(D3DRS_STENCILREF,  0x10);
	dev->SetRenderState(D3DRS_STENCILMASK, ~0);
	dev->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
	dev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	dev->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);

	drawFloor(lit, false);
	drawPlayer(lit, false);

	//turn off stencil
	dev->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::drawFrame()
{
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 0.1f, 500.0f, true);
	float4x4 modelview = rotateXY4(-wx, -wy);
	float4x4 mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	MVP = projection * modelview;

	if (oneLight == lightMenu->getCheckedRadioButton(1))
		numLights = 1;
	else if (twoLight == lightMenu->getCheckedRadioButton(1))
		numLights = 2;
	else if (threeLight == lightMenu->getCheckedRadioButton(1))
		numLights = 3;
	else if (fourLight == lightMenu->getCheckedRadioButton(1))
		numLights = 4;

	renderer->clearBuffers(DEPTH_BUFFER);

	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));
	// Tell the driver that stream 1 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DMAP));

	// play animation of player
	playPlayer();
	// re-compute per-face normal and render it to vertex buffer
	psCalculateShadowVolumeNormals();

	ambientPass();
	// lighting for every light source
	for (uint i=0; i<numLights; i++)
	{
		lightingPass(light[i]);
	}

	// Stream 1 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);
	dev->SetTexture(D3DVERTEXTEXTURESAMPLER0, NULL);
	// Disable render to vertex buffer extension
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));

	drawSkybox(mvpSB);
}
