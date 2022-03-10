/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#if 0
const char* filename[] =
{
"/shd/glslsandbox.com/e#79415.0.glsl",
"/shd/glslsandbox.com/e#79537.0.glsl",
"/shd/www.shadertoy.com/Seascape.glsl",
"/shd/glslsandbox.com/e#79620.0.glsl",
"/shd/glslsandbox.com/e#79629.0.glsl",
"/shd/www.shadertoy.com/Voxel_Hall_Colors.hlsl",
"/shd/www.shadertoy.com/Voxel_Corridor.hlsl",
"/shd/www.shadertoy.com/The_Cave.hlsl",
"/shd/glslsandbox.com/e#79394.0.hlsl",
"/shd/www.shadertoy.com/simple_flare.hlsl",
"/shd/www.shadertoy.com/Hell.hlsl",
"/shd/www.shadertoy.com/Hell_2.hlsl",
"/shd/www.shadertoy.com/Badlands.hlsl",
"/shd/www.shadertoy.com/Glacial_valleys.hlsl",
"/shd/www.shadertoy.com/Eroded_mountain_terrain_(v2).hlsl",
"/shd/www.shadertoy.com/simple_terrain.hlsl",
"/shd/glslsandbox.com/e#75266.0.hlsl",
"/shd/glslsandbox.com/e#75269.0.hlsl",
"/shd/www.shadertoy.com/Super_simple_raymarching_example.hlsl",
"/shd/www.shadertoy.com/Ray_Marching_Part_1.hlsl",
"/shd/www.shadertoy.com/Ray_Marching_Part_2.hlsl",
#if 0
"/shd/www.shadertoy.com/Ray_Marching_Part_3.hlsl",
"/shd/www.shadertoy.com/Ray_Marching_Part_4.hlsl",
"/shd/www.shadertoy.com/Ray_Marching_Part_5.hlsl",
"/shd/www.shadertoy.com/Ray_Marching_Part_6.hlsl",
#endif
"/shd/www.shadertoy.com/simple_Starfield.hlsl",
"/shd/www.shadertoy.com/Skin_peeler.hlsl",
"/shd/www.shadertoy.com/SimpleStars.hlsl",
"/shd/www.shadertoy.com/7dyGWV.hlsl",
"/shd/www.shadertoy.com/Mandelbrot_smooth.hlsl",
"/shd/www.shadertoy.com/Logistic_Map_Real_Bifurcation.hlsl",
"/shd/www.shadertoy.com/TileableNoise.hlsl",
"/shd/glslsandbox.com/e#59382.0.hlsl",
"/shd/www.shadertoy.com/301FireShaderRemix1.hlsl",
"/shd/www.shadertoy.com/301FireShaderRemix2.hlsl",
"/shd/www.shadertoy.com/301FireShaderRemix3.hlsl",
"/shd/www.shadertoy.com/AdaptiveSamplingDiagram.hlsl",
"/shd/www.shadertoy.com/BallOfFire.hlsl",
"/shd/www.shadertoy.com/CheapCloudFlythrough.hlsl",
"/shd/www.shadertoy.com/CheapCurvature.hlsl",
"/shd/www.shadertoy.com/explosionno12.hlsl",
"/shd/www.shadertoy.com/FakeVolumetricClouds.hlsl",
"/shd/www.shadertoy.com/flame.hlsl",
"/shd/www.shadertoy.com/Flameinthewind.hlsl",
"/shd/www.shadertoy.com/fires.hlsl",
"/shd/www.shadertoy.com/FireShader.hlsl",
"/shd/www.shadertoy.com/RaymarchingSampleCode.hlsl",
"/shd/www.shadertoy.com/pyroclasticexplosion.hlsl",
"/shd/www.shadertoy.com/PyroclasticFireball.hlsl",
"/shd/www.shadertoy.com/Simplestarfield.hlsl",
"/shd/www.shadertoy.com/Simplicity.hlsl",
"/shd/www.shadertoy.com/Simplicity3D.hlsl",
"/shd/www.shadertoy.com/Xyptonjtroz.hlsl",
"/shd/www.shadertoy.com/campfire.hlsl",
"/shd/www.shadertoy.com/CloudTen.hlsl",
"/shd/www.shadertoy.com/CloudySpikeBall.hlsl",
"/shd/www.shadertoy.com/DustyNebula3.hlsl",
"/shd/www.shadertoy.com/DustyNebula4.hlsl",
"/shd/www.shadertoy.com/ProtoplanetaryDisk.hlsl",
"/shd/www.shadertoy.com/SamplePinning.hlsl",
"/shd/www.shadertoy.com/supernova.hlsl",
"/shd/www.shadertoy.com/SupernovaRemnant.hlsl",
"/shd/www.shadertoy.com/type2supernova.hlsl",
"/shd/www.shadertoy.com/volumetricexplosion.hlsl",
"/shd/www.shadertoy.com/Watchitburn.hlsl",
"/shd/glslsandbox.com/e#72423.0.hlsl",
"/shd/glslsandbox.com/e#68825.0.hlsl",
"/shd/glslsandbox.com/e#72317.0.glsl",
"/shd/glslsandbox.com/e#72645.0.hlsl",
"/shd/glslsandbox.com/e#67654.0.hlsl",
"/shd/glslsandbox.com/e#68826.0.hlsl",
"/shd/glslsandbox.com/e#72421.0.hlsl",
"/shd/glslsandbox.com/e#73091.1.hlsl",
"/shd/glslsandbox.com/e#68576.0.hlsl",
"/shd/glslsandbox.com/e#68827.0.hlsl",
"/shd/glslsandbox.com/e#73092.0.glsl",
"/shd/glslsandbox.com/e#68577.0.hlsl",
"/shd/glslsandbox.com/e#68829.0.hlsl",
"/shd/glslsandbox.com/e#72444.0.hlsl",
"/shd/www.shadertoy.com/XslGRr.hlsl",
"/shd/www.shadertoy.com/Soccermania.hlsl",
"/shd/www.shadertoy.com/TDM_Seascape_Sailing.glsl",
"/shd/www.shadertoy.com/Fantasy_World_Map.hlsl",
"/shd/www.shadertoy.com/Ocean_waves_ride.hlsl",
"/shd/www.shadertoy.com/Artistic_Black_hole.hlsl",
"/shd/www.shadertoy.com/Main_Sequence_Star.hlsl",
"/shd/www.shadertoy.com/Planet_shader.hlsl",
"/shd/www.shadertoy.com/Mario_World.hlsl",
"/shd/www.shadertoy.com/Cheap_Ocean.hlsl",
"/shd/www.shadertoy.com/buildings_by_night.hlsl",
"/shd/www.shadertoy.com/Re-entry.glsl",
#if 0
"/shd/www.shadertoy.com/Islands_in_wavy_shallow_water.hlsl",
"/shd/www.shadertoy.com/Fork_Islands_in_Arrangemon_275.hlsl",
"/shd/www.shadertoy.com/The_One_Ring.hlsl",
"/shd/www.shadertoy.com/Desert_biome.hlsl",
"/shd/www.shadertoy.com/Sandstone_canyons.hlsl",
"/shd/www.shadertoy.com/Logistic_Map_Complex.hlsl",
"/shd/www.shadertoy.com/Menger_Sponge.hlsl",
"/shd/www.shadertoy.com/Hashscape.hlsl",
"/shd/www.shadertoy.com/Mountains.hlsl",
"/shd/www.shadertoy.com/Tree_in_the_wind.hlsl",
"/shd/www.shadertoy.com/explosionno13.hlsl",
"/shd/www.shadertoy.com/Flamethrower.hlsl",
"/shd/www.shadertoy.com/SpaceElevator.hlsl",
"/shd/www.shadertoy.com/explosioneffect.hlsl",
"/shd/www.shadertoy.com/gasexplosion.hlsl",
"/shd/www.shadertoy.com/StructuredVolSampling.hlsl",
"/shd/www.shadertoy.com/Auroras.hlsl",
"/shd/www.shadertoy.com/Himalayas.hlsl",
"/shd/www.shadertoy.com/Desert_mountains.hlsl",
"/shd/www.shadertoy.com/Desert_mountains_v2.hlsl",
"/shd/www.shadertoy.com/Volcanic_planet.hlsl",
"/shd/www.shadertoy.com/Ray_Marching_Tutorial_01.hlsl",
"/shd/www.shadertoy.com/Lindenmayer_Systems.hlsl",
"/shd/www.shadertoy.com/sierpinski_triangle_(default).hlsl",
"/shd/www.shadertoy.com/Commodore_64.hlsl",
#if 0
"/shd/www.shadertoy.com/Road01.hlsl",
"/shd/www.shadertoy.com/Road02.hlsl",
"/shd/glslsandbox.com/e#61428.0.hlsl",
"/shd/glslsandbox.com/e#63847.0.hlsl",
#endif
"/shd/www.shadertoy.com/ED-209.hlsl",
"/shd/www.shadertoy.com/Truchet_Kaleidoscope_FTW.hlsl",
"/shd/www.shadertoy.com/5_0_fbm.hlsl",
"/shd/www.shadertoy.com/Clock_by_Vicking.hlsl",
"/shd/www.shadertoy.com/Voxel_Edges.hlsl",
"/shd/www.shadertoy.com/Swirl_planet.hlsl",
"/shd/www.shadertoy.com/Simplicity_Galaxy.hlsl",
"/shd/www.shadertoy.com/Patched_Fibonacci_sphere_mapping,shd",
"/shd/www.shadertoy.com/OCEAN_SURFACE_NOISE_FBM.hlsl",
"/shd/www.shadertoy.com/Voxel_Hall_Colors_3D.hlsl",
"/shd/www.shadertoy.com/Voxel_Corridor_3D.hlsl",
"/shd/glslsandbox.com/e#79313.0.hlsl",
"/shd/www.shadertoy.com/2D_Clouds.hlsl",
"/shd/www.shadertoy.com/Clouds.hlsl",
"/shd/www.shadertoy.com/OceanView.hlsl",
"/shd/www.shadertoy.com/Oceanic.hlsl",
"/shd/glslsandbox.com/e#75266.1.hlsl",
//"/SDF.hlsl",
"/shd/www.shadertoy.com/Chaos_Game_Sierpinsky_Triangle.hlsl",
"/shd/www.shadertoy.com/video_heightfield2.hlsl",
"/shd/www.shadertoy.com/InfiniteTerrainII.hlsl",
"/shd/www.shadertoy.com/AlienBeacon.hlsl",
"/shd/www.shadertoy.com/Common.hlsl",
"/shd/www.shadertoy.com/dustynebula1.hlsl",
"/shd/www.shadertoy.com/StructuredSamplingDiagram.hlsl",
"/shd/www.shadertoy.com/anatomyofanexplosion.hlsl",
"/shd/www.shadertoy.com/EmissionClouds.hlsl",
"/shd/www.shadertoy.com/Protoplanet.hlsl",
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
		std::string f=stx_convertpath("/shd/BallOfFire.txt");
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
	//stx_InitShape3D("/shd/www.shadertoy.com/Shaders.xml", ShaderFiles, shd, vf, TextureFiles, tex);
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
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/shd/www.shadertoy.com/world1.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/shd/www.shadertoy.com/cloud1.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
	tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture("/shd/www.shadertoy.com/light1.jpg", false, IRenderer::GetRendererInstance()->Getlinear()));
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

