/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#if 0
const char* filename[] =
{
"/glslsandbox.com/e#79415.0.glsl",
"/glslsandbox.com/e#79537.0.glsl",
"/www.shadertoy.com/Seascape.glsl",
"/glslsandbox.com/e#79620.0.glsl",
"/glslsandbox.com/e#79629.0.glsl",
"/www.shadertoy.com/Voxel_Hall_Colors.hlsl",
"/www.shadertoy.com/Voxel_Corridor.hlsl",
"/www.shadertoy.com/The_Cave.hlsl",
"/glslsandbox.com/e#79394.0.hlsl",
"/www.shadertoy.com/simple_flare.hlsl",
"/www.shadertoy.com/Hell.hlsl",
"/www.shadertoy.com/Hell_2.hlsl",
"/www.shadertoy.com/Badlands.hlsl",
"/www.shadertoy.com/Glacial_valleys.hlsl",
"/www.shadertoy.com/Eroded_mountain_terrain_(v2).hlsl",
"/www.shadertoy.com/simple_terrain.hlsl",
"/glslsandbox.com/e#75266.0.hlsl",
"/glslsandbox.com/e#75269.0.hlsl",
"/www.shadertoy.com/Super_simple_raymarching_example.hlsl",
"/www.shadertoy.com/Ray_Marching_Part_1.hlsl",
"/www.shadertoy.com/Ray_Marching_Part_2.hlsl",
#if 0
"/www.shadertoy.com/Ray_Marching_Part_3.hlsl",
"/www.shadertoy.com/Ray_Marching_Part_4.hlsl",
"/www.shadertoy.com/Ray_Marching_Part_5.hlsl",
"/www.shadertoy.com/Ray_Marching_Part_6.hlsl",
#endif
"/www.shadertoy.com/simple_Starfield.hlsl",
"/www.shadertoy.com/Skin_peeler.hlsl",
"/www.shadertoy.com/SimpleStars.hlsl",
"/www.shadertoy.com/7dyGWV.hlsl",
"/www.shadertoy.com/Mandelbrot_smooth.hlsl",
"/www.shadertoy.com/Logistic_Map_Real_Bifurcation.hlsl",
"/www.shadertoy.com/TileableNoise.hlsl",
"/glslsandbox.com/e#59382.0.hlsl",
"/www.shadertoy.com/301FireShaderRemix1.hlsl",
"/www.shadertoy.com/301FireShaderRemix2.hlsl",
"/www.shadertoy.com/301FireShaderRemix3.hlsl",
"/www.shadertoy.com/AdaptiveSamplingDiagram.hlsl",
"/www.shadertoy.com/BallOfFire.hlsl",
"/www.shadertoy.com/CheapCloudFlythrough.hlsl",
"/www.shadertoy.com/CheapCurvature.hlsl",
"/www.shadertoy.com/explosionno12.hlsl",
"/www.shadertoy.com/FakeVolumetricClouds.hlsl",
"/www.shadertoy.com/flame.hlsl",
"/www.shadertoy.com/Flameinthewind.hlsl",
"/www.shadertoy.com/fires.hlsl",
"/www.shadertoy.com/FireShader.hlsl",
"/www.shadertoy.com/RaymarchingSampleCode.hlsl",
"/www.shadertoy.com/pyroclasticexplosion.hlsl",
"/www.shadertoy.com/PyroclasticFireball.hlsl",
"/www.shadertoy.com/Simplestarfield.hlsl",
"/www.shadertoy.com/Simplicity.hlsl",
"/www.shadertoy.com/Simplicity3D.hlsl",
"/www.shadertoy.com/Xyptonjtroz.hlsl",
"/www.shadertoy.com/campfire.hlsl",
"/www.shadertoy.com/CloudTen.hlsl",
"/www.shadertoy.com/CloudySpikeBall.hlsl",
"/www.shadertoy.com/DustyNebula3.hlsl",
"/www.shadertoy.com/DustyNebula4.hlsl",
"/www.shadertoy.com/ProtoplanetaryDisk.hlsl",
"/www.shadertoy.com/SamplePinning.hlsl",
"/www.shadertoy.com/supernova.hlsl",
"/www.shadertoy.com/SupernovaRemnant.hlsl",
"/www.shadertoy.com/type2supernova.hlsl",
"/www.shadertoy.com/volumetricexplosion.hlsl",
"/www.shadertoy.com/Watchitburn.hlsl",
"/glslsandbox.com/e#72423.0.hlsl",
"/glslsandbox.com/e#68825.0.hlsl",
"/glslsandbox.com/e#72317.0.glsl",
"/glslsandbox.com/e#72645.0.hlsl",
"/glslsandbox.com/e#67654.0.hlsl",
"/glslsandbox.com/e#68826.0.hlsl",
"/glslsandbox.com/e#72421.0.hlsl",
"/glslsandbox.com/e#73091.1.hlsl",
"/glslsandbox.com/e#68576.0.hlsl",
"/glslsandbox.com/e#68827.0.hlsl",
"/glslsandbox.com/e#73092.0.glsl",
"/glslsandbox.com/e#68577.0.hlsl",
"/glslsandbox.com/e#68829.0.hlsl",
"/glslsandbox.com/e#72444.0.hlsl",
"/www.shadertoy.com/XslGRr.hlsl",
"/www.shadertoy.com/Soccermania.hlsl",
"/www.shadertoy.com/TDM_Seascape_Sailing.glsl",
"/www.shadertoy.com/Fantasy_World_Map.hlsl",
"/www.shadertoy.com/Ocean_waves_ride.hlsl",
"/www.shadertoy.com/Artistic_Black_hole.hlsl",
"/www.shadertoy.com/Main_Sequence_Star.hlsl",
"/www.shadertoy.com/Planet_shader.hlsl",
"/www.shadertoy.com/Mario_World.hlsl",
"/www.shadertoy.com/Cheap_Ocean.hlsl",
"/www.shadertoy.com/buildings_by_night.hlsl",
"/www.shadertoy.com/Re-entry.glsl",
#if 0
"/www.shadertoy.com/Islands_in_wavy_shallow_water.hlsl",
"/www.shadertoy.com/Fork_Islands_in_Arrangemon_275.hlsl",
"/www.shadertoy.com/The_One_Ring.hlsl",
"/www.shadertoy.com/Desert_biome.hlsl",
"/www.shadertoy.com/Sandstone_canyons.hlsl",
"/www.shadertoy.com/Logistic_Map_Complex.hlsl",
"/www.shadertoy.com/Menger_Sponge.hlsl",
"/www.shadertoy.com/Hashscape.hlsl",
"/www.shadertoy.com/Mountains.hlsl",
"/www.shadertoy.com/Tree_in_the_wind.hlsl",
"/www.shadertoy.com/explosionno13.hlsl",
"/www.shadertoy.com/Flamethrower.hlsl",
"/www.shadertoy.com/SpaceElevator.hlsl",
"/www.shadertoy.com/explosioneffect.hlsl",
"/www.shadertoy.com/gasexplosion.hlsl",
"/www.shadertoy.com/StructuredVolSampling.hlsl",
"/www.shadertoy.com/Auroras.hlsl",
"/www.shadertoy.com/Himalayas.hlsl",
"/www.shadertoy.com/Desert_mountains.hlsl",
"/www.shadertoy.com/Desert_mountains_v2.hlsl",
"/www.shadertoy.com/Volcanic_planet.hlsl",
"/www.shadertoy.com/Ray_Marching_Tutorial_01.hlsl",
"/www.shadertoy.com/Lindenmayer_Systems.hlsl",
"/www.shadertoy.com/sierpinski_triangle_(default).hlsl",
"/www.shadertoy.com/Commodore_64.hlsl",
#if 0
"/www.shadertoy.com/Road01.hlsl",
"/www.shadertoy.com/Road02.hlsl",
"/glslsandbox.com/e#61428.0.hlsl",
"/glslsandbox.com/e#63847.0.hlsl",
#endif
"/www.shadertoy.com/ED-209.hlsl",
"/www.shadertoy.com/Truchet_Kaleidoscope_FTW.hlsl",
"/www.shadertoy.com/5_0_fbm.hlsl",
"/www.shadertoy.com/Clock_by_Vicking.hlsl",
"/www.shadertoy.com/Voxel_Edges.hlsl",
"/www.shadertoy.com/Swirl_planet.hlsl",
"/www.shadertoy.com/Simplicity_Galaxy.hlsl",
"/www.shadertoy.com/Patched_Fibonacci_sphere_mapping,shd",
"/www.shadertoy.com/OCEAN_SURFACE_NOISE_FBM.hlsl",
"/www.shadertoy.com/Voxel_Hall_Colors_3D.hlsl",
"/www.shadertoy.com/Voxel_Corridor_3D.hlsl",
"/glslsandbox.com/e#79313.0.hlsl",
"/www.shadertoy.com/2D_Clouds.hlsl",
"/www.shadertoy.com/Clouds.hlsl",
"/www.shadertoy.com/OceanView.hlsl",
"/www.shadertoy.com/Oceanic.hlsl",
"/glslsandbox.com/e#75266.1.hlsl",
//"/SDF.hlsl",
"/www.shadertoy.com/Chaos_Game_Sierpinsky_Triangle.hlsl",
"/www.shadertoy.com/video_heightfield2.hlsl",
"/www.shadertoy.com/InfiniteTerrainII.hlsl",
"/www.shadertoy.com/AlienBeacon.hlsl",
"/www.shadertoy.com/Common.hlsl",
"/www.shadertoy.com/dustynebula1.hlsl",
"/www.shadertoy.com/StructuredSamplingDiagram.hlsl",
"/www.shadertoy.com/anatomyofanexplosion.hlsl",
"/www.shadertoy.com/EmissionClouds.hlsl",
"/www.shadertoy.com/Protoplanet.hlsl",
#endif
};
#endif

std::vector<std::string> g_filenames;
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
	{
		std::string f=stx_convertpath("/BallOfFire.txt");
		std::ifstream file(f.c_str());
		std::string str; 
		while (std::getline(file, str))
		{
			if(str.length())
				if(str[0]!='#')
        				g_filenames.push_back(str);
    		}
	}
	LOG_FNLN;
	#if 0
	//stx_InitShape3D("/www.shadertoy.com/Shaders.xml", ShaderFiles, shd, vf, TextureFiles, tex);
	shape3D[0].CreateTexturedQuad(2.0f, 2.0f, eShaderNone);
        shape3D[1].CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
        shape3D[2].CreateSphere(1.0f, eShaderNone);
	#endif
	LOG_FNLN;
	for(unsigned int i=0;i<g_filenames.size();i++)
	{
	#if 0
		printf("%s\n", g_filenames[i].c_str());
		continue;
	#endif
		ShaderID id=IRenderer::GetRendererInstance()->addShaderFromFile(g_filenames[i].c_str(), "main2", "main");
		if(id==-1)
		{
			printf("Shader=%s failed!\n", g_filenames[i]);
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
		stx_snprintf(txt2, 512, "s_i=%d/%d: %s\n", s_i+1, g_filenames.size(), g_filenames[s_i].c_str());
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
	IRenderer* r=IRenderer::GetRendererInstance("BallOfFire");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

