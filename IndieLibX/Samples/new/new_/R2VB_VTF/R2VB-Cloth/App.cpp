/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <Util/ProgrammerArt.h>
#include <Math/Perlin.h>

#define CLOTH_SIZE 64

#if (CLOTH_SIZE * CLOTH_SIZE <= 65536)
typedef ushort INDEX_TYPE;
#else
typedef uint INDEX_TYPE;
#endif

class App {
public:
    float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed;
    bool debugRuntime;
void setup(){
	width  = 960;
	height = 720;
	speed = 3;
}

void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(0, 0, -2);
	wx = 0;
	wy = 0;
}

#ifdef R2VB_GUI
void initGUI(){
	// Set up menu items
	clothMenu = BaseApp::getMenu()->addMenu("Cloth");
	curtain = clothMenu->addItem("Curtain", MENU_CHECKABLE);
	sail = clothMenu->addItem("Sail", MENU_CHECKABLE | MENU_CHECKED);
	safetyNet = clothMenu->addItem("Safety net", MENU_CHECKABLE);
	roundTable = clothMenu->addItem("Round table", MENU_CHECKABLE);
	squareTable = clothMenu->addItem("Square table", MENU_CHECKABLE);
	squareTable2 = clothMenu->addItem("Square table 2", MENU_CHECKABLE);

	clothMenu->setRadioButtonGroup(curtain, squareTable2, 1);

	multipass = clothMenu->addItem("Multipass", MENU_CHECKABLE);
	separateStreams = clothMenu->addItem("Separate R2VB streams", MENU_CHECKABLE | MENU_CHECKED);
	clothMenu->addSeparatorBefore(multipass);

	// Add sliders for wind control
	BaseApp::addWidget(xLabel = new Label(13, 583, BaseApp::getMainFont(), "X", 14, 18, 0, 0, HA_CENTER));
	BaseApp::addWidget(yLabel = new Label(13, 613, BaseApp::getMainFont(), "Y", 14, 18, 0, 0, HA_CENTER));
	BaseApp::addWidget(zLabel = new Label(13, 643, BaseApp::getMainFont(), "Z", 14, 18, 0, 0, HA_CENTER));
	BaseApp::addWidget(xWind = new Slider(26, 580, 170, 22, true, -10, 10, 4));
	BaseApp::addWidget(yWind = new Slider(26, 610, 170, 22, true, -10, 10, 3));
	BaseApp::addWidget(zWind = new Slider(26, 640, 170, 22, true, -10, 10, -2));

	BaseApp::addWidget(xnLabel = new Label(13, 683, BaseApp::getMainFont(), "Nx", 14, 18, 0, 0, HA_CENTER));
	BaseApp::addWidget(ynLabel = new Label(13, 713, BaseApp::getMainFont(), "Ny", 14, 18, 0, 0, HA_CENTER));
	BaseApp::addWidget(znLabel = new Label(13, 743, BaseApp::getMainFont(), "Nz", 14, 18, 0, 0, HA_CENTER));
	BaseApp::addWidget(xnWind = new Slider(26, 680, 170, 22, true, 0, 15, 5));
	BaseApp::addWidget(ynWind = new Slider(26, 710, 170, 22, true, 0, 15, 5));
	BaseApp::addWidget(znWind = new Slider(26, 740, 170, 22, true, 0, 15, 5));

	BaseApp::initGUI();
}
#endif

void onMenuItemPress(Menu *menu, MenuItemID item){
	// Handle the presets
	if (menu == clothMenu){
		if (item != separateStreams){
			if (item == curtain){
				xWind->setValue(0);
				yWind->setValue(0);
				zWind->setValue(0);
				xnWind->setValue(10);
				ynWind->setValue(2);
				znWind->setValue(10);
			} else if (item == sail){
				xWind->setValue(4);
				yWind->setValue(3);
				zWind->setValue(-2);
				xnWind->setValue(5);
				ynWind->setValue(5);
				znWind->setValue(5);
			} else if (item == safetyNet){
				xWind->setValue(0);
				yWind->setValue(3);
				zWind->setValue(0);
				xnWind->setValue(5);
				ynWind->setValue(15);
				znWind->setValue(5);
			} else if (item == roundTable){
				xWind->setValue(4);
				yWind->setValue(0);
				zWind->setValue(0);
				xnWind->setValue(8);
				ynWind->setValue(3);
				znWind->setValue(8);
			} else if (item == squareTable){
				xWind->setValue(0);
				yWind->setValue(2);
				zWind->setValue(0);
				xnWind->setValue(8);
				ynWind->setValue(3);
				znWind->setValue(8);
			} else if (item == squareTable2){
				xWind->setValue(0);
				yWind->setValue(2);
				zWind->setValue(0);
				xnWind->setValue(8);
				ynWind->setValue(3);
				znWind->setValue(8);
			}

			needsInit = true;
		}
	} else {
		//D3D9App::onMenuItemPress(menu, item);
	}
}

bool load(){
		#if 0
	if(IRenderer::GetRendererInstance()->GetSHADING_LANGUAGE_VERSION_MAJOR()<2){
		LOG_PRINT("No ps2.0 support");
		return false;
	}

	// Check for VS3.0 support.
	if(IRenderer::GetRendererInstance()->GetSHADING_LANGUAGE_VERSION_MAJOR()<3){
		clothMenu->checkItem(separateStreams, false);
		clothMenu->enableItem(separateStreams, false);
		#if 0
		if (caps.PS20Caps.NumInstructionSlots < 512){
			clothMenu->checkItem(multipass, true);
			clothMenu->enableItem(multipass, false);
		}
		#endif
	}
		#endif

	// Check whether half floats are supported in vertex declarations (R300 doesn't support this).
	bool supportsHalf = false;// IRenderer::GetRendererInstance()->supportsHalf();
	FORMAT d3dFormat = supportsHalf? FORMAT_RGBA16F : FORMAT_RGBA32F;
	AttributeFormat attribFormat = supportsHalf? FORMAT_HALF : FORMAT_FLOAT;

	// Initialization shaders for the presets
	initCloth[0] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/initCloth.hlsl", "main", "main");
	initCloth[1] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/initCloth2.hlsl", "main", "main");
	initCloth[2] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/initCloth3.hlsl", "main", "main");
	initCloth[3] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/initCloth4.hlsl", "main", "main");
	initCloth[4] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/initCloth5.hlsl", "main", "main");
	initCloth[5] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/initCloth6.hlsl", "main", "main");

    // The physics shaders
	//if (caps.PS20Caps.NumInstructionSlots >= 512)
	{
		physics = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/physics1.hlsl", "main", "main");
	}

	physicsMP[0] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/physics2.hlsl", "main", "main");
	physicsMP[1] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/physics3.hlsl", "main", "main");
	physicsMP[2] = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/physics4.hlsl", "main", "main");

    // The rest of the needed shaders
	lighting = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/lighting.hlsl", "main", "main");
	tangentSpace = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/tangentSpace.hlsl", "main", "main");
	interleave = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/interleave.hlsl", "main", "main");
	hit = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/hit.hlsl", "main", "main");
	setNode = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Cloth/setNode.hlsl", "main", "main");

    // Load vertex attributes
	FormatDesc quadAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };
//	if ((quadVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), -1)) == TEXTURE_NONE) return false;

	FormatDesc clothAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT,  4,
		1, TYPE_TANGENT,  attribFormat,  4,
		2, TYPE_BINORMAL, attribFormat,  4,
		3, TYPE_NORMAL,   attribFormat,  4,
		4, TYPE_TEXCOORD, FORMAT_USHORT, 2,
	};
//	if ((clothVF = IRenderer::GetRendererInstance()->addVertexFormat(clothAttribs, elementsOf(clothAttribs), -1)) == TEXTURE_NONE) return false;

	FormatDesc clothILAttribs[] = {
		0, TYPE_VERTEX,   attribFormat,  4,
		0, TYPE_TANGENT,  attribFormat,  4,
		0, TYPE_BINORMAL, attribFormat,  4,
		0, TYPE_NORMAL,   attribFormat,  4,
		1, TYPE_TEXCOORD, FORMAT_USHORT, 2,
	};
	if ((clothInterleavedVF = IRenderer::GetRendererInstance()->addVertexFormat(clothILAttribs, elementsOf(clothILAttribs), interleave)) == TEXTURE_NONE) return false;

	FormatDesc hitAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT,  3
	};
	if ((hitVF = IRenderer::GetRendererInstance()->addVertexFormat(hitAttribs, elementsOf(hitAttribs), hit)) == TEXTURE_NONE) return false;

	if(initClothVF[0] = IRenderer::GetRendererInstance()->addVertexFormat(clothAttribs, elementsOf(clothAttribs), initCloth[0]) == TEXTURE_NONE) return false;
	if(initClothVF[1] = IRenderer::GetRendererInstance()->addVertexFormat(clothAttribs, elementsOf(clothAttribs), initCloth[1]) == TEXTURE_NONE) return false;
	if(initClothVF[2] = IRenderer::GetRendererInstance()->addVertexFormat(clothAttribs, elementsOf(clothAttribs), initCloth[2]) == TEXTURE_NONE) return false;
	if(initClothVF[3] = IRenderer::GetRendererInstance()->addVertexFormat(clothAttribs, elementsOf(clothAttribs), initCloth[3]) == TEXTURE_NONE) return false;
	if(initClothVF[4] = IRenderer::GetRendererInstance()->addVertexFormat(clothAttribs, elementsOf(clothAttribs), initCloth[4]) == TEXTURE_NONE) return false;
	if(initClothVF[5] = IRenderer::GetRendererInstance()->addVertexFormat(clothAttribs, elementsOf(clothAttribs), initCloth[5]) == TEXTURE_NONE) return false;
	if(physicsMPVF[0] = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), physicsMP[0]) == TEXTURE_NONE) return false;
	if(physicsMPVF[1] = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), physicsMP[1]) == TEXTURE_NONE) return false;
	if(physicsMPVF[2] = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), physicsMP[2]) == TEXTURE_NONE) return false;
	if(physicsVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), physics) == TEXTURE_NONE) return false;
	if(tangentSpaceVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), tangentSpace) == TEXTURE_NONE) return false;
	if(setNodeVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), setNode) == TEXTURE_NONE) return false;
	if(interleaveVF = IRenderer::GetRendererInstance()->addVertexFormat(quadAttribs, elementsOf(quadAttribs), interleave) == TEXTURE_NONE) return false;

	// Render targets for tracking the cloth state
	for (int i = 0; i < 2; i++){
		pos[i] = IRenderer::GetRendererInstance()->addRenderTarget(CLOTH_SIZE, CLOTH_SIZE, FORMAT_RG32f,   IRenderer::GetRendererInstance()->GetnearestClamp());
		dir[i] = IRenderer::GetRendererInstance()->addRenderTarget(CLOTH_SIZE, CLOTH_SIZE, FORMAT_RG32f,   IRenderer::GetRendererInstance()->GetnearestClamp());

	}

	// Render targets for the tangent space
	tangent = IRenderer::GetRendererInstance()->addRenderTarget(CLOTH_SIZE, CLOTH_SIZE, FORMAT_RG32f,   IRenderer::GetRendererInstance()->GetnearestClamp());
	binormal = IRenderer::GetRendererInstance()->addRenderTarget(CLOTH_SIZE, CLOTH_SIZE, FORMAT_RG32f,   IRenderer::GetRendererInstance()->GetnearestClamp());
	normal = IRenderer::GetRendererInstance()->addRenderTarget(CLOTH_SIZE, CLOTH_SIZE, FORMAT_RG32f,   IRenderer::GetRendererInstance()->GetnearestClamp());

    // Create a render target for interleaving the streams
	rtInterleaved = IRenderer::GetRendererInstance()->addRenderTarget(CLOTH_SIZE, CLOTH_SIZE, FORMAT_RG32f,   IRenderer::GetRendererInstance()->GetnearestClamp());

	// Render targets used to figure out where on the cloth the user clicked
	rtHit = IRenderer::GetRendererInstance()->addRenderTarget(1, 1, FORMAT_RGBA16f,   IRenderer::GetRendererInstance()->GetnearestClamp());
	rtHitDepth = IRenderer::GetRendererInstance()->addRenderDepthStencil(1, 1, 16, 0);

	// The cloth texture
	base=IRenderer::GetRendererInstance()->addImageLibTexture("/Textures/wallpaper.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	// Load a bumpmap for the cloth. We use UV5L6 format for improved performance.
	Image3 img;
	img.loadImageLibImage("/Textures/wallpaperBump.png");
	img.generateMipMaps();
	// ??? img.toNormalMap(FORMAT_UV5L6, 0.4f);
	bump = IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	// Create an index buffer for the cloth.
	std::vector<INDEX_TYPE> iDest(getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE));
		fillRectangleIB(&iDest[0], CLOTH_SIZE, CLOTH_SIZE);
	clothIB = IRenderer::GetRendererInstance()->addIndexBuffer(getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE), 2, STATIC, &iDest[0]);

	// Create a vertex buffer for the cloth. It only contains the texture coordinates.
	std::vector<D3DXFROMWINEVECTOR2> p(CLOTH_SIZE * CLOTH_SIZE);
	ushort *vDest = (ushort*)&p[0];
		for (int y = 0; y < CLOTH_SIZE; y++){
			for (int x = 0; x < CLOTH_SIZE; x++){
				*vDest++ = (65535 * x) / (CLOTH_SIZE - 1);
				*vDest++ = (65535 * y) / (CLOTH_SIZE - 1);
			}
		}
	clothVB = IRenderer::GetRendererInstance()->addVertexBuffer(CLOTH_SIZE * CLOTH_SIZE * 2, STATIC, &p[0]);
	/*
		Create a dummy vertex buffer.
		For the debug runtime we need to make it large enough to be valid for the draw call.
		The release runtime doesn't have this validation, and since we're really sampling
		from the texture we don't want to waste memory, so we create a minimal vertex buffer
		in this case.
	*/
	//dummy = IRenderer::GetRendererInstance()->addVertexBuffer(debugRuntime? 4 * CLOTH_SIZE * CLOTH_SIZE * (IRenderer::GetRendererInstance()->supportsHalf()? sizeof(half4) : sizeof(D3DXFROMWINEVECTOR4)) : 1);

	currRT  = 0;
	otherRT = 1;
	needsInit = true;

	grabCoord = D3DXFROMWINEVECTOR2(0, 0);
	grabPos = D3DXFROMWINEVECTOR3(0, 0, 0);
	grabbed = false;

	grabCoordSurface.create(FORMAT_RGBA16, 1, 1, 1, 1);
	grabPosSurface.create(/* FORMAT_RGBA32f */ FORMAT_RGBA16f, CLOTH_SIZE, CLOTH_SIZE, 1, 1);

	initPerlin();
	
	return true;
}

bool unload(){
	//grabCoordSurface->Release();
	//grabPosSurface->Release();
	return true;
}

bool onReset(){
	needsInit = true;
	return true;
}

void drawFrame(){
	D3DXFROMWINEMATRIX projection = perspectiveMatrixY(1.3f, width, height, 0.02f, 40, true);

	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	modelview.translate(-camPos);
	D3DXFROMWINEMATRIX mvp = projection * modelview;

	D3DXFROMWINEVECTOR3 lightPos = D3DXFROMWINEVECTOR3(3, 1, -1);

	// Protect against framerate fluctuation.
	static float fTime = 0;
	fTime *= 0.85f;
	fTime += min(1000.0f*timeGetTime(), 0.007f);
	float pfTime = 0.15f * fTime;


	// Compute wind direction
	D3DXFROMWINEVECTOR3 windDir;
#if 0
	windDir.x = xWind->getValue();
	windDir.y = yWind->getValue();
	windDir.z = zWind->getValue();
	windDir.x += xnWind->getValue() * noise1(-0.5f * 1000.0f*timeGetTime());
	windDir.y += ynWind->getValue() * noise1(0.5f * 1000.0f*timeGetTime() + 19.291f);
	windDir.z += znWind->getValue() * noise1(0.5f * 1000.0f*timeGetTime());
#else
	windDir.x = 4;
	windDir.y = 3;
	windDir.z = -2;
	windDir.x += 5 * noise1(-0.5f * 1000.0f*timeGetTime());
	windDir.y += 5 * noise1(0.5f * 1000.0f*timeGetTime() + 19.291f);
	windDir.z += 5 * noise1(0.5f * 1000.0f*timeGetTime());
#endif

	const float vertices[] = { -1, -1,   1, -1,   1, 1,   -1, 1 };
	if (STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_r)){
		needsInit = true;
	}


	TextureID rtPN[] = { pos[currRT], dir[currRT] };
	IRenderer::GetRendererInstance()->changeRenderTargets(rtPN, elementsOf(rtPN));

		if (clothMenu && needsInit){
			// Initialize the cloth
			float scale = CLOTH_SIZE / (CLOTH_SIZE - 1.0f);
			float bias = 1.0f / (CLOTH_SIZE - 1.0f);

			IRenderer::GetRendererInstance()->setShader(initCloth[clothMenu->getCheckedRadioButton(1)]);
			IRenderer::GetRendererInstance()->setVertexFormat(initClothVF[clothMenu->getCheckedRadioButton(1)]);
			IRenderer::GetRendererInstance()->setShaderConstant2f("scale", D3DXFROMWINEVECTOR2(scale, -scale));
			IRenderer::GetRendererInstance()->setShaderConstant1f("bias", bias);
			IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

			// Also reset the normals to a reasonable starting point
			// as they are used in the simulation.
			IRenderer::GetRendererInstance()->changeRenderTarget(normal);
			IRenderer::GetRendererInstance()->Clear(true, true, stx_GetRGBA(0x0000FF00));

			needsInit = false;
		} else {
			/*
				The physics pass
			*/
			// Each node have springs connecting to neighboring nodes in the following pattern
			float nodeDistance = 2.0f / (CLOTH_SIZE - 1);
			float sq2 = sqrtf(2);

			D3DXFROMWINEVECTOR3 springs[] = {
				D3DXFROMWINEVECTOR3( 1.0f / CLOTH_SIZE,  0.0f,              nodeDistance),
				D3DXFROMWINEVECTOR3( 0.0f,               1.0f / CLOTH_SIZE, nodeDistance),
				D3DXFROMWINEVECTOR3(-1.0f / CLOTH_SIZE,  0.0f,              nodeDistance),
				D3DXFROMWINEVECTOR3( 0.0f,              -1.0f / CLOTH_SIZE, nodeDistance),

				D3DXFROMWINEVECTOR3( 1.0f / CLOTH_SIZE,  1.0f / CLOTH_SIZE, nodeDistance * sq2),
				D3DXFROMWINEVECTOR3(-1.0f / CLOTH_SIZE,  1.0f / CLOTH_SIZE, nodeDistance * sq2),
				D3DXFROMWINEVECTOR3(-1.0f / CLOTH_SIZE, -1.0f / CLOTH_SIZE, nodeDistance * sq2),
				D3DXFROMWINEVECTOR3( 1.0f / CLOTH_SIZE, -1.0f / CLOTH_SIZE, nodeDistance * sq2),

				D3DXFROMWINEVECTOR3( 2.0f / CLOTH_SIZE,  2.0f,              nodeDistance * 2 * sq2),
				D3DXFROMWINEVECTOR3(-2.0f,               2.0f / CLOTH_SIZE, nodeDistance * 2 * sq2),
				D3DXFROMWINEVECTOR3(-2.0f / CLOTH_SIZE, -2.0f,              nodeDistance * 2 * sq2),
				D3DXFROMWINEVECTOR3( 2.0f,              -2.0f / CLOTH_SIZE, nodeDistance * 2 * sq2),

				D3DXFROMWINEVECTOR3( 3.0f / CLOTH_SIZE,  0.0f / CLOTH_SIZE, nodeDistance * 3),
				D3DXFROMWINEVECTOR3( 0.0f / CLOTH_SIZE,  3.0f / CLOTH_SIZE, nodeDistance * 3),
				D3DXFROMWINEVECTOR3(-3.0f / CLOTH_SIZE,  0.0f / CLOTH_SIZE, nodeDistance * 3),
				D3DXFROMWINEVECTOR3( 0.0f / CLOTH_SIZE, -3.0f / CLOTH_SIZE, nodeDistance * 3),

				D3DXFROMWINEVECTOR3( 4.0f / CLOTH_SIZE,  4.0f,              nodeDistance * 4 * sq2),
				D3DXFROMWINEVECTOR3(-4.0f,               4.0f / CLOTH_SIZE, nodeDistance * 4 * sq2),
				D3DXFROMWINEVECTOR3(-4.0f / CLOTH_SIZE, -4.0f,              nodeDistance * 4 * sq2),
				D3DXFROMWINEVECTOR3( 4.0f,              -4.0f / CLOTH_SIZE, nodeDistance * 4 * sq2),
			};

			float damp;
			if (STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_END)){
				// To manually dampen the cloth simulation to near halt.
				damp = powf(1e-20f, pfTime);
			} else {
				// Compute a dampen factor to avoid energy build up in the system.
				damp = powf(0.005f, pfTime);
			}
			// Wind direction with 1000.0f*timeGetTime() pre-multiplied into the vector to simplify the math in the shader
			D3DXFROMWINEVECTOR3 ftWindDir = sqrtf(pfTime) * windDir;
			// Frame-time pre-multiplied with a set of numbers to simplify the shader
			D3DXFROMWINEVECTOR4 frameTimePM = pfTime * D3DXFROMWINEVECTOR4(1, 5, 10, 3500);


			if (clothMenu && clothMenu->isItemChecked(multipass)){
				/*
					Multipass physics (for ps2_0 cards)
				*/

				// First pass
					IRenderer::GetRendererInstance()->setShader(physicsMP[0]);
					IRenderer::GetRendererInstance()->setVertexFormat(physicsMPVF[0]);
					IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					IRenderer::GetRendererInstance()->setShaderConstant4f("1000.0f*timeGetTime()", frameTimePM);
					IRenderer::GetRendererInstance()->setShaderConstant1f("damp", damp);
					IRenderer::GetRendererInstance()->setShaderConstantArray3f("springs", springs, 6);
					IRenderer::GetRendererInstance()->setTexture("Pos", pos[otherRT]);
					IRenderer::GetRendererInstance()->setTexture("Dir", dir[otherRT]);
					
					IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
					

					IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

				// Second pass
				IRenderer::GetRendererInstance()->changeRenderTarget(dir[otherRT]);

					IRenderer::GetRendererInstance()->setShader(physicsMP[1]);
					IRenderer::GetRendererInstance()->setVertexFormat(physicsMPVF[1]);
					IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					IRenderer::GetRendererInstance()->setShaderConstant4f("1000.0f*timeGetTime()", frameTimePM);
					IRenderer::GetRendererInstance()->setShaderConstantArray3f("springs", springs + 6, 5);
					IRenderer::GetRendererInstance()->setTexture("Pos", pos[currRT]);
					IRenderer::GetRendererInstance()->setTexture("Dir", dir[currRT]);
					
					IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
					

					IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

				// Third pass
				IRenderer::GetRendererInstance()->changeRenderTarget(dir[currRT]);

					IRenderer::GetRendererInstance()->setShader(physicsMP[1]);
					IRenderer::GetRendererInstance()->setVertexFormat(physicsMPVF[1]);
					IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					IRenderer::GetRendererInstance()->setShaderConstant4f("1000.0f*timeGetTime()", frameTimePM);
					IRenderer::GetRendererInstance()->setShaderConstantArray3f("springs", springs + 11, 5);
					IRenderer::GetRendererInstance()->setTexture("Pos", pos[otherRT]);
					IRenderer::GetRendererInstance()->setTexture("Dir", dir[otherRT]);
					
					IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
					

					IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

				// Fourth and final pass
				TextureID rtOtherPN[] = { pos[otherRT], dir[otherRT] };
				IRenderer::GetRendererInstance()->changeRenderTargets(rtOtherPN, elementsOf(rtOtherPN));

					IRenderer::GetRendererInstance()->setShader(physicsMP[2]);
					IRenderer::GetRendererInstance()->setVertexFormat(physicsMPVF[2]);
					IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
					IRenderer::GetRendererInstance()->setShaderConstant4f("1000.0f*timeGetTime()", frameTimePM);
					IRenderer::GetRendererInstance()->setShaderConstant3f("windDir", ftWindDir);
					IRenderer::GetRendererInstance()->setShaderConstantArray3f("springs", springs + 16, 4);
					IRenderer::GetRendererInstance()->setTexture("Pos", pos[currRT]);
					IRenderer::GetRendererInstance()->setTexture("Dir", dir[currRT]);
					IRenderer::GetRendererInstance()->setTexture("Normal", normal);
					IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
					

					IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

				currRT = otherRT;
				otherRT = 1 - currRT;

			} else {
				/*
					Single pass physics (for ps_2_b and up)
				*/
				IRenderer::GetRendererInstance()->setShader(physics);
				IRenderer::GetRendererInstance()->setVertexFormat(physicsVF);
				IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
				IRenderer::GetRendererInstance()->setShaderConstant4f("1000.0f*timeGetTime()", frameTimePM);
				IRenderer::GetRendererInstance()->setShaderConstant1f("damp", damp);
				IRenderer::GetRendererInstance()->setShaderConstant3f("windDir", ftWindDir);
				IRenderer::GetRendererInstance()->setShaderConstantArray3f("springs", springs, elementsOf(springs));
				IRenderer::GetRendererInstance()->setTexture("Pos", pos[otherRT]);
				IRenderer::GetRendererInstance()->setTexture("Dir", dir[otherRT]);
				IRenderer::GetRendererInstance()->setTexture("Normal", normal);
				IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
				

				IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));
			}

			if (grabbed){
				// Position in clip space. We don't know z, so we set it arbitrarily to 0.
				D3DXFROMWINEVECTOR4 cPos(2 * float(STX_Service::GetInputInstance()->GetMouseX()) / float(width) - 1, 1 - 2 * float(STX_Service::GetInputInstance()->GetMouseY()) / float(height), 0, 1);
				// Reverse transform to unproject it back to world space.
				// This gives a point that would map to the clicked pixel.
				D3DXFROMWINEVECTOR4 unProj = !mvp * cPos;

				// Compute direction from camera to this position
				D3DXFROMWINEVECTOR3 dir = normalize((unProj.xyz() / unProj.w) - camPos);

				// Intersect with the plane
				float d = dot(camPos, grabNormal) + grabOffset;
				D3DXFROMWINEVECTOR3 pos = camPos - (d / dot(grabNormal, dir)) * dir;

				IRenderer::GetRendererInstance()->setShader(setNode);
				IRenderer::GetRendererInstance()->setVertexFormat(setNodeVF);
				IRenderer::GetRendererInstance()->setShaderConstant3f("pos", pos);
				IRenderer::GetRendererInstance()->setShaderConstant1f("free", grabFree);
				IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
				

				float ps = 5;
				//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_POINTSIZE, *(__DWORD__ *) &ps);

				IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_POINTS, 1, grabCoord, grabCoord, sizeof(D3DXFROMWINEVECTOR2));

			}

		}

	TextureID rtTS[] = { tangent, binormal, normal };
	IRenderer::GetRendererInstance()->changeRenderTargets(rtTS, elementsOf(rtTS));

		/*
			Compute the tangent space
		*/
		IRenderer::GetRendererInstance()->setShader(tangentSpace);
		IRenderer::GetRendererInstance()->setVertexFormat(tangentSpaceVF);
		IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / CLOTH_SIZE, 0.5f / CLOTH_SIZE));
		IRenderer::GetRendererInstance()->setShaderConstant1f("pixel", 1.0f / CLOTH_SIZE);
		IRenderer::GetRendererInstance()->setTexture("Pos", pos[currRT]);
		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
		

		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));

		/*
			Produce an interleaved vertex buffer.
			This is neccesary for cards that don't support VS3.0 since they can't
			use the vertex samplers and thus can only use one R2VB vertex stream.
		*/
		if (clothMenu && (!clothMenu->isItemChecked(separateStreams))){

			IRenderer::GetRendererInstance()->changeRenderTarget(rtInterleaved);

			IRenderer::GetRendererInstance()->setShader(interleave);
			IRenderer::GetRendererInstance()->setVertexFormat(interleaveVF);
			IRenderer::GetRendererInstance()->setTexture("Position", pos[currRT]);
			IRenderer::GetRendererInstance()->setTexture("Tangent",  tangent);
			IRenderer::GetRendererInstance()->setTexture("Binormal", binormal);
			IRenderer::GetRendererInstance()->setTexture("Normal",   normal);
			IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / (4 * CLOTH_SIZE), 0.5f / CLOTH_SIZE));
			IRenderer::GetRendererInstance()->setShaderConstant1f("width", CLOTH_SIZE);
			IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);
			

			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));
		}

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	IRenderer::GetRendererInstance()->Clear(true, true, stx_GetRGBA(0xFF224466));
	/*
		The lighting pass
	*/
	if (clothMenu && clothMenu->isItemChecked(separateStreams)){
		IRenderer::GetRendererInstance()->setShader(lighting);
		IRenderer::GetRendererInstance()->setVertexFormat(clothVF);
		IRenderer::GetRendererInstance()->setTexture("Base", base);
		IRenderer::GetRendererInstance()->setTexture("Bump", bump);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
		IRenderer::GetRendererInstance()->setShaderConstant3f("lightPos", lightPos);
		IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", camPos);
		//IRenderer::GetRendererInstance()->setVertexBuffer(0, dummy);
		//IRenderer::GetRendererInstance()->setVertexBuffer(1, dummy);
		//IRenderer::GetRendererInstance()->setVertexBuffer(2, dummy);
		//IRenderer::GetRendererInstance()->setVertexBuffer(3, dummy);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, clothVB);
		IRenderer::GetRendererInstance()->setIndexBuffer(clothIB);
		

		// Bind the R2VB render targets to the vertex texture samplers
		IRenderer::GetRendererInstance()->setTexture("DMAP0", (pos[currRT]));
		IRenderer::GetRendererInstance()->setTexture("DMAP1", (tangent));
		IRenderer::GetRendererInstance()->setTexture("DMAP2", (binormal));
		IRenderer::GetRendererInstance()->setTexture("DMAP3", (normal));

		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLE_STRIP, 0, 0, CLOTH_SIZE * CLOTH_SIZE, 0, getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE) - 2);
	} else {
		IRenderer::GetRendererInstance()->setShader(lighting);
		IRenderer::GetRendererInstance()->setVertexFormat(clothInterleavedVF);
		IRenderer::GetRendererInstance()->setTexture("Base", base);
		IRenderer::GetRendererInstance()->setTexture("Bump", bump);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
		IRenderer::GetRendererInstance()->setShaderConstant3f("lightPos", lightPos);
		IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", camPos);
		//IRenderer::GetRendererInstance()->setVertexBuffer(0, dummy);
		IRenderer::GetRendererInstance()->setVertexBuffer(0, clothVB);
		IRenderer::GetRendererInstance()->setIndexBuffer(clothIB);

		// Bind the interleaved R2VB render target to the DMAP sampler
		IRenderer::GetRendererInstance()->setTexture("DMAP", rtInterleaved);

		IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLE_STRIP, 0, 0, CLOTH_SIZE * CLOTH_SIZE, 0, getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE) - 2);
		
	}

	/*
		Handle mouse clicks
	*/
	static bool lastMouse = false;
#if 0
	if (STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_MIDDLE) || STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT)){
		if (!lastMouse){
			// First render to a 1x1 texture.
			// We scale projection so that the clicked pixel maps right into this render target.
			IRenderer::GetRendererInstance()->changeRenderTarget(rtHit, rtHitDepth);

				IRenderer::GetRendererInstance()->Clear(true, true, 0xFF000000, 1.0f, 0);

				float x = (float(STX_Service::GetInputInstance()->GetMouseX()) / width ) * 2 - 1;
				float y = 1 - (float(STX_Service::GetInputInstance()->GetMouseY()) / height) * 2;

				D3DXFROMWINEMATRIX sMvp = scale4(width / 2.0f, height / 2.0f, 1) * translate(-x, -y, 0) * mvp;

				IRenderer::GetRendererInstance()->setShader(hit);
				IRenderer::GetRendererInstance()->setVertexFormat(hitVF);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", sMvp);
				//IRenderer::GetRendererInstance()->setVertexBuffer(0, dummy);
				IRenderer::GetRendererInstance()->setVertexBuffer(0, clothVB);
				IRenderer::GetRendererInstance()->setIndexBuffer(clothIB);
				


				IRenderer::GetRendererInstance()->setTexture("DMAP", pos[currRT]));

				IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLE_STRIP, 0, 0, CLOTH_SIZE * CLOTH_SIZE, 0, getRectangleIndexCount(CLOTH_SIZE, CLOTH_SIZE) - 2);

			IRenderer::GetRendererInstance()->changeToMainFramebuffer();

			// Read back the pixel
int pitch=0;
ushort *src = (ushort *)IRenderer::GetRendererInstance()->LockTexture(rtHit 0, pitch);
{

					// If src[3] != 0, we're clicking somewhere outside the cloth
					if (src[3] == 0){
						// Compute the texture coordinates for the point we clicked
						grabCoord.x = (src[0] / 65535.0f) * 2 - 1;
						grabCoord.y = 1 - (src[1] / 65535.0f) * 2;

						// Read back the position of the closest node
						int pitch;
						BYTE* src=IRenderer::GetRendererInstance()->LockTexture(pos[currRT], 0, pitch);
						{
							int s = CLOTH_SIZE * src[0] / 65536;
							int t = CLOTH_SIZE * src[1] / 65536;
							{
                                // Get the position
								grabPos = *(D3DXFROMWINEVECTOR3 *) (((ubyte *) src) + t * pitch + s * sizeof(D3DXFROMWINEVECTOR4));

								// Compute a plane parallel with the view plane intersection the node.
								// This is the plane we'll move the point in when we drag the mouse.
								grabNormal = modelview.getForwardVec();
								grabOffset = -dot(grabPos, grabNormal);

								grabFree = STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT)? 1.0f : 0.0f;
								grabbed = true;

								IRenderer::GetRendererInstance()->UnlockTexture(pos[currRT], 0);
							}
						}
					}
					IRenderer::GetRendererInstance()->UnlockTexture(rtHit, 0);
				}
			}
		}
	} else {
		grabbed = false;
	}
#endif
	lastMouse = (STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_MIDDLE) || STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT));

	currRT = otherRT;
	otherRT = 1 - currRT;
}

private:
	TextureID base, bump;
	ShaderID initCloth[6], physics, physicsMP[3], lighting, tangentSpace, hit, setNode, interleave;
	VertexFormatID initClothVF[6], physicsVF, physicsMPVF[3], tangentSpaceVF, setNodeVF, interleaveVF;

	VertexFormatID clothVF, clothInterleavedVF, hitVF; // quadVF
	VertexBufferID clothVB;
	IndexBufferID clothIB;

	//VertexBufferID dummy;

	Image3 grabCoordSurface;
	Image3 grabPosSurface;

	D3DXFROMWINEVECTOR2 grabCoord;
	D3DXFROMWINEVECTOR3 grabPos;
	float grabFree;
	D3DXFROMWINEVECTOR3 grabNormal;
	float grabOffset;
	bool grabbed;

	TextureID pos[2], dir[2], tangent, binormal, normal, rtInterleaved, rtHit, rtHitDepth;
	int currRT, otherRT;
	bool needsInit;
	#if 1
	Menu *clothMenu;
	MenuItemID curtain, sail, safetyNet, roundTable, squareTable, squareTable2, multipass, separateStreams;
	Slider *xWind, *yWind, *zWind;
	Label *xLabel, *yLabel, *zLabel;
	Slider *xnWind, *ynWind, *znWind;
	Label *xnLabel, *ynLabel, *znLabel;
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
	app.drawFrame();
	IRenderer::GetRendererInstance()->Present();
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-Cloth");
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

