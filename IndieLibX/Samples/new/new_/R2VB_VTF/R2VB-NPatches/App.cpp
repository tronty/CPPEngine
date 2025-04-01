/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

class App {
public:

void setup(){
	width  = 800;
	height = 600;
	speed = 2;
}

void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(2, 0, 0);
	wx = 0;
	wy = D3DXFROMWINE_PI / 2;
}

#ifdef R2VB_GUI
void initGUI(){
	// Initialize the GUI
	nPatchMenu = BaseApp::getMenu()->addMenu("N-Patches");
	noNPatches = nPatchMenu->addItem("Draw normally", MENU_CHECKABLE);
	nPatchMenu->addSeparatorAfter(noNPatches);
	nPatchMenu->addItem("1:1",  MENU_CHECKABLE);
	nPatchMenu->addItem("1:4",  MENU_CHECKABLE);
	nPatchMenu->addItem("1:9",  MENU_CHECKABLE | MENU_CHECKED);
	nPatchMenu->addItem("1:16", MENU_CHECKABLE);
	nPatchMenu->addItem("1:25", MENU_CHECKABLE);
	nPatchMenu->addItem("1:36", MENU_CHECKABLE);
	nPatchMenu->addItem("1:49", MENU_CHECKABLE);
	MenuItemID np = nPatchMenu->addItem("1:64", MENU_CHECKABLE);
	nPatchMenu->setRadioButtonGroup(noNPatches, np, 1);
	nPatchMenu->addSeparatorAfter(np);

	drawWireframe = nPatchMenu->addItem("Draw wireframe", MENU_CHECKABLE);
	updateEveryFrame = nPatchMenu->addItem("Update every frame", MENU_CHECKABLE | MENU_CHECKED);

	//D3D9App::initGUI();
}
#endif
void changeNPatchLevel(const int level){
	static int lastLevel = -1;
	if (level == lastLevel) return;
	lastLevel = level;

	// The number of vertices created from each source triangle
	int vertexFactor = (level + 2) * (level + 3) / 2;

#if 0
	if (rtVertex != TEXTURE_NONE) IRenderer::GetRendererInstance()->deleteTexture(rtVertex);
	if (rtNormal != TEXTURE_NONE) IRenderer::GetRendererInstance()->deleteTexture(rtNormal);
#endif

	/*
		Since this render target will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	rtVertex = IRenderer::GetRendererInstance()->addRenderTarget(vertexFactor * texWidth, texHeight, /* FORMAT_RGBA32f */ FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());

	rtNormal = IRenderer::GetRendererInstance()->addRenderTarget(vertexFactor * texWidth, texHeight, /* FORMAT_RGBA32f */ FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());


	/*
		We need a dummy vertex buffer to bind. When the debug runtime is used we have to ensure it's large
		enough to be valid for the draw call. For the relase runtime this is not needed, so we use a minimal
		vertex buffer instead.
	*/
#if 0
	if (debugRuntime){
		if (dummy != TEXTURE_NONE) IRenderer::GetRendererInstance()->deleteVertexBuffer(dummy);
		if ((dummy = IRenderer::GetRendererInstance()->addVertexBuffer(vertexFactor * texWidth * texHeight * sizeof(half) * 4)) == VB_NONE) return;
	} else {
		if (dummy == TEXTURE_NONE){
			if ((dummy = IRenderer::GetRendererInstance()->addVertexBuffer(1)) == VB_NONE) return;
		}
	}
#else
    dummy = IRenderer::GetRendererInstance()->addVertexBuffer(vertexFactor * texWidth * texHeight * sizeof(float) * 2);
#endif
	needsUpdate = true;
}

bool load(){
#if 0
	// Check for VS3.0 support
	if(IRenderer::GetRendererInstance()->GetSHADING_LANGUAGE_VERSION_MAJOR()<3){
		LOG_PRINT("VS3.0 is not supported");
		return false;
	}
#endif
	// Load shaders
	lighting = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-NPatches/lighting.hlsl", "main", "main");
	nPatchLighting = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-NPatches/lighting.hlsl", "main", "main");//, "#define N_PATCHES\n");
	wireframe = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-NPatches/wireframe.hlsl", "main", "main");
	nPatch = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-NPatches/NPatch.hlsl", "main", "main");

#if 0
	// Load the source model
	MeshRenderer2* mesh;
	if (FAILED(D3DXFROMWINELoadMeshFromX(MODEL_PATH "tiger.x", D3DXFROMWINEMESH_MANAGED, 0, 0, 0, 0, &mesh))) return false;

	// Set up the vertex attributes we require
	const FormatDesc modelVertexAttribs[] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	// Clone mesh to make room for normals
	mesh->CloneMesh(D3DXFROMWINEMESH_SYSTEMMEM, modelVertexAttribs, &tiger);
	mesh->Release();

	D3DXFROMWINEComputeNormals(tiger, 0);

	// Compute how large textures we need to store the model.
	nFaces = tiger->GetNumFaces();
	texWidth = 64;
	texHeight = (nFaces + texWidth - 1) / texWidth;

	struct Vertex {
		D3DXFROMWINEVECTOR3 pos;
		D3DXFROMWINEVECTOR3 normal;
		D3DXFROMWINEVECTOR2 texCoord;
	};

#define INDEX_TYPE ushort

	Vertex *vertices;
	INDEX_TYPE *indices;

	tiger->LockVertexBuffer(0, (void **) &vertices);
	tiger->LockIndexBuffer(0, (void **) &indices);

		// Copy the model data over to a set of textures
		Image3 imgVert[3], imgNorm[3];
		half4 *vDest[3], *nDest[3];

		for (int i = 0; i < 3; i++){
			vDest[i] = (half4 *) imgVert[i].create(FORMAT_RGBA16f, texWidth, texHeight, 1, 1);
			nDest[i] = (half4 *) imgNorm[i].create(FORMAT_RGBA16f, texWidth, texHeight, 1, 1);
		}

		for (int i = 0; i < nFaces; i++){
			for (int k = 0; k < 3; k++){
				int index = indices[3 * i + k];
				/*
					Both position and normal only needs three components, so we use
					the two spare .w components for storing the texture coordinates.
				*/
				vDest[k][i] = D3DXFROMWINEVECTOR4(vertices[index].pos,    vertices[index].texCoord.x);
				nDest[k][i] = D3DXFROMWINEVECTOR4(vertices[index].normal, vertices[index].texCoord.y);
			}
		}

		for (int i = 0; i < 3; i++){
			modelVertices[i] = IRenderer::GetRendererInstance()->addTexture(imgVert[i], IRenderer::GetRendererInstance()->GetnearestClamp());
			modelNormals[i] = IRenderer::GetRendererInstance()->addTexture(imgNorm[i], IRenderer::GetRendererInstance()->GetnearestClamp());
		}

	tiger->UnlockVertexBuffer();
	tiger->UnlockIndexBuffer();
#endif
	base=IRenderer::GetRendererInstance()->addImageLibTexture("/dx9sdk/tiger.bmp", false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	// Setup initial N-Patch mode
	rtVertex = rtNormal = TEXTURE_NONE;
	dummy = VB_NONE;
	changeNPatchLevel(2);

	/*
		Compute barycentric coordinates. These will be used for cubic interpolation
		of vertices and quadratic interpolation of normals when tesselating.

		        (1,0,0)
		           /\
		          /__\
		         /\  /\
		        /__\/__\
		       /\  /\  /\
		      /__\/__\/__\
		     /\  /\  /\  /\
		    /__\/__\/__\/__\
		(0,0,1)          (0,1,0)
	*/
	for (int i = 0; i < 8; i++){
		int n = i + 2;
		int w = n * (n + 1) / 2;

		Image3 img;
		half4 *dest = (half4 *) img.create(FORMAT_RGBA16f, w, 1, 1, 1);

		uint index = 0;
		for (int y = 0; y < n; y++){
			float w = float(y) / float(n - 1);
			float w0 = 1.0f - w;

			for (int x = 0; x <= y; x++){
				float t = (y > 0)? float(x) / float(y) : 0.0f;

				float w1 = lerp(0, w, t);
				float w2 = lerp(w, 0, t);

				dest[index].x = w0;
				dest[index].y = w1;
				dest[index].z = w2;
				// Also use the available .w component to cut a few instructions in the shader.
				dest[index].w = 6 * w0 * w1 * w2;
				index++;
			}
		}

		weights[i] = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->GetnearestClamp());
	}

	

	FormatDesc quadAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
	quadVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), nPatch);

	FormatDesc nPatchAttribs[] = {
		0, TYPE_VERTEX, FORMAT_HALF, 4,
		1, TYPE_NORMAL, FORMAT_HALF, 4,
	};
	nPatchVF = IRenderer::GetRendererInstance()->addVertexFormat(nPatchAttribs, elementsOf(nPatchAttribs), nPatch);

	/*
		Compute index buffers. This is the vertex ordering:
		       0
		      /\
		    1/__\2
		    /\  /\
		  3/__\4__\5
		  /\  /\  /\
		6/__\7__\8__\9
	*/
	for (int k = 0; k < 8; k++){
		int faceFactor = (k + 1) * (k + 1);
		int vertexFactor = (k + 2) * (k + 3) / 2;
		int n = k + 1;
		std::vector<uint> vi(3 * faceFactor * nFaces);
		uint *iDest = &vi[0];
		for (int i = 0; i < nFaces; i++){
			int base = i * vertexFactor;

			for (int y = 0; y < n; y++){
				int r0 = base + y * (y + 1) / 2;
				int r1 = base + (y + 1) * (y + 2) / 2;
				for (int x = 0; x < y; x++){
					*iDest++ = r0;
					*iDest++ = r1 + 1;
					*iDest++ = r1;

					*iDest++ = r0;
					*iDest++ = r0 + 1;
					*iDest++ = r1 + 1;

					r0++;
					r1++;
				}
				*iDest++ = r0;
				*iDest++ = r1 + 1;
				*iDest++ = r1;
			}
		}
		nPatchIndices[k] = IRenderer::GetRendererInstance()->addIndexBuffer(3 * faceFactor * nFaces, 2, STATIC, &vi[0]);
	}

	return true;
}

bool unload(){
	//tiger->Release();
	return true;
}

void drawFrame(){
	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 0.01f, 10, true);

	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	D3DXFROMWINEMATRIX mvp = projection * modelview;

	D3DXFROMWINEVECTOR3 lightPos(10, 4, 1);

	const float depthBias = -0.00001f;
	const float slopeBias = -0.15f;

	if (nPatchMenu && nPatchMenu->isItemChecked(noNPatches))
    {
		// Render the model normally with no N-Patches
		IRenderer::GetRendererInstance()->Clear(true, true, 0xFF336688);

		IRenderer::GetRendererInstance()->setCullFace(eBACK);
		IRenderer::GetRendererInstance()->setShader(lighting);
		IRenderer::GetRendererInstance()->setTexture("Base", base);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
		IRenderer::GetRendererInstance()->setShaderConstant3f("lightPos", lightPos);
		IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", camPos);
		if(tiger)
			tiger->render(&mvp);

		if (nPatchMenu && nPatchMenu->isItemChecked(drawWireframe))
        {
			// Draw a wireframed version on top
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DEPTHBIAS, *(__DWORD__ *) &depthBias);
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(__DWORD__ *) &slopeBias);

			IRenderer::GetRendererInstance()->setCullFace(eBACK);
			IRenderer::GetRendererInstance()->setShader(wireframe);
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
			IRenderer::GetRendererInstance()->setMask(eCOLOR);

			if(tiger)
				tiger->render(&mvp);

			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DEPTHBIAS, 0);
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}

	} else {
		int nPatchLevel = 1;
		if(nPatchMenu)
			nPatchMenu->getCheckedRadioButton(1) - 1;

		// The number of vertices and faces created from each source triangle
		int vertexFactor = (nPatchLevel + 2) * (nPatchLevel + 3) / 2;
		int faceFactor = (nPatchLevel + 1) * (nPatchLevel + 1);

		const D3DXFROMWINEVECTOR2 vertices[]  = { D3DXFROMWINEVECTOR2(-1, -1), D3DXFROMWINEVECTOR2(1, -1), D3DXFROMWINEVECTOR2(1, 1), D3DXFROMWINEVECTOR2(-1, 1) };

		if (needsUpdate || (nPatchMenu && nPatchMenu->isItemChecked(updateEveryFrame)))
        {
			/*
				This pass tesselates and interpolates the N-Patch from the source model.
				Since we in this sample only use one model and it's static we don't really need
				to do this more than once. But for studying performance of doing N-Patches this
				way it's useful to see the framerate with it updated every frame, which it
				likely would have to be in real world applications (otherwise you would just
				expand the N-Patches on the CPU instead).
			*/
			TextureID rt[] = { rtVertex, rtNormal };
			IRenderer::GetRendererInstance()->changeRenderTargets(rt, elementsOf(rt));
				IRenderer::GetRendererInstance()->setShader(nPatch);
				IRenderer::GetRendererInstance()->setVertexFormat(quadVF);
				IRenderer::GetRendererInstance()->setTexture("Pos0", modelVertices[0]);
				IRenderer::GetRendererInstance()->setTexture("Nrm0", modelNormals [0]);
				IRenderer::GetRendererInstance()->setTexture("Pos1", modelVertices[1]);
				IRenderer::GetRendererInstance()->setTexture("Nrm1", modelNormals [1]);
				IRenderer::GetRendererInstance()->setTexture("Pos2", modelVertices[2]);
				IRenderer::GetRendererInstance()->setTexture("Nrm2", modelNormals [2]);
				IRenderer::GetRendererInstance()->setTexture("Weights", weights[nPatchLevel]);
				IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / (vertexFactor * texWidth), 0.5f / texHeight));
				IRenderer::GetRendererInstance()->setShaderConstant1f("width", float(texWidth));
				IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

				IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

			IRenderer::GetRendererInstance()->changeToMainFramebuffer();
			needsUpdate = false;
		}

		IRenderer::GetRendererInstance()->Clear(true, true, 0xFF336688);
		
			IRenderer::GetRendererInstance()->setShader(nPatchLighting);
			IRenderer::GetRendererInstance()->setVertexFormat(nPatchVF);

			// Bind the render target to the vertex texture samplers
			IRenderer::GetRendererInstance()->setTexture("DMAP0", rtVertex);
			IRenderer::GetRendererInstance()->setTexture("DMAP1", rtNormal);


			IRenderer::GetRendererInstance()->setCullFace(eBACK);
			IRenderer::GetRendererInstance()->setTexture("Base", base);
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
			IRenderer::GetRendererInstance()->setShaderConstant3f("lightPos", lightPos);
			IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", camPos);
			IRenderer::GetRendererInstance()->setVertexBuffer(0,dummy); // Dummy vertex buffer
			//IRenderer::GetRendererInstance()->setVertexBuffer(1, dummy); // Dummy vertex buffer
			IRenderer::GetRendererInstance()->setIndexBuffer(nPatchIndices[nPatchLevel]);
			

			IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, vertexFactor * texWidth * texHeight, 0, faceFactor * nFaces);

			if (nPatchMenu && nPatchMenu->isItemChecked(drawWireframe))
            {
				IRenderer::GetRendererInstance()->setShader(wireframe);
				IRenderer::GetRendererInstance()->setVertexFormat(nPatchVF);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
				// Draw a wireframed version on top
				IRenderer::GetRendererInstance()->setCullFace(eBACK);
				IRenderer::GetRendererInstance()->setVertexBuffer(0, dummy); // Dummy vertex buffer
				//IRenderer::GetRendererInstance()->setVertexBuffer(1, dummy); // Dummy vertex buffer
				IRenderer::GetRendererInstance()->setIndexBuffer(nPatchIndices[nPatchLevel]);
				IRenderer::GetRendererInstance()->setMask(eCOLOR);
				

				//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

				//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DEPTHBIAS, *(__DWORD__ *) &depthBias);
				//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(__DWORD__ *) &slopeBias);

				IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, vertexFactor * texWidth * texHeight, 0, faceFactor * nFaces);

				//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_DEPTHBIAS, 0);
				//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

				//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			}
	}
}

private:
	ShaderID nPatch, lighting, nPatchLighting, wireframe;
	TextureID base;

	VertexFormatID quadVF, nPatchVF;
	VertexBufferID dummy;
	IndexBufferID nPatchIndices[8];

	TextureID weights[8];

	TextureID modelVertices[3], modelNormals[3];
	int texWidth, texHeight;
	int nFaces;

	TextureID rtVertex, rtNormal;

	MeshRenderer2* tiger;

	bool needsUpdate;
	float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;
#if 1//def R2VB_GUI
	Menu *nPatchMenu;
	MenuItemID noNPatches, updateEveryFrame, drawWireframe;
#endif
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
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-NPatches");
	IInput*    i=STX_Service::GetInputInstance();
	//STX_Service::GetAudioInstance()->Init();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

