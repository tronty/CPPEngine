/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* filename[] =
{
	"/Smoke/fireBase.png",
	//"/textures/NVIDIA_Corporation/2d/flame.png",
	//"/NatureScene/Sky/clouds.png",
	//"/AdvAniCh12/Particle_Smoke.bmp",
	//"/Orcs/Firetex.png",
	"/textures/ATI_SDK/Spectrum.png",
	"/DeferredParticles/DeferredParticle.png",
	"/12_Explosion_Particle_Effect/explosion.png",
        "/dx8sdk/tree01S.png",
        "/dx8sdk/tree02S.png",
        "/dx8sdk/tree35S.png",
        "/textures/NVIDIA_Corporation/2d/Palm.png",
        "/textures/ATI_SDK/grass001.png",
        "/textures/ATI_SDK/grass002.png",
	//"/new/R2VB_VTF/R2VB-Sort/grass001.png",
	//"/new/R2VB_VTF/R2VB-Sort/grass002.png",
	//"/new/R2VB_VTF/R2VB-Sort/MapleLeaves.png",
        "/AdvAniCh12/Particle_People1.png",
        "/AdvAniCh12/Particle_People2.png",
	"/Smoke/FireTransferFunction.png",
	//"/textures/NVIDIA_Corporation/2d/cloudmix.png",
	"/new/R2VB_VTF/R2VB-Sort/FirePalette.png",
	"/textures/NVIDIA_Corporation/1D/fire_gradient3.png",
	"/textures/NVIDIA_Corporation/1D/FireGrade.png",
	//"/new/R2VB_VTF/R2VB-Sort/SmokeAnimation.dds",// black ??? TRILINEAR, CLAMP
};

ShaderID m_pShader=-1;
VertexFormatID m_pDecl=-1;
ShaderID shd=-1;
TextureID texID=-1;
VertexFormatID vf=-1;

int init(const char* aTitle)
{
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.shd", "mainVS", "mainPS");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);

        SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	if(!stx_fileExists("/atlas.png"))
	{
	std::vector<std::string> av;
	for(unsigned int i=0;i<elementsOf(filename);i++)
		av.push_back(filename[i]);

	Image3 atlas;
	stx_CreateAtlas(atlas, av);
	texID=IRenderer::GetRendererInstance()->addTexture(&atlas, false, ss, 0);
	if(0)
	{
		std::string fn=stx_convertpath("/atlas.png");
		//atlas.compress();
		atlas.saveImageLibImage(fn.c_str(), false);
	}
	}
	else
		texID=IRenderer::GetRendererInstance()->addImageLibTexture("/atlas.png", false, ss);

	//Image3 img2D=stx_Create2DNoiseTexture(64, 64);

	m_pShader=IRenderer::GetRendererInstance()->addShaderFromFile("/ParticleVS/Particles3.shd", "main", "main");
	FormatDesc g_VSParticleDecl[] = {
		0, TYPE_VERTEX, FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
	};
	m_pDecl = IRenderer::GetRendererInstance()->addVertexFormat(g_VSParticleDecl, elementsOf(g_VSParticleDecl),m_pShader);

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setTexture("Base", texID);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	//		   x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f };
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("AtlasTexture");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

