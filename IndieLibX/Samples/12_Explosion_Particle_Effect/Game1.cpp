/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

    struct VertexExplosion
    {
        D3DXFROMWINEVECTOR3 Position;
        D3DXFROMWINEVECTOR4 TexCoord;
        D3DXFROMWINEVECTOR4 AdditionalInfo;
        VertexExplosion(D3DXFROMWINEVECTOR3 aPosition, D3DXFROMWINEVECTOR4 aTexCoord, D3DXFROMWINEVECTOR4 aAdditionalInfo)
        {
            Position = aPosition;
            TexCoord = aTexCoord;
            AdditionalInfo = aAdditionalInfo;
        }
	VertexExplosion(const VertexExplosion& rhs)
	{
            Position = rhs.Position;
            TexCoord = rhs.TexCoord;
            AdditionalInfo = rhs.AdditionalInfo;
	}
	VertexExplosion()
        {
            Position = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
            TexCoord = D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
            AdditionalInfo = D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
        }
	};


        TextureID myTexture=-1;
        std::vector<VertexExplosion> explosionVertices;
        VertexFormatID myVertexDeclaration=-1;
        ShaderID expEffect=-1;
        //float time = 0;

        D3DXFROMWINEMATRIX I, ProjectionMatrix, ViewMatrix;
        D3DXFROMWINEVECTOR3 Position, UpVector;

        void CreateExplosionVertices(float time)
        {
            int particles = 80;
		explosionVertices.clear();
            explosionVertices.resize(particles * 6);

            int i = 0;
            for (int partnr = 0; partnr < particles; partnr++)
            {
                D3DXFROMWINEVECTOR3 startingPos(5, 0, 0);

                float r1 = (float)stx_rand() - 0.5f;
                float r2 = (float)stx_rand() - 0.5f;
                float r3 = (float)stx_rand() - 0.5f;
                D3DXFROMWINEVECTOR3 moveDirection(r1, r2, r3);
                D3DXFROMWINEVec3Normalize(&moveDirection,&moveDirection);

                float r4 = (float)stx_rand();
                r4 = r4 / 4.0f * 3.0f + 0.25f;

                explosionVertices[i++] = VertexExplosion(startingPos, D3DXFROMWINEVECTOR4(1, 1, time, 1000), D3DXFROMWINEVECTOR4(moveDirection.x, moveDirection.y, moveDirection.z, r4));
                explosionVertices[i++] = VertexExplosion(startingPos, D3DXFROMWINEVECTOR4(0, 0, time, 1000), D3DXFROMWINEVECTOR4(moveDirection.x, moveDirection.y, moveDirection.z, r4));
                explosionVertices[i++] = VertexExplosion(startingPos, D3DXFROMWINEVECTOR4(1, 0, time, 1000), D3DXFROMWINEVECTOR4(moveDirection.x, moveDirection.y, moveDirection.z, r4));

                explosionVertices[i++] = VertexExplosion(startingPos, D3DXFROMWINEVECTOR4(1, 1, time, 1000), D3DXFROMWINEVECTOR4(moveDirection.x, moveDirection.y, moveDirection.z, r4));
                explosionVertices[i++] = VertexExplosion(startingPos, D3DXFROMWINEVECTOR4(0, 1, time, 1000), D3DXFROMWINEVECTOR4(moveDirection.x, moveDirection.y, moveDirection.z, r4));
                explosionVertices[i++] = VertexExplosion(startingPos, D3DXFROMWINEVECTOR4(0, 0, time, 1000), D3DXFROMWINEVECTOR4(moveDirection.x, moveDirection.y, moveDirection.z, r4));
            }
        }

int init(const char* aTitle)
{
        expEffect = IRenderer::GetRendererInstance()->addShaderFromFile("/12_Explosion_Particle_Effect/explosionEffect.fx", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 4
	};
	myVertexDeclaration = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), expEffect);
	SamplerStateID ss=IRenderer::GetRendererInstance()->
								//Getlinear();
								GetlinearClamp();

	myTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/12_Explosion_Particle_Effect/explosion.png", false, ss);

	D3DXFROMWINEMatrixPerspectiveFovLH(&ProjectionMatrix, D3DXFROMWINE_PI/4.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight(), 0.5f, 1000.0f);
	D3DXFROMWINEMatrixTranslation(&ViewMatrix, 0.0f, 0.0f, 20.0f);

	D3DXFROMWINEMatrixIdentity(&I);
        Position=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 20.0f);
        UpVector=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);

	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	IRenderer::GetRendererInstance()->setShader(expEffect);
	IRenderer::GetRendererInstance()->setVertexFormat(myVertexDeclaration);

	if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_LEFT) || STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT) || STX_Service::GetInputInstance()->OnMouseButtonPress(STX_MBUTTON_WHEELDOWN))
		CreateExplosionVertices((float)timeGetTime()/1000.0f);

        IRenderer::GetRendererInstance()->setShaderConstant4x4f("xWorld", I);
        IRenderer::GetRendererInstance()->setShaderConstant4x4f("xProjection", ProjectionMatrix);
        IRenderer::GetRendererInstance()->setShaderConstant4x4f("xView", ViewMatrix);

        IRenderer::GetRendererInstance()->setShaderConstant3f("xCamPos", Position);
        IRenderer::GetRendererInstance()->setTexture("textureSampler", myTexture);
        IRenderer::GetRendererInstance()->setShaderConstant3f("xCamUp", UpVector);
        IRenderer::GetRendererInstance()->setShaderConstant1f("xTime", (float)timeGetTime()/1000.0f);
/*
                device.RenderState.AlphaBlendEnable = true;
                device.RenderState.SourceBlend = Blend.SourceAlpha;
                device.RenderState.DestinationBlend = Blend.One;
                device.RenderState.DepthBufferWriteEnable = false;
*/
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetnoDepthWrite());
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
    	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOneReverseSubstract());
                   
		
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, explosionVertices.size()/3, &explosionVertices[0], &explosionVertices[0], sizeof(VertexExplosion));

		const char* txt = "Use mouse buttons.";
		IRenderer::GetRendererInstance()->drawText(txt, 10, 10, 
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
	IRenderer* r=IRenderer::GetRendererInstance("12_Explosion_Particle_Effect");	
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}
