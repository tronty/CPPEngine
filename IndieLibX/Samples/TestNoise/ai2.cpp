/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

std::vector<std::string> g_filenames;
//MeshRenderer2 shape3D[3];
//std::vector<std::string> ShaderFiles;
ShaderID shd=-1;
VertexFormatID vf=-1;
//std::vector<std::string> TextureFiles;
std::vector<TextureID> tex;
unsigned int s_i=0;
unsigned int t_i=0;
unsigned int m_i=2;

D3DXFROMWINEVECTOR3 lightDir = D3DXFROMWINEVECTOR3(1.0, 1.0, 1.0);
D3DXFROMWINEVECTOR3 cubeCenter = D3DXFROMWINEVECTOR3(200.0, 150.0, 0.0);

void LoadShader(int i =-1)
{
	if(i<0)
		return;
	if(i>=g_filenames.size())
		return;
	shd=-1;
	vf=-1;

		ShaderID id=IRenderer::GetRendererInstance()->addShaderFromFile(g_filenames[i].c_str(), "main2", "main");
		if(id==-1)
		{
			printf("Shader=%s failed!\n", g_filenames[i].c_str());
			//stx_exit(0);
			//continue;
		}
		shd=id;
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
		if(shd>-1)
			vf=IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
}

int init(const char* aTitle)
{
	{
		std::string f=stx_convertpath("/shd/ai2.txt");
		std::ifstream file(f.c_str());
		std::string str; 
		while (std::getline(file, str))
		{
			if(str.length())
				if(str[0]!='#')
        				g_filenames.push_back(str);
    		}
		LoadShader(0);
	}
	LOG_FNLN;
	#if 0
	//stx_InitShape3D("/shd/www.shadertoy.com/Shaders.xml", ShaderFiles, shd, vf, TextureFiles, tex);
	shape3D[0].CreateTexturedQuad(2.0f, 2.0f, eShaderNone);
        shape3D[1].CreateTexturedBox(1.0f, 1.0f, 1.0f, eShaderNone);
        shape3D[2].CreateSphere(1.0f, eShaderNone);
	#endif
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
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();
	
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1))
	{
		IRenderer::GetRendererInstance()->DeleteShader(s_i);
		if(!s_i)
			s_i=g_filenames.size()-1;
		else
			s_i--;
		LoadShader(s_i);
		while((shd==-1)||(vf==-1))
		{
			IRenderer::GetRendererInstance()->DeleteShader(s_i);
			if(!s_i)
				s_i=g_filenames.size()-1;
			else
				s_i--;
			LoadShader(s_i);
		}
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		IRenderer::GetRendererInstance()->DeleteShader(s_i);
		s_i++;
		if(s_i>g_filenames.size()-1)
			s_i=0;		
		LoadShader(s_i);
		while((shd==-1)||(vf==-1))
		{
			IRenderer::GetRendererInstance()->DeleteShader(s_i);
			s_i++;
			if(s_i>g_filenames.size()-1)
				s_i=0;	
			LoadShader(s_i);
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

	//printf("ViewportWidth=%d\n", IRenderer::GetRendererInstance()->GetViewportWidth());
	//printf("ViewportHeight=%d\n", IRenderer::GetRendererInstance()->GetViewportHeight());
	D3DXFROMWINEVECTOR2 mouse((float)STX_Service::GetInputInstance()->GetMouseX(), (float)STX_Service::GetInputInstance()->GetMouseY());
	D3DXFROMWINEVECTOR2 resolution(400.0f,300.0f);//(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
	static float start=timeGetTime();
	float time=.00025 * (timeGetTime() - start );
	D3DXFROMWINEVECTOR4 iDate(0.0f, 0.0f, 0.0f, time);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	if(shd==-1) return;
	if(vf==-1) return;
#if 0
        shape3D[m_i].BeginDraw(&W, -1, shd[s_i], vf[s_i], AmbientColor, DiffuseColor, LightDir, EyeDir);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", W);
	//IRenderer::GetRendererInstance()->setShader(shd);
	//IRenderer::GetRendererInstance()->setVertexFormat(vf);
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
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
	IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
	
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("modelViewProjection", I);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iMouse", mouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("iResolution", resolution);
	IRenderer::GetRendererInstance()->setShaderConstant2f("mouse", mouse);
	IRenderer::GetRendererInstance()->setShaderConstant2f("resolution", resolution);
	IRenderer::GetRendererInstance()->setShaderConstant4f("iDate", iDate);

	//time=timeGetTime()/1000.0f;
	IRenderer::GetRendererInstance()->setShaderConstant1f("iTime", time);
	IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	IRenderer::GetRendererInstance()->setShaderConstant1f("iGlobalTime", time);

	IRenderer::GetRendererInstance()->setShaderConstant3f("lightDir", lightDir);
	IRenderer::GetRendererInstance()->setShaderConstant3f("cubeCenter", cubeCenter);

	IRenderer::GetRendererInstance()->setShaderConstant2f("iChannelResolution", resolution);
	if(tex.size()>0) IRenderer::GetRendererInstance()->setTexture("iChannel0", tex[0]);
	if(tex.size()>1) IRenderer::GetRendererInstance()->setTexture("iChannel1", tex[1]);
	if(tex.size()>2) IRenderer::GetRendererInstance()->setTexture("iChannel2", tex[2]);
	if(tex.size()>3) IRenderer::GetRendererInstance()->setTexture("iChannel3", tex[3]);
	if(tex.size()>4) IRenderer::GetRendererInstance()->setTexture("iChannel4", tex[4]);
	if(tex.size()>5) IRenderer::GetRendererInstance()->setTexture("iChannel5", tex[5]);
	if(tex.size()>6) IRenderer::GetRendererInstance()->setTexture("iChannel6", tex[6]);
	if(tex.size()>7) IRenderer::GetRendererInstance()->setTexture("iChannel7", tex[7]);
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
	IRenderer* r=IRenderer::GetRendererInstance("ai2");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

