/*
  Copyright (c) 2022 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char* filename[] =
{
"/glslsandbox.com/shd/e#75737.0.glsl",
"/glslsandbox.com/shd/e#79313.0.glsl",
"/www.shadertoy.com/shd/2D_Clouds.shd",
//"/www.shadertoy.com/shd/2D_noise_on_a_Fibonacci_sphere.shd",
//"/www.shadertoy.com/shd/5_0_fbm.shd",
"/www.shadertoy.com/shd/AlienBeacon.shd",
"/www.shadertoy.com/shd/anatomyofanexplosion.shd",
"/www.shadertoy.com/shd/Chaos_Game_Sierpinsky_Triangle.shd",
//"/www.shadertoy.com/shd/Clock_by_Vicking.shd",
//"/www.shadertoy.com/shd/Clouds.shd",
"/www.shadertoy.com/shd/Commodore_64.shd",
"/www.shadertoy.com/shd/Desert_biome.shd",
"/www.shadertoy.com/shd/Desert_mountains.shd",
"/www.shadertoy.com/shd/Desert_mountains_v2.shd",
"/www.shadertoy.com/shd/dustynebula1.shd",
#if 0
"/www.shadertoy.com/shd/ED-209.shd",
"/www.shadertoy.com/shd/EmissionClouds.shd",
"/www.shadertoy.com/shd/Even_faster_procedural_ocean.shd",
"/www.shadertoy.com/shd/explosioneffect.shd",
"/www.shadertoy.com/shd/explosionno13.shd",
"/www.shadertoy.com/shd/Flamethrower.shd",
"/www.shadertoy.com/shd/Fork_Islands_in_Arrangemon_275.shd",
"/www.shadertoy.com/shd/gasexplosion.shd",
"/www.shadertoy.com/shd/Hashscape.shd",
"/www.shadertoy.com/shd/HashwithoutSine.shd",
"/www.shadertoy.com/shd/Himalayas.shd",
"/www.shadertoy.com/shd/Islands_in_wavy_shallow_water.shd",
"/www.shadertoy.com/shd/Lindenmayer_Systems.shd",
"/www.shadertoy.com/shd/Load_Custom_Textures.shd",
"/www.shadertoy.com/shd/Logistic_Map_Complex.shd",
"/www.shadertoy.com/shd/Menger_Sponge.shd",
"/www.shadertoy.com/shd/Mountains.shd",
"/www.shadertoy.com/shd/Oceanic.shd",
"/www.shadertoy.com/shd/OCEAN_SURFACE_NOISE_FBM.shd",
"/www.shadertoy.com/shd/OceanView.shd",
"/www.shadertoy.com/shd/Patched_Fibonacci_sphere_mapping.shd",
"/www.shadertoy.com/shd/Protoplanet.shd",
"/www.shadertoy.com/shd/Ray_Marching_Part_3.shd",
"/www.shadertoy.com/shd/Ray_Marching_Part_4.shd",
"/www.shadertoy.com/shd/Ray_Marching_Part_5.shd",
"/www.shadertoy.com/shd/Ray_Marching_Part_6.shd",
"/www.shadertoy.com/shd/Raymarching_Primitives.shd",
"/www.shadertoy.com/shd/Ray_Marching_Tutorial_01.shd",
"/www.shadertoy.com/shd/Red_Sun_(flow_noise_demo).shd",
"/www.shadertoy.com/shd/Sandstone_canyons.shd",
"/www.shadertoy.com/shd/Seacape_original.shd",
"/www.shadertoy.com/shd/Seascape_Clubber_VJ_version.shd",
"/www.shadertoy.com/shd/Seascape_Documented.shd",
"/www.shadertoy.com/shd/Seascape.shd",
"/www.shadertoy.com/shd/sierpinski_triangle_(default).shd",
"/www.shadertoy.com/shd/Simplicity_Galaxy.shd",
"/www.shadertoy.com/shd/SpaceElevator.shd",
"/www.shadertoy.com/shd/Stormy_night.shd",
#endif
"/www.shadertoy.com/shd/StructuredSamplingDiagram.shd",
#if 0
"/www.shadertoy.com/shd/StructuredVolSampling.shd",
"/www.shadertoy.com/shd/Swirl_planet.shd",
"/www.shadertoy.com/shd/TDM_Seascape_Sailing.shd",
"/www.shadertoy.com/shd/The_One_Ring.shd",
"/www.shadertoy.com/shd/Tree_in_the_wind.shd",
"/www.shadertoy.com/shd/Truchet_Kaleidoscope_FTW.shd",
"/www.shadertoy.com/shd/Urban_Smog.shd",
"/www.shadertoy.com/shd/Very_fast_procedural_ocean.shd",
"/www.shadertoy.com/shd/video_heightfield2.shd",
"/www.shadertoy.com/shd/Volcanic_planet.shd",
"/www.shadertoy.com/shd/Voxel_Corridor2.shd",
"/www.shadertoy.com/shd/Voxel_Corridor_3D.shd",
#endif
"/www.shadertoy.com/shd/Voxel_Edges.shd",
//"/www.shadertoy.com/shd/Voxel_Hall_Colors_3D.shd"
};

MeshRenderer2 shape3D[3];
//std::vector<std::string> ShaderFiles;
std::vector<ShaderID> shd;
std::vector<VertexFormatID> vf;
//std::vector<std::string> TextureFiles;
std::vector<TextureID> tex;
unsigned int s_i=0;
unsigned int t_i=0;
unsigned int m_i=2;

int init(const char* aTitle)
{
	LOG_FNLN;
	#if 0
	//stx_InitShape3D("/www.shadertoy.com/Shaders.xml", ShaderFiles, shd, vf, TextureFiles, tex);
	shape3D[0].CreateTexturedQuad(2.0f, 2.0f, eShaderNone);
        shape3D[1].CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
        shape3D[2].CreateSphere(1.0f, eShaderNone);
	#endif
	LOG_FNLN;
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
		shd.push_back(id);
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
		if(shd[shd.size()-1]!=-1)
		vf.push_back(IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd[shd.size()-1]));
		else
		vf.push_back(-1);
	}
	LOG_FNLN;
	#if 0
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
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/lSystem/brick.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/1024px-brick.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/bcga-examples/MarekBrick002.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/bcga-examples/MarekBrick004.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	//tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/bcga-examples/MarekSeamlessBrick003.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/RadeonTerrainDemo/HardRock.bmp", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/RadeonTerrainDemo/HardRock2.bmp", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/RadeonTerrainDemo/HardRock3.bmp", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Sky/clouds.png", false, IRenderer::GetRendererInstance()->Getlinear()));
	#endif
	LOG_FNLN;
	LOG_PRINT("shd.size()=%d\n", shd.size());
	LOG_PRINT("vf.size()=%d\n", vf.size());
	LOG_FNLN;
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
	{
		if(!s_i)
			s_i=shd.size()-1;
		else
			s_i--;
		while((shd[s_i]==-1)||(vf[s_i]==-1))
		{
			if(!s_i)
				s_i=shd.size()-1;
			else
				s_i--;
		}
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		s_i++;
		if(s_i>shd.size()-1)
			s_i=0;
		while((shd[s_i]==-1)||(vf[s_i]==-1))
		{
			s_i++;
			if(s_i>shd.size()-1)
				s_i=0;
		}
	}

	float mAngleX=0.0f;
	float mAngleY=0.0f;
//if((shd[s_i]>-1)&&(vf[s_i]>-1))
{
	stx_GetAngles(mAngleX, mAngleY);

		D3DXFROMWINEMATRIX W,R,S;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );


		D3DXFROMWINEVECTOR4 silver=stx_GetRGBA(eSilver);
		D3DXFROMWINEVECTOR4 AmbientColor=silver;
		D3DXFROMWINEVECTOR4 DiffuseColor=0.1f*silver;
		D3DXFROMWINEVECTOR4 LightDir(0.0f, 0.0f, -1.0f, 1.0f);
		D3DXFROMWINEVECTOR4 EyeDir(0.0f, 0.0f, 1.0f, 1.0f);

	//D3DXFROMWINEMatrixIdentity(&R);
	float s=0.25f*4.0f;
	D3DXFROMWINEMatrixScaling(&S, s, s, s);
	W=R*S;
	#if 1
	LOG_FNLN;
	LOG_PRINT("s_i=%d\n", s_i);
	LOG_PRINT("shd.size()=%d\n", shd.size());
	LOG_PRINT("vf.size()=%d\n", vf.size());
	LOG_PRINT("shd[%d]=%d\n", s_i, shd[s_i]);
	LOG_PRINT("vf[%d]=%d\n", s_i, vf[s_i]);
	#endif

	//printf("ViewportWidth=%d\n", IRenderer::GetRendererInstance()->GetViewportWidth());
	//printf("ViewportHeight=%d\n", IRenderer::GetRendererInstance()->GetViewportHeight());
	D3DXFROMWINEVECTOR2 mouse(1.0f,1.0f);//((float)STX_Service::GetInputInstance()->GetMouseX(), (float)STX_Service::GetInputInstance()->GetMouseY());
	D3DXFROMWINEVECTOR2 resolution(1.0f,1.0f);//(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
	static float start=timeGetTime();
	float time=.00025 * (timeGetTime() - start );
	D3DXFROMWINEVECTOR4 iDate(0.0f, 0.0f, 0.0f, time);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
#if 0
        shape3D[m_i].BeginDraw(&W, -1, shd[s_i], vf[s_i], AmbientColor, DiffuseColor, LightDir, EyeDir);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", W);
	//IRenderer::GetRendererInstance()->setShader(shd[s_i]);
	//IRenderer::GetRendererInstance()->setVertexFormat(vf[s_i]);
	IRenderer::GetRendererInstance()->setTexture("iChannel0", tex[0]);
	IRenderer::GetRendererInstance()->setTexture("iChannel1", tex[1]);
	IRenderer::GetRendererInstance()->setTexture("iChannel2", tex[2]);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", resolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTime", time);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iMouse", mouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iChannelResolution", resolution);
	#if 0
	IRenderer::GetRendererInstance()->setShaderConstant1f("iGlobalTime", );                  // shader playback time (in seconds)
	IRenderer::GetRendererInstance()->setShaderConstant4f("iMouse", );                       // mouse pixel coords
	IRenderer::GetRendererInstance()->setShaderConstant4f("iDate", );                        // (year, month, day, time in seconds)
	IRenderer::GetRendererInstance()->setShaderConstant1f("iSampleRate", );                  // sound sample rate (i.e., 44100)
	IRenderer::GetRendererInstance()->setShaderConstant2f("ifFragCoordOffsetUniform", );     // used for tiled based hq rendering
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTimeDelta", );                   // render time (in seconds)
	IRenderer::GetRendererInstance()->setShaderConstant1i("iFrame", );                       // shader playback frame
	IRenderer::GetRendererInstance()->setShaderConstant1f("iFrameRate", );
	#endif
	shape3D[m_i].EndDraw();
#else
	IRenderer::GetRendererInstance()->setShader(shd[s_i]);
	IRenderer::GetRendererInstance()->setVertexFormat(vf[s_i]);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", I);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iMouse", mouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", resolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTime", time);
	IRenderer::GetRendererInstance()->setShaderConstant2f("mouse", mouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("resolution", resolution);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	IRenderer::GetRendererInstance()->setShaderConstant4f("iDate", iDate);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iChannelResolution", resolution);
	IRenderer::GetRendererInstance()->setTexture("iChannel0", tex[0]);
	IRenderer::GetRendererInstance()->setTexture("iChannel1", tex[1]);
	IRenderer::GetRendererInstance()->setTexture("iChannel2", tex[2]);
	IRenderer::GetRendererInstance()->setTexture("iChannel3", tex[3]);
	IRenderer::GetRendererInstance()->setTexture("iChannel4", tex[4]);
	IRenderer::GetRendererInstance()->setTexture("iChannel5", tex[5]);
	IRenderer::GetRendererInstance()->setTexture("iChannel6", tex[6]);
	IRenderer::GetRendererInstance()->setTexture("iChannel7", tex[7]);
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

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("TestNoise");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

