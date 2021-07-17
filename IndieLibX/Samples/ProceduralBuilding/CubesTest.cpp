/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#define __PREGENERATE__ 1

#include <Framework3/IRenderer.h>




#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
#define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2

MeshRenderer2 meshCube3D;
MeshRenderer2 meshCantorDust3D;
TextureID texID=-1;

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
	unsigned int k=meshCube3D.indices.size();
	if(k)
	{
	for(unsigned int j=0;j<meshCube3D.indices.size();j++)
	{
		stx_VertexPositionNormalTexture v=meshCube3D.vertices[meshCube3D.indices[j]];
		D3DXFROMWINEVec3TransformCoord(&v.Position, &v.Position, &W);
		//LOG_FNLN;
		//LOG_PRINT("v.Position.x=%f\nv.Position.y=%f\nv.Position.z=%f\n", v.Position.x, v.Position.y, v.Position.z);
		meshCantorDust3D.vertices.push_back(v);
		meshCantorDust3D.indices.push_back(meshCube3D.indices[j]);
	}}
	else
	for(unsigned int j=0;j<meshCube3D.vertices.size();j++)
	{
		stx_VertexPositionNormalTexture v=meshCube3D.vertices[j];
		D3DXFROMWINEVec3TransformCoord(&v.Position, &v.Position, &W);
		//LOG_FNLN;
		//LOG_PRINT("v.Position.x=%f\nv.Position.y=%f\nv.Position.z=%f\n", v.Position.x, v.Position.y, v.Position.z);
		meshCantorDust3D.vertices.push_back(v);
	}
}

int init(const char* aTitle)
{
#if 0
	LuaScript script;
	bool r=script.Load("/CantorDust3D.lua");
	if(!r){
		Niterations = int(script.get<float>("Niterations"));
		epsilon=script.get<float>("epsilon");
		dy=script.get<float>("dy");
	}
#elif 0
	TiXmlDocument* document;
	TiXmlHandle* docHandle;
	TiXmlHandle* BodyHandle;
	std::string fn=stx_convertpath("/CantorDust3D.xml");
	LOG_PRINT("fn.c_str()=%s\n", fn.c_str());

	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	TiXmlHandle BodyHandle = docHandle.FirstChild("Body");
	TiXmlElement* BodyElement = BodyHandle.Element();
	if(BodyElement) {LOG_PRINT("!BodyElement\n");stx_exit(0);}

	Niterations = atoi(BodyHandle.FirstChild("Niterations").Element()->Attribute("Value"));
	epsilon = atof(BodyHandle.FirstChild("epsilon").Element()->Attribute("Value"));
	dy = atof(BodyHandle.FirstChild("dy").Element()->Attribute("Value"));
#endif
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);

	float awidth=1.0f;
	float aheight=1.0f;
	float adepth=1.0f;
	//tShader aShader=eShaderNone;
	meshCube3D.CreateTexturedBox(awidth, aheight, adepth, defaultshader);
	meshCantorDust3D.CreateTexturedBox(awidth, aheight, adepth, defaultshader);
	#ifdef __PREGENERATE__
	meshCantorDust3D.vertices.clear();
	meshCantorDust3D.indices.clear();
	D3DXFROMWINEVECTOR3 aSize(1.0f, 1.0f, 1.0f);
	D3DXFROMWINEVECTOR3 aAngle(0.0f, 0.0f, 0.0f);
	D3DXFROMWINEVECTOR3 aPosition;
	float f=1.0f;
	float p=2.5f;
	aPosition=D3DXFROMWINEVECTOR3(-1.0f*f-p, -1.0f*f-p, 0.0f);
	CubeToArrays(aPosition, aSize, aAngle);
	aPosition=D3DXFROMWINEVECTOR3(-1.0f*f-p,  1.0f*f+p, 0.0f);
	CubeToArrays(aPosition, aSize, aAngle);
	aPosition=D3DXFROMWINEVECTOR3( 1.0f*f+p, -1.0f*f-p, 0.0f);
	CubeToArrays(aPosition, aSize, aAngle);
	aPosition=D3DXFROMWINEVECTOR3( 1.0f*f+p,  1.0f*f+p, 0.0f);
	CubeToArrays(aPosition, aSize, aAngle);
	#endif

	LOG_PRINT("meshCube3D.vertices.size()=%d\n", meshCube3D.vertices.size());
	LOG_PRINT("meshCube3D.indices.size()=%d\n", meshCube3D.indices.size());
	LOG_PRINT("meshCantorDust3D.vertices.size()=%d\n", meshCantorDust3D.vertices.size());
	LOG_PRINT("meshCantorDust3D.indices.size()=%d\n", meshCantorDust3D.indices.size());
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

	D3DXFROMWINEMATRIX R, S, T;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, 0.25f, 0.25f, 0.25f);
	//D3DXFROMWINEMatrixScaling(&S, 1.0f, 1.0f, 1.0f);
	D3DXFROMWINEMatrixTranslation(&T, 0.0f, 0.0f, 0.0f);
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

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("CubesTest");
	IInput* in=STX_Service::GetInputInstance();
	init("");
	while (!in->OnKeyPress (STX_KEY_ESCAPE) && !in->Quit())
	{
		in->Update();
		render();
	}
	return 0;
}

