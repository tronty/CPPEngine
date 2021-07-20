//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#include "App.h"

BaseApp *app = new App();

void App::setup(char *args[], int nArguments){
	width  = 800;
	height = 600;
	speed = 2;
}

void App::resetCamera(){
	camPos = float3(2, 0, 0);
	wx = 0;
	wy = PI / 2;
}

void App::initGUI(){
	// Initialize the GUI
	nPatchMenu = mainMenu->addMenu("N-Patches");
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

	D3D9App::initGUI();
}

void App::onMenuItemPress(Menu *menu, MenuItemID item){
	if (menu == nPatchMenu){
		if (item != noNPatches && item < drawWireframe){
			changeNPatchLevel(item - 1);
		}
	} else {
		D3D9App::onMenuItemPress(menu, item);
	}
}

void App::changeNPatchLevel(const int level){
	static int lastLevel = -1;
	if (level == lastLevel) return;
	lastLevel = level;

	// The number of vertices created from each source triangle
	int vertexFactor = (level + 2) * (level + 3) / 2;

	if (rtVertex != TEXTURE_NONE) renderer->deleteTexture(rtVertex);
	if (rtNormal != TEXTURE_NONE) renderer->deleteTexture(rtNormal);

	/*
		Since this render target will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	LPDIRECT3DTEXTURE9 d3drt;
	if (dev->CreateTexture(vertexFactor * texWidth, texHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return;
	}
	if ((rtVertex = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return;

	if (dev->CreateTexture(vertexFactor * texWidth, texHeight, 1, D3DUSAGE_RENDERTARGET | D3DUSAGE_DMAP, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &d3drt, NULL) != D3D_OK){
		ERRORMSG("Couldn't create render target");
		return;
	}
	if ((rtNormal = ((D3D9Renderer *) renderer)->addRenderTarget(d3drt, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return;


	/*
		We need a dummy vertex buffer to bind. When the debug runtime is used we have to ensure it's large
		enough to be valid for the draw call. For the relase runtime this is not needed, so we use a minimal
		vertex buffer instead.
	*/
	if (debugRuntime){
		if (dummy != TEXTURE_NONE) renderer->deleteVertexBuffer(dummy);
		if ((dummy = renderer->addVertexBuffer(vertexFactor * texWidth * texHeight * sizeof(half) * 4)) == VB_NONE) return;
	} else {
		if (dummy == TEXTURE_NONE){
			if ((dummy = renderer->addVertexBuffer(1)) == VB_NONE) return;
		}
	}

	needsUpdate = true;
}

bool App::load(){
	// Check for R2VB support
	if (d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, (D3DFORMAT) FOURCC('R','2','V','B')) != D3D_OK){
		ERRORMSG("No R2VB support");
		return false;
	}

	// Check for VS3.0 support
	if (caps.VertexShaderVersion < D3DVS_VERSION(3,0)){
		ERRORMSG("VS3.0 is not supported");
		return false;
	}

	// Load the source model
	LPD3DXMESH mesh;
	if (FAILED(D3DXLoadMeshFromX(MODEL_PATH "tiger.x", D3DXMESH_MANAGED, dev, NULL, NULL, NULL, NULL, &mesh))) return false;

	// Set up the vertex attributes we require
	const D3DVERTEXELEMENT9 modelVertexAttribs[] = {
		{ 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	// Clone mesh to make room for normals
	mesh->CloneMesh(D3DXMESH_SYSTEMMEM, modelVertexAttribs, dev, &tiger);
	mesh->Release();

	D3DXComputeNormals(tiger, NULL);

	// Compute how large textures we need to store the model.
	nFaces = tiger->GetNumFaces();
	texWidth = 64;
	texHeight = (nFaces + texWidth - 1) / texWidth;

	struct Vertex {
		vec3 pos;
		vec3 normal;
		vec2 texCoord;
	};

#define INDEX_TYPE ushort

	Vertex *vertices;
	INDEX_TYPE *indices;

	tiger->LockVertexBuffer(0, (void **) &vertices);
	tiger->LockIndexBuffer(0, (void **) &indices);

		// Copy the model data over to a set of textures
		Image imgVert[3], imgNorm[3];
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
				vDest[k][i] = float4(vertices[index].pos,    vertices[index].texCoord.x);
				nDest[k][i] = float4(vertices[index].normal, vertices[index].texCoord.y);
			}
		}

		for (int i = 0; i < 3; i++){
			if ((modelVertices[i] = renderer->addTexture(imgVert[i], FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
			if ((modelNormals [i] = renderer->addTexture(imgNorm[i], FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
		}

	tiger->UnlockVertexBuffer();
	tiger->UnlockIndexBuffer();

	if ((base = renderer->addTexture(TEX_PATH "tiger.bmp", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	// Setup initial N-Patch mode
	rtVertex = rtNormal = TEXTURE_NONE;
	dummy = VB_NONE;
	changeNPatchLevel(2);

	// Load shaders
	if ((lighting = renderer->addShader("lighting.shd")) == SHADER_NONE) return false;
	if ((nPatchLighting = renderer->addShader("lighting.shd", "#define N_PATCHES\n")) == SHADER_NONE) return false;
	if ((wireframe = renderer->addShader("wireframe.shd")) == SHADER_NONE) return false;
	if ((nPatch = renderer->addShader("NPatch.shd")) == SHADER_NONE) return false;



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

		Image img;
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

		if ((weights[i] = renderer->addTexture(img, FILTER_POINT, CLAMP)) == TEXTURE_NONE) return false;
	}

	

	VertexAttribute quadAttribs[] = { TYPE_VERTEX, FORMAT_FLOAT, 2 };
	if ((quadVF = renderer->addVertexFormat(quadAttribs, elementsOf(quadAttribs))) == TEXTURE_NONE) return false;

	StreamVertexAttribute nPatchAttribs[] = {
		0, TYPE_VERTEX, FORMAT_HALF, 4,
		1, TYPE_NORMAL, FORMAT_HALF, 4,
	};
	if ((nPatchVF = renderer->addVertexFormat(nPatchAttribs, elementsOf(nPatchAttribs))) == TEXTURE_NONE) return false;

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

		if ((nPatchIndices[k] = renderer->addIndexBuffer(3 * faceFactor * nFaces, sizeof(uint))) == IB_NONE) return false;
		uint *iDest = (uint *) renderer->lockIndexBuffer(nPatchIndices[k]);
		if (iDest == NULL) return false;

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
		renderer->unlockIndexBuffer(nPatchIndices[k]);
	}

	return true;
}

bool App::unload(){
	tiger->Release();
	return true;
}

void App::drawFrame(){
	float4x4 projection = perspectiveMatrixX(PI / 2, width, height, 0.01f, 10, true);

	float4x4 modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	float4x4 mvp = projection * modelview;

	float3 lightPos(10, 4, 1);

	const float depthBias = -0.00001f;
	const float slopeBias = -0.15f;

	if (nPatchMenu->isItemChecked(noNPatches)){
		// Render the model normally with no N-Patches
		dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF336688, 1.0f, 0);

		renderer->setCullFace(BACK);
		renderer->setShader(lighting);
		renderer->setTexture("Base", base);
		renderer->setShaderConstant4x4f("mvp", mvp);
		renderer->setShaderConstant3f("lightPos", lightPos);
		renderer->setShaderConstant3f("camPos", camPos);
		renderer->apply();

		tiger->DrawSubset(0);

		if (nPatchMenu->isItemChecked(drawWireframe)){
			// Draw a wireframed version on top
			dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD *) &depthBias);
			dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *) &slopeBias);

			renderer->setCullFace(BACK);
			renderer->setShader(wireframe);
			renderer->setShaderConstant4x4f("mvp", mvp);
			renderer->setMask(COLOR);
			renderer->apply();

			tiger->DrawSubset(0);

			dev->SetRenderState(D3DRS_DEPTHBIAS, 0);
			dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
			dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}

	} else {
		int nPatchLevel = nPatchMenu->getCheckedRadioButton(1) - 1;

		// The number of vertices and faces created from each source triangle
		int vertexFactor = (nPatchLevel + 2) * (nPatchLevel + 3) / 2;
		int faceFactor = (nPatchLevel + 1) * (nPatchLevel + 1);

		const float2 vertices[]  = { float2(-1, -1), float2(1, -1), float2(1, 1), float2(-1, 1) };

		if (needsUpdate || nPatchMenu->isItemChecked(updateEveryFrame)){
			/*
				This pass tesselates and interpolates the N-Patch from the source model.
				Since we in this sample only use one model and it's static we don't really need
				to do this more than once. But for studying performance of doing N-Patches this
				way it's useful to see the framerate with it updated every frame, which it
				likely would have to be in real world applications (otherwise you would just
				expand the N-Patches on the CPU instead).
			*/
			TextureID rt[] = { rtVertex, rtNormal };
			renderer->changeRenderTargets(elementsOf(rt), rt);
				renderer->setShader(nPatch);
				renderer->setTexture("Pos0", modelVertices[0]);
				renderer->setTexture("Nrm0", modelNormals [0]);
				renderer->setTexture("Pos1", modelVertices[1]);
				renderer->setTexture("Nrm1", modelNormals [1]);
				renderer->setTexture("Pos2", modelVertices[2]);
				renderer->setTexture("Nrm2", modelNormals [2]);
				renderer->setTexture("Weights", weights[nPatchLevel]);
				renderer->setShaderConstant2f("halfPixel", float2(0.5f / (vertexFactor * texWidth), 0.5f / texHeight));
				renderer->setShaderConstant1f("width", float(texWidth));
				renderer->setVertexFormat(quadVF);
				renderer->setDepthFunc(DEPTH_NONE);
				renderer->apply();

				dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(float2));

			renderer->resetRenderTarget();
			needsUpdate = false;
		}

		dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF336688, 1.0f, 0);


		// Enable render to vertex buffer extension
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(TRUE));

			// Bind the render target to the vertex texture samplers
			dev->SetTexture(D3DVERTEXTEXTURESAMPLER0, ((D3D9Renderer *) renderer)->getD3DTexture(rtVertex));
			dev->SetTexture(D3DVERTEXTEXTURESAMPLER1, ((D3D9Renderer *) renderer)->getD3DTexture(rtNormal));

			// Tell the driver that stream 0 and 1 are to be fetched from the vertex textures
			dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_VTX0));
			dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_VTX1));


			renderer->setCullFace(BACK);
			renderer->setShader(nPatchLighting);
			renderer->setTexture("Base", base);
			renderer->setShaderConstant4x4f("mvp", mvp);
			renderer->setShaderConstant3f("lightPos", lightPos);
			renderer->setShaderConstant3f("camPos", camPos);
			renderer->setVertexFormat(nPatchVF);
			renderer->setVertexBuffer(dummy, 0); // Dummy vertex buffer
			renderer->setVertexBuffer(dummy, 1); // Dummy vertex buffer
			renderer->setIndexBuffer(nPatchIndices[nPatchLevel]);
			renderer->apply();

			dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexFactor * texWidth * texHeight, 0, faceFactor * nFaces);

			if (nPatchMenu->isItemChecked(drawWireframe)){
				// Draw a wireframed version on top
				renderer->setCullFace(BACK);
				renderer->setShader(wireframe);
				renderer->setShaderConstant4x4f("mvp", mvp);
				renderer->setVertexFormat(nPatchVF);
				renderer->setVertexBuffer(dummy, 0); // Dummy vertex buffer
				renderer->setVertexBuffer(dummy, 1); // Dummy vertex buffer
				renderer->setIndexBuffer(nPatchIndices[nPatchLevel]);
				renderer->setMask(COLOR);
				renderer->apply();

				dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

				dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD *) &depthBias);
				dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD *) &slopeBias);

				dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexFactor * texWidth * texHeight, 0, faceFactor * nFaces);

				dev->SetRenderState(D3DRS_DEPTHBIAS, 0);
				dev->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

				dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			}

			// Streams restored to regular vertex buffer mode
			dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(0, R2VB_VSMP_OVR_DIS));
			dev->SetRenderState(D3DRS_POINTSIZE, r2vbVStrm2SmpMap_Set(1, R2VB_VSMP_OVR_DIS));

			// Unbind the vertex textures
			dev->SetTexture(D3DVERTEXTEXTURESAMPLER0, NULL);
			dev->SetTexture(D3DVERTEXTEXTURESAMPLER1, NULL);

		// Disable render to vertex buffer extension
		dev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set(FALSE));
	}
}
