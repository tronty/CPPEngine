#include "LitVertexVS2PS.h"

#include "Viewer.h"
#include "DXErrors.h"



bool LitVertexVS2PS::inited=false;

ShaderID LitVertexVS2PS::m_pEffect=-1;
//VertexFormatID LitVertexVS2PS::VERTEXVF=-1;
VertexFormatID LitVertexVS2PS::LVERTEX1VF=-1;

//-----------------------------------------------------------------------------

// Name: init()

// Desc:

//-----------------------------------------------------------------------------

void LitVertexVS2PS::init( void )

{
	

	if(inited)return;
/*
	if(g_pApp->m_renderer==D3D11)

	m_pEffect = IRenderer::GetRendererInstance()->addShaderFromFile("LitVertex11.shd", "mainVS", "mainPS");

	else*/

	m_pEffect = IRenderer::GetRendererInstance()->addShaderFromFile("/RadeonTerrainDemo/LitVertex.shd", "mainVS", "mainPS");



	bool useHalf=false;
#if 0
	FormatDesc D3DFVF_VERTEXFmt[] = {

		0, TYPE_VERTEX, useHalf? FORMAT_HALF : FORMAT_FLOAT, 3,

		0, TYPE_NORMAL, useHalf? FORMAT_HALF : FORMAT_FLOAT, 3,

		0, TYPE_TEXCOORD, useHalf? FORMAT_HALF : FORMAT_FLOAT, 2,

	};



////LOG_PRINT("LitVertexVS2PS::m_pEffect=%x\n", LitVertexVS2PS::m_pEffect);

	VERTEXVF = IRenderer::GetRendererInstance()->addVertexFormat(D3DFVF_VERTEXFmt, elementsOf(D3DFVF_VERTEXFmt), m_pEffect);
#endif

	FormatDesc D3DFVF_LVERTEX1Fmt[] = {

		0, TYPE_VERTEX, useHalf? FORMAT_HALF : FORMAT_FLOAT, 3,

		//0, TYPE_NORMAL, useHalf? FORMAT_HALF : FORMAT_FLOAT, 3,

		0, TYPE_TEXCOORD, useHalf? FORMAT_HALF : FORMAT_FLOAT, 2,

	};

	LVERTEX1VF = IRenderer::GetRendererInstance()->addVertexFormat(D3DFVF_LVERTEX1Fmt, elementsOf(D3DFVF_LVERTEX1Fmt), m_pEffect);

	inited=true;
}

//-----------------------------------------------------------------------------

// Name: render()

// Desc: Draws the scene

//-----------------------------------------------------------------------------



void LitVertexVS2PS::SetTexture( TextureID pTexture )
{
	//LOG_PRINT("LitVertexVS2PS::SetTexture:setTexture:TexID=%d\n", pTexture);
	if(pTexture > -1)
	{
		IRenderer::GetRendererInstance()->setTexture("Sampler", pTexture);
	}
}



void LitVertexVS2PS::Begin( bool aSky, D3DXFROMWINEMATRIX* aM, TextureID pTexture )
{
	//LOG_FNLN;
	//LOG_PRINT("LitVertexVS2PS::Begin:setShader:m_pEffect=%x\n", m_pEffect);
	IRenderer::GetRendererInstance()->setShader( m_pEffect );
IRenderer::GetRendererInstance()->setVertexFormat(LitVertexVS2PS::LVERTEX1VF);
if(aM)
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", *aM);
else{
	D3DXFROMWINEMATRIX matWorld=IRenderer::GetRendererInstance()->Getww();
	D3DXFROMWINEMATRIX matView=IRenderer::GetRendererInstance()->Getvv();
	D3DXFROMWINEMATRIX matProj=IRenderer::GetRendererInstance()->Getpp();
  D3DXFROMWINEMATRIX matVP = matWorld*(matView) * (matProj);
  D3DXFROMWINEMatrixTranspose(&matVP, &matVP);
  IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matVP);
}
	if(pTexture > -1)
	{
		IRenderer::GetRendererInstance()->setTexture("Sampler", pTexture);
	}
}



