/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

class App: public STXGUI
{
	TextureID texture[7];
	unsigned int iI;
	ShaderID shd;
	VertexFormatID vf;
public:
App(){}
virtual ~App(){}
int init(const char* aTitle)
{
	iI=6;
	texture[0]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[1]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/fire_gradient3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[2]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireGrade.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[3]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/grad3.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[4]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FireTransferFunction.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[5]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/FirePalette.png",false, IRenderer::GetRendererInstance()->Getlinear());
	//texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/12_Explosion_Particle_Effect/explosion.png",false, IRenderer::GetRendererInstance()->Getlinear());
	texture[6]=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/NVIDIA_Corporation/2d/Spectrum.png",false, IRenderer::GetRendererInstance()->Getlinear());

	STXGUI::init("/MeshRenderer2/GUILayout4.xml");

	unsigned int i=0;
	shd=IRenderer::GetRendererInstance()->addShaderFromFile("/VertexDisplacement/hlsl2D.shd", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
		0, TYPE_NORMAL,   FORMAT_FLOAT, 2,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 
	};
	vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
	
	return 0;
}

void render( )
{
	if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F1) && (iI>0))
	{
		iI--;
	}
	else if(STX_Service::GetInputInstance()->OnKeyPress (STX_KEY_F2))
	{
		iI++;
		if(iI>7)
			iI=0;
	}
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setTexture("tExplosion", texture[iI]);
		// Javascript: = .00025 * ( Date.now() - start );
		static float start=timeGetTime();
		float time=.00025 * (timeGetTime() - start );
		IRenderer::GetRendererInstance()->setShaderConstant1f("time", time);
	D3DXFROMWINEMATRIX I;
	D3DXFROMWINEMatrixIdentity(&I);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	#if 1
	{
	const int ww=IRenderer::GetRendererInstance()->GetViewportWidth();
	const int wh=IRenderer::GetRendererInstance()->GetViewportHeight();
	const float x=0.5f*ww;
	const float y=0.5f*wh;
	const float r=0.5f*wh;
	const float lineWidth = 0;

	bool lines = (lineWidth > 0);
	float d = 0.5f * lineWidth;

	struct Vertex
	{
		D3DXFROMWINEVECTOR2 vertex;
		D3DXFROMWINEVECTOR2 normal;
		D3DXFROMWINEVECTOR2 uv;
	};
	const unsigned int N=16;
	Vertex vertices[N+1];

	for (int i = 0; i < N; i++){
		float ang = i * 2 * D3DXFROMWINE_PI / N;
		float cosA = cosf(ang);
		float sinA = sinf(ang);

		if (lines){
			vertices[2 * i    ].vertex = D3DXFROMWINEVECTOR2(x + (r + d) * cosA, y + (r + d) * sinA);
			D3DXFROMWINEVec2Normalize((D3DXFROMWINEVECTOR2*)&vertices[2 * i].normal,(D3DXFROMWINEVECTOR2*)&vertices[2 * i].vertex);
			vertices[2 * i].uv=D3DXFROMWINEVECTOR2(vertices[2 * i].vertex.x/ww, vertices[2 * i].vertex.y/wh);
			vertices[2 * i + 1].vertex = D3DXFROMWINEVECTOR2(x + (r - d) * cosA, y + (r - d) * sinA);
			D3DXFROMWINEVec2Normalize((D3DXFROMWINEVECTOR2*)&vertices[2 * i + 1].normal,(D3DXFROMWINEVECTOR2*)&vertices[2 * i + 1].vertex);
			vertices[2 * i + 1].uv=D3DXFROMWINEVECTOR2(vertices[2 * i + 1].vertex.x/ww, vertices[2 * i + 1].vertex.y/wh);
		} else {
			vertices[i].vertex = D3DXFROMWINEVECTOR2(x + r * cosA, y + r * sinA);
			D3DXFROMWINEVec2Normalize((D3DXFROMWINEVECTOR2*)&vertices[i].normal,(D3DXFROMWINEVECTOR2*)&vertices[i].vertex);
			vertices[i].uv=D3DXFROMWINEVECTOR2(vertices[i].vertex.x/ww, vertices[i].vertex.y/wh);
	
		}
	}

	if (lines){
		// Full loop
		vertices[2*N] = vertices[0];
		vertices[2*N+1] = vertices[1];
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2*N, vertices, vertices, sizeof(Vertex));
	} else {
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, N-2, vertices, vertices, sizeof(Vertex));
	}
	}
	#else
	//		   x	 y     tx    ty
	float v[24] =  {  1.0f,-1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			 -1.0f,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	for(unsigned int i=0;i<4;i++)
	{
		D3DXFROMWINEVECTOR2 v_(v[i*6+2], v[i*6+3]);
		D3DXFROMWINEVec2Normalize(&v_, &v_);
		v[i*6+2]=v_.x;
		v[i*6+3]=v_.y;
	}
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 6*sizeof(float));
	#endif
	
	STXGUI::update();
	
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}
virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
		if(callbackString == "Texture1")
      		{
			iI = 0;
		}
		if(callbackString == "Texture2")
      		{
			iI = 1;
		}
		if(callbackString == "Texture3")
      		{
			iI = 2;
		}
		if(callbackString == "Texture4")
      		{
			iI = 3;
		}
		if(callbackString == "Texture5")
      		{
			iI = 4;
		}
		if(callbackString == "Texture6")
      		{
			iI = 5;
		}
		if(callbackString == "Texture7")
      		{
			iI = 6;
		}
	}
	}
}
};
App shape3D;
int init(const char* aTitle)
{
	shape3D.init("");
	return 0;
}

void render()
{
	shape3D.render();
}
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("VertexDisplacement2D");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();	
	}
	return 0;
}

