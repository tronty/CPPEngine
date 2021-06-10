/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//
// This sample uses R2VB technique to solve real-time IK problem in the GPU. 
// We use textures to store pivot and transform matrix of every link and  
// compute the solutions of every link in the pixel shader. 
//-----------------------------------------------------------------------------

#define D2R						(D3DXFROMWINE_PI/180.0f)
#define R2D						(1.0f/D2R)
#define ITERATION_TIMES			40

#define __xUSE_HALF__
#define __xUPDATE_EVERY_INNER_LOOP__

#ifdef __xUSE_HALF__
	#define VERTEX_FORMAT					FORMAT_RGBA16F
	#define MATRIX_FORMAT					FORMAT_RGBA16F
	#define FRAMETIME_MATRIX_FORMAT			FORMAT_RGBA16F
	#define R2VB_VBO_FORMAT					FORMAT_RGBA32F	
	#define R2VB_TEXCOORD_FORMAT			FORMAT_FLOAT
	
	#define VERTEX_VALUE_FORMAT				half
	#define MATRIX_VALUE_FORMAT				half
	#define FRAMETIME_MATRIX_VALUE			half
	#define R2VB_VERTEX_FORMAT				FORMAT_FLOAT	
	#define R2VB_VERTEX_VALUE_FORMAT		float		
	#define R2VB_TEXCOORD_VALUE_FORMAT		float
#else
	#define VERTEX_FORMAT					FORMAT_RGBA32F
	#define MATRIX_FORMAT					FORMAT_RGBA32F
	#define FRAMETIME_MATRIX_FORMAT			FORMAT_RGBA32F
	#define R2VB_VBO_FORMAT					FORMAT_RGBA32F	
	#define R2VB_TEXCOORD_FORMAT			FORMAT_FLOAT

	#define VERTEX_VALUE_FORMAT				float
	#define MATRIX_VALUE_FORMAT				float
	#define FRAMETIME_MATRIX_VALUE			float
	#define R2VB_VERTEX_FORMAT				FORMAT_FLOAT	
	#define R2VB_VERTEX_VALUE_FORMAT		float		
	#define R2VB_TEXCOORD_VALUE_FORMAT		float
#endif


#include <Util/ATIModel.h>
#include <Util/Model.h>

struct Link{
	D3DXFROMWINEMATRIX pivotMatrix;
	D3DXFROMWINEMATRIX transformMatrix;

	TextureID tOPivotMatrix;
	TextureID tRotationDegMap[2], tPivotMatrix[2], tTransformMatrix;

	Link *parent;
	Model *pModel;
};

#define NUM_LINKS				4
#define LINK_LEN				9.0f

class Target : public ATIModel {
public:
	bool load(char *name);
	void setup();
protected:
	VertexFormatID VertexFormat;
	VertexBufferID VertexBuffer;
	IndexBufferID IndexBuffer;
	ShaderID Shader;
	TextureID DiffuseMap;
};

class App {
public:
    float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;
//-----------------------------------------------------------------------------------------
// Create vertex buffer of each link
//-----------------------------------------------------------------------------------------
Model *createLink(const float w, const float len)
{
	static const D3DXFROMWINEVECTOR3 verts[] = {
		D3DXFROMWINEVECTOR3(-w, -len, -w), D3DXFROMWINEVECTOR3(-w, -len, w), D3DXFROMWINEVECTOR3(-w, 0, -w), D3DXFROMWINEVECTOR3(-w, 0, w),
		D3DXFROMWINEVECTOR3( w, -len, -w), D3DXFROMWINEVECTOR3( w, -len, w), D3DXFROMWINEVECTOR3( w, 0, -w), D3DXFROMWINEVECTOR3( w, 0, w),
	};

	static const D3DXFROMWINEVECTOR3 norms[] = { D3DXFROMWINEVECTOR3(-1, 0, 0), D3DXFROMWINEVECTOR3(0, 0, 1), D3DXFROMWINEVECTOR3(1, 0, 0), D3DXFROMWINEVECTOR3(0, 0, -1), D3DXFROMWINEVECTOR3(0, 1, 0), D3DXFROMWINEVECTOR3(0, -1, 0) };
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

			*((D3DXFROMWINEVECTOR3 *) dest) = verts[indices[4 * i + k]];
			dest += 3;
			*((D3DXFROMWINEVECTOR3 *) dest) = norms[i];
			dest += 3;
			*((D3DXFROMWINEVECTOR2 *) dest) = D3DXFROMWINEVECTOR2((k >= 1) && (k <= 2), (k > 1));
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
void resetCamera()
{
	camPos = D3DXFROMWINEVECTOR3(30, -15, 0);
	wx = 0.0f;
	wy = 90.0f*D3DXFROMWINE_PI/180.0f;
	speed = 50;
}
//-----------------------------------------------------------------------------------------
// Create R2VB vertex buffer for all vertices of links.
//-----------------------------------------------------------------------------------------
bool createLinksVBO()
{
	numVertices = (NUM_LINKS-1)*36;
	numTriangles = numVertices/3;

	// Create vertex declarations
	if(0){
	FormatDesc modelVertexAttribs[] = {
		1, TYPE_VERTEX,   R2VB_VERTEX_FORMAT, 4,
		0, TYPE_TEXCOORD, R2VB_TEXCOORD_FORMAT, 2,
	};
	if ((VF = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), -1)) == VF_NONE) return false;}

	// 1 pixels can store 1 vertex positions, 1 pixel can store 4 transform matrix indices, so 1 vertex == 2 pixels
	Image3 tex;
	VERTEX_VALUE_FORMAT* lpC = (VERTEX_VALUE_FORMAT*)tex.create(VERTEX_FORMAT, numVertices, 1, 1, 1);
	{
		for (int i=0; i<NUM_LINKS-1; i++)
		{
			unsigned char *lpV = link[i].pModel->getVertexArray();
			
			for (int v=0; v<36; v++)
			{
				D3DXFROMWINEVECTOR3 *lpVertex;
				lpVertex = (D3DXFROMWINEVECTOR3 *)&lpV[v*(link[i].pModel->getVertexSize())];
				*lpC++ = VERTEX_VALUE_FORMAT(lpVertex->x);
				*lpC++ = VERTEX_VALUE_FORMAT(lpVertex->y);
				*lpC++ = VERTEX_VALUE_FORMAT(lpVertex->z);
				*lpC++ = VERTEX_VALUE_FORMAT(1.0f);
			}
		}
	}
	linkVertexTexture = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	// 1 pixels can store 1 vertex positions, 1 pixel can store 4 transform matrix indices, so 1 vertex == 2 pixels
	lpC = (float*)tex.getPixels();
	{
		for (int i=0; i<NUM_LINKS-1; i++)
		{
			unsigned char *lpV = link[i].pModel->getVertexArray();
			
			for (int v=0; v<36; v++)
			{
				D3DXFROMWINEVECTOR3 *lpVertex;
				lpVertex = (D3DXFROMWINEVECTOR3 *)&lpV[v*(link[i].pModel->getVertexSize())];
				*lpC++ = VERTEX_VALUE_FORMAT(i);
				*lpC++ = VERTEX_VALUE_FORMAT(0);
				*lpC++ = VERTEX_VALUE_FORMAT(0);
				*lpC++ = VERTEX_VALUE_FORMAT(0);
			}
		}
	}
	matrixIndexTexture = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Create the texture coord of links
	std::vector<R2VB_TEXCOORD_VALUE_FORMAT> vv(numVertices * 2);
	R2VB_TEXCOORD_VALUE_FORMAT *uvdest = &vv[0];
		for (int i=0; i<NUM_LINKS-1; i++)
		{
			unsigned char *lpV = link[i].pModel->getVertexArray();
			for (int v=0; v<36; v++)
			{
				D3DXFROMWINEVECTOR2 *lpUVList;
				lpUVList = (D3DXFROMWINEVECTOR2 *)&lpV[v*(link[i].pModel->getVertexSize())+(2*sizeof(D3DXFROMWINEVECTOR3))];
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList->x);
				*uvdest++ = R2VB_TEXCOORD_VALUE_FORMAT(lpUVList->y);
			}
		}
	texCoord_VBO = IRenderer::GetRendererInstance()->addVertexBuffer(vv.size(), STATIC, &vv[0]);

	// craete R2VB vertex buffer,Since the render target will be used as vertex buffer we need to
	// signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	R2VB_VBO = IRenderer::GetRendererInstance()->addRenderTarget(numVertices, 1, R2VB_VBO_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());

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
		size = (numVertices) * 1 * 4 * sizeof(R2VB_VERTEX_VALUE_FORMAT);
	else
		size = 2;
	if ((R2VB_DummyVBO = IRenderer::GetRendererInstance()->addVertexBuffer(size)) == VB_NONE) return false;
#endif
	
	//craete index buffer of model
	std::vector<__WORD__> vi(numVertices);
	__WORD__ *iDest = &vi[0];
		for (int i=0; i<numVertices; i++)
			*iDest++ = i;
	IB = IRenderer::GetRendererInstance()->addIndexBuffer(numVertices, sizeof(__WORD__), STATIC, iDest);

	for (int i=0; i<NUM_LINKS; i++)
	{		
		delete link[i].pModel;
	}

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool load()
{
	BaseApp::initGUI();
	// Load shaders
	quadShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-IK/quad.shd", "main", "main");
	computeCCDShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-IK/computeCCD.shd", "main", "main");
	computeTransformMatrixShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-IK/computeTransformMatrix.shd", "main", "main");
	drawLinkShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-IK/drawLink.shd", "main", "main");
	
	transformLinkShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-IK/transformLink.shd", "main", "main");
	generatePivotMatrixShd = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-IK/generatePivotMatrix.shd", "main", "main");

	{FormatDesc quadAttribs[] = {	0, TYPE_VERTEX, FORMAT_FLOAT, 4,
	                            	0, TYPE_TEXCOORD, FORMAT_FLOAT, 2};

	QuadVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), quadShd);
	computeCCDVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), computeCCDShd);
	computeTransformMatrixVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), computeTransformMatrixShd);
	drawLinkVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), drawLinkShd);
	transformLinkVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), transformLinkShd);
	generatePivotMatrixVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), generatePivotMatrixShd);}

	// initialize link data
	link[0].pivotMatrix = identity4();
	link[0].transformMatrix = identity4();
	link[0].parent = 0;
	link[0].pModel = createLink(0.5, LINK_LEN);

	for (int i=1; i<NUM_LINKS; i++)
	{
		link[i].pivotMatrix = identity4();
		link[i].pivotMatrix.m[3][1] = -LINK_LEN;
		link[i].transformMatrix = identity4();

		link[i].parent = &link[i-1];
		link[i].pModel = createLink(0.5, LINK_LEN);
	}

	Image3 tex;
	MATRIX_VALUE_FORMAT* lpC = (MATRIX_VALUE_FORMAT*)tex.create(MATRIX_FORMAT, 4, 1, 1, 1);
	{
		float *lpM = &link[0].pivotMatrix.m[0][0];
			for (int i=0; i<16; i++)
				*lpC++ = MATRIX_VALUE_FORMAT(lpM[i]);
	}
	identityMatrix = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());

	// load target ball model
	target.load(stx_convertpath("/new/R2VB_VTF/R2VB/IK_Target/IK_Target.AM").c_str());

	currentPivotMatrix = 1;
	outputPivotMatrix = 0;
	currentRoatationDegMap = 1;
	outputRoatationDegMap = 0;

	DiffuseMap = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/ChessBoard.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	for (int i=0; i<NUM_LINKS; i++)
	{
		Image3 tex;
		MATRIX_VALUE_FORMAT* lpC = (MATRIX_VALUE_FORMAT*)tex.create(MATRIX_FORMAT, 4, 1, 1, 1);
		{
			float *lpM = &link[i].pivotMatrix.m[0][0];
				for (int i=0; i<16; i++)
					*lpC++ = MATRIX_VALUE_FORMAT(lpM[i]);
		}
		link[i].tOPivotMatrix = IRenderer::GetRendererInstance()->addTexture(&tex, false, IRenderer::GetRendererInstance()->GetnearestClamp());
		
		// Create render target for transform matrixes of links.
		// 1 pixel can save four matrix elements, so one matrix need 4 pixels.
		link[i].tTransformMatrix = IRenderer::GetRendererInstance()->addRenderTarget(4, 1, MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());

		// Create render targets for pivot matrixes of links.
		// 1 pixel can save four matrix elements, so one matrix need 4 pixels.
		// We create two textures for pin-pong rendering.
		link[i].tPivotMatrix[0] = IRenderer::GetRendererInstance()->addRenderTarget(4, 1, MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());
		link[i].tPivotMatrix[1] = IRenderer::GetRendererInstance()->addRenderTarget(4, 1, MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());

		// Create render target for rotation degree of links.
		// 1 pixel can save four rotation degree.
		// We create two textures for pin-pong rendering.
		link[i].tRotationDegMap[0] = IRenderer::GetRendererInstance()->addRenderTarget(1, 1, MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());
		link[i].tRotationDegMap[1] = IRenderer::GetRendererInstance()->addRenderTarget(1, 1, MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());
	}

	// create render target for transform matrix of every links, every 4 pixels can store one transform matrix.
	combinedTransformMatrix = IRenderer::GetRendererInstance()->addRenderTarget(4*(NUM_LINKS-1), 1, MATRIX_FORMAT, IRenderer::GetRendererInstance()->GetnearestClamp());	

	// Create R2VB vertex buffer for all vertices of links.
	if (!createLinksVBO())
	{
		LOG_PRINT("Couldn't create links VBO");
		return false;
	}

	if (!onReset()) return false;

	return true;
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
bool onReset()
{
	for (int i=0; i<NUM_LINKS; i++)
	{
		static D3DXFROMWINEVECTOR4 vertices[] = {
			D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
			D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
			D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
			D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
		};
		
		// copy original pivot matrix into pivot matrix texture
		IRenderer::GetRendererInstance()->changeRenderTarget(link[i].tPivotMatrix[0]);
		IRenderer::GetRendererInstance()->BeginScene();	
			
			IRenderer::GetRendererInstance()->setShader(quadShd);
			IRenderer::GetRendererInstance()->setVertexFormat(QuadVF);
			IRenderer::GetRendererInstance()->setTexture("Diffuse", link[i].tOPivotMatrix);
			IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
			
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));

		IRenderer::GetRendererInstance()->EndScene();

		IRenderer::GetRendererInstance()->changeRenderTarget(link[i].tPivotMatrix[1]);
		IRenderer::GetRendererInstance()->BeginScene();	

			IRenderer::GetRendererInstance()->setShader(quadShd);
			IRenderer::GetRendererInstance()->setVertexFormat(QuadVF);
			IRenderer::GetRendererInstance()->setTexture("Diffuse", link[i].tOPivotMatrix);
			IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
			
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));

		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->changeToMainFramebuffer();		
	}

	return true;
}
//-----------------------------------------------------------------------------------------
// Computer CCD in the GPU. The render target will hold the new pivot matrix.
//------------------------------------------------------------------------------------
void computeCCD(int i, D3DXFROMWINEVECTOR3& endpos)
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};	

	IRenderer::GetRendererInstance()->changeRenderTarget(link[i].tPivotMatrix[outputPivotMatrix]);

		IRenderer::GetRendererInstance()->setShader(computeCCDShd);
		IRenderer::GetRendererInstance()->setVertexFormat(computeCCDVF);
		IRenderer::GetRendererInstance()->setTexture("PivotM", link[i].tPivotMatrix[currentPivotMatrix]);
		IRenderer::GetRendererInstance()->setTexture("LinkTM", link[i].tTransformMatrix);	
		IRenderer::GetRendererInstance()->setTexture("EffectLinkTM", link[NUM_LINKS-1].tTransformMatrix);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		
		IRenderer::GetRendererInstance()->setShaderConstant4f("EndPos", D3DXFROMWINEVECTOR4(endpos, 1.0f));		

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));
}
//-----------------------------------------------------------------------------------------
// Compute hierarchy transform matrixes for each link.
//-----------------------------------------------------------------------------------------
void computeTransformMatrix(int i)
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	IRenderer::GetRendererInstance()->changeRenderTarget(link[i].tTransformMatrix);

		IRenderer::GetRendererInstance()->setShader(computeTransformMatrixShd);
		IRenderer::GetRendererInstance()->setVertexFormat(computeTransformMatrixVF);
		if (link[i].parent)	// if link has parent, use transform matrix of the parent.	
			IRenderer::GetRendererInstance()->setTexture("ParentM", link[i].parent->tTransformMatrix);
		else // if link has no parent, it's the root link. Use indentity matrix as parent transfom matrix.
			IRenderer::GetRendererInstance()->setTexture("ParentM", identityMatrix);		
		IRenderer::GetRendererInstance()->setTexture("PivotM", link[i].tPivotMatrix[outputPivotMatrix]);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));
}
//-----------------------------------------------------------------------------------------
// Combine each transform matrix of links into one texture.
//-----------------------------------------------------------------------------------------
void combineTransformMatrix()
{
	D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	IRenderer::GetRendererInstance()->changeRenderTarget(combinedTransformMatrix);

		vertices[1].x = vertices[0].x + 2.0f/(NUM_LINKS-1);
		vertices[2].x = vertices[0].x + 2.0f/(NUM_LINKS-1);

		for (int i=0; i<NUM_LINKS-1; i++)
		{
			IRenderer::GetRendererInstance()->setShader(quadShd);
			IRenderer::GetRendererInstance()->setVertexFormat(QuadVF);
			IRenderer::GetRendererInstance()->setTexture("Diffuse", link[i].tTransformMatrix);
			IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));

			vertices[0].x += 2.0f/(NUM_LINKS-1);
			vertices[1].x += 2.0f/(NUM_LINKS-1);
			vertices[2].x += 2.0f/(NUM_LINKS-1);
			vertices[3].x += 2.0f/(NUM_LINKS-1);
		}
}
//-----------------------------------------------------------------------------------------
// Transform all vertices into one R2VB vertex buffer.
//-----------------------------------------------------------------------------------------
void transformLinks()
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};

	IRenderer::GetRendererInstance()->changeRenderTarget(R2VB_VBO);

		IRenderer::GetRendererInstance()->setShader(transformLinkShd);
		IRenderer::GetRendererInstance()->setVertexFormat(transformLinkVF);
		IRenderer::GetRendererInstance()->setTexture("VertexTexture", linkVertexTexture);	
		IRenderer::GetRendererInstance()->setTexture("MatrixIndexTexture", matrixIndexTexture);	
		IRenderer::GetRendererInstance()->setTexture("TransformMatrix", combinedTransformMatrix);	
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		
		IRenderer::GetRendererInstance()->setShaderConstant1f("NumLinks", (float)(NUM_LINKS-1));		
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));
}

void drawTarget(D3DXFROMWINEMATRIX& mvp)
{
	target.setup();
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	
	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, target.getVertexCount(), 0, target.getTriangleCount());
}
//-----------------------------------------------------------------------------------------
// Draw all links.
//-----------------------------------------------------------------------------------------
void drawLinks(D3DXFROMWINEMATRIX& mvp)
{
	IRenderer::GetRendererInstance()->setShader(drawLinkShd);
	IRenderer::GetRendererInstance()->setVertexFormat(drawLinkVF);	
	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", R2VB_VBO);

	//IRenderer::GetRendererInstance()->setVertexBuffer(1, R2VB_DummyVBO); //dummy vertex buffer
	IRenderer::GetRendererInstance()->setVertexBuffer(0, texCoord_VBO);
	IRenderer::GetRendererInstance()->setIndexBuffer(IB);	
	IRenderer::GetRendererInstance()->setTexture("DiffuseMap", DiffuseMap);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);

	IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, numVertices, 0, numTriangles);
}
//-----------------------------------------------------------------------------------------
// Dump texture to screen.
//-----------------------------------------------------------------------------------------
void dumpTexture(TextureID ti)
{
	static D3DXFROMWINEVECTOR4 vertices[] = {
		D3DXFROMWINEVECTOR4(-1, -1, 0, 0),
		D3DXFROMWINEVECTOR4( 1, -1, 1, 0),
		D3DXFROMWINEVECTOR4( 1,  1, 1, 1),
		D3DXFROMWINEVECTOR4(-1,  1, 0, 1),
	};
	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
		IRenderer::GetRendererInstance()->setShader(quadShd);
		IRenderer::GetRendererInstance()->setVertexFormat(QuadVF);
		IRenderer::GetRendererInstance()->setTexture("Diffuse", ti);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR4));
}
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
void drawFrame()
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

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 1, 20000, true);

	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMATRIX mvpSB = projection * modelview;
	modelview.translate(-camPos);
	D3DXFROMWINEMATRIX mvp = projection * modelview;

	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.5, 0.5, 0.5, 0));
	drawLinks(mvp);
	drawTarget(mvp);
}
private:
	D3DXFROMWINEVECTOR3 endPos;
	Target target;
	Link link[NUM_LINKS];
	TextureID DiffuseMap;

	int numVertices, numTriangles;
	TextureID identityMatrix;
	TextureID combinedTransformMatrix;
	VertexFormatID VF;
	TextureID linkVertexTexture, matrixIndexTexture, R2VB_VBO;
	//VertexBufferID R2VB_DummyVBO;
	VertexBufferID texCoord_VBO;
	IndexBufferID IB;

	ShaderID quadShd, computeCCDShd, computeTransformMatrixShd, drawLinkShd, generatePivotMatrixShd, transformLinkShd;
	VertexFormatID QuadVF, computeCCDVF, computeTransformMatrixVF, drawLinkVF, generatePivotMatrixVF, transformLinkVF;
	//VertexBufferID QuadVB;

	int currentPivotMatrix, outputPivotMatrix;
	int currentRoatationDegMap, outputRoatationDegMap;
};
App app;
int init(const char* aTitle)
{
	//app.setup();
	app.resetCamera();
	app.load();
	return 0;
}

void render()
{
	app.drawFrame();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-IK");
	IInput*    i=STX_Service::GetInputInstance();
	STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

//-----------------------------------------------------------------------------------------
// Load target ball model data.
//-----------------------------------------------------------------------------------------
bool Target::load(char *name)
{
	Shader = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-IK/target.shd", "main", "main");

	ATIModel::load(name);
	FormatDesc modelVertexAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD,  FORMAT_FLOAT, 2,
	};

	if ((VertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(modelVertexAttribs, elementsOf(modelVertexAttribs), Shader)) == VF_NONE) return false;
	// Create the model vertex buffer for vertes shader animation	
	std::vector<float> vv(numVertices*6);
	float *dest = &vv[0];
	for (__DWORD__ i=0; i<numVertices; i++) 
	{
		(*dest++) = pVPosition[i].x;
		(*dest++) = pVPosition[i].y;
		(*dest++) = pVPosition[i].z;
		(*dest++) = 1.0f;
		(*dest++) = pVTexcoord[i].x;
		(*dest++) = pVTexcoord[i].y;
	}
	VertexBuffer = IRenderer::GetRendererInstance()->addVertexBuffer(vv.size(), STATIC, &vv[0]);

	std::vector<__DWORD__> vi(numTriangles*3);
	__DWORD__ *iDest = &vi[0];
		for (__DWORD__ i=0; i<numTriangles; i++) 
		{
			(*iDest++) = pTriangle[i].v[0];
			(*iDest++) = pTriangle[i].v[1];
			(*iDest++) = pTriangle[i].v[2];
		}		
	IndexBuffer = IRenderer::GetRendererInstance()->addIndexBuffer(numTriangles*3, sizeof(__DWORD__), STATIC, &vi[0]);

	char FullName[256];	
	sprintf(FullName, "%s%s", TEX_PATH, pMaterial[0].diffuseMap);
	DiffuseMap = IRenderer::GetRendererInstance()->addImageLibTexture(FullName, false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	setRange(0.0f, 1000.0f);
	setTime(0.0f);

	return true;
}

//-----------------------------------------------------------------------------------------
// Draw target ball.
//-----------------------------------------------------------------------------------------
void Target::setup(){
	IRenderer::GetRendererInstance()->setShader(Shader);
	IRenderer::GetRendererInstance()->setVertexFormat(VertexFormat);
	IRenderer::GetRendererInstance()->setVertexBuffer(0, VertexBuffer);
	IRenderer::GetRendererInstance()->setIndexBuffer(IndexBuffer);	
	IRenderer::GetRendererInstance()->setTexture("DiffuseMap", DiffuseMap);
	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("trans", pMesh[0].getTransformMatrix());
}

