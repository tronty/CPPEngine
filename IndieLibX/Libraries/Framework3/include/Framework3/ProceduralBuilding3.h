#ifndef __ProceduralBuilding3___
#define __ProceduralBuilding3___
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
std::vector<stx_Mesh> mesh_;

enum tTerminal
{
	eQuad_Terminal,
	eVertex_Terminal,
};

struct ProceduralBuilding3
{
	ShaderID shd;
	VertexFormatID vf;
	ShaderID shdC;
	VertexFormatID vfC;
struct IndexedMesh
{
	std::string m_sInstanceName;
	std::vector<D3DXFROMWINEMATRIX> m_mW;
	std::vector<D3DXFROMWINEVECTOR3> m_vertices3;
	std::vector<TextureID> m_texID;
	std::vector<D3DXFROMWINEVECTOR3> m_normals3;
	std::vector<D3DXFROMWINEVECTOR2> m_texcoords2;
	std::vector<__WORD__> m_indices1;
	std::vector<__WORD__> m_indices2;		
	D3DXFROMWINEVECTOR3 _max;
	D3DXFROMWINEVECTOR3 _min;
	__WORD__ m_dStart;
	__WORD__ m_dStop;
	float Hits(const D3DXFROMWINEVECTOR3& start, const D3DXFROMWINEVECTOR3& direction, D3DXFROMWINEVECTOR3& p)
	{
		float d=1e10f;
		float t=1e10f;
		for(unsigned int i=0;i<m_indices1.size()/3;i++)
		{
			D3DXFROMWINEVECTOR3 v0=m_vertices3[3*i+0];
			D3DXFROMWINEVECTOR3 v1=m_vertices3[3*i+1];
			D3DXFROMWINEVECTOR3 v2=m_vertices3[3*i+2];
			bool r=stx_RayIntersectsTriangle(start, direction, v0, v1, v2, t);
			if(t<d)
				d=t;
		}
		return d;
	}
	void maxmin(	D3DXFROMWINEVECTOR3& amin,
			D3DXFROMWINEVECTOR3& amax,
			D3DXFROMWINEVECTOR3& ac)
	{					
		D3DXFROMWINEVECTOR3 max_=D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
		D3DXFROMWINEVECTOR3 min_=D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
		//for(unsigned int i=m_dStart;i<m_dStop;i++)
		for(unsigned int i=0;i<m_vertices3.size();i++)
		{
			if(m_vertices3[i].x>max_.x)
				max_.x=m_vertices3[i].x;
			if(m_vertices3[i].y>max_.y)
				max_.y=m_vertices3[i].y;
			if(m_vertices3[i].z>max_.z)
				max_.z=m_vertices3[i].z;
			if(m_vertices3[i].x<min_.x)
				min_.x=m_vertices3[i].x;
			if(m_vertices3[i].y<min_.y)
				min_.y=m_vertices3[i].y;
			if(m_vertices3[i].z<min_.z)
				min_.z=m_vertices3[i].z;
		}
		amin=min_;
		amax=max_;
		//_min=min_;
		//_max=max_;
		ac=0.5f*(amax-amin);
	}
	IndexedMesh()
	{
		m_sInstanceName="IndexedMesh";
		m_dStart=0;
		m_dStop=0;			
		_max=D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
		_min=D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
	}
	IndexedMesh(const IndexedMesh& rhs)
	{
		m_dStart=rhs.m_dStart;
		m_dStop=rhs.m_dStop;
		m_sInstanceName=rhs.m_sInstanceName;
		m_vertices3=rhs.m_vertices3;
		m_mW=rhs.m_mW;
		m_normals3=rhs.m_normals3;
		m_texcoords2=rhs.m_texcoords2;
		m_indices1=rhs.m_indices1;
		m_indices2=rhs.m_indices2;	
		_max=rhs._max;
		_min=rhs._min;
		m_texID=rhs.m_texID;
	}
	virtual ~IndexedMesh()
	{
		m_sInstanceName="";
		m_vertices3.clear();
		m_normals3.clear();
		m_texcoords2.clear();
		m_indices1.clear();
		m_indices2.clear();
	}
};
	TiXmlNode* BodyNode;

	bool m_b2D;

	std::vector<std::string> axioms;
	unsigned int axiomN;

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

	D3DXFROMWINEVECTOR3 g_color3_;
	D3DXFROMWINEVECTOR4 g_color4_;
	__WORD__ m_dStart;
	__WORD__ m_dStop;
	IndexedMesh m_IndexedMesh;
	//IndexedMesh m_IndexedMesh2;
	std::vector<IndexedMesh> m_Meshes;
	std::map<std::string, D3DXFROMWINEMATRIX> m_Transforms;
	std::vector<D3DXFROMWINEVECTOR3> m_vR;
	std::vector<D3DXFROMWINEVECTOR3> m_vS;
	std::vector<D3DXFROMWINEVECTOR3> m_vT;
	std::vector<__WORD__> m_vStart;
	D3DXFROMWINEVECTOR3 vR, vS, vT;
	D3DXFROMWINEMATRIX I;
	ProceduralBuilding3(){}
	void init(std::string afn="", std::string afiletosave="", bool abUseLua=true)
	{
		stx_Mesh m;
		mesh_.push_back(m);
		m_b2D=false;

		m_bUseColor=false;
		m_bUseLua=abUseLua;

		//g_Terminal=eQuad_Terminal;
		g_Terminal=eVertex_Terminal;
		vR=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		vS=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);
		vT=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXFROMWINEMatrixIdentity(&I);

		texID=-1;

		f=1.0f;

		g_color3_=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);

		g_color4_=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

		if(!m_bUseLua)
		{
			std::string fn=stx_convertpath(afn.c_str());
			//LOG_PRINT("fn.c_str()=%s\n", fn.c_str());
		}

		init_(afn, afiletosave);
LOG_FNLN;

	LOG_PRINT("g_vertices_.size() =%d\n", g_vertices_.size());
	LOG_PRINT("g_verticesC_.size() =%d\n", g_verticesC_.size());
	LOG_PRINT("m_IndexedMesh.m_vertices3.size() =%d\n", m_IndexedMesh.m_vertices3.size());
	LOG_PRINT("m_IndexedMesh.m_texcoords2.size()=%d\n", m_IndexedMesh.m_texcoords2.size());
	LOG_PRINT("m_IndexedMesh.m_indices1.size()  =%d\n", m_IndexedMesh.m_indices1.size());

		if((m_IndexedMesh.m_vertices3.size()==0)&&(g_verticesC_.size()==0))
			stx_exit(0);
		{
		shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD1.HLSL.shd", "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,     FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD,   FORMAT_FLOAT, 2
		};
		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
		}{
		shdC = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/texSHD3N.HLSL.shd", "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,     FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD,   FORMAT_FLOAT, 4,
			0, TYPE_TEXCOORD,   FORMAT_FLOAT, 2
		};
		vfC = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shdC);
		}
	}
	virtual ~ProceduralBuilding3()
	{
		//Textures.clear();
		vt.clear();

		g_vertices_.clear();
		g_verticesC_.clear();

		m_IndexedMesh.m_vertices3.clear();
		m_IndexedMesh.m_normals3.clear();
		m_IndexedMesh.m_texcoords2.clear();
		m_IndexedMesh.m_indices1.clear();
		m_IndexedMesh.m_indices2.clear();
	}


void DrawVertex(IndexedMesh* aIndexedMesh, const D3DXFROMWINEMATRIX& aW, const D3DXFROMWINEVECTOR3& aC, const D3DXFROMWINEVECTOR2& aTC, const D3DXFROMWINEVECTOR3& aV_)
{
	//LOG_FNLN;
	//if(texID==-1)return;
	aIndexedMesh->m_mW.push_back(aW);
	aIndexedMesh->m_vertices3.push_back(aV_);
	aIndexedMesh->m_texID.push_back(texID);
	D3DXFROMWINEVECTOR3 N;
	D3DXFROMWINEVec3Normalize(&N, &aV_);
	aIndexedMesh->m_normals3.push_back(N);
	aIndexedMesh->m_texcoords2.push_back(aTC);
	aIndexedMesh->m_indices1.push_back(aIndexedMesh->m_vertices3.size()-1);
	if(0==((aIndexedMesh->m_indices1.size()%3)))
	{
		if(texID>-1)
		{
			unsigned int  pos = find(textures.begin(), textures.end(), texID) - textures.begin();
			//LOG_FNLN;
			//LOG_PRINT("aIndexedMesh->m_indices2.push_back(%d)\n", pos);
			aIndexedMesh->m_indices2.push_back(pos);
		}
		else
			aIndexedMesh->m_indices2.push_back(0);
	}

	stx_VertexPositionNormalTexture v_;
	v_.Position=aV_;
	v_.Normal=N;
	v_.Tex=aTC;
	mesh_[0].vertices.push_back(v_);
	mesh_[0].indices.push_back(mesh_[0].vertices.size()-1);

	if(m_bUseColor){
	VertexC vx(aV_, g_color4_, aTC);
	g_verticesC_.push_back(vx);g_indices.push_back(g_verticesC_.size()-1);
	}else{
	Vertex vx(aV_, aTC);
	g_vertices_.push_back(vx);g_indices.push_back(g_vertices_.size()-1);
	}
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

#if 0
void Draw_(	IndexedMesh& m, const std::vector<stx_Rule>& arules, 
		const std::string astrn, 
		unsigned int& iStart, unsigned int& iStop,
		const std::string aaxis="X", 
		const D3DXFROMWINEVECTOR2 aX=D3DXFROMWINEVECTOR2(-1.0f, 1.0f), 
		const D3DXFROMWINEVECTOR2 aY=D3DXFROMWINEVECTOR2(-1.0f, 1.0f))
{
	iStart=m_IndexedMesh.m_vertices3.size();
	Draw(arules, astrn, aaxis, aX, aY);
	iStop=m_IndexedMesh.m_vertices3.size();
	//m.m_dStart=iStart;
	//m.m_dStop=iStop;
	for(unsigned int i=iStart;i<iStop;i++)
	{
		m.m_vertices3.push_back(m_IndexedMesh.m_vertices3[i]);
		m.m_mW.push_back(I);
		m.m_texID.push_back(m_IndexedMesh.m_texID[i]);
		m.m_normals3.push_back(m_IndexedMesh.m_normals3[i]);
		m.m_texcoords2.push_back(m_IndexedMesh.m_texcoords2[i]);
		m.m_indices1.push_back(m_IndexedMesh.m_indices1[i]);
		if(0==(i%3))
			m.m_indices2.push_back(m_IndexedMesh.m_indices2[i/3]);
	}
	#if 0
	D3DXFROMWINEVECTOR3 _max;
	D3DXFROMWINEVECTOR3 _min;
	m.maxmin(_max, _min);
	#endif
}
#endif

void Draw(	const std::vector<stx_Rule>& arules, 
		const std::string astrn, 
		const std::string aaxis, 
		const D3DXFROMWINEVECTOR2 aX, 
		const D3DXFROMWINEVECTOR2 aY,
		D3DXFROMWINEVECTOR3& amin,
		D3DXFROMWINEVECTOR3& amax)
{
	if(astrn=="")
		return;
	if(stx_is_number(astrn))
		return;
	std::vector<std::string> v=stx_GetRuleVec(arules, astrn);
	if(!v.size())
		return;
	int rp=stx_GetRulePosition(arules, astrn);
	if(rp<0)
		return;

			unsigned int i1=m_IndexedMesh.m_vertices3.size();
			DrawV(arules, v, aaxis, aX, aY, amin, amax);
			unsigned int i2=m_IndexedMesh.m_vertices3.size();
			IndexedMesh IndexedMesh_;
			IndexedMesh_.m_sInstanceName=astrn;
			IndexedMesh_.m_dStart=i1;
			IndexedMesh_.m_dStop=i2;
			//LOG_PRINT("I(%s)\n", v[i+2].c_str());
			//LOG_PRINT("i1=%d\n", i1);
			//LOG_PRINT("i2=%d\n", i2);
			for(unsigned int j=i1;j<i2;j++)
			{
				IndexedMesh_.m_texID.push_back(m_IndexedMesh.m_texID[j]);
				IndexedMesh_.m_normals3.push_back(m_IndexedMesh.m_normals3[j]);
				IndexedMesh_.m_texcoords2.push_back(m_IndexedMesh.m_texcoords2[j]);
				IndexedMesh_.m_indices1.push_back(m_IndexedMesh.m_indices1[j]);
				//if(0==(j%3))
				//	IndexedMesh_.m_indices2.push_back(m_IndexedMesh.m_indices2[j/3]);
				D3DXFROMWINEVECTOR3 v3=m_IndexedMesh.m_vertices3[j];
				D3DXFROMWINEVec3TransformCoord(&v3, &v3, &m_IndexedMesh.m_mW[j]);
				D3DXFROMWINEMATRIX m=stx_GetPivotMatrix3(vR, vS, vT);
				D3DXFROMWINEVec3TransformCoord(&v3, &v3, &m);
				IndexedMesh_.m_mW.push_back(I);
				IndexedMesh_.m_vertices3.push_back(v3);
			}
			D3DXFROMWINEVECTOR3 ac;
			IndexedMesh_.maxmin(amin, amax, ac);
			m_Transforms.insert(std::pair<std::string, D3DXFROMWINEMATRIX>(astrn, stx_GetPivotMatrix3(vR, vS, vT)));
			bool found=false;
			unsigned int j=0;
			for(j=0;j<m_Meshes.size();j++)
			{
				if(m_Meshes[j].m_sInstanceName==astrn)
				{
					found=true;
					break;
				}
			}
			if(!found)
			{
				m_Meshes.push_back(IndexedMesh_);
				//LOG_PRINT("1:m_Meshes.push_back(IndexedMesh_:%s);\n", astrn.c_str());
				//LOG_PRINT("amin.x=%f\n", amin.x);
				//LOG_PRINT("amin.y=%f\n", amin.y);
				//LOG_PRINT("amin.z=%f\n", amin.z);
				//LOG_PRINT("amax.x=%f\n", amax.x);
				//LOG_PRINT("amax.y=%f\n", amax.y);
				//LOG_PRINT("amax.z=%f\n", amax.z);
				//LOG_PRINT("ac.x=%f\n", ac.x);
				//LOG_PRINT("ac.y=%f\n", ac.y);
				//LOG_PRINT("ac.z=%f\n", ac.z);
			}
}

void DrawV(	const std::vector<stx_Rule>& arules,
		const std::vector<std::string>& v, 
		const std::string aaxis, 
		const D3DXFROMWINEVECTOR2 aX, 
		const D3DXFROMWINEVECTOR2 aY,
		D3DXFROMWINEVECTOR3& amin,
		D3DXFROMWINEVECTOR3& amax)
{
	D3DXFROMWINEVECTOR2 vTC(0.0f, 1.0f);
	D3DXFROMWINEVECTOR3 vP(0.0f, 0.0f, 0.0f);
	//D3DXFROMWINEVECTOR3 vC(1.0f, 1.0f, 1.0f);
	D3DXFROMWINEMATRIX w;
	unsigned int i=0;
	while(i<v.size())
	{
		if(v[i]=="I")
		{
			Draw(arules, v[i+2], aaxis, aX, aY, amin, amax);
			i+=4;
		}
		else if(v[i]=="Push")
		{
			m_vR.push_back(vR);
			m_vS.push_back(vS);
			m_vT.push_back(vT);
			m_vStart.push_back(m_IndexedMesh.m_vertices3.size());
			i+=3;
		}
		else if(v[i]=="Pop")
		{
			if (!m_vR.empty()){vR=m_vR.back();m_vR.pop_back();}
			if (!m_vS.empty()){vS=m_vS.back();m_vS.pop_back();}
			if (!m_vT.empty()){vT=m_vT.back();m_vT.pop_back();}
			unsigned int iStart=m_IndexedMesh.m_vertices3.size();
			if (!m_vStart.empty()){ iStart=m_vStart.back(); m_vStart.pop_back();}
			#if 1
			unsigned int iStop=m_IndexedMesh.m_vertices3.size();
			D3DXFROMWINEMATRIX W;
			if(v[i+2]=="Identity")
			{
				#if 0
				D3DXFROMWINEMATRIX R,S,T;
				D3DXFROMWINEMatrixRotationYawPitchRoll(&R, vR.x, vR.y, vR.z);
				D3DXFROMWINEMatrixScaling(&S, vS.x, vS.y, vS.z);
				D3DXFROMWINEMatrixTranslation(&T, vT.x, vT.y, vT.z);
				W=R*S*T;
				#else
				D3DXFROMWINEMatrixIdentity(&W);
				#endif
			}
			else 
				W=stx_GetPivotMatrix3(vR, vS, vT);
			for(unsigned int j=iStart;j<iStop;j++)
			{
				D3DXFROMWINEVec3TransformCoord(&m_IndexedMesh.m_vertices3[j], &m_IndexedMesh.m_vertices3[j], &m_IndexedMesh.m_mW[j]);
				m_IndexedMesh.m_mW[j]=W;
			}
			#endif
			if(v[i+2]=="Identity")
				i+=4;
			else
				i+=3;
		}
		#if 1
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
				DrawVertex(&m_IndexedMesh, w, vC_, vTC_, vP_);
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
				DrawVertex(&m_IndexedMesh, w, vC_, vTC_, vP_);
			}
			i+=4;
		}
		else if(v[i]=="Shape") // && (v[i+1]=="(") && (v[i+3]==")"))
		{
			D3DXFROMWINEVECTOR3 scale(1.0f, 1.0f, 1.0f);
			float awidth=1.0f;
			float adepth=1.0f;
			float aradius=1.0f;
			float abottomRadius=1.0f;
			float atopRadius=1.0f;
			float aradMajor=1.0f;
			float aradMinor=1.0f;
			float afA=1.0f;
			float afB=1.0f;
			float afC=1.0f;
			float aheight=1.0f;
			float alength=1.0f;
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
				afA=awidth;
				afB=awidth;
				afC=awidth;
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
			if(v[i+2]=="TexturedBox")
				mesh3D.CreateTexturedBox(awidth, aheight, adepth, aShader);
			else if(v[i+2]=="TexturedQuad")
				mesh3D.CreateTexturedQuad(awidth, aheight, aShader);
			else if(v[i+2]=="Box")
				mesh3D.CreateBox(awidth, aheight, adepth, aShader);
			else if(v[i+2]=="Sphere")
       				mesh3D.CreateSphere(aradius, aShader);
			else if(v[i+2]=="Cone")
				mesh3D.CreateCone(aradius, aheight, aShader);
#if 1
			else if(v[i+2]=="Hemis")
				mesh3D.CreateHemis(aShader);
			else if(v[i+2]=="Torus")
				mesh3D.CreateTorus(aradMajor, aradMinor, aShader);
#endif
			else if(v[i+2]=="Octa")
				mesh3D.CreateOcta(aShader);
			else if(v[i+2]=="Tetra")
				mesh3D.CreateTetra(aShader);
			else if(v[i+2]=="Ellipsoid")
				mesh3D.CreateEllipsoid(afA, afB, afC, aShader);
			else if(v[i+2]=="Cylinder")
				mesh3D.CreateCylinder(abottomRadius, atopRadius, alength, aShader);
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
				DrawVertex(&m_IndexedMesh, w, vC_, vTC_, vP_);
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
				DrawVertex(&m_IndexedMesh, w, vC_, vTC_, vP_);
			}
			i+=4;
		}
		#endif
		else if(v[i]=="Align")
		{
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
			std::string n=v[i+4];
			Draw(arules, n, ax, aX*vS.x, aY*vS.y, amin, amax);
			i+=5;
		}
		else if(v[i]=="Align3D")
		{
			// Align3D(SIDE_FACE_X,  4, Alternating7Z)	
			std::string ax="X";
			f=atof(v[i+3].c_str());
			D3DXFROMWINEVECTOR3 vR_=vR;
			D3DXFROMWINEVECTOR3 vT_=vT;
			if(v[i+2]=="SIDE_FACE_X")
			{
				vR=D3DXFROMWINEVECTOR3(0*90.0f, 0.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(0.0f, 0.0f, f*1.0f);
				ax="X";
			}
			else if(v[i+2]=="SIDE_FACE_Y")
			{
				vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(0.0f, f*1.0f,  0.0f);
				ax="Y";
			}
			else if(v[i+2]=="SIDE_FACE_Z")
			{
				vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(f*1.0f, 0.0f, 0.0f);
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
				vR=D3DXFROMWINEVECTOR3(0.0f, 1*90.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(0.0f, -1.0f*f, 0.0f);
				ax="Y";
			}
			else if(v[i+2]=="SIDE_FACE_NZ")
			{
				vR=D3DXFROMWINEVECTOR3(1*90.0f, 0.0f, 0.0f);
				vT=D3DXFROMWINEVECTOR3(-1.0f*f, 0.0f, 0.0f);
				ax="Z";
			}
			std::string n=v[i+4];
			Draw(arules, n, ax, aX, aY, amin, amax);
			vT=vT_;
			vR=vR_;
			i+=6;
		}
		else if(v[i]=="Repeat")
		{
			// Repeat(X,2,wall_window_wall)
			D3DXFROMWINEVECTOR2 X=aX;
			D3DXFROMWINEVECTOR2 Y=aY;
			unsigned int dimension=atoi(v[i+3].c_str());
			for(unsigned int j=0;j<dimension;j++)
			{
				if(v[i+2]=="X")
					X.y=X.x+(aX.y-aX.x)/dimension;
				else if(v[i+2]=="Y")
					Y.y=Y.x+(aY.y-aY.x)/dimension;
				Draw(arules, v[i+4], v[i+2], X*vS.x, Y*vS.y, amin, amax);
				if(v[i+2]=="X")
					X.x=X.y;
				else if(v[i+2]=="Y")
					Y.x=Y.y;
			}
			i+=6;
		}
		else if(v[i]=="Split")
		{ // 	Split(X,[30%:40%:30%],[Quad:vertical_door_wall:Quad])
			std::vector<std::string> symbols;
			std::vector<float> percents;
			unsigned int k=i+4;
			while(v[k]!="]")
				percents.push_back(atof(v[k++].c_str()));
			k+=2;
			while(v[k]!="]")
				symbols.push_back(v[k++]);
			D3DXFROMWINEVECTOR2 X=aX;
			D3DXFROMWINEVECTOR2 Y=aY;
			for(unsigned int j=0;j<symbols.size();j++)
			{
				if(v[i+2]=="X")
					X.y=X.x+(aX.y-aX.x)*0.5f*percents[j]/50.0f;
				else if(v[i+2]=="Y")
					Y.y=Y.x+(aY.y-aY.x)*0.5f*percents[j]/50.0f;
				if(symbols[j]!="Empty")
					Draw(arules, symbols[j], v[i+2], X*vS.x, Y*vS.y, amin, amax);
				if(v[i+2]=="X")
					X.x=X.y;
				else if(v[i+2]=="Y")
					Y.x=Y.y;
			}
			i+=8+symbols.size()+percents.size();
		}
		else if(v[i]=="Rim3D")
		{ // 	Rim3D([Quad:vertical_door_wall:Quad])
			D3DXFROMWINEVECTOR3 vT_=vT;
			std::vector<std::string> symbols;
			unsigned int k=i+2;
			while(v[k]!="]")
				symbols.push_back(v[k++]);
			if(symbols.size()>0){Draw(arules, symbols[0], "X", aX, aY, amin, amax);
			vT.x+=1.0f*0.5f*(amax.x-amin.x);}
			if(symbols.size()>1){Draw(arules, symbols[1], "X", aX, aY, amin, amax);
			vT.x+=1.0f*0.5f*(amax.x-amin.x);}
			if(symbols.size()>2){Draw(arules, symbols[2], "X", aX, aY, amin, amax);
			vT.y+=1.0f*0.5f*(amax.y-amin.y);}
			if(symbols.size()>3){Draw(arules, symbols[3], "Y", aX, aY, amin, amax);
			vT.y+=1.0f*0.5f*(amax.y-amin.y);}
			if(symbols.size()>4){Draw(arules, symbols[4], "X", aX, aY, amin, amax);
			vT.x-=1.0f*0.5f*(amax.x-amin.x);}
			if(symbols.size()>5){Draw(arules, symbols[5], "X", aX, aY, amin, amax);
			vT.x-=1.0f*0.5f*(amax.x-amin.x);}
			if(symbols.size()>6){Draw(arules, symbols[6], "X", aX, aY, amin, amax);
			vT.y-=1.0f*0.5f*(amax.y-amin.y);}
			if(symbols.size()>7){Draw(arules, symbols[7], "Y", aX, aY, amin, amax);}
			vT=vT_;
			i+=4+symbols.size();
		}
		else if(v[i]=="Line3D")
		{
			// Line3D(5,X,CubeIII)
				//LOG_PRINT("iii:vT.x=%f\n", vT.x);
				//LOG_PRINT("iii:vT.y=%f\n", vT.y);
				//LOG_PRINT("iii:vT.z=%f\n", vT.z);
			D3DXFROMWINEVECTOR3 vT_=vT;
			unsigned int dimension=atoi(v[i+2].c_str());
			std::string n=v[i+4];
			D3DXFROMWINEVECTOR3 v3;
			for(unsigned int xxx=0;xxx<dimension;xxx++)
			{
				//LOG_PRINT("iii:v[i+3]=%s\n", v[i+3].c_str());
				Draw(arules, n, v[i+3], aX, aY, amin, amax);
				#if 0
				amax.x=1.0f;
				amax.y=1.0f;
				amax.z=1.0f;
				amin.x=-1.0f;
				amin.y=-1.0f;
				amin.z=-1.0f;
				#endif
				if(v[i+3]=="X")
					vT.x+=1.0f*0.5f*(amax.x-amin.x);
				else if(v[i+3]=="Y")
					vT.y+=1.0f*0.5f*(amax.y-amin.y);
				else if(v[i+3]=="Z")
					vT.z+=1.0f*0.5f*(amax.z-amin.z);
				//LOG_PRINT("iii:vT.x=%f\n", vT.x);
			}
			vT=vT_;
			i+=6;
		}
		else if(v[i]=="Grid3D")
		{
			// Grid3D(NX,NZ,wall_window_wall)
				//LOG_PRINT("jjj:vT.x=%f\n", vT.x);
				//LOG_PRINT("jjj:vT.y=%f\n", vT.y);
				//LOG_PRINT("jjj:vT.z=%f\n", vT.z);
			D3DXFROMWINEVECTOR3 vT_=vT;
			unsigned int dimensionX=atoi(v[i+2].c_str());
			unsigned int dimensionZ=atoi(v[i+3].c_str());
			std::string n=v[i+4];
			D3DXFROMWINEVECTOR3 v3;
			float vTx=vT.x;
			for(unsigned int zzz=0;zzz<dimensionZ;zzz++)
			{
				vT.x=vTx;
			for(unsigned int xxx=0;xxx<dimensionX;xxx++)
			{
				Draw(arules, n, "X", aX, aY, amin, amax);
				#if 0
				amax.x=1.0f;
				amax.y=1.0f;
				amax.z=1.0f;
				amin.x=-1.0f;
				amin.y=-1.0f;
				amin.z=-1.0f;
				#endif
				vT.x+=1.0f*0.5f*(amax.x-amin.x);
				//LOG_PRINT("jjj:vT.x=%f\n", vT.x);
			}
				vT.z+=1.0f*0.5f*(amax.z-amin.z);
				//LOG_PRINT("jjj:vT.z=%f\n", vT.z);
			}
			vT=vT_;
			i+=6;
		}
		else if(v[i]=="Repeat3D")
		{
			// Repeat(X,2,wall_window_wall)
			D3DXFROMWINEVECTOR3 vT_=vT;
			unsigned int dimension=atoi(v[i+3].c_str());
			std::string n=v[i+4];
			float step=2.0f/dimension;
			for(unsigned int j=0;j<dimension;j++)
			{
				Draw(arules, n, v[i+2], aX, aY, amin, amax);
				if(v[i+2]=="X")
				{
					D3DXFROMWINEVECTOR3 v3(1.0f*0.5f*(amax.x-amin.x), 0.0f, 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Y")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 1.0f*0.5f*(amax.y-amin.y), 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Z")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 0.0f, 1.0f*0.5f*(amax.z-amin.z));
					vT+=v3;
				}
			}
			vT=vT_;
			i+=6;
		}
		else if(v[i]=="Split3D")
		{ // 	Split(X,[30%:40%:30%],[Quad:vertical_door_wall:Quad])
			D3DXFROMWINEVECTOR3 vT_=vT;
			std::vector<std::string> symbols;
			std::vector<float> percents;
			unsigned int k=i+4;
			while(v[k]!="]")
				percents.push_back(atof(v[k++].c_str()));
			k+=2;
			while(v[k]!="]")
				symbols.push_back(v[k++]);
			for(unsigned int j=0;j<symbols.size();j++)
			{			
				if((symbols[j]=="Step")&&
				   (symbols[j+1]=="(")&&
				   (symbols[j+3]==")"))
				{
					float d=atof(symbols[j+2].c_str());	
					if(v[i+2]=="X")
						vT+=D3DXFROMWINEVECTOR3(d, 0.0f, 0.0f);
					else if(v[i+2]=="Y")
						vT+=D3DXFROMWINEVECTOR3(0.0f, d, 0.0f);
					else if(v[i+2]=="Z")
						vT+=D3DXFROMWINEVECTOR3(0.0f, 0.0f, d);
					i+=4;	
					continue;
				}
				Draw(arules, symbols[j], v[i+2], aX, aY, amin, amax);
			if(1)//(m_Meshes.size()>=2)
			{
				//LOG_PRINT("mmm:amax.y=%f\n", amax.y);
				//LOG_PRINT("mmm:amin.y=%f\n", amin.y);
				if(v[i+2]=="X")
				{
					D3DXFROMWINEVECTOR3 v3(0.5f*(amax.x-amin.x), 0.0f, 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Y")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 0.5f*(amax.y-amin.y), 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Z")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 0.0f, 0.5f*(amax.z-amin.z));
					vT+=v3;
				}
			}
			}
			i+=8+symbols.size()+percents.size();
			vT=vT_;
		}
		else if(v[i]=="Repeat3D2")
		{
			// Repeat(X,2,wall_window_wall)
			D3DXFROMWINEVECTOR3 vT_=vT;
			unsigned int dimension=atoi(v[i+3].c_str());
			std::string n=v[i+4];
			float step=2.0f/dimension;
			for(unsigned int j=0;j<dimension;j++)
			{
				Draw(arules, n, v[i+2], aX, aY, amin, amax);
				if(v[i+2]=="X")
				{
					D3DXFROMWINEVECTOR3 v3(1.0f*(amax.x-amin.x), 0.0f, 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Y")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 1.0f*(amax.y-amin.y), 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Z")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 0.0f, 1.0f*(amax.z-amin.z));
					vT+=v3;
				}
			}
			vT=vT_;
			i+=6;
		}
		else if(v[i]=="Split3D2")
		{ // 	Split(X,[30%:40%:30%],[Quad:vertical_door_wall:Quad])
			D3DXFROMWINEVECTOR3 vT_=vT;
			std::vector<std::string> symbols;
			std::vector<float> percents;
			unsigned int k=i+4;
			while(v[k]!="]")
				percents.push_back(atof(v[k++].c_str()));
			k+=2;
			while(v[k]!="]")
				symbols.push_back(v[k++]);
			for(unsigned int j=0;j<symbols.size();j++)
			{			
				if((symbols[j]=="Step")&&
				   (symbols[j+1]=="(")&&
				   (symbols[j+3]==")"))
				{
					float d=atof(symbols[j+2].c_str());	
					if(v[i+2]=="X")
						vT+=D3DXFROMWINEVECTOR3(d, 0.0f, 0.0f);
					else if(v[i+2]=="Y")
						vT+=D3DXFROMWINEVECTOR3(0.0f, d, 0.0f);
					else if(v[i+2]=="Z")
						vT+=D3DXFROMWINEVECTOR3(0.0f, 0.0f, d);
					i+=4;	
					continue;
				}
				Draw(arules, symbols[j], v[i+2], aX, aY, amin, amax);
			if(1)//(m_Meshes.size()>=2)
			{
				//LOG_PRINT("mmm:amax.y=%f\n", amax.y);
				//LOG_PRINT("mmm:amin.y=%f\n", amin.y);
				if(v[i+2]=="X")
				{
					D3DXFROMWINEVECTOR3 v3(1.0f*(amax.x-amin.x), 0.0f, 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Y")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 1.0f*(amax.y-amin.y), 0.0f);
					vT+=v3;
				}
				else if(v[i+2]=="Z")
				{
					D3DXFROMWINEVECTOR3 v3(0.0f, 0.0f, 1.0f*(amax.z-amin.z));
					vT+=v3;
				}
			}
			}
			i+=8+symbols.size()+percents.size();
			vT=vT_;
		}
		else if(v[i]=="R")
		{
			vR=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
			i+=6;
		}
		else if(v[i]=="S")
		{
			vS=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
			if(1)
				vT=D3DXFROMWINEVECTOR3(vT.x/vS.x, vT.y/vS.y, vT.z/vS.z);
			i+=6;
		}
		else if(v[i]=="T")
		{
			vT=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
			i+=6;
		}
		else if(v[i]=="TR")
		{
			vT+=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
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
		else if(v[i]=="Color")
		{
			D3DXFROMWINEVECTOR3 vC(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));

			g_color3_=vC;
			D3DXFROMWINEVECTOR4 color(vC.x, vC.y, vC.z, 1.0f);
			g_color4_=color;

			i+=6;
		}
		else if(v[i]=="TexCoord")
		{
			vTC=D3DXFROMWINEVECTOR2(atof(v[i+2].c_str()), atof(v[i+3].c_str()));
			i+=5;
		}
		else if(v[i]=="Texture")
		{
			if(!m_bUseColor){
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
				D3DXFROMWINEVECTOR3 vP_=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
				if(m_b2D)
				{
				#if 1
				float epsilon=0.000005f;
				if(vP_.x<(-1.0f+epsilon))
				{
					vP_.x=aX.x;
				}
				else if(vP_.x>(1.0f-epsilon))
				{
					vP_.x=aX.y;
				}
				if(vP_.y<(-1.0f+epsilon))
				{
					vP_.y=aY.x;
				}
				else if(vP_.y>(1.0f-epsilon))
				{
					vP_.y=aY.y;
				}
				#else
					float distance=1.0f;
        				vP_.x += distance * cosf(aX.x);
        				vP_.z += distance * sinf(aX.x);
				#endif
				}
				w=stx_GetPivotMatrix3(vR, vS, vT);

				D3DXFROMWINEVECTOR2 vTC_=vTC;
				#ifdef __ANISO__
				vTC_.x=vP_.x;
				vTC_.y=vP_.y;
				#endif				
				DrawVertex(&m_IndexedMesh, w, g_color3_, vTC_, vP_);
			}
			i+=6;
		}
		else
			i++;
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
	m_IndexedMesh.m_vertices3.clear();
	m_IndexedMesh.m_texID.clear();
	m_IndexedMesh.m_normals3.clear();
	m_IndexedMesh.m_texcoords2.clear();
	m_IndexedMesh.m_indices1.clear();
	m_IndexedMesh.m_indices2.clear();

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
	//LOG_PRINT("3BodyElement=%x\n", BodyNode);
	TiXmlElement* AxiomElement = BodyNode->FirstChild("Axiom")->ToElement();
	axiom=AxiomElement->GetText();
	if(!stx_strlen(axiom.c_str()))
		axiom=AxiomElement->Attribute("I");
	
	std::string dim=AxiomElement->Attribute("Dimension");
	if(dim=="2D")
		m_b2D=true;
	
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
	
	TiXmlHandle RulesHandle = BodyNode->FirstChild("Rules");

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

	LOG_FNLN;

	if((axiom[0]=='I') && (axiom[1]=='(') && (axiom[axiom.length()-1]==')'))
	{
		axiom=axiom.substr(2,axiom.length()-3);
		D3DXFROMWINEVECTOR2 aX(-1.0f, 1.0f); 
		D3DXFROMWINEVECTOR2 aY(-1.0f, 1.0f);
		D3DXFROMWINEVECTOR3 amin(1e10f,1e10f,1e10f);
		D3DXFROMWINEVECTOR3 amax(-1e10f,-1e10f,-1e10f);

	LOG_FNLN;
		Draw(rules, axiom, "X", aX, aY, amin, amax);

	LOG_FNLN;
	}
	#endif

	LOG_FNLN;
	LOG_PRINT("axioms[0]=%s\n", axioms[0].c_str());
	LOG_PRINT("textures.size()=%d\n", textures.size());
	LOG_PRINT("vt.size()=%d\n", vt.size());

	LOG_PRINT("g_vertices_.size() =%d\n", g_vertices_.size());
	LOG_PRINT("g_verticesC_.size() =%d\n", g_verticesC_.size());
	LOG_PRINT("m_IndexedMesh.m_vertices3.size() =%d\n", m_IndexedMesh.m_vertices3.size());
	LOG_PRINT("m_IndexedMesh.m_normals3.size()  =%d\n", m_IndexedMesh.m_normals3.size());
	LOG_PRINT("m_IndexedMesh.m_texcoords2.size()=%d\n", m_IndexedMesh.m_texcoords2.size());
	LOG_PRINT("m_IndexedMesh.m_indices1.size()  =%d\n", m_IndexedMesh.m_indices1.size());
	LOG_PRINT("m_IndexedMesh.m_indices2.size()  =%d\n", m_IndexedMesh.m_indices2.size());

	LOG_PRINT("m_Meshes.size() =%d\n", m_Meshes.size());
	if(0)for (unsigned int i=0;i<m_Meshes.size();i++)
	{
		LOG_PRINT("obj[%d].m_sInstanceName=%s\n", i, m_Meshes[i].m_sInstanceName.c_str());
		LOG_PRINT("obj[%d].m_dStart=%d\n", i, m_Meshes[i].m_dStart);
		LOG_PRINT("obj[%d].m_dStop=%d\n", i, m_Meshes[i].m_dStop);
		LOG_PRINT("obj[%d].m_vertices3.size() =%d\n", i, m_Meshes[i].m_vertices3.size());
		LOG_PRINT("obj[%d].m_normals3.size()  =%d\n", i, m_Meshes[i].m_normals3.size());
		LOG_PRINT("obj[%d].m_texcoords2.size()=%d\n", i, m_Meshes[i].m_texcoords2.size());
		LOG_PRINT("obj[%d].m_indices1.size()  =%d\n", i, m_Meshes[i].m_indices1.size());
		LOG_PRINT("obj[%d].m_indices2.size()  =%d\n", i, m_Meshes[i].m_indices2.size());
    	}
	
	//LOG_PRINT("m_Transforms.size() =%d\n", m_Transforms.size());
	unsigned int i=0;
	for (std::map<std::string, D3DXFROMWINEMATRIX>::iterator it=m_Transforms.begin(); it!=m_Transforms.end(); ++it)
	{
		//LOG_PRINT("m_sInstanceName=%s\n", it->first.c_str());
		i++;
	}

	//stx_exit(0);
	if(!m_bUseColor)
	{
	#if 1
	if(mesh_.size()){
	mesh_[0].vertices.clear();
	mesh_[0].indices.clear();
	for(unsigned int i=0;i<m_IndexedMesh.m_vertices3.size();i++)
	{
		stx_VertexPositionNormalTexture v;
		D3DXFROMWINEVec3TransformCoord(&m_IndexedMesh.m_vertices3[i], &m_IndexedMesh.m_vertices3[i], &m_IndexedMesh.m_mW[i]);
		v.Position=m_IndexedMesh.m_vertices3[i];
		v.Normal=m_IndexedMesh.m_normals3[i];
		v.Tex=m_IndexedMesh.m_texcoords2[i];
		mesh_[0].vertices.push_back(v);
	}
	mesh_[0].indices=m_IndexedMesh.m_indices1;

	#if 0
	//LOG_FNLN;
	for(unsigned int i=0;i<m_IndexedMesh.m_indices2.size();i++)
	{
		//LOG_PRINT("m_IndexedMesh.m_indices2[%d]=%d\n", i, m_IndexedMesh.m_indices2[i]);
	}
	#endif

	stx_Materials_(vt, m_IndexedMesh.m_indices2, mesh_);
	stx_writeXFile(afiletosave.c_str(), mesh_);//, m_IndexedMesh.m_vertices3, m_IndexedMesh.m_normals3, m_IndexedMesh.m_texcoords2, m_IndexedMesh.m_indices1, texture);
	}
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
		//LOG_PRINT("vtex[%d]:%s\n", i, vtex[i].c_str());
		std::string str=stx_trim(vtex[i]);
		stx_ReplaceAll(str, "&#32;", " ");
			//LOG_PRINT("str=%s\n", str.c_str());
		textures.push_back(IRenderer::GetRendererInstance()->addImageLibTexture(str.c_str(), false, ss));
		stx_Material m(str.c_str()+1, vC);
		vt.push_back(m);
	}
}else{
		//LOG_PRINT("4BodyElement=%x\n", BodyNode);
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
			//LOG_PRINT("str[%d]=%s\n", j, str.c_str());
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
	{//LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
    TiXmlHandle docHandle(&doc);
    BodyNode = docHandle.FirstChild("Body").Node();

	{
	TiXmlHandle RulesHandle = docHandle.FirstChild("Body");
	int i=1;
	TiXmlHandle RuleHandle = RulesHandle.FirstChild("Axiom");
	TiXmlElement* RuleElement = RuleHandle.Element();
	while ( RuleElement )
	{
		std::string n=RuleElement->GetText();

	//LOG_PRINT("Axiom=%s\n", n.c_str());

	if((n[0]=='I') && (n[1]=='(') && (n[n.length()-1]==')'))
	{
		n=n.substr(2,n.length()-3);
		axioms.push_back(n);
	}

		RuleHandle = RulesHandle.Child( "Axiom", i++ );
		RuleElement = RuleHandle.Element();
	}
	}

	m_sfn=afn;
	m_sfiletosave=afiletosave;

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
	else {
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
};
#endif
/*
Scope Scope is a term describing space properties of a shape. It is defined
as a bounding volume consisting of:
• (local) position
• (local) orientation
• scale
• parent scope if any
*/

