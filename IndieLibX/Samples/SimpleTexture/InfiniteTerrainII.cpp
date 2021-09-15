/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

//define STX_PRINT(...) printf(__VA_ARGS__)
//define STX_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__)

ShaderID shd=-1;
TextureID texture=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	STX_FNLN;
	#if 0
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/InfiniteTerrainII.shd", "mainVS", "mainPS");
	#elif 0
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/InfiniteTerrainII.shd", "mainTerrainVS", "mainTerrainPS");
	#else
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/www.shadertoy.com/Xyptonjtroz2.shd", "main2", "main");
	#endif
	STX_FNLN;
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	STX_FNLN;
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	STX_FNLN;
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
	STX_FNLN;
	return 0;
}

void draw()
{
	STX_FNLN;
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
	//D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	//IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	D3DXFROMWINEMATRIX I, projection, R, S, T;
	D3DXFROMWINEMatrixIdentity(&I);
	#if 0
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	#else
	#if 0
	D3DXFROMWINEVECTOR3 camPos(0.0f, 0.0f, -0.0f);
	float wx = D3DXFROMWINE_PI*0.0f;
	float wy = D3DXFROMWINE_PI*0.0f;
	D3DXFROMWINEMatrixIdentity(&projection);
	D3DXFROMWINEMatrixPerspectiveFovLH(&projection, 1.5f, 1.0f*IRenderer::GetRendererInstance()->GetViewportWidth()/IRenderer::GetRendererInstance()->GetViewportHeight(), 0.001f, 10000.0f);
	#else
	D3DXFROMWINEVECTOR3 camPos(0.0f, 0.0f, -0.5f);
	float wx = D3DXFROMWINE_PI/0.6f;
	float wy = D3DXFROMWINE_PI/0.6f;
	D3DXFROMWINEMatrixPerspectiveFovLH(&projection, 1.5f, 1.0f*IRenderer::GetRendererInstance()->GetViewportWidth()/IRenderer::GetRendererInstance()->GetViewportHeight(), 0.001f, 10000.0f);
	#endif
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, wx, wy, 0.0f);
	D3DXFROMWINEMatrixIdentity(&S);
	D3DXFROMWINEMatrixTranslation(&T, camPos.x, camPos.y, camPos.z );
	D3DXFROMWINEMATRIX view=R*S*T; 
	D3DXFROMWINEMATRIX mvp = view;//*projection;

	D3DXFROMWINEVECTOR2 mouse(1.0f,1.0f);//((float)STX_Service::GetInputInstance()->GetMouseX(), (float)STX_Service::GetInputInstance()->GetMouseY());
	D3DXFROMWINEVECTOR2 resolution(1.0f,1.0f);//(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
	static float start=timeGetTime();
	float time=.00025 * (timeGetTime() - start );

	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", I);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iMouse", mouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", resolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTime", time);
	IRenderer::GetRendererInstance()->setShaderConstant2f("mouse", mouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("resolution", resolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iChannelResolution", resolution);
	#if 0
	IRenderer::GetRendererInstance()->setTexture("iChannel0", tex[0]);
	IRenderer::GetRendererInstance()->setTexture("iChannel1", tex[1]);
	IRenderer::GetRendererInstance()->setTexture("iChannel2", tex[2]);	
	#endif	
	#endif
	#if 1
	//		           x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			          1.0f, 1.0f, 1.0f, 0.0f,
			         -1.0f,-1.0f, 0.0f, 1.0f,
			         -1.0f, 1.0f, 0.0f, 0.0f };
	unsigned int N=4;
	#else
	float v[16] =  {  1.0f,-1.0f,
			           1.0f, 1.0f,
			          -1.0f,-1.0f,
			          -1.0f, 1.0f };
	unsigned int N=2;
	#endif
#if 1
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], N*sizeof(float));
#else
	__WORD__ i[6] ={0,1,2,2,1,3};
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 4, 2, &i[0], &i[0], CONSTANT_INDEX2, &v[0], &v[0], N*sizeof(float));
#endif
	STX_FNLN;
}

void render()
{
	STX_FNLN;
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	STX_FNLN;
	IRenderer::GetRendererInstance()->BeginScene();
	STX_FNLN;
	draw();
	STX_FNLN;
	IRenderer::GetRendererInstance()->EndScene();
	STX_FNLN;
	IRenderer::GetRendererInstance()->Present( );
	STX_FNLN;
}

int ApplicationLogic()
{
	STX_FNLN;
	IRenderer* r=IRenderer::GetRendererInstance("InfiniteTerrainII_2D");
	STX_FNLN;
	IInput*    i=STX_Service::GetInputInstance();
	STX_FNLN;
	init("");
	STX_FNLN;
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
	STX_FNLN;
		i->Update();
	STX_FNLN;
		render();
	STX_FNLN;
		//stx_exit(0);
	}
	STX_FNLN;
	return 0;
}

