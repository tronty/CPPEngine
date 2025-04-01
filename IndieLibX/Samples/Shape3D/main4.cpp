/*
  Copyright (c) 2024 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>
#include <FW3.h>

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
//define alternativeshader eDiffuseLighting
//define defaultshader ePhongLighting1
//define alternativeshader ePhongLighting2
#define alternativeshader defaultshader

    D3DXFROMWINEMATRIX lightSpaceMatrix;
	TextureID depthMapFBO;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
class _Shape3D: public STXGUI
{
	D3DXFROMWINEMATRIX model;
	D3DXFROMWINEMATRIX view;
	D3DXFROMWINEMATRIX projection;
	ShaderID shd;
	VertexFormatID vf;
	TextureID texID;//, tileTexID, windowTexID;
	MeshRenderer2 shape3D[20];
	unsigned int m_i, m_s;
public:
int init(const char* aTitle)
{
	int slices = (int) 10*(4 * std::cbrt(1.0f) + 6);
	LOG_PRINT("slices=%d\n", slices);        	
	m_s=0;      	
	m_i=0;
        shape3D[0].CreateTexturedBox(1.0f, 1.0f, 1.0f, defaultshader);
        shape3D[1].CreateBox(1.0f, 1.0f, 1.0f, defaultshader);
	shape3D[2].CreateCylinder(1.0f, 1.0f, 1.0f, defaultshader);
	shape3D[3].CreateCylinder(1.0f, 1.0f, 1.0f, defaultshader, 2);	
        shape3D[4].CreateSphere(1.0f, defaultshader);
        shape3D[5].CreateHalfSphere(1.0f, defaultshader);
        shape3D[6].CreateOcta(defaultshader);
        shape3D[7].CreateTetra(defaultshader);
        shape3D[8].CreateEllipsoid(1.0f, 1.5f, 2.0f, defaultshader);
        shape3D[9].CreateCone(1.0f, 1.0f, defaultshader);
        shape3D[10].CreateHemis(defaultshader);
        shape3D[11].CreateTorus(0.1f, 1.0f, defaultshader);
        shape3D[12].CreateTorus2(0.1f, 1.0f, defaultshader);
        shape3D[13].CreateTire(0.25f, 0.75f, defaultshader);       
        shape3D[14].CreatePrism(defaultshader);
#if 0
#if 0
        shape3D[15].CreateIcosahedron(defaultshader);
#else
        shape3D[16].CreateIcosahedron_(1.0f, defaultshader);
#endif
        shape3D[16].CreateDodecahedron(defaultshader);
#endif
        shape3D[15].CreateTetrahedron(1.0f, defaultshader);
        shape3D[16].CreateHexahedron(1.0f, defaultshader);
        shape3D[17].CreateOctahedron(1.0f, defaultshader);
        shape3D[18].CreateDodecahedron(1.0f, defaultshader);
        shape3D[19].CreateIcosahedron(1.0f, defaultshader); 

	STXGUI::init("/MeshRenderer2/GUILayout.xml");

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);
#if 0
	tileTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/textures/ATI_SDK/1024px-brick.png", false, ss);
	//texID=tileTexID;
    windowTexID=IRenderer::GetRendererInstance()->addImageLibTexture("/RadeonTerrainDemo/CastleWindow256px.png", false, ss);
#endif
#if 0
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/vnoise/vnoise.hlsl", "main", "main");
#else
	shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SDL2ProceduralArchitecture/SDL2ProceduralArchitecture.glsl", "main", "main");
#endif
	FormatDesc fmt[] = { 
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 };
	vf = IRenderer::GetRendererInstance()->addVertexFormat(fmt, elementsOf(fmt), shd);

#if 0
	// Samplerstates
	if ((linear = renderer->addSamplerState(LINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;
	if ((nearestClamp = renderer->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((bilinearClamp = renderer->addSamplerState(BILINEAR, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;
	if ((trilinearAniso = renderer->addSamplerState(TRILINEAR, WRAP, WRAP, WRAP)) == SS_NONE) return false;
#endif
	// Height render target
	depthMapFBO = IRenderer::GetRendererInstance()->addRenderTarget(SHADOW_WIDTH, SHADOW_HEIGHT, FORMAT_R32F, IRenderer::GetRendererInstance()->GetnearestClamp());

	return 0;
}
void SetParam()
{
	D3DXFROMWINEVECTOR3 lightPos(1.2f, 1.0f, 2.0f);
	D3DXFROMWINEVECTOR3 lightColor(1.0f, 1.0f, 1.0f);
	D3DXFROMWINEVECTOR3 viewPos(0.0f, 0.0f, 3.0f);
	D3DXFROMWINEVECTOR3 ambientColor(0.2f, 0.2f, 0.2f);
    
	IRenderer::GetRendererInstance()->setShaderConstant3f("lightPos", lightPos);
	IRenderer::GetRendererInstance()->setShaderConstant3f("lightColor", lightColor);
	IRenderer::GetRendererInstance()->setShaderConstant3f("viewPos", viewPos);
	IRenderer::GetRendererInstance()->setShaderConstant3f("ambientColor", ambientColor);

    // Configure matrices
    D3DXFROMWINEMATRIX lightProjection, lightView;
    float near_plane = 1.0f, far_plane = 7.5f;
    D3DXFROMWINEMatrixOrthoOffCenterLH(&lightProjection, -10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    {
    	{D3DXFROMWINEVECTOR3 eye(1.2f, 1.0f, 2.0f);
    	D3DXFROMWINEVECTOR3 at(0.0f, 0.0f, 0.0f);
    	D3DXFROMWINEVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&lightView, &eye, &at, &up);}
	}
    lightSpaceMatrix = lightProjection * lightView;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("lightSpaceMatrix", lightSpaceMatrix);
	
	
	#if 0
  {D3DXFROMWINEVECTOR3 v1(1.2f, 1.0f, 2.0f);
  D3DXFROMWINEVECTOR3 v2(0.0f, 0.0f, 0.0f);
  D3DXFROMWINEVECTOR3 v3(0.0f, 1.0f, 0.0f);
	D3DXFROMWINEMatrixLookAtLH(&view, &v1, &v2, &v3);}
	    D3DXFROMWINEMatrixPerspectiveFovLH( &projection, D3DXFROMWINEToRadian( 45.0f ),
                                //640.0f / 480.0f, 0.1f, 100.0f
								IRenderer::GetRendererInstance()->GetAspect(), 0.001f, 20.0f
		#else
D3DXFROMWINEMatrixIdentity(&view);
D3DXFROMWINEMatrixIdentity(&projection);
#endif						);
D3DXFROMWINEMatrixIdentity(&model);
}
void render( )
{
	{
	#if 0
        // Render to depth map
        IRenderer::GetRendererInstance()->changeRenderTarget(depthMapFBO); 
	IRenderer::GetRendererInstance()->BeginScene();       
        IRenderer::GetRendererInstance()->viewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//IRenderer::GetRendererInstance()->Clear(false, true,
	IRenderer::GetRendererInstance()->Clear(true, true,
		//D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7f, 1.0f));
		D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
        // Render scene from light's perspective (depth-only)
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("lightSpaceMatrix", lightSpaceMatrix);
        
        //IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	renderScene( );
        IRenderer::GetRendererInstance()->EndScene();
        //IRenderer::GetRendererInstance()->Present( );

	//IRenderer::GetRendererInstance()->setTexture("Height", heightRT, IRenderer::GetRendererInstance()->GetnearestClamp());

	IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	#endif
	IRenderer::GetRendererInstance()->BeginScene();
	{
		int w=STX_Service::GetWindowInstance()->Getwidth();
		int h=STX_Service::GetWindowInstance()->GetHeight();
        	IRenderer::GetRendererInstance()->viewport(0, 0, w, h);
        }
        IRenderer::GetRendererInstance()->setTexture("shadowMap", depthMapFBO);
	IRenderer::GetRendererInstance()->Clear(true, true,
		D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7f, 1.0f));
		//D3DXFROMWINEVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
        // Render scene
        renderScene( );
	if(1)
	{
	STXGUI::update();

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

		char txt2[128];
		stx_snprintf(txt2, 128, "m_i=%d\n", m_i);
		IRenderer::GetRendererInstance()->drawText(txt2, 10, 50, 
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(), 
			IRenderer::GetRendererInstance()->GetlinearClamp(), 
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(), 
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	}		
        IRenderer::GetRendererInstance()->EndScene();
        IRenderer::GetRendererInstance()->Present( );
	}
}
void renderScene( )
{
	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

	if((m_i==2)||(m_i==3)||(m_i==4))
		mAngleY+=180.0f;

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
/*
	if(m_s==0)
	{
	printf("0\n");
		SetParam();
	if(m_i==2)
	{
	printf("1\n");
        	shape3D[m_i].Draw(&matRot, tileTexID, -1, -1, a, d, l, e);
        	//shape3D[m_i].Draw(&matRot, windowTexID, -1, -1, a, d, l, e);
	}
	else{
	printf("2\n");
        	shape3D[m_i].Draw(&matRot, texID, -1, -1, a, d, l, e);
	}}
	else */
	{
	//printf("3\n");
	#if 0
	shape3D[m_i].BeginDraw(0,-1,shd,vf);
	#else
	//printf("shd=%d\n", shd);
	//printf("vf=%d\n", vf);
	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
	#endif
	
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("model", model);
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("view", view);
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("projection", projection);	
	
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot );
	float displacement=1.0; 
    IRenderer::GetRendererInstance()->setShaderConstant1f("Displacement", displacement );
    IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
    	SetParam();
    	int j=0;
			if(shape3D[m_i].meshes[j].rindices[0].size()/3)
			{
    				int z=0;
				//LOG_FNLN;
				#if 0
				printf("DrawIndexedPrimitiveUP:\n");
				printf("meshes[%d].vertices.size()=%d\n", j, shape3D[m_i].meshes[j].vertices.size());
				printf("meshes[%d].rindices[%d].size()=%d\n", j, z, shape3D[m_i].meshes[j].rindices[z].size());
				printf("&meshes[%d].vertices[0]=%x\n", j, &shape3D[m_i].meshes[j].vertices[0]);
				printf("&meshes[%d].rindices[%d].at(0)=%x\n", j, z, &shape3D[m_i].meshes[j].rindices[z].at(0));
				//printf("1\n");
				#endif
				IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, shape3D[m_i].meshes[j].vertices.size(), shape3D[m_i].meshes[j].rindices[z].size()/3, &shape3D[m_i].meshes[j].rindices[z].at(0), &shape3D[m_i].meshes[j].rindices[z].at(0), CONSTANT_INDEX2, &shape3D[m_i].meshes[j].vertices[0], &shape3D[m_i].meshes[j].vertices[0], sizeof(stx_VertexPositionNormalTexture));
			}else{
				//LOG_FNLN;
				//LOG_PRINT("DrawPrimitiveUP:\n");
				//LOG_PRINT("meshes[%d].vertices.size()=%d\n", j, meshes[j].vertices.size());
				//LOG_PRINT("&meshes[%d].vertices[0]=%x\n", j, &meshes[j].vertices[0]);
				//printf("2\n");
				IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, shape3D[m_i].meshes[j].vertices.size()/3, &shape3D[m_i].meshes[j].vertices[0], &shape3D[m_i].meshes[j].vertices[0], sizeof(stx_VertexPositionNormalTexture));
			}
			
	//shape3D[m_i].EndDraw();
	}

}
virtual void actionPerformed(GUIEvent &evt)
{
#if 1
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
		if(callbackString == "TexturedBox")
      		{
			m_i = 0;
		}
         	if(callbackString == "Box")
      		{
			m_i = 1;
		}
         	if(callbackString == "Cylinder")
      		{
			m_i = 2;
		}
         	if(callbackString == "Cylinder2")
      		{
			m_i = 3;
		}
         	if(callbackString == "Sphere")
      		{
			m_i = 4;
		}
         	if(callbackString == "HalfSphere")
      		{
			m_i = 5;
		}
         	if(callbackString == "Octa")
      		{
			m_i = 6;
		}
         	if(callbackString == "Tetra")
      		{
			m_i = 7;
		}
         	if(callbackString == "Ellipsoid")
      		{
			m_i = 8;
		}
#if 1
         	if(callbackString == "Cone")
      		{
			m_i = 9;
		}
         	if(callbackString == "Hemis")
      		{
			m_i = 10;
		}
         	if(callbackString == "Torus")
      		{
			m_i = 11;
		}
         	if(callbackString == "Torus2")
      		{
			m_i = 12;
		}
         	if(callbackString == "Tire")
      		{
			m_i = 13;
		}
		if(callbackString == "Prism")
      		{
			m_i = 14;
		}
		if(callbackString == "Tetrahedron")
      		{
			m_i = 15;
		}
		if(callbackString == "Hexahedron")
      		{
			m_i = 16;
		}
		if(callbackString == "Octahedron")
      		{
			m_i = 17;
		}
		if(callbackString == "Dodecahedron")
      		{
			m_i = 18;
		}
		if(callbackString == "Icosahedron")
      		{
			m_i = 19;
		}
         	if(callbackString == "Simple")
      		{
			m_s = 0;
		}
         	if(callbackString == "vnoise")
      		{
			m_s = 1;
		}
#endif
	}
	}
	#endif
}
};
int ApplicationLogic()
{	
	IRenderer* r=IRenderer::GetRendererInstance("Shape3D2");	
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

