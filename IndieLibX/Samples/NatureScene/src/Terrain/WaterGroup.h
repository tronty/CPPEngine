/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class _3DAnimation
{
public:
	_3DAnimation()
	{
		m_Mesh=0;
		init();
	}
	virtual ~_3DAnimation(){delete m_Mesh;}
	MeshRenderer2* m_Mesh;
	ShaderID shd;
	TextureID reflection;
	TextureID normalmap;
	TextureID depthmap;
	VertexFormatID vf;
	virtual void init(const char* aTitle="")
	{
		shd = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/WaterShader.shd", "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		};
		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);		SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
		=IRenderer::GetRendererInstance()->addImageLibTexture("/", false, ss);
	
#if 0
		m_Mesh=FW3::GetInstance()->GetMesh("/NatureScene/Water/watermesh.3ds", eSimpleShader);
#else
		m_Mesh=new MeshRenderer2("/NatureScene/Water/watermesh.3ds", eSimpleShader);
#endif

		m_Mesh->SetPlay(false);
		m_Mesh->SetAnimIndex(0);
		m_Mesh->SetPlay(true);
		size_t n=m_Mesh->GetNumAnimations();
		LOG_PRINT("NumAnimations=%d\n", n);
	}
	virtual void render(void)
	{
		IRenderer::GetRendererInstance()->setShader(shd);
		IRenderer::GetRendererInstance()->setVertexFormat(vf);
		IRenderer::GetRendererInstance()->setShaderConstant4f("translation", D3DXFROMWINEVECTOR4 (, 0.0f, 0.0f, 0.0f, 0.0f); 
		IRenderer::GetRendererInstance()->setShaderConstant4f("camera", D3DXFROMWINEVECTOR4 (0.0f, 0.0f, 0.0f, 0.0f); 
		IRenderer::GetRendererInstance()->setShaderConstant1f("timer", timeGetTime()/1000.0f);
		IRenderer::GetRendererInstance()->setTexture("reflection", -1);
		IRenderer::GetRendererInstance()->setTexture("normalmap", -1);
		IRenderer::GetRendererInstance()->setTexture("depthmap", -1);
		IRenderer::GetRendererInstance()->setShaderConstant4f("terrainInfo", D3DXFROMWINEVECTOR4 (, 0.0f, 0.0f, 0.0f, 0.0f); 
		D3DXFROMWINEMATRIX T;
		D3DXFROMWINEMatrixTranslation(&T, 510.0f, -10.0f, 1500.0f);
		m_Mesh->render(T);
	}
};
#endif

