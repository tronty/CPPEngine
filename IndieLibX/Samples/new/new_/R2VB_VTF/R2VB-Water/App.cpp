/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define LOG_FNLN STX_PRINT("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
//-----------------------------------------------------------------------------
// File: App.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#define RT_SIZE 128
#define WATER_POS_Y 10.0f

//-----------------------------------------------------------------------------------------
// A fullscreen quad
//-----------------------------------------------------------------------------------------
#if 1 // ???
static D3DXFROMWINEVECTOR2 vertices[] = {
	D3DXFROMWINEVECTOR2(-1, -1),
	D3DXFROMWINEVECTOR2( 1, -1),
	D3DXFROMWINEVECTOR2( 1,  1),
	D3DXFROMWINEVECTOR2(-1,  1)
};
#else
	float vertices[] = { -1, -1 , 1, -1, 1, 1, -1, 1};
#endif

class App {
public:
  unsigned int width, height;
  D3DXFROMWINEVECTOR3 camPos;
  float wx, wy, speed;

#ifdef R2VB_GUI
void initGUI(){
	BaseApp::addWidget(dropRate = new Slider(770, 340, 26, 256, false, 0, 1, 0.4f));

	BaseApp::initGUI();
}
#endif

void onSize(const int w, const int h){
	//D3D9App::onSize(w, h);

	dropRate->setPosition(w - 30.0f, h - 260.0f);
}

void setup(){
	width  = 800;
	height = 600;
	speed = 500;
}

void resetCamera(){
	camPos = D3DXFROMWINEVECTOR3(150, 100, 0);
	wx = 0.7f;
	wy = D3DXFROMWINE_PI / 2;
}

void moveCamera(const D3DXFROMWINEVECTOR3 &dir){
	D3DXFROMWINEVECTOR3 pos = camPos + dir * (speed * 1000.0f*timeGetTime());
	D3DXFROMWINEVECTOR3 cPos;

	D3DXFROMWINEVECTOR3 minLim(-160, -104, -160);
	D3DXFROMWINEVECTOR3 maxLim(160, 24, 160);

	float d = 0;
	for (int i = 0; i < 3; i++){
		if (pos[i] < minLim[i]){
			float s = pos[i] - minLim[i];
			d += s * s;
			cPos[i] = minLim[i];
		} else if (pos[i] > maxLim[i]){
			float s = pos[i] - maxLim[i];
			d += s * s;
			cPos[i] = maxLim[i];
		} else {
			cPos[i] = pos[i];
		}
	}

	if (d < 30 * 30){
		D3DXFROMWINEVECTOR3 dir = pos - cPos;
		camPos = cPos + 30 * normalize(dir);
	} else {
		camPos = pos;
	}
}

bool load(){

	// Load shaders
	
	wave   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Water/wave.shd", "main", "main");
	
	drop   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Water/drop.shd", "main", "main");
	
	skybox = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Water/skybox.shd", "main", "main");
	
	water  = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Water/water.shd", "main", "main");
	
	pool   = IRenderer::GetRendererInstance()->addShaderFromFile("/new/R2VB_VTF/R2VB-Water/pool.shd", "main", "main");
	

	/*
		Create render targets for the water dynamics.
		Since these render targets will be used as vertex buffers we need to
		signal this special usage to the driver with the D3DUSAGE_DMAP flag.
	*/
	rt0 = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetlinearClamp());
	rt1 = IRenderer::GetRendererInstance()->addRenderTarget(RT_SIZE, RT_SIZE, FORMAT_RGBA16f, IRenderer::GetRendererInstance()->GetlinearClamp());

	// Create screen-sized render targets for reflection and refraction
	reflection = IRenderer::GetRendererInstance()->addRenderTarget(width, height, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetlinearClamp());
	refraction = IRenderer::GetRendererInstance()->addRenderTarget(width, height, FORMAT_RGBA8, IRenderer::GetRendererInstance()->GetlinearClamp());
	depth = IRenderer::GetRendererInstance()->addRenderDepthStencil(width, height, 16, 0, TRACK_WINDOW_SIZE);

	currRT  = rt0;
	otherRT = rt1;

	// Load textures
	#if 0
	env = IRenderer::GetRendererInstance()->addCubemap("Mountains", "jpg");
	#else
    	sb.init("Mountains", "jpg");
	#endif


	brick=IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB/parqfloor2.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());

	stone=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/bump_normal/StoneWall2.dds", false, IRenderer::GetRendererInstance()->GettrilinearAniso());


	// Create the main water vertex buffer
	std::vector<float> vv(RT_SIZE * RT_SIZE*7);

	float *dest = &vv[0];

		for (int z = 0; z < RT_SIZE; z++){
			float fz = float(2 * z) / (RT_SIZE - 1) - 1.0f;
			for (int x = 0; x < RT_SIZE; x++){
				float fx = float(2 * x) / (RT_SIZE - 1) - 1.0f;
				dest++;
				dest++;
				dest++;
				*dest++ = 128 * fx;
				*dest++ = 128 * fz;
				dest++;
				dest++;
			}
		}

	waterVB = IRenderer::GetRendererInstance()->addVertexBuffer(vv.size()*sizeof(float), STATIC, &vv[0]);


	// Create the water index buffer
	std::vector<uint> vi(6 * (RT_SIZE - 1) * (RT_SIZE - 1));

	uint *iDest = &vi[0];

		for (int z = 0; z < RT_SIZE - 1; z++){
			for (int x = 0; x < RT_SIZE - 1; x++){
				*iDest++ = (z * RT_SIZE + x);
				*iDest++ = (z * RT_SIZE + x + 1);
				*iDest++ = ((z + 1) * RT_SIZE + x);
				*iDest++ = ((z + 1) * RT_SIZE + x);
				*iDest++ = (z * RT_SIZE + x + 1);
				*iDest++ = ((z + 1) * RT_SIZE + x + 1);
			}
		}

	waterIB = IRenderer::GetRendererInstance()->addIndexBuffer(6 * (RT_SIZE - 1) * (RT_SIZE - 1), 4, STATIC, &vi[0]);


	// Create vertex declarations
	FormatDesc waterAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		1, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
	};

	if ((waterVF = IRenderer::GetRendererInstance()->addVertexFormat(waterAttribs, elementsOf(waterAttribs), water)) == VF_NONE) return false;


	FormatDesc waveAttribs[] = { 0, TYPE_VERTEX, FORMAT_FLOAT, 2 };

	if ((waveVF = IRenderer::GetRendererInstance()->addVertexFormat(waveAttribs, elementsOf(waveAttribs), wave)) == VF_NONE) return false;


	FormatDesc skyboxAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
	};

	if ((skyboxVF = IRenderer::GetRendererInstance()->addVertexFormat(skyboxAttribs, elementsOf(skyboxAttribs), skybox)) == VF_NONE) return false;


	FormatDesc poolAttribs[] = {
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 1,
	};

	if ((poolVF = IRenderer::GetRendererInstance()->addVertexFormat(poolAttribs, elementsOf(poolAttribs), pool)) == VF_NONE) return false;


	onReset();

	return true;
}

bool onReset(){
	// Initialize the render targets
	IRenderer::GetRendererInstance()->changeRenderTarget(rt0);
	IRenderer::GetRendererInstance()->Clear(true, true, stx_GetRGBA(0x80808080));

	IRenderer::GetRendererInstance()->changeRenderTarget(rt1);
	IRenderer::GetRendererInstance()->Clear(true, true, stx_GetRGBA(0x80808080));

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

	needInit = true;

	return true;
}

void drawDrop(const D3DXFROMWINEVECTOR2 &xy, const float size, const float strength, const float intp){

	static D3DXFROMWINEVECTOR2 dVertices[] = {
		D3DXFROMWINEVECTOR2(-0.707f, -0.707f),
		D3DXFROMWINEVECTOR2( 0, -1),
		D3DXFROMWINEVECTOR2( 0.707f, -0.707f),
		D3DXFROMWINEVECTOR2( 1,  0),
		D3DXFROMWINEVECTOR2( 0.707f,  0.707f),
		D3DXFROMWINEVECTOR2( 0,  1),
		D3DXFROMWINEVECTOR2(-0.707f,  0.707f),
		D3DXFROMWINEVECTOR2(-1,  0),
	};

	/*
		Drop a drop on the current render target.
		Emulate floating point blending by reading the other render target.
	*/
	IRenderer::GetRendererInstance()->setShader(drop);
	IRenderer::GetRendererInstance()->setVertexFormat(waveVF);
	IRenderer::GetRendererInstance()->setTexture("Tex", otherRT);
	IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);

	IRenderer::GetRendererInstance()->setShaderConstant1f("off", 0.5f / RT_SIZE);

	IRenderer::GetRendererInstance()->setShaderConstant2f("xy", xy);
	IRenderer::GetRendererInstance()->setShaderConstant1f("size", size);
	IRenderer::GetRendererInstance()->setShaderConstant1f("strength", strength);
	IRenderer::GetRendererInstance()->setShaderConstant2f("intp", D3DXFROMWINEVECTOR2(intp, 1.0f - intp));

	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 6, dVertices, dVertices, sizeof(D3DXFROMWINEVECTOR2));
}

void updateWater(const D3DXFROMWINEMATRIX &mvp, float f){

	IRenderer::GetRendererInstance()->changeRenderTarget(currRT);


		IRenderer::GetRendererInstance()->setShader(wave);

		IRenderer::GetRendererInstance()->setVertexFormat(waveVF);

		IRenderer::GetRendererInstance()->setTexture("Tex", otherRT);

		IRenderer::GetRendererInstance()->setDepthFunc(eDEPTH_NONE);


		IRenderer::GetRendererInstance()->setShaderConstant1f("off", 0.5f / RT_SIZE);

		IRenderer::GetRendererInstance()->setShaderConstant1f("acceleration", 17 * 1000.0f*timeGetTime());

		IRenderer::GetRendererInstance()->setShaderConstant1f("velocity", 60 * 1000.0f*timeGetTime());

		IRenderer::GetRendererInstance()->setShaderConstant1f("damp", pow(0.5f, 1000.0f*timeGetTime()));


		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, vertices, vertices, sizeof(D3DXFROMWINEVECTOR2));


		if (needInit){

			/*
				When the water RTs are first created and cleared they need a bit of noise in them to behave
				naturally. Otherwise the fact that 0.0 is right between 32677 and 32768 when the texture is
				mapped to signed range and thus isn't exactly represented it makes the entire surface either
				lift or sink depending on if you clear to 0x7F or 0x80 since all heights will be equal after
				the clear and therefore the force computed for each pixel will be zero, maintaining the slight
				movement. Initially drawing a drop over the entire surface with minimal strength adds enough
				differences between neighboring texels to make this effect go away.
			*/
			drawDrop(D3DXFROMWINEVECTOR2(0, 0), 1.414f, 0.0001f, 0.5f);

		}

		/*
			Interact with the surface when the right mouse button is pressed
		*/
		if (STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_MIDDLE) || STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT)){
			// Position in clip space. We don't know z, so we set it arbitrarily to 0.
			D3DXFROMWINEVECTOR4 cPos(2 * float(STX_Service::GetInputInstance()->GetMouseX()) / float(width) - 1, 1 - 2 * float(STX_Service::GetInputInstance()->GetMouseY()) / float(height), 0, 1);
			// Reverse transform to unproject it back to world space.
			// This gives a point that would map to the clicked pixel.
			D3DXFROMWINEVECTOR4 unProj = !mvp * cPos;

			// Compute direction from camera to this position
			D3DXFROMWINEVECTOR3 dir = normalize((unProj.xyz() / unProj.w) - camPos);
			D3DXFROMWINEVECTOR3 pos = camPos - ((camPos.y/* + WATER_POS_Y*/) / dir.y) * dir;

			// Compute where this vector would hit the water plane
			D3DXFROMWINEVECTOR2 xy(floorf(pos.x + 0.5f) / 128.0f, floorf(-pos.z + 0.5f) / 128.0f);

			drawDrop(xy, 0.125f, STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT)? 0.005f : -0.005f, 0.95f);
		}


		float dr = 0;
		if(dropRate)
			dr=dropRate->getValue();

		if (dr > 0){

			static D3DXFROMWINEVECTOR2 xy;
			static uint c = 0;
			static float t = 0;
			t += 1000.0f*timeGetTime();

			dr = 0.2f / dr;

			if (t > dr){
				xy = D3DXFROMWINEVECTOR2(float(rand()) / RAND_MAX * 2 - 1, float(rand()) / RAND_MAX * 2 - 1);
				t -= dr;
				c = 0;
			}
			if (c < 10){
				drawDrop(xy, 0.125f, 0.008f, 0.95f);
				c++;
			}
		}


	IRenderer::GetRendererInstance()->changeToMainFramebuffer();

}

void drawSkybox(const D3DXFROMWINEMATRIX &mvp){
	#if 0
	IRenderer::GetRendererInstance()->setShader(skybox);
	IRenderer::GetRendererInstance()->setVertexFormat(skyboxVF);
	IRenderer::GetRendererInstance()->setTexture("Skybox", env);
	IRenderer::GetRendererInstance()->setMask(eCOLOR);
	

	/*
		Render the skybox in clip-space. We use a big triangle spanning outside
		the screen to avoid polygon edges within the screen for a tiny performance
		boost. Z is set to constant 1 to put it behind everything.

		The texture coordinates for the cubemap is computed by projecting the
		clip-space position back into world space with the inverse MVP matrix.
	*/
	D3DXFROMWINEMATRIX iMvp; // = !mvp;
	FLOAT determinant=0;
	D3DXFROMWINEMatrixInverse(&iMvp, &determinant, &mvp);

	D3DXFROMWINEVECTOR4 v0( 0,  2, 1, 1);
	D3DXFROMWINEVECTOR4 v1( 3, -1, 1, 1);
	D3DXFROMWINEVECTOR4 v2(-3, -1, 1, 1);

	struct Vertex {
		D3DXFROMWINEVECTOR4 clipPos;
		D3DXFROMWINEVECTOR4 cubeCoord;
	};
	D3DXFROMWINEVECTOR4 vout;

	Vertex vertices[3] = {
		v0, *D3DXFROMWINEVec4TransformCoord(&vout, &v0, &iMvp),
		v1, *D3DXFROMWINEVec4TransformCoord(&vout, &v1, &iMvp),
		v2, *D3DXFROMWINEVec4TransformCoord(&vout, &v2, &iMvp),
	};

	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, 1, vertices, vertices, sizeof(Vertex));
	#else

        sb.render();

	#endif
}

void drawPool(const D3DXFROMWINEMATRIX &mvp){

	struct Vertex {
		D3DXFROMWINEVECTOR3 vertex;
		D3DXFROMWINEVECTOR2 texCoord;
		float  color;
	};

	static const Vertex vertices[] = {
		D3DXFROMWINEVECTOR3(-128, -104,  128), D3DXFROMWINEVECTOR2(0, 1), 0.3f, D3DXFROMWINEVECTOR3(-128, 24,  128), D3DXFROMWINEVECTOR2(0, 0), 0.3f,
		D3DXFROMWINEVECTOR3( 128, -104,  128), D3DXFROMWINEVECTOR2(2, 1), 0.7f, D3DXFROMWINEVECTOR3( 128, 24,  128), D3DXFROMWINEVECTOR2(2, 0), 0.7f,
		D3DXFROMWINEVECTOR3( 128, -104, -128), D3DXFROMWINEVECTOR2(4, 1), 0.7f, D3DXFROMWINEVECTOR3( 128, 24, -128), D3DXFROMWINEVECTOR2(4, 0), 0.7f,
		D3DXFROMWINEVECTOR3(-128, -104, -128), D3DXFROMWINEVECTOR2(2, 1), 0.3f, D3DXFROMWINEVECTOR3(-128, 24, -128), D3DXFROMWINEVECTOR2(2, 0), 0.3f,

		D3DXFROMWINEVECTOR3(-160, -104,  160), D3DXFROMWINEVECTOR2(0, 1), 1.0f, D3DXFROMWINEVECTOR3(-160, 24,  160), D3DXFROMWINEVECTOR2(0, 0), 1.0f,
		D3DXFROMWINEVECTOR3( 160, -104,  160), D3DXFROMWINEVECTOR2(2, 1), 0.5f, D3DXFROMWINEVECTOR3( 160, 24,  160), D3DXFROMWINEVECTOR2(2, 0), 0.5f,
		D3DXFROMWINEVECTOR3( 160, -104, -160), D3DXFROMWINEVECTOR2(4, 1), 0.5f, D3DXFROMWINEVECTOR3( 160, 24, -160), D3DXFROMWINEVECTOR2(4, 0), 0.5f,
		D3DXFROMWINEVECTOR3(-160, -104, -160), D3DXFROMWINEVECTOR2(2, 1), 1.0f, D3DXFROMWINEVECTOR3(-160, 24, -160), D3DXFROMWINEVECTOR2(2, 0), 1.0f,

		D3DXFROMWINEVECTOR3(-160, 24,  160), D3DXFROMWINEVECTOR2(-0.25f, -0.25f), 0.8f, D3DXFROMWINEVECTOR3(-128, 24,  128), D3DXFROMWINEVECTOR2(0, 0), 0.8f,
		D3DXFROMWINEVECTOR3( 160, 24,  160), D3DXFROMWINEVECTOR2( 2.25f, -0.25f), 0.8f, D3DXFROMWINEVECTOR3( 128, 24,  128), D3DXFROMWINEVECTOR2(2, 0), 0.8f,
		D3DXFROMWINEVECTOR3( 160, 24, -160), D3DXFROMWINEVECTOR2( 2.25f,  2.25f), 0.8f, D3DXFROMWINEVECTOR3( 128, 24, -128), D3DXFROMWINEVECTOR2(2, 2), 0.8f,
		D3DXFROMWINEVECTOR3(-160, 24, -160), D3DXFROMWINEVECTOR2(-0.25f,  2.25f), 0.8f, D3DXFROMWINEVECTOR3(-128, 24, -128), D3DXFROMWINEVECTOR2(0, 2), 0.8f,

		D3DXFROMWINEVECTOR3(-128, -104,  128), D3DXFROMWINEVECTOR2(0, 0), 0.3f, D3DXFROMWINEVECTOR3(128, -104,  128), D3DXFROMWINEVECTOR2(2, 0), 1.0f,
		D3DXFROMWINEVECTOR3(-128, -104, -128), D3DXFROMWINEVECTOR2(0, 2), 0.3f, D3DXFROMWINEVECTOR3(128, -104, -128), D3DXFROMWINEVECTOR2(2, 2), 1.0f,
	};

	static const ushort indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1 };


	IRenderer::GetRendererInstance()->setShader(pool);

	IRenderer::GetRendererInstance()->setVertexFormat(poolVF);

	IRenderer::GetRendererInstance()->setTexture("Base", brick);

	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);


	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 8, 8, indices, indices, CONSTANT_INDEX2, vertices, vertices,      sizeof(Vertex));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 8, 8, indices, indices, CONSTANT_INDEX2, vertices, vertices + 8,  sizeof(Vertex));

	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLE_STRIP, 0, 8, 8, indices, indices, CONSTANT_INDEX2, vertices, vertices + 16, sizeof(Vertex));


	IRenderer::GetRendererInstance()->setTexture("Base", stone);

	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, vertices + 24, vertices + 24, sizeof(Vertex));

}

void drawMainScene(const D3DXFROMWINEMATRIX &mvpSB, const D3DXFROMWINEMATRIX &mvp, const float *clipPlane=0){

	IRenderer::GetRendererInstance()->Clear(true, true, stx_GetRGBA(0x80808080));


	if (clipPlane){

		IRenderer::GetRendererInstance()->SetClipPlane(0, clipPlane);
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	}

	drawPool(mvp);

	if (clipPlane){
		//IRenderer::GetRendererInstance()->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	}

	drawSkybox(mvpSB);

}

void drawFrame(){

	
	D3DXFROMWINEMATRIX projection = perspectiveMatrixX(D3DXFROMWINE_PI / 2, width, height, 1, 2000, true);

	D3DXFROMWINEMATRIX modelview = rotateXY4(-wx, -wy);
	D3DXFROMWINEMATRIX mvpSB = projection * modelview;
	D3DXFROMWINEMATRIX T;
	D3DXFROMWINEMatrixTranslation(&T, -camPos.x, -camPos.y, -camPos.z);
	D3DXFROMWINEMATRIX mvp = mvpSB * T;

	/*
		Only update reflection and refraction textures if the view changed. Boosts performance quite a bit.
	*/
	static D3DXFROMWINEVECTOR3 lastPos;
	static float lastWx, lastWy;

	
	if (needInit || lastPos != camPos || lastWx != wx || lastWy != wy){

	
		IRenderer::GetRendererInstance()->changeRenderTarget(reflection, depth);

	
			D3DXFROMWINEMATRIX mirrorMVP = mvp * mirrorMatrix(D3DXFROMWINEVECTOR3(0, 1, 0), WATER_POS_Y);
			D3DXFROMWINEVECTOR4 clipPlane = (!transpose(mirrorMVP)) * D3DXFROMWINEVECTOR4(0, 1, 0, WATER_POS_Y + 10);

	
			drawMainScene(mvpSB * scale4(1, -1, 1), mirrorMVP, clipPlane);

	

		IRenderer::GetRendererInstance()->changeRenderTarget(refraction, depth);

	
			clipPlane = (!transpose(mvp)) * D3DXFROMWINEVECTOR4(0, -1, 0, -WATER_POS_Y + 10);

			drawMainScene(mvpSB, mvp, clipPlane);

	

		IRenderer::GetRendererInstance()->changeToMainFramebuffer();

	

		lastPos = camPos;
		lastWx = wx;
		lastWy = wy;
	}


	
	// Draw scene, excluding water
	drawMainScene(mvpSB, mvp);


	

	/*
		Draw the water using the R2VB extension. This extension allows us to bind a texture as a
		vertex buffer, allowing for interesting effects by rendering to vertex buffer, in this case
		letting a pixel shader do physics on a water surface and pass that to the vertex shader for
		a truely geometric water effect done entirely on the VPU.
	*/

	
	IRenderer::GetRendererInstance()->setShader(water);

	
	IRenderer::GetRendererInstance()->setVertexFormat(waterVF);

	
	IRenderer::GetRendererInstance()->setVertexBuffer(0, waterVB);

	
	//IRenderer::GetRendererInstance()->setVertexBuffer(1, waterVB); // Dummy vertex buffer. Only the stride and offset data will be used.

	IRenderer::GetRendererInstance()->setIndexBuffer(waterIB);

	
	IRenderer::GetRendererInstance()->setTexture("Reflection", reflection);

	
	IRenderer::GetRendererInstance()->setTexture("Refraction", refraction);

	
	

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);

	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvpProj", translate(0.5f, 0.5f, 0) * scale4(0.5f, -0.5f, 1) * mvp);

	
	IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", camPos);


	

	// Bind the render target to the DMAP sampler
	IRenderer::GetRendererInstance()->setTexture("DMAP", currRT);

	
	
	

	// ??? IRenderer::GetRendererInstance()->DrawIndexedPrimitive(PRIM_TRIANGLES, 0, 0, RT_SIZE * RT_SIZE, 0, 2 * (RT_SIZE - 1) * (RT_SIZE - 1));

	

	/*
		The water simulation can become unstable if the framerate fluctuates too much.
		Here we try to keep the update frequency within a certain range to avoid that.
	*/
#define MIN_FPS 50
#define MAX_FPS 100
#define MIN_TIME (1.0f / MAX_FPS)
#define MAX_TIME (1.0f / MIN_FPS)

	// Main water dynamics pass
	static float timeCredit = MIN_TIME;
	timeCredit += 1000.0f*timeGetTime();

	

	if (timeCredit > MIN_TIME){

		float ft = min(floor(timeCredit * MAX_FPS) * MIN_TIME, MAX_TIME);


		updateWater(mvp, ft);


		timeCredit -= ft;

		// Flip render targets
		TextureID temp = currRT;
		currRT = otherRT;
		otherRT = temp;
	}


	needInit = false;
	

}

private:
	VertexFormatID waterVF, skyboxVF, waveVF, poolVF;
	VertexBufferID waterVB;
	IndexBufferID waterIB;

	ShaderID wave, drop, skybox, water, pool;
	TextureID env, rt0, rt1, currRT, otherRT, brick, stone, reflection, refraction, depth;
	stx_SkyBox sb;

	Slider *dropRate;

	bool needInit;
public:
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
	IRenderer* r=IRenderer::GetRendererInstance("R2VB-Water");
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

