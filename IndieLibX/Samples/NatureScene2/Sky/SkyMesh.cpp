/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

struct SkyMesh {
public:
	SkyMesh(){}
	virtual ~SkyMesh(){}
	MeshRenderer2 shape3D;
    	TextureID skyTexture;
	ShaderID skyShader;
    	VertexFormatID skyVF;
	virtual void init(tShader aShader=eShaderNone)
	{
  //skyTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Sky/clouds.png", false, IRenderer::GetRendererInstance()->Getlinear());
  skyTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/RadeonTerrainDemo/Sky1.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
	//skyTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
  skyShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/SkyShader2.shd", "main", "main");
	{FormatDesc format[] =
	{
					0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
					0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
					0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
					0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	skyVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), skyShader);}
		
        	shape3D.CreateHalfSphere(1.0f, eShaderNone);
	//LOG_FNLN;
	//LOG_PRINT("vertices.size()=%d\n", shape3D.vertices.size());
	//LOG_PRINT("indices.size()=%d\n", shape3D.indices.size());
	}
	virtual unsigned int  render(D3DXFROMWINEMATRIX* mvp=0)
	{
	//LOG_FNLN;

  IRenderer::GetRendererInstance()->setShader(skyShader);
      IRenderer::GetRendererInstance()->setVertexFormat(skyVF);

	float    skyTimer    = 0.001f*timeGetTime();

  IRenderer::GetRendererInstance()->setTexture("noise", skyTexture);

#if 1
float4  params=float4(-0.167888, -0.035063, 0.930527, skyTimer);
float3  offset=float3(2560.000000, 2560.000000, -5100.000000);
float4 skyInfo=float4(0.25, 0.5, 1.0, 0.0);
#elif 0
float4  params=float4(0.000000, 0.000000, 1.000333, skyTimer);
float3  offset=float3(324.739990, 193.405914, -2237.400879);
float4 skyInfo=float4(0.25, 0.5, 1.0, 0.0);
#elif 0
float4  params=float4(0.0, 0.0, 0.0, skyTimer);
float3  offset=float3(0.0, 0.0, 0.0);
float4 skyInfo=float4(0.0, 0.0, 0.0, 0.0);
#endif
		IRenderer::GetRendererInstance()->setShaderConstant4f("params", params);
		IRenderer::GetRendererInstance()->setShaderConstant3f("offset", offset);
		IRenderer::GetRendererInstance()->setShaderConstant4f("skyInfo", skyInfo);


	float AngleX=0.0f;
	float AngleY=0.0f;
		D3DXFROMWINEMATRIX w;
		#if 1
		if(1)
		{
			static float mAngleX=0.0f;
			static float mAngleY=0.0f;

		stx_GetAngles(mAngleX, mAngleY);

  			if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F5)) mAngleX+=90.0f;
  			if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F6)) mAngleY+=90.0f;
			float s=5100.0f;
			//s=1.0f;
			D3DXFROMWINEMATRIX W,R,S,T;
			D3DXFROMWINEMatrixRotationYawPitchRoll(&R, D3DXFROMWINEToRadian(mAngleX), D3DXFROMWINEToRadian(mAngleY), 0.0f); 
			D3DXFROMWINEMatrixScaling(&S, s, s, s);
			D3DXFROMWINEVECTOR3 position(0.0f, 0.0f, s);
			D3DXFROMWINEMatrixTranslation(&T, position.x, position.y, position.z);
			W=R*S*T;
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		}
		else if(0)
		{
		D3DXFROMWINEMATRIX I;
		D3DXFROMWINEMatrixIdentity(&I);

	stx_GetAngles(AngleX, AngleY);


		D3DXFROMWINEMatrixRotationYawPitchRoll( &w,
		                            D3DXFROMWINEToRadian(AngleX),
		                            D3DXFROMWINEToRadian(AngleY),
		                            0.0f );

		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", w);
			//stx_Dump4x4f(*mvp, "MVP");
		}
		else if(0)//(mvp)
		{
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", *mvp);
			//stx_Dump4x4f(*mvp, "MVP");
		}
		else if(0)
		{
		D3DXFROMWINEMATRIX W, R, S;
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R, D3DXFROMWINEToRadian(180.0f), D3DXFROMWINEToRadian(180.0f), 0.0f);
		D3DXFROMWINEMatrixScaling(&S, 2.0f, 1.5f, 2.0f);
		W=R*S;
			//stx_Dump4x4f(S, "S");
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		}

	#if 1
	const uint flags=RESET_ALL;
	#else
	const uint flags=RESET_VB|RESET_IB;
	#endif
		if(shape3D.indices.size())
			IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, shape3D.vertices.size(), shape3D.indices.size()/3, &shape3D.indices[0], &shape3D.indices[0], CONSTANT_INDEX2, &shape3D.vertices[0], &shape3D.vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
		else if(shape3D.vertices.size())
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, shape3D.vertices.size()/3, &shape3D.vertices[0], &shape3D.vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
		else
			shape3D.render(&w);
//LOG_FNLN;
		#else
		shape3D.render(&w);
//LOG_FNLN;
		#endif

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
		stx_snprintf(txt2, 128, "AngleX=%f\n", AngleX);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 70, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		stx_snprintf(txt2, 128, "AngleY=%f\n", AngleY);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 90, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	return shape3D.indices.size()/3;
	}
};

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("NatureSceneSky");	
	IInput*    i=STX_Service::GetInputInstance();
	SkyMesh skyMesh;
	skyMesh.init();
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	//LOG_PRINT("1:Clear\n");
	IRenderer::GetRendererInstance()->BeginScene();
	//LOG_PRINT("1:BeginScene\n");
		i->Update();
		skyMesh.render();

	IRenderer::GetRendererInstance()->EndScene();
	//LOG_PRINT("1:EndScene\n");
	IRenderer::GetRendererInstance()->Present( );
	//LOG_PRINT("1:Present\n");
	}
	return 0;
}

