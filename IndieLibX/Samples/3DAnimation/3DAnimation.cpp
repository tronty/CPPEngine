/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#undef DRAWGLOBE

const char* filename[] =
{
//"/MotionBlur/Warrior.x",
//"/dx9sdk/sphere.x",
//"/dx9sdk/sphere2.x",
"/skeleton/skeleton.x",
"/skeleton/skeleton.bin.x",
0
};

class _3DAnimation : public STXGUI {
public:
	_3DAnimation(){}
	virtual ~_3DAnimation(){}
	MeshRenderer2 m_Mesh;
	TextureID texID;
	virtual void actionPerformed(GUIEvent &evt)
{
  const std::string &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
	GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
	unsigned int i=0;
	if(radioButton->isPressed() && radioButton->isChecked())
	{
		while(i<5)
		{
			char buf[256] = {0};
			stx_snprintf(buf, 256, "Skeleton_%d\0", i+1);
      			if(callbackString == buf)
			{
				m_Mesh.SetPlay(false);
				m_Mesh.SetAnimIndex(i);
				m_Mesh.SetPlay(true);
				break;
			}
			i++;
		}
	}
  }
}
	virtual void init(const char* aTitle="")
	{

		STXGUI::init("/skeleton/GUILayout.xml");
        texID=-1;
#ifndef DRAWGLOBE
#if 0
		FW3::GetInstance()->GetMesh(m_Mesh. filename[0], eSimpleShader);
#elif 1
		m_Mesh.Load(filename[0], eSimpleShader, true, true);
#elif 0
		m_Mesh.Load(filename[0], ePhongLighting1, true, true);
#endif
#else
		//m_Mesh.CreateSphere(1.0f, eShaderNone);
		m_Mesh.CreateSphere(1.0f, eSimpleShader);
		SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
		//ss=IRenderer::GetRendererInstance()->GetnearestClamp();
		texID=IRenderer::GetRendererInstance()->addImageLibTexture("/dx9sdk/earth.bmp", false, ss);
		//texID=IRenderer::GetRendererInstance()->addImageLibTexture("/AccurateAtmosphericalScattering/earthmap1k.jpg", true, ss);
		//texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", true, ss);
#endif

		m_Mesh.SetPlay(false);
		m_Mesh.SetAnimIndex(0);
		m_Mesh.SetPlay(true);
		size_t n=m_Mesh.GetNumAnimations();
		LOG_PRINT("NumAnimations=%d\n", n);
		LOG_PRINT("m_Mesh.vertices.size()=%d\n", m_Mesh.vertices.size());
		LOG_PRINT("m_Mesh.indices.size()=%d\n", m_Mesh.indices.size());
if(m_Mesh.meshes.size())
{
		LOG_PRINT("m_Mesh.meshes[0].vertices.size()=%d\n", m_Mesh.meshes[0].vertices.size());
		LOG_PRINT("m_Mesh.meshes[0].indices.size()=%d\n", m_Mesh.meshes[0].indices.size());
}
	if(0)
	{
	std::vector<stx_Material> vt;
	std::vector<__WORD__> g_indices2;
	std::vector<stx_Mesh> mesh_;
	stx_Mesh m;
	m.vertices=m_Mesh.meshes[0].vertices;
	m.indices=m_Mesh.meshes[0].indices;

	for(unsigned int i=0;i<m_Mesh.meshes[0].textures.size();i++)
		if(m_Mesh.meshes[0].textureNames[i]=="BONES.BMP")
			for(unsigned int j=0;j<m_Mesh.meshes[0].rindices.size()/3;j++)
				g_indices2.push_back(i);
		else
			for(unsigned int j=0;j<m_Mesh.meshes[0].rindices.size()/3;j++)
				g_indices2.push_back(0);

	mesh_.push_back(m);
	stx_Materials_(vt, g_indices2, mesh_);
	stx_writeXFile("/assimp--1.0.412-sdk/test2.x", mesh_);
	}
	}
	virtual void render(void)
	{

	IRenderer::GetRendererInstance()->Clear(true,true, D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	D3DXFROMWINEMATRIX mvp, R, S, T;
	float mAngleX=0.0f;
	float mAngleY=0.0f;
	if
	(0)
	//(std::string(filename[0])=="/Orcs/Bonfire_wOrcs.x")
	{
    		float s=1.0f;
    		D3DXFROMWINEVECTOR3 t(0.0f, 0.0f, -7.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX+18.37f),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
		D3DXFROMWINEMatrixScaling( &S, s, s, s);
		D3DXFROMWINEMatrixTranslation( &T, t.x, t.y, t.z);
    		mvp= R*S*T;
	} else {
		stx_GetAngles(mAngleX, mAngleY, 0.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
		}
#ifndef DRAWGLOBE
	m_Mesh.render(&mvp, texID);
#else
	D3DXFROMWINEVECTOR4 a(0.0f, 0.1f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR4 d(0.0f, 1.0f, 0.0f, 1.0f);
	D3DXFROMWINEVECTOR4 l(0.0f, 1.0f, -1.0f, 1.0f);
	D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
	/* D3DXFROMWINEVECTOR4 gold=stx_GetRGBA(eGold);
	D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
	a=silver;
	d=silver; */
        m_Mesh.Draw(&mvp, texID, -1, -1, a, d, l, e);
#endif
	STXGUI::update();

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
	IRenderer::GetRendererInstance()->Present();
	}
};
int ApplicationLogic()
{
	//LOG_PRINT("elementsOf(filename)=%d\n", elementsOf(filename));
	IRenderer* r=IRenderer::GetRendererInstance("3DAnimation");
	IInput*    i=STX_Service::GetInputInstance();
	_3DAnimation app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}
/*
tronty@tronty-OMEN-by-HP-Desktop-PC-880-p1xx:~/IndieLibX_assimp_4.0.1_bin/IndieLibX$ ./bin/3DAnimation.bin
fn.c_str()=../../IndieLib_resources/Framework3/Framework3.xml
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
SDL_Init1
SDL_Init2
m_dpy=b1731c60
m_win=5e0000e
g_X11_display=b1731c60
g_X11_window=5e0000e
SDL_window_=b1a9f640
GL_MAX_TEXTURE_IMAGE_UNITS=32
GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS=32
GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS=128
GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS=128
GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS=128
GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS=128
GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS=32
GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT=0.000000
GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT=0.000000
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/NSGUI/GUIElements.png
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/NSGUI/ArialSmall.tga
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/NSGUI/ArialSmall.tga
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
MeshRenderer2::LoadThreadProc2:aFileNameMesh=../../IndieLib_resources/dx9sdk/sphere2.x
mesh->mFaces=b1aeaa00
mesh->mVertices=b235d4a0
mesh->mNormals=b23cce40
mesh->mTangents=b23d3cc0
mesh->mBitangents=b23b22b0
mesh->mTextureCoords[0]=b23b9130
../../../Libraries/MeshRenderer2/MeshRenderer2.cpp:LoadThreadProc2:669
s:../AccurateAtmosphericalScattering/earthmap1k.jpg
f1:../AccurateAtmosphericalScattering/earthmap1k.jpg
txfn:../../IndieLib_resources/dx9sdk/../AccurateAtmosphericalScattering/earthmap1k.jpg
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/dx9sdk/../AccurateAtmosphericalScattering/earthmap1k.jpg
../../../Libraries/MeshRenderer2/MeshRenderer2.cpp:LoadFile:778
meshes.size()=1
meshes[0].vertices.size()=2356
../../../Libraries/MeshRenderer2/MeshRenderer2.cpp:LoadFile:796
vertices.size()=0
indices.size()=0
meshes[0].vertices.size()=2356
meshes[0].indices.size()=0
min.x=-0.577350
min.y=-0.577350
min.z=-0.577350
max.x=0.577350
max.y=0.577350
max.z=0.577350
NumAnimations=0
m_Mesh.vertices.size()=0
m_Mesh.indices.size()=0
m_Mesh.meshes[0].vertices.size()=2356
m_Mesh.meshes[0].indices.size()=0
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/Framework3/Future.dds
tronty@tronty-OMEN-by-HP-Desktop-PC-880-p1xx:~/IndieLibX_assimp_4.0.1_bin/IndieLibX$ ./bin/3DAnimation.bin

tronty@tronty-OMEN-by-HP-Desktop-PC-880-p1xx:~/IndieLibX_assimp_4.0.1_src/IndieLibX$ ./bin/3DAnimation.bin
fn.c_str()=../../IndieLib_resources/Framework3/Framework3.xml
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
SDL_Init1
SDL_Init2
m_dpy=84931760
m_win=1180000e
g_X11_display=84931760
g_X11_window=1180000e
SDL_window_=849a5da0
GL_MAX_TEXTURE_IMAGE_UNITS=32
GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS=32
GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS=128
GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS=128
GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS=128
GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS=128
GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS=32
GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT=0.000000
GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT=0.000000
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/NSGUI/GUIElements.png
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/NSGUI/ArialSmall.tga
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/NSGUI/ArialSmall.tga
../../../Libraries/tinyxml/src/tinyxml.cpp:LoadFile:1010
filename=../../IndieLib_resources/Framework3/Framework3.xml
MeshRenderer2::LoadThreadProc2:aFileNameMesh=../../IndieLib_resources/dx9sdk/sphere2.x
mesh->mFaces=84cf8530
mesh->mVertices=85570d40
mesh->mNormals=85572850
mesh->mTangents=84cf2950
mesh->mBitangents=8556ddc0
mesh->mTextureCoords[0]=84cf0e40
mesh->mFaces=854d58c0
mesh->mVertices=8557c580
mesh->mNormals=855af120
mesh->mTangents=8569bee0
mesh->mBitangents=856cea80
mesh->mTextureCoords[0]=855e1cc0
../../../Libraries/MeshRenderer2/MeshRenderer2.cpp:LoadThreadProc2:669
f1:../AccurateAtmosphericalScattering/earthmap1k.jpg
txfn:../../IndieLib_resources/dx9sdk/../AccurateAtmosphericalScattering/earthmap1k.jpg
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/dx9sdk/../AccurateAtmosphericalScattering/earthmap1k.jpg
../../../Libraries/MeshRenderer2/MeshRenderer2.cpp:LoadFile:774
meshes.size()=1
meshes[0].vertices.size()=288
../../../Libraries/MeshRenderer2/MeshRenderer2.cpp:LoadFile:792
vertices.size()=8657
indices.size()=0
meshes[0].vertices.size()=288
meshes[0].indices.size()=0
min.x=-0.065125
min.y=-0.995750
min.z=-0.065125
max.x=0.065125
max.y=0.995750
max.z=0.065125
NumAnimations=0
m_Mesh.vertices.size()=8657
m_Mesh.indices.size()=0
m_Mesh.meshes[0].vertices.size()=288
m_Mesh.meshes[0].indices.size()=0
../../../Libraries/Image3/Image.cpp:loadImageLibImage:504
filename=../../IndieLib_resources/Framework3/Future.dds
tronty@tronty-OMEN-by-HP-Desktop-PC-880-p1xx:~/IndieLibX_assimp_4.0.1_src/IndieLibX$ 
*/

