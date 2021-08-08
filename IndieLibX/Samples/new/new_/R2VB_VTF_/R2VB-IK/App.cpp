//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses R2VB technique to solve real-time IK problem in the GPU. 
// We use textures to store pivot and transform matrix of every link and  
// compute the solutions of every link in the pixel shader. 
//-----------------------------------------------------------------------------



#include "App.h"
#include "../../Framework/Direct3D/atir2vb.h"

#define D2R						(PI/180.0f)
#define R2D						(1.0f/D2R)
#define ITERATION_TIMES			40

#define __xUSE_HALF__
#define __xUPDATE_EVERY_INNER_LOOP__

#ifdef __USE_HALF__
	#define VERTEX_FORMAT					D3DFMT_A16B16G16R16F
	#define MATRIX_FORMAT					D3DFMT_A16B16G16R16F
	#define FRAMETIME_MATRIX_FORMAT			D3DFMT_A16B16G16R16F
	#define R2VB_VBO_FORMAT					D3DFMT_A32B32G32R32F	
	#define R2VB_TEXCOORD_FORMAT			FORMAT_FLOAT
	
	#define VERTEX_VALUE_FORMAT				half
	#define MATRIX_VALUE_FORMAT				half
	#define FRAMETIME_MATRIX_VALUE			half
	#define R2VB_VERTEX_FORMAT				FORMAT_FLOAT	
	#define R2VB_VERTEX_VALUE_FORMAT		float		
	#define R2VB_TEXCOORD_VALUE_FORMAT		float
#else
	#define VERTEX_FORMAT					D3DFMT_A32B32G32R32F
	#define MATRIX_FORMAT					D3DFMT_A32B32G32R32F
	#define FRAMETIME_MATRIX_FORMAT			D3DFMT_A32B32G32R32F
	#define R2VB_VBO_FORMAT					D3DFMT_A32B32G32R32F	
	#define R2VB_TEXCOORD_FORMAT			FORMAT_FLOAT

	#define VERTEX_VALUE_FORMAT				float
	#define MATRIX_VALUE_FORMAT				float
	#define FRAMETIME_MATRIX_VALUE			float
	#define R2VB_VERTEX_FORMAT				FORMAT_FLOAT	
	#define R2VB_VERTEX_VALUE_FORMAT		float		
	#define R2VB_TEXCOORD_VALUE_FORMAT		float
#endif

BaseApp *app = new App();

//-----------------------------------------------------------------------------------------
// Create vertex buffer of each link
//-----------------------------------------------------------------------------------------
Model *createLink(const float w, const float len)
{
	static const vec3 verts[] = {
		vec3(-w, -len, -w), vec3(-w, -len, w), vec3(-w, 0, -w), vec3(-w, 0, w),
		vec3( w, -len, -w), vec3( w, -len, w), vec3( w, 0, -w), vec3( w, 0, w),
	};

	static const vec3 norms[] = { vec3(-1, 0, 0), vec3(0, 0, 1), vec3(1, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0), vec3(0, -1, 0) };
	static const uint indices[] = {
		0, 1, 3, 2,
		1, 5, 7, 3,
		5, 4, 6, 7,
		4, 0, 2, 6,
		3, 7, 6, 2,
		5, 1, 0, 4,
	};

	int vertexSize = 3;
	vertexSize += 3;
	vertexSize += 2;

	float *vertices = new float[36 * vertexSize];
	float *dest = vertices;
	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 6; j++){
			static int ind[] = { 0, 1, 2, 0, 2, 3 };
			int k = ind[j];

			*((vec3 *) dest) = verts[indices[4 * i + k]];
			dest += 3;
			*((vec3 *) dest) = norms[i];
			dest += 3;
			*((vec2 *) dest) = vec2((k >= 1) && (k <= 2), (k > 1));
			dest += 2;
		}
	}

	Model *model = new Model();
	model->setVBData(vertices, 36);
	model->addFormat(TYPE_VERTEX, 3);
	model->addFormat(TYPE_NORMAL,   3);
	model->addFormat(TYPE_TEXCOORD, 2);
	model->addBatch(0, 0, 0, 36);

	return model;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::initGUI()
{
	D3D9App::initGUI();
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::onSize(const int w, const int h)
{
	D3D9App::onSize(w, h);
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
	camPos = float3(30, -15, 0);
	wx = 0.0f;
	wy = 90.0f*PI/180.0f;
	speed = 50;
}
//-----------------------------------------------------------------------------------------
// Create R2VB vertex buffer for all vertices of links.
//-----------------------------------------------------------------------------------------
bool App::createLinksVBO()
{
	LPDIRECT3DTEXTURE9 tex;
	D3DLOCKED_RECT rect;

	numVertices = (NUM_LINKS-1)*36;
	numTriangles = numVertices/3;

	// Create vertex declarations
	StreamVertexAttribute modelVertexAttribs[] = {
		1, TYPE_VERTEX,   R2VB_VERTEX_FORMAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
	if ((VF = renderer->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs))) == VF_NONE) return false;

	// 1 pixels can store 1 vertex positions, 1 pixel can store 4 transform matrix indices, so 1 vertex == 2 pixels
	if (dev->CreateTexture(numVertices, 1, 1, 0, VERTEX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}		

	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		VERTEX_VALUE_FORMAT* lpC = (VERTEX_VALUE_FORMAT *)rect.pBits;

		for (int i=0; i<NUM_LINKS-1; i++)
		{
			unsigned char *lpV = link[i].pModel->getVertexArray();
			
			for (int v=0; v<36; v++)
			{
				vec3 *lpVertex;
				lpVertex = (vec3 *)&lpV[v*(link[i].pModel->getVertexSize())];
				*lpC++ = VERTEX_VALUE_FORMAT(lpVertex->x);
				*lpC++ = VERTEX_VALUE_FORMAT(lpVertex->y);
				*lpC++ = VERTEX_VALUE_FORMAT(lpVertex->z);
				*lpC++ = VERTEX_VALUE_FORMAT(1.0f);
			}
		}
		tex->UnlockRect(0);
	}
	if ((linkVertexTexture = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	// 1 pixels can store 1 vertex positions, 1 pixel can store 4 transform matrix indices, so 1 vertex == 2 pixels
	if (dev->CreateTexture(numVertices, 1, 1, 0, VERTEX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}		

	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		VERTEX_VALUE_FORMAT* lpC = (VERTEX_VALUE_FORMAT *)rect.pBits;

		for (int i=0; i<NUM_LINKS-1; i++)
		{
			unsigned char *lpV = link[i].pModel->getVertexArray();
			
			for (int v=0; v<36; v++)
			{
				vec3 *lpVertex;
				lpVertex = (vec3 *)&lpV[v*(link[i].pModel->getVertexSize())];
				*lpC++ = VERTEX_VALUE_FORMAT(i);
				*lpC++ = VERTEX_VALUE_FORMAT(0);
				*lpC++ = VERTEX_VALUE_FORMAT(0);
				*lpC++ = VERTEX_VALUE_FORMAT(0);
			}
		}
		tex->UnlockRect(0);
	}
	if ((matrixIndexTexture = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	// Create the texture coord of links	
	if ((texCoord_VBO = renderer->addVertexBuffer(numVertices * (sizeof(R2VB_TEXCOORD_VALUE_FORMAT)*2))) == VB_NONE) return false;
	R2VB_TEXCOORD_VALUE_FORMAT *uvdest = (R2VB_TEXCOORD_VALUE_FORMAT *) renderer->lockVertexBuffer(texCoord_VBO);
		for (int i=0; i<NUM_LINKS-1; i++)
		{
			unsigned char *lpV = link[i].pModel->getVertexArray();
			for (int v=0; v<36; v++)
			{
				vec2 *lpUVList;
				lpUVList = (vec2 *)&lpV[v*(link[i].pModel->getVertexSize())+(2*sizeof(vec3))];
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList->x);
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList->y);
			}
		}
	renderer->unlockVertexBuffer(texCoord_VBO);	

	// craete R2VB vertex buffer,Since the render target will be used as vertex buffer we need to
	// signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	LPDIRECT3DTEXTURE9 d3drt0;
	if (dev->CreateTexture(numVertices, 1, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, R2VB_VBO_FORMAT, D3DPOOL_DEFAULT, &d3drt0, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((R2VB_VBO = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt0, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	/*
		We need a dummy vertex buffer when we specify stride and offset when calling SetStreamSource()
		for R2VB. Usually we can just pass any buffer we have laying around, but the debug runtime does
		additional validation, so we have to ensure the dummy vertex buffer is large enough to be valid
		for the draw call in this case, even though it's not going to be used. For the release runtime
		we stick with the smaller size to save memory.
	*/
	int size;
	if (debugRuntime)
		size = (numVertices) * 1 * 4 * sizeof(R2VB_VERTEX_VALUE_FORMAT);
	else
		size = 2;
	if ((R2VB_DummyVBO = renderer->addVertexBuffer(size)) == VB_NONE) return false;

	
	//craete index buffer of model
	if ((IB = renderer->addIndexBuffer(numVertices, sizeof(WORD))) == IB_NONE) return false;
	WORD *iDest = (WORD *) renderer->lockIndexBuffer(IB);
		for (int i=0; i<numVertices; i++)
			*iDest++ = i;
	renderer->unlockIndexBuffer(IB);

	for (int i=0; i<NUM_LINKS; i++)
	{		
		delete link[i].pModel;
	}

	return true;
}
//-----------------------------------------------------------------------------------------
// Load target ball model data.
//-----------------------------------------------------------------------------------------
bool Target::load(char *name, Renderer* renderer)
{
	ATIModel::load(name);
	StreamVertexAttribute modelVertexAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD,  FORMAT_FLOAT, 2,
	};

	if ((VertexFormat = renderer->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs))) == VF_NONE) return false;
	// Create the model vertex buffer for vertes shader animation	
	if ((VertexBuffer = renderer->addVertexBuffer(numVertices*6*(sizeof(float)))) == VB_NONE) return false;
	float *dest = (float *) renderer->lockVertexBuffer(VertexBuffer);
	for (DWORD i=0; i<numVertices; i++) 
	{
		(*dest++) = pVPosition[i].x;
		(*dest++) = pVPosition[i].y;
		(*dest++) = pVPosition[i].z;
		(*dest++) = 1.0f;
		(*dest++) = pVTexcoord[i].x;
		(*dest++) = pVTexcoord[i].y;
	}
	renderer->unlockVertexBuffer(VertexBuffer);	

	if ((IndexBuffer = renderer->addIndexBuffer(numTriangles*3, sizeof(DWORD))) == IB_NONE) return false;
	DWORD *iDest = (DWORD *) renderer->lockIndexBuffer(IndexBuffer);
		for (DWORD i=0; i<numTriangles; i++) 
		{
			(*iDest++) = pTriangle[i].v[0];
			(*iDest++) = pTriangle[i].v[1];
			(*iDest++) = pTriangle[i].v[2];
		}		
	renderer->unlockIndexBuffer(IndexBuffer);

	if ((Shader = renderer->addShader("target.shd")) == SHADER_NONE) return false;

	char FullName[256];	
	sprintf(FullName, "%s%s", TEX_PATH, pMaterial[0].diffuseMap);
	if ((DiffuseMap = renderer->addTexture(FullName, FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	setRange(0.0f, 1000.0f);
	setTime(0.0f);

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::load()
{
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK)
	{
		ERRORMSG("No R2VB support");
		return false;
	}

	// initialize link data
	link[0].pivotMatrix = identity4();
	link[0].transformMatrix = identity4();
	link[0].parent = NULL;
	link[0].pModel = createLink(0.5, LINK_LEN);

	for (int i=1; i<NUM_LINKS; i++)
	{
		link[i].pivotMatrix = identity4();
		link[i].pivotMatrix.elem[3][1] = -LINK_LEN;
		link[i].transformMatrix = identity4();

		link[i].parent = &link[i-1];
		link[i].pModel = createLink(0.5, LINK_LEN);
	}

	VertexAttribute quadAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2,
	                                  TYPE_TEXCOORD, FORMAT_FLOAT, 2};
	// Load shaders
	if ((quadShd = renderer->addShader("quad.shd")) == SHADER_NONE) return false;
	if ((computeTransformMatrixShd = renderer->addShader("computeTransformMatrix.shd")) == SHADER_NONE) return false;	
	if ((computeCCDShd = renderer->addShader("computeCCD.shd")) == SHADER_NONE) return false;	
	if ((transformLinkShd = renderer->addShader("transformLink.shd")) == SHADER_NONE) return false;
	if ((drawLinkShd = renderer->addShader("drawLink.shd")) == SHADER_NONE) return false;
	if ((generatePivotMatrixShd = renderer->addShader("generatePivotMatrix.shd")) == SHADER_NONE) return false;

	if ((QuadVF = renderer->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == VF_NONE) return false;

	LPDIRECT3DTEXTURE9 tex;
	D3DLOCKED_RECT rect;

	if (dev->CreateTexture(4, 1, 1, 0, MATRIX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create texture");
		return false;
	}		
	if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
	{
		MATRIX_VALUE_FORMAT* lpC = (MATRIX_VALUE_FORMAT *)rect.pBits;
		float *lpM = &link[0].pivotMatrix.elem[0][0];
			for (int i=0; i<16; i++)
				*lpC++ = MATRIX_VALUE_FORMAT(lpM[i]);
		tex->UnlockRect(0);
	}
	if ((identityMatrix = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;

	// load target ball model
	target.load(MODEL_PATH "IK_Target/IK_Target.am", renderer);

	currentPivotMatrix = 1;
	outputPivotMatrix = 0;
	currentRoatationDegMap = 1;
	outputRoatationDegMap = 0;

	DiffuseMap = renderer->addTexture(TEX_PATH "ChessBoard.dds", FILTER_TRILINEAR_ANISO);


	LPDIRECT3DTEXTURE9 d3drt;
	for (int i=0; i<NUM_LINKS; i++)
	{
		if (dev->CreateTexture(4, 1, 1, 0, MATRIX_FORMAT, D3DPOOL_MANAGED, &tex, NULL) != D3D_OK)
		{
			ERRORMSG("Couldn't create texture");
			return false;
		}		
		if (tex->LockRect(0, &rect, NULL, 0) == D3D_OK)
		{
			MATRIX_VALUE_FORMAT* lpC = (MATRIX_VALUE_FORMAT *)rect.pBits;
			float *lpM = &link[i].pivotMatrix.elem[0][0];
				for (int i=0; i<16; i++)
					*lpC++ = MATRIX_VALUE_FORMAT(lpM[i]);
			tex->UnlockRect(0);
		}
		if ((link[i].tOPivotMatrix = ((D3D9Renderer *) renderer)->addTexture(tex, FILTER_POINT, 0)) == TEXTURE_NONE) return false;
		
		// Create render target for transform matrixes of links.
		// 1 pixel can save four matrix elements, so one matrix need 4 pixels.
		if (dev->CreateTexture(4, 1, 1, D3DUSAGE_RENDERTARGET, MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
		{
			ERRORMSG("Couldn't create render target");
			return false;
		}
		if ((link[i].tTransformMatrix = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

		// Create render targets for pivot matrixes of links.
		// 1 pixel can save four matrix elements, so one matrix need 4 pixels.
		// We create two textures for pin-pong rendering.
		if (dev->CreateTexture(4, 1, 1, D3DUSAGE_RENDERTARGET, MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
		{
			ERRORMSG("Couldn't create render target");
			return false;
		}
		if ((link[i].tPivotMatrix[0] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
		if (dev->CreateTexture(4, 1, 1, D3DUSAGE_RENDERTARGET, MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
		{
			ERRORMSG("Couldn't create render target");
			return false;
		}
		if ((link[i].tPivotMatrix[1] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

		// Create render target for rotation degree of links.
		// 1 pixel can save four rotation degree.
		// We create two textures for pin-pong rendering.
		if (dev->CreateTexture(1, 1, 1, D3DUSAGE_RENDERTARGET, MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
		{
			ERRORMSG("Couldn't create render target");
			return false;
		}
		if ((link[i].tRotationDegMap[0] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

		if (dev->CreateTexture(1, 1, 1, D3DUSAGE_RENDERTARGET, MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
		{
			ERRORMSG("Couldn't create render target");
			return false;
		}
		if ((link[i].tRotationDegMap[1] = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;

	}

	// create render target for transform matrix of every links, every 4 pixels can store one transform matrix.
	if (dev->CreateTexture(4*(NUM_LINKS-1), 1, 1, D3DUSAGE_RENDERTARGET, MATRIX_FORMAT, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK)
	{
		ERRORMSG("Couldn't create render target");
		return false;
	}
	if ((combinedTransformMatrix = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;	

	// Create R2VB vertex buffer for all vertices of links.
	if (!createLinksVBO())
	{
		ERRORMSG("Couldn't create links VBO");
		return false;
	}

	if (!onReset()) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::unload()
{
	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool App::onReset()
{
	for (int i=0; i<NUM_LINKS; i++)
	{
		static float4 vertices[] = {
			float4(-1, -1, 0, 0),
			float4( 1, -1, 1, 0),
			float4( 1,  1, 1, 1),
			float4(-1,  1, 0, 1),
		};
		
		// copy original pivot matrix into pivot matrix texture
		renderer->changeRenderTarget(link[i].tPivotMatrix[0]);
		dev->BeginScene();	
			
			renderer->setVertexFormat(QuadVF);
			renderer->setShader(quadShd);
			renderer->setTexture("Diffuse", link[i].tOPivotMatrix);
			renderer->setDepthFunc(DEPTH_NONE);
			renderer->apply();
			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));

		dev->EndScene();

		renderer->changeRenderTarget(link[i].tPivotMatrix[1]);
		dev->BeginScene();	

			renderer->setVertexFormat(QuadVF);
			renderer->setShader(quadShd);
			renderer->setTexture("Diffuse", link[i].tOPivotMatrix);
			renderer->setDepthFunc(DEPTH_NONE);
			renderer->apply();
			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));

		dev->EndScene();
		renderer->resetRenderTarget();		
	}

	return true;
}
//-----------------------------------------------------------------------------------------
// Computer CCD in the GPU. The render target will hold the new pivot matrix.
//------------------------------------------------------------------------------------
void App::computeCCD(int i, vec3& endpos)
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};	

	renderer->changeRenderTarget(link[i].tPivotMatrix[outputPivotMatrix]);

		renderer->setVertexFormat(QuadVF);
		renderer->setShader(computeCCDShd);
		renderer->setTexture("PivotM", link[i].tPivotMatrix[currentPivotMatrix]);
		renderer->setTexture("LinkTM", link[i].tTransformMatrix);	
		renderer->setTexture("EffectLinkTM", link[NUM_LINKS-1].tTransformMatrix);	
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();
		renderer->changeShaderConstant4f("EndPos", vec4(endpos, 1.0f));		

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));
}
//-----------------------------------------------------------------------------------------
// Compute hierarchy transform matrixes for each link.
//-----------------------------------------------------------------------------------------
void App::computeTransformMatrix(int i)
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	renderer->changeRenderTarget(link[i].tTransformMatrix);

		renderer->setVertexFormat(QuadVF);
		renderer->setShader(computeTransformMatrixShd);
		if (link[i].parent)	// if link has parent, use transform matrix of the parent.	
			renderer->setTexture("ParentM", link[i].parent->tTransformMatrix);
		else // if link has no parent, it's the root link. Use indentity matrix as parent transfom matrix.
			renderer->setTexture("ParentM", identityMatrix);		
		renderer->setTexture("PivotM", link[i].tPivotMatrix[outputPivotMatrix]);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();		

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));
}
//-----------------------------------------------------------------------------------------
// Combine each transform matrix of links into one texture.
//-----------------------------------------------------------------------------------------
void App::combineTransformMatrix()
{
	float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	renderer->changeRenderTarget(combinedTransformMatrix);

		vertices[1].x = vertices[0].x + 2.0f/(NUM_LINKS-1);
		vertices[2].x = vertices[0].x + 2.0f/(NUM_LINKS-1);

		for (int i=0; i<NUM_LINKS-1; i++)
		{
			renderer->setVertexFormat(QuadVF);
			renderer->setShader(quadShd);
			renderer->setTexture("Diffuse", link[i].tTransformMatrix);
			renderer->setDepthFunc(DEPTH_NONE);
			renderer->apply();					

			dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));

			vertices[0].x += 2.0f/(NUM_LINKS-1);
			vertices[1].x += 2.0f/(NUM_LINKS-1);
			vertices[2].x += 2.0f/(NUM_LINKS-1);
			vertices[3].x += 2.0f/(NUM_LINKS-1);
		}
}
//-----------------------------------------------------------------------------------------
// Transform all vertices into one R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void App::transformLinks()
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};

	renderer->changeRenderTarget(R2VB_VBO);

		renderer->setVertexFormat(QuadVF);
		renderer->setShader(transformLinkShd);
		renderer->setTexture("VertexTexture", linkVertexTexture);	
		renderer->setTexture("MatrixIndexTexture", matrixIndexTexture);	
		renderer->setTexture("TransformMatrix", combinedTransformMatrix);	
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();
		renderer->changeShaderConstant1f("NumLinks", (float)(NUM_LINKS-1));		
		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));
}
//-----------------------------------------------------------------------------------------
// Draw target ball.
//-----------------------------------------------------------------------------------------
void Target::setup(Renderer *renderer){
	renderer->setShader(Shader);
	renderer->setVertexFormat(VertexFormat);
	renderer->setVertexBuffer(VertexBuffer, 0);
	renderer->setIndexBuffer(IndexBuffer);	
	renderer->setTexture("DiffuseMap", DiffuseMap);
	renderer->apply();

	renderer->changeShaderConstant4x4f("trans", pMesh[0].getTransformMatrix());
}

void App::drawTarget(float4x4& mvp)
{
	target.setup(renderer);
	renderer->changeShaderConstant4x4f("mvp", mvp);
	
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, target.getVertexCount(), 0, target.getTriangleCount());
}
//-----------------------------------------------------------------------------------------
// Draw all links.
//-----------------------------------------------------------------------------------------
void App::drawLinks(float4x4& mvp)
{
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));	
	// Tell the driver that stream 1 is to be fetched from the DMAP texture
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, 0));
	// Bind the render target to the DMAP sampler
	dev->SetTexture(D3DDMAPSAMPLER, ((D3D9Renderer *) renderer)->getD3DTexture(R2VB_VBO));

	renderer->setVertexFormat(VF);	
	renderer->setVertexBuffer(R2VB_DummyVBO, 1); //dummy vertex buffer
	renderer->setVertexBuffer(texCoord_VBO, 0);
	renderer->setIndexBuffer(IB);	
	renderer->setShader(drawLinkShd);
	renderer->setTexture("DiffuseMap", DiffuseMap);
	renderer->apply();
	renderer->changeShaderConstant4x4f("mvp", mvp);

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertices, 0, numTriangles);

	// Stream 1 restored to regular vertex buffer mode
	dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));
	// Unbind the DMAP texture
	dev->SetTexture(D3DDMAPSAMPLER, NULL);
	// Disable render to vertex buffer extension
    dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));
}
//-----------------------------------------------------------------------------------------
// Dump texture to screen.
//-----------------------------------------------------------------------------------------
void App::dumpTexture(TextureID ti)
{
	static float4 vertices[] = {
		float4(-1, -1, 0, 0),
		float4( 1, -1, 1, 0),
		float4( 1,  1, 1, 1),
		float4(-1,  1, 0, 1),
	};
	renderer->resetRenderTarget();
		renderer->setVertexFormat(QuadVF);
		renderer->setShader(quadShd);
		renderer->setTexture("Diffuse", ti);
		renderer->setDepthFunc(DEPTH_NONE);
		renderer->apply();

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float4));
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void App::drawFrame()
{
	target.play(1.0f);
	
	target.getMesh(0).getPosition(endPos);

	// compute global transform matrix for each link
	for (int i=0; i<NUM_LINKS; i++)
	{
		computeTransformMatrix(i);
	}

	// Compute CCD interatively to get the desired degree of each link.
	for (int times=0; times<ITERATION_TIMES; times++)
	{
		int Temp = currentPivotMatrix;
		currentPivotMatrix = outputPivotMatrix;
		outputPivotMatrix = Temp;		

		for (int i=NUM_LINKS-2; i>=0; i--)
		{
			computeCCD(i, endPos);
			#ifdef __UPDATE_EVERY_INNER_LOOP__
				for (int xx=0; xx<NUM_LINKS; xx++)
				{
					computeTransformMatrix(xx);
				}
			#endif
		}

		// compute global transform matrix for each link
		#ifndef __UPDATE_EVERY_INNER_LOOP__
			for (int xx=0; xx<NUM_LINKS; xx++)
			{
				computeTransformMatrix(xx);
			}
		#endif
	}
	// combine all transform matrix textures into one texture
	combineTransformMatrix();
	transformLinks();

	renderer->resetRenderTarget();
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 1, 20000, true);

	float4x4 modelview = rotateXY4(-wx, -wy);
	float4x4 mvpSB = projection * modelview;
	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	renderer->clearBuffers(COLOR_BUFFER | DEPTH_BUFFER, float4(0.5, 0.5, 0.5, 0));
	drawLinks(mvp);
	drawTarget(mvp);
}