/*****************************************************************************************
 * File: Global.cpp
 * Desc: Global class
 *****************************************************************************************/
#include <CIndieLib.h>

//include "..\libheaders\CIndieLib.h"
#include <Defines.h>
DebugApi *Debug = new DebugApi;
int bbb=0;
int mmm=0;

VertexFormatID D3DFVF_CUSTOMVERTEX2D=-1;
ShaderID D3DSHA_CUSTOMVERTEX2D=-1;
VertexFormatID D3DFVF_CUSTOMVERTEX2DW=-1;
ShaderID D3DSHA_CUSTOMVERTEX2DW=-1;
VertexFormatID D3DFVF_PIXEL=-1;
ShaderID D3DSHA_PIXEL=-1;

DebugApi * GetDebugAPI(){return Debug;}
int Getbbb(){return bbb;}
int Getmmm(){return mmm;}
VertexFormatID& GetCUSTOMVertexFormat2DW(){return D3DFVF_CUSTOMVERTEX2DW;}
ShaderID& GetCUSTOMShader2DW(){return D3DSHA_CUSTOMVERTEX2DW;}
VertexFormatID& GetCUSTOMVertexFormat2D(){return D3DFVF_CUSTOMVERTEX2D;}
ShaderID& GetCUSTOMShader2D(){return D3DSHA_CUSTOMVERTEX2D;}
VertexFormatID& GetPIXELVertexFormat(){return D3DFVF_PIXEL;}
ShaderID& GetPIXELShader(){return D3DSHA_PIXEL;}

void InitPixel(ShaderID& s, VertexFormatID& vf)
{
	if(vf==-1)
	{
		std::string texSHD = IndieLibShadersFactory::GetShader("texSHD1");
		s = IRenderer::GetRendererInstance()->addShader(texSHD.c_str(), "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		}
		;
		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), s);
	}
}
void InitCUSTOMVERTEX2D(ShaderID& s, VertexFormatID& vf)
{

	if(vf==-1)
	{
		std::string texSHD = IndieLibShadersFactory::GetShader("texSHD2");
		s = IRenderer::GetRendererInstance()->addShader(texSHD.c_str(), "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		}
		;

		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), s);

	}

}


void InitCUSTOMVERTEX2DW(ShaderID& s, VertexFormatID& vf)
{
	if(vf==-1)
	{
		std::string texSHD = IndieLibShadersFactory::GetShader("texSHD3");
		s = IRenderer::GetRendererInstance()->addShader(texSHD.c_str(), "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		}
		;
		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), s);
	}
}

D3DXFROMWINEMATRIX GetMVP()
{
	/*
	if(IRenderer::GetInputInstance()->OnKeyPress (IND_M)) mmm++;
	if(mmm==6)mmm=0;
	if(IRenderer::GetInputInstance()->OnKeyPress (IND_B)) bbb++;
	if(bbb==2)bbb=0;
*/
		D3DXFROMWINEMATRIX mvp;
#if 1
	D3DXFROMWINEMATRIX matWorld=CIndieLib::Instance()->Render->mInfo.mDevice->Getww();
	D3DXFROMWINEMATRIX matView=CIndieLib::Instance()->Render->mInfo.mDevice->Getvv();
	D3DXFROMWINEMATRIX matProj=CIndieLib::Instance()->Render->mInfo.mDevice->Getpp();
  mvp = matWorld*(matView) * (matProj);
  D3DXFROMWINEMatrixTranspose(&mvp, &mvp);
//  m_pDevice->setShaderConstant4x4f("WorldViewProjection", matVP);
#else
	switch(mmm)
	{
#if 0
	case 0:
	mvp=//CIndieLib::Instance()->Render->mInfo.mDevice->Getww()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getvv()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getpp();
	break;
	case 1:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getpp()
		*CIndieLib::Instance()->Render->mInfo.mDevice->Getvv();
		//*CIndieLib::Instance()->Render->mInfo.mDevice->Getww();
	break;
	case 2:
	mvp=
		//CIndieLib::Instance()->Render->mInfo.mDevice->Getww()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getpp()
		*CIndieLib::Instance()->Render->mInfo.mDevice->Getvv();
	break;
	case 3:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getvv()*
		//CIndieLib::Instance()->Render->mInfo.mDevice->Getww()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getpp();
	break;

	case 4:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getvv()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getpp();
		//*CIndieLib::Instance()->Render->mInfo.mDevice->Getww();
	break;
	case 5:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getpp()*
		//CIndieLib::Instance()->Render->mInfo.mDevice->Getww()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getvv();
	break;
#else
	case 0:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getww()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getvv()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getpp();
	break;
	case 1:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getpp()
		*CIndieLib::Instance()->Render->mInfo.mDevice->Getvv()
		*CIndieLib::Instance()->Render->mInfo.mDevice->Getww();
	break;
	case 2:
	mvp=
		CIndieLib::Instance()->Render->mInfo.mDevice->Getww()
		*CIndieLib::Instance()->Render->mInfo.mDevice->Getpp()
		*CIndieLib::Instance()->Render->mInfo.mDevice->Getvv();
	break;
	case 3:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getvv()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getww()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getpp();
	break;

	case 4:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getvv()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getpp()
		*CIndieLib::Instance()->Render->mInfo.mDevice->Getww();
	break;
	case 5:
	mvp=CIndieLib::Instance()->Render->mInfo.mDevice->Getpp()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getww()*
		CIndieLib::Instance()->Render->mInfo.mDevice->Getvv();
	break;
#endif
	};
#endif
	return mvp;
	}

