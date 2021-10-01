/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* filename[] =
{
#if 1
#if 0
"/new/R2VB_VTF/R2VB-Sort/particles.shd",
"/vnoise.shd",
//"/DDSAnimation/DDSAnimation.shd",
"/HLSL-noise2/HLSL-noise3.fx",
//"/vnoise2.shd",
"/noise/noise3D2.shd",
"/noise/noise3D.shd",
"/noise/noise2D.shd",
//"/Framework3/Effect.shd",
//"/Framework3/Effect2.shd",
//"/HLSL-noise/HLSL-noise2.shd",
//"/HLSL-noise/HLSL-noise5.shd",
//"/HLSL-noise/HLSL-noise1.shd",
//"/HLSL-noise/HLSL-noise3.shd",
//"/HLSL-noise/HLSL-noise4.shd"
#endif
//"/www.shadertoy.com/Commodore_64.shd",
#if 0
"/www.shadertoy.com/Road01.shd",
"/www.shadertoy.com/Road02.shd",
"/glslsandbox.com/e#61428.0.shd",
"/glslsandbox.com/e#63847.0.shd",
#endif
"/www.shadertoy.com/Super_simple_raymarching_example.shd",
"/www.shadertoy.com/Ray_Marching_Part_1.shd",
"/www.shadertoy.com/Ray_Marching_Part_2.shd",
"/www.shadertoy.com/Ray_Marching_Part_3.shd",
"/SDF.shd",
"/www.shadertoy.com/simple_Starfield.shd",
"/www.shadertoy.com/Skin_peeler.shd",
"/www.shadertoy.com/The_One_Ring.shd",
"/www.shadertoy.com/SimpleStars.shd",
"/www.shadertoy.com/The_Cave.shd",
"/www.shadertoy.com/Seacape.shd",
"/www.shadertoy.com/Desert_biome.shd",
"/www.shadertoy.com/Sandstone_canyons.shd",
"/www.shadertoy.com/7dyGWV.shd",
"/www.shadertoy.com/Mandelbrot_smooth.shd",
"/www.shadertoy.com/Logistic_Map_Real_Bifurcation.shd",
"/www.shadertoy.com/Logistic_Map_Complex.shd",
"/www.shadertoy.com/Menger_Sponge.shd",
"/www.shadertoy.com/Chaos_Game_Sierpinsky_Triangle.shd",
"/www.shadertoy.com/Hashscape.shd",
"/www.shadertoy.com/video_heightfield2.shd",
"/www.shadertoy.com/OCEAN_SURFACE_NOISE_FBM.shd",
"/www.shadertoy.com/TileableNoise.shd",
//"/www.shadertoy.com/InfiniteTerrainII.shd",
"/glslsandbox.com/e#59382.0.shd",
"/www.shadertoy.com/Mountains.shd",
"/www.shadertoy.com/Seascape.shd",
"/www.shadertoy.com/Tree_in_the_wind.shd",
"/www.shadertoy.com/301FireShaderRemix1.shd",
"/www.shadertoy.com/301FireShaderRemix2.shd",
"/www.shadertoy.com/301FireShaderRemix3.shd",
"/www.shadertoy.com/AdaptiveSamplingDiagram.shd",
"/www.shadertoy.com/AlienBeacon.shd",
"/www.shadertoy.com/BallOfFire.shd",
"/www.shadertoy.com/CheapCloudFlythrough.shd",
"/www.shadertoy.com/CheapCurvature.shd",
//"/www.shadertoy.com/Common.shd",
"/www.shadertoy.com/dustynebula1.shd",
"/www.shadertoy.com/explosionno12.shd",
"/www.shadertoy.com/explosionno13.shd",
"/www.shadertoy.com/FakeVolumetricClouds.shd",
"/www.shadertoy.com/flame.shd",
"/www.shadertoy.com/Flameinthewind.shd",
"/www.shadertoy.com/Flamethrower.shd",
"/www.shadertoy.com/fires.shd",
#endif
"/www.shadertoy.com/FireShader.shd",
"/www.shadertoy.com/HashwithoutSine.shd",
"/www.shadertoy.com/RaymarchingSampleCode.shd",
"/www.shadertoy.com/pyroclasticexplosion.shd",
"/www.shadertoy.com/PyroclasticFireball.shd",
"/www.shadertoy.com/Simplestarfield.shd",
"/www.shadertoy.com/Simplicity.shd",
"/www.shadertoy.com/Simplicity3D.shd",
"/www.shadertoy.com/SpaceElevator.shd",
"/www.shadertoy.com/StructuredSamplingDiagram.shd",
"/www.shadertoy.com/Xyptonjtroz.shd",
"/www.shadertoy.com/anatomyofanexplosion.shd",
"/www.shadertoy.com/campfire.shd",
"/www.shadertoy.com/Clouds.shd",
"/www.shadertoy.com/CloudTen.shd",
"/www.shadertoy.com/CloudySpikeBall.shd",
"/www.shadertoy.com/DustyNebula3.shd",
"/www.shadertoy.com/DustyNebula4.shd",
"/www.shadertoy.com/EmissionClouds.shd",
"/www.shadertoy.com/explosioneffect.shd",
"/www.shadertoy.com/gasexplosion.shd",
"/www.shadertoy.com/Hell.shd",
"/www.shadertoy.com/ProtoplanetaryDisk.shd",
"/www.shadertoy.com/SamplePinning.shd",
"/www.shadertoy.com/StructuredVolSampling.shd",
"/www.shadertoy.com/supernova.shd",
"/www.shadertoy.com/SupernovaRemnant.shd",
"/www.shadertoy.com/type2supernova.shd",
"/www.shadertoy.com/volumetricexplosion.shd",
"/www.shadertoy.com/Watchitburn.shd",
"/glslsandbox.com/e#72423.0.shd",
"/glslsandbox.com/e#68825.0.shd",
"/glslsandbox.com/e#72317.0.shd",
"/glslsandbox.com/e#72645.0.shd",
"/glslsandbox.com/e#67654.0.shd",
"/glslsandbox.com/e#68826.0.shd",
"/glslsandbox.com/e#72421.0.shd",
"/glslsandbox.com/e#73091.1.shd",
"/glslsandbox.com/e#68576.0.shd",
"/glslsandbox.com/e#68827.0.shd",
"/glslsandbox.com/e#73092.0.shd",
"/glslsandbox.com/e#68577.0.shd",
"/glslsandbox.com/e#68829.0.shd",
"/glslsandbox.com/e#72444.0.shd",
"/www.shadertoy.com/XslGRr.shd",
"/www.shadertoy.com/Soccermania.shd",
"/www.shadertoy.com/TDM_Seascape_Sailing.shd",
"/www.shadertoy.com/Fantasy_World_Map.shd"
};

class _Shape3D
{
	TextureID texID;
	MeshRenderer2 shape3D[3];
	std::vector<ShaderID> shader;
	//std::vector<std::string> ShaderFiles;
	std::vector<VertexFormatID> vf;
    std::vector<TextureID> tex;
	unsigned int s_i, m_i, t_i;
public:
int init(const char* aTitle)
{
    	t_i=0;
    	m_i=2;
    	s_i=0;
    	#if 0
	shape3D[0].CreateTexturedQuad(2.0f, 2.0f, eShaderNone);
        shape3D[1].CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
        shape3D[2].CreateSphere(1.0f, eShaderNone);
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
	#elif 0
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Sky/clouds.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/Smoke/FireTransferFunction.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/Smoke/fireBase.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/flame.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/1D/fire_gradient3.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/1D/FireGrade.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/new/R2VB_VTF/R2VB-Sort/FirePalette.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/Spectrum.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	#elif 0
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/www.shadertoy.com/world1.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/www.shadertoy.com/cloud1.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/www.shadertoy.com/light1.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	#elif 1
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/ViewportProjectionContent/bluetexture.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/ViewportProjectionContent/greentexture.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/ViewportProjectionContent/redtexture.png", false, IRenderer::GetRendererInstance()->Getlinear()));	
	#endif

	for(unsigned int i=0;i<elementsOf(filename);i++)
	{	
			//printf("Shader=%s failed!\n", filename[i]);
		ShaderID id=IRenderer::GetRendererInstance()->addShaderFromFile(filename[i], "main2", "main");
		if(id==-1)
		{
			printf("Shader=%s failed!\n", filename[i]);
			//stx_exit(0);
			//continue;
		}
		shader.push_back(id);
		
		//ShaderFiles.push_back(filename[i]);
		#if 1
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
		#else
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
			0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
			0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
			0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		};
		#endif
		if(shader[shader.size()-1]!=-1)
		vf.push_back(IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shader[shader.size()-1]));
		else
		vf.push_back(-1);
	}

	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
	{
		if(!s_i)
			s_i=shader.size()-1;
		else
			s_i--;
		while((shader[s_i]==-1)||(vf[s_i]==-1))
		{
			if(!s_i)
				s_i=shader.size()-1;
			else
				s_i--;
		}
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		s_i++;
		if(s_i>shader.size()-1)
			s_i=0;
		while((shader[s_i]==-1)||(vf[s_i]==-1))
		{
			s_i++;
			if(s_i>shader.size()-1)
				s_i=0;
		}
	}

	float mAngleX=21.0f;
	float mAngleY=112.0f;
//if((shader[s_i]>-1)&&(vf[s_i]>-1))
{
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX matRot;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &matRot,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

					D3DXFROMWINEVECTOR4 a(0.1f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 d(1.0f, 0.0f, 0.0f, 1.0f);
					D3DXFROMWINEVECTOR4 l(0.0f, 1.0f, -1.0f, 1.0f);
					D3DXFROMWINEVECTOR4 e(0.0f, 0.0f, -1.0f, 1.0f);
					D3DXFROMWINEVECTOR4 gold=stx_GetRGBA(eGold);
					D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
					a=silver;
					d=silver;

	D3DXFROMWINEVECTOR2 vMouse(1.0f,1.0f);//((float)STX_Service::GetInputInstance()->GetMouseX(), (float)STX_Service::GetInputInstance()->GetMouseY());
	D3DXFROMWINEVECTOR2 vResolution(1.0f,1.0f);//(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
	static float start=timeGetTime();
	float time=.00025 * (timeGetTime() - start );
#if 0
	shape3D[m_i].BeginDraw(&matRot, -1, shader[s_i], vf[s_i], a, d, l, e);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", matRot);
#if 1
	IRenderer::GetRendererInstance()->setShaderConstant2f("iMouse", vMouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", vResolution);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iChannelResolution", vResolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTime", time);
	IRenderer::GetRendererInstance()->setTexture("iChannel0", tex[0]);
	IRenderer::GetRendererInstance()->setTexture("iChannel1", tex[1]);
	IRenderer::GetRendererInstance()->setTexture("iChannel2", tex[2]);
#else
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
int iI=0;
sampler2D tExplosion;
#endif
	shape3D[m_i].EndDraw();
#else
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	matRot=I;
	IRenderer::GetRendererInstance()->setShader(shader[s_i]);
	IRenderer::GetRendererInstance()->setVertexFormat(vf[s_i]);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", matRot);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iMouse", vMouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", vResolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTime", time);
	IRenderer::GetRendererInstance()->setShaderConstant2f("mouse", vMouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("resolution", vResolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iChannelResolution", vResolution);
	IRenderer::GetRendererInstance()->setTexture("iChannel0", tex[0]);
	IRenderer::GetRendererInstance()->setTexture("iChannel1", tex[1]);
	IRenderer::GetRendererInstance()->setTexture("iChannel2", tex[2]);

	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	#if 1
	//		           x	 y     tx    ty
	float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			          1.0f, 1.0f, 1.0f, 0.0f,
			         -1.0f,-1.0f, 0.0f, 1.0f,
			         -1.0f, 1.0f, 0.0f, 0.0f };
	unsigned int N=4;
	#else
	float v[16] =  {  1.0f,-1.0f,
			           1.0f, 1.0f,
			          -1.0f,-1.0f,
			          -1.0f, 1.0f };
	unsigned int N=2;
	#endif
#if 1
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], N*sizeof(float));
#else
	__WORD__ i[6] ={0,1,2,2,1,3};
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 4, 2, &i[0], &i[0], CONSTANT_INDEX2, &v[0], &v[0], N*sizeof(float));
#endif
#endif
}

	IRenderer::GetRendererInstance()->drawText("F1-key: previous shader", 10, 10, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

	IRenderer::GetRendererInstance()->drawText("F2-key: next shader", 10, 30, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());

		char txt2[512];
		stx_snprintf(txt2, 512, "s_i=%d: %s\n", s_i, filename[s_i]);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
			

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
};
int ApplicationLogic()
{	
	IRenderer* r=IRenderer::GetRendererInstance("TestNoise");	
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

