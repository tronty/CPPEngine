/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

const char *getFormatString(const FORMAT format);

ShaderID effect2;
TextureID texture,texture2,texture3,texture4;
struct PCVertex {
	D3DXFROMWINEVECTOR2 vertex;
	//D3DXFROMWINEVECTOR4 color;
};
unsigned int drawRoundRect_(const float x0, const float y0, const float x1, const float y1, const float r, const D3DXFROMWINEVECTOR4 &color, const float lineWidth=0){
	unsigned int col = toBGRA(color);

	bool lines = (lineWidth > 0);
	float d = 0.5f * lineWidth;

	PCVertex vertices[42];

	int k = 0;
	// Four arcs
	#if 1
	for (int j = 0; j < 4; j++)
	#else
	int j = 0;
	#endif
	{
		// Arc center
		float x = (j >= 1 && j <= 2)? x0 + r : x1 - r;
		float y = (j > 1)? y0 + r : y1 - r;

		// Arc
		for (int i = 0; i <= 4; i++){
			float ang = (4 * j + i) * 2 * D3DXFROMWINE_PI / 16;
			float cosA = cosf(ang);
			float sinA = sinf(ang);

			if (lines){
				vertices[2 * k    ].vertex = D3DXFROMWINEVECTOR2(x + (r + d) * cosA, y + (r + d) * sinA);
				vertices[2 * k + 1].vertex = D3DXFROMWINEVECTOR2(x + (r - d) * cosA, y + (r - d) * sinA);
				//vertices[2 * k    ].color = color;
				//vertices[2 * k + 1].color = color;
			} else {
				vertices[k].vertex = D3DXFROMWINEVECTOR2(x + r * cosA, y + r * sinA);
				//vertices[k].color = color;
			}
			k++;
		}
	}
	static ShaderID plainShader=-1;
	static VertexFormatID plainVF=-1;
	IRenderer::GetRendererInstance()->InitPlain(plainShader, plainVF);
	IRenderer::GetRendererInstance()->setShader(plainShader);
	IRenderer::GetRendererInstance()->setVertexFormat(plainVF);
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", color);
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	if (lines){
		// Full loop
		vertices[40] = vertices[0];
		vertices[41] = vertices[1];
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 40, vertices, vertices, sizeof(PCVertex));
	} else {
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_FAN, 18, vertices, vertices, sizeof(PCVertex));
	}
	return 0;
}

int init(const char* aTitle)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->GetlinearClamp();
	texture=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/test.bmp"
		,false,ss);
	texture2=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/dx8sdk/tree01S.png"
		,false,ss);
	texture3=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/AdvAniCh12/Particle_People1.png"
		,false,ss);
	texture4=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/NSGUI/GUIElements.png"
		,false,ss);
	return 0;
}

void drawTexture(TextureID texture, int x, int y, int sx, int sy, bool InScene, float tx, float ty, float stx, float sty)
{
	IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(),IRenderer::GetRendererInstance()->GettexVF());

	float qx=x;
	float qy=y;
	float dx=sx;
	float dy=sy;

	float* vertices=new float[16];
	bool fq=((tx==0.0f)&&(ty==0.0f)&&(stx==1.0f)&&(sty==1.0f));
	{
		if(fq)
		{
			float verts2[] =
			{
				qx+dx,qy,1.0f, 0.0f
				,qx+dx,qy+dy,1.0f, 1.0f
				,qx,qy,0.0f, 0.0f
				,qx,qy+dy,0.0f, 1.0f
			}
			;
			stx_memcpy(vertices,verts2,16*sizeof(float));
		}
		else
		{
			float verts2[] =
			{
				qx+dx,qy,tx+stx,ty
				,qx+dx,qy+dy,tx+stx,ty+sty
				,qx,qy,tx,ty
				,qx,qy+dy,tx,ty+sty
			}
			;
			stx_memcpy(vertices,verts2,16*sizeof(float));
		}
	}
	if(!InScene)
	{
		IRenderer::GetRendererInstance()->BeginScene();
	}
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF());
	IRenderer::GetRendererInstance()->setTexture("Base", texture);
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias",
		IRenderer::GetRendererInstance()->GetscaleBias2D());

	IRenderer::GetRendererInstance()->DrawPrimitiveUP(
		PRIM_TRIANGLE_STRIP,
		IRenderer::GetRendererInstance()->getPrimitiveCount(PRIM_TRIANGLE_STRIP, 4),
		vertices, vertices, 4*sizeof(float));
	if(!InScene)
	{
		IRenderer::GetRendererInstance()->EndScene();
	}
	delete[] vertices;
}

void drawBlending( void )
{
	static int s=0;
	drawTexture(texture, 0, 0, 400, 300, true, 0.0f, 0.0f, 1.0f, 1.0f);
	drawTexture(texture2, 400, 300, 400, 300, true, 0.0f, 0.0f, 1.0f, 1.0f);
}
	void renderframe(D3DXFROMWINEVECTOR2 aposition, D3DXFROMWINEVECTOR2 asize, TextureID texid, D3DXFROMWINEVECTOR2 atex=D3DXFROMWINEVECTOR2(0.0f, 0.0f), D3DXFROMWINEVECTOR2 atexsize=D3DXFROMWINEVECTOR2(1.0f, 1.0f)){
	TexVertex dest[4];
	float x=aposition.x;
	float y=aposition.y;
	dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
	dest[0].texCoord = D3DXFROMWINEVECTOR2(atex.x + atexsize.x, atex.y);
	dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
	dest[1].texCoord = D3DXFROMWINEVECTOR2(atex.x + atexsize.x, atex.y + atexsize.y);
	dest[2].position = D3DXFROMWINEVECTOR2(x, y);
	dest[2].texCoord = D3DXFROMWINEVECTOR2(atex.x, atex.y);
	dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
	dest[3].texCoord = D3DXFROMWINEVECTOR2(atex.x, atex.y + atexsize.y);
	IRenderer::GetRendererInstance()->drawTextured(PRIM_TRIANGLE_STRIP, dest, 2, texid, IRenderer::GetRendererInstance()->GetblendSrcAlpha(), IRenderer::GetRendererInstance()->GetnoDepthTest());
}

void render5(const TextureID texture, float qx, float qy, float dx, float dy, float tx=0.0f, float ty=0.0f, float stx=1.0f, float sty=1.0f)
{
	D3DXFROMWINEVECTOR4 c(1.0f, 1.0f, 1.0f, 1.0f);
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);

	float sizeX=dx;
	float sizeY=dy;

	IRenderer::GetRendererInstance()->TexCoord2f( sty, sty);
	IRenderer::GetRendererInstance()->Vertex2f(sizeX,-sizeY);
	IRenderer::GetRendererInstance()->TexCoord2f( sty, 0);
	IRenderer::GetRendererInstance()->Vertex2f(sizeX,sizeY);

	IRenderer::GetRendererInstance()->TexCoord2f( 0, sty);
	IRenderer::GetRendererInstance()->Vertex2f( -sizeX,-sizeY);

	IRenderer::GetRendererInstance()->TexCoord2f( 0, 0);
	IRenderer::GetRendererInstance()->Vertex2f( -sizeX,sizeY);

	IRenderer::GetRendererInstance()->End(texture);
}

void render4(const TextureID texture, float qx, float qy, float dx, float dy, float tx=0.0f, float ty=0.0f, float stx=1.0f, float sty=1.0f)
{
	D3DXFROMWINEVECTOR4 c(1.0f, 1.0f, 1.0f, 1.0f);
	IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);

	IRenderer::GetRendererInstance()->TexCoord2f(1, 0);IRenderer::GetRendererInstance()->Vertex2f(qx,qy+dy);
	IRenderer::GetRendererInstance()->TexCoord2f(1, 1);IRenderer::GetRendererInstance()->Vertex2f(qx,qy);
	IRenderer::GetRendererInstance()->TexCoord2f(0, 0);IRenderer::GetRendererInstance()->Vertex2f(qx+dx,qy+dy);
	IRenderer::GetRendererInstance()->TexCoord2f(0, 1);IRenderer::GetRendererInstance()->Vertex2f(qx+dx,qy);

	IRenderer::GetRendererInstance()->End(texture);
}

void render3(const TextureID texture, float qx, float qy, float dx, float dy, float tx=0.0f, float ty=0.0f, float stx=1.0f, float sty=1.0f)
{
	D3DXFROMWINEVECTOR4 c(1.0f, 1.0f, 1.0f, 1.0f);

	float xPos=qx;
	float yPos=qy;
	float height=dx;
		
TexVertex quad[] = {  MAKETEXQUAD2(qx, qy, dx, tx, ty, stx) };

	IRenderer::GetRendererInstance()->drawTexture(texture, qx, qy, dx, dy, true, tx, ty, stx, sty);

	TexVertex verticestex[] = {
	TexVertex(D3DXFROMWINEVECTOR2(qx+dx,qy), D3DXFROMWINEVECTOR2(tx+stx,ty)),
	TexVertex(D3DXFROMWINEVECTOR2(qx+dx,qy+dy), D3DXFROMWINEVECTOR2(tx+stx,ty+sty)),
	TexVertex(D3DXFROMWINEVECTOR2(qx,qy), D3DXFROMWINEVECTOR2(tx,ty)),
	TexVertex(D3DXFROMWINEVECTOR2(qx,qy+dy), D3DXFROMWINEVECTOR2(tx,ty+sty))
				};
	IRenderer::GetRendererInstance()->drawTextured(PRIM_TRIANGLE_STRIP, &verticestex[0], 4, texture, IRenderer::GetRendererInstance()->GetblendSrcAlpha(), IRenderer::GetRendererInstance()->GetnoDepthTest(), &c);
}
void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));

	IRenderer::GetRendererInstance()->BeginScene();
#if 1

	//drawBlending();

	IRenderer::GetRendererInstance()->drawLine(0,0,10,10,D3DXFROMWINEVECTOR4(1,1,1,1));
		IRenderer::GetRendererInstance()->drawRect(10,10,20,20,D3DXFROMWINEVECTOR4(1,1,1,1));//,1.0f);
		IRenderer::GetRendererInstance()->drawRoundRect(20,20,30,30,5.0f,D3DXFROMWINEVECTOR4(1,1,1,1));//,1.0f);
drawRoundRect_(20,270,70,320,5.0f,D3DXFROMWINEVECTOR4(1,1,1,1));

IRenderer::GetRendererInstance()->drawEllipse(20, 370, 25, 50,D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawHyperbole(120, 370, 25, 50,D3DXFROMWINEVECTOR4(1,1,1,1));
#if 0
	float x0=220.0f;
	float y0=370.0f;
	float a2=25.0f;
	unsigned int N=100.0f;
IRenderer::GetRendererInstance()->drawParabole(x0, y0, a2, N, D3DXFROMWINEVECTOR4(1,1,1,1));
#elif 0
IRenderer::GetRendererInstance()->drawParabole(220, 370, -4, 35, 0,-5, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(1.0f, -30.0f, 450.0f, 0.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(220.0f, 0.0f, 330.0f, 220.0f, 440.0f, 0.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
#elif 0
IRenderer::GetRendererInstance()->drawParabole(0.20f, 100.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(0.16f, 200.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(0.12f, 300.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(0.08f, 400.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(0.04f, 500.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
#elif 0
IRenderer::GetRendererInstance()->drawParabole(1.25f, 100.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(1.5625f, 150.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(2.0833f, 200.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(3.125f, 250.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(6.25f, 300.0f, 300.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
#elif 1
IRenderer::GetRendererInstance()->drawParabole(100.0f, 25.0f, 50.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(200.0f, 50.0f, 75.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(300.0f, 50.0f, 100.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
#elif 0
IRenderer::GetRendererInstance()->drawParabole(100.0f, 25.0f, 50.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(200.0f, 50.0f, 75.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
IRenderer::GetRendererInstance()->drawParabole(300.0f, 50.0f, 100.0f, D3DXFROMWINEVECTOR4(1,1,1,1));
#endif

		IRenderer::GetRendererInstance()->drawTriangle(30,30,40,40,50,30,D3DXFROMWINEVECTOR4(1,1,1,1));//,1.0f);
		IRenderer::GetRendererInstance()->drawQuad(40,40,40,50,50,50,50,40,D3DXFROMWINEVECTOR4(1,1,1,1));
		IRenderer::GetRendererInstance()->drawCircle(100,100,5.0f,D3DXFROMWINEVECTOR4(1,1,1,1));//,1.0f);

	renderframe(D3DXFROMWINEVECTOR2(100.0f,100.0f), D3DXFROMWINEVECTOR2(20.0f,20.0f), texture2);
	renderframe(D3DXFROMWINEVECTOR2(250.0f,250.0f), D3DXFROMWINEVECTOR2(30.0f,30.0f), texture3);
	renderframe(D3DXFROMWINEVECTOR2(400.0f,400.0f), D3DXFROMWINEVECTOR2(50.0f,50.0f), texture);

	render5(texture4, 140.0f,150.0f,50.0f,50.0f,0.0f, 0.0f,1.0f, 1.0f);
	render5(texture4, 150.0f,160.0f,10.0f,10.0f,0.86f, 1.0f,0.8f, 1.0f);
#endif

	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("Blending");		
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();	
		render();
	}
	return 0;
}

