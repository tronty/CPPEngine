/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

#define LOG_FNLN STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses an R2VB technique to compute the per-face normal of a skeletal
// animation model on the GPU, then uses the computed per-face normal for shadow
// volume extrusion.
//-----------------------------------------------------------------------------

#define __USE_HALF__ 1

#ifdef __USE_HALF__
	#define VERTEX_FORMAT					FORMAT_RGBA16F
	#define BONE_INDEX_FORMAT				FORMAT_RGBA16F
	#define VERTEX_WEIGHT_FORMAT			FORMAT_RGBA16F
	#define BONE_MATRIX_FORMAT				FORMAT_RGBA16F
	#define FRAMETIME_MATRIX_FORMAT			FORMAT_RGBA16F
	#define R2VB_VBO_FORMAT					FORMAT_RGBA32F
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
	#define VERTEX_FORMAT					FORMAT_RGBA32F
	#define BONE_INDEX_FORMAT				FORMAT_RGBA32F
	#define VERTEX_WEIGHT_FORMAT			FORMAT_RGBA32F
	#define BONE_MATRIX_FORMAT				FORMAT_RGBA32F
	#define FRAMETIME_MATRIX_FORMAT			FORMAT_RGBA32F
	#define R2VB_VBO_FORMAT					FORMAT_RGBA32F
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

#include <Util/ATIModel.h>

typedef struct {
	__DWORD__ numVertices;
	__DWORD__ numTriangles;
	__DWORD__ numBones;
	__DWORD__ numFrames;
	D3DXFROMWINEMATRIX transformMatrix;
	D3DXFROMWINEMATRIX bonesMatrix[300][48];
	D3DXFROMWINEMATRIX tBonesMatrix[48];
	D3DXFROMWINEVECTOR3 *pVertex;
	__WORD__ *pTriangleList;

	__DWORD__ vertexTextureWidth, vertexTextureHeight;

	ShaderID animationSH, skinningSH, modelSH, ambientModelSH;
	VertexFormatID animationVF, skinningVF, modelVF, ambientModelVF;
	TextureID skinningVertex, vertexBoneIndex, vertexWeight, boneAnimation, boneMatrix;

	VertexFormatID VF;
	VertexBufferID R2VB_DummyVBO, R2VB_ShadowVolumeNormal_DummyVBO, UVBuffer;
	TextureID R2VB_VBO;
	IndexBufferID IB;

	TextureID diffuseMap;

	float time, interpolate;

	__DWORD__ numVerticesShadowVolume, numTrianglesShadowVolume;
	VertexBufferID ShadowVolumePosition_VB;
	IndexBufferID shadowVolumeIB;
	TextureID R2VB_ShadowVolumeNormal_VBO;
	TextureID shadowVolumeVertexIndexMap;
	VertexFormatID calculateNormalVF, shadowVolumeVF;
	ShaderID calculateNormalSH, shadowVolumeSH;

	//VertexFormatID QuadVF;
	VertexBufferID QuadVB;
}Model;

typedef struct {
	D3DXFROMWINEVECTOR4 position;
	D3DXFROMWINEVECTOR4 color;
}Light;

class Floor : public ATIModel {
public:
	VertexFormatID modelVF, ambientModelVF;
	VertexBufferID VertexBuffer;
	IndexBufferID IndexBuffer;
	ShaderID modelSH, ambientModelSH;
	TextureID DiffuseMap, BumpMap;

	bool load(char *name);
};

class App {
public:
	unsigned int width, height, speed, stencilBits;
	D3DXFROMWINEVECTOR3 camPos;
	float wx, wy;
	const int debugRuntime=0;
#if 1//def R2VB_GUI
	Menu *lightMenu;
	MenuItemID oneLight, twoLight, threeLight, fourLight;
#endif
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void setup()
{
	width  = 800;
	height = 600;
	speed = 10;
	stencilBits = 8;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void resetCamera()
{
	camPos = D3DXFROMWINEVECTOR3(-10, 20, -20);
	wx = 0.5f;
	wy = -0.5f;
}
#ifdef R2VB_GUI
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void initGUI()
{
	lightMenu  = BaseApp::getMenu()->addMenu("Light");
	oneLight   = lightMenu->addItem("1 Light",  MENU_CHECKABLE);
	twoLight   = lightMenu->addItem("2 Lights", MENU_CHECKABLE);
	threeLight = lightMenu->addItem("3 Lights", MENU_CHECKABLE);
	fourLight  = lightMenu->addItem("4 Lights", MENU_CHECKABLE);
	lightMenu->setRadioButtonGroup(oneLight, fourLight, 1);
	lightMenu->toggleItemCheck(twoLight);

	//D3D9App::initGUI();
}
#endif
//-----------------------------------------------------------------------------------------
// Load skinning vertex data into textures.
// It includes vertex position, bone index and weight value.
//-----------------------------------------------------------------------------------------
bool loadSkinningVertexMap(char* name)
{
	FILE	*fp;
	__DWORD__	NumVertices, NumBones;
	D3DXFROMWINEVECTOR4	*vertexOffset;
	D3DXFROMWINEVECTOR4	*vertexNormal;

	fp = fopen(name, "rb");
	if (!fp)
		return false;

	fread(&NumBones, sizeof(__DWORD__), 1, fp);
	fread(&NumVertices, sizeof(__DWORD__), 1, fp);

	model.pVertex = new D3DXFROMWINEVECTOR3[NumVertices];
	vertexOffset = new D3DXFROMWINEVECTOR4[NumVertices];
	vertexNormal = new D3DXFROMWINEVECTOR4[NumVertices];

	for (uint i=0; i<NumVertices; i++)
	{
		fread(&vertexOffset[i], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
		fseek(fp, sizeof(D3DXFROMWINEVECTOR4)*3, SEEK_CUR);
		fread(&vertexNormal[i], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
		vertexNormal[i].w = 0.0f;
		model.pVertex[i].x = vertexOffset[i].x;
		model.pVertex[i].y = vertexOffset[i].y;
		model.pVertex[i].z = vertexOffset[i].z;
	}

	//TextureID tex;
	// 1 pixel can store 1 vertex positions, 1 pixel can store 1 vertex normal, so one skinning vertex == 2 pixels.
	Image3 tex;
	VERTEX_VALUE_FORMAT* lpC = (VERTEX_VALUE_FORMAT*)tex.create(VERTEX_FORMAT, model.vertexTextureWidth*2, model.vertexTextureHeight, 1, 1);
	{
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
	}
	model.skinningVertex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	// 1 pixel can store 4 bone indices, so one vertex skinning bone == 1 pixel.
	{
		BONE_INDEX_VALUE_FORMAT* lpC = (BONE_INDEX_VALUE_FORMAT*)tex.create(BONE_INDEX_FORMAT, model.vertexTextureWidth, model.vertexTextureHeight, 1, 1);
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
	}
	model.vertexBoneIndex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	// 1 pixel can store 4 weight values, so one skinning vertex weight value == 1 pixel.
	{
		VERTEX_WEIGHT_VALUE_FORMAT* lpC = (VERTEX_WEIGHT_VALUE_FORMAT*)tex.create(VERTEX_WEIGHT_FORMAT, model.vertexTextureWidth, model.vertexTextureHeight, 1, 1);

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
	}
	model.vertexWeight = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	fclose(fp);

	delete [] vertexOffset;
	delete [] vertexNormal;
	
	return true;
}
//-----------------------------------------------------------------------------------------
// Load bone animation matrices into texture.
//-----------------------------------------------------------------------------------------
bool loadBoneAnimationMap(char* name)
{
	FILE	*fp;

	fp = fopen(name, "rb");
	if (!fp)
		return false;

	fread(&model.numBones, sizeof(__DWORD__), 1, fp);
	fread(&model.numFrames, sizeof(__DWORD__), 1, fp);
	for (uint i=0; i<model.numFrames; i++)
		fread(model.bonesMatrix[i], sizeof(D3DXFROMWINEMATRIX), model.numBones, fp);

	fclose(fp);

	Image3 tex;
	BONE_MATRIX_VALUE_FORMAT* lpC = (BONE_MATRIX_VALUE_FORMAT*)tex.create(BONE_MATRIX_FORMAT, 4*model.numBones, model.numFrames, 1, 1);
	{
		for (uint f=0; f<model.numFrames; f++)
			for (__DWORD__ i=0; i<model.numBones; i++)
			{
				float *lpM = &model.bonesMatrix[f][i].m[0][0];
				for (int i=0; i<16; i++)
					*lpC++ = BONE_MATRIX_VALUE_FORMAT(lpM[i]);
			}
	}
	model.boneAnimation = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Create render target for storage of bones' transform matrices.
	// 4 pixels == 1 4x4 matrix, so a (4*model.numBones)x1 floating-point texture can store (model.numBones) bone matrices.
	model.boneMatrix = IRenderer::GetRendererInstance()->addRenderTarget(4*model.numBones, 1, BONE_MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());

	return true;
}
//-----------------------------------------------------------------------------------------
// Load model triangle and vertex data, that will be used for vertex shader animation.
// We also create the R2VB vertex buffer here.
//-----------------------------------------------------------------------------------------
bool createPlayer(char *modelname, char *skinname, char *animation)
{
	// Load shaders
	model.animationSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/animation.hlsl", "main", "main");
	{FormatDesc modelVertexAttribs[] = {
		1, TYPE_VERTEX,   R2VB_VERTEX_FORMAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
	model.animationVF = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), model.animationSH);}	
	model.skinningSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/skinning.hlsl", "main", "main");
	model.modelSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/model.hlsl", "main", "main");
	model.ambientModelSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/model_ambient.hlsl", "main", "main");
	model.calculateNormalSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/normal.hlsl", "main", "main");
	model.shadowVolumeSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/ShadowVolume.hlsl", "main", "main");

	FILE	*fp;
	D3DXFROMWINEVECTOR2	*lpUVList;
	char Path[256];

	sprintf(Path, "%s%s", MODEL_PATH, modelname);
	fp = fopen(Path, "rb");
	if (!fp)
		return false;

	fread(&model.numVertices, sizeof(__DWORD__), 1, fp);
	model.vertexTextureWidth = model.numVertices;
	model.vertexTextureHeight = 1;
	if (model.vertexTextureWidth > (IRenderer::GetRendererInstance()->GetMaxTextureWidth()/2))
	{
		model.vertexTextureHeight = (model.vertexTextureWidth/(IRenderer::GetRendererInstance()->GetMaxTextureWidth()/2));
		if (model.vertexTextureWidth%(IRenderer::GetRendererInstance()->GetMaxTextureWidth()/2))
			model.vertexTextureHeight++;
		if (model.vertexTextureHeight%2)
			model.vertexTextureHeight++;
		model.vertexTextureWidth = (IRenderer::GetRendererInstance()->GetMaxTextureWidth()/2);
	}

	lpUVList = new D3DXFROMWINEVECTOR2[model.numVertices];

	// Create vertex declarations
	FormatDesc modelVertexAttribs[] = {
		1, TYPE_VERTEX,   R2VB_VERTEX_FORMAT, 4,
		1, TYPE_NORMAL,   R2VB_VERTEX_FORMAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
	if ((model.VF = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), model.modelSH)) == VF_NONE) return false;
	if ((model.ambientModelVF = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), model.ambientModelSH)) == VF_NONE) return false;

	for (__DWORD__ i=0; i<model.numVertices; i++) 
	{
		fseek(fp, sizeof(D3DXFROMWINEVECTOR4)*7, SEEK_CUR);
	}
	// Create the texture coordinates of the model
	std::vector<R2VB_TEXCOORD_VALUE_FORMAT> uv(model.numVertices*2);
	R2VB_TEXCOORD_VALUE_FORMAT *uvdest = &uv[0];
		fread(lpUVList, sizeof(D3DXFROMWINEVECTOR2), model.numVertices, fp);
		for (uint j=0; j<model.numVertices; j++)
		{
			*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].x);
			*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].y);
		}
	model.UVBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(uv.size() * sizeof(R2VB_TEXCOORD_VALUE_FORMAT), STATIC, &uv[0]);

	fread(&model.numTriangles, sizeof(__DWORD__), 1, fp);
	model.pTriangleList = new __WORD__[model.numTriangles*3];

	// Create index buffer of the model
	std::vector<__WORD__> vi(model.numTriangles*3);
	__WORD__ *iDest = &vi[0];
		fread(model.pTriangleList, sizeof(__WORD__), model.numTriangles*3, fp);
		stx_memcpy(iDest, model.pTriangleList, sizeof(__WORD__)*model.numTriangles*3);
	model.IB = IRenderer::GetRendererInstance()->addIndexBuffer(vi.size(), sizeof(__WORD__), STATIC, &vi[0]);

	/*
		Create render target for the model animation.
		Since this render target will be used as a vertex buffer we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	model.R2VB_VBO = IRenderer::GetRendererInstance()->addRenderTarget(model.vertexTextureWidth*2, model.vertexTextureHeight, R2VB_VBO_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling setVertexBuffer()
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

	if ((model.R2VB_DummyVBO = IRenderer::GetRendererInstance()->addVertexBuffer(size)) == VB_NONE) return false;

	fclose(fp);

	delete [] lpUVList;
	
	sprintf(Path, "%s%s", MODEL_PATH, skinname);
	loadSkinningVertexMap(Path);
	sprintf(Path, "%s%s", MODEL_PATH, animation);
	loadBoneAnimationMap(Path);

	// Load textures
	model.diffuseMap = IRenderer::GetRendererInstance()->addImageLibTexture("boy03.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());
#if 0
	FormatDesc quadAttribs[] = { 
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	if ((model.QuadVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == VF_NONE) return false;
#endif
	float quadV[4][6] = {
		-1,  1, 0, 1, 0, 0,
		 1,  1, 0, 1, 1, 0,
		 1, -1, 0, 1, 1, 1,
		-1, -1, 0, 1, 0, 1,
	};
	// Create the quad vertex buffer
	std::vector<float> vv(4 * 6);
	float *dest = &vv[0];
		stx_memcpy(dest, quadV, sizeof(float)*6*4);
	model.QuadVB = IRenderer::GetRendererInstance()->addVertexBuffer(4 * 6 * sizeof(float), STATIC, &quadV[0][0]);

	model.transformMatrix = identity4();
	model.time = 1.0f;
	model.interpolate = 0.0f;

	return true;
}

struct EdgeData{
	bool bGenerated;
	__WORD__ v0, v1, v2;
	D3DXFROMWINEVECTOR3 p0, p1;
};
//-----------------------------------------------------------------------------------------
// Generate shadow volume mesh from the model's triangles. We generate a quad for every two
// adjacent triangles. The model must be closed for us to be able to generate a correct
// shadow volume mesh.
//-----------------------------------------------------------------------------------------
bool generateShadowVolume() 
{
	EdgeData*lpEdgeData; 
	__WORD__ *pVertexIndexMap;
	__DWORD__ shadowVolumeTexWidth, shadowVolumeTexHeight;
	__DWORD__ numIndex;
	__DWORD__ numEdge;

	lpEdgeData = new EdgeData[model.numTriangles*3];
	pVertexIndexMap = new __WORD__[model.numTriangles*3*4*3];

	for (__DWORD__ t=0; t<model.numTriangles; t++)
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
	for (__DWORD__ t=0; t<model.numTriangles*3; t++)
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
			for (__DWORD__ j=0; j<model.numTriangles*3; j++)
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
				LOG_PRINT("Couldn't create shadow volume because mesh is not closed");
			}
		}
	}

	// Create vertex declarations
	FormatDesc VertexAttribs[] = {
		1, TYPE_VERTEX, FORMAT_FLOAT, 4,
		1, TYPE_NORMAL, FORMAT_FLOAT, 4,
	};
	if ((model.shadowVolumeVF = IRenderer::GetRendererInstance()->addVertexFormat(VertexAttribs, elementsOf(VertexAttribs))) == VF_NONE) return false;

	model.numVerticesShadowVolume = numEdge*4;

	// Try to find a texture size that's roughly square
	ubyte shift = 0;
	while ((2U << (2 * shift)) < model.numVerticesShadowVolume) shift++;
	shadowVolumeTexWidth = 1 << shift;
	shadowVolumeTexHeight = (model.numVerticesShadowVolume + shadowVolumeTexWidth - 1) >> shift;

	// Create a texture to store triangle vertex indices of every vertex of the shadow volume mesh
	Image3 tex;
	#define VERTEX_FORMAT_RGBA16f FORMAT_RGBA16F
	short *lpC = (short *)tex.create(VERTEX_FORMAT_RGBA16f, shadowVolumeTexWidth*2, shadowVolumeTexHeight, 1, 1);
	{
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
	}
	model.shadowVolumeVertexIndexMap = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	model.numTrianglesShadowVolume = numEdge*2;

	// Create the triangle index buffer of the shadow volume
	std::vector<__WORD__> vi(model.numTrianglesShadowVolume*3);
	__WORD__ *iDest = &vi[0];
	for (uint i=0; i<numEdge; i++)
	{
		*iDest++ = i*4 + 0;
		*iDest++ = i*4 + 1;
		*iDest++ = i*4 + 2;
		*iDest++ = i*4 + 2;
		*iDest++ = i*4 + 1;
		*iDest++ = i*4 + 3;
	}
	model.shadowVolumeIB = IRenderer::GetRendererInstance()->addIndexBuffer(vi.size(), sizeof(__WORD__), STATIC, &vi[0]);

	// Create R2VB vertex buffer. Since the render target will be used as vertex buffer
	// we need to signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	model.R2VB_ShadowVolumeNormal_VBO = IRenderer::GetRendererInstance()->addRenderTarget(shadowVolumeTexWidth*2, shadowVolumeTexHeight, /* FORMAT_RGBA32F */ FORMAT_RGBA16F, IRenderer::GetRendererInstance()->GetnearestClamp());
	/*
		We need a dummy vertex buffer when we specify stride and offset when calling setVertexBuffer()
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
	if ((model.R2VB_ShadowVolumeNormal_DummyVBO = IRenderer::GetRendererInstance()->addVertexBuffer(size)) == VB_NONE) return false;

	delete [] lpEdgeData;
	delete [] pVertexIndexMap;

	return true;
}
//-----------------------------------------------------------------------------------------
// Load all the resources the sample needs
//-----------------------------------------------------------------------------------------
bool load()
{

	#if 0
	// check if the hardware support two sided stencil buffer?
	if ((caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED) == 0)
	{
		LOG_PRINT("Two-sided stencil is not supported");
		return false;
	}
	#endif

	// Load textures
	#if 0
	// Load shaders
	if ((skybox = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/skybox.hlsl")) == SHADER_NONE) return false;
	env = IRenderer::GetRendererInstance()->addCubemap("Mountains", "jpg");

	FormatDesc skyboxAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 3 };
	if ((skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs), -1)) == VF_NONE) return false;
	#else
    	sb.init("/SkyBox/cubemaps/x/MountainsGL.dds");
	#endif


	// ??? if (!floor.load(MODEL_PATH "floor/floor.am")) return false;

	if (!createPlayer("boy03/boy03.mdl", "boy03/boy03.skn", "boy03/boy03.ani")) return false;

	if (!generateShadowVolume()) return false;


	delete [] model.pVertex;

	delete [] model.pTriangleList;


	ambient.color = D3DXFROMWINEVECTOR4(0, 0, 0.3f, 1.0f);

	light[0].position.x = cosf(-60.0f*D3DXFROMWINE_PI/180.0f)*20.0f;
	light[0].position.z = sinf(-60.0f*D3DXFROMWINE_PI/180.0f)*20.0f;
	light[0].position.y = 20.0f;
	light[0].color = D3DXFROMWINEVECTOR4(0.45f, 0.45f, 0.45f, 1);

	light[1].position.x = cosf(90.0f*D3DXFROMWINE_PI/180.0f)*30.0f;
	light[1].position.z = sinf(90.0f*D3DXFROMWINE_PI/180.0f)*30.0f;
	light[1].position.y = 20.0f;
	light[1].color = D3DXFROMWINEVECTOR4(0.45f, 0.45f, 0.45f, 1);

	light[2].position.x = cosf(180.0f*D3DXFROMWINE_PI/180.0f)*30.0f;
	light[2].position.z = sinf(180.0f*D3DXFROMWINE_PI/180.0f)*30.0f;
	light[2].position.y = 40.0f;
	light[2].color = D3DXFROMWINEVECTOR4(0.45f, 0.45f, 0.45f, 1);

	light[3].position.x = cosf(270.0f*D3DXFROMWINE_PI/180.0f)*30.0f;
	light[3].position.z = sinf(270.0f*D3DXFROMWINE_PI/180.0f)*30.0f;
	light[3].position.y = 30.0f;
	light[3].color = D3DXFROMWINEVECTOR4(0.45f, 0.45f, 0.45f, 1);

	numLights = 2;

	//onReset();

	return true;
}
//-----------------------------------------------------------------------------------------
// Render the skybox
//-----------------------------------------------------------------------------------------
void drawSkybox(const D3DXFROMWINEMATRIX &mvp)
{
	#if 0
	static const float verts[] = { -1,-1,-1, -1,-1,1, -1,1,-1, -1,1,1, 1,-1,-1, 1,-1,1, 1,1,-1, 1,1,1 };
	static const ushort inds[] = { 2,3,0, 0,3,1, 3,7,1, 1,7,5, 7,6,5, 5,6,4, 6,2,4, 4,2,0, 6,7,2, 2,7,3, 0,1,4, 4,1,5 };

	IRenderer::GetRendererInstance()->setShader(skybox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setTexture("Skybox", env);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);
	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 8, 12, inds, CONSTANT_INDEX2, verts, 3 * sizeof(float));
	#else
        sb.render();
	#endif
}
//-----------------------------------------------------------------------------------------
// Render the floor
//-----------------------------------------------------------------------------------------
void drawFloor(Light &lit, bool ambient)
{
	if (ambient) //ambient pass
	{
		IRenderer::GetRendererInstance()->setShader(floor.ambientModelSH);
	IRenderer::GetRendererInstance()->setVertexFormat(floor.ambientModelVF);
		IRenderer::GetRendererInstance()->setMask(eCOLOR | eDEPTH);
		IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	}
	else //lighting pass
	{
		IRenderer::GetRendererInstance()->setShader(floor.modelSH);
	IRenderer::GetRendererInstance()->setVertexFormat(floor.modelVF);
		IRenderer::GetRendererInstance()->setBlending(eONE, eONE);
		IRenderer::GetRendererInstance()->setMask(eCOLOR);
		IRenderer::GetRendererInstance()->setDepthFunc(eLEQUAL);
	}
	IRenderer::GetRendererInstance()->setVertexBuffer(0, floor.VertexBuffer);
	IRenderer::GetRendererInstance()->setIndexBuffer(floor.IndexBuffer);
	IRenderer::GetRendererInstance()->setTexture("DiffuseMap", floor.DiffuseMap);
	if (!ambient)
		IRenderer::GetRendererInstance()->setTexture("BumpMap", floor.BumpMap);
	IRenderer::GetRendererInstance()->setCullFace(eBACK);	
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", MVP);
	if (!ambient)
	{
		IRenderer::GetRendererInstance()->setShaderConstant4f("LightPos", lit.position);
		IRenderer::GetRendererInstance()->setShaderConstant4f("CameraPos", D3DXFROMWINEVECTOR4(camPos, 1.0f));
	}
	IRenderer::GetRendererInstance()->setShaderConstant4f("LightColor", lit.color);

	// ??? IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, floor.getVertexCount(), 0, floor.getTriangleCount());
}
//-----------------------------------------------------------------------------------------
// Play the animation of player.
//-----------------------------------------------------------------------------------------
void playPlayer()
{

	model.interpolate += 1000.0f*timeGetTime()*30.0f;
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
void psCalculateAnimation(float t, float interp)
{

	IRenderer::GetRendererInstance()->changeRenderTarget(model.boneMatrix);

		IRenderer::GetRendererInstance()->setShader(model.animationSH);

		IRenderer::GetRendererInstance()->setVertexFormat(model.animationVF);

		IRenderer::GetRendererInstance()->setVertexBuffer(0, model.QuadVB);

		IRenderer::GetRendererInstance()->setTexture("boneAnimation", model.boneAnimation);

		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);


		IRenderer::GetRendererInstance()->setShaderConstant4f("time_interp", D3DXFROMWINEVECTOR4(t-1, t, interp, 0.0f));

		IRenderer::GetRendererInstance()->setShaderConstant1f("XBias", 0.1f/(model.numBones*4.0f));

		IRenderer::GetRendererInstance()->setShaderConstant1f("iBoneAnimationHeight", 1.0f/model.numFrames);

		
		// ??? IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_FAN, 0, 2);

}
//-----------------------------------------------------------------------------------------
// Use pixel shader to transform the model's vertices.
//-----------------------------------------------------------------------------------------
void psSkinning()
{

	// ??? IRenderer::GetRendererInstance()->changeRenderTarget(model.R2VB_VBO);


		IRenderer::GetRendererInstance()->setShader(model.skinningSH);

		IRenderer::GetRendererInstance()->setVertexFormat(model.skinningVF);

		IRenderer::GetRendererInstance()->setVertexBuffer(0, model.QuadVB);

		IRenderer::GetRendererInstance()->setTexture("skinningVertex", model.skinningVertex);

		IRenderer::GetRendererInstance()->setTexture("vertexBoneIndex", model.vertexBoneIndex);

		IRenderer::GetRendererInstance()->setTexture("vertexWeight", model.vertexWeight);

		IRenderer::GetRendererInstance()->setTexture("boneMatrix", model.boneMatrix);

		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);


		IRenderer::GetRendererInstance()->setShaderConstant1f("xBias", 0.1f/(model.vertexTextureWidth));

		IRenderer::GetRendererInstance()->setShaderConstant4f("bias", D3DXFROMWINEVECTOR4(0.1f/(model.numBones*4.0f), 1.1f/(model.numBones*4.0f), 2.1f/(model.numBones*4.0f), 3.1f/(model.numBones*4.0f)));


		// ??? IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_FAN, 0, 2);


	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

}
//-----------------------------------------------------------------------------------------
// Render model using R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void psDrawModel(Light &lit, bool ambient)
{
	if (ambient) //ambient pass
	{
		IRenderer::GetRendererInstance()->setShader(model.ambientModelSH);
		IRenderer::GetRendererInstance()->setVertexFormat(model.ambientModelVF);
		IRenderer::GetRendererInstance()->setMask(eCOLOR | eDEPTH);
		IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	}
	else  //lighting pass
	{
		IRenderer::GetRendererInstance()->setShader(model.modelSH);
		IRenderer::GetRendererInstance()->setVertexFormat(model.VF);
		IRenderer::GetRendererInstance()->setBlending(eONE, eONE);
		IRenderer::GetRendererInstance()->setMask(eCOLOR);
		IRenderer::GetRendererInstance()->setDepthFunc(eLEQUAL);
	}

	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", model.R2VB_VBO);
	//IRenderer::GetRendererInstance()->setVertexBuffer(1, model.R2VB_DummyVBO);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, model.UVBuffer);
	IRenderer::GetRendererInstance()->setIndexBuffer(model.IB);
	IRenderer::GetRendererInstance()->setTexture("diffuseMap", model.diffuseMap);
	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", MVP);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("trans", model.transformMatrix);
	if (!ambient)
		IRenderer::GetRendererInstance()->setShaderConstant4f("LightPos", lit.position);
	IRenderer::GetRendererInstance()->setShaderConstant4f("LightColor", lit.color);

	// ??? IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, model.numVertices, 0, model.numTriangles);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void drawPlayer(Light& lit, bool ambient)
{
	psDrawModel(lit, ambient);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to re-compute per-face normals of the model.
// They will be rendered to an R2VB vertex buffer of the shadow volume mesh.
//-----------------------------------------------------------------------------------------
void psCalculateShadowVolumeNormals()
{
	// ??? IRenderer::GetRendererInstance()->changeRenderTarget(model.R2VB_ShadowVolumeNormal_VBO);

		IRenderer::GetRendererInstance()->setShader(model.calculateNormalSH);
		IRenderer::GetRendererInstance()->setVertexFormat(model.calculateNormalVF);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, model.QuadVB);
		IRenderer::GetRendererInstance()->setTexture("vertexPosition", model.R2VB_VBO);
		IRenderer::GetRendererInstance()->setTexture("vertexIndexMap", model.shadowVolumeVertexIndexMap);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		

		IRenderer::GetRendererInstance()->setShaderConstant1f("iVertexTextureWidth", 1.0f/(float)(model.vertexTextureWidth*2.0f));

		// ??? IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_FAN, 0, 2);

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
}
//-----------------------------------------------------------------------------------------
// Render shadow volume mesh to mark shadowed area in the stencil buffer.
// We use two sided stencil buffer for marking the shadowing area.
//-----------------------------------------------------------------------------------------
void drawShadowVolume(Light &lit)
{
	IRenderer::GetRendererInstance()->setShader(model.shadowVolumeSH);
	IRenderer::GetRendererInstance()->setVertexFormat(model.shadowVolumeVF);
	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", model.R2VB_ShadowVolumeNormal_VBO);

	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	//IRenderer::GetRendererInstance()->Clear(0, 0, D3DCLEAR_STENCIL, 0, 1.0f, 0x10);

	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFF);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILREF,  0x10);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILMASK, 0xFF);
	// Use double-sided stencil
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
	// Setup stencil functions for CW faces
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_INCR);
	// Setup stencil functions for CCW faces
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CCW_STENCILPASS,  D3DSTENCILOP_DECR);
	//enable clip plane
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	// ??? D3DXFROMWINEMATRIX mat = !transpose(MVP);
	// ??? IRenderer::GetRendererInstance()->SetClipPlane(0, mat * D3DXFROMWINEVECTOR4(0, 1, 0, 0));
	// Use a depth bias because clip planes create new geometry
	float bias = -0.00004f;
	float slopeBias = -0.4f;
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DEPTHBIAS, *(__DWORD__ *) &bias);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(__DWORD__ *) &slopeBias);

	IRenderer::GetRendererInstance()->setVertexBuffer(1, model.R2VB_ShadowVolumeNormal_DummyVBO);
	//IRenderer::GetRendererInstance()->setVertexBuffer(2, model.R2VB_ShadowVolumeNormal_DummyVBO);
	IRenderer::GetRendererInstance()->setIndexBuffer(model.shadowVolumeIB);
	IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	IRenderer::GetRendererInstance()->setCullFace(eCULL_NONE);
	IRenderer::GetRendererInstance()->setMask(eNONE);
	IRenderer::GetRendererInstance()->setShaderConstant4f("LightPos", lit.position);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", MVP);
	
	// ??? IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, model.numVerticesShadowVolume, 0, model.numTrianglesShadowVolume);

	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	
	//disable clip plane
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DEPTHBIAS, 0);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
}
//-----------------------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------------------
void ambientPass()
{
	drawFloor(ambient, true);
	drawPlayer(ambient, true);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void lightingPass(Light lit)
{
	drawShadowVolume(lit);

	// Setup stencil function for the final pass, 
	// we only render the part of the scene not covered by the shadow volume
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILREF,  0x10);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILMASK, ~0);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);

	drawFloor(lit, false);
	drawPlayer(lit, false);

	//turn off stencil
	//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void drawFrame()
{

	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 0.1f, 500.0f, true);
	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMATRIX mvpSB = pegToFarPlane(projection * modelview);
	modelview.translate(-camPos);
	MVP = projection * modelview;


	if(lightMenu)
	{
	if (oneLight == lightMenu->getCheckedRadioButton(1))
		numLights = 1;
	else if (twoLight == lightMenu->getCheckedRadioButton(1))
		numLights = 2;
	else if (threeLight == lightMenu->getCheckedRadioButton(1))
		numLights = 3;
	else if (fourLight == lightMenu->getCheckedRadioButton(1))
		numLights = 4;
	}
	else numLights = 1;


	IRenderer::GetRendererInstance()->Clear(false, true);

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


	drawSkybox(mvpSB);

}

private:
	Model model;
	Floor floor;

	D3DXFROMWINEMATRIX MVP;
	VertexFormatID skyboxVF;
	ShaderID skybox;
	TextureID env;
	stx_SkyBox sb;
	Light ambient;
	__DWORD__ numLights;
	Light light[8];
};
App app;
int init(const char* aTitle)
{
	app.setup();
	app.resetCamera();
	app.load();
#ifdef R2VB_GUI
	app.initGUI();
#endif
	return 0;
}

void render()
{
	IRenderer::GetRendererInstance()->BeginScene();
	app.drawFrame();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-ShadowVolume");
	IInput*    i=STX_Service::GetInputInstance();
	//STX_Service::GetAudioInstance()->Init();
	init("");
	STX_PRINT("Function init finished!\n");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

//-----------------------------------------------------------------------------------------
// Load a floor model
//-----------------------------------------------------------------------------------------
bool Floor::load(char *name)
{
	// load shaders for this model
	if ((modelSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/floor.hlsl")) == SHADER_NONE) return false;
	if ((ambientModelSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-ShadowVolume/floor_ambient.hlsl")) == SHADER_NONE) return false;
	ATIModel::load(name);
	FormatDesc modelVertexAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	if ((modelVF = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), modelSH)) == VF_NONE) return false;
	if ((ambientModelVF = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), ambientModelSH)) == VF_NONE) return false;
	// Create the vertex buffer for this model
	std::vector<float> vv(numVertices*5);
	float *dest = &vv[0];
	for (__DWORD__ i=0; i<numVertices; i++)
	{
		(*dest++) = pVPosition[i].x;
		(*dest++) = pVPosition[i].y;
		(*dest++) = pVPosition[i].z;
		(*dest++) = pVTexcoord[i].x;
		(*dest++) = pVTexcoord[i].y;
	}
	VertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(numVertices * 5 * sizeof(float), STATIC, &vv[0]);
	// Create the index buffer for this model
	std::vector<__DWORD__> vi(numTriangles*3);
	__DWORD__ *iDest = &vi[0];
		for (__DWORD__ i=0; i<numTriangles; i++)
		{
			(*iDest++) = pTriangle[i].v[0];
			(*iDest++) = pTriangle[i].v[1];
			(*iDest++) = pTriangle[i].v[2];
		}
	IndexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(numTriangles*3, sizeof(__DWORD__), STATIC, &vi[0]);
	// load diffuse map for this model
	char FullName[256];
	sprintf(FullName, "%s%s", TEX_PATH, pMaterial[0].diffuseMap);
	DiffuseMap = IRenderer::GetRendererInstance()->addImageLibTexture(FullName, false, IRenderer::GetRendererInstance()->GettrilinearAniso());
	if ((BumpMap = IRenderer::GetRendererInstance()->addImageLibTexture(TEX_PATH "layingrockBump.png", false, FILTER_TRILINEAR_ANISO, NORMALMAP)) == TEXTURE_NONE) return false;

	return true;
}

