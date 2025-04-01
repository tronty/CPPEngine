/*
  Copyright (c) 2022 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>
#include <FW3.h>

class _Shape3D: public STXGUI
{
	ShaderID shd;
	VertexFormatID vf;
	MeshRenderer2 shape3D;
public:
int init(const char* aTitle)
{
        shape3D.CreateSphere(1.0f, -1);
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Ashli_Viewer_1.7.0/venus.hlsl", "main", "main");
	FormatDesc fmt[] = { 
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 };
	vf = IRenderer::GetRendererInstance()->addVertexFormat(fmt, elementsOf(fmt), shd);
	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=-90.0f;
	float mAngleY= 90.0f;
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX matRot;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            0.0f,
		                            D3DXFROMWINEToRadian(mAngleY)
					);
	shape3D.BeginDraw(0);
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot );
	shape3D.EndDraw();

		const char* txt = "Use mouse buttons to rotate the model.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		txt = "CTRL-r resets the scene.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		char txt2[128];
		stx_snprintf(txt2, 128, "mAngleX=%f\n", mAngleX);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 70, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "mAngleY=%f\n", mAngleY);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 90, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
virtual void actionPerformed(GUIEvent &evt)
{
}
};
int ApplicationLogic()
{	
	IRenderer* r=IRenderer::GetRendererInstance("Venus");	
	IInput*    i=STX_Service::GetInputInstance();
	_Shape3D shape3D;
	shape3D.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		shape3D.render();
	}
	return 0;
}

