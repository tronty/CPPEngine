/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>





std::vector<TextureID> textures;
TextureID texID=-1;

IRenderer* r=0;
IInput*    in=0;

float mAngleX=0.0f;
float mAngleY=0.0f;

std::vector<stx_Texture> Textures;
std::vector<stx_Material> vt;
std::vector<stx_Mesh> mesh_;

void dump(const char* n, std::vector<std::string>& v)
{
	LOG_PRINT("name=%s\n", n);
	for(unsigned int i=0;i<v.size();i++)
	{
		LOG_PRINT("v[%d]=%s\n", i, v[i].c_str());	
	}
}

std::vector<Vertex> g_vertices_;

std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
std::vector<D3DXFROMWINEVECTOR3> g_normals3;
std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
std::vector<__WORD__> g_indices1;
std::vector<__WORD__> g_indices2;
D3DXFROMWINEVECTOR3 g_color3(1, 1, 1);

std::vector<stx_VertexPositionNormalTexture> Cylindervertices;
//std::vector<unsigned int> indices;

D3DXFROMWINEMATRIX R,T;

void DrawVertex(D3DXFROMWINEMATRIX& aW, D3DXFROMWINEVECTOR3& aC, D3DXFROMWINEVECTOR2& aTC, D3DXFROMWINEVECTOR3& aV_)
{
	//LOG_FNLN;
	D3DXFROMWINEVECTOR3 aV;
	D3DXFROMWINEVec3TransformCoord(&aV, &aV_, &aW);
	g_vertices3.push_back(aV);
	D3DXFROMWINEVECTOR3 N;
	D3DXFROMWINEVec3Normalize(&N, &aV);
	g_normals3.push_back(N);
	g_texcoords2.push_back(aTC);
	g_indices1.push_back(g_vertices3.size()-1);
	if(0==((g_indices1.size()%3)))
		g_indices2.push_back(0);
	g_color3=aC;
	Vertex vx(g_vertices3[g_vertices3.size()-1], g_texcoords2[g_texcoords2.size()-1]);
	g_vertices_.push_back(vx);
}

void makeCylinder(float height, float base)
{
	g_color3=D3DXFROMWINEVECTOR3(0.64f, 0.16, 0.16f);
	D3DXFROMWINEMatrixRotationX(&R, -90.0f);

	//gluCylinder(obj, base,base-(0.2*base), height, 20,20);
	MeshRenderer2 shape3D;	
	shape3D.CreateCylinder(base,base-(0.2*base), height, 20, 20, eSimpleShader);

	Cylindervertices=shape3D.vertices;

	for(unsigned int i=0;i<Cylindervertices.size();i++)
	{
		D3DXFROMWINEVec3TransformCoord(&Cylindervertices[i].Position, &Cylindervertices[i].Position, &R);
		D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);
		D3DXFROMWINEMATRIX W=R*T;
		DrawVertex(W, g_color3, tc, Cylindervertices[i].Position);
	}
}

void makeTree(float height, float base)
{
	float angle;
	makeCylinder(height, base);
	D3DXFROMWINEMatrixTranslation(&T, 0.0, height, 0.0);
	height -= height*.2; base-= base*0.3;
	for(int a= 0; a<3; a++)
	{
		angle = stx_rand()%50+20;
		if(angle >48)
			angle = -(stx_rand()%50+20);
		if (height >1)
		{
			//glPushMatrix();
			//glRotatef(angle,1,0.0,1);
			D3DXFROMWINEVECTOR3 v(1,0.0,1);
			D3DXFROMWINEMatrixRotationAxis(&R, &v, angle);
			makeTree(height,base);
			//glPopMatrix();
		}
 		//else glColor3f(0.0,1.0/a,0.0);glutSolidSphere(.2,10,10); add like fruits...
	}
}

int init(const char* aTitle)
{
	//gluPerspective(30.0, (GLfloat) w/(GLfloat) h, 0.001, 1000.0);
	//glTranslatef(0.0,-8.0,-50.0);

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
#if 0
	LuaScript script;
	bool r=script.Load("/ProceduralBuilding/ProceduralBuilding.lua");
	std::string texturefilenames_ = script.get<std::string>("textures");
#else
	TiXmlDocument* document;
	TiXmlHandle* docHandle;
	TiXmlHandle* BodyHandle;
	std::string fn=stx_convertpath("/ProceduralBuilding/ProceduralBuilding.xml");
	LOG_PRINT("fn.c_str()=%s\n", fn.c_str());

	TiXmlDocument document(fn.c_str());
	document.LoadFile();
	TiXmlHandle docHandle( &document );
	TiXmlHandle BodyHandle = docHandle.FirstChild("Body");
	TiXmlElement* BodyElement = BodyHandle.Element();
	if(!BodyElement) {LOG_PRINT("!BodyElement\n");stx_exit(0);}

	std::string texturefilenames_ = BodyHandle.FirstChild("Textures").Element()->GetText();
#endif
	std::vector<std::string> vtex=stx_split(texturefilenames_, ':');
	D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
	for(unsigned int i=0;i<vtex.size();i++)
	{
		//LOG_PRINT("vtex[%d]:%s\n", i, vtex[i].c_str());
		textures.push_back(IRenderer::GetRendererInstance()->addImageLibTexture(vtex[i].c_str(), false, ss));
		vt.push_back(stx_Material(vtex[i].c_str()+1, vC));
	}
	makeTree(4.0f, 0.2f);

	LOG_PRINT("g_vertices3.size()=%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()=%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()=%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()=%d\n", g_indices2.size());

	#if 0
	std::vector<stx_Material> vt;
	D3DXFROMWINEVECTOR4 color=stx_GetRGBA(eGreen);
	vt.push_back(stx_Material("Orcs/Black.png", D3DXFROMWINEVECTOR3(color.x, color.y, color.z)));
	stx_Texture texture(vt, g_indices2);
	stx_writeXFile("/TexturedCube.x", mesh_);//, g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);
	#endif
	return 0;
}

void render()
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

	IRenderer::GetRendererInstance()->InitTex3(IRenderer::GetRendererInstance()->GettexShader3(), IRenderer::GetRendererInstance()->GettexVF3());
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader3());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF3());
	D3DXFROMWINEMATRIX R, S, W;
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	D3DXFROMWINEMatrixScaling(&S, 0.25f, 0.25f, 0.25f);
	W=R*S;
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
	IRenderer::GetRendererInstance()->setTexture("Sampler", texID);
	if(g_indices1.size())
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, g_vertices_.size(), g_indices1.size()/3, &g_indices1[0], &g_indices1[0], CONSTANT_INDEX2, &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));
	else
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, g_vertices_.size()/3, &g_vertices_[0], &g_vertices_[0], sizeof(Vertex));

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
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("3D Tree Using Recursion");//1200, 800
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

