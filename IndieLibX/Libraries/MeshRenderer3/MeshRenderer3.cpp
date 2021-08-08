/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include "MeshRenderer3.h"

//define //STX_PRINTF
#define LOG_PRINT3 LOG_PRINT

/*
MeshRenderer3
{
	struct Animator
	{
		Animator():mRootNode(0), mScene(0){}
		void SetAnimIndex(unsigned int pAnimIndex){}
		unsigned int CurrentAnimIndex(){return 0;}
		int GetNumAnimations(){return 0;}
		void* mRootNode;
		void* mScene;
	};
	struct MeshHelper
	{
		MeshHelper(){}
		~MeshHelper(){}
		Animator* mAnimator;
		MeshHelper** apcMeshes;
	};
*/

ShaderID MeshRenderer3::AmbientShader=-1;
ShaderID MeshRenderer3::DiffuseShader=-1;
ShaderID MeshRenderer3::PhongShader1=-1;
ShaderID MeshRenderer3::PhongShader2=-1;
ShaderID MeshRenderer3::Shader=-1;
VertexFormatID MeshRenderer3::AmbientVertexDeclaration=-1;
VertexFormatID MeshRenderer3::DiffuseVertexDeclaration=-1;
VertexFormatID MeshRenderer3::PhongVertexDeclaration1=-1;
VertexFormatID MeshRenderer3::PhongVertexDeclaration2=-1;
VertexFormatID MeshRenderer3::VertexDeclaration=-1;

ShaderID MeshRenderer3::SimpleShader=-1;
VertexFormatID MeshRenderer3::SimpleVertexDeclaration=-1;
ShaderID MeshRenderer3::OutlineShader=-1;
VertexFormatID MeshRenderer3::OutlineVertexDeclaration=-1;
ShaderID MeshRenderer3::ShadingShader=-1;
VertexFormatID MeshRenderer3::ShadingVertexDeclaration=-1;
ShaderID MeshRenderer3::CelShader=-1;
VertexFormatID MeshRenderer3::CelVertexDeclaration=-1;
ShaderID MeshRenderer3::CelShader1=-1;
VertexFormatID MeshRenderer3::CelVertexDeclaration1=-1;
ShaderID MeshRenderer3::CelShader2=-1;
VertexFormatID MeshRenderer3::CelVertexDeclaration2=-1;
ShaderID MeshRenderer3::CelShader3=-1;
VertexFormatID MeshRenderer3::CelVertexDeclaration3=-1;
ShaderID MeshRenderer3::CelShader4=-1;
VertexFormatID MeshRenderer3::CelVertexDeclaration4=-1;

ShaderID MeshRenderer3::DirectionalShader=-1;
VertexFormatID MeshRenderer3::DirectionalVertexDeclaration=-1;
ShaderID MeshRenderer3::SpotShader=-1;
VertexFormatID MeshRenderer3::SpotVertexDeclaration=-1;
ShaderID MeshRenderer3::MultiShader=-1;
VertexFormatID MeshRenderer3::MultiVertexDeclaration=-1;

ShaderID MeshRenderer3::SnowShader=-1;
VertexFormatID MeshRenderer3::SnowVertexDeclaration=-1;
TextureID MeshRenderer3::ExposureDepthMapTexture=-1;
TextureID MeshRenderer3::ShadingTexture=-1;
TextureID MeshRenderer3::CelShade=-1;

int MeshRenderer3::InitShader(tShader aShader)
{
	//if(Shader==-1)
	{

		tShader aShader=aShader_;
	std::string fn=stx_convertpath("/Framework3/Framework3.xml");
    TiXmlDocument doc(fn.c_str());
    if (!doc.LoadFile())
	{//LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
    TiXmlHandle docHandle(&doc);
    std::string Shader3DStr = docHandle.FirstChild("Body").FirstChild("Shader3D").Element()->Attribute("Value");
	LOG_PRINT("Shader3DStr=%s\n", Shader3DStr.c_str());

			if(Shader3DStr=="SimpleShader"){aShader=eSimpleShader;}
			if(Shader3DStr=="AmbientLighting"){aShader=eAmbientLighting;}
			if(Shader3DStr=="DiffuseLighting"){aShader=eDiffuseLighting;}
			if(Shader3DStr=="PhongLighting1"){aShader=ePhongLighting1;}
			if(Shader3DStr=="PhongLighting2"){aShader=ePhongLighting2;}
			if(Shader3DStr=="DirectionaLighting"){aShader=eDirectionalLighting;}
			if(Shader3DStr=="SpotLighting"){aShader=eSpotLighting;}
			if(Shader3DStr=="MultiLighting"){aShader=eMultiLighting;}
			if(Shader3DStr=="SnowShader"){aShader=eSnowShader;}
			if(Shader3DStr=="CelShader"){aShader=eCelShader;}
			if(Shader3DStr=="CelShader1"){aShader=eCelShader1;}
			if(Shader3DStr=="CelShader2"){aShader=eCelShader2;}
			if(Shader3DStr=="CelShader3"){aShader=eCelShader3;}
			if(Shader3DStr=="CelShader4"){aShader=eCelShader4;}

		mShader=aShader;
		FormatDesc format[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
					};

		const char* mainVS="main";
		const char* mainFS="main";
		if(aShader==eSimpleShader2)
			mainFS="main2";

		m_v4AmbientColor=D3DXFROMWINEVECTOR4(0.1f, 0.0f, 0.0f, 1.0f);
		m_v4DiffuseColor=D3DXFROMWINEVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		m_v4LightDir=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		m_v4EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);		

		FormatDesc lineformat[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 3
					};
		

		if((aShader==eCelShader)||(aShader==eCelShader3))
		{
			if(CelShade==-1)
				CelShade=IRenderer::GetRendererInstance()->addImageLibTexture("/CelShading/celshade.png", false, IRenderer::GetRendererInstance()->Getlinear());
		}

		switch(aShader)
		{
			case eSimpleShader:
				if(SimpleShader==-1)
				{
					SimpleShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/rest.HLSL.shd", mainVS, mainFS);
					SimpleVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SimpleShader);
				}
				break;
			case eAmbientLighting:
				if(AmbientShader==-1)
				{
					////{LOG_FNLN;}
					AmbientShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/AmbientLighting.HLSL.shd", mainVS, mainFS);
					AmbientVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), AmbientShader);
				}
				break;
			case eDiffuseLighting:
				if(DiffuseShader==-1)
				{
					////{LOG_FNLN;}
					DiffuseShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/DiffuseLighting.HLSL.shd", mainVS, mainFS);
					DiffuseVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), DiffuseShader);
				}
				break;
			case ePhongLighting1:
				if(PhongShader1==-1)
				{
					////{LOG_FNLN;}
					PhongShader1 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/PhongLighting1.HLSL.shd", mainVS, mainFS);
					PhongVertexDeclaration1 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), PhongShader1);
				}
				break;
			case ePhongLighting2:
				if(PhongShader2==-1)
				{
					////{LOG_FNLN;}
					PhongShader2 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/PhongLighting2.HLSL.shd", mainVS, mainFS);
					PhongVertexDeclaration2 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), PhongShader2);
				}
				break;
			case eDirectionalLighting:
				if(DirectionalShader==-1)
				{
					DirectionalShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/DirectionalLighting.HLSL.shd", mainVS, mainFS);
					DirectionalVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), DirectionalShader);
				}
				break;
			case eSpotLighting:
				if(SpotShader==-1)
				{
					SpotShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/SpotLighting.HLSL.shd", mainVS, mainFS);
					SpotVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SpotShader);
				}
				break;
			case eMultiLighting:
				if(DirectionalShader==-1)
				{
					MultiShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/MultiLighting.HLSL.shd", mainVS, mainFS);
					MultiVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), MultiShader);
				}
				break;
			#if 1
			case eSnowShader:
				if(SnowShader==-1)
				{
					SnowShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/SnowAccumulation/Snow.shd", "RenderSceneVS_SM30", "RenderScenePS");
					//SnowShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/SnowAccumulation/Snow.shd", "RenderSceneVS_NoSnow", "RenderScenePS_NoSnow");
					SnowVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SnowShader);
					ExposureDepthMapTexture=-1;//IRenderer::GetRendererInstance()->addImageLibTexture("/", false, IRenderer::GetRendererInstance()->Getlinear());
				}
				break;
			case eCelShader:
				if(OutlineShader==-1)
				{
					OutlineShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/NVIDIA/outline.shd", "main", "main");
					OutlineVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(lineformat, elementsOf(lineformat), OutlineShader);
					ShadingShader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/NVIDIA/shading.shd", "main", "main");
					ShadingVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(lineformat, elementsOf(lineformat), ShadingShader);
					ShadingTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/CelShading/NVIDIA/celshade.png", false, IRenderer::GetRendererInstance()->Getlinear());
				}
				break;
			case eCelShader1:
				if(CelShader1==-1)
				{
					CelShader1 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/x/CelShader.shd", "main", "main");
					CelVertexDeclaration1 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), CelShader1);
				}
				break;
			case eCelShader2:
				if(CelShader2==-1)
				{
					CelShader2 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/x/CelShader2.shd", "main", "main");
					CelVertexDeclaration2 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), CelShader2);
				}
				break;
			case eCelShader3:
				if(CelShader3==-1)
				{
					//CelShader3 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/x/Toon.fx", "OutlineVertexShader", "OutlinePixelShader");
					CelShader3 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/x/Toon.fx", "CelVertexShader", "CelPixelShader");
					CelVertexDeclaration3 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), CelShader3);
					ShadingTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/CelShading/NVIDIA/celshade.png", false, IRenderer::GetRendererInstance()->Getlinear());
				}
				break;
			case eCelShader4:
				if(CelShader4==-1)
				{
					//CelShader4 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/x/ToonColored.fx", "OutlineVertexShader", "OutlinePixelShader");
					CelShader4 = IRenderer::GetRendererInstance()->addShaderFromFile(	"/CelShading/x/ToonColored.fx", "CelVertexShader", "CelPixelShader");
					CelVertexDeclaration4 = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), CelShader4);
				}
				break;
			#endif
			default:
				if(Shader==-1)
				{
					////{LOG_FNLN;}
					Shader = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer3/rest.HLSL.shd", mainVS, mainFS);
					VertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), Shader);
				}
				break;
		}
	}
	return 0;
}

MeshRenderer3::MeshRenderer3()
{
		_max=D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
		_min=D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
		texID = -1;
		Shader = -1;
            VertexDeclaration = -1;
	m_bMeshRenderer3Simpler_ProceduralBuilding=false;
	j_tmp=-1;
	z_tmp=-1;
	tex_tmp=-1;
}
MeshRenderer3::~MeshRenderer3(){}
int MeshRenderer3::Load(const char* aFilename, tShader aShader, bool scaleasset, bool a_bMeshRenderer3Simpler_ProceduralBuilding)
{
	return LoadFile(aFilename, aShader, scaleasset, a_bMeshRenderer3Simpler_ProceduralBuilding);
}
int MeshRenderer3::LoadFile(const char* aFilename_, tShader aShader_, bool scaleasset, bool a_bMeshRenderer3Simpler_ProceduralBuilding)
{
		Clear();
	m_bMeshRenderer3Simpler_ProceduralBuilding=a_bMeshRenderer3Simpler_ProceduralBuilding;
	//STX_PRINTF("sizeof(__WORD__)=%d\n", sizeof(__WORD__));
	//STX_PRINTF("sizeof(__DWORD__)=%d\n", sizeof(__DWORD__));
	//STX_PRINTF("sizeof(unsigned int)=%d\n", sizeof(unsigned int));

	if(std::string(aFilename_)=="/Lightning/scene.X")
		return 0;
	//if(std::string(aFilename_)=="/Smoke/Gargoyle/gargoyle.x")
	//	return 0;
	if(std::string(aFilename_)=="/DXJune2010/Dwarf/DwarfWithEffectInstance.x")
		return 0;
	#if 0
	if(std::string(aFilename_)=="/Orcs/Bonfire_wOrcs.x")
		return 0;
	if(std::string(aFilename_)=="/NatureScene/Water/watermesh.3ds")
		return 0;
	if(std::string(aFilename_)=="/blender_3ds/cube.3ds")
		return 0;
	if(std::string(aFilename_)=="/blender_3ds/cup/cup.obj")
		return 0;
	if(std::string(aFilename_)=="/PM/P-TankHistory/allied/t34_76/t34ds/T34-76.3DS")
		return 0;
	#endif

	meshes.clear();

	std::string fn=stx_convertpath(aFilename_);
	const char* aFilename=fn.c_str();

	InitShader(aShader_);
	m_sFileName=aFilename;

	unsigned int found = m_sFileName.find_last_of("/\\");	
	if (found != string::npos)
  		m_szPathTex=m_sFileName.substr(0,found);

	#if 0
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "sdkmesh")
		LoadSDKMESHFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "SDKMESH")
		LoadSDKMESHFile(aFilename);
	#endif
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "3ds")
		Load3DSFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "3DS")
		Load3DSFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "obj")
		LoadOBJFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "OBJ")
		LoadOBJFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "dae")
		LoadDAEFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "DAE")
		LoadDAEFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "x")
		LoadXFile(aFilename);
  	if(m_sFileName.substr(m_sFileName.find_last_of(".") + 1) == "X")
		LoadXFile(aFilename);
	if(scaleasset)
		ScaleAsset();	
	for(unsigned int i=0;i<meshes.size();i++)
	{
		stx_NormalizeMesh(meshes[i]);
		if(aShader_==eCelShader)
			meshes[i].initNormals();
	}

	//initrindices();
	
	for(unsigned int i=0;i<meshes.size();i++)
		meshes[i].indices.clear();

	#if 1
	//LOG_FNLN;
	//STX_PRINTF("fn=%s\n", aFilename);
	//STX_PRINTF("meshes.size()=%d\n", meshes.size());
	for(unsigned int j=0;j<meshes.size();j++)
	{
		//STX_PRINTF("meshes[%d].name=%s\n", j, meshes[j].m_sName.c_str());
		//STX_PRINTF("meshes[%d].vertices.size()=%d\n", j, meshes[j].vertices.size());
		//STX_PRINTF("meshes[%d].indices.size()=%d\n", j, meshes[j].indices.size());
		//STX_PRINTF("meshes[%d].faces.size()=%d\n", j, meshes[j].faces.size());
		//STX_PRINTF("meshes[%d].textures.size()=%d\n", j, meshes[j].textures.size());
		//STX_PRINTF("meshes[%d].textures_.size()=%d\n", j, meshes[j].textures_.size());
		//STX_PRINTF("meshes[%d].textureNames.size()=%d\n", j, meshes[j].textureNames.size());
		//STX_PRINTF("meshes[%d].rindices.size()=%d\n", j, meshes[j].rindices.size());
		for(unsigned int i=0;i<meshes[j].rindices.size();i++)
		{
			//STX_PRINTF("meshes[%d].textureName_=%s\n", j, meshes[j].textureNames_[i].c_str());
			//STX_PRINTF("meshes[%d].textures_[%d]=%d\n", j, i, meshes[j].textures_[i]);
			//STX_PRINTF("meshes[%d].rindices[%d].size()=%d\n", j, i, meshes[j].rindices[i].size());
		}
	} 
	#endif
	return 0;
}
int MeshRenderer3::LoadDAEFile(const char* aFilename)
{
	TiXmlDocument document(aFilename);
	document.LoadFile();
	TiXmlHandle docHandle( &document );

	std::vector<std::string> v_position;
	std::vector<std::string> v_tex1;
	std::vector<unsigned int> v_size;

	TiXmlHandle geometriesHandle = docHandle.FirstChild("COLLADA").FirstChild("library_geometries");
	TiXmlHandle geometryHandle = geometriesHandle.FirstChild("geometry");
	TiXmlElement* geometryElement = geometryHandle.Element();
	int i=1;
	while ( geometryElement )
	{
		std::string geometryID=std::string(geometryElement->Attribute("id"));
		std::string geometryName=std::string(geometryElement->Attribute("name"));

		//STX_PRINTF("%d: geometryID=%s, geometryName=%s\n", i, geometryID.c_str(), geometryName.c_str());

		TiXmlHandle sourceHandle = geometryHandle.FirstChild("mesh").FirstChild("source");
		TiXmlElement* sourceElement = sourceHandle.Element();
		int j=1;
/*
		while ( sourceElement )
		{
			std::string sourceName=std::string(sourceElement->Attribute("name"));

			//STX_PRINTF("%d: sourceName=%s\n", j, sourceName.c_str()); // position, map1

			TiXmlHandle float_arrayHandle = sourceHandle.FirstChild("float_array");
			TiXmlElement* float_arrayElement = float_arrayHandle.Element();
			std::string float_arraycount=std::string(float_arrayElement->Attribute("count"));
			//STX_PRINTF("%d: float_arraycount=%s\n", j, float_arraycount.c_str());
			if(!atoi(float_arraycount.c_str()))
				break;
			meshes[meshes.size()-1].vertices.resize(atoi(float_arraycount.c_str())/3);
			meshes[meshes.size()-1].indices.resize(atoi(float_arraycount.c_str())/3);
			sourceHandle = geometryHandle.FirstChild("mesh").Child( "source", j++ );
			sourceElement = sourceHandle.Element();
		}
		j=1;
*/
		while ( sourceElement )
		{
			std::string sourceName=std::string(sourceElement->Attribute("name"));
			//STX_PRINTF("%d: sourceName=%s\n", j, sourceName.c_str()); // position, map1
			TiXmlHandle float_arrayHandle = sourceHandle.FirstChild("float_array");
			TiXmlElement* float_arrayElement = float_arrayHandle.Element();
			std::string float_arraycount=std::string(float_arrayElement->Attribute("count"));
			//STX_PRINTF("%d: float_arraycount=%s\n", j, float_arraycount.c_str());
			if(!atoi(float_arraycount.c_str()))
				break;

			if(atoi(float_arraycount.c_str())>65535)
			{
				//LOG_FNLN;
				//STX_PRINTF("VB: %d > 65535\n", atoi(float_arraycount.c_str()));
				meshes.clear();
				return 1;
			}

			v_size.push_back(atoi(float_arraycount.c_str())/3);
			std::string float_v=std::string(float_arrayElement->GetText());
			//STX_PRINTF("%d: float_v=%s\n", j, float_v.c_str());
			//std::vector<std::string> v=stx_split(float_v);
			//float_v="";
			if(sourceName=="position")
			{
				v_position.push_back(float_v);
				/*
				for(unsigned int k = 0; k < atoi(float_arraycount.c_str())/3; k++)
				{
					D3DXFROMWINEVECTOR3 v3(atof(v[3*k+0].c_str()), atof(v[3*k+1].c_str()), atof(v[3*k+2].c_str()));
					meshes[meshes.size()-1].vertices[k].Position=v3;
					meshes[meshes.size()-1].indices[k]=k;
				}
				*/
			}
			else if(sourceName=="map1")
			{
				v_tex1.push_back(float_v);
				/*
				for(unsigned int k = 0; k < atoi(float_arraycount.c_str())/2; k++)
				{
					D3DXFROMWINEVECTOR2 v2(atof(v[2*k+0].c_str()), atof(v[2*k+1].c_str()));
					meshes[meshes.size()-1].vertices[k].Tex=v2;
				}
				*/
			}
			float_v="";
			//v.clear();

			sourceHandle = geometryHandle.FirstChild("mesh").Child( "source", j++ );
			sourceElement = sourceHandle.Element();
		}
		geometryHandle = geometriesHandle.Child( "geometry", i++ );
		geometryElement = geometryHandle.Element();
	}
	for(unsigned int i=0;i<v_position.size();i++)
	{
		stx_Mesh m;
		meshes.push_back(m);

		meshes[meshes.size()-1].vertices.resize(v_size[i]);
		meshes[meshes.size()-1].faces.resize(v_size[i]/3);

		std::vector<std::string> v=stx_split(v_position[i]);

		for(unsigned int k=0;k<v_size[i]/3;k++)
		{
			D3DXFROMWINEVECTOR3 v3(atof(v[3*k+0].c_str()), atof(v[3*k+1].c_str()), atof(v[3*k+2].c_str()));
			meshes[meshes.size()-1].vertices[k].Position=v3;
			meshes[meshes.size()-1].faces[k]=0;
			meshes[meshes.size()-1].indices.push_back(meshes[meshes.size()-1].vertices.size()-1);
		}
		v.clear();
		v=stx_split(v_tex1[i]);	
		for(unsigned int k=0;k<v_size[i]/2;k++)
		{
			D3DXFROMWINEVECTOR2 v2(atof(v[2*k+0].c_str()), atof(v[2*k+1].c_str()));
			meshes[meshes.size()-1].vertices[k].Tex=v2;
		}
		v.clear();
	}

	TiXmlHandle imagesHandle = docHandle.FirstChild("COLLADA").FirstChild("library_images");
	TiXmlHandle imageHandle = imagesHandle.FirstChild("image");
	TiXmlElement* imageElement = imageHandle.Element();
	int y=1;
	while ( imageElement )
	{
		std::string imageID=std::string(imageElement->Attribute("id"));
		std::string imageName=std::string(imageElement->Attribute("name"));

	TiXmlHandle init_fromHandle = imageHandle.FirstChild("init_from");
	TiXmlElement* init_fromElement = init_fromHandle.Element();

		std::string imageFile=std::string("Smoke/Gargoyle/")+std::string(init_fromElement->GetText());

		//LOG_PRINT("imageFile=%s\n", imageFile.c_str());

		TextureID txid=IRenderer::GetRendererInstance()->addImageLibTexture(imageFile.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
		meshes[meshes.size()-1].textures.push_back(txid);
		meshes[meshes.size()-1].textureNames.push_back(imageFile);						
		std::vector<__WORD__> rindices;
		meshes[meshes.size()-1].rindices.push_back(rindices);

		//STX_PRINTF("%d: imageID=%s, imageName=%s, imageFile=%s\n", y, imageID.c_str(), imageName.c_str(), imageFile.c_str());

		imageHandle = imagesHandle.Child( "image", y++ );
		imageElement = imageHandle.Element();
	}
	
	return 0;
}

#include "3ds.h"
int MeshRenderer3::Load3DSFile(const char* aFilename)
{
	CLoad3DS g_Load3ds;
	stx_3DModel g_3DModel;
	g_Load3ds.Import3DS(&g_3DModel, aFilename);
	//STX_PRINTF("numOfObjects=%d\n", g_3DModel.numOfObjects);
	//STX_PRINTF("numOfMaterials=%d\n", g_3DModel.numOfMaterials);
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		if(g_3DModel.pObject.size() <= 0) break;	
		stx_Mesh mesh;
		meshes.push_back(mesh);
		stx_3DObject *pObject = &g_3DModel.pObject[i];
		//STX_PRINTF("numOfVerts[%d]=%d\n", i, pObject->numOfVerts);
		//STX_PRINTF("numOfFaces[%d]=%d\n", i, pObject->numOfFaces);

		if(pObject->numOfVerts>65535)
		{
			//LOG_FNLN;
			//STX_PRINTF("VB: %d > 65535\n", pObject->numOfVerts);
			meshes.clear();
			return 1;
		}
		if(pObject->numOfFaces>65535)
		{
			//LOG_FNLN;
			//STX_PRINTF("VB: %d > 65535\n", pObject->numOfFaces);
			meshes.clear();
			return 1;
		}

		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			for(__WORD__ whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				__WORD__ vertIndex = pObject->pFaces[j].vertIndex[whichVertex];
				__WORD__ coordIndex = pObject->pFaces[j].coordIndex[whichVertex];
				if(vertIndex<0)
				{
					//LOG_FNLN;
					//STX_PRINTF("vertIndex<0\n");
					//STX_PRINTF("vertIndex=%d\n", vertIndex);
					continue;
				}
				stx_VertexPositionNormalTexture v;
				v.Position=pObject->pVerts[ vertIndex ];
				//v.Normal=pObject->pNormals[ vertIndex ];
				//stx_CalculatePositionalSphericalMapping(v);
				if(pObject->bHasTexture) {
					if(pObject->pTexVerts.size()) {
						v.Tex=pObject->pTexVerts[ coordIndex ];
					}
				}
				meshes[meshes.size()-1].vertices.push_back(v);
				meshes[meshes.size()-1].indices.push_back(vertIndex);
			}
			if(g_3DModel.numOfMaterials==1)
				meshes[meshes.size()-1].faces.push_back(0);
		}
	}
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0) {
			std::string tx=g_3DModel.pMaterials[i].strFile;
			TextureID txid=-1;
			std::string txfn=m_szPathTex+"/"+g_3DModel.pMaterials[i].strFile;
			txid=IRenderer::GetRendererInstance()->addImageLibTexture(txfn.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
			//STX_PRINTF("txfn=%s\ntxid=%d\n", txfn.c_str(), txid);
			meshes[meshes.size()-1].textures.push_back(txid);
			meshes[meshes.size()-1].textureNames.push_back(tx);				
			std::vector<__WORD__> rindices;
			meshes[meshes.size()-1].rindices.push_back(rindices);
		}
		g_3DModel.pMaterials[i].texureId = i;
	}
	
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		g_3DModel.pObject[i].pFaces.clear();
		g_3DModel.pObject[i].pNormals.clear();
		g_3DModel.pObject[i].pVerts.clear();
		g_3DModel.pObject[i].pTexVerts.clear();
	}
	return 0;
}
#include "Obj.h"
int MeshRenderer3::LoadOBJFile(const char* aFilename)
{
	CLoadObj g_LoadObj;
	stx_3DModel g_3DModel;
	g_LoadObj.ImportObj(&g_3DModel, aFilename);
	//g_LoadObj.AddMaterial(&g_3DModel, "Bone", "Bone.bmp", 255, 255, 255);
	//g_LoadObj.SetObjectMaterial(&g_3DModel, 0, 0);
	//STX_PRINTF("numOfObjects=%d\n", g_3DModel.numOfObjects);
	//STX_PRINTF("numOfMaterials=%d\n", g_3DModel.numOfMaterials);

	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0) {
			std::string tx=g_3DModel.pMaterials[i].strFile;
			TextureID txid=-1;
			std::string txfn=m_szPathTex+"/"+g_3DModel.pMaterials[i].strFile;
			txid=IRenderer::GetRendererInstance()->addImageLibTexture(txfn.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
			//STX_PRINTF("txfn=%s\ntxid=%d\n", txfn.c_str(), txid);
			meshes[meshes.size()-1].textures.push_back(txid);
			meshes[meshes.size()-1].textureNames.push_back(txfn);							
			std::vector<__WORD__> rindices;
			meshes[meshes.size()-1].rindices.push_back(rindices);
		}
		g_3DModel.pMaterials[i].texureId = i;
	}
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		if(g_3DModel.pObject.size() <= 0) break;
		stx_Mesh mesh;
		meshes.push_back(mesh);
		stx_3DObject *pObject = &g_3DModel.pObject[i];
		//STX_PRINTF("numOfVerts[%d]=%d\n", i, pObject->numOfVerts);
		//STX_PRINTF("numOfFaces[%d]=%d\n", i, pObject->numOfFaces);

	if(pObject->numOfVerts>65535)
	{
		//LOG_FNLN;
		//STX_PRINTF("VB: %d > 65535\n", pObject->numOfVerts);
		meshes.clear();
		return 1;
	}
	if(pObject->numOfFaces>65535)
	{
		//LOG_FNLN;
		//STX_PRINTF("VB: %d > 65535\n", pObject->numOfFaces);
		meshes.clear();
		return 1;
	}
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				int vertIndex = pObject->pFaces[j].vertIndex[whichVertex];
				int coordIndex = pObject->pFaces[j].coordIndex[whichVertex];
				if(vertIndex<0)
				{
					////LOG_FNLN;
					//STX_PRINTF("vertIndex<0\n");
					//STX_PRINTF("vertIndex=%d\n", vertIndex);
					continue;
				}
				stx_VertexPositionNormalTexture v;
				v.Position=pObject->pVerts[ vertIndex ];
				//STX_PRINTF("v.Position.x=%f, v.Position.y=%f, v.Position.z=%f\n", v.Position.x, v.Position.y, v.Position.z);
				//STX_PRINTF("vertIndex=%d\n", vertIndex);
				//v.Normal=pObject->pNormals[ vertIndex ];
				//stx_CalculatePositionalSphericalMapping(v);
				if(pObject->bHasTexture) {
					if(pObject->pTexVerts.size()) {
						v.Tex=pObject->pTexVerts[ coordIndex ];
					}
				}
				meshes[meshes.size()-1].vertices.push_back(v);
				meshes[meshes.size()-1].indices.push_back(vertIndex);
			}
			if(g_3DModel.numOfMaterials==1)
				meshes[meshes.size()-1].faces.push_back(0);
		}
	}	
	
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		g_3DModel.pObject[i].pFaces.clear();
		g_3DModel.pObject[i].pNormals.clear();
		g_3DModel.pObject[i].pVerts.clear();
		g_3DModel.pObject[i].pTexVerts.clear();
	}
	return 0;
}
//include <sdkmesh/obj.h>
//include <sdkmesh/sdkmesh.h>
int MeshRenderer3::LoadSDKMESHFile(const char* aFilename)
{
	stx_GetSDKMESHFile(aFilename, meshes);
	return 0;
}
int findClosingHyphen(std::vector<std::string>& av, unsigned int aStart)
{
	return -1;
}
int MeshRenderer3::LoadXFile(const char* aFilename)
{
	const char* symbols[] =
	{
		"AnimationSet","Animation","AnimationKey","Frame", "FrameTransformMatrix",	"Mesh", "MeshNormals", "MeshTextureCoords", "MeshMaterialList", "Material", "TextureFilename"
	};
	unsigned int nMesh=0;
	unsigned int nMeshNormals=0;
	unsigned int nMeshTextureCoords=0;
	unsigned int nMeshMaterialList=0;
	{
		std::ifstream t(m_sFileName);
		std::string txt((std::istreambuf_iterator<char>(t)),
                			std::istreambuf_iterator<char>());
		stx_ReplaceAll(txt, ";;;", " %endblock% ");
		stx_ReplaceAll(txt, ";;,","  ");
		stx_ReplaceAll(txt, ";;"," %endblock% ");
		stx_ReplaceAll(txt, ";,","  ");
		stx_ReplaceAll(txt, ";"," ");
		stx_ReplaceAll(txt, ","," ");
		stx_ReplaceAll(txt, "TextureFileName","TextureFilename");
		stx_ReplaceAll(txt, "}"," ");
		std::vector<std::string> v=stx_split(txt);

		if((v[0]!="xof") || (v[1]!="0303txt") || (v[2]!="0032"))
			return -1;

		for(unsigned int y=0;y<elementsOf(symbols);y++)
		{
			std::string symbol=symbols[y];
			if(true)
			{
				if((symbol=="AnimationSet") || (symbol=="Animation") || (symbol=="AnimationKey") || (symbol=="Mesh") || (symbol=="MeshNormals") || (symbol=="MeshTextureCoords") || (symbol=="MeshMaterialList"))
				{
					unsigned int indice_=0;
					for(unsigned int z=0;z<v.size();z++)
					{
						if(v[z]==symbol)
							indice_=z;
						else
							continue;
						if ((indice_>0) && (v[indice_-1]=="template"))
							continue;
						if ((symbol=="Material") || (symbol=="FrameTransformMatrix"))
							continue;
						/*
 Animation {
  

  AnimationKey {
   4;
   2;
   0;16;1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;,
   4960;16;1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;;
  }
  { Scene_Root }
 }
						*/
						#if 0
						if(v[indice_]=="AnimationSet")
						{
							unsigned int c=2;
							if(v[indice_+2]=="{")
							{
								c=3;
								//STX_PRINTF("%s: %s\n", symbol.c_str(), v[indice_+2].c_str());
							}
							else
							{
								//STX_PRINTF("%s:\n", symbol.c_str());
							}
						}
						else if(v[indice_]=="Animation")
						{
							if((v[indice_+2]=="{") || (v[indice_+3]=="{"))
							{
								//STX_PRINTF("1:[%d]=%s\n", indice_+4, v[indice_+4].c_str());
							}
							else
							{
								unsigned int c=2;
								if(v[indice_+2]=="{")
								{
									c=3;
									//STX_PRINTF("%s: %s\n", symbol.c_str(), v[indice_+2].c_str());
								}
								else
								{
									//STX_PRINTF("%s:\n", symbol.c_str());
								}
							}
						}
						else if(v[indice_]=="AnimationKey")
						{
							unsigned int c=2;
							if(v[indice_+2]=="{")
							{
								c=3;
								//STX_PRINTF("%s: %s\n", symbol.c_str(), v[indice_+2].c_str());
							}
							else
							{
								//STX_PRINTF("%s:\n", symbol.c_str());
							}
							unsigned int animationType=atoi(v[indice_+c].c_str());
							unsigned int animationN=atoi(v[indice_+c+1].c_str());
							//STX_PRINTF("animationType=%d\n", animationType);
							//STX_PRINTF("animationN=%d\n", animationN);
							for(unsigned int x=0;x<animationN;x++)
							{
								unsigned int animationTime=atoi(v[indice_+c+2].c_str());
								unsigned int animationM=atoi(v[indice_+c+3].c_str());
							//STX_PRINTF("animationTime=%d\n", animationTime);
							//STX_PRINTF("animationM=%d\n", animationM);
								c+=2+animationM;
							}
							unsigned int c_=indice_+c;
							//STX_PRINTF("[%d]=%s\n", c_+3, v[c_+3].c_str()); // '{'
							if("{"==v[c_+3])
							{
								//STX_PRINTF("2:[%d]=%s\n", c_+4, v[c_+4].c_str());
							}
						}
						else
						#endif
						if (v[indice_]=="MeshTextureCoords")
						{
							nMeshTextureCoords++;
							//STX_PRINTF("%s: VN: %s\n", symbol.c_str(), v[indice_+2].c_str());
							for(unsigned int zz=0;zz<atoi(v[indice_+2].c_str());zz++)
							{
								unsigned int ii=indice_+3+zz*2;
								if(meshes.size())if(zz<meshes[meshes.size()-1].vertices.size())if((ii+1)<v.size()) // ???
									meshes[meshes.size()-1].vertices[zz].Tex=D3DXFROMWINEVECTOR2(atof(v[ii].c_str()), atof(v[ii+1].c_str()));
							}
						}
						else if (v[indice_]=="Mesh")
						{
							nMesh++;
							nMeshNormals++;
							unsigned int o = 0;
							if((v[indice_+2]=="{"))
								o = 1;
							unsigned int c = 3*atoi(v[indice_+2+o].c_str());
							unsigned int c2 = 3*atoi(v[indice_+4+o+c].c_str());
							//STX_PRINTF("%s: VN: %s IN: %s\n", symbol.c_str(), v[indice_+2+o].c_str(), v[indice_+4+o+c].c_str());

							if((c/3)>65535)
							{
								//LOG_FNLN;
								//STX_PRINTF("VB: %ld > 65535\n", c);
								//meshes.clear();
								//return 1;
							}
							if((c2/3)>65535)
							{
								//LOG_FNLN;
								//STX_PRINTF("VB: %ld > 65535\n", c2);
								//meshes.clear();
								//return 1;
							}
							stx_Mesh mesh;
							meshes.push_back(mesh);
							if((v[indice_+2]=="{"))
								meshes[meshes.size()-1].m_sName=v[indice_+1];
							std::vector<__WORD__> faces;
							meshes[meshes.size()-1].faces=faces;
							D3DXFROMWINEVECTOR3 v3;
							for(unsigned int zz=0;zz<atoi(v[indice_+2+o].c_str());zz++)
							{
								unsigned int ii=indice_+3+o+3*zz;
								v3=D3DXFROMWINEVECTOR3(atof(v[ii].c_str()), atof(v[ii+1].c_str()), atof(v[ii+2].c_str()));
								meshes[meshes.size()-1].vertices.push_back(v3);
							}
							for(unsigned int zz=0;zz<atoi(v[indice_+4+o+c].c_str());zz++)
							{
								meshes[meshes.size()-1].indices.push_back(atoi(v[indice_+5+o+c+4*zz+1].c_str()));
								meshes[meshes.size()-1].indices.push_back(atoi(v[indice_+5+o+c+4*zz+2].c_str()));
								meshes[meshes.size()-1].indices.push_back(atoi(v[indice_+5+o+c+4*zz+3].c_str()));
							}
						}
						else if (v[indice_]==	"MeshMaterialList")
						{
							unsigned int nMeshMaterialList_=0;
							//STX_PRINTF("%s: MN: %s FN: %s %s\n", symbol.c_str(), v[indice_+2].c_str(), v[indice_+3].c_str(), v[indice_+4+atoi(v[indice_+3].c_str())+1].c_str());
							{
							for(unsigned int zz=indice_+4;zz<indice_+4+atoi(v[indice_+3].c_str());zz++)
							{
						meshes[nMeshMaterialList_].faces.push_back(atoi(v[zz].c_str())); // ??? // ???
							}
							if(v[indice_+3+atoi(v[indice_+3].c_str())+1]=="{")
							{
								for(unsigned int y=0;y<v.size();y++)
								{
									if(v[y]==v[indice_+4+atoi(v[indice_+3].c_str())+1])
									if(v[y-1]=="Material")
									{
										//STX_PRINTF("MeshMaterialList8: {%s[%d]} %s\n", v[indice_+4+atoi(v[indice_+3].c_str())+1].c_str(), y, v[y+18].c_str());
						std::string tx=v[y+18];
						stx_ReplaceAll(tx, "\"","");
						if(1)//(tx.length())
						{
							if(m_bMeshRenderer3Simpler_ProceduralBuilding)
								meshes[meshes.size()-1].textures.push_back(-1);
							meshes[meshes.size()-1].textureNames.push_back(tx);
							if(m_bMeshRenderer3Simpler_ProceduralBuilding){
							std::string txfn=m_szPathTex+"/"+tx;
				TextureID texID=IRenderer::GetRendererInstance()->addImageLibTexture(txfn.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
							meshes[meshes.size()-1].textures.push_back(texID);
							}
						}
									}
								}
							}}

							if((atoi(v[indice_+2].c_str())==1) && (atoi(v[indice_+3].c_str())==1))
							{
								meshes[nMeshMaterialList_].rindices.push_back(meshes[nMeshMaterialList_].indices); // ???
							}

							nMeshMaterialList_++;
						}
						else
						{
							unsigned int c = 3*atoi(v[indice_+2].c_str());
							//STX_PRINTF("%s: VN: %s IN: %s\n", symbol.c_str(), v[indice_+2].c_str(), v[indice_+4+c].c_str());
						}
					}
				}
				#if 1
				if(symbol=="TextureFilename")
				{
					nMeshTextureCoords++;
					unsigned int indice_=0;
					for(unsigned int z=0;z<v.size();z++)
					{
						if(v[z]==symbol)
							indice_=z;
						else
							continue;
						if ((indice_) && (v[indice_-1]=="template"))
							continue;
						std::string tx=v[indice_+2];
						stx_ReplaceAll(tx, "\"","");

						//STX_PRINTF("tex: %s\n", tx.c_str());

						if(1)//(tx.length())
						{
							meshes[meshes.size()-1].textures.push_back(-1);
							meshes[meshes.size()-1].textureNames.push_back(tx);
						}
					}
				}
				#endif
			}
		}
		v.clear();
	}
	initrindices();
	return 0;
}
void MeshRenderer3::initrindices()
{
	for(unsigned int m=0;m<meshes.size();m++)
	{
if(m_bMeshRenderer3Simpler_ProceduralBuilding){
		for(unsigned int t=0;t<meshes[m].textureNames.size();t++)
		{
			if(meshes[m].textureNames[t].length())
			{	
				std::string txfn=m_szPathTex+"/"+meshes[m].textureNames[t];
				meshes[m].textures[t]=IRenderer::GetRendererInstance()->addImageLibTexture(txfn.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
			}
		}
		if(meshes[m].rindices.size())
			continue;
		meshes[m].rindices.resize(meshes[m].textureNames.size());
		if(meshes[m].faces.size())
		{
			for(unsigned int f=0;f<meshes[m].faces.size();f++)
			{
				for(unsigned int t=0;t<meshes[m].textureNames.size();t++)
				{
					if(meshes[m].faces[f]==t)
					{
						meshes[m].rindices[t].push_back(meshes[m].indices[3*f+0]);
						meshes[m].rindices[t].push_back(meshes[m].indices[3*f+1]);
						meshes[m].rindices[t].push_back(meshes[m].indices[3*f+2]);
					}
				}
			}
		}
		else
			meshes[m].rindices.push_back(meshes[m].indices);
}else{
		if(meshes[m].rindices.size())
		{
			for(unsigned int f=0;f<meshes[m].faces.size();f++)
			{
				meshes[m].textures_.push_back(meshes[m].faces[0]);
				meshes[m].textureNames_.push_back(meshes[m].textureNames[meshes[m].faces[0]]);
			}
			continue;
		}
		std::vector<__WORD__> faces_;
		{
			for(unsigned int f=0;f<meshes[m].faces.size();f++)
			{
				if ( std::find(faces_.begin(), faces_.end(), meshes[m].faces[f]) == faces_.end() )
					faces_.push_back(meshes[m].faces[f]);
			}
			meshes[m].rindices.resize(0);
			meshes[m].textures_.resize(0);
			for(unsigned int f_=0;f_<faces_.size();f_++)
			{
				meshes[m].rindices.resize(meshes[m].rindices.size()+1);
				for(unsigned int f=0;f<meshes[m].faces.size();f++)
				{
					{
						if((meshes[m].faces[f]==faces_[f_]))
						{
							meshes[m].rindices[f_].push_back(meshes[m].indices[3*f+0]);
							meshes[m].rindices[f_].push_back(meshes[m].indices[3*f+1]);
							meshes[m].rindices[f_].push_back(meshes[m].indices[3*f+2]);
						}
					}
				}
				meshes[m].textures_.push_back(faces_[f_]);
				if(meshes[m].faces.size()>f_)
					if(meshes[m].textureNames.size()>meshes[m].faces[f_])
						meshes[m].textureNames_.push_back(meshes[m].textureNames[meshes[m].faces[f_]]); // ??? Muistialueen ylitys (luotiin core-tiedosto)
			}
		}

		if(!meshes[m].textureNames_.size())
			if(meshes[m].textureNames.size())
				meshes[m].textureNames_.push_back(meshes[m].textureNames[0]);
			else
				meshes[m].textureNames_.push_back("");
}
	}
}
int MeshRenderer3::LoadAsset(const char* filename, bool scaleasset)
{
	Clear();
	Load(filename, eShaderNone, scaleasset);
	return 0;
}
const char* MeshRenderer3::GetFileName(){return m_sFileName.c_str();}

void MeshRenderer3::SetAmbientColor(D3DXFROMWINEVECTOR4 a){m_v4AmbientColor=a;}
void MeshRenderer3::SetDiffuseColor(D3DXFROMWINEVECTOR4 a){m_v4DiffuseColor=a;}
void MeshRenderer3::SetLightDir(D3DXFROMWINEVECTOR4 a){m_v4LightDir=a;}
void MeshRenderer3::SetEyeDir(D3DXFROMWINEVECTOR4 a){m_v4EyeDir=a;}
void MeshRenderer3::SetTX(float aX){}
void MeshRenderer3::SetTY(float aY){}
void MeshRenderer3::SetTZ(float aZ){}
void MeshRenderer3::SetSX(float aScaleX){}
void MeshRenderer3::SetSY(float aScaleY){}
void MeshRenderer3::SetSZ(float aScaleZ){}
void MeshRenderer3::SetSpinX(float aAngleX){}
void MeshRenderer3::SetSpinY(float aAngleY){}
void MeshRenderer3::SetSpinZ(float aAngleZ){}

//void MeshRenderer3::GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max){}
//void MeshRenderer3::GetBoundingSphereRadius(float& r){}

void MeshRenderer3::SetRenderingMode(unsigned int i){}
void MeshRenderer3::ChangeShader(D3DXFROMWINEMATRIX* mat, TextureID id, ShaderID shdid, VertexFormatID vfid, 
			D3DXFROMWINEVECTOR4 AmbientColor,
			D3DXFROMWINEVECTOR4 DiffuseColor,
			D3DXFROMWINEVECTOR4 LightDir,
			D3DXFROMWINEVECTOR4 EyeDir)
{
#if 0
LOG_FNLN;
LOG_PRINT("id=%d\n", id);
#endif
	if(shdid>-1)
		IRenderer::GetRendererInstance()->setShader(shdid);
	else if(0)//(Shader > -1)
	{
        	IRenderer::GetRendererInstance()->setShader(Shader);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", *mat);
		IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", AmbientColor);
		IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", DiffuseColor);
		IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", LightDir);
		IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", EyeDir);

		if(id > -1) IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", id);
		else if(texID > -1) IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
	}
	if((shdid>-1)&&(vfid>-1))
		IRenderer::GetRendererInstance()->setVertexFormat(vfid);
	//else if (VertexDeclaration > -1)
	//	IRenderer::GetRendererInstance()->setVertexFormat(VertexDeclaration);




	if((shdid>-1)&&mat)
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", *mat);
	if((shdid>-1)&&(id>-1))
		IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", id);
	if(shdid>-1)
		return;
	D3DXFROMWINEMATRIX w, I;
	D3DXFROMWINEMatrixIdentity(&I);
	if(mat)
		w=*mat;
	else
	{
		float mAngleX=0.0f;
		float mAngleY=0.0f;
		stx_GetAngles(mAngleX, mAngleY);
		D3DXFROMWINEMatrixRotationYawPitchRoll(&w, mAngleX, mAngleY, 0.0f);
		//D3DXFROMWINEMatrixIdentity(&w);
	}
	if(id > -1)
		texID=id;
	//if(mShader!=eCelShader)
		tex_tmp=texID;

		
		switch(mShader)
		{
			case eSimpleShader:
					//{LOG_FNLN;}//LOG_PRINT("eSimpleShader:\n");}
				IRenderer::GetRendererInstance()->setShader(SimpleShader);
				IRenderer::GetRendererInstance()->setVertexFormat(SimpleVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
				break;
			case eAmbientLighting:
//{LOG_FNLN;}
				IRenderer::GetRendererInstance()->setShader(AmbientShader);
				IRenderer::GetRendererInstance()->setVertexFormat(AmbientVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", m_v4AmbientColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", m_v4DiffuseColor);
				IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
				break;
			case eDiffuseLighting:
//{LOG_FNLN;}
				IRenderer::GetRendererInstance()->setShader(DiffuseShader);
				IRenderer::GetRendererInstance()->setVertexFormat(DiffuseVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", m_v4AmbientColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", m_v4DiffuseColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", m_v4LightDir);
				IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
				break;
			case ePhongLighting1:
//{LOG_FNLN;}
				IRenderer::GetRendererInstance()->setShader(PhongShader1);
				IRenderer::GetRendererInstance()->setVertexFormat(PhongVertexDeclaration1);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", m_v4AmbientColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", m_v4DiffuseColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", m_v4LightDir);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", m_v4EyeDir);
				IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
				break;
			case ePhongLighting2:
//{LOG_FNLN;}
				IRenderer::GetRendererInstance()->setShader(PhongShader2);
				IRenderer::GetRendererInstance()->setVertexFormat(PhongVertexDeclaration2);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				IRenderer::GetRendererInstance()->setShaderConstant4f("AmbientColor", m_v4AmbientColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("DiffuseColor", m_v4DiffuseColor);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecLightDir", m_v4LightDir);
				IRenderer::GetRendererInstance()->setShaderConstant4f("vecEye", m_v4EyeDir);
				IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
				break;
			case eDirectionalLighting:
				IRenderer::GetRendererInstance()->setShader(DirectionalShader);
				IRenderer::GetRendererInstance()->setVertexFormat(DirectionalVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				break;
			case eSpotLighting:
				IRenderer::GetRendererInstance()->setShader(DirectionalShader);
				IRenderer::GetRendererInstance()->setVertexFormat(DirectionalVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant3f("SpotLightdirection", LightDirection);
				break;
			case eMultiLighting:
				IRenderer::GetRendererInstance()->setShader(DirectionalShader);
				IRenderer::GetRendererInstance()->setVertexFormat(DirectionalVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstantArray3f("LightPosition", LightPosition, 5);
				IRenderer::GetRendererInstance()->setShaderConstantArray3f("LightIntensity", LightIntensity, 5);
				break;
			#if 1
			case eSnowShader:
//{LOG_FNLN;}
				IRenderer::GetRendererInstance()->setShader(SnowShader);
				IRenderer::GetRendererInstance()->setVertexFormat(SnowVertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorld", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldIT", wit);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldView", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mViewProj", I);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mProjection", I);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mWorldViewProjection", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mViewInverse", I);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("g_mExposureWorldViewOrthoProj", w); // mWorld*g_pExposureView*g_pExposureOrthoProj
				IRenderer::GetRendererInstance()->setShaderConstant1f("SceneWidth",  float(IRenderer::GetRendererInstance()->GetViewportWidth()));
				IRenderer::GetRendererInstance()->setShaderConstant1f("SceneHeight", float(IRenderer::GetRendererInstance()->GetViewportHeight()));
				//IRenderer::GetRendererInstance()->setTexture("MeshTextureSampler", MeshTexture);
				IRenderer::GetRendererInstance()->setTexture("ExposureDepthMapSampler", ExposureDepthMapTexture);
				break;
			case eCelShader:
//{LOG_FNLN;}
				if(OutlineShader!=-1)
				{
					IRenderer::GetRendererInstance()->setShader(OutlineShader);
					IRenderer::GetRendererInstance()->setVertexFormat(OutlineVertexDeclaration);
					IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
					w_tmp=w;
					IRenderer::GetRendererInstance()->setTexture("CelShade", ShadingTexture);
				}
				break;
			case eCelShader1:
				IRenderer::GetRendererInstance()->setShader(CelShader1);
				IRenderer::GetRendererInstance()->setVertexFormat(CelVertexDeclaration1);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				break;
			case eCelShader2:
				IRenderer::GetRendererInstance()->setShader(CelShader2);
				IRenderer::GetRendererInstance()->setVertexFormat(CelVertexDeclaration2);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				break;
			case eCelShader3:
				IRenderer::GetRendererInstance()->setShader(CelShader3);
				IRenderer::GetRendererInstance()->setVertexFormat(CelVertexDeclaration3);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("View", I);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("Projection", I);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldInverseTranspose", wit);
				IRenderer::GetRendererInstance()->setTexture("textureSampler", ShadingTexture);
				break;
			case eCelShader4:
				IRenderer::GetRendererInstance()->setShader(CelShader4);
				IRenderer::GetRendererInstance()->setVertexFormat(CelVertexDeclaration4);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("World", w);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("View", I);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("Projection", I);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldInverseTranspose", wit);
				break;
			#endif
			default:
					//{LOG_FNLN;}//LOG_PRINT("default:\n");}
				IRenderer::GetRendererInstance()->setShader(Shader);
				IRenderer::GetRendererInstance()->setVertexFormat(VertexDeclaration);
				IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w);
				IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
				break;
		}
}



void MeshRenderer3::BeginDraw(	D3DXFROMWINEMATRIX* amat, TextureID id, ShaderID shdid, VertexFormatID vfid, 
			D3DXFROMWINEVECTOR4 AmbientColor,
			D3DXFROMWINEVECTOR4 DiffuseColor,
			D3DXFROMWINEVECTOR4 LightDir,
			D3DXFROMWINEVECTOR4 EyeDir)
{
#if 0
	D3DXFROMWINEMATRIX mat;
	if(amat)
		mat=*amat;
	else
		D3DXFROMWINEMatrixIdentity(&mat);
#endif
	ChangeShader(amat, id, shdid, vfid, AmbientColor, DiffuseColor, LightDir, EyeDir);
}

	unsigned int MeshRenderer3::EndDraw(const unsigned int flags)
        {
		unsigned int ret=0;
#if 0
	if(indices.size()){
		ret=IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, vertices.size(), indices.size()/3, &indices[0], &indices[0], sizeof(__WORD__), &vertices[0], &vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
		if(ret){LOG_PRINT("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}}
	else{
		ret=IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, vertices.size()/3, &vertices[0], &vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
		if(ret){LOG_PRINT("%s:%s:%d:ret=%x\n",__FILE__,__FUNCTION__,__LINE__,ret);}}
#else
		int j=0;
		int z=0;
		if(j_tmp>-1)
			j=j_tmp;
		if(z_tmp>-1)
			z=z_tmp;

		if(!meshes.size())
		{
			LOG_FNLN;
			LOG_PRINT("meshes.size()=0\n");
			stx_exit(0);
		}

		if(j>meshes.size())
		{
			LOG_FNLN;
			LOG_PRINT("j>meshes.size()\n");
			LOG_PRINT("%d>meshes.size()=%d\n", j, meshes.size());
			stx_exit(0);
		}

		if(z>meshes[j].rindices.size())
		{
			LOG_FNLN;
			LOG_PRINT("j>meshes[j].rindices.size()\n");
			LOG_PRINT("%d>meshes[%d].rindices.size()=%d\n", j, j, meshes[j].rindices.size());
			stx_exit(0);
		}

		if(mShader!=eCelShader)
		{
			if(tex_tmp>-1)
			{
				IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", tex_tmp);
				tex_tmp=-1;
			}
			if(meshes[j].rindices[z].size()/3)
				IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, meshes[j].vertices.size(), meshes[j].rindices[z].size()/3, &meshes[j].rindices[z].at(0), &meshes[j].rindices[z].at(0), sizeof(__WORD__), &meshes[j].vertices[0], &meshes[j].vertices[0], sizeof(stx_VertexPositionNormalTexture));
			else
				IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, meshes[j].vertices.size()/3, &meshes[j].vertices[0], &meshes[j].vertices[0], sizeof(stx_VertexPositionNormalTexture));
		}
		else
		{
			IRenderer::GetRendererInstance()->setShader(OutlineShader);
			IRenderer::GetRendererInstance()->setVertexFormat(OutlineVertexDeclaration);
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w_tmp);
			if(meshes[j].rindices[z].size()/3)
				IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, meshes[j].linevertices.size(), meshes[j].rindices[z].size()/3, &meshes[j].rindices[z].at(0), &meshes[j].rindices[z].at(0), sizeof(__WORD__), &meshes[j].linevertices[0], &meshes[j].linevertices[0], sizeof(stx_VertexPositionNormalNormal));
			else
				IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, meshes[j].linevertices.size()/3, &meshes[j].linevertices[0], &meshes[j].linevertices[0], sizeof(stx_VertexPositionNormalNormal));
			IRenderer::GetRendererInstance()->setShader(ShadingShader);
			IRenderer::GetRendererInstance()->setVertexFormat(ShadingVertexDeclaration);
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", w_tmp);
			static TextureID CelShade=-1;
			if(CelShade==-1)
				CelShade=IRenderer::GetRendererInstance()->addImageLibTexture("/CelShading/celshade.png", false, IRenderer::GetRendererInstance()->Getlinear());
			IRenderer::GetRendererInstance()->setTexture("CelShade", CelShade);
			if(meshes[j].rindices[z].size()/3)
				IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, meshes[j].linevertices.size(), meshes[j].rindices[z].size()/3, &meshes[j].rindices[z].at(0), &meshes[j].rindices[z].at(0), sizeof(__WORD__), &meshes[j].linevertices[0], &meshes[j].linevertices[0], sizeof(stx_VertexPositionNormalNormal));
			else
				IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, meshes[j].linevertices.size()/3, &meshes[j].linevertices[0], &meshes[j].linevertices[0], sizeof(stx_VertexPositionNormalNormal));
			
		if(j_tmp>-1)
			j_tmp=-1;
		if(z_tmp>-1)
			z_tmp=-1;
		}
#if 0
		float mAngleX=0.0f;
		float mAngleY=0.0f;
		stx_GetAngles(mAngleX, mAngleY);
		D3DXFROMWINEMATRIX R;
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
		D3DXFROMWINEVECTOR3 v3(1,0,0);
		D3DXFROMWINEVec3TransformCoord(&v3, &v3, &R);
		m_v4LightDir.x=v3.x;
		m_v4LightDir.y=v3.y;
		m_v4LightDir.z=v3.z;
#endif
#endif
		return ret;
        }


void MeshRenderer3::render(const D3DXFROMWINEMATRIX* aw)
{
	for(unsigned int j=0;j<meshes.size();j++)
	{
		if(m_bMeshRenderer3Simpler_ProceduralBuilding)
			if(!meshes[j].vertices.size())
				continue;
	for(unsigned int z=0;z<meshes[j].rindices.size();z++)
	{
		if(m_bMeshRenderer3Simpler_ProceduralBuilding)
			if(!meshes[j].rindices[z].size())
				continue;
		TextureID texID_=-1;
		if(z<meshes[j].textures.size())
			if(m_bMeshRenderer3Simpler_ProceduralBuilding)
				texID_=meshes[j].textures /* _ */ [z];
			else
				texID_=meshes[j].textures[z];
	j_tmp=j;
	z_tmp=z;
	BeginDraw(aw,  texID_);
	EndDraw();
	}}
}
void MeshRenderer3::render_(const D3DXFROMWINEMATRIX* aw)
{
#if 0
	D3DXFROMWINEMATRIX w;
	if(aw)
		w=*aw;
	else
		D3DXFROMWINEMatrixIdentity(&w);
#endif
	render(aw);
}
void MeshRenderer3::render(
		float a_fTX,float a_fTY,float a_fTZ,
		float a_fSX,float a_fSy,float a_fSZ,
		float a_fSpinX,
		float a_fSpinY,
		float a_fSpinZ)
{
	 m_fTX=a_fTX;
	 m_fTY=a_fTY;
	 m_fTZ=a_fTZ;
			m_fSX=a_fSX;
			m_fSy=a_fSy;
			m_fSZ=a_fSZ;
			m_fSpinX=a_fSpinX;
			m_fSpinY=a_fSpinY;
			m_fSpinZ=a_fSpinZ;
		D3DXFROMWINEMATRIX mMatR;
D3DXFROMWINEMATRIX mMatS;
D3DXFROMWINEMATRIX mMatT;
		D3DXFROMWINEMatrixRotationYawPitchRoll(&mMatR,
			m_fSpinX,
			m_fSpinY,
			m_fSpinZ);
D3DXFROMWINEMatrixScaling(&mMatS,m_fSX,m_fSy,m_fSZ);
D3DXFROMWINEMatrixTranslation(&mMatT,m_fTX,m_fTY,m_fTZ);
D3DXFROMWINEMATRIX mMat = mMatR* mMatS *mMatT;
render( &mMat);
}

void MeshRenderer3::SetPlay(bool a){}
void MeshRenderer3::SetAnimIndex( unsigned int pAnimIndex){}
size_t MeshRenderer3::GetCurrentAnimIndex(){return 0;}
size_t MeshRenderer3::GetNumAnimations(){return 0;}

//-------------------------------------------------------------------------------
// Calculate the boundaries of a given node and all of its children
// The boundaries are in Worldspace (AABB)
// piNode Input node
// p_avOut Receives the min/max boundaries. Must point to 2 D3DXFROMWINEVECTOR3s
// piMatrix Transformation matrix of the graph at this position
//-------------------------------------------------------------------------------
int MeshRenderer3::CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut,
	const D3DXFROMWINEMATRIX& piMatrix)
{
	/*
	D3DXFROMWINEMATRIX mTemp; // ??? = piNode->mTransformation;
	//mTemp.Transpose();
	D3DXFROMWINEMatrixIdentity(&mTemp);
	D3DXFROMWINEMATRIX aiMe = mTemp * piMatrix;
	*/
	for (unsigned int i = 0; i < meshes.size();++i)
	{
		for( unsigned int a = 0; a < meshes[i].vertices.size();++a)
		{
			D3DXFROMWINEVECTOR3 pc =meshes[i].vertices[a].Position;
			D3DXFROMWINEVECTOR3 pc1=pc;
			
			//D3DXFROMWINEVec3TransformCoord((D3DXFROMWINEVECTOR3*)&pc1,(D3DXFROMWINEVECTOR3*)&pc, (D3DXFROMWINEMATRIX*)&aiMe);

			p_avOut[0].x = std::min( p_avOut[0].x, pc1.x);
			p_avOut[0].y = std::min( p_avOut[0].y, pc1.y);
			p_avOut[0].z = std::min( p_avOut[0].z, pc1.z);
			p_avOut[1].x = std::max( p_avOut[1].x, pc1.x);
			p_avOut[1].y = std::max( p_avOut[1].y, pc1.y);
			p_avOut[1].z = std::max( p_avOut[1].z, pc1.z);
		}
	} /*
	for (unsigned int i = 0; i < piNode->mNumChildren;++i)
	{
		CalculateBounds( piNode->mChildren[i], p_avOut, aiMe );
	} */
	return 1;
}
//-------------------------------------------------------------------------------
// Scale the asset that it fits perfectly into the viewer window
// The function calculates the boundaries of the mesh and modifies the
// global world transformation matrix according to the aset AABB
//-------------------------------------------------------------------------------
void MeshRenderer3::GetBoundingSphereRadius(float& r)
{
	D3DXFROMWINEVECTOR3 ____min;
	D3DXFROMWINEVECTOR3 ____max;
	GetBounds(____min, ____max);
	D3DXFROMWINEVECTOR3 vDelta = ____max-____min;
	r = 0.5f*D3DXFROMWINEVec3Length(&vDelta);
}
void MeshRenderer3::GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max)
{
	D3DXFROMWINEVECTOR3 aiVecs[2] = {D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f),
		D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f) };

	{
		D3DXFROMWINEMATRIX m;
		CalculateBounds(aiVecs,m);
	}

	____min = *(D3DXFROMWINEVECTOR3*)&aiVecs[0];
	____max = *(D3DXFROMWINEVECTOR3*)&aiVecs[1];
}
    	D3DXFROMWINEVECTOR3 MeshRenderer3::GetMeshBBoxExtents( unsigned int )
	{
		D3DXFROMWINEVECTOR3 __min;
		D3DXFROMWINEVECTOR3 __max;
		GetBounds(__min, __max);
		return __max;
	}
    	D3DXFROMWINEVECTOR3 MeshRenderer3::GetMeshBBoxCenter( unsigned int )
	{
		D3DXFROMWINEVECTOR3 __min;
		D3DXFROMWINEVECTOR3 __max;
		GetBounds(__min, __max);
		return __max/2.0f;// is this correct ???
	}
D3DXFROMWINEMATRIX MeshRenderer3::ScaleAsset(void)
{
	D3DXFROMWINEVECTOR3 aiVecs[2] = {D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f),
		D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f) };

	{
		D3DXFROMWINEMATRIX m;
		CalculateBounds(aiVecs,m);
	}

	D3DXFROMWINEVECTOR3 vDelta = aiVecs[1]-aiVecs[0];
	D3DXFROMWINEVECTOR3 vHalf =  aiVecs[0] + (vDelta / 2.0f);
	float fScale = 2.0f / D3DXFROMWINEVec3Length(&vDelta);

	m_mWorld =  D3DXFROMWINEMATRIX(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
		D3DXFROMWINEMATRIX(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);

	for (unsigned int i = 0; i < meshes.size();++i)
	{
		for( unsigned int j = 0; j < meshes[i].vertices.size();++j)
		{
			D3DXFROMWINEVec3TransformCoord(&meshes[i].vertices[j].Position, &meshes[i].vertices[j].Position, &m_mWorld);
		}
	}
	return m_mWorld;
}

#if 0
	void MeshRenderer3::RemoveHiddenTriangles2()
	{
		std::vector<__WORD__> indicestmp=indices;	
		// ??? for(int i=indices.size()-1;i>=0;3*i--)
            	{
			D3DXFROMWINEVECTOR3 p[3],v[3],d[3];
			for(unsigned int j=0;j<indices.size()/3;j++)
            		{
				if(i==j)
					continue;
				p[0]=vertices[indices[3*j+0]].Position;
				p[1]=vertices[indices[3*j+1]].Position;
				p[2]=vertices[indices[3*j+2]].Position;
				v[0]=vertices[indices[3*i+0]].Position;
				v[1]=vertices[indices[3*i+1]].Position;
				v[2]=vertices[indices[3*i+2]].Position;
		
				for (unsigned int n=0;n<3;n++)
					for (unsigned int m=0;m<3;m++)
					{
						d[m]=v[n]-p[m];
						d[m]/=D3DXFROMWINEVec3Length(&d[m]);
					}
				for (unsigned int m=0;m<3;m++)
				{
					boor r=stx_RayIntersectsTriangle(p[m], d[m], v[0], v[1], v[2]);
					if(r) indicestmp.erase(indicestmp.begin() + 3*i, indicestmp.begin() + 3*i + 3);
				}̈́
			}
		}
		indices=indicestmp;	
	}
#endif
	void MeshRenderer3::RemoveHiddenTriangles(const std::vector<D3DXFROMWINEVECTOR3>& minmax)
	{
		for (unsigned int k=0;k<minmax.size()/2;k++)
            	{
		for (unsigned int i=0;i<indices.size()/3;i++)
            	{
			int iRemoveTriangle=0;
			{D3DXFROMWINEVECTOR3 ap=vertices[indices[3*i+0]].Position;
			 D3DXFROMWINEVECTOR3 amin=minmax[2*k+0];
			 D3DXFROMWINEVECTOR3 amax=minmax[2*k+1];		
		if((ap.x>amin.x)&&(ap.y>amin.y)&&(ap.z>amin.z)&&(ap.x<amax.x)&&(ap.y<amax.y)&&(ap.z<amax.z))
			{
				vertices.erase(vertices.begin() + indices[3*i+0]);
				iRemoveTriangle++;
			}}
			{D3DXFROMWINEVECTOR3 ap=vertices[indices[3*i+1]].Position;
			 D3DXFROMWINEVECTOR3 amin=minmax[2*k+0];
			 D3DXFROMWINEVECTOR3 amax=minmax[2*k+1];		
		if((ap.x>amin.x)&&(ap.y>amin.y)&&(ap.z>amin.z)&&(ap.x<amax.x)&&(ap.y<amax.y)&&(ap.z<amax.z))
			{
				vertices.erase(vertices.begin() + indices[3*i+1]);
				iRemoveTriangle++;
			}}
			{D3DXFROMWINEVECTOR3 ap=vertices[indices[3*i+2]].Position;
			 D3DXFROMWINEVECTOR3 amin=minmax[2*k+0];
			 D3DXFROMWINEVECTOR3 amax=minmax[2*k+1];		
		if((ap.x>amin.x)&&(ap.y>amin.y)&&(ap.z>amin.z)&&(ap.x<amax.x)&&(ap.y<amax.y)&&(ap.z<amax.z))
			{
				vertices.erase(vertices.begin() + indices[3*i+2]);
				iRemoveTriangle++;
			}}
			if(iRemoveTriangle==3)
				indices.erase(indices.begin() + 3*i, indices.begin() + 3*i + 3);
				
		}
	}}
	void MeshRenderer3::CreateCombinedShape(const std::vector<MeshRenderer3>& av, const std::vector<D3DXFROMWINEMATRIX>& aW)
	{
		Clear();
		std::vector<D3DXFROMWINEVECTOR3> minmax;
            	unsigned int i=0;
            	unsigned int ind0=0;
            	while(1)
		{
		if(i>=av.size())
			break;
		if(i>=aW.size())
			break;
		//LOG_PRINT("i=%d\n", i);
		for (unsigned int j=0;j<av[i].indices.size();j++)
            	{
		//LOG_PRINT("j=%d\n", j);
			D3DXFROMWINEVECTOR3 v1=av[i].vertices[av[i].indices[j]].Position;
			stx_VertexPositionNormalTexture v2;
			D3DXFROMWINEVec3TransformCoord(&v2.Position, &v1, &aW[i]);
			vertices.push_back(v2);
			indices.push_back(av[i].indices[j]+ind0);
		}
		minmax.push_back(_min);
		minmax.push_back(_max);
		ind0+=av[i].indices.size();
		i++;
		}
		RemoveHiddenTriangles(minmax);
		for(unsigned int i=0;i<vertices.size();i++)
			stx_CalculatePositionalSphericalMapping(vertices[i]);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());

	}
	unsigned int MeshRenderer3::getVertexCount(){return vertices.size();}

	unsigned int MeshRenderer3::getTriangleCount(){return indices.size()/3;}

	VertexFormatID MeshRenderer3::getVertexDeclaration(){return VertexDeclaration;}
	void MeshRenderer3::setVertexDeclaration(VertexFormatID aVertexDeclaration){VertexDeclaration=aVertexDeclaration;}

        unsigned int MeshRenderer3::Draw(D3DXFROMWINEMATRIX* amat, TextureID id, ShaderID shdid, VertexFormatID vfid, D3DXFROMWINEVECTOR4 AmbientColor, D3DXFROMWINEVECTOR4 DiffuseColor, D3DXFROMWINEVECTOR4 LightDir, D3DXFROMWINEVECTOR4 EyeDir)
        {
	j_tmp=0;
	z_tmp=0;
		BeginDraw(amat, id, shdid, vfid, AmbientColor, DiffuseColor, LightDir, EyeDir);
		EndDraw(RESET_ALL);
		return 0;
        }

        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a box.
        /// </summary>
        /// <remarks>Always draw using a non-indexed TriangleList of 12 primitives.</remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created box.</param>
        /// <param name="width">Width of the box, along the x-axis.</param>
        /// <param name="height">Height of the box, along the y-axis.</param>
        /// <param name="depth">Depth of the box, along the z-axis.</param>
        void MeshRenderer3::CreateTexturedBox(float width, float height, float depth, tShader aShader_)
        {
		Clear();
		m_Shape=eTexturedBox;
            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;
            depth = depth / 2.0f;

            D3DXFROMWINEVECTOR3 topLeftFront(-width, height, depth);
            D3DXFROMWINEVECTOR3 bottomLeftFront(-width, -height, depth);
            D3DXFROMWINEVECTOR3 topRightFront(width, height, depth);
            D3DXFROMWINEVECTOR3 bottomRightFront(width, -height, depth);
            D3DXFROMWINEVECTOR3 topLeftBack(-width, height, -depth);
            D3DXFROMWINEVECTOR3 topRightBack(width, height, -depth);
            D3DXFROMWINEVECTOR3 bottomLeftBack(-width, -height, -depth);
            D3DXFROMWINEVECTOR3 bottomRightBack(width, -height, -depth);

            D3DXFROMWINEVECTOR2 textureTopLeft(0.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureBottomLeft(0.0f, 1.0f);
            D3DXFROMWINEVECTOR2 textureTopRight(1.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureBottomRight(1.0f, 1.0f);

            D3DXFROMWINEVECTOR3 frontNormal(0.0f, 0.0f, 1.0f);
            D3DXFROMWINEVECTOR3 backNormal(0.0f, 0.0f, -1.0f);
            D3DXFROMWINEVECTOR3 topNormal(0.0f, 1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 bottomNormal(0.0f, -1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 leftNormal(-1.0f, 0.0f, 0.0f);
            D3DXFROMWINEVECTOR3 rightNormal(1.0f, 0.0f, 0.0f);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, backNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal, textureBottomLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomRight));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal, textureTopLeft));
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureTopRight));
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer

	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();
	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	texID=-1;
	
        }
        void MeshRenderer3::CreateTexturedQuad(float width, float height, tShader aShader_)
        {
		Clear();
		m_Shape=eTexturedQuad;
            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;

            D3DXFROMWINEVECTOR3 topLeftFront(-width, height, 0.0f);
            D3DXFROMWINEVECTOR3 bottomLeftFront(-width, -height, 0.0f);
            D3DXFROMWINEVECTOR3 topRightFront(width, height, 0.0f);
            D3DXFROMWINEVECTOR3 bottomRightFront(width, -height, 0.0f);

            D3DXFROMWINEVECTOR2 textureTopLeft(0.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureTopRight(1.0f, 0.0f);
            D3DXFROMWINEVECTOR2 textureBottomLeft(0.0f, 1.0f);
            D3DXFROMWINEVECTOR2 textureBottomRight(1.0f, 1.0f);

            D3DXFROMWINEVECTOR3 frontNormal(0.0f, 0.0f, 1.0f);

            // Front face.
            vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal, textureTopLeft, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
            vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal, textureTopRight, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
            vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal, textureBottomLeft, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
            vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal, textureBottomRight, D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));

            unsigned int indices_[] = { 1, 0, 2, 1, 2, 3};

	    for(unsigned int i=0;i<elementsOf(indices_);i++)
		indices.push_back(indices_[i]);
            
            

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer

	    // ??? computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();
            

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	texID=-1;
	
        }
        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a box.
        /// </summary>
        /// <remarks>Always draw using a non-indexed TriangleList of 12 primitives.</remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created box.</param>
        /// <param name="width">Width of the box, along the x-axis.</param>
        /// <param name="height">Height of the box, along the y-axis.</param>
        /// <param name="depth">Depth of the box, along the z-axis.</param>
        void MeshRenderer3::CreateBox(float width, float height, float depth, tShader aShader_)
        {
		Clear();
		m_Shape=eBox;
            // Because the box is centered at the origin, need to divide by two to find the + and - offsets
            width = width / 2.0f;
            height = height / 2.0f;
            depth = depth / 2.0f;

            D3DXFROMWINEVECTOR3 topLeftFront(-width, height, depth);
            D3DXFROMWINEVECTOR3 bottomLeftFront(-width, -height, depth);
            D3DXFROMWINEVECTOR3 topRightFront(width, height, depth);
            D3DXFROMWINEVECTOR3 bottomRightFront(width, -height, depth);
            D3DXFROMWINEVECTOR3 topLeftBack(-width, height, -depth);
            D3DXFROMWINEVECTOR3 topRightBack(width, height, -depth);
            D3DXFROMWINEVECTOR3 bottomLeftBack(-width, -height, -depth);
            D3DXFROMWINEVECTOR3 bottomRightBack(width, -height, -depth);

            D3DXFROMWINEVECTOR3 frontNormal(0.0f, 0.0f, 1.0f);
            D3DXFROMWINEVECTOR3 backNormal(0.0f, 0.0f, -1.0f);
            D3DXFROMWINEVECTOR3 topNormal(0.0f, 1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 bottomNormal(0.0f, -1.0f, 0.0f);
            D3DXFROMWINEVECTOR3 leftNormal(-1.0f, 0.0f, 0.0f);
            D3DXFROMWINEVECTOR3 rightNormal(1.0f, 0.0f, 0.0f);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
		
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);
	
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightBack, rightNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topRightFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(bottomLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftBack, backNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(topLeftFront, frontNormal));
		stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		
	//	0 1 3 3 2 0
		indices.push_back(vertices.size()-4);
		indices.push_back(vertices.size()-3);
		indices.push_back(vertices.size()-1);

		indices.push_back(vertices.size()-1);
		indices.push_back(vertices.size()-2);
		indices.push_back(vertices.size()-4);

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();
   
	if(aShader_!=eShaderNone)
		InitShader(aShader_);
        }

        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a sphere.
        /// </summary>
        /// <remarks>
        /// Draw with an INDEXED TriangleList.  The values to use are as follows:
        /// Vertex Count    = slices * (stacks - 1) + 2
        /// Primitive Count = slices * (stacks - 1) * 2
        /// </remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created sphere.</param>
        /// <param name="radius">Radius of the sphere. This value should be greater than or equal to 0.0f.</param>
        /// <param name="slices">Number of slices about the Y axis.</param>
        /// <param name="stacks">Number of stacks along the Y axis. Should be 2 or greater. (stack of 1 is just a cylinder)</param>
        void MeshRenderer3::CreateSphere(float radius, tShader aShader_)
        {
		int slices = (int) 10*(4 * std::cbrt(radius) + 6);
        	int stacks = (int) 10*(4 * std::cbrt(radius) + 6);
		CreateSphere_(radius, slices, stacks, aShader_);
	}
        void MeshRenderer3::CreateSphere_(float radius, int aslices, int stacks, tShader aShader_)
        {
		Clear();
		m_Shape=eSphere;
            unsigned int slices=aslices;//+1;
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = D3DXFROMWINE_PI / stacks;
            unsigned int vertexCount = slices * (stacks - 1) + 2;
            unsigned int triangleCount = slices * (stacks - 1) * 2;
            unsigned int indexCount = triangleCount * 3;

            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, -radius, 0), D3DXFROMWINEVECTOR3(0, -1, 0)));
            float stackAngle = D3DXFROMWINE_PI - stackStep;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
                    //NOTE: y and z were switched from normal spherical coordinates because the sphere is "oriented" along the Y axis as opposed to the Z axis
                    float z = (float)(radius * sinf(stackAngle) * sinf(sliceAngle));
                    float x = (float)(radius * sinf(stackAngle) * cosf(sliceAngle));
                    float y = (float)(radius * cosf(stackAngle));
                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);
			D3DXFROMWINEVec3Normalize(&normal, &normal);
    			float tu = asinf(normal.x)/D3DXFROMWINE_PI + 0.5;
    			float tv = -asinf(normal.y)/D3DXFROMWINE_PI - 0.5;

                   vertices.push_back(stx_VertexPositionNormalTexture(position, normal, D3DXFROMWINEVECTOR2(tu, tv), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
                    sliceAngle += sliceStep;
                }
                stackAngle -= stackStep;
            }
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, radius, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));

		D3DXFROMWINEVECTOR3 aiVecs[2];
		D3DXFROMWINEMATRIX m;
		//D3DXFROMWINEMatrixIdentity(&m);		
	D3DXFROMWINEMatrixScaling(&m, radius, radius, radius);
		CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertexCount);
            CreateIndexBuffer(vertexCount, indexCount, slices);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	
        }

        void MeshRenderer3::CreateCone(float bottomRadius, float height, tShader aShader_)
        {
		Clear();
		m_Shape=eSphere;
        int slices = 20;//(int) 10*(4 * std::cbrt(bottomRadius) + 6)
        float angle = 2.0f*D3DXFROMWINE_PI / slices;
        vertices.resize((slices + 2));
        int i = 0;
        for (int s = 0; s < slices; s++) {
            vertices[i].Position.x = bottomRadius * cosf(s * angle);
            vertices[i].Position.y = -height / 2;
            vertices[i].Position.z = bottomRadius * sinf(s * angle);
		i++;
        }
        vertices[i].Position.x = vertices[i].Position.z = 0;
        vertices[i].Position.y = -height / 2;
		i++;
        vertices[i].Position.y = height / 2;
        vertices[i].Position.x = vertices[i].Position.z = 0;
		i++;
/*
        indices = new short[slices * 6];
        short p = 0;
        for (i = 0; i < indices.length - 6; i += 3) {
            indices[i++] = indices[i + 2] = p++;
            indices[i++] = indices[i + 2] = p;
            indices[i++] = indices[i + 2] = slices;
            indices[i + 2]++;
        }
        indices[i++] = indices[i + 2] = p;
        indices[i++] = indices[i + 2] = 0;
        indices[i++] = indices[i + 2] = slices;
        indices[i + 2]++;
*/
#if 1
        indices.resize(slices * 6);
        int p = 0;
        for (i = 0; i < indices.size() - 6; i += 3) {
            indices[i++] = indices[i + 2] = p++;
            indices[i++] = indices[i + 2] = p;
            indices[i++] = indices[i + 2] = slices;
            indices[i + 2]++;
        }
        indices[i++] = indices[i + 2] = p;
        indices[i++] = indices[i + 2] = 0;
        indices[i++] = indices[i + 2] = slices;
        indices[i + 2]++;
#endif

        for (i = 0; i < vertices.size();i++) 
		stx_CalculatePositionalSphericalMapping(vertices[i]);
		D3DXFROMWINEVECTOR3 aiVecs[2];
		D3DXFROMWINEMATRIX m;	
	D3DXFROMWINEMatrixScaling(&m, bottomRadius, bottomRadius, bottomRadius);
		CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
            CreateIndexBuffer(vertices.size(), indices.size(), slices);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();


	if(aShader_!=eShaderNone)
		InitShader(aShader_);
        }

        void MeshRenderer3::CreateHalfSphere(float radius, tShader aShader_)
        {
		Clear();
		m_Shape=eHalfSphere;
		int slices = (int) 10*(4 * std::cbrt(radius) + 6);
        	int stacks = (int) 10*(4 * std::cbrt(radius) + 6);
            
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = 0.5f*D3DXFROMWINE_PI / stacks;
            unsigned int vertexCount = slices * (stacks - 1) + 2;
            unsigned int triangleCount = slices * (stacks - 1) * 2;
            unsigned int indexCount = triangleCount * 3;

            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, radius, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));
            //float stackAngle = D3DXFROMWINE_PI - stackStep;
            float stackAngle = 0.0f;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0.0f;
                for (unsigned int j = 0; j < slices; j++)
                {
                    //NOTE: y and z were switched from normal spherical coordinates because the sphere is "oriented" along the Y axis as opposed to the Z axis
                    float x = (float)(radius * sinf(stackAngle) * cosf(sliceAngle));
                    float y = (float)(radius * cosf(stackAngle));
                    float z = (float)(radius * sinf(stackAngle) * sinf(sliceAngle));
                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);
			D3DXFROMWINEVec3Normalize(&normal, &normal);
    			float tu = asinf(normal.x)/D3DXFROMWINE_PI + 0.5;
    			float tv = -asinf(normal.y)/D3DXFROMWINE_PI - 0.5;

                   vertices.push_back(stx_VertexPositionNormalTexture(position, normal, D3DXFROMWINEVECTOR2(tu, tv), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
                    sliceAngle += sliceStep;
                }
                stackAngle += stackStep;
            }
            //vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, radius, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
            CreateIndexBuffer(vertices.size(), indexCount, slices, false);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();
	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	
        }
        /// <summary>
        /// Fills vertices std::vector  with <see cref="stx_VertexPositionNormalTexture"/> vertices forming a cylinder.
        /// </summary>
        /// <remarks>
        /// Draw with an INDEXED TriangleList.  The values to use are as follows:
        /// Vertex Count    = slices * (stacks + 1) + 2
        /// Primitive Count = slices * (stacks + 1) * 2
        /// </remarks>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created cylinder.</param>
        /// <param name="bottomRadius">Radius at the negative Y end. Value should be greater than or equal to 0.0f.</param>
        /// <param name="topRadius">Radius at the positive Y end. Value should be greater than or equal to 0.0f.</param>
        /// <param name="length">Length of the cylinder along the Y-axis.</param>
        /// <param name="slices">Number of slices about the Y axis.</param>
        /// <param name="stacks">Number of stacks along the Y axis.</param>
        void MeshRenderer3::CreateCylinder(float bottomRadius, float topRadius, float length, tShader aShader_)
        {
		Clear();
		m_Shape=eCylinder;
		int slices = (int) 10*(4 * std::cbrt(bottomRadius+topRadius) + 6);
        	int stacks = (int) 10*(4 * std::cbrt(bottomRadius+topRadius) + 6);
		LOG_PRINT("slices = %d\n", slices);
		LOG_PRINT("stacks = %d\n", stacks);

            // if both the top and bottom have a radius of zero, just return 0, because invalid
            if (bottomRadius <= 0 && topRadius <= 0)
            {

            }

            float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float heightStep = length / stacks;
            float radiusStep = (topRadius - bottomRadius) / stacks;
            float currentHeight = -length / 2;
            unsigned int vertexCount = (stacks + 1) * slices + 2;   //cone = stacks * slices + 1
            unsigned int triangleCount = (stacks + 1) * slices * 2; //cone = stacks * slices * 2 + slices
            unsigned int indexCount = triangleCount * 3;
            float currentRadius = bottomRadius;

            // Start at the bottom of the cylinder
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, currentHeight, 0), D3DXFROMWINEVECTOR3(0, -1, 0)));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
            for (unsigned int i = 0; i <= stacks; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
                    float x = currentRadius * (float)cosf(sliceAngle);
                    float y = currentHeight;
                    float z = currentRadius * (float)sinf(sliceAngle);

                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);

			D3DXFROMWINEVec3Normalize(&normal, &normal);

                    vertices.push_back(stx_VertexPositionNormalTexture(position, normal));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);

                    sliceAngle += sliceStep;
                }
                currentHeight += heightStep;
                currentRadius += radiusStep;
            }
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, length / 2, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

            // Create the actual vertex buffer object
            computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertexCount);
            CreateIndexBuffer(vertexCount, indexCount, slices);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	
        }

        /// <summary>
        /// Fills indices std::vector  for spherical shapes like Spheres, Cylinders, and Cones.
        /// </summary>
        /// <param name="device">The <see cref="GraphicsDevice"/> that is associated with the created cylinder.</param>
        /// <param name="vertexCount">The total number of vertices making up the </param>
        /// <param name="indexCount">The total number of indices making up the </param>
        /// <param name="slices">The number of slices about the Y axis.</param>
        /// <returns>The index buffer containing the index data for the </returns>
        void MeshRenderer3::CreateIndexBuffer(unsigned int vertexCount, unsigned int indexCount, unsigned int slices, bool abBottom)
        {
            indices.resize(indexCount);
            unsigned int currentIndex = 0;

            // Bottom circle/cone of shape
            for (unsigned int i = 1; i <= slices; i++)
            {
                indices[currentIndex++] = 0;
                indices[currentIndex++] = i;
                if (i - 1 == 0)
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
            }

            // Middle sides of shape
            for (unsigned int i = 1; i < vertexCount - (unsigned int)slices - 1; i++)
            {
                indices[currentIndex++] = i;
                indices[currentIndex++] = i + (unsigned int)slices;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i + (unsigned int)slices - 1;

                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
                indices[currentIndex++] = i;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
            }

            // Top circle/cone of shape
		if(abBottom)
            for (unsigned int i = (unsigned int)vertexCount - (unsigned int)slices - 1; i < vertexCount - 1; i++)
            {
                indices[currentIndex++] = (unsigned int)vertexCount - 1;
                if ((i - 1) % slices == 0)
                    indices[currentIndex++] = i + (unsigned int)slices - 1;
                else
                    indices[currentIndex++] = i - 1;
                indices[currentIndex++] = i;
            }
        }
typedef int (*sortFunction)(const void*, const void*);
int comparePosition(float *a, float *b)
{
	return stx_memcmp(a, b, sizeof(D3DXFROMWINEVECTOR3));
}

void MeshRenderer3::computeNormals(void* aVertices, unsigned int aVertexSizeInBytes, unsigned int avertexCount, bool aSmoothGroup)
{
		return;
	if(aVertexSizeInBytes<sizeof(D3DXFROMWINEVECTOR3)*2)
		return;

  __WORD__ vertexIndex;

  std::vector<float> faceNormals;
  float *normalVecs  = 0,
        *texCoords   = 0,
        *vertices    = 0,
         magnitude   = 0,
         normal[3],
         v0[3],
         v1[3];

  int (*TriangleRefs)[32];

  int  i, j, k,
       count  = 0,
       vertex = 0,
       result = 0;

  unsigned int *pFaces  = 0;

  //D3DXFROMWINEVECTOR3* meshnormalVecs  = new D3DXFROMWINEVECTOR3[getVertexCount()];
  normalVecs        = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3);
  vertices          = (float*)&aVertices;

  if(!aSmoothGroup)
  {
    for(i=0;i<getTriangleCount();i++)
    {

      v0[0] = vertices[3*pFaces[3*i+1]+0] - vertices[3*pFaces[3*i+0]+0];
      v0[1] = vertices[3*pFaces[3*i+1]+1] - vertices[3*pFaces[3*i+0]+1];
      v0[2] = vertices[3*pFaces[3*i+1]+2] - vertices[3*pFaces[3*i+0]+2];

      v1[0] = vertices[3*pFaces[3*i+2]+0] - vertices[3*pFaces[3*i+0]+0];
      v1[1] = vertices[3*pFaces[3*i+2]+1] - vertices[3*pFaces[3*i+0]+1];
      v1[2] = vertices[3*pFaces[3*i+2]+2] - vertices[3*pFaces[3*i+0]+2];

      normal[0] = (v0[1]*v1[2]-v0[2]*v1[1]);
      normal[1] = (v0[2]*v1[0]-v0[0]*v1[2]);
      normal[2] = (v0[0]*v1[1]-v0[1]*v1[0]);

      magnitude = sqrtf(normal[0]*normal[0] +
                                 normal[1]*normal[1] +
                                 normal[2]*normal[2]);

      if(magnitude > .00001f){
        magnitude  = 1.0f/magnitude;
        normal[0] *= magnitude;
        normal[1] *= magnitude;
        normal[2] *= magnitude;
      }

      for(j=0;j<3;j++){
        normalVecs[3*pFaces[3*i+j]+0] += normal[0];
        normalVecs[3*pFaces[3*i+j]+1] += normal[1];
        normalVecs[3*pFaces[3*i+j]+2] += normal[2];
      }
    }
    return;
  }

#if 0
  //indexArray(&vertexIndex, (char *)vertices, sizeof(float)*3, getVertexCount(), (sortFunction)comparePosition);

  TriangleRefs = (int (*)[32])malloc(sizeof(int)*32*vertexIndex.count);

  if(TriangleRefs==0)
    return;

  stx_memset(TriangleRefs, 0, sizeof(int)*32*vertexIndex.count);

  for(i=0;i<getTriangleCount();i++)
  {
    for(j=0;j<3;j++){
     vertex = indexFind(&vertexIndex, &vertices[3*pFaces[3*i+j]], &result);

      if(TriangleRefs[vertex][0]<48){
        TriangleRefs[vertex][0]++;
        TriangleRefs[vertex][TriangleRefs[vertex][0]]=i;
      }
    }
  }
#endif

  faceNormals.resize(3*getTriangleCount());

  for(i=0;i<getTriangleCount();i++)
  {
    v0[0] = vertices[3*pFaces[3*i+1]+0] - vertices[3*pFaces[3*i+0]+0];
    v0[1] = vertices[3*pFaces[3*i+1]+1] - vertices[3*pFaces[3*i+0]+1];
    v0[2] = vertices[3*pFaces[3*i+1]+2] - vertices[3*pFaces[3*i+0]+2];

    v1[0] = vertices[3*pFaces[3*i+2]+0] - vertices[3*pFaces[3*i+0]+0];
    v1[1] = vertices[3*pFaces[3*i+2]+1] - vertices[3*pFaces[3*i+0]+1];
    v1[2] = vertices[3*pFaces[3*i+2]+2] - vertices[3*pFaces[3*i+0]+2];

    faceNormals[3*i+0] = (v0[1]*v1[2]-v0[2]*v1[1]);
    faceNormals[3*i+1] = (v0[2]*v1[0]-v0[0]*v1[2]);
    faceNormals[3*i+2] = (v0[0]*v1[1]-v0[1]*v1[0]);

    magnitude = sqrtf(faceNormals[3*i+0]*faceNormals[3*i+0] +
                               faceNormals[3*i+1]*faceNormals[3*i+1] +
                               faceNormals[3*i+2]*faceNormals[3*i+2]);

    if(magnitude)
    {
      magnitude           = 1.0f/magnitude;
      faceNormals[3*i+0] *= magnitude;
      faceNormals[3*i+1] *= magnitude;
      faceNormals[3*i+2] *= magnitude;
    }
  }
#if 0
  for(i=0;i<getTriangleCount();i++)
  {
    for(j=0;j<3;j++){

      vertex = indexFind(&vertexIndex, (void *)&vertices[3*pFaces[3*i+j]], &result);
      count  = 0;

      for(k=1; k <= TriangleRefs[vertex][0];k++)
      {
        if(aSmoothGroup[i] == aSmoothGroup[TriangleRefs[vertex][k]])
        {
          normalVecs[3*pFaces[3*i+j]+0] += faceNormals[3*TriangleRefs[vertex][k]+0];
          normalVecs[3*pFaces[3*i+j]+1] += faceNormals[3*TriangleRefs[vertex][k]+1];
          normalVecs[3*pFaces[3*i+j]+2] += faceNormals[3*TriangleRefs[vertex][k]+2];
          count++;
        }
      }

      normalVecs[3*pFaces[3*i+j]+0] /= (float)count;
      normalVecs[3*pFaces[3*i+j]+1] /= (float)count;
      normalVecs[3*pFaces[3*i+j]+2] /= (float)count;
    }
  }
  indexFree(&vertexIndex);

  stx_free(TriangleRefs);

#endif
}

void MeshRenderer3::computeTangentSpace(void* aVertices, unsigned int aVertexSizeInBytes, unsigned int avertexCount, bool aSmoothGroup)
{
	LOG_FNLN;
	LOG_PRINT("aVertices=%x\n", aVertices);
	LOG_PRINT("vertices.size()=%d\n", vertices.size());
	LOG_PRINT("indices.size()=%d\n", indices.size());
	LOG_PRINT("Triangle Count=%d\n", getTriangleCount());
	LOG_PRINT("avertexCount=%d\n", avertexCount);
	LOG_PRINT("aVertexSizeInBytes=%d\n", aVertexSizeInBytes);
	if(aVertexSizeInBytes<sizeof(D3DXFROMWINEVECTOR3)*4)
		return;
#if 0
  if(!Mesh->texCoords){
    computeNormals(aVertices, aVertexSizeInBytes, avertexCount);
    return;
  }
#endif

	int i, j, k, Count, Vertex=0, Result=0;
	__WORD__  VertexIndex;
	int (*TriangleRefs)[32];
	std::vector<float> FaceTangent;
	std::vector<float> FaceBinormal;
	std::vector<float> FaceNormal;
	float v0[3], v1[3], cross[3];
  float *pTexCoords = 0,
        *pBiNormals = 0,
        *pVertices  = 0,
        *pTangents  = 0,
        *pNormals   = 0,
        Mag;

	D3DXFROMWINEVECTOR3 Tangent,
          Binormal,
          Normal;

  __WORD__   *pFaces = 0;

  //meshnormalVecs  = new D3DXFROMWINEVECTOR3[getVertexCount()];
  float* normalVecs        = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3);
  float* vertices          = (float*)&aVertices;

  pBiNormals         = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3)*2;
  pTexCoords         = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3)*4;
  pTangents          = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3)*3;
  pVertices          = (float*)&aVertices;
  pNormals           = (float*)&aVertices+sizeof(D3DXFROMWINEVECTOR3);
  pFaces             = (__WORD__*)&indices[0];

  if(aSmoothGroup)
	{
		for(i=0;i<getTriangleCount();i++)
		{
			v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			if(fabs(cross[0])>0.000001f)
			{
				Tangent[0]  =-cross[1]/cross[0];
				Binormal[0] =cross[2]/cross[0];
			}

			v0[0]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			v1[0]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			if(fabs(cross[0])>0.000001f)
			{
				Tangent[1]=-cross[1]/cross[0];
				Binormal[1]=cross[2]/cross[0];
			}

			v0[0]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];
			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			v1[0]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];
			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			if(fabs(cross[0])>0.000001f)
			{
				Tangent[2]=-cross[1]/cross[0];
				Binormal[2]=cross[2]/cross[0];
			}

			Mag = sqrtf(Tangent[0]*Tangent[0]+Tangent[1]*Tangent[1]+Tangent[2]*Tangent[2]);

			if(Mag)
			{
				Mag=1.0f/Mag;
				Tangent[0]*=Mag;
				Tangent[1]*=Mag;
				Tangent[2]*=Mag;
			}

			Mag= sqrtf(Binormal[0]*Binormal[0]+Binormal[1]*Binormal[1]+Binormal[2]*Binormal[2]);

			if(Mag)
			{
				Mag=1.0f/Mag;
				Binormal[0]*=Mag;
				Binormal[1]*=Mag;
				Binormal[2]*=Mag;
			}

			v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
			v0[1]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
			v0[2]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];

			v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
			v1[1]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
			v1[2]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];

			Normal[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
			Normal[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
			Normal[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

			Mag = sqrtf(Normal[0]*Normal[0]+Normal[1]*Normal[1]+Normal[2]*Normal[2]);

			if(Mag)
			{
				Mag=1.0f/Mag;
				Normal[0]*=Mag;
				Normal[1]*=Mag;
				Normal[2]*=Mag;
			}

			for(j=0;j<3;j++)
			{
				pTangents[3*pFaces[3*i+j]+0]+=Tangent[0];
				pTangents[3*pFaces[3*i+j]+1]+=Tangent[1];
				pTangents[3*pFaces[3*i+j]+2]+=Tangent[2];

				pBiNormals[3*pFaces[3*i+j]+0]+=Binormal[0];
				pBiNormals[3*pFaces[3*i+j]+1]+=Binormal[1];
				pBiNormals[3*pFaces[3*i+j]+2]+=Binormal[2];

				pNormals[3*pFaces[3*i+j]+0]+=Normal[0];
				pNormals[3*pFaces[3*i+j]+1]+=Normal[1];
				pNormals[3*pFaces[3*i+j]+2]+=Normal[2];
			}
		}

		return;
	}
#if 0
 	//indexArray(&VertexIndex, (char *)pVertices, sizeof(float)*3, Mesh->NumVertex, (sortFunction)comparePosition);

	TriangleRefs=(int (*)[32])malloc(sizeof(int)*32*VertexIndex.count);

	if(TriangleRefs==0)
		return;

	stx_memset(TriangleRefs, 0, sizeof(int)*32*VertexIndex.count);

	for(i=0;i<getTriangleCount();i++)
	{
		for(j=0;j<3;j++)
		{
			Vertex=indexFind(&VertexIndex, &pVertices[3*pFaces[3*i+j]], &Result);

			if(TriangleRefs[Vertex][0]<48)
			{
				TriangleRefs[Vertex][0]++;
				TriangleRefs[Vertex][TriangleRefs[Vertex][0]]=i;
			}
		}
	}
#endif
	FaceTangent.resize(3*getTriangleCount());

	FaceBinormal.resize(3*getTriangleCount());

	FaceNormal.resize(3*getTriangleCount());

	for(i=0;i<getTriangleCount();i++)
	{
	//LOG_PRINT("i=%d\n", i);
		v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		if(fabs(cross[0])>0.000001f)
		{
			FaceTangent[3*i+0]=-cross[1]/cross[0];
			FaceBinormal[3*i+0]=cross[2]/cross[0];
		}

		v0[0]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		v1[0]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		if(fabs(cross[0])>0.000001f)
		{
			FaceTangent[3*i+1]=-cross[1]/cross[0];
			FaceBinormal[3*i+1]=cross[2]/cross[0];
		}

		v0[0]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];
		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		v1[0]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];
		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];
		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];

		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		if(fabs(cross[0])>0.000001f)
		{
			FaceTangent[3*i+2]=-cross[1]/cross[0];
			FaceBinormal[3*i+2]=cross[2]/cross[0];
		}

		Mag=(float)sqrt(FaceTangent[3*i+0]*FaceTangent[3*i+0]+FaceTangent[3*i+1]*FaceTangent[3*i+1]+FaceTangent[3*i+2]*FaceTangent[3*i+2]);

		if(Mag)
		{
			Mag=1.0f/Mag;
			FaceTangent[3*i+0]*=Mag;
			FaceTangent[3*i+1]*=Mag;
			FaceTangent[3*i+2]*=Mag;
		}

		Mag=(float)sqrt(FaceBinormal[3*i+0]*FaceBinormal[3*i+0]+FaceBinormal[3*i+1]*FaceBinormal[3*i+1]+FaceBinormal[3*i+2]*FaceBinormal[3*i+2]);

		if(Mag)
		{
			Mag=1.0f/Mag;
			FaceBinormal[3*i+0]*=Mag;
			FaceBinormal[3*i+1]*=Mag;
			FaceBinormal[3*i+2]*=Mag;
		}

		v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];
		v0[1]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];
		v0[2]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];

		v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];
		v1[1]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];
		v1[2]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];

		FaceNormal[3*i+0]=(v0[1]*v1[2]-v0[2]*v1[1]);
		FaceNormal[3*i+1]=(v0[2]*v1[0]-v0[0]*v1[2]);
		FaceNormal[3*i+2]=(v0[0]*v1[1]-v0[1]*v1[0]);

		Mag=(float)sqrt(FaceNormal[3*i+0]*FaceNormal[3*i+0]+FaceNormal[3*i+1]*FaceNormal[3*i+1]+FaceNormal[3*i+2]*FaceNormal[3*i+2]);

		if(Mag)
		{
			Mag=1.0f/Mag;
			FaceNormal[3*i+0]*=Mag;
			FaceNormal[3*i+1]*=Mag;
			FaceNormal[3*i+2]*=Mag;
		}
	}

#if 0

	for(i=0;i<getTriangleCount();i++)
	{
		for(j=0;j<3;j++)
		{
			Vertex=indexFind(&VertexIndex, (void *)&pVertices[3*pFaces[3*i+j]], &Result);

			Count=0;

			for(k=1;k<=TriangleRefs[Vertex][0];k++)
			{
				if(Mesh->smoothGroup[i]==Mesh->smoothGroup[TriangleRefs[Vertex][k]])
				{
					pTangents[3*pFaces[3*i+j]+0]+=FaceTangent[3*TriangleRefs[Vertex][k]+0];
					pTangents[3*pFaces[3*i+j]+1]+=FaceTangent[3*TriangleRefs[Vertex][k]+1];
					pTangents[3*pFaces[3*i+j]+2]+=FaceTangent[3*TriangleRefs[Vertex][k]+2];

					pBiNormals[3*pFaces[3*i+j]+0]+=FaceBinormal[3*TriangleRefs[Vertex][k]+0];
					pBiNormals[3*pFaces[3*i+j]+1]+=FaceBinormal[3*TriangleRefs[Vertex][k]+1];
					pBiNormals[3*pFaces[3*i+j]+2]+=FaceBinormal[3*TriangleRefs[Vertex][k]+2];

					pNormals[3*pFaces[3*i+j]+0]+=FaceNormal[3*TriangleRefs[Vertex][k]+0];
					pNormals[3*pFaces[3*i+j]+1]+=FaceNormal[3*TriangleRefs[Vertex][k]+1];
					pNormals[3*pFaces[3*i+j]+2]+=FaceNormal[3*TriangleRefs[Vertex][k]+2];

					Count++;
				}
			}

			pTangents[3*pFaces[3*i+j]+0]/=(float)Count;
			pTangents[3*pFaces[3*i+j]+1]/=(float)Count;
			pTangents[3*pFaces[3*i+j]+2]/=(float)Count;

			pBiNormals[3*pFaces[3*i+j]+0]/=(float)Count;
			pBiNormals[3*pFaces[3*i+j]+1]/=(float)Count;
			pBiNormals[3*pFaces[3*i+j]+2]/=(float)Count;

			pNormals[3*pFaces[3*i+j]+0]/=(float)Count;
			pNormals[3*pFaces[3*i+j]+1]/=(float)Count;
			pNormals[3*pFaces[3*i+j]+2]/=(float)Count;
		}
	}
	indexFree(&VertexIndex);

	stx_free(TriangleRefs);

#endif
}

D3DXFROMWINEVECTOR3 v4tov3(D3DXFROMWINEVECTOR4& a)
{
	D3DXFROMWINEVECTOR3 r;
	r.x=a.x;
	r.y=a.y;
	r.z=a.z;
	return r;
}
void MeshRenderer3::Grid(std::vector<stx_VertexPositionNormalTexture>& vertices, int gridX, int gridZ)
{
	vertices.resize(0);//(gridX * gridZ);
	indices.resize(0);
	//mesh->resize(gridX * gridZ, (gridX - 1) * (gridZ - 1) * 2);
	int vertCnt = 0;
	int faceCnt = 0;
	for (int x = 0; x < gridX; x++)
		for (int z = 0; z < gridZ; z++)
		{
			vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1 - (float) x / (gridX - 1), 0, 1 - (float) z / (gridZ - 1))));
	stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
			if (x < gridX - 1 && z < gridZ - 1)
			{
				indices.push_back(vertCnt + 0);
				indices.push_back(vertCnt + 1);
				indices.push_back(vertCnt + 1 + gridZ);
				indices.push_back(vertCnt + 1 + gridZ);
				indices.push_back(vertCnt + gridZ);
				indices.push_back(vertCnt + 0);
			}
			vertCnt++;
		}
	
}

void MeshRenderer3::setFace(unsigned int c, unsigned int i1, unsigned int i2, unsigned int i3)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}

void MeshRenderer3::makeGrid(int gridX, int gridZ)
{
	int vertCnt = 0;
	int faceCnt = 0;
	for (int x = 0; x < gridX; x++)
		for (int z = 0; z < gridZ; z++)
		{
			vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1 - (float) x / (gridX - 1), 0, 1 - (float) z / (gridZ - 1))));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
			if (x < gridX - 1 && z < gridZ - 1)
			{
				setFace(
					faceCnt++, vertCnt + 0, vertCnt + 1, vertCnt + 1 + gridZ
					);
				setFace(
					faceCnt++, vertCnt + 1 + gridZ, vertCnt + gridZ, vertCnt + 0
					);
			}
			vertCnt++;
		}
}
void MeshRenderer3::CreateHemis(tShader aShader_)
{
		Clear();
	m_Shape=eHemis;
		int stepLng = (int) 10*(4 * std::cbrt(1.0f) + 6);
        	int stepLat = (int) 10*(4 * std::cbrt(1.0f) + 6);
	makeGrid(stepLng + 1, (stepLat / 4) + 2);
	float unitLng = 2 * D3DXFROMWINE_PI / stepLng;
	float unitLat = D3DXFROMWINE_PI / (stepLat / 2);
	int vertCnt = 0;
	for (int i = 0; i < stepLng + 1; i++)
	{
		for (int j = 0; j < (stepLat / 4) + 1; j++)
		{
			D3DXFROMWINEVECTOR3 v(0, 1, 0);
		D3DXFROMWINEMATRIX RM;
			D3DXFROMWINEMatrixRotationYawPitchRoll(&RM, j * unitLat, i * unitLng, 0.0f);
		D3DXFROMWINEVec3TransformCoord(&v, &v, &RM);
			vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(v.x, v.y, v.z)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
			indices.push_back(vertices.size()-1);
		}
		vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
		indices.push_back(vertices.size()-1);
	}
        //CreateIndexBuffer(vertices.size(), vertices.size(), stepLng);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	texID=-1;
}

#if 1
void MeshRenderer3::CreateTorus(float radMajor, float radMinor, tShader aShader_)
{
		Clear();
	m_Shape=eTorus;
		int num_rings = (int) 10*(4 * std::cbrt(radMinor) + 6);
        	int num_segments = (int) 10*(4 * std::cbrt(radMajor) + 6);
	    int slices = (int) 10*(4 * std::cbrt(radMinor) + 6);
            int stacks = (int) 10*(4 * std::cbrt(radMajor) + 6);
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = D3DXFROMWINE_PI / stacks;
    // Compute torus vertices
    const float pi = D3DXFROMWINE_PI;
    const float r1 = 1.0f;
    const float r2 = 0.3f;
#if 1
    for(std::size_t i=0, index=0; i<=num_rings; ++i) {
      for(std::size_t j=0; j<=num_segments; ++j, ++index) {
#else
            float stackAngle = D3DXFROMWINE_PI - stackStep;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
#endif
        // Compute texture coordinates (surface parameters)
        // Note that u is added to j. This creates a spiral pattern, which
        // allows us to draw the entire torus in one triangle strip without
        // using degenerate triangles.
        // (Yet we still need degenerate triangles to avoid texture
        // wrap-around)
        float u = float(i) / num_rings;
        float v = (float(j) + u) / num_segments;

        // Compute angles
        float u_angle = u * 2 * pi;
        float v_angle = v * 2 * pi;

        // Position
        float x = cos(u_angle) * (r1 + cos(v_angle) * r2);
        float y = sin(u_angle) * (r1 + cos(v_angle) * r2);
        float z = sin(v_angle) * r2;

        // Normal vector
        float nx = cos(u_angle) * cos(v_angle);
        float ny = sin(u_angle) * cos(v_angle);
        float nz = sin(v_angle);

	stx_VertexPositionNormalTexture v3;
        v3.Position = D3DXFROMWINEVECTOR3(x, y, z);
        v3.Normal = D3DXFROMWINEVECTOR3(nx, ny, nz);
        v3.Tex = D3DXFROMWINEVECTOR2(u, v);
	stx_CalculatePositionalSphericalMapping(v3);
	vertices.push_back(v3);
#if 1
      }
    }
#else
                   sliceAngle += sliceStep;
                }
                stackAngle -= stackStep;
            }
#endif
#if 1
	{
    int idx = 0;

	int nbrRingSegments=num_rings;
        int nbrTubeSegments=num_segments;

	int nbrRingSteps=num_rings;
        int nbrTubes=num_segments;

    indices.resize(nbrRingSegments * nbrTubeSegments * 6);
    for (int t = 0; t < nbrTubeSegments; t++) {
        for (int r = 0; r < nbrRingSegments; r++) {
            int idxTL = r + t * nbrRingSteps;
            int idxBL = r + (t + 1) * nbrRingSteps;
            int idxBR = r + 1 + (t + 1) * nbrRingSteps;
            int idxTR = r + 1 + t * nbrRingSteps;
            // Top left triangle defined in anti-clockwise order
            indices[idx++] = idxTL;   // Point
            indices[idx++] = idxBL;   // Point
            indices[idx++] = idxTR;   // Point
            // Bottom right triangle defined in anti-clockwise order
            indices[idx++] = idxBL;   // Point
            indices[idx++] = idxBR;   // Point
            indices[idx++] = idxTR;   // Point
        }
    }}
#elif 1
    for(std::size_t i=0, index=0; i<=vertices.size(); ++i) {
      indices.push_back((int)(i % vertices.size()));
      indices.push_back((int)((i + num_segments+1) % vertices.size()));
    }
#elif +
	D3DXFROMWINEVECTOR3 aiVecs[2];
	D3DXFROMWINEMATRIX m;
	D3DXFROMWINEMatrixIdentity(&m);
	CalculateBounds(&aiVecs[0], &m);
	computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
	//unsigned int slices=num_rings;
	unsigned int slices=num_segments;
            unsigned int triangleCount = num_rings * (num_segments - 1) * 2;
            unsigned int indexCount = triangleCount * 3;
        CreateIndexBuffer(vertices.size(), indexCount, slices);
#endif

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	texID=-1;
}
#endif

void MeshRenderer3::CreateOcta(tShader aShader_)
{
		Clear();
	m_Shape=eOcta;
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 0, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0.5f, -1, 0.5f)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0.5f, 1, 0.5f)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	setFace(0, 0, 1, 4);
	setFace(1, 1, 2, 4);
	setFace(2, 2, 3, 4);
	setFace(3, 3, 0, 4);
	setFace(4, 0, 5, 1);
	setFace(5, 1, 5, 2);
	setFace(6, 2, 5, 3);
	setFace(7, 3, 5, 0);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	texID=-1;
}
void MeshRenderer3::CreateTetra(tShader aShader_)
{
		Clear();
	m_Shape=eTetra;
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 0, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 1, 0)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, 1, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(1, 0, 1)));
			stx_CalculatePositionalSphericalMapping(vertices[vertices.size()-1]);
	setFace(0, 0, 1, 3);
	setFace(1, 0, 2, 1);
	setFace(2, 0, 3, 2);
	setFace(3, 1, 2, 3);
	D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	texID=-1;
} 

void MeshRenderer3::CreateEllipsoid(float a, float b, float c, tShader aShader_)
        {
		Clear();
		m_Shape=eEllipsoid;
		int slices = (int) 10*(4 * std::cbrt(a) + 6);
        	int stacks = (int) 10*(4 * std::cbrt(b) + 6);
            
	    float sliceStep = 2.0f*D3DXFROMWINE_PI / slices;
            float stackStep = D3DXFROMWINE_PI / stacks;
            unsigned int vertexCount = slices * (stacks - 1) + 2;
            unsigned int triangleCount = slices * (stacks - 1) * 2;
            unsigned int indexCount = triangleCount * 3;
            std::vector<stx_VertexPositionNormalTexture> sphereVertices(vertexCount);
            unsigned int currentVertex = 0;
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, -c, 0), D3DXFROMWINEVECTOR3(0, -1, 0)));
            float stackAngle = D3DXFROMWINE_PI - stackStep;
            for (unsigned int i = 0; i < stacks - 1; i++)
            {
                float sliceAngle = 0;
                for (unsigned int j = 0; j < slices; j++)
                {
                    //NOTE: y and z were switched from normal spherical coordinates because the sphere is "oriented" along the Y axis as opposed to the Z axis
                    float x = (float)a*cosf(sliceAngle)*sinf(stackAngle);
                    float y = (float)c*cosf(stackAngle);
                    float z = (float)b*sinf(sliceAngle)*sinf(stackAngle);
                    D3DXFROMWINEVECTOR3 position(x, y, z);
			D3DXFROMWINEVECTOR3 normal(x, y, z);
			D3DXFROMWINEVec3Normalize(&normal, &normal);
    			float tu = asinf(normal.x)/D3DXFROMWINE_PI + 0.5;
    			float tv = -asinf(normal.y)/D3DXFROMWINE_PI - 0.5;
                   vertices.push_back(stx_VertexPositionNormalTexture(position, normal, D3DXFROMWINEVECTOR2(tu, -(tv-1.0f)), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f)));
                    sliceAngle += sliceStep;
                }
                stackAngle -= stackStep;
            }
            vertices.push_back(stx_VertexPositionNormalTexture(D3DXFROMWINEVECTOR3(0, c, 0), D3DXFROMWINEVECTOR3(0, 1, 0)));

		D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertexCount);
            CreateIndexBuffer(vertexCount, indexCount, slices);

	stx_Mesh mesh;
	meshes.push_back(mesh);
	meshes[0].vertices=vertices;vertices.clear();
	meshes[0].indices=indices;
	meshes[0].rindices.push_back(indices);indices.clear();meshes[0].indices.clear();

	if(aShader_!=eShaderNone)
		InitShader(aShader_);
	texID=-1;
}

//-------------------------------------------------------------------------------
// Calculate the boundaries of a given node and all of its children
// The boundaries are in Worldspace (AABB)
// piNode Input node
// p_avOut Receives the min/max boundaries. Must point to 2 D3DXFROMWINEVECTOR3s
// piMatrix Transformation matrix of the graph at this position
//-------------------------------------------------------------------------------
D3DXFROMWINEMATRIX MeshRenderer3::CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut, D3DXFROMWINEMATRIX* piMatrix)
{
	p_avOut[0] = D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
	p_avOut[1] = D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
	for( unsigned int a = 0; a < vertices.size();++a)
	{
		D3DXFROMWINEVECTOR3 pc1 = vertices[a].Position;
		p_avOut[0].x = std::min( p_avOut[0].x, pc1.x);
		p_avOut[0].y = std::min( p_avOut[0].y, pc1.y);
		p_avOut[0].z = std::min( p_avOut[0].z, pc1.z);
		p_avOut[1].x = std::max( p_avOut[1].x, pc1.x);
		p_avOut[1].y = std::max( p_avOut[1].y, pc1.y);
		p_avOut[1].z = std::max( p_avOut[1].z, pc1.z);
	}

	D3DXFROMWINEVECTOR3 vDelta = p_avOut[1]-p_avOut[0];
	D3DXFROMWINEVECTOR3 vHalf =  p_avOut[0] + (vDelta / 2.0f);
	float fScale = 2.0f / D3DXFROMWINEVec3Length(&vDelta);

	D3DXFROMWINEMATRIX m_mWorld =  D3DXFROMWINEMATRIX(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
		D3DXFROMWINEMATRIX(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);

	if(0)
	for( unsigned int j = 0; j < vertices.size();++j)
	{
		D3DXFROMWINEVec3TransformCoord(&vertices[j].Position, &vertices[j].Position, &m_mWorld);
		if(piMatrix)
			D3DXFROMWINEVec3TransformCoord(&vertices[j].Position, &vertices[j].Position, piMatrix);
	}
	if(piMatrix)
		m_mWorld*=(*piMatrix);
	return m_mWorld;
}
#if 0
void MeshRenderer3::GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max)
{
	D3DXFROMWINEVECTOR3 aiVecs[2] = {D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f),
		D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f) };

	D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);

	____min = *(D3DXFROMWINEVECTOR3*)&aiVecs[0];
	____max = *(D3DXFROMWINEVECTOR3*)&aiVecs[1];
}
#endif
    	D3DXFROMWINEVECTOR3 MeshRenderer3::GetMeshBBoxExtents()
	{
		D3DXFROMWINEVECTOR3 __min;
		D3DXFROMWINEVECTOR3 __max;
		GetBounds(__min, __max);
		return __max;
	}
    	D3DXFROMWINEVECTOR3 MeshRenderer3::GetMeshBBoxCenter()
	{
		D3DXFROMWINEVECTOR3 __min;
		D3DXFROMWINEVECTOR3 __max;
		GetBounds(__min, __max);
		return __max/2.0f;// is this correct ???
	}
void MeshRenderer3::CreateMesh(const char* afn)
{
		Clear();
	m_Shape=eMesh;
	vertices.clear();
	indices.clear();
	MeshRenderer3 mesh_;
	mesh_.Load(afn, eSimpleShader, false);
	//std::vector<stx_Mesh> meshes;
	if(mesh_.meshes.size())
	{
		vertices=mesh_.meshes[0].vertices;
		indices=mesh_.meshes[0].indices;
	}
}

/*
# The L-system for the 3D-modelling of an house.

# scene
# highway
# road
# path
# yard
# house
# wall
# door
# window
# ceiling/roof
# chimney
# rain gutter

# VertexVector
# IndexVector
# TRIANGLESTRIP
# TRIANGLE
*/
/*
http://paulbourke.net/geometry/platonic/:
The measured properties of the 3 dimensional regular polyhedra
Tetrahedron

	Vertices: 4
Edges: 6
Faces: 4
Edges per face: 3
Edges per vertex: 3
Sin of angle at edge: 2 * sqrt(2) / 3
Surface area: sqrt(3) * edgelength^2
Volume: sqrt(2) / 12 * edgelength^3
Circumscribed radius: sqrt(6) / 4 * edgelength
Inscribed radius: sqrt(6) / 12 * edgelength

Coordinates

 1  1  1   -1  1 -1    1 -1 -1
-1  1 -1   -1 -1  1    1 -1 -1
 1  1  1    1 -1 -1   -1 -1  1
 1  1  1   -1 -1  1   -1  1 -1

Divide each coordinate by 2
Octahedron

	Vertices: 6
Edges: 12
Faces: 8
Edges per face:3
Edges per vertex: 4
Sin of angle at edge: 2 * sqrt(2) / 3
Surface area: 2 * sqrt(3) * edgelength^2
Volume: sqrt(2) / 3 * edgelength^3
Circumscribed radius: sqrt(2) / 2 * edgelength
Inscribed radius: sqrt(6) / 6 * edgelength

Coordinates

-a  0  a   -a  0 -a    0  b  0
-a  0 -a    a  0 -a    0  b  0
 a  0 -a    a  0  a    0  b  0
 a  0  a   -a  0  a    0  b  0
 a  0 -a   -a  0 -a    0 -b  0
-a  0 -a   -a  0  a    0 -b  0
 a  0  a    a  0 -a    0 -b  0
-a  0  a    a  0  a    0 -b  0

Where a = 1 / (2 * sqrt(2)) and b = 1 / 2
Hexahedron (cube)

	Vertices: 8
Edges: 12
Faces: 6
Edges per face: 4
Edges per vertex: 3
Sin of angle at edge: 1
Surface area: 6 * edgelength^2
Volume: edgelength^3
Circumscribed radius: sqrt(3) / 2 * edgelength
Inscribed radius: 1 / 2 * edgelength

Coordinates

-1 -1 -1    1 -1 -1    1 -1  1   -1 -1  1
-1 -1 -1   -1 -1  1   -1  1  1   -1  1 -1
-1 -1  1    1 -1  1    1  1  1   -1  1  1
-1  1 -1   -1  1  1    1  1  1    1  1 -1
 1 -1 -1    1  1 -1    1  1  1    1 -1  1
-1 -1 -1   -1  1 -1    1  1 -1    1 -1 -1

Divide each vertex by 2
Icosahedron

	Vertices: 12
Edges: 30
Faces: 20
Edges per face: 3
Edges per vertex: 5
Sin of angle at edge: 2 / 3
Surface area: 5 * sqrt(3) * edgelength^2
Volume: 5 * (3 + sqrt(5)) / 12 * edgelength^3
Circumscribed radius: sqrt(10 + 2 * sqrt(5)) / 4 * edgelength
Inscribed radius: sqrt(42 + 18 * sqrt(5)) / 12 * edgelength

Coordinates

 0  b -a    b  a  0   -b  a  0
 0  b  a   -b  a  0    b  a  0
 0  b  a    0 -b  a   -a  0  b
 0  b  a    a  0  b    0 -b  a
 0  b -a    0 -b -a    a  0 -b
 0  b -a   -a  0 -b    0 -b -a
 0 -b  a    b -a  0   -b -a  0
 0 -b -a   -b -a  0    b -a  0
-b  a  0   -a  0  b   -a  0 -b
-b -a  0   -a  0 -b   -a  0  b
 b  a  0    a  0 -b    a  0  b
 b -a  0    a  0  b    a  0 -b
 0  b  a   -a  0  b   -b  a  0
 0  b  a    b  a  0    a  0  b
 0  b -a   -b  a  0   -a  0 -b
 0  b -a    a  0 -b    b  a  0
 0 -b -a   -a  0 -b   -b -a  0
 0 -b -a    b -a  0    a  0 -b
 0 -b  a   -b -a  0   -a  0  b
 0 -b  a    a  0  b    b -a  0

Where a = 1 / 2 and b = 1 / (2 * phi)
phi is the golden ratio = (1 + sqrt(5)) / 2

Contribution by Craig Reynolds: vertices and faces for the icosahedron. Along with C++ code to create a sphere based upon the icosahedron: sphere.cpp, see also surface refinement for related ideas.
Dodecahedron

	Vertices: 20
Edges: 30
Faces: 12
Edges per face: 5
Edges per vertex: 3
Sin of angle at edge: 2 / sqrt(5)
Surface area: 3 * sqrt(25 + 10 * sqrt(5)) * edgelength^2
Volume: (15 + 7 * sqrt(5)) / 4 * edgelength^3
Circumscribed radius: (sqrt(15) + sqrt(3)) / 4 * edgelength
Inscribed radius: sqrt(250 + 110 * sqrt(5)) / 20 * edgelength

Coordinates

 c  0  1   -c  0  1   -b  b  b    0  1  c    b  b  b
-c  0  1    c  0  1    b -b  b    0 -1  c   -b -b  b
 c  0 -1   -c  0 -1   -b -b -b    0 -1 -c    b -b -b
-c  0 -1    c  0 -1    b  b -b    0  1 -c   -b  b -b
 0  1 -c    0  1  c    b  b  b    1  c  0    b  b -b
 0  1  c    0  1 -c   -b  b -b   -1  c  0   -b  b  b
 0 -1 -c    0 -1  c   -b -b  b   -1 -c  0   -b -b -b
 0 -1  c    0 -1 -c    b -b -b    1 -c  0    b -b  b
 1  c  0    1 -c  0    b -b  b    c  0  1    b  b  b
 1 -c  0    1  c  0    b  b -b    c  0 -1    b -b -b
-1  c  0   -1 -c  0   -b -b -b   -c  0 -1   -b  b -b
-1 -c  0   -1  c  0   -b  b  b   -c  0  1   -b -b  b

Where b = 1 / phi and c = 2 - phi
Divide each coordinate by 2.
*/

