/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>
#include <Framework/Util/Model.h>
#include <Framework/Util/BSP.h>

//-----------------------------------------------------------------------------
// File: App.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

struct Vertex_ {
	D3DXFROMWINEVECTOR3 pos;
	D3DXFROMWINEVECTOR2 texCoord0;
	D3DXFROMWINEVECTOR2 texCoord1;
};
class App {
public:
	App(){}
	virtual ~App(){}
public:
	char *getTitleString(){ return "HDR"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/HDR"; }

private:
	TextureID	lpBlurTarget;
	SurfaceID		lpFSAARenderTarget, lpFSAADepthBuffer;
	TextureID				base[4], lightMap, environment;
	TextureID				renderTarget, renderDepth, blurTarget, blurTarget2, blurTarget3, averageTarget, exposureTarget[2];
	ShaderID				lighting, skybox, hdrCPU, hdrGPU, blur, convert, average, exposure;
	VertexFormatID				lightingVF, hdrCPUVF, hdrGPUVF, blurVF, convertVF, averageVF, exposureVF;
	VertexBufferID			m_fullscreenQuad;
	VertexFormatID			envVF;//, m_quadVertexFormat;
	int						nFSAASamples;

	Image3					envImage, lmImage;
	float					maxSkyValue, maxLMValue;

	#if 1
	Model					*house;
	BSP						bsp;

	Menu					*hdrMenu;
	MenuItemID				useCPU, useGPU;

	Menu					*m_FSAAMenu;
	MenuItemID				m_NoFSAAMenuItem, m_FSAAMenuItem[16];
  	Menu* mainMenu;
	#endif
    float width;
    float height;
    D3DXFROMWINEVECTOR3 camPos;
    float wx;
    float wy;
    float speed, frameTime;
    bool debugRuntime;
	D3DXFROMWINEMATRIX g_Proj, g_View;
	bool fixedSize;
  	int antiAliasSamples;

void setup(){
	// Default width and height
	width  = 800;
	height = 600;
	fixedSize = true;

	// No antialiasing on back buffer
	antiAliasSamples = 0;
}

void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(0, -300, -200);
	wx = 0;
	wy = 0;
}

void moveCamera(const D3DXFROMWINEVECTOR3 &dir){
	D3DXFROMWINEVECTOR3 newPos = camPos + dir * (speed * frameTime);

	D3DXFROMWINEVECTOR3 point;
	const BTri *tri;
	if (bsp.intersects(camPos, newPos, &point, &tri)){
		newPos = point + tri->normal;
	}
	bsp.pushSphere(newPos, 70);

	camPos = newPos;
}

float getLuminance(const D3DXFROMWINEVECTOR3 &dir){
	D3DXFROMWINEVECTOR3 point;
	const BTri *tri;
	if (bsp.intersects(camPos, camPos + 10000 * dir, &point, &tri)){
		// The ray hit the house, so we'll look up luminance in the lightmap

		uint index = (uint) intptr(tri->data);

		Vertex_ *vertices = (Vertex_ *) house->getVertexArray();
		Vertex_ *v0 = vertices + index;
		Vertex_ *v1 = v0 + 1;
		Vertex_ *v2 = v0 + 2;

		// Find barycentric coordinates
		D3DXFROMWINEMATRIX vm(v0->pos.x, v1->pos.x, v2->pos.x, 0.0f,
				v0->pos.y, v1->pos.y, v2->pos.y, 0.0f,
				v0->pos.z, v1->pos.z, v2->pos.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		D3DXFROMWINEMATRIX vmInv;
		D3DXFROMWINEMatrixInverse(&vmInv, 0, &vm);
		D3DXFROMWINEVECTOR3 bary;
		D3DXFROMWINEVec3TransformCoord(&bary, &point, &vmInv);

		// Compute lightmap coordinate
		D3DXFROMWINEVECTOR2 lmCoord = saturate(v0->texCoord1 * bary.x + v1->texCoord1 * bary.y + v2->texCoord1 * bary.z);

		// Look up the texel
		ubyte *src = lmImage.getPixels();
		int w = lmImage.getWidth();
		int h = lmImage.getHeight();

		src += (int(h * lmCoord.y) * w + int(w * lmCoord.x)) * 4;

		// Expand RGBS
		float lum = (src[0] + src[1] + src[2]) * src[3] * (1.0f / (255.0f * 255.0f));

		return lum * maxLMValue;
	} else {
		// The ray didn't hit anything, so we'll look up the luminance in the environment map
		float s, t;
		ubyte *src = envImage.getPixels();

		int w = envImage.getWidth();
		int size = getImageSize(FORMAT_RGBA16, w, w, 1, 1);

		// Find what cubemap face we hit and compute the coordinates
		if (fabsf(dir.x) > max(fabsf(dir.y), fabsf(dir.z))){
			s = -dir.z / dir.x;
			t = -dir.y / fabsf(dir.x);
			if (dir.x < 0){
				src += size;
			}
		} else if (fabsf(dir.y) > max(fabsf(dir.x), fabsf(dir.z))){
			s = dir.x / fabsf(dir.y);
			t = dir.z / dir.y;
			if (dir.y > 0){
				src += 2 * size;
			} else {
				src += 3 * size;
			}
			
		} else {
			s =  dir.x / dir.z;
			t = -dir.y / fabsf(dir.z);
			if (dir.z > 0){
				src += 4 * size;
			} else {
				src += 5 * size;
			}
		}
		s = 0.5f * s + 0.5f;
		t = 0.5f * t + 0.5f;
		src += (int(w * t) * w + int(w * s)) * 4 * sizeof(ushort);

		// Expand RGBS
		ushort *srcW = (ushort *) src;
		float lum = (srcW[0] + srcW[1] + srcW[2]) * srcW[3] * (1.0f / (65535.0f * 65535.0f));

		return lum * maxSkyValue;
	}
}

void initGUI()
{
	hdrMenu = 0;// ??? mainMenu->addMenu("Exposure");
	useCPU = hdrMenu->addItem("Compute on CPU", MENU_CHECKABLE);
	useGPU = hdrMenu->addItem("Compute on GPU", MENU_CHECKABLE | MENU_CHECKED);

	hdrMenu->setRadioButtonGroup(useCPU, useGPU, 1);


	char	pszFSAAString[256];

	// Add FSAA menu
	m_FSAAMenu = 0;// ??? mainMenu->addMenu( "FSAA" );

	// Add No FSAA menu item
	m_NoFSAAMenuItem = m_FSAAMenu->addItem( "No FSAA", MENU_CHECKABLE | MENU_CHECKED);

	// Add all other possible AA modes
	for (int i=2; i<16; i++) {
		sprintf(pszFSAAString, "FSAA %d samples", i);
		m_FSAAMenuItem[i] = m_FSAAMenu->addItem(pszFSAAString, MENU_CHECKABLE );
	}

	// Set radio button group for all FSAA options
	m_FSAAMenu->setRadioButtonGroup(m_NoFSAAMenuItem, m_FSAAMenuItem[15], 1);

	//D3D9initGUI();
}

void onMenuItemPress( Menu *menu, MenuItemID item )
{
	if (menu==m_FSAAMenu) {
		if (item==m_NoFSAAMenuItem && nFSAASamples!=0) {
			// No FSAA has been selected, reset device
			//D3D9resetDevice();
		}
		else {
			for (int i=2; i<16; i++) {
				if (item==m_FSAAMenuItem[i] && nFSAASamples!=i) {
					// An FSAA mode has been selected, reset device
					//D3D9resetDevice();
					break;
				}
			}
		}
	}

	//BaseonMenuItemPress( menu, item );
}

bool load(){

	// Load shaders
	skybox   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/skybox.hlsl"  );
	lighting = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/lighting.hlsl");
	blur     = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/blur.hlsl"    );
	convert  = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/convert.hlsl" );
	average  = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/average.hlsl" );
	exposure = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/exposure.hlsl");
	hdrCPU   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/HDRCPU.hlsl"     );
	hdrGPU   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/HDR/HDR/HDRGPU.hlsl"     );

	// Load vertex declarations
	FormatDesc envAttribs[] = 
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3
	};
	envVF = IRenderer::GetRendererInstance()->addVertexFormat(envAttribs, elementsOf(envAttribs), -1);

	FormatDesc quadDecl[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	//m_quadVertexFormat = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), -1);
	lightingVF = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), lighting);
	hdrCPUVF = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), hdrCPU);
	hdrGPUVF = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), hdrGPU);
	blurVF = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), blur);
	convertVF = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), convert);
	averageVF = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), average);
	exposureVF = IRenderer::GetRendererInstance()->addVertexFormat(quadDecl, elementsOf(quadDecl), exposure);
#if 0
	// Disable menu items pointing to FSAA modes not supported
	for (int i=2; i<16; i++) {
		if (0) {
			// FSAA mode is not supported
			m_FSAAMenu->enableItem(m_FSAAMenuItem[i], false);
		}
		else {
			// FSAA mode is supported
			m_FSAAMenu->enableItem(m_FSAAMenuItem[i], true);
		}
	}

	// Enable 4 samples FSAA by default if supported
	if (m_FSAAMenu->isItemEnabled(m_FSAAMenuItem[4])) {
		m_FSAAMenu->toggleItemCheck(m_FSAAMenuItem[4]);
	}
	else {
		// Set no FSAA by default
		m_FSAAMenu->toggleItemCheck(m_NoFSAAMenuItem);
	}
#endif
	// Call onReset to create application-managed surfaces
	if (!onReset()) return false;

	// Floating point RTs
	renderTarget = IRenderer::GetRendererInstance()->addRenderTarget(800, 600, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetnearestClamp());
	renderDepth  = IRenderer::GetRendererInstance()->addRenderDepthStencil(800, 600, 16, 0);

	// Fixed point RTs
	blurTarget  = IRenderer::GetRendererInstance()->addRenderTarget(400, 300, FORMAT_RGBA16, IRenderer::GetRendererInstance()->GetlinearClamp());
	blurTarget2 = IRenderer::GetRendererInstance()->addRenderTarget(200, 150, FORMAT_RGBA16, IRenderer::GetRendererInstance()->GetlinearClamp());
	blurTarget3 = IRenderer::GetRendererInstance()->addRenderTarget(200, 150, FORMAT_RGBA16, IRenderer::GetRendererInstance()->GetlinearClamp());

	// Render targets for GPU based exposure level computation
	averageTarget = IRenderer::GetRendererInstance()->addRenderTarget(64, 64, FORMAT_RGBA16, IRenderer::GetRendererInstance()->GetnearestClamp());
	exposureTarget[0] = IRenderer::GetRendererInstance()->addRenderTarget(1, 1, FORMAT_R16F, IRenderer::GetRendererInstance()->GetnearestClamp());
	exposureTarget[1] = IRenderer::GetRendererInstance()->addRenderTarget(1, 1, FORMAT_R16F, IRenderer::GetRendererInstance()->GetnearestClamp());

	// Load textures
	if ((base[0] = IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/brick.dds",      FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((base[1] = IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/parqfloor2.dds", FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((base[2] = IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/bump_normal/brick2.dds",     FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;
	if ((base[3] = IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/bump_normal/Wood.dds",       FILTER_TRILINEAR_ANISO)) == TEXTURE_NONE) return false;

	/*
		Floating point filter is expensive and not widely supported, but FP textures aren't always really needed
		for HDR data. Here we convert FP RGB textures into RGBA16. Alpha is used to store range, 0 to maxValue.
		In the shader this is expanded by multiplying rgb with alpha and maxValue.
	*/
	if (!envImage.loadHDR(TEX_PATH "rnl_cross.hdr")) return false;
	envImage.crossToCube();

	 // Allowing a range up to 1000. Should be more than enough (the texture actually has a max value of 166).
	maxSkyValue = 1000;
	// ??? envImage.toRGBS16(maxSkyValue, 16, 16);

	environment = IRenderer::GetRendererInstance()->addTexture(&envImage, false, IRenderer::GetRendererInstance()->Getlinear());

	if (!lmImage.loadHDR(MODEL_PATH "House/LightMap.hdr")) return false;
	maxLMValue = 1000;
	#if 0 // ???
	lmImage.toRGBS16(maxLMValue, 8, 8);
	lmImage.toByte();
	#endif

	lightMap = IRenderer::GetRendererInstance()->addTexture(&lmImage, false, IRenderer::GetRendererInstance()->Getlinear());

	house = new Model();

	if (!house->load(MODEL_PATH "House/House.model")) return false;
	house->addFormat(TYPE_VERTEX,   3);
	house->addFormat(TYPE_TEXCOORD, 2); // Base texture coordinate
	house->addFormat(TYPE_TEXCOORD, 2); // Lightmap coordinate

	Vertex_ *vertices = (Vertex_ *) house->getVertexArray();
	uint nVertices = house->getVertexCount();

	for (uint i = 0; i < nVertices; i += 3){
		bsp.addTriangle(vertices[i].pos, vertices[i+1].pos, vertices[i+2].pos, (void *) intptr(i));
	}
	bsp.build();

	if (!house->makeDrawable()) return false;



	/*
		Create a fullscreen "quad". It's actually a triangle, but covers the entire
		screen / render target. We're using a triangle instead of a quad to avoid
		having any polygon edges within the quad. This gives us a very slight but
		measurable performance increase.
	*/
	const float quadVertices[] = {
		 0.0f,  2.0f, 0.0f, 1.0f,  0.5f, -0.5f,  // x y z w tu tv
		 3.0f, -1.0f, 0.0f, 1.0f,  2.0f,  1.0f,
		-3.0f, -1.0f, 0.0f, 1.0f, -1.0f,  1.0f,
	};

	m_fullscreenQuad = IRenderer::GetRendererInstance()->addVertexBuffer( 18*sizeof( float ), STATIC, (void*)&quadVertices[0] );

	// Exposure initially zero
	IRenderer::GetRendererInstance()->changeRenderTarget(exposureTarget[0]);
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

	return true;
}


bool onReset(){
	// Is FSAA requested?
	nFSAASamples = 0;
#if 0
	for (int i=2; i<16; i++) {
		if (m_FSAAMenu->isItemChecked(m_FSAAMenuItem[i])) {
			// Yes, create render target with required samples
			IRenderer::GetRendererInstance()->CreateRenderTarget(800, 600, FORMAT_RGBA16f);
			IRenderer::GetRendererInstance()->CreateDepthStencilSurface(800, 600, FORMAT_D16);

			nFSAASamples = i;
			break;
		}
	}
#endif
	return true;
}

bool onPreReset(){
	// Release FSAA render target and depth buffer
	if (nFSAASamples!=0) {
		//lpFSAADepthBuffer->Release();
		//lpFSAARenderTarget->Release();
	}

	return true;
}

bool unload(){
	onPreReset();

	delete house;
	return true;
}


void drawEnvironment(const D3DXFROMWINEMATRIX &mvp){
	// Set render states
	IRenderer::GetRendererInstance()->setShader(skybox);
	IRenderer::GetRendererInstance()->setVertexFormat(envVF);
	IRenderer::GetRendererInstance()->setShaderConstant1f("maxValue", maxSkyValue);
	IRenderer::GetRendererInstance()->setTexture("Skybox", environment);
	

	/*
		Render the skybox in clip-space. We use a big triangle spanning outside
		the screen to avoid polygon edges within the screen for a tiny performance
		boost. Z is set to constant 1 to put it behind everything.

		The texture coordinates for the cubemap is computed by projecting the
		clip-space position back into world space with the inverse MVP matrix.
	*/
	D3DXFROMWINEMATRIX iMvp;
	D3DXFROMWINEMatrixInverse(&iMvp, 0, &mvp);

	D3DXFROMWINEVECTOR3 v0( 0,  2, 1);
	D3DXFROMWINEVECTOR3 v1( 3, -1, 1);
	D3DXFROMWINEVECTOR3 v2(-3, -1, 1);
	D3DXFROMWINEVECTOR3 v0_, v1_, v2_;

	struct Vertex_ {
		D3DXFROMWINEVECTOR3 clipPos;
		D3DXFROMWINEVECTOR3 cubeCoord;
	};
	D3DXFROMWINEVec3TransformCoord(&v0_, &v0, &iMvp);
	D3DXFROMWINEVec3TransformCoord(&v1_, &v1, &iMvp);
	D3DXFROMWINEVec3TransformCoord(&v2_, &v2, &iMvp);
	Vertex_ vertices[3] = {
		v0, v0_,
		v1, v1_,
		v2, v2_,
	};

	// Render geometry
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, 1, vertices, vertices, sizeof(Vertex_));
}


void drawFrame(){
	int hres;

	// Set render target: render to screen-size texture and draw scene as normal (environment + house)
	if(0)// (m_FSAAMenu->isItemChecked(m_NoFSAAMenuItem))
	{
		// No FSAA selected, set default non-AAed render targets
		IRenderer::GetRendererInstance()->changeRenderTarget(renderTarget, renderDepth);
	}
	else { 
		// FSAA selected, set offscreen FSAA render target and depth surface
		IRenderer::GetRendererInstance()->changeRenderTarget(0, lpFSAARenderTarget);
		//IRenderer::GetRendererInstance()->SetDepthStencilSurface(lpFSAADepthBuffer);
	}

		// Clear Z buffer
		float f=128.0f/256.0f;
		IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));

		// Calculate matrices
		D3DXFROMWINEMATRIX projection =	perspectiveMatrixX(1.5f, width, height, 1, 7000, true);
		g_Proj=projection;
		D3DXFROMWINEMATRIX modelview =	rotateXY4(-wx, -wy);
		g_View=modelview;

		/* Calculate MVP matrix for environment */
		D3DXFROMWINEMATRIX mvpEnv = projection * modelview;

		// Translate modelview matrix by camera position
		modelview.translate(-camPos);
		g_View=modelview;
		
		/* Calculate MVP matrix for house */
		D3DXFROMWINEMATRIX mvp = projection * modelview;

		// Draw the house normally
		for (unsigned int i = 0; i < house->getBatchCount(); i++){
			// Set render states
			//IRenderer::GetRendererInstance()->setCullFace(IRenderer::GetRendererInstance()->GetcullBack());
			IRenderer::GetRendererInstance()->setShader(lighting);
			IRenderer::GetRendererInstance()->setTexture("Base", base[i]);
			IRenderer::GetRendererInstance()->setTexture("LightMap", lightMap);
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
			IRenderer::GetRendererInstance()->setShaderConstant1f("maxValue", maxLMValue);
			
			
			// Draw geometry
			house->drawBatch( i);
		}

		// This pass is only needed because the source model for this sample contains T-junctions,
		// which causes pixel leakage. Leaked pixels show up as ugly artifacts after the blur filters below.
		// Real applications should not have T-junctions.
		//IRenderer::GetRendererInstance()->setCullFace(IRenderer::GetRendererInstance()->GetcullFront());
		//IRenderer::GetRendererInstance()->setDepthFunc(LESS);
		
		if(std::string(IRenderer::GetRendererInstance()->getString())=="D3D9")
		{
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_AMBIENT, 0xFF000000);	// Set global ambient color to black
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_LIGHTING, TRUE);		// Enable ambient lighting to get black on all pixels rendered
		}
		house->draw();
		if(std::string(IRenderer::GetRendererInstance()->getString())=="D3D9")
			//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_LIGHTING, FALSE);
		

		// Render environment
		drawEnvironment(mvpEnv);

		// Resolve FSAA render target by copying it to non-AA render target of the same size
		if (!m_FSAAMenu->isItemChecked(m_NoFSAAMenuItem)) {
			TextureID	lpNonAARenderTargetTexture;
			SurfaceID		lpNonAARenderTargetSurface;
#if 0 // ???
			lpNonAARenderTargetTexture = (renderTarget);
			hres = ((TextureID)lpNonAARenderTargetTexture)->GetSurfaceLevel(0, &lpNonAARenderTargetSurface);

			IRenderer::GetRendererInstance()->StretchRect(lpFSAARenderTarget, 0, lpNonAARenderTargetSurface, 0, D3DTEXF_POINT);
			lpNonAARenderTargetSurface->Release();
#endif
		}

	IRenderer::GetRendererInstance()->changeRenderTarget(blurTarget);

		// FP16 to I16 conversion
		IRenderer::GetRendererInstance()->setShader(convert);
		IRenderer::GetRendererInstance()->setVertexFormat(convertVF);
		IRenderer::GetRendererInstance()->setShaderConstant2f("halfPixel", D3DXFROMWINEVECTOR2(0.5f / 800, 0.5f / 600));
		IRenderer::GetRendererInstance()->setTexture("RT", renderTarget);
		IRenderer::GetRendererInstance()->setVertexBuffer(0,  m_fullscreenQuad);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		

		// Render quad covering render target
		IRenderer::GetRendererInstance()->drawArrays( PRIM_TRIANGLE_STRIP, 0, 3 );

	IRenderer::GetRendererInstance()->changeRenderTarget(blurTarget2);

		const D3DXFROMWINEVECTOR2 halfPixel = D3DXFROMWINEVECTOR2(0.5f / 200, 0.5f / 150);

		const D3DXFROMWINEVECTOR2 offsets0[4] = {
			D3DXFROMWINEVECTOR2( 0.33f / 400,  1.00f / 300) + halfPixel,
			D3DXFROMWINEVECTOR2( 1.00f / 400, -0.33f / 300) + halfPixel,
			D3DXFROMWINEVECTOR2(-0.33f / 400, -1.00f / 300) + halfPixel,
			D3DXFROMWINEVECTOR2(-1.00f / 400,  0.33f / 300) + halfPixel,
		};

		// Screen space blur
		IRenderer::GetRendererInstance()->setShader(blur);
		IRenderer::GetRendererInstance()->setVertexFormat(blurVF);
		IRenderer::GetRendererInstance()->setShaderConstantArray2f("offsets", offsets0, 4);
		IRenderer::GetRendererInstance()->setTexture("RT", blurTarget);
		IRenderer::GetRendererInstance()->setVertexBuffer(0,  m_fullscreenQuad);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		

		// Render quad covering render target
		IRenderer::GetRendererInstance()->drawArrays( PRIM_TRIANGLE_STRIP, 0, 3 );

	IRenderer::GetRendererInstance()->changeRenderTarget(blurTarget3);

		const D3DXFROMWINEVECTOR2 offsets1[4] = {
			D3DXFROMWINEVECTOR2( 1.00f / 200,  0.33f / 150) + halfPixel,
			D3DXFROMWINEVECTOR2( 0.33f / 200, -1.00f / 150) + halfPixel,
			D3DXFROMWINEVECTOR2(-1.00f / 200, -0.33f / 150) + halfPixel,
			D3DXFROMWINEVECTOR2(-0.33f / 200,  1.00f / 150) + halfPixel,
		};

		// Screen space blur
		IRenderer::GetRendererInstance()->setShader(blur);
		IRenderer::GetRendererInstance()->setVertexFormat(blurVF);
		IRenderer::GetRendererInstance()->setShaderConstantArray2f("offsets", offsets1, 4);
		IRenderer::GetRendererInstance()->setTexture("RT", blurTarget2);
		IRenderer::GetRendererInstance()->setVertexBuffer(0,  m_fullscreenQuad);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		

		// Render quad covering render target
		IRenderer::GetRendererInstance()->drawArrays( PRIM_TRIANGLE_STRIP, 0, 3 );

	IRenderer::GetRendererInstance()->changeRenderTarget(blurTarget2);

		const D3DXFROMWINEVECTOR2 offsets2[4] = {
			D3DXFROMWINEVECTOR2( 0.0f / 200,  0.0f / 150) + halfPixel,
			D3DXFROMWINEVECTOR2( 0.0f / 200,  1.0f / 150) + halfPixel,
			D3DXFROMWINEVECTOR2( 1.0f / 200, -0.5f / 150) + halfPixel,
			D3DXFROMWINEVECTOR2(-1.0f / 200, -0.5f / 150) + halfPixel,
		};

		// Screen space blur
		IRenderer::GetRendererInstance()->setShader(blur);
		IRenderer::GetRendererInstance()->setVertexFormat(blurVF);
		IRenderer::GetRendererInstance()->setShaderConstantArray2f("offsets", offsets2, 4);
		IRenderer::GetRendererInstance()->setTexture("RT", blurTarget3);
		IRenderer::GetRendererInstance()->setVertexBuffer(0,  m_fullscreenQuad);
		IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
		

		// Render quad covering render target
		IRenderer::GetRendererInstance()->drawArrays( PRIM_TRIANGLE_STRIP, 0, 3 );

	static int target = 0;
	if (hdrMenu->isItemChecked(useGPU)){
		/*
			Determine an appropriate exposure level by averaging the image on the GPU.
			To speed things up we use the blur render target as it has already reduced
			resolution a bit and it's blurry so it doesn't matter if we sample it at a
			bit lower rate. The 200x150 is mapped into a 64x64 texture, which has mipmap
			generation enabled. The 1x1 mipmap will contain the average.
		*/
		IRenderer::GetRendererInstance()->changeRenderTarget(averageTarget);

			IRenderer::GetRendererInstance()->setShader(average);
			IRenderer::GetRendererInstance()->setVertexFormat(averageVF);
			IRenderer::GetRendererInstance()->setTexture("RT", blurTarget2);
			IRenderer::GetRendererInstance()->setVertexBuffer(0,  m_fullscreenQuad);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			

			// Render quad covering render target
			IRenderer::GetRendererInstance()->drawArrays( PRIM_TRIANGLE_STRIP, 0, 3 );


		// Toggle between render targets per frame
		target = 1 - target;

		// Interpolate between previous and new exposure values to get smooth a transition.
		IRenderer::GetRendererInstance()->changeRenderTarget(exposureTarget[target]);

			IRenderer::GetRendererInstance()->setShader(exposure);
			IRenderer::GetRendererInstance()->setVertexFormat(exposureVF);
			IRenderer::GetRendererInstance()->setTexture("Average", averageTarget);
			IRenderer::GetRendererInstance()->setTexture("Exposure", exposureTarget[1 - target]);
			IRenderer::GetRendererInstance()->setShaderConstant1f("frameTime", frameTime);
			IRenderer::GetRendererInstance()->setVertexBuffer(0,  m_fullscreenQuad);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			

			// Render quad covering render target
			IRenderer::GetRendererInstance()->drawArrays( PRIM_TRIANGLE_STRIP, 0, 3 );
	}

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

	if (hdrMenu->isItemChecked(useCPU)){
		/*
			To determine an appropriate exposure level we shoot a number of rays into the scene
			and look up the luminance in each direction. The average value is used to determine
			a good exposure level. This method won't exactly match the GPU method, but close enough.
		*/
		D3DXFROMWINEVECTOR3 dx = modelview.getRightVec();
		D3DXFROMWINEVECTOR3 dy = modelview.getUpVec();
		D3DXFROMWINEVECTOR3 dz = modelview.getForwardVec();

		float average = 0;
		for (int y = -7; y <= 7; y++){
			for (int x = -7; x <= 7; x++){
				average += getLuminance(dz + (x / 8.0f) * dx + (y / 8.0f) * dy);
			}
		}
		average /= 15 * 15;

		// Make a smooth transistion between different exposure levels
		float newExp = 1.0f / (0.5f + average);
		static float exposure;
		exposure = lerp(exposure, newExp, frameTime);

		// Premultiply with constant so that we can use exp2 instead of exp in the shader, which boosts performance.
		const float expToExp2 = 1.0f / logf(2);

		// Setup HDR shader with shader constant exposure
		IRenderer::GetRendererInstance()->setShader(hdrCPU);
		IRenderer::GetRendererInstance()->setVertexFormat(hdrCPUVF);
		IRenderer::GetRendererInstance()->setShaderConstant1f("exposure", exposure * expToExp2);
	} else {
		// Setup HDR shader with a render target containing the exposure
		IRenderer::GetRendererInstance()->setShader(hdrGPU);
		IRenderer::GetRendererInstance()->setVertexFormat(hdrGPUVF);
		IRenderer::GetRendererInstance()->setTexture("Exposure", exposureTarget[target]);
	}
	IRenderer::GetRendererInstance()->setVertexBuffer(0, m_fullscreenQuad );
	IRenderer::GetRendererInstance()->setTexture("Scene", renderTarget);
	IRenderer::GetRendererInstance()->setTexture("Blur", blurTarget2);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	
	
	// Compose the final picture
	IRenderer::GetRendererInstance()->drawArrays( PRIM_TRIANGLE_STRIP, 0, 3 );
}
public:
int init(const char* aTitle)
{
	setup();
	resetCamera();
	//initGUI();
	load();
	return 0;
}

void render()
{
	drawFrame();
}
};
App app;
int init(const char* aTitle)
{
	app.init("");
	return 0;
}

void render()
{
	app.render();
}
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("HDR");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		IRenderer::GetRendererInstance()->BeginScene();
		render();
		IRenderer::GetRendererInstance()->EndScene();
		IRenderer::GetRendererInstance()->Present( );
	}
	return 0;
}

