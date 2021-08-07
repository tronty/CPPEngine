/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2

MeshRenderer2 meshCube3D;
MeshRenderer2 meshCantorDust3D;
TextureID texID=-1;
TextureID mortarjointTexID=-1;
TextureID redbrickTexID=-1;

	std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
	std::vector<D3DXFROMWINEVECTOR3> g_normals3;
	std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
	std::vector<__WORD__> g_indices1;
	std::vector<__WORD__> g_indices2;
	D3DXFROMWINEVECTOR3 g_color3_;
	D3DXFROMWINEVECTOR4 g_color4_;

void CubeToArrays(D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR3 aSize, D3DXFROMWINEVECTOR3 aAngle)
{
#if 0
	LOG_FNLN;
	LOG_PRINT("aPosition.x=%f\naPosition.y=%f\naPosition.z=%f\n", aPosition.x, aPosition.y, aPosition.z);
	LOG_PRINT("aSize.x=%f\naSize.y=%f\naSize.z=%f\n", aSize.x, aSize.y, aSize.z);
	LOG_PRINT("aAngle.x=%f\naAngle.y=%f\naAngle.z=%f\n", aAngle.x, aAngle.y, aAngle.z);
#endif
	D3DXFROMWINEMATRIX R, S, T, W;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, aAngle.x, aAngle.y, aAngle.z);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, 0.0f, 0.0f, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, aSize.x, aSize.y, aSize.z);
	D3DXFROMWINEMatrixTranslation(&T, aPosition.x, aPosition.y, aPosition.z);
	W=R*S*T;
	if(meshCube3D.indices.size())
	for(unsigned int j=0;j<meshCube3D.indices.size();j++)
	{
		stx_VertexPositionNormalTexture v=meshCube3D.vertices[meshCube3D.indices[j]];
		D3DXFROMWINEVec3TransformCoord(&v.Position, &v.Position, &W);
		meshCantorDust3D.vertices.push_back(v);
		meshCantorDust3D.indices.push_back(meshCantorDust3D.vertices.size()-1);
	}
	else
	for(unsigned int j=0;j<meshCube3D.vertices.size();j++)
	{
		stx_VertexPositionNormalTexture v=meshCube3D.vertices[j];
		D3DXFROMWINEVec3TransformCoord(&v.Position, &v.Position, &W);
		meshCantorDust3D.vertices.push_back(v);
	}
}

int init(const char* aTitle)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();	
	mortarjointTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/mortarjoint.png", false, ss); 
	redbrickTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/redbrick.png", false, ss);
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);

	float awidth=1.0f;
	float aheight=1.0f;
	float adepth=1.0f;
	//tShader aShader=eShaderNone;
	meshCube3D.CreateTexturedBox(awidth, aheight, adepth, defaultshader);
	meshCantorDust3D.CreateTexturedBox(awidth, aheight, adepth, defaultshader);
	
	meshCantorDust3D.vertices.clear();
	meshCantorDust3D.indices.clear();

	D3DXFROMWINEVECTOR3 aAngle(0.0f, 0.0f, 0.0f);
	for(unsigned int i=0;i<4;i++)
	{
	for(unsigned int j=0;j<5;j++)
	{
		{
			D3DXFROMWINEVECTOR3 p(i*(1.0f+0.05f), j*(0.5f+0.05f), 0.0f);
			D3DXFROMWINEVECTOR3 s(1.0f, 0.5f, 1.0f);
			CubeToArrays(p, s, aAngle);

		}
		{
			D3DXFROMWINEVECTOR3 p(0.0f+i*1.05f, -0.05f+j*0.55f, 0.0f);
			D3DXFROMWINEVECTOR3 s(1.05f, 0.05f, 1.0f);
			CubeToArrays(p, s, aAngle);
		}
		{
			D3DXFROMWINEVECTOR3 p(-0.05f+i*1.05f, -0.05f+j*0.55f, 0.0f);
			D3DXFROMWINEVECTOR3 s(0.05f, 0.55f, 1.0f);
			CubeToArrays(p, s, aAngle);
		}
	}
	}

	LOG_PRINT("meshCube3D.vertices.size()=%d\n", meshCube3D.vertices.size());
	LOG_PRINT("meshCube3D.indices.size()=%d\n", meshCube3D.indices.size());
	LOG_PRINT("meshCantorDust3D.vertices.size()=%d\n", meshCantorDust3D.vertices.size());
	LOG_PRINT("meshCantorDust3D.indices.size()=%d\n", meshCantorDust3D.indices.size());
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

	D3DXFROMWINEMATRIX R, S, T;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
	//D3DXFROMWINEMatrixScaling(&S, 1.0f, 1.0f, 1.0f);
	D3DXFROMWINEMatrixTranslation(&T, 0.0f, -0.5f, 0.0f);
	D3DXFROMWINEMATRIX W=R*S*T;
	//meshCantorDust3D.Draw(W, texID);

	IRenderer::GetRendererInstance()->setShader(meshCantorDust3D.Shader);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", W);
	

		D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
		D3DXFROMWINEVECTOR4 AmbientColor=silver;
		D3DXFROMWINEVECTOR4 DiffuseColor=0.1f*silver;
	D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);
        
		IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
		IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", DiffuseColor);
		IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", LightDir);
		IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", EyeDir);

		IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
	IRenderer::GetRendererInstance()->setVertexFormat(meshCantorDust3D.VertexDeclaration);

	if(meshCantorDust3D.indices.size())
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, meshCantorDust3D.vertices.size(), meshCantorDust3D.indices.size()/3, &meshCantorDust3D.indices[0], &meshCantorDust3D.indices[0], CONSTANT_INDEX2, &meshCantorDust3D.vertices[0], &meshCantorDust3D.vertices[0], sizeof(stx_VertexPositionNormalTexture));
	else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, meshCantorDust3D.vertices.size()/3, &meshCantorDust3D.vertices[0], &meshCantorDust3D.vertices[0], sizeof(stx_VertexPositionNormalTexture));

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("SimpleTiling3D");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
/*
		   	   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
*/
/*
		  	   x	  y     tx    ty
	float v[16] =  { -1.0f, -1.0f, 0.0f, 1.0f,  
			  1.0f, -1.0f, 1.0f, 1.0f,   
			  1.0f,  1.0f, 1.0f, 0.0f,  
			 -1.0f,  1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 2, &v[0], &v[0], 4*sizeof(float));
*/

