/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define defaultshader eSimpleShader
//define alternativeshader eSimpleShader
//define alternativeshader eAmbientLighting
//define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
#define alternativeshader ePhongLighting2

#undef __ANISO__ 

#define USEMESH 1
std::vector<stx_Mesh> mesh__;

enum tTerminal
{
	eQuad_Terminal,
	eVertex_Terminal,
};

struct ProceduralBuilding5
{
	ShaderID shdC;
	VertexFormatID vfC;
	TiXmlNode* BodyNode;

	std::string m_sfn;
	std::string m_sfiletosave;
	bool m_bUseColor;
	bool m_bUseLua;
	tTerminal g_Terminal;

	std::string rulestr;

	std::vector<TextureID> textures;
	TextureID texID;

	float f;

	//std::vector<stx_Texture> Textures;
	std::vector<stx_Material> vt;

	MeshRenderer2 mesh;
    float g_Rx, g_Ry;

	std::vector<Vertex> g_vertices_;
	std::vector<VertexC> g_verticesC_;
	std::vector<__WORD__> g_indices;

	std::vector<D3DXFROMWINEVECTOR3> g_vertices3;
	std::vector<D3DXFROMWINEVECTOR3> g_normals3;
	std::vector<D3DXFROMWINEVECTOR2> g_texcoords2;
	std::vector<__WORD__> g_indices1;
	std::vector<__WORD__> g_indices2;
	D3DXFROMWINEVECTOR3 g_color3_;
	D3DXFROMWINEVECTOR4 g_color4_;

	ProceduralBuilding5(){}
	void init(std::string afn="", std::string afiletosave="", bool abUseLua=true)
	{
		stx_Mesh m;
		mesh__.push_back(m);

		m_bUseColor=false;
		m_bUseLua=abUseLua;

		//g_Terminal=eQuad_Terminal;
		g_Terminal=eVertex_Terminal;

		texID=-1;
		shdC=-1;
		vfC=-1;

		f=1.0f;

		g_color3_=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);

		g_color4_=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

		if(!m_bUseLua)
		{
			std::string fn=stx_convertpath(afn.c_str());
			LOG_PRINT("fn.c_str()=%s\n", fn.c_str());
		}

		init_(afn, afiletosave);
		
		shdC = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD3N.hlsl", "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,     FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD,   FORMAT_FLOAT, 4,
			0, TYPE_TEXCOORD,   FORMAT_FLOAT, 2
		};
		vfC = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shdC);
	}
	virtual ~ProceduralBuilding5()
	{
		//Textures.clear();
		vt.clear();

		g_vertices_.clear();
		g_verticesC_.clear();

		g_vertices3.clear();
		g_normals3.clear();
		g_texcoords2.clear();
		g_indices1.clear();
		g_indices2.clear();
	}


void DrawVertex(const D3DXFROMWINEMATRIX& aW, const D3DXFROMWINEVECTOR3& aC, const D3DXFROMWINEVECTOR2& aTC, const D3DXFROMWINEVECTOR3& aV_)
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
	{
		if(texID>-1)
		{
			unsigned int  pos = find(textures.begin(), textures.end(), texID) - textures.begin();
			//LOG_FNLN;
			//LOG_PRINT("g_indices2.push_back(%d)\n", pos);
			g_indices2.push_back(pos);
		}
		else
			g_indices2.push_back(0);
	}

	g_color3_=aC;
	D3DXFROMWINEVECTOR4 color(aC.x, aC.y, aC.z, 1.0f);
	g_color4_=color;

	stx_VertexPositionNormalTexture v_;
	v_.Position=aV;
	v_.Normal=N;
	v_.Tex=aTC;
	mesh__[0].vertices.push_back(v_);
	mesh__[0].indices.push_back(g_vertices3.size()-1);

	if(m_bUseColor){
	VertexC vx(g_vertices3[g_vertices3.size()-1], color, g_texcoords2[g_texcoords2.size()-1]);
	g_verticesC_.push_back(vx);g_indices.push_back(g_verticesC_.size()-1);
	}else{
	Vertex vx(g_vertices3[g_vertices3.size()-1], g_texcoords2[g_texcoords2.size()-1]);
	g_vertices_.push_back(vx);g_indices.push_back(g_vertices_.size()-1);
	}
}

void DrawQuad(D3DXFROMWINEMATRIX& aM, D3DXFROMWINEVECTOR2 asize=D3DXFROMWINEVECTOR2(1.0f, 1.0f))
{
	stx_VertexPositionNormalTexture v_;
	{D3DXFROMWINEVECTOR3 v(-asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);VertexC vc(vo, g_color4_, tc);g_verticesC_.push_back(vc);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;v_.Tex=tc;
mesh__[0].vertices.push_back(v_);mesh__[0].indices.push_back(mesh__[0].vertices.size()-1);}
	{D3DXFROMWINEVECTOR3 v( asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);VertexC vc(vo, g_color4_, tc);g_verticesC_.push_back(vc);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;v_.Tex=tc;
mesh__[0].vertices.push_back(v_);mesh__[0].indices.push_back(mesh__[0].vertices.size()-1);}
	{D3DXFROMWINEVECTOR3 v( -asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);VertexC vc(vo, g_color4_, tc);g_verticesC_.push_back(vc);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;v_.Tex=tc;
mesh__[0].vertices.push_back(v_);mesh__[0].indices.push_back(mesh__[0].vertices.size()-1);}
	//g_indices2.push_back(vt.size()-1);
	{D3DXFROMWINEVECTOR3 v(-asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(0.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);VertexC vc(vo, g_color4_, tc);g_verticesC_.push_back(vc);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;v_.Tex=tc;
mesh__[0].vertices.push_back(v_);mesh__[0].indices.push_back(mesh__[0].vertices.size()-1);}
	{D3DXFROMWINEVECTOR3 v( asize.x, asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 1.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);VertexC vc(vo, g_color4_, tc);g_verticesC_.push_back(vc);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;v_.Tex=tc;
mesh__[0].vertices.push_back(v_);mesh__[0].indices.push_back(mesh__[0].vertices.size()-1);}
	{D3DXFROMWINEVECTOR3 v(asize.x, -asize.y, 0.0f);D3DXFROMWINEVECTOR3 vo=stx_Vec3Transform(aM, v);D3DXFROMWINEVECTOR2 tc(1.0f, 0.0f);Vertex vx(vo, tc);g_vertices_.push_back(vx);VertexC vc(vo, g_color4_, tc);g_verticesC_.push_back(vc);g_vertices3.push_back(vo);g_indices1.push_back(g_vertices3.size()-1);g_texcoords2.push_back(tc);
v_.Position=vo;v_.Tex=tc;
mesh__[0].vertices.push_back(v_);mesh__[0].indices.push_back(mesh__[0].vertices.size()-1);}
	//g_indices2.push_back(vt.size()-1);
}

void GetMeshVertices(const char* afn, std::vector<Vertex332>& aVertices, std::vector<__WORD__>& aIndices)
{
	aVertices.clear();
	aIndices.clear();
	MeshRenderer2 mesh_;
	mesh_.Load(afn, defaultshader, false, true);
	if(mesh_.meshes.size())
	{
		for(unsigned int i=0;i<mesh_.meshes[0].vertices.size();i++)
		{
			Vertex332 v;
			v.position=mesh_.meshes[0].vertices[i].Position;
			//D3DXFROMWINEVec3Normalize(&v.Normal, &v.Position);
			v.texcoord=mesh_.meshes[0].vertices[i].Tex;
			aVertices.push_back(v);
		}
		aIndices=mesh_.meshes[0].indices;
	}
}

void Draw(const std::vector<stx_Rule>& arules, const std::vector<std::string>& v, const std::string aaxis="X", const D3DXFROMWINEVECTOR2 aX=D3DXFROMWINEVECTOR2(-1.0f, 1.0f), const D3DXFROMWINEVECTOR2 aY=D3DXFROMWINEVECTOR2(-1.0f, 1.0f), D3DXFROMWINEVECTOR2 vTC=D3DXFROMWINEVECTOR2(0.0f, 1.0f),  D3DXFROMWINEVECTOR3 vR=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3 vS=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f), D3DXFROMWINEVECTOR3 vT=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3 vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f), D3DXFROMWINEVECTOR3 vC=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f), float awidth=1.0f, float aheight=1.0f, float adepth=1.0f, float alength=1.0f, float aradius=1.0f, float abottomRadius=1.0f, float atopRadius=1.0f, float aradMajor=1.0f, float aradMinor=1.0f, float afA=1.0f, float afB=1.0f, float afC=1.0f)
{
	if(!v.size())
		return;
	//LOG_FNLN;
	//LOG_PRINT("aX.x=%f\n", aX.x);
	//LOG_PRINT("aX.y=%f\n", aX.y);
	//LOG_PRINT("aY.x=%f\n", aY.x);
	//LOG_PRINT("aY.y=%f\n", aY.y);

	//LOG_PRINT("v.size()=%d\n", v.size());
#if 0
	{unsigned int i=0;
	while(i<v.size())
	{
		//LOG_PRINT("v[%d]=%s\n", i, v[i++].c_str());
	}}
#endif
	D3DXFROMWINEMATRIX w;
	unsigned int i=0;
	while(i<v.size())
	{
		//LOG_FNLN;
		//LOG_PRINT("i=%d\n", i);
		if(v[i]=="I") //&& (v[i+1]=="(") && (v[i+3]==")"))
		{
			if(g_Terminal==eQuad_Terminal)
			{
				D3DXFROMWINEVECTOR2 asize(1.0f, 1.0f);
				D3DXFROMWINEMATRIX w=stx_GetPivotMatrix(vR, vT);
				{D3DXFROMWINEVECTOR2 vTC_(-1.0f, -1.0f);D3DXFROMWINEVECTOR3 vP_(-asize.x, -asize.y, 0.0f);DrawVertex(w, vC, vTC_, vP_);}
				{D3DXFROMWINEVECTOR2 vTC_(1.0f, -1.0f);D3DXFROMWINEVECTOR3 vP_( asize.x, -asize.y, 0.0f);DrawVertex(w, vC, vTC_, vP_);}
				{D3DXFROMWINEVECTOR2 vTC_(-1.0f, 1.0f);D3DXFROMWINEVECTOR3 vP_(-asize.x,  asize.y, 0.0f);DrawVertex(w, vC, vTC_, vP_);}
				{D3DXFROMWINEVECTOR2 vTC_(-1.0f, 1.0f);D3DXFROMWINEVECTOR3 vP_(-asize.x,  asize.y, 0.0f);DrawVertex(w, vC, vTC_, vP_);}
				{D3DXFROMWINEVECTOR2 vTC_(1.0f, 1.0f);D3DXFROMWINEVECTOR3 vP_( asize.x,  asize.y, 0.0f);DrawVertex(w, vC, vTC_, vP_);}
				{D3DXFROMWINEVECTOR2 vTC_(1.0f, -1.0f);D3DXFROMWINEVECTOR3 vP_( asize.x, -asize.y, 0.0f);DrawVertex(w, vC, vTC_, vP_);}
			}
			else
			{
				//LOG_FNLN;
				//LOG_PRINT("I(%s):vP.x=%f vP.y=%f vP.z=%f\n", v[i+2].c_str(), vP.x, vP.y, vP.z);
				std::string strn=v[i+2];
				#if 0
				if(strn=="castleI")
				{
					//LOG_PRINT("I(%s):\n", strn.c_str());
					//LOG_PRINT("vS.x=%f\nvS.y=%f\nvS.z=%f\n", vS.x, vS.y, vS.z);
				}
				#endif
				//LOG_PRINT("arules.size()=%d\n", arules.size());
				//LOG_PRINT("strn=%s\n", strn.c_str());
				std::vector<std::string> v_=stx_GetRuleVec(arules, strn);
		
				//LOG_PRINT("v_.size()=%d\n", v_.size());
				if(v_.size())
					Draw(arules, v_, aaxis, aX*vS.x, aY*vS.y, vTC, vR, vS, vT, vP, vC,awidth, aheight, adepth, alength, aradius, abottomRadius, atopRadius, aradMajor, aradMinor, afA, afB, afC);
			}
			i+=4;
		}
		else if(v[i]=="Push") // && (v[i+1]=="(") && (v[i+2]==")"))// ???
		{
			//LOG_FNLN;
			//LOG_PRINT("Push()\n");
		#if 0	
			Scopes_g.push(scope);
		#endif
			i+=3;
		}
		else if(v[i]=="Pop") // && (v[i+1]=="(") && (v[i+2]==")")) // ???
		{
			//LOG_FNLN;
			//LOG_PRINT("Pop()\n");
		#if 0	
			if(!Scopes_g.empty())
			{
				scope=Scopes_g.top();
				Scopes_g.pop();
			}
			else
				LOG_PRINT("Scopes stack empty.\n");
		#endif
			i+=3;
		}
		else if(v[i]=="Mesh")
		{
			std::vector<Vertex332> vertices;
			std::vector<__WORD__> indices;
			GetMeshVertices(v[i+2].c_str(), vertices, indices);
			
			D3DXFROMWINEVECTOR3 scale(1.0f, 1.0f, 1.0f);
			if(aaxis=="X")
				scale.x=aX.y-aX.x;
			else if(aaxis=="Y")
				scale.y=aY.y-aY.x;
			scale.z=(scale.x+scale.y)*0.5f;
			D3DXFROMWINEMATRIX S;
			D3DXFROMWINEMatrixScaling(&S, scale.x, scale.y, scale.z);

			//if(!g_bColor)texID=mesh3D.texID;
			if(indices.size())
			for(unsigned int j=0;j<indices.size();j++)
			{
				Vertex332 v=vertices[indices[j]];
				D3DXFROMWINEVECTOR3 vP_=v.position;

				D3DXFROMWINEVec3TransformCoord(&vP_, &vP_, &S);

				D3DXFROMWINEVECTOR3 vC_=v.color;
				D3DXFROMWINEVECTOR2 vTC_=v.texcoord;
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(w, vC_, vTC_, vP_);
			}
			else
			for(unsigned int j=0;j<vertices.size();j++)
			{
				Vertex332 v=vertices[j];
				D3DXFROMWINEVECTOR3 vP_=v.position;

				D3DXFROMWINEVec3TransformCoord(&vP_, &vP_, &S);
				D3DXFROMWINEVECTOR3 vC_=v.color;
				D3DXFROMWINEVECTOR2 vTC_=v.texcoord;
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(w, vC_, vTC_, vP_);
			}

			i+=4;
		}
		else if(v[i]=="Shape") // && (v[i+1]=="(") && (v[i+3]==")"))
		{
			
			D3DXFROMWINEVECTOR3 scale(1.0f, 1.0f, 1.0f);
			if(aaxis=="X")
			{
				scale.x=aX.y-aX.x;
				awidth=scale.x;
				adepth=awidth;
				aradius=awidth;
				abottomRadius=awidth;
				atopRadius=awidth;
				aradMajor=awidth;
				aradMinor=awidth;
				float fA=awidth;
				float fB=awidth;
				float fC=awidth;
			}
			else if(aaxis=="Y")
			{
				scale.y=aY.y-aY.x;
				aheight=scale.y;
				alength=aheight;
			}
			scale.z=(scale.x+scale.y)*0.5f;
			D3DXFROMWINEMATRIX S;
			D3DXFROMWINEMatrixScaling(&S, scale.x, scale.y, scale.z);

			MeshRenderer2 mesh3D;
			tShader aShader=eShaderNone;
			int step=20;
			int stepLng=20;
			int stepLat=20;
			int stepMajor=20;
			int stepMinor=20;

			if(v[i+2]=="TexturedQuad")
				mesh3D.CreateTexturedQuad(awidth, aheight, aShader);
			else if(v[i+2]=="Box")
				mesh3D.CreateBox(awidth, aheight, adepth, aShader);
			else if(v[i+2]=="TexturedBox")
				mesh3D.CreateTexturedBox(awidth, aheight, adepth, aShader);
			else if(v[i+2]=="Cylinder")
				mesh3D.CreateCylinder(abottomRadius, atopRadius, alength, aShader);
			else if(v[i+2]=="Sphere")
       				mesh3D.CreateSphere(aradius, aShader);
			else if(v[i+2]=="Ellipsoid")
				mesh3D.CreateEllipsoid(afA, afB, afC, aShader);
			else if(v[i+2]=="Torus")
				mesh3D.CreateTorus(aradMajor, aradMinor, aShader);
			else if(v[i+2]=="Tire")
				mesh3D.CreateTire(aradMajor, aradMinor, aShader);
				
			else if(v[i+2]=="Cone")
				mesh3D.CreateCone(aradius, aheight, aShader);
#if 1
			else if(v[i+2]=="Hemis")
				mesh3D.CreateHemis(aShader);
#endif
			else if(v[i+2]=="Octa")
				mesh3D.CreateOcta(aShader);
			else if(v[i+2]=="Tetra")
				mesh3D.CreateTetra(aShader);
			else if(v[i+2]=="HalfSphere")
				mesh3D.CreateHalfSphere(aradius, aShader);
		  //else if(v[i+2]=="Grid,
			else if(v[i+2]=="Icosahedron")
				mesh3D.CreateIcosahedron(aShader);
			else if(v[i+2]=="Dodecahedron")
				mesh3D.CreateDodecahedron(aShader);
			else if(v[i+2]=="Prism")
				mesh3D.CreatePrism(aShader);
          //else if(v[i+2]=="Hexahedron (cube)
		  //else if(v[i+2]=="Tetrahedron
		  //else if(v[i+2]=="Octahedron
		  //else if(v[i+2]=="Dodecahedron

			//if(!g_bColor)texID=mesh3D.texID;
			if(mesh3D.indices.size())
			for(unsigned int j=0;j<mesh3D.indices.size();j++)
			{
				stx_VertexPositionNormalTexture v=mesh3D.vertices[mesh3D.indices[j]];
				D3DXFROMWINEVECTOR3 vP_=v.Position;

				D3DXFROMWINEVec3TransformCoord(&vP_, &vP_, &S);
				D3DXFROMWINEVECTOR3 vC_=v.Color;
				D3DXFROMWINEVECTOR2 vTC_=v.Tex;
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(w, vC_, vTC_, vP_);
			}
			else
			for(unsigned int j=0;j<mesh3D.vertices.size();j++)
			{
				stx_VertexPositionNormalTexture v=mesh3D.vertices[j];
				D3DXFROMWINEVECTOR3 vP_=v.Position;

				D3DXFROMWINEVec3TransformCoord(&vP_, &vP_, &S);
				D3DXFROMWINEVECTOR3 vC_=v.Color;
				D3DXFROMWINEVECTOR2 vTC_=v.Tex;
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(w, vC_, vTC_, vP_);
			}
			i+=4;
		}
		else if(v[i]=="Align")
		{	
			//LOG_FNLN;
			//LOG_PRINT("Align\n");
			std::string ax="X";
			f=atof(v[i+3].c_str())/100.0f;
			if(v[i+2]=="SIDE_FACE_X")
			{
				vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(0.0f, 0.0f, f*1.0f);
				ax="X";
			}
			else if(v[i+2]=="SIDE_FACE_Y")
			{
				vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(f*1.0f, 0.0f, 0.0f);
				ax="Y";
			}
			else if(v[i+2]=="SIDE_FACE_Z")
			{
				vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(0.0f, f*1.0f, 0.0f);
				ax="Z";
			}
			else if(v[i+2]=="SIDE_FACE_NX")
			{
				vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f*f);
				ax="X";
			}
			else if(v[i+2]=="SIDE_FACE_NY")
			{
				vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(-1.0f*f, 0.0f, 0.0f);
				ax="Y";
			}
			else if(v[i+2]=="SIDE_FACE_NZ")
			{
				vR=D3DXFROMWINEVECTOR3(0.0f, -1*90.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(0.0f, -1.0f*f, 0.0f);
				ax="Z";
			}
				//LOG_PRINT("Align:v[%d]=%s\n", i+3, v[i+3].c_str());
			std::string n=v[i+4];
			std::vector<std::string> v_=stx_GetRuleVec(arules, n);
			if(v_.size())
				Draw(arules, v_, ax, aX*vS.x, aY*vS.y, vTC, vR, vS, vT, vP, vC,awidth, aheight, adepth, alength, aradius, abottomRadius, atopRadius, aradMajor, aradMinor, afA, afB, afC);
			i+=5;
		}
		else if(v[i]=="Repeat")
		{
			// Repeat(X,2,wall_window_wall)
			//LOG_FNLN;
			//LOG_PRINT("Repeat\n");
			D3DXFROMWINEVECTOR2 X=aX;
			D3DXFROMWINEVECTOR2 Y=aY;
			unsigned int dimension=atoi(v[i+3].c_str());
			for(unsigned int j=0;j<dimension;j++)
			{
				//LOG_PRINT("Repeat:%d\n", j);
				if(v[i+2]=="X")
					X.y=X.x+(aX.y-aX.x)/dimension;
				else if(v[i+2]=="Y")
					Y.y=Y.x+(aY.y-aY.x)/dimension;
					//LOG_FNLN;
				//LOG_PRINT("Repeat:v[%d]=%s\n", i+4, v[i+4].c_str());

				std::string n=v[i+4];
				
				std::vector<std::string> v_=stx_GetRuleVec(arules, n);
				if(v_.size())
					Draw(arules, v_, v[i+2], X*vS.x, Y*vS.y, vTC, vR, vS, vT, vP, vC,awidth, aheight, adepth, alength, aradius, abottomRadius, atopRadius, aradMajor, aradMinor, afA, afB, afC);
				
				if(v[i+2]=="X")
					X.x=X.y;
				else if(v[i+2]=="Y")
					Y.x=Y.y;
			}
			i+=6;
		}
		else if(v[i]=="Split")
		{ // 	Split(X,[30%:40%:30%],[Quad:vertical_door_wall:Quad])
			//LOG_FNLN;
			//LOG_PRINT("Split\n");
			std::vector<std::string> symbols;
			std::vector<float> percents;
			unsigned int k=i+4;
			while(v[k]!="]")
			{
				//LOG_PRINT("Split percent%d %s\n", k, v[k].c_str());
				percents.push_back(atof(v[k++].c_str()));
			}
			//LOG_PRINT("percents.size=%d\n", percents.size());
			k+=2;
			while(v[k]!="]")
			{
				//LOG_PRINT("Split symbol%d %s\n", k, v[k].c_str());
				symbols.push_back(v[k++]);
				//LOG_PRINT("symbols[%d]=%s\n", k, symbols[k].c_str());
			}
			//LOG_PRINT("symbols.size=%d\n", symbols.size());
			#if 0
			D3DXFROMWINEVECTOR3 vR_=vR;
			D3DXFROMWINEVECTOR3 vT_=vT;
			if(v[i+2]=="X")
			{
				vR_=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
				vT_=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f*f);
			}
			else if(v[i+2]=="Y")
			{
				vR_=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
				vT_=D3DXFROMWINEVECTOR3(1.0f*f, 0.0f, 0.0f);
			}
			else if(v[i+2]=="Z")
			{
				vR_=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
				vT_=D3DXFROMWINEVECTOR3(0.0f, 1.0f*f, 0.0f);
			}
			#endif
			D3DXFROMWINEVECTOR2 X=aX;
			D3DXFROMWINEVECTOR2 Y=aY;
			for(unsigned int j=0;j<symbols.size();j++)
			{
				//LOG_FNLN;			
				//LOG_PRINT("Split[%d]:symbols[%d]=%s\n", j, j, symbols[j].c_str());
				// Split(Y,[40%:40%:20%],[Wall:Window:Wall])
				
				if(v[i+2]=="X")
					X.y=X.x+(aX.y-aX.x)*0.5f*percents[j]/50.0f;
				else if(v[i+2]=="Y")
					Y.y=Y.x+(aY.y-aY.x)*0.5f*percents[j]/50.0f;
				
				std::vector<std::string> v_=stx_GetRuleVec(arules, symbols[j]);
				if((v_.size())&&(symbols[j]!="Empty"))
					Draw(arules, v_, v[i+2], X*vS.x, Y*vS.y, vTC, vR, vS, vT, vP, vC,awidth, aheight, adepth, alength, aradius, abottomRadius, atopRadius, aradMajor, aradMinor, afA, afB, afC);
				
				if(v[i+2]=="X")
					X.x=X.y;
				else if(v[i+2]=="Y")
					Y.x=Y.y;
			}
			i+=8+symbols.size()+percents.size();
		}
		else if(v[i]=="R")
		{
			//LOG_FNLN;
			//LOG_PRINT("R\n");
			vR=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
			i+=6;
		}
		else if(v[i]=="S")
		{
			//LOG_FNLN;
			//LOG_PRINT("S\n");
			vS=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
			i+=6;
		}
		else if(v[i]=="T")
		{
			//LOG_FNLN;
			//LOG_PRINT("T\n");
			vT=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
			i+=6;
		}
		else if(v[i]=="SizeX")
		{
			vS.x=atof(v[i+2].c_str());
			i+=4;
		}
		else if(v[i]=="SizeY")
		{
			vS.y=atof(v[i+2].c_str());
			i+=4;
		}
		else if(v[i]=="SizeZ")
		{
			vS.z=atof(v[i+2].c_str());
			i+=4;
		}
		else if(v[i]=="Enable")
		{
			if(0)//v[i+2]=="Color")
			{
				m_bUseColor=true;
				initVectors(m_sfn, m_sfiletosave);
			}
			i+=4;
		}
		else if(v[i]=="Disable")
		{
			if(0)//v[i+2]=="Color")
			{
				m_bUseColor=false;
				initVectors(m_sfn, m_sfiletosave);
			}
			i+=4;
		}
		else if(v[i]=="Color")
		{
			vC=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
			//LOG_FNLN;
			//LOG_PRINT("Color:\nvC.x=%f\nvC.y=%f\nvC.z=%f\n", vC.x, vC.y, vC.z);
			i+=6;
		}
		else if(v[i]=="TexCoord")
		{
			//LOG_FNLN;
			//LOG_PRINT("TexCoord\n");
			vTC=D3DXFROMWINEVECTOR2(atof(v[i+2].c_str()), atof(v[i+3].c_str()));
			i+=5;
		}
		else if(v[i]=="Texture")
		{
			if(!m_bUseColor){
			//LOG_FNLN;
			//LOG_PRINT("Texture(%d)\n", texID);
			texID=atoi(v[i+2].c_str());
			}
			else
				texID=-1;
			i+=4;
		}
		else if(v[i]=="Vertex")
		{	
			if(g_Terminal==eVertex_Terminal)
			{
				//LOG_FNLN;
				//LOG_PRINT("Vertex\n");
				D3DXFROMWINEVECTOR3 vP_=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
				float epsilon=0.000005f;
				#if 1
				if(vP_.x<(-1.0f+epsilon))
				{
					//LOG_PRINT("vP_.x=%f\n", vP_.x);
					//LOG_PRINT("aX.x=%f\n", aX.x);
					vP_.x=aX.x;
				}
				else if(vP_.x>(1.0f-epsilon))
				{
					//LOG_PRINT("vP_.x=%f\n", vP_.x);
					//LOG_PRINT("aX.y=%f\n", aX.y);
					vP_.x=aX.y;
				}
				if(vP_.y<(-1.0f+epsilon))
				{
					//LOG_PRINT("vP_.y=%f\n", vP_.y);
					//LOG_PRINT("aY.x=%f\n", aY.x);
					vP_.y=aY.x;
				}
				else if(vP_.y>(1.0f-epsilon))
				{
					//LOG_PRINT("vP_.y=%f\n", vP_.y);
					//LOG_PRINT("aY.y=%f\n", aY.y);
					vP_.y=aY.y;
				}
				#else
					float distance=1.0f;
        				vP_.x += distance * cosf(aX.x);
        				vP_.z += distance * sinf(aX.x);
				#endif
				//LOG_FNLN;
				//LOG_PRINT("Vertex:\nvP_.x=%f\nvP_.y=%f\nvP_.z=%f\n", vP_.x, vP_.y, vP_.z);
				w=stx_GetPivotMatrix(vR, vT);

				D3DXFROMWINEVECTOR2 vTC_=vTC;
				#ifdef __ANISO__
				vTC_.x=vP_.x;
				vTC_.y=vP_.y;
				#endif		
				DrawVertex(w, vC, vTC_, vP_);
			}
			i+=6;
		}
		else
		{
			//LOG_FNLN;
			//LOG_PRINT("else\n");
			i++;
		}
	}
}

std::string XXX(LuaScript& script, std::string& astr)
{
	std::vector<std::string> v=script.getTableKeys("symbolsF");
	for(unsigned int i=0;i<v.size();i++)
	{
		std::string rulename = v[i];
		char buf[64];
		stx_snprintf(buf, 64, "symbolsF.%s", rulename.c_str());
		std::string rulebody = script.get<std::string>(buf);
		stx_ReplaceAll(astr, rulename, rulebody);
	}
	return astr;
}

std::string XXXFloats(std::string& astr)
{
			//LOG_PRINT("2BodyElement=%x\n", BodyNode);
	TiXmlHandle FloatsHandle = BodyNode->FirstChild("Floats");
	TiXmlHandle FloatHandle = FloatsHandle.FirstChild("Float");
	TiXmlElement* FloatElement = FloatHandle.Element();
	int i=1;
	while ( FloatElement )
	{
		std::string n=FloatElement->Attribute("Name");
		std::string b=FloatElement->Attribute("Value");
		stx_ReplaceAll(astr, n, b);
		FloatHandle = FloatsHandle.Child( "Float", i++ );
		FloatElement = FloatHandle.Element();
	}
	return astr;
}

int initVectors(std::string afn, std::string afiletosave)
{
	g_vertices_.clear();
	g_verticesC_.clear();
	g_vertices3.clear();
	g_normals3.clear();
	g_texcoords2.clear();
	g_indices1.clear();
	g_indices2.clear();

	#if 0
	m_bUseColor=true;
	renderCube();
	#else
	std::string axiom="	Texture(0) \
				R(  0,  0,0)S(1,1,1)T( 0, 0, 1)I(Quad) \
				R(  0,  0,0)S(1,1,1)T( 0, 0,-1)I(Quad) \
				R( 90,  0,0)S(1,1,1)T( 1, 0, 0)I(Quad) \
				R(-90,  0,0)S(1,1,1)T(-1, 0, 0)I(Quad) \
				R(  0, 90,0)S(1,1,1)T( 0, 1, 0)I(Quad) \
				R(  0,-90,0)S(1,1,1)T( 0,-1, 0)I(Quad)";
	std::vector<stx_Rule> rules;
if(m_bUseLua){
	LuaScript script;
	bool r=script.Load(afn.c_str());
	if(!r)
		axiom = script.get<std::string>("axiom");

	std::vector<std::string> v=script.getTableKeys("symbols");
	for(unsigned int i=0;i<v.size();i++)
	{
		std::string rulename = v[i];
		//LOG_PRINT("symbols[%d].name=%s\n", i, rulename.c_str());

		char buf[64];
		stx_snprintf(buf, 64, "symbols.%s", rulename.c_str());
		std::string rulebody = script.get<std::string>(buf);
		rulebody = XXX(script, rulebody);
		//LOG_PRINT("symbols[%d].value=%s\n", i, rulebody.c_str());

		stx_Rule m(rulename, rulebody);
		rules.push_back(m);
	}
	}else{
			LOG_PRINT("3BodyElement=%x\n", BodyNode);
	TiXmlElement* AxiomElement = BodyNode->FirstChild("Axiom")->ToElement();
	axiom=AxiomElement->GetText();
	if(!stx_strlen(axiom.c_str()))
		axiom=AxiomElement->Attribute("I");

	TiXmlHandle RulesHandle = BodyNode->FirstChild("Rules");
	
	std::string texture=AxiomElement->Attribute("Texture");
	if(texture!="True")
		m_bUseColor=true;
	else
		m_bUseColor=false;
    g_Rx=0.0f;
    g_Ry=0.0f;
	if(AxiomElement)
	{
        std::string fs=AxiomElement->Attribute("Rx");
        g_Rx=atof(fs.c_str());
        fs=AxiomElement->Attribute("Ry");
        g_Ry=atof(fs.c_str());
	}
	int i=1;
	TiXmlHandle RuleHandle = RulesHandle.FirstChild("Rule");
	TiXmlElement* RuleElement = RuleHandle.Element();
	while ( RuleElement )
	{
		std::string n=RuleElement->Attribute("Name");
		std::string b=RuleElement->Attribute("Body");
		b=XXXFloats(b);
		stx_Rule m(n, b);
		rules.push_back(m);
		RuleHandle = RulesHandle.Child( "Rule", i++ );
		RuleElement = RuleHandle.Element();
	}
	}

	//LOG_PRINT("axiom=%s\n", axiom.c_str());
	//LOG_PRINT("rules.size()=%d\n", rules.size());

	if((axiom[0]=='I') && (axiom[1]=='(') && (axiom[axiom.length()-1]==')'))
	{
		axiom=axiom.substr(2,axiom.length()-3);
		std::vector<std::string> v_;
		v_.push_back("I");
		v_.push_back("(");
		v_.push_back(axiom);
		v_.push_back(")");
		Draw(rules, v_);
	}
	#endif
	//LOG_PRINT("textures.size()=%d\n", textures.size());
	//LOG_PRINT("vt.size()=%d\n", vt.size());

	LOG_PRINT("g_vertices_.size() =%d\n", g_vertices_.size());
	LOG_PRINT("g_verticesC_.size() =%d\n", g_verticesC_.size());
	LOG_PRINT("g_vertices3.size() =%d\n", g_vertices3.size());
	LOG_PRINT("g_normals3.size()  =%d\n", g_normals3.size());
	LOG_PRINT("g_texcoords2.size()=%d\n", g_texcoords2.size());
	LOG_PRINT("g_indices1.size()  =%d\n", g_indices1.size());
	LOG_PRINT("g_indices2.size()  =%d\n", g_indices2.size());
	if(!m_bUseColor)
	{
	#if 1
	stx_Materials_(vt, g_indices2, mesh__);
	stx_writeXFile(afiletosave.c_str(), mesh__);//, g_vertices3, g_normals3, g_texcoords2, g_indices1, texture);
	#endif
	
	#if 0
	FW3::GetInstance()->GetMesh( mesh, afiletosave.c_str(), defaultshader);
	#else
	mesh.Load(afiletosave.c_str(), defaultshader, true, true);
	#endif
	}
	return 0;
}
int initTextures(std::string afn, std::string afiletosave)
{
#ifndef __ANISO__
	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
#else
	SamplerStateID ss=IRenderer::GetRendererInstance()->GetlinearAniso();
#endif
if(m_bUseLua){
	LuaScript script;
	bool r=script.Load(afn.c_str());

	std::string texturefilenames_ = script.get<std::string>("textures");
	std::vector<std::string> vtex=stx_split(texturefilenames_, ':');
	D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
	for(unsigned int i=0;i<vtex.size();i++)
	{
		LOG_PRINT("vtex[%d]:%s\n", i, vtex[i].c_str());
		std::string str=stx_trim(vtex[i]);
		stx_ReplaceAll(str, "&#32;", " ");
			//LOG_PRINT("str=%s\n", str.c_str());
		textures.push_back(IRenderer::GetRendererInstance()->addImageLibTexture(str.c_str(), false, ss));
		stx_Material m(str.c_str()+1, vC);
		vt.push_back(m);
	}
}else{
			LOG_PRINT("4BodyElement=%x\n", BodyNode);
		std::string b;
		if(BodyNode)
			b = BodyNode->FirstChild("Textures")->ToElement()->GetText();
		std::vector<std::string> v=stx_split(b, ':');
		D3DXFROMWINEVECTOR3 vC(0.0f, 1.0f, 0.0f);
		for(unsigned int j=0;j<v.size();j++)
		{
			//LOG_PRINT("j=%d\n", j);
			std::string str=stx_trim(v[j]);
			stx_ReplaceAll(str, "&#32;", " ");
			LOG_PRINT("str[%d]=%s\n", j, str.c_str());
			textures.push_back(IRenderer::GetRendererInstance()->addImageLibTexture(str.c_str(), false, ss));
			stx_Material m(str.c_str()+1, vC);
			vt.push_back(m);
		}
}
	if(!m_bUseColor && textures.size()){
	texID=textures[0];
	}else{
	texID=-1;//IRenderer::GetRendererInstance()->addImageLibTexture("/Orcs/Black.png", false, ss);
	}
	return 0;
}

int init_(std::string afn, std::string afiletosave)
{
	std::string fn=stx_convertpath(afn);
    TiXmlDocument doc(fn.c_str());
    if (!doc.LoadFile())
	{LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
    TiXmlHandle docHandle(&doc);
    BodyNode = docHandle.FirstChild("Body").Node();

	m_sfn=afn;
	m_sfiletosave=afiletosave;

	shdC = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD3N.hlsl", "main", "main");
	FormatDesc format[] =
	{
		0, TYPE_VERTEX,     FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD,   FORMAT_FLOAT, 4,
		0, TYPE_TEXCOORD,   FORMAT_FLOAT, 2
	};
	vfC = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shdC);
	initTextures(afn, afiletosave);
	initVectors(afn, afiletosave);
	return 0;
}

void render()
{
	D3DXFROMWINEMATRIX W, R, S;
	static float mAngleX=g_Rx;
	static float mAngleY=g_Ry;
	stx_GetAngles(mAngleX, mAngleY);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	if(m_bUseColor)
		D3DXFROMWINEMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
	else
		D3DXFROMWINEMatrixScaling(&S, 1.0f, 1.0f, 1.0f);
	W=R*S;
	if(!m_bUseColor)
		mesh.render(&W);
#ifndef _MSC_VER
	else
	{	
		IRenderer::GetRendererInstance()->setShader(shdC);
        	IRenderer::GetRendererInstance()->setVertexFormat(vfC);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		IRenderer::GetRendererInstance()->setTexture("Sampler", texID);
		IRenderer::GetRendererInstance()->setShaderConstant1i("iSampler", 0);
        	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, g_verticesC_.size(), g_indices.size()/3, &g_indices[0], &g_indices[0], CONSTANT_INDEX2, &g_verticesC_[0], &g_verticesC_[0], sizeof(g_verticesC_[0]));
	}
#endif
	#if 1
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
		char buf[64];
		stx_snprintf(buf, 64, "mAngleX=%f\n", mAngleX);
		IRenderer::GetRendererInstance()->drawText(buf, 10, 50,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
		stx_snprintf(buf, 64, "mAngleY=%f\n", mAngleY);
		IRenderer::GetRendererInstance()->drawText(buf, 10, 70,
			15, 18,
			IRenderer::GetRendererInstance()->GetdefaultFont(),
			IRenderer::GetRendererInstance()->GetlinearClamp(),
			IRenderer::GetRendererInstance()->GetblendSrcAlpha(),
			IRenderer::GetRendererInstance()->GetnoDepthTest());
	#endif
}
#if 0
void renderCube(
const D3DXFROMWINEVECTOR3 aPosition=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f),
const D3DXFROMWINEVECTOR2 aSize=D3DXFROMWINEVECTOR2(1.0f, 1.0f),
const D3DXFROMWINEVECTOR3 aAngles=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f))
{
	D3DXFROMWINEMATRIX  w;
	D3DXFROMWINEVECTOR3 vR, vP;

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f)+aAngles;
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, -1.0f)+aPosition;

	g_color3_=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	g_color4_=D3DXFROMWINEVECTOR4(g_color3_.x, g_color3_.y, g_color3_.z, 1.0f);
	vt.push_back(stx_Material("test.bmp", g_color3_));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, aSize);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f)+aAngles;
	vP=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f)+aPosition;
	
	g_color3_=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	g_color4_=D3DXFROMWINEVECTOR4(g_color3_.x, g_color3_.y, g_color3_.z, 1.0f);
	vt.push_back(stx_Material("test.bmp", g_color3_));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, aSize);

	vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f)+aAngles;
	vP=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f)+aPosition;
	
	g_color3_=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f);
	g_color4_=D3DXFROMWINEVECTOR4(g_color3_.x, g_color3_.y, g_color3_.z, 1.0f);
	vt.push_back(stx_Material("test.bmp", g_color3_));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, aSize);

	vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f)+aAngles;
	vP=D3DXFROMWINEVECTOR3(-1.0f, 0.0f, 0.0f)+aPosition;
	
	g_color3_=D3DXFROMWINEVECTOR3(1.0f, 0.0f, 0.0f);
	g_color4_=D3DXFROMWINEVECTOR4(g_color3_.x, g_color3_.y, g_color3_.z, 1.0f);
	vt.push_back(stx_Material("test.bmp", g_color3_));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, aSize);

	vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f)+aAngles;
	vP=D3DXFROMWINEVECTOR3(0.0f, 1.0f, 0.0f)+aPosition;
	
	g_color3_=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	g_color4_=D3DXFROMWINEVECTOR4(g_color3_.x, g_color3_.y, g_color3_.z, 1.0f);
	vt.push_back(stx_Material("test.bmp", g_color3_));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, aSize);

	vR=D3DXFROMWINEVECTOR3(0.0f, -1*90.0f, 0.0f)+aAngles;
	vP=D3DXFROMWINEVECTOR3(0.0f, -1.0f, 0.0f)+aPosition;
	
	g_color3_=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0f);
	g_color4_=D3DXFROMWINEVECTOR4(g_color3_.x, g_color3_.y, g_color3_.z, 1.0f);
	vt.push_back(stx_Material("test.bmp", g_color3_));
	w=stx_GetPivotMatrix(vR, vP);
	DrawQuad(w, aSize);
}
#endif
};
/*
Listing 3.5: Implementation of Repeat
Repeat ( axis , dimension , symbol , random )
n = dscope[axis]/dimensione
i f random
tmp = ( rnd(1, random), ... , rnd(1, random) )
|
{z
100
unit = P n−1
i=0
else
}
n
tmp i
ratios = ( tmp 0 ∗ unit %, ... , tmp n−1 ∗ unit % )
ratios = (
100
100
%, ... ,
% )
| n
{z n }
n
r e t u r n S p l i t ( axis , ratios , ( symbol, ... , symbol ) )
*/
ProceduralBuilding5 app;
void proceduralBuilding5_init(std::string afn, std::string afiletosave, bool abUseLua)
{
	app.init(afn, afiletosave, abUseLua);
}
void proceduralBuilding5_render()
{
	app.render();
}
/*
Scope Scope is a term describing space properties of a shape. It is defined
as a bounding volume consisting of:
• (local) position
• (local) orientation
• scale
• parent scope if any
*/

