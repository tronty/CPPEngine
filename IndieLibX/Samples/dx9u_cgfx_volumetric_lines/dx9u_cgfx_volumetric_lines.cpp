//-----------------------------------------------------------------------------
//           Name: dx9u_cgfx_volumetric_lines.cpp
//         Author: Adel Amro (adel_w_amro@hotmail.com)
//  Last Modified: November 13, 2007
//    Description: This sample demonstrates a technique for efficiently drawing
//                 what is often called volume lines. These are quads oriented
//                 and textured in such a way to be rendered as though they were
//                 thick lines. This sample is based on the OpenGL sample from
//                 Nvidia titled CG Volume Lines. It should work on low end
//                 cards.
//
//   Control Keys: Click and drag to spin the bunch of lines.
//                 The + and - keys to control line thickness.
//                 Page Down (PgDn) to cycle through available textures.
//                 Arrow keys: Move the bunch of lines.
//-----------------------------------------------------------------------------
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

# define ARRAY_SIZE( x ) ( (sizeof((x))/sizeof((x)[0])) )

inline float frand01() { return (float)stx_rand()/RAND_MAX; }
inline float frand( float fMin, float fMax ) { return fMin + (fMax - fMin) * ((float)stx_rand()/RAND_MAX); }
inline float frand() { return (float)stx_rand()/RAND_MAX + stx_rand(); }

// Globals

// Array of texture file names.
const char* g_aFileNames[] = {
		"/fx_volumetric_lines/1d_INNER1.png" ,
		"/fx_volumetric_lines/1d_INNER2.png" ,
		"/fx_volumetric_lines/1d_DEBUG.png" ,
		"/fx_volumetric_lines/1d_DEBUG2.png" ,
		"/fx_volumetric_lines/1d_glow1.png", 0 
};

TextureID g_pTexture [] = {
		-1 ,
		-1 ,
		-1 ,
		-1 ,
		-1 , 0 
}; 
ShaderID g_pEffect = -1;
VertexFormatID g_pVF = -1;
VertexBufferID g_pVB = -1;
D3DXFROMWINEVECTOR3 g_aLines[ 16 ];

int g_iCurrentTexture = 1;
float g_fThickness = 1.f;

float g_fTX = 0.f;
float g_fTY = 0.f;
float g_fTZ = 14.f;

D3DXFROMWINEMATRIX g_mProjection;

// The vertex structure we'll be using for line drawing. Each line is defined as two vertices,
// and the vertex shader will create a quad from these two vertices. However, since the vertex
// shader can only process one vertex at a time, we need to store information in each vertex
// about the other vertex (the other end of the line).
struct TVertex
{
	D3DXFROMWINEVECTOR3 pos;
	D3DXFROMWINEVECTOR3 otherPos;
	D3DXFROMWINEVECTOR4 texOffset;
	D3DXFROMWINEVECTOR3 thickness;
/*
	static const __DWORD__ FVF = D3DFVF_XYZ | D3DFVF_NORMAL |
		D3DFVF_TEX2 | // D3DFVF_TEX2 specifies we have two sets of texture coordinates.
		D3DFVF_TEXCOORDSIZE4(0) | // This specifies that the first (0th) tex coord set has size 4 floats.
		D3DFVF_TEXCOORDSIZE3(1); // Specifies that second tex coord set has size 2 floats.
*/
};

// Initialize the application.
int init(const char* aTitle)
{
	int hr=0;

	// Load the texture.
		SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	int i=0;
	while(g_pTexture[i]==-1)
		g_pTexture[i]=IRenderer::GetRendererInstance()->addImageLibTexture(g_aFileNames[i++], false, ss);

        g_pEffect = IRenderer::GetRendererInstance()->addShaderFromFile("/fx_volumetric_lines/VolumeLines.fx", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_NORMAL, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 3
	};
	g_pVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), g_pEffect);

	// Set up some device states.
	D3DXFROMWINEMatrixPerspectiveFovLH( &g_mProjection, 45.f,
		float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight(),
		0.1f, 100.f );

	//IRenderer::GetRendererInstance()->SetTransform( TS_PROJECTION, &g_mProjection );
	//IRenderer::GetRendererInstance()->SetRenderState( D3DRS_LIGHTING, false );
	//IRenderer::GetRendererInstance()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );


	// Now create a random list of lines (pairs of vertices) that we will render
	stx_srand( timeGetTime() );
	for( UINT i=0; i<ARRAY_SIZE(g_aLines); i++ )
	{
		g_aLines[i].x = frand(-1, 1) * 5.f;
		g_aLines[i].y = frand(-1, 1) * 5.f;
		g_aLines[i].z = frand(-1, 1) * 5.f;
	}

	return S_OK;
}

void render()
{
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (0.0f,0.0f,70.0f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

		D3DXFROMWINEMATRIX mWorld;
		D3DXFROMWINEMATRIX mWorldProjection;
		D3DXFROMWINEMATRIX mTranslation;
		D3DXFROMWINEMATRIX mRotation;

	float mAngleX=0.0f;
	float mAngleY=0.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX matRot;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mRotation,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

		D3DXFROMWINEMatrixTranslation( &mTranslation, g_fTX, g_fTY, g_fTZ );//0.f, 0.f, 14.f );
		mWorld = mRotation * mTranslation;
		mWorldProjection = mWorld * g_mProjection;

			for( UINT i=0; i < ARRAY_SIZE( g_aLines ) / 2; i++ )
			{

	IRenderer::GetRendererInstance()->setShader(g_pEffect);
	IRenderer::GetRendererInstance()->setVertexFormat(g_pVF);

		// We will not be using a viewing transformation, so the view matrix will be identity.
		IRenderer::GetRendererInstance()->setShaderConstant4x4f( "mWV", mWorld );
		IRenderer::GetRendererInstance()->setShaderConstant4x4f( "mWVP", mWorldProjection );
				D3DXFROMWINEVECTOR3& v0=g_aLines[i*2];
				D3DXFROMWINEVECTOR3& v1=g_aLines[i*2 + 1];
	TVertex vrts[4];
	vrts[0].pos = v0;		vrts[0].otherPos = v1;
	vrts[1].pos = v1;		vrts[1].otherPos = v0;
	vrts[2].pos = v0;		vrts[2].otherPos = v1;
	vrts[3].pos = v1;		vrts[3].otherPos = v0;

	vrts[0].thickness = D3DXFROMWINEVECTOR3( -g_fThickness, 0.f, g_fThickness * 0.5f );
	vrts[1].thickness = D3DXFROMWINEVECTOR3(  g_fThickness, 0.f, g_fThickness * 0.5f );
	vrts[2].thickness = D3DXFROMWINEVECTOR3(  g_fThickness, 0.f, g_fThickness * 0.5f );
	vrts[3].thickness = D3DXFROMWINEVECTOR3( -g_fThickness, 0.f, g_fThickness * 0.5f );

	vrts[0].texOffset = D3DXFROMWINEVECTOR4( g_fThickness, g_fThickness, 0.f, 0.f );
	vrts[1].texOffset = D3DXFROMWINEVECTOR4( g_fThickness, g_fThickness, 0.25f, 0.f );
	vrts[2].texOffset = D3DXFROMWINEVECTOR4( g_fThickness, g_fThickness, 0.f, 0.25f );
	vrts[3].texOffset = D3DXFROMWINEVECTOR4( g_fThickness, g_fThickness, 0.25f, 0.25f );
	
	IRenderer::GetRendererInstance()->setTexture( "textureSampler", g_pTexture[g_iCurrentTexture] );

	IRenderer::GetRendererInstance()->DrawPrimitiveUP( PRIM_TRIANGLE_STRIP, 2, &vrts[0], &vrts[0], sizeof( TVertex ) );
			}

		char txt[128];
		stx_snprintf(txt, 128, "g_iCurrentTexture=%d\n", g_iCurrentTexture);
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		stx_snprintf(txt, 128, "g_fThickness=%f\n", g_fThickness);
		IRenderer::GetRendererInstance()->drawText(txt, 10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		stx_snprintf(txt, 128, "g_fTX=%f\n", g_fTX);
		IRenderer::GetRendererInstance()->drawText(txt, 10, 50, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		stx_snprintf(txt, 128, "g_fTZ=%f\n", g_fTZ);
		IRenderer::GetRendererInstance()->drawText(txt, 10, 70, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("fx_volumetric_lines");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_PAGEUP ))
			g_iCurrentTexture = (g_iCurrentTexture+1)%ARRAY_SIZE(g_aFileNames);
		else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_PAGEDOWN ))
			g_iCurrentTexture = (g_iCurrentTexture-1)%ARRAY_SIZE(g_aFileNames);
		else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_HOME ))
			g_fThickness += 0.03f;
		else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_END ))
			g_fThickness -= 0.03f;
		else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_LEFT ))
			g_fTX += 0.5f;
		else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_RIGHT ))
			g_fTX -= 0.5f;
		else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_UP ))
			g_fTZ -= 0.5f;
		else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_DOWN ))
			g_fTZ += 0.5f;
		render();
	}
	return 0;
}

