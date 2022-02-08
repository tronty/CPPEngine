#if !defined(__APPLE__)
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#if 0
#define LOG_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#endif
//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses R2VB technique to compute skinning animation in the pixel 
// shader. Using the R2VB we can compute 2048-4096 (depending on hardware)
// different animations at one time and render 2048-4096 models with one
// DrawPrimitive() call. 
//-----------------------------------------------------------------------------

#define SCALE							0.02f

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

#define MODELNAME						"npc/npc_01"
#define MODELNAME_2						"ruby/ruby"

#define D2R				(3.141590f/180.0f)
#define MAX_NUM_MODELS							65536

#define MAX_NUM_BONES							120		
#define MAX_NUM_ANIMATION_FRAMES				256

enum NPC_State{
	RUN = 0,
	DYING,
	DANCE,
	RESPAWN,
	KICK,
};

#define RUN_T0									1.0f
#define RUN_T1									21.0f

#define DIE_T0									22.0f
#define DIE_T1									30.0f
#define DIE_T2									37.0f

#define DANCE_T0								40.0f
#define DANCE_T1								113.0f

#define KICK_T0									114.0f
#define KICK_T1									171.0f

#define RESPAWN_T0								173.0f
#define RESPAWN_T1								198.0f


typedef struct {
	float			sT, eT;
	float			value[4];
}AniKey;

typedef struct {
	float			ID;
	float			parentID;
	__DWORD__			numPosKeys;
	__DWORD__			numRotKeys;
	__DWORD__			numSclKeys;
}Bone;

typedef struct {
	__DWORD__			numVertices;
	__DWORD__			numTriangles;
	__DWORD__			numBones;	
	__DWORD__			numFrames;
	D3DXFROMWINEMATRIX		bonesMatrix[MAX_NUM_ANIMATION_FRAMES][MAX_NUM_BONES];

	VertexFormatID	vertexFormat;
	TextureID		R2VB_VBO;	
	VertexBufferID	UVBuffer;//R2VB_DummyVBO, R2VB_OnceDummyVBO;
	TextureID		skinningVertex, vertexBoneIndex, vertexWeight, boneAnimation, boneMatrix, diffuse;

	VertexFormatID	vsVertexFormat;
	VertexBufferID	vsVertexBuffer;

	IndexBufferID	indexBuffer;	

	__DWORD__			vertexTextureWidth, vertexTextureHeight;		
}Model;

typedef struct {
	__DWORD__			numVertices;
	__DWORD__			numTriangles;

	TextureID		R2VB_VBO;	
	int				currentFrameTimeTexture;
	TextureID		FrameTime[3];
	TextureID		boneMatrix;

	VertexFormatID	vertexFormat;
	VertexBufferID	UVBuffer;

	IndexBufferID	indexBuffer;		

	__DWORD__			vertexTextureWidth, vertexTextureHeight;
}Batch;

typedef struct {
	D3DXFROMWINEVECTOR3			Scale;
	D3DXFROMWINEMATRIX		Trans;
	float			Speed;
	float			Deg;
	bool			bBounce;
	bool			bInSafeZone;

	float			time;
	float			interpolate;

	bool			bDie, bBounceByGround;
	float			upSpeed;
	D3DXFROMWINEVECTOR3			flyDir;

	NPC_State		currentState;	
}AI;

class App {
public:
#if 0
	VertexFormatID QuadVF, batch_QuadVF;
	VertexBufferID QuadVB, batch_QuadVB;

	Model model[2];
	AI modelAI[MAX_NUM_MODELS];
	Batch batch[2];
	DWORD maxBatchOnce;

	float4x4 MVP;
	float time;
	float interpolate;
	int	numModels;	
	float4 Quat;
	int currentModel;
	ShaderID animationSH, skinningSH, modelSH, batchAnimationSH, batchSkinningSH, batchSkinningNormalSH, batchModelSH, vsModelSH;	
#endif
    float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;
//-----------------------------------------------------------------------------------------
// Convert quaterion to float4X4 matrix
//-----------------------------------------------------------------------------------------
void Quaternion2float4x4(D3DXFROMWINEMATRIX& m, D3DXFROMWINEVECTOR4& q)
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0f;

	xs = q.x*s;        ys = q.y*s;        zs = q.z*s;
	wx = q.w*xs;       wy = q.w*ys;       wz = q.w*zs;
	xx = q.x*xs;       xy = q.x*ys;       xz = q.x*zs;
	yy = q.y*ys;       yz = q.y*zs;       zz = q.z*zs;

	m.m[0][0] = 1.0f - (yy+zz);
	m.m[0][1] = xy + wz;
	m.m[0][2] = xz - wy;

	m.m[1][0] = xy - wz;
	m.m[1][1] = 1.0f - (xx+zz);
	m.m[1][2] = yz + wx;
	
	m.m[2][0] = xz + wy;
	m.m[2][1] = yz - wx;
	m.m[2][2] = 1.0f - (xx + yy);

	m.m[3][0] = m.m[3][1] = m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void setup()
{
	width  = 800;
	height = 600;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void resetCamera()
{
	camPos = D3DXFROMWINEVECTOR3(0, 1500*SCALE, -1500*SCALE);
	wx = 0.7f;
	wy = 0.0f;
	speed = 1000.0f*SCALE;
}

//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void initGUI()
{
#ifndef R2VB_GUI
	mainMenu=0;
	menu=0;
	animationMenu=0;
	AIMenu=0;
#else
	menu = BaseApp::getMenu()->addMenu("Method");
	vsAnimation = menu->addItem("Vertex shader skinning",  MENU_CHECKABLE);
	psAnimation = menu->addItem("Pixel shader skinning", MENU_CHECKABLE);
	menu->setRadioButtonGroup(vsAnimation, psAnimation, 1);
	menu->toggleItemCheck(psAnimation);

	animationMenu = BaseApp::getMenu()->addMenu("Animation");
	sameAnimation = animationMenu->addItem("Same animation everyone",  MENU_CHECKABLE);
	differentAnimation = animationMenu->addItem("Different animation everyone",  MENU_CHECKABLE);
	animationMenu->setRadioButtonGroup(sameAnimation, differentAnimation, 1);
	animationMenu->toggleItemCheck(differentAnimation);

	AIMenu = BaseApp::getMenu()->addMenu("AI");
	noAI = AIMenu->addItem("No AI",  MENU_CHECKABLE);
	respawn = AIMenu->addItem("Respawn",  MENU_CHECKABLE);
	bounce = AIMenu->addItem("Bounce", MENU_CHECKABLE);
	AIMenu->setRadioButtonGroup(noAI, bounce, 1);
	AIMenu->toggleItemCheck(bounce);
	lastAI = bounce;

	BaseApp::getMenu()->addWidget(numModelSlider = new Slider(760, 10, 26, 580, false, 1, MAX_NUM_MODELS/4, 2500));

	BaseApp::initGUI();
#endif
}

//-----------------------------------------------------------------------------------------
// Load skinning vertex data into texture. It includes vertex position, bone index and 
// weight value.
//-----------------------------------------------------------------------------------------
bool loadSkinningVertexMap(Model *m, char* name)
{
	FILE	*fp;
	__DWORD__	NumVertices, NumBones;
	D3DXFROMWINEVECTOR4	*vertexOffset;
	D3DXFROMWINEVECTOR4	*vertexNormal;

	std::string f=std::string(name);
	std::string file=stx_convertpath(f);
    
    LOG_PRINT("file=%s\n", file.c_str());
	fp = fopen(file.c_str(), "rb");

    
	fread(&NumBones, sizeof(__DWORD__), 1, fp);
    
	fread(&NumVertices, sizeof(__DWORD__), 1, fp);
    

	vertexOffset = new D3DXFROMWINEVECTOR4[NumVertices];
    
	vertexNormal = new D3DXFROMWINEVECTOR4[NumVertices];
    

	for (uint i=0; i<NumVertices; i++)
	{
		fread(&vertexOffset[i], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
		fseek(fp, sizeof(D3DXFROMWINEVECTOR4)*3, SEEK_CUR);
		fread(&vertexNormal[i], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
		vertexNormal[i].w = 0.0f;
	}
    

		// 1 pixel can store 1 vertex position and 1 pixel can store 1 vertex normal, so one skinning vertex == 2 pixels.
		Image3 tex;		
    
    {
		VERTEX_VALUE_FORMAT* lpC  = (VERTEX_VALUE_FORMAT*)tex.create(VERTEX_FORMAT, m->vertexTextureWidth*2, m->vertexTextureHeight, 1, 1);
    
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
			}}
    
		m->skinningVertex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	// 1 pixel can store 4 bone indices, so one skinning vertex bone index data == 1 pixel.
	{
        BONE_INDEX_VALUE_FORMAT* lpC  = (BONE_INDEX_VALUE_FORMAT*)tex.create(BONE_INDEX_FORMAT, m->vertexTextureWidth, m->vertexTextureHeight, 1, 1);
    
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
	}
    
	m->vertexBoneIndex = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	// 1 pixel can store 4 weight values, so one skinning vertex weight value == 1 pixel.
	VERTEX_WEIGHT_VALUE_FORMAT* lpC = (VERTEX_WEIGHT_VALUE_FORMAT *)tex.create(VERTEX_WEIGHT_FORMAT, m->vertexTextureWidth, m->vertexTextureHeight, 1, 1);
    	
	{
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
    	
	m->vertexWeight = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	fclose(fp);
    	
	delete [] vertexNormal;
    	
	delete [] vertexOffset;
    
	return true;
}
//-----------------------------------------------------------------------------------------
// Load bone animation matrixes into texture.
//-----------------------------------------------------------------------------------------
bool loadBoneAnimationMap(Model *m, Batch *b, char* name)
{
	FILE	*fp;

	std::string f=std::string(name);
	std::string file=stx_convertpath(f);
    
    LOG_PRINT("file=%s\n", file.c_str());
	fp = fopen(file.c_str(), "rb");
    
	fread(&m->numBones, sizeof(unsigned int), 1, fp);
    	
	fread(&m->numFrames, sizeof(unsigned int), 1, fp);
    	
	for (uint i=0; i<m->numFrames; i++)
		fread(m->bonesMatrix[i], sizeof(D3DXFROMWINEMATRIX), m->numBones, fp);
    
	fclose(fp);
    
	Image3 tex;

	BONE_MATRIX_VALUE_FORMAT* lpC = (BONE_MATRIX_VALUE_FORMAT *)tex.create(BONE_MATRIX_FORMAT, 4*m->numBones, m->numFrames, 1, 1);
    	
	{
		for (uint f=0; f<m->numFrames; f++)
			for (unsigned int i=0; i<m->numBones; i++)
			{
				float *lpM = &m->bonesMatrix[f][i].m[0][0];
				for (int i=0; i<16; i++)
					(*lpC++) = BONE_MATRIX_VALUE_FORMAT(lpM[i]);
			}
	}
	
	m->boneAnimation = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    	
	// Create render target for storing bone's transform matrices.
	// 4 pixels == 1 matrix4x4, so (4*model[0].numBones)x1 floating-point texture can store (model[0].numBones) bone matrices.
	m->boneMatrix = IRenderer::GetRendererInstance()->addRenderTarget(4*m->numBones, 1, BONE_MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	// Create render target for storing bones' transform matrices of all models.
	// 4 pixels == 1 matrix4x4, so (4*model[0].numBones)x(maxBatchOnce) floating-point texture can store (model[0].numBones) bone matrices for (maxBatchOnce) models.
	b->boneMatrix = IRenderer::GetRendererInstance()->addRenderTarget(4*m->numBones, maxBatchOnce, BONE_MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	return true;
}
//-----------------------------------------------------------------------------------------
// Load model triangle and vertex data, that will be used on vertex shader animation.
// We also create R2VB vertex buffer here.
//-----------------------------------------------------------------------------------------
bool loadModel(Model *m, Batch *b, char* name)
{
	LOG_FNLN;
	FILE	*fp;

	std::string f=std::string(name);
	std::string file=stx_convertpath(f);
    
    LOG_PRINT("file=%s\n", file.c_str());
	fp = fopen(file.c_str(), "rb");
	LOG_FNLN;
	if (!fp)
		return false;
	LOG_FNLN;
    
	fread(&m->numVertices, sizeof(unsigned int), 1, fp);
	LOG_FNLN;
    	
	if (m->numVertices%2)
		m->vertexTextureWidth =  m->numVertices+1;
	else
		m->vertexTextureWidth =  m->numVertices;		
	LOG_FNLN;
	m->vertexTextureHeight = 1;
	b->vertexTextureWidth = m->vertexTextureWidth;
	b->vertexTextureHeight = m->vertexTextureHeight * maxBatchOnce;
    
	// Create vertex declarations
	FormatDesc modelVertexAttribs[] = {
		1, TYPE_VERTEX,   R2VB_VERTEX_FORMAT, 4,
		1, TYPE_NORMAL,   R2VB_VERTEX_FORMAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
	LOG_FNLN;
    	
	if ((m->vertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), modelSH)) == VF_NONE) return false;
	LOG_FNLN;
    
	FormatDesc vsModelVertexAttribs[] = {
		1, TYPE_VERTEX,   FORMAT_FLOAT, 4,		
		1, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		1, TYPE_NORMAL,	  FORMAT_FLOAT, 4,
		1, TYPE_TANGENT,  FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
    
	if ((m->vsVertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(vsModelVertexAttribs, elementsOf(vsModelVertexAttribs), vsModelSH)) == VF_NONE) return false;
	LOG_FNLN;
    	
	// Create the model vertex buffer for vertex shader animation
	std::vector<D3DXFROMWINEVECTOR4> vv(m->numVertices * 4);
	D3DXFROMWINEVECTOR4 *dest = &vv[0];
	LOG_FNLN;
    	
	for (unsigned int i=0; i<m->numVertices; i++)
	{
		fread(&vv[4*i], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
		fseek(fp, sizeof(D3DXFROMWINEVECTOR4)*3, SEEK_CUR);
		fread(&vv[4*i+1], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
		fread(&vv[4*i+2], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
		fread(&vv[4*i+3], sizeof(D3DXFROMWINEVECTOR4), 1, fp);
	}
	LOG_FNLN;
    	
	m->vsVertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(vv.size()*sizeof(D3DXFROMWINEVECTOR4), STATIC, &vv[0]);
	LOG_FNLN;
    
	// Create the texture coord of model
	std::vector<float> uv(m->numVertices * 6);
	fread(&uv[0], sizeof(D3DXFROMWINEVECTOR2), m->numVertices, fp);
	for (uint j=0; j<m->numVertices; j++)
	{
		uv[6*j+4] = R2VB_TEXCOORD_VALUE_FORMAT(uv[6*j]);
		uv[6*j+5] = R2VB_TEXCOORD_VALUE_FORMAT(uv[6*j+1]);
	}
	m->UVBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(uv.size()*sizeof(float), STATIC, &uv[0]);
	LOG_FNLN;
    
	// Create index buffer of model
	fread(&m->numTriangles, sizeof(unsigned int), 1, fp);
	LOG_FNLN;
    
	// Create the triangle index buffer
	std::vector<unsigned short int> vi(m->numTriangles*3);
	fread(&vi[0], sizeof(unsigned short int), m->numTriangles*3, fp);
	m->indexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(m->numTriangles*3, 2, STATIC, &vi[0]);
	LOG_FNLN;
    
	/*
		Create render target for the model animation.
		Since these render targets will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	/*
../../../Samples/new/new_/R2VB_VTF/R2VB-Animation/App.cpp:loadModel:528
corrupted double-linked list
Keskeytetty (luotiin core-tiedosto)	
	*/
	m->R2VB_VBO = IRenderer::GetRendererInstance()->addRenderTarget(m->vertexTextureWidth*2, m->vertexTextureHeight, R2VB_VBO_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	/*
		We need a dummy vertex buffer when we specify stride and offset when calling setVertexBuffer()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
    #if 0
	int size;
	if (debugRuntime)
		size = (m->vertexTextureWidth*2) * m->vertexTextureHeight * 4 * sizeof(R2VB_VERTEX_VALUE_FORMAT);
	else
		size = 2;
	m->R2VB_DummyVBO = IRenderer::GetRendererInstance()->addVertexBuffer(size, STATIC, );
    #endif
	fclose(fp);
    
	// Setup batch data -----------------------------------------------------------------------------------------------------------------------------------------------------------------
	LOG_FNLN;
	b->numVertices = m->vertexTextureWidth*m->vertexTextureHeight*maxBatchOnce;
	b->numTriangles = m->numTriangles*maxBatchOnce;
	LOG_FNLN;
    	
	if ((b->vertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), modelSH)) == VF_NONE) return false;

    {
	// Create the vertex texture coordinate buffer of models for batch rendering
	#if 0
	if ((b->UVBuffer = renderer->addVertexBuffer(b->vertexTextureWidth * b->vertexTextureHeight * (sizeof(R2VB_TEXCOORD_VALUE_FORMAT)*2))) == VB_NONE) return false;
	uvdest = (R2VB_TEXCOORD_VALUE_FORMAT *) renderer->lockVertexBuffer(b->UVBuffer);
	R2VB_TEXCOORD_VALUE_FORMAT *lpCUV = uvdest;
		for (uint i=0; i<maxBatchOnce; i++)
		{			
			float OffsetX = (stx_rand()%4)*0.25f;
			float OffsetY = (stx_rand()%4)*0.25f;
			for (uint j=0; j<m->numVertices; j++)
			{
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].x+OffsetX);
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList[j].y+OffsetY);
			}
			lpCUV += (m->vertexTextureWidth*m->vertexTextureHeight*2);
			uvdest = lpCUV;
		}
	renderer->unlockVertexBuffer(b->UVBuffer);	
	#else
	std::vector<float> uv3(b->vertexTextureWidth * b->vertexTextureHeight * (sizeof(R2VB_TEXCOORD_VALUE_FORMAT)*6));
		float* uvdest = &uv3[0];
	R2VB_TEXCOORD_VALUE_FORMAT *lpCUV = uvdest;
		for (uint i=0; i<maxBatchOnce; i++)
		{			
			float OffsetX = (stx_rand()%4)*0.25f;
			float OffsetY = (stx_rand()%4)*0.25f;
			for (uint j=0; j<m->numVertices; j++)
			{
				uvdest+=4;
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(uv[6*j+4]+OffsetX);
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(uv[6*j+5]+OffsetY);
			}
			lpCUV += (m->vertexTextureWidth*m->vertexTextureHeight*2);
			uvdest = lpCUV;
		}
	b->UVBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(uv3.size()*sizeof(float), STATIC, &uv3[0]);
	#endif
    }
        
	/*
		Create render target for the model animation of batch rendering.
		Since these render targets will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	b->R2VB_VBO = IRenderer::GetRendererInstance()->addRenderTarget(b->vertexTextureWidth*2, b->vertexTextureHeight, R2VB_VBO_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	/*
		We need a dummy vertex buffer when we specify stride and offset when calling setVertexBuffer()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
    #if 0
	if (debugRuntime)
		size = (b->vertexTextureWidth*2) * b->vertexTextureHeight * 4 * sizeof(R2VB_VERTEX_VALUE_FORMAT);
	else
		size = 2;
	m->R2VB_OnceDummyVBO = IRenderer::GetRendererInstance()->addVertexBuffer(size);
    #endif
    {
	// Create index buffer for batch rendering
	#if 0
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
	#else
	std::vector<__DWORD__> vi2(b->numTriangles*3);
	__DWORD__ i=0;
	for (__DWORD__ i=0; i<maxBatchOnce; i++)
	{
		for (__DWORD__ t=0; t<m->numTriangles*3; t++)
		{
			vi2[i++] = (__DWORD__)vi[t] + i*m->vertexTextureWidth*m->vertexTextureHeight;
		}
	}
	b->indexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(vi2.size(), 4, STATIC, &vi2[0]);
	#endif
	}

    	
    Image3 tex;
	tex.create(FRAMETIME_MATRIX_FORMAT, 4, maxBatchOnce, 1, 1);
    	
	// Create a texture for storing constants for the batch rendering. We store frame time,
	// interpolation values and transform matrix here. We create 3 textures here for ping-ponging rendering.
    b->FrameTime[0] = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
        
    b->FrameTime[1] = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
        
    b->FrameTime[2] = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
    
	b->currentFrameTimeTexture = 0;
    
	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool load()
{
	
	numModels = MAX_NUM_MODELS/4;
	// Load shaders
	animationSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Animation/animation.shd", "main", "main");
    	if(animationSH==-1) printf("animationSH=%x\n", animationSH);
	skinningSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Animation/skinning.shd", "main", "main");
    	if(skinningSH==-1) printf("skinningSH=%x\n", skinningSH);
	modelSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Animation/model.shd", "main", "main");
    	if(modelSH==-1) printf("modelSH=%x\n", modelSH);
	//stx_exit(0);
	#if 0
	vsModelSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Animation/vsModel.shd", "main", "main");
	batchAnimationSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Animation/batchAnimation.shd", "main", "main");
	batchSkinningSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Animation/batchSkinning.shd", "main", "main");
	batchModelSH = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Animation/batchModel.shd", "main", "main");
	#endif

	// The maximum number of models that can be rendered at one time depends on the maximum size of the texture.
	maxBatchOnce = IRenderer::GetRendererInstance()->GetMaxTextureSize();

	// Load textures
	model[0].diffuse = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/boy.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());
	model[1].diffuse = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/ruby.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());
	
	FormatDesc quadAttribs[] = { 
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2	};
	QuadanimationVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), animationSH);
	QuadskinningVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), skinningSH);
	QuadbatchAnimationVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), batchAnimationSH);
	QuadbatchSkinningVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), batchSkinningSH);

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
	QuadVB = IRenderer::GetRendererInstance()->addVertexBuffer(sizeof(float)*6*4, STATIC, dest);

	#if 0
	FormatDesc batchQuadAttribs[] = { 
		0, TYPE_VERTEX, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2};
	batch_QuadVF = IRenderer::GetRendererInstance()->addVertexFormat(batchQuadAttribs, elementsOf(batchQuadAttribs), -1);
	#endif

	float batchQuadV[4][6] = {
		-1,  1, 0, 1, 0, 0,
		 1,  1, 0, 1, 1, 0,
		 1, -1, 0, 1, 1, 1,
		-1, -1, 0, 1, 0, 1,
	};
	// Create the quad vertex buffer for batch rendering
	dest = &vv[0];
		stx_memcpy(dest, batchQuadV, sizeof(float)*4*6);
	batch_QuadVB = IRenderer::GetRendererInstance()->addVertexBuffer(sizeof(float)*6*4, STATIC, dest);
#if 0
	// Load model data
	if (!loadModel(&model[0], &batch[0], "/new/R2VB_VTF/R2VB/npc/npc_01.mdl")) return false;
	// Load skinning vertex maps
	if (!loadSkinningVertexMap(&model[0], "/new/R2VB_VTF/R2VB/npc/npc_01.skn")) return false;
	// Load animation maps
	if (!loadBoneAnimationMap(&model[0], &batch[0], "/new/R2VB_VTF/R2VB/npc/npc_01.ani")) return false;

	// Load model data
	if (!loadModel(&model[1], &batch[1], "/new/R2VB_VTF/R2VB/ruby/ruby.mdl")) return false;
	// Load skinning vertex maps
	if (!loadSkinningVertexMap(&model[1], "/new/R2VB_VTF/R2VB/ruby/ruby.skn")) return false;
	// Load animation maps
	if (!loadBoneAnimationMap(&model[1], &batch[1], "/new/R2VB_VTF/R2VB/ruby/ruby.ani")) return false;
#endif
	resetModels();

	time = 1;
	interpolate = 0.0f;
	currentModel = 0;


	return true;
}
//-----------------------------------------------------------------------------------------
// Reset model's AI data.
//-----------------------------------------------------------------------------------------
void resetModels()
{
	for (int x=0; x<MAX_NUM_MODELS; x++)
	{
		modelAI[x].bBounce = false;
		modelAI[x].bInSafeZone = true;
		modelAI[x].Deg = (float)(stx_rand()%360);
		modelAI[x].Speed = (1.5f + stx_rand()%50/10.0f)*SCALE;
		Quat.x = sinf(modelAI[x].Deg*D2R/2) * 0.0f;
		Quat.y = sinf(modelAI[x].Deg*D2R/2) * 1.0f;
		Quat.z = sinf(modelAI[x].Deg*D2R/2) * 0.0f;
		Quat.w = cosf(modelAI[x].Deg*D2R/2);
		modelAI[x].Trans = identity4();
		Quaternion2float4x4(modelAI[x].Trans, Quat);
		modelAI[x].Trans.m[0][3] = (float)(stx_rand()%3000-1500)*SCALE;
		modelAI[x].Trans.m[1][3] = 0.0f*SCALE;
		modelAI[x].Trans.m[2][3] = (float)(stx_rand()%3000-1500)*SCALE;

		modelAI[x].time = (float)(stx_rand()%((int)RUN_T1))+1.0f;
		modelAI[x].interpolate = (stx_rand()%10)/10.0f;

		modelAI[x].bBounceByGround = false;
		modelAI[x].Scale.y = (stx_rand()%60)/100.0f + 0.8f;

		modelAI[x].currentState = RUN;
	}
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to calculate new bone matrices.
//-----------------------------------------------------------------------------------------
void psCalculateAnimation(float t, float interp)
{
	IRenderer::GetRendererInstance()->changeRenderTarget(model[currentModel].boneMatrix);

		IRenderer::GetRendererInstance()->setShader(animationSH);
		IRenderer::GetRendererInstance()->setVertexFormat(QuadanimationVF);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, QuadVB);
		IRenderer::GetRendererInstance()->setTexture("boneAnimation", model[currentModel].boneAnimation);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		IRenderer::GetRendererInstance()->setCullFace(eBACK);
		IRenderer::GetRendererInstance()->setShaderConstant4f("time_interp", D3DXFROMWINEVECTOR4(t-1, t, interp, 0.0f));
		IRenderer::GetRendererInstance()->setShaderConstant1f("XBias", 0.1f/(model[currentModel].numBones*4.0f));
		IRenderer::GetRendererInstance()->setShaderConstant1f("iBoneAnimationHeight", 1.0f/model[currentModel].numFrames);
			
		LOG_FNLN;
		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_FAN, 0, 2);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to skin the vertices of the model[currentModel].
//-----------------------------------------------------------------------------------------
void psSkinning()
{
	IRenderer::GetRendererInstance()->changeRenderTarget(model[currentModel].R2VB_VBO);

		IRenderer::GetRendererInstance()->setShader(skinningSH);
		IRenderer::GetRendererInstance()->setVertexFormat(QuadskinningVF);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, QuadVB);
		IRenderer::GetRendererInstance()->setTexture("skinningVertex", model[currentModel].skinningVertex);
		IRenderer::GetRendererInstance()->setTexture("vertexBoneIndex", model[currentModel].vertexBoneIndex);
		IRenderer::GetRendererInstance()->setTexture("vertexWeight", model[currentModel].vertexWeight);
		IRenderer::GetRendererInstance()->setTexture("boneMatrix", model[currentModel].boneMatrix);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		IRenderer::GetRendererInstance()->setShaderConstant1f("xBias", 0.1f/(model[currentModel].vertexTextureWidth*2.0f));
		IRenderer::GetRendererInstance()->setShaderConstant4f("bias", D3DXFROMWINEVECTOR4(0.1f/(model[currentModel].numBones*4.0f), 1.1f/(model[currentModel].numBones*4.0f), 2.1f/(model[currentModel].numBones*4.0f), 3.1f/(model[currentModel].numBones*4.0f)));
		

		LOG_FNLN;
		IRenderer::GetRendererInstance()->DrawPrimitive(PRIM_TRIANGLE_FAN, 0, 2);
}
//-----------------------------------------------------------------------------------------
// Render model using R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void psDrawModel(int mi)
{
	IRenderer::GetRendererInstance()->setShader(modelSH);
	IRenderer::GetRendererInstance()->setVertexFormat(model[currentModel].vertexFormat);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, model[currentModel].UVBuffer);
	//IRenderer::GetRendererInstance()->setVertexBuffer(1, model[currentModel].R2VB_DummyVBO);
	IRenderer::GetRendererInstance()->setIndexBuffer(model[currentModel].indexBuffer);	
	IRenderer::GetRendererInstance()->setTexture("diffuseMap", model[currentModel].diffuse);	
	IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", MVP);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("trans", modelAI[mi].Trans);
	LOG_FNLN;
		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, model[currentModel].numVertices, 0, model[currentModel].numTriangles);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to calculate new bone matrices of multiple models.
//-----------------------------------------------------------------------------------------
void psBatchCalculateAnimation(int num)
{
	IRenderer::GetRendererInstance()->changeRenderTarget(batch[currentModel].boneMatrix);

		IRenderer::GetRendererInstance()->setShader(batchAnimationSH);
		IRenderer::GetRendererInstance()->setVertexFormat(QuadbatchAnimationVF);
		IRenderer::GetRendererInstance()->setTexture("boneAnimation", model[currentModel].boneAnimation);
		IRenderer::GetRendererInstance()->setTexture("trans", batch[currentModel].FrameTime[batch[currentModel].currentFrameTimeTexture]);	
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		IRenderer::GetRendererInstance()->setCullFace(eBACK);
		IRenderer::GetRendererInstance()->setShaderConstant1f("XBias", 0.1f/(model[currentModel].numBones*4.0f));
		IRenderer::GetRendererInstance()->setShaderConstant1f("iBoneAnimationHeight", 1.0f/model[currentModel].numFrames);
		

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

		LOG_FNLN;
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, batchQuadV, batchQuadV, sizeof(float)*6);
}
//-----------------------------------------------------------------------------------------
// Use pixel shader to transform vertices of multiple models.
//-----------------------------------------------------------------------------------------
void psBatchSkinning(int num)
{
	IRenderer::GetRendererInstance()->changeRenderTarget(batch[currentModel].R2VB_VBO);

		IRenderer::GetRendererInstance()->setShader(batchSkinningSH);
		IRenderer::GetRendererInstance()->setVertexFormat(QuadbatchSkinningVF);
		IRenderer::GetRendererInstance()->setTexture("skinningVertex", model[currentModel].skinningVertex);
		IRenderer::GetRendererInstance()->setTexture("vertexBoneIndex", model[currentModel].vertexBoneIndex);
		IRenderer::GetRendererInstance()->setTexture("vertexWeight", model[currentModel].vertexWeight);
		IRenderer::GetRendererInstance()->setTexture("boneMatrix", batch[currentModel].boneMatrix);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		IRenderer::GetRendererInstance()->setShaderConstant1f("xBias", 0.1f/(model[currentModel].vertexTextureWidth*2.0f));
		IRenderer::GetRendererInstance()->setShaderConstant1f("yBias", 0.1f/(float)maxBatchOnce);
		IRenderer::GetRendererInstance()->setShaderConstant1f("numBatch", (float)maxBatchOnce);
		IRenderer::GetRendererInstance()->setShaderConstant4f("bias", D3DXFROMWINEVECTOR4(0.1f/(model[currentModel].numBones*4.0f), 1.1f/(model[currentModel].numBones*4.0f), 2.1f/(model[currentModel].numBones*4.0f), 3.1f/(model[currentModel].numBones*4.0f)));
		

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

		LOG_FNLN;
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, batchQuadV, batchQuadV, sizeof(float)*6);
}
//-----------------------------------------------------------------------------------------
// Render multiple models using one R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void psBatchDrawModel(int num)
{
	IRenderer::GetRendererInstance()->setShader(batchModelSH);
	IRenderer::GetRendererInstance()->setVertexFormat(batch[currentModel].vertexFormat);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, batch[currentModel].UVBuffer);
	//IRenderer::GetRendererInstance()->setVertexBuffer(1, model[currentModel].R2VB_OnceDummyVBO);
	IRenderer::GetRendererInstance()->setIndexBuffer(batch[currentModel].indexBuffer);	
	IRenderer::GetRendererInstance()->setTexture("diffuseMap", model[currentModel].diffuse);	
	IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", MVP);
		
	// Render multiple models at one time.
	LOG_FNLN;
		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, num*model[currentModel].vertexTextureWidth*model[currentModel].vertexTextureHeight, 0, num*model[currentModel].numTriangles);
}
//-----------------------------------------------------------------------------------------
// Pixel shader animation 
//-----------------------------------------------------------------------------------------
void drawPixelShaderAnimation()
{
	if (1)//(animationMenu && (sameAnimation == animationMenu->getCheckedRadioButton(1)))
	{
		psCalculateAnimation(time, interpolate);
		psSkinning();			
		IRenderer::GetRendererInstance()->changeToMainFramebuffer();

		// Bind the render target to the DMAP sampler
		IRenderer::GetRendererInstance()->setTexture("DMAP", model[currentModel].R2VB_VBO);

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
	}
	else if (0)//(animationMenu && (differentAnimation == animationMenu->getCheckedRadioButton(1)))
	{
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
			#if 0
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
			#elif 0
			TextureID lpTex = batch[currentModel].FrameTime[batch[currentModel].currentFrameTimeTexture];
            int Pitch;
            FRAMETIME_MATRIX_VALUE* lpC = (FRAMETIME_MATRIX_VALUE*)
                IRenderer::GetRendererInstance()->LockTexture(lpTex, 0, Pitch);
			{
				for (int i=0; i<NumData; i++)
				{
					float *lpV = (float*)(&(modelAI[Count+i].Trans.m[0][0]));
					for (int j=0; j<16; j++)
					{
						*lpC++ = FRAMETIME_MATRIX_VALUE(lpV[j]);
					}
				}
				IRenderer::GetRendererInstance()->UnlockTexture(lpTex, 0);
			}
			#else
			Image3 tex;
			FRAMETIME_MATRIX_VALUE* lpC = (FRAMETIME_MATRIX_VALUE*)tex.create(FRAMETIME_MATRIX_FORMAT, NumData*16, 1, 1, 1);			
			for (int i=0; i<NumData; i++)
			{
				float *lpV = (float*)(&(modelAI[Count+i].Trans.m[0][0]));
				for (int j=0; j<16; j++)
				{
						*lpC++ = FRAMETIME_MATRIX_VALUE(lpV[j]);
				}
			}
			IRenderer::GetRendererInstance()->deleteTexture(batch[currentModel].FrameTime[batch[currentModel].currentFrameTimeTexture]);
			batch[currentModel].FrameTime[batch[currentModel].currentFrameTimeTexture]=
			IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
			#endif
			psBatchCalculateAnimation(Num);
			psBatchSkinning(Num);
			IRenderer::GetRendererInstance()->changeToMainFramebuffer();

			// Bind the render target to the DMAP sampler
			IRenderer::GetRendererInstance()->setTexture("DMAP", batch[currentModel].R2VB_VBO);
			
			psBatchDrawModel(Num);

			// Unbind the DMAP texture
			

			Count += Num;
			batch[currentModel].currentFrameTimeTexture ++;
			if (batch[currentModel].currentFrameTimeTexture >= 3)
				batch[currentModel].currentFrameTimeTexture = 0;
		}
	}	
}
//-----------------------------------------------------------------------------------------
// Use vertex shader to calculate new bone matrices and send constants to shader. 
//-----------------------------------------------------------------------------------------
void vsCalculateAnimation(float t, float interp, float scale=1.0f)
{
	static D3DXFROMWINEMATRIX	M[48];

	IRenderer::GetRendererInstance()->setShader(vsModelSH);
	IRenderer::GetRendererInstance()->setVertexFormat(model[currentModel].vsVertexFormat);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, model[currentModel].UVBuffer);
	IRenderer::GetRendererInstance()->setVertexBuffer(1, model[currentModel].vsVertexBuffer);
	IRenderer::GetRendererInstance()->setIndexBuffer(model[currentModel].indexBuffer);
	IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	IRenderer::GetRendererInstance()->setCullFace(eBACK);	

	for (__DWORD__ i=0; i<model[currentModel].numBones; i++)
	{
		// compute bone matrices of the current frame
		for (int x= 0; x<4; x++)
			for (int y= 0; y<4; y++)
				M[i].m[x][y] = lerp(model[currentModel].bonesMatrix[(int)(t)][i].m[x][y], model[currentModel].bonesMatrix[(int)(t+1)][i].m[x][y], interp);

		// do some scale
		M[i].m[0][0] *= scale; M[i].m[0][1] *= scale; M[i].m[0][2] *= scale;
		M[i].m[1][0] *= scale; M[i].m[1][1] *= scale; M[i].m[1][2] *= scale;
		M[i].m[2][0] *= scale; M[i].m[2][1] *= scale; M[i].m[2][2] *= scale;
	}
    IRenderer::GetRendererInstance()->setShaderConstantArray4x4f("boneMatrix", M, model[currentModel].numBones);
}
//-----------------------------------------------------------------------------------------
// Render model using regular vertex buffer.
//-----------------------------------------------------------------------------------------
void vsDrawModel(int mi)
{
	IRenderer::GetRendererInstance()->setShader(vsModelSH);
	IRenderer::GetRendererInstance()->setVertexFormat(model[currentModel].vsVertexFormat);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, model[currentModel].UVBuffer);
	IRenderer::GetRendererInstance()->setVertexBuffer(1, model[currentModel].vsVertexBuffer);
	IRenderer::GetRendererInstance()->setIndexBuffer(model[currentModel].indexBuffer);	
	IRenderer::GetRendererInstance()->setDepthFunc(eLESS);
	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setTexture("diffuseMap", model[currentModel].diffuse);	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", MVP);	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("trans", modelAI[mi].Trans);
	

	LOG_FNLN;
		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, model[currentModel].numVertices, 0, model[currentModel].numTriangles);
}
//-----------------------------------------------------------------------------------------
// Vertex shader animation.
//-----------------------------------------------------------------------------------------
void drawVertexShaderAnimation()
{
	if (animationMenu && (sameAnimation == animationMenu->getCheckedRadioButton(1)))
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
	else if (animationMenu && (differentAnimation == animationMenu->getCheckedRadioButton(1)))
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
void AILoop()
{
	static float Grav = -0.7f*SCALE;

	if (AIMenu && (lastAI != AIMenu->getCheckedRadioButton(1)))
		resetModels();

	if (animationMenu && (differentAnimation == animationMenu->getCheckedRadioButton(1))) // every character has different animation
	{
		for (int x=0; x<numModels; x++)
		{
			D3DXFROMWINEVECTOR3		Dist = D3DXFROMWINEVECTOR3(modelAI[x].Trans.m[0][3], modelAI[x].Trans.m[1][3], modelAI[x].Trans.m[2][3]) - camPos;
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
					modelAI[x].upSpeed = (8.0f + ((stx_rand()%800)/100.0f))*SCALE;
					modelAI[x].flyDir = Dist/Len;
					modelAI[x].bDie = false;
					modelAI[x].bBounceByGround = false;
				}
			}
			else
			{
				if (!(stx_rand()%600)) // Sometimes they want dancing
				{
					if (modelAI[x].currentState == RUN)
					{
						modelAI[x].currentState = DANCE;
						modelAI[x].time = DANCE_T0;
						modelAI[x].interpolate = 0.0f;
					}
				}
				else if (!(stx_rand()%600)) // Sometimes they want kicking
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
	if (AIMenu && (bounce == AIMenu->getCheckedRadioButton(1)))
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
						D3DXFROMWINEVECTOR3    v1, v2;
						float     d;
						v1 = D3DXFROMWINEVECTOR3(modelAI[x].Trans.m[0][3], 0.0f, modelAI[x].Trans.m[2][3]);
						v2 = D3DXFROMWINEVECTOR3(modelAI[x].Trans.m[0][2], 0.0f, modelAI[x].Trans.m[2][2]);
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
						if (!(stx_rand()%20))
						{
							modelAI[x].Deg += 3.0f;
							Quat.y = sinf(modelAI[x].Deg*D2R/2.0f);
							Quat.w = cosf(modelAI[x].Deg*D2R/2.0f);
							Quaternion2float4x4(modelAI[x].Trans, Quat);
						}
					}	
					// if the character is running and not dead, move the character
					modelAI[x].Trans.m[0][3] -= modelAI[x].Trans.m[0][2]*modelAI[x].Speed;
					modelAI[x].Trans.m[1][3] -= modelAI[x].Trans.m[1][2]*modelAI[x].Speed;
					modelAI[x].Trans.m[2][3] -= modelAI[x].Trans.m[2][2]*modelAI[x].Speed;
					break;

				// if the caharcter is dying
				case DYING: 
					if (!modelAI[x].bDie)
					{
						// charcater fly away and do some physical computing
						modelAI[x].Trans.m[0][3] += modelAI[x].flyDir.x*10.0f*SCALE;					
						modelAI[x].Trans.m[2][3] += modelAI[x].flyDir.z*10.0f*SCALE;					
						modelAI[x].Trans.m[1][3] += modelAI[x].upSpeed;	
						modelAI[x].upSpeed += Grav;
						if (modelAI[x].upSpeed < 0.0f)
						{
							if (modelAI[x].Trans.m[1][3] <= 0.0f)
							{
								modelAI[x].Trans.m[1][3] = 0.0f;
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
				if (modelAI[x].Trans.m[0][3] > 2500.0f*SCALE || modelAI[x].Trans.m[0][3] < -2500.0f*SCALE ||
					modelAI[x].Trans.m[1][3] > 2500.0f*SCALE || modelAI[x].Trans.m[1][3] < -2500.0f*SCALE ||
					modelAI[x].Trans.m[2][3] > 2500.0f*SCALE || modelAI[x].Trans.m[2][3] < -2500.0f*SCALE)
				{
					// charcater out of safe zone, then need to turn him back into the boundary
					modelAI[x].bBounce = true;
					modelAI[x].bInSafeZone = false;
				}
			}
			else 
			{
				if (modelAI[x].Trans.m[0][3] < 2000.0f*SCALE && modelAI[x].Trans.m[0][3] > -2000.0f*SCALE &&
					modelAI[x].Trans.m[1][3] < 2000.0f*SCALE && modelAI[x].Trans.m[1][3] > -2000.0f*SCALE &&
					modelAI[x].Trans.m[2][3] < 2000.0f*SCALE && modelAI[x].Trans.m[2][3] > -2000.0f*SCALE)
				{
					// charcater out of the boundary, go into safe zone
					modelAI[x].bInSafeZone = true;
				}
			}
		}
	}
	else if (AIMenu && (respawn == AIMenu->getCheckedRadioButton(1))) // AI for respawn
	{
		for (int x=0; x<numModels; x++)
		{
			// turn a degree randomly 
			if (!(stx_rand()%20))
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
					modelAI[x].Trans.m[0][3] -= modelAI[x].Trans.m[0][2]*modelAI[x].Speed;
					modelAI[x].Trans.m[1][3] -= modelAI[x].Trans.m[1][2]*modelAI[x].Speed;
					modelAI[x].Trans.m[2][3] -= modelAI[x].Trans.m[2][2]*modelAI[x].Speed;
					break;

				// if the caharcter is dying
				case DYING:
					if (!modelAI[x].bDie)
					{
						// charcater fly away and do some physical computing
						modelAI[x].Trans.m[0][3] += modelAI[x].flyDir.x*10.0f*SCALE;					
						modelAI[x].Trans.m[2][3] += modelAI[x].flyDir.z*10.0f*SCALE;					
						modelAI[x].Trans.m[1][3] += modelAI[x].upSpeed;	
						modelAI[x].upSpeed += Grav;
						if (modelAI[x].upSpeed < 0.0f)
						{
							if (modelAI[x].Trans.m[1][3] <= 0.0f)
							{
								modelAI[x].Trans.m[1][3] = 0.0f;
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
			if (modelAI[x].Trans.m[0][3] > 2500.0f*SCALE || modelAI[x].Trans.m[0][3] < -2500.0f*SCALE ||
				modelAI[x].Trans.m[1][3] > 2500.0f*SCALE || modelAI[x].Trans.m[1][3] < -2500.0f*SCALE ||
				modelAI[x].Trans.m[2][3] > 2500.0f*SCALE || modelAI[x].Trans.m[2][3] < -2500.0f*SCALE)
			{
				// respawn the character inside the boundary
				modelAI[x].Trans.m[0][3] = (float)(stx_rand()%3000-1500)*SCALE;
				modelAI[x].Trans.m[1][3] = 0.0f;
				modelAI[x].Trans.m[2][3] = (float)(stx_rand()%3000-1500)*SCALE;
			}
		}
	}	

	// play the animation
	if (animationMenu && (sameAnimation == animationMenu->getCheckedRadioButton(1))) // every character has same animation
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
	

	//if (animationMenu && (differentAnimation == animationMenu->getCheckedRadioButton(1) && psAnimation == menu->getCheckedRadioButton(1)))
	if(1)
	{
		// update the data of each character
		for (int x=0; x<numModels; x++)
		{
			modelAI[x].Trans.m[3][0] = modelAI[x].time-1;
			modelAI[x].Trans.m[3][1] = modelAI[x].time;
			modelAI[x].Trans.m[3][2] = modelAI[x].interpolate;
			modelAI[x].Trans.m[3][3] = modelAI[x].Scale.y;
		}
	}
	else
	{
		// reset the data of each character
		for (int x=0; x<numModels; x++)
		{
			modelAI[x].Trans.m[3][0] = 0.0f;
			modelAI[x].Trans.m[3][1] = 0.0f;
			modelAI[x].Trans.m[3][2] = 0.0f;
			modelAI[x].Trans.m[3][3] = 1.0f;
		}
	}
}
//-----------------------------------------------------------------------------------------
// Render the frame
//-----------------------------------------------------------------------------------------
void drawFrame()
{

	D3DXFROMWINEMATRIX rootM;
	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, SCALE, 20000*SCALE, true);

	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	MVP = projection * modelview;

	if(numModelSlider)
		numModels = (int)numModelSlider->getValue()*4;

	AILoop();

	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.5, 0.5, 0.5, 0));
#if 0
	if (menu && (vsAnimation == menu->getCheckedRadioButton(1)))
	{

		drawVertexShaderAnimation();
	}
	else
#endif
	{

		drawPixelShaderAnimation();
	}


	if(AIMenu)
		lastAI = AIMenu->getCheckedRadioButton(1);
#if 0
	char  str[256];
	stx_snprintf(str, 256, "%d models", numModels);

	IRenderer::GetRendererInstance()->setCullFace(eBACK);
	IRenderer::GetRendererInstance()->setBlending(eCONSTANT_COLOR, eZERO);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
	
	IRenderer::GetRendererInstance()->changeBlendConstantColor(D3DXFROMWINEVECTOR4(1, 0.6f, 0.3f, 0));
	//BaseApp::getMainFont().drawText( str, 120, 7, 32, 40, 0.17f, -0.12f, HA_LEFT, VA_TOP);
#endif

}
private:
	Menu *mainMenu, *menu, *animationMenu, *AIMenu;
	MenuItemID vsAnimation, psAnimation, sameAnimation, differentAnimation, noAI, respawn, bounce, lastAI;
	#if 1//def R2VB_GUI
	Slider *numModelSlider;
	#endif

	//VertexFormatID QuadVF, batch_QuadVF;
	VertexBufferID QuadVB, batch_QuadVB;

	Model model[2];
	AI modelAI[MAX_NUM_MODELS];
	Batch batch[2];
	__DWORD__ maxBatchOnce;

	D3DXFROMWINEMATRIX MVP;
	float time;
	float interpolate;
	int	numModels;	
	D3DXFROMWINEVECTOR4 Quat;
	int currentModel;
	ShaderID animationSH, skinningSH, modelSH, batchAnimationSH, batchSkinningSH, batchSkinningNormalSH, batchModelSH, vsModelSH;
	VertexFormatID QuadanimationVF, QuadskinningVF, QuadbatchAnimationVF, QuadbatchSkinningVF;
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
	app.drawFrame();
	IRenderer::GetRendererInstance()->Present();
	//stx_exit(0);
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-Animation");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	STX_PRINT("Function init finished!\n");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
#endif

