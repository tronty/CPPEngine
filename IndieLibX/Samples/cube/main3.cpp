/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
TextureID texture=-1;
enum eV
{
	eV1,
	eV2,
	eV3,
	eV4,
	eV5,
	eV6,
	eV7,
	eV8
};

const float vertices[] = {
#if 0
	 1.0f,-1.0f, 1.0f,   //V1
	 1.0f, 1.0f, 1.0f,   //V2
	 1.0f,-1.0f,-1.0f,   //V3
	 1.0f, 1.0f,-1.0f,   //V4
	-1.0f,-1.0f,-1.0f,   //V5
	-1.0f, 1.0f,-1.0f,   //V6
	-1.0f,-1.0f, 1.0f,   //V7
	-1.0f, 1.0f, 1.0f    //V8
#else
	 1.0f,-1.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
#endif
};

const unsigned int surfaces[] = {
	eV1,eV2,eV3,eV3,eV2,eV4,
	eV6,eV5,eV8,eV8,eV5,eV7,
	eV5,eV1,eV7,eV7,eV1,eV3,
	eV2,eV6,eV4,eV4,eV6,eV8,
	eV5,eV6,eV1,eV1,eV6,eV2,
	eV3,eV4,eV7,eV7,eV4,eV8,
/*
Triangle List

    Front: 1-2-3, 3-2-4
    Back: 6-5-8, 8-5-7
    Left: 5-1-7, 7-1-3
    Right: 2-6-4, 4-6-8
    Top: 5-6-1, 1-6-2
    Bottom: 3-4-7, 7-4-8

How about a Triangle Fan

    Front: 1-2-4, 1-4-3
    Left: 1-3-7, 1-7-5
    Top: 1-5-6, 1-6-2

    Back: 8-6-5, 8-5-7
    Bottom: 8-7-3, 8-3-4
    Right: 8-4-2, 8-2-6

Triangle Strip

    Front: 1-2-3, 3-2-4
    Back: 6-5-8, 8-5-7
    Left: 5-1-7, 7-1-3
    Right: 2-6-4, 4-6-8
    Top: 5-6-1, 1-6-2
    Bottom: 3-4-7, 7-4-8
*/
};

const unsigned int edges[] = {
	eV2,eV1,
	eV3,eV4,

	eV1,eV4,
	eV1,eV5,
	eV3,eV2,
	eV3,eV8,
	eV7,eV4,
	eV7,eV5,
	eV7,eV8,
	eV6,eV2,
	eV6,eV5,
	eV6,eV8};

const float colors[] = {
	0.0f,1.0f,0.0f,
	0.0f,0.75f,0.0f,
	0.0f,0.5f,0.0f,
	0.0f,0.25f,0.0f};

void Cube1(const D3DXFROMWINEMATRIX& MVP)
{
#if 0
#if 1
    //LOG_FNLN;
    //LOG_PRINT("PRIM_QUADS:\n");
    IRenderer::GetRendererInstance()->Begin(PRIM_QUADS);
#if 0
    for surface in surfaces:
        x = 0
        for vertex in surface:
            x+=1
            glColor3fv(colors[x])
            glVertex3fv(verticies[vertex])
#endif
    for(unsigned int surface=0;surface<elementsOf(surfaces)/4;surface++)
    {
        for(unsigned int vertex=0;vertex<4;vertex++)
        {
            IRenderer::GetRendererInstance()->Color3fv(&colors[vertex]);
            IRenderer::GetRendererInstance()->Vertex3fv(&vertices[4*surface+vertex]);
	}
    }
    IRenderer::GetRendererInstance()->End(-1, &MVP);
#endif
#else
    IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);
    for(unsigned int surface=0;surface<elementsOf(surfaces)/4;surface++)
    {
        for(unsigned int vertex=0;vertex<4;vertex++)
        {
            IRenderer::GetRendererInstance()->Color3fv(&colors[vertex]);
            IRenderer::GetRendererInstance()->Vertex3fv(&vertices[4*surface+vertex]);
	}
    }
    IRenderer::GetRendererInstance()->End(-1, &MVP);
#endif
}

void Cube2(const D3DXFROMWINEMATRIX& MVP)
{
    //LOG_FNLN;
    //LOG_PRINT("PRIM_LINES:\n");
    IRenderer::GetRendererInstance()->Begin(PRIM_LINES);
    for(unsigned int edge=0;edge<elementsOf(edges)/2;edge++)
    {
        IRenderer::GetRendererInstance()->Vertex3fv(&vertices[2*edge+0]);
        IRenderer::GetRendererInstance()->Vertex3fv(&vertices[2*edge+1]);
    }
    IRenderer::GetRendererInstance()->End(-1, &MVP);
}

void Cube4(const D3DXFROMWINEMATRIX& MVP)
{
    IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLE_STRIP);

    IRenderer::GetRendererInstance()->Vertex3f( 1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f( 1.0f,  1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 1.0f,  0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(0.0f,  1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 0.0f, 0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[3]);

    IRenderer::GetRendererInstance()->Vertex3f( 1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f( 1.0f,  1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 1.0f,  0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(0.0f,  1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 0.0f, 0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[3]);

    IRenderer::GetRendererInstance()->Vertex3f( 1.0f,  1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f( 1.0f,  1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 1.0f,  0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f,  1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(0.0f,  1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 0.0f, 0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[3]);

    IRenderer::GetRendererInstance()->Vertex3f( 1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f( 1.0f,  -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 1.0f,  0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(0.0f,  1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 0.0f, 0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[3]);

    IRenderer::GetRendererInstance()->Vertex3f( 1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f( 1.0f, 1.0f,  1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 1.0f,  0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(0.0f,  1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 0.0f, 0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[3]);

    IRenderer::GetRendererInstance()->Vertex3f( -1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f( -1.0f, 1.0f,  1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 1.0f,  0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(0.0f,  1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f( 0.0f, 0.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[3]);

    IRenderer::GetRendererInstance()->End(texture, &MVP, "../../IndieLib_resources/cube.x", "../../IndieLib_resources/test.bmp");
}

void Cube3(const D3DXFROMWINEMATRIX& MVP)
{
    IRenderer::GetRendererInstance()->Begin(PRIM_TRIANGLES);

    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
                
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);

    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
                
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);

    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
                
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);

    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
                
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);

    IRenderer::GetRendererInstance()->Vertex3f( 1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
                
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);

    IRenderer::GetRendererInstance()->Vertex3f( -1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);
                
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, -1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, -1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[1]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, 1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[0]);
    IRenderer::GetRendererInstance()->Vertex3f(-1.0f, -1.0f, 1.0f);
    IRenderer::GetRendererInstance()->TexCoord2f(-1.0f, 1.0f);
    IRenderer::GetRendererInstance()->Color3fv(&colors[2]);

    IRenderer::GetRendererInstance()->End(texture, &MVP);
}

int init(const char* aTitle)
{
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texture=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	#if 0
    	glTranslatef();
        glRotatef(1, 3, 1, 1);
	#endif

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY, 0.25f);

		D3DXFROMWINEMATRIX MVP, R, S, T;
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );

		D3DXFROMWINEMatrixScaling(&S, 0.1f, 0.1f, 0.1f);
		D3DXFROMWINEMatrixTranslation(&T, 0.0f, 0.0f, 0.0f);

	MVP=R*S*T;

#if 0
    Cube1(MVP);
    Cube2(MVP);
#elif 0
    Cube3(MVP);
#else
    Cube4(MVP);
#endif

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
	IRenderer::GetRendererInstance()->Present( );
	//stx_exit(0);
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("cube3");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
#if 0
bool CD3dmeshInst::UpdateWorldMatrix()
{
	if(!mDynamic) return false;
	if(!mIsMoved && !mIsRotated && !mIsScaled) return false;

	if(mIsMoved)
	{
//		D3DXMatrixIdentity(&mMatTranslate);
		D3DXMatrixTranslation(&mMatTranslate, mWorldPos.x, mWorldPos.y, mWorldPos.z);
		mIsMoved = false;
	}
	if(mIsRotated)
	{
//		D3DXMatrixIdentity(&mMatRotateX);
		D3DXMatrixRotationX(&mMatRotateX, D3DXToRadian(mRot.x));
//		D3DXMatrixIdentity(&mMatRotateY);
		D3DXMatrixRotationY(&mMatRotateY, D3DXToRadian(mRot.y));
//		D3DXMatrixIdentity(&mMatRotateZ);
		D3DXMatrixRotationZ(&mMatRotateZ, D3DXToRadian(mRot.z));
		mIsRotated = false;
	}
	if(mIsScaled)
	{
//		D3DXMatrixIdentity(&mMatScale);
		D3DXMatrixScaling(&mMatScale, mScale, mScale, mScale);
		mIsScaled = false;
	}
	mMatWorld = (D3DXMATRIXA16)(mMatScale*mMatRotateX*mMatRotateY*mMatRotateZ*mMatTranslate);

	D3DXMatrixInverse(&mMatWorldInv, NULL, &mMatWorld);
	D3DXMatrixTranspose(&mMatWorldInvTransp, &mMatWorldInv);
	return true;
}
#endif

