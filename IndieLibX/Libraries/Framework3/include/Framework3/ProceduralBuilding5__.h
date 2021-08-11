#ifndef __ProceduralBuilding5___
#define __ProceduralBuilding5___
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

#define USEMESH 1

enum tTerminal
{
	eQuad_Terminal,
	eVertex_Terminal,
};

struct ProceduralBuilding5
{
	ShaderID shd;
	VertexFormatID vf;
struct IndexedMesh
{
	std::string m_sInstanceName;
	std::vector<D3DXFROMWINEMATRIX> m_mW;
	std::vector<stx_VertexPositionColor> m_vertices;
	std::vector<__WORD__> m_indices;		
	D3DXFROMWINEVECTOR3 _max;
	D3DXFROMWINEVECTOR3 _min;
	__WORD__ m_dStart;
	__WORD__ m_dStop;
	float Hits(const D3DXFROMWINEVECTOR3& start, const D3DXFROMWINEVECTOR3& direction, D3DXFROMWINEVECTOR3& p)
	{
		float d=1e10f;
		float t=1e10f;
		for(unsigned int i=0;i<m_indices.size()/3;i++)
		{
			D3DXFROMWINEVECTOR3 v0=m_vertices[3*i+0].Position;
			D3DXFROMWINEVECTOR3 v1=m_vertices[3*i+1].Position;
			D3DXFROMWINEVECTOR3 v2=m_vertices[3*i+2].Position;
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
		for(unsigned int i=0;i<m_vertices.size();i++)
		{
			if(m_vertices[i].Position.x>max_.x)
				max_.x=m_vertices[i].Position.x;
			if(m_vertices[i].Position.y>max_.y)
				max_.y=m_vertices[i].Position.y;
			if(m_vertices[i].Position.z>max_.z)
				max_.z=m_vertices[i].Position.z;
			if(m_vertices[i].Position.x<min_.x)
				min_.x=m_vertices[i].Position.x;
			if(m_vertices[i].Position.y<min_.y)
				min_.y=m_vertices[i].Position.y;
			if(m_vertices[i].Position.z<min_.z)
				min_.z=m_vertices[i].Position.z;
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
		m_vertices=rhs.m_vertices;
		m_mW=rhs.m_mW;
		m_indices=rhs.m_indices;	
		_max=rhs._max;
		_min=rhs._min;
	}
	virtual ~IndexedMesh()
	{
		m_sInstanceName="";
		m_vertices.clear();
		m_indices.clear();
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
	float f;

	std::vector<stx_Material> vt;

	std::vector<stx_VertexPositionColor> g_verticesC_;
	std::vector<__WORD__> g_indices;

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
	ProceduralBuilding5(){}
	void init(std::string afn="", std::string afiletosave="", bool abUseLua=true)
	{
		m_b2D=false;

		m_bUseColor=true;
		m_bUseLua=abUseLua;

		//g_Terminal=eQuad_Terminal;
		g_Terminal=eVertex_Terminal;
		vR=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		vS=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f);
		vT=D3DXFROMWINEVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXFROMWINEMatrixIdentity(&I);

		f=1.0f;

		g_color4_=D3DXFROMWINEVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

		if(!m_bUseLua)
		{
			std::string fn=stx_convertpath(afn.c_str());
			//LOG_PRINT("fn.c_str()=%s\n", fn.c_str());
		}

		init_(afn, afiletosave);
		if((m_IndexedMesh.m_vertices.size()==0)&&(g_verticesC_.size()==0))
			stx_exit(0);
		{
		shd = IRenderer::GetRendererInstance()->addShaderFromFile("/Framework3/plainSHD2C.HLSL.shd", "main", "main");
		FormatDesc format[] =
		{
			0, TYPE_VERTEX,     FORMAT_FLOAT, 3,
			0, TYPE_TEXCOORD,   FORMAT_FLOAT, 4
		};
		vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
		}
	}
	virtual ~ProceduralBuilding5()
	{
		vt.clear();
		g_verticesC_.clear();

		m_IndexedMesh.m_vertices.clear();
		m_IndexedMesh.m_indices.clear();
	}


void DrawVertex(IndexedMesh* aIndexedMesh, const D3DXFROMWINEMATRIX& aW, const D3DXFROMWINEVECTOR4& aC, const D3DXFROMWINEVECTOR3& aV_)
{
	LOG_FNLN;
	stx_VertexPositionColor v;
	v.Position=aV_;
	v.Color=aC;
	aIndexedMesh->m_mW.push_back(aW);
	aIndexedMesh->m_vertices.push_back(v);
	aIndexedMesh->m_indices.push_back(aIndexedMesh->m_vertices.size()-1);
	g_color4_=aC;
	//g_color3_=D3DXFROMWINEVECTOR3(aC.x, aC.y, aC.z);
	
	stx_VertexPositionColor vx(aIndexedMesh->m_vertices[aIndexedMesh->m_vertices.size()-1].Position, aC);
	g_verticesC_.push_back(vx);g_indices.push_back(g_verticesC_.size()-1);
}

void GetMeshVertices(const char* afn, std::vector<Vertex332>& aVertices, std::vector<__WORD__>& aIndices)
{
	aVertices.clear();
	aIndices.clear();
	MeshRenderer2 mesh_;
	mesh_.Load(afn, eSimpleShader, false, true);
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
	iStart=m_IndexedMesh.m_vertices.size();
	Draw(arules, astrn, aaxis, aX, aY);
	iStop=m_IndexedMesh.m_vertices.size();
	//m.m_dStart=iStart;
	//m.m_dStop=iStop;
	for(unsigned int i=iStart;i<iStop;i++)
	{
		m.m_vertices.push_back(m_IndexedMesh.m_vertices[i].Position);
		m.m_mW.push_back(I);
		m.m_texID.push_back(m_IndexedMesh.m_texID[i]);
		m.m_colors.push_back(m_IndexedMesh.m_colors[i]);
		m.m_indices.push_back(m_IndexedMesh.m_indices[i]);
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

			unsigned int i1=m_IndexedMesh.m_vertices.size();
			DrawV(arules, v, aaxis, aX, aY, amin, amax);
			unsigned int i2=m_IndexedMesh.m_vertices.size();
			IndexedMesh IndexedMesh_;
			IndexedMesh_.m_sInstanceName=astrn;
			IndexedMesh_.m_dStart=i1;
			IndexedMesh_.m_dStop=i2;
			//LOG_PRINT("I(%s)\n", v[i+2].c_str());
			//LOG_PRINT("i1=%d\n", i1);
			//LOG_PRINT("i2=%d\n", i2);
			for(unsigned int j=i1;j<i2;j++)
			{
				IndexedMesh_.m_indices.push_back(m_IndexedMesh.m_indices[j]);
				stx_VertexPositionColor v3;
				v3.Position=m_IndexedMesh.m_vertices[j].Position;
				v3.Color=m_IndexedMesh.m_vertices[j].Color;
				D3DXFROMWINEVec3TransformCoord(&v3.Position, &v3.Position, &m_IndexedMesh.m_mW[j]);
				D3DXFROMWINEMATRIX m=stx_GetPivotMatrix3(vR, vS, vT);
				D3DXFROMWINEVec3TransformCoord(&v3.Position, &v3.Position, &m);
				IndexedMesh_.m_mW.push_back(I);
				IndexedMesh_.m_vertices.push_back(v3);
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
	D3DXFROMWINEVECTOR4 vC(0.5f, 0.5f, 0.5f, 0.0f);
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
			m_vStart.push_back(m_IndexedMesh.m_vertices.size());
			i+=3;
		}
		else if(v[i]=="Pop")
		{
			if (!m_vR.empty()){vR=m_vR.back();m_vR.pop_back();}
			if (!m_vS.empty()){vS=m_vS.back();m_vS.pop_back();}
			if (!m_vT.empty()){vT=m_vT.back();m_vT.pop_back();}
			unsigned int iStart=m_IndexedMesh.m_vertices.size();
			if (!m_vStart.empty()){ iStart=m_vStart.back(); m_vStart.pop_back();}
			#if 1
			unsigned int iStop=m_IndexedMesh.m_vertices.size();
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
				D3DXFROMWINEVec3TransformCoord(&m_IndexedMesh.m_vertices[j].Position, &m_IndexedMesh.m_vertices[j].Position, &m_IndexedMesh.m_mW[j]);
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
				D3DXFROMWINEVECTOR4 vC_=D3DXFROMWINEVECTOR4(v.color.x, v.color.y, v.color.z, 1.0f);
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(&m_IndexedMesh, w, vC_, vP_);
			}
			else
			for(unsigned int j=0;j<vertices.size();j++)
			{
				Vertex332 v=vertices[j];
				D3DXFROMWINEVECTOR3 vP_=v.position;
				D3DXFROMWINEVec3TransformCoord(&vP_, &vP_, &S);
				D3DXFROMWINEVECTOR4 vC_=D3DXFROMWINEVECTOR4(v.color.x, v.color.y, v.color.z, 1.0f);
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(&m_IndexedMesh, w, vC_, vP_);
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
				D3DXFROMWINEVECTOR4 vC_=g_color4_;
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(&m_IndexedMesh, w, vC_, vP_);
			}
			else
			for(unsigned int j=0;j<mesh3D.vertices.size();j++)
			{
				stx_VertexPositionNormalTexture v=mesh3D.vertices[j];
				D3DXFROMWINEVECTOR3 vP_=v.Position;
				D3DXFROMWINEVec3TransformCoord(&vP_, &vP_, &S);
				D3DXFROMWINEVECTOR4 vC_=g_color4_;
				w=stx_GetPivotMatrix(vR, vT);
				DrawVertex(&m_IndexedMesh, w, vC_, vP_);
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
			vC=D3DXFROMWINEVECTOR4(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()), atof(v[i+5].c_str()));
			i+=7;
		}
		else if(v[i]=="TexCoord")
		{
			vTC=D3DXFROMWINEVECTOR2(atof(v[i+2].c_str()), atof(v[i+3].c_str()));
			i+=5;
		}
		else if(v[i]=="Texture")
		{
			i+=4;
		}
		else if(v[i]=="Vertex")
		{	
			if(g_Terminal==eVertex_Terminal)
			{
				D3DXFROMWINEVECTOR3 vP_=D3DXFROMWINEVECTOR3(atof(v[i+2].c_str()), atof(v[i+3].c_str()), atof(v[i+4].c_str()));
				if(m_b2D)
				{
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
				}
				w=stx_GetPivotMatrix3(vR, vS, vT);
				DrawVertex(&m_IndexedMesh, w, vC, vP_);
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
	g_verticesC_.clear();
	m_IndexedMesh.m_vertices.clear();
	m_IndexedMesh.m_indices.clear();

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
	
	std::string dim=AxiomElement->Attribute("Dimension");
	if(dim=="2D")
		m_b2D=true;
	
	std::string texture=AxiomElement->Attribute("Texture");
	if(texture!="True")
		m_bUseColor=true;
	else
		m_bUseColor=false;
	
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

	if((axiom[0]=='I') && (axiom[1]=='(') && (axiom[axiom.length()-1]==')'))
	{
		axiom=axiom.substr(2,axiom.length()-3);
		D3DXFROMWINEVECTOR2 aX(-1.0f, 1.0f); 
		D3DXFROMWINEVECTOR2 aY(-1.0f, 1.0f);
		D3DXFROMWINEVECTOR3 amin(1e10f,1e10f,1e10f);
		D3DXFROMWINEVECTOR3 amax(-1e10f,-1e10f,-1e10f);
		Draw(rules, axiom, "X", aX, aY, amin, amax);
	}
	#endif

	LOG_FNLN;
	LOG_PRINT("axioms[0]=%s\n", axioms[0].c_str());
	LOG_PRINT("vt.size()=%d\n", vt.size());
	LOG_PRINT("g_verticesC_.size() =%d\n", g_verticesC_.size());
	LOG_PRINT("m_IndexedMesh.m_vertices.size() =%d\n", m_IndexedMesh.m_vertices.size());
	LOG_PRINT("m_IndexedMesh.m_indices.size()  =%d\n", m_IndexedMesh.m_indices.size());

	LOG_PRINT("m_Meshes.size() =%d\n", m_Meshes.size());
	for (unsigned int i=0;i<m_Meshes.size();i++)
	{
		LOG_PRINT("2:obj[%d].m_sInstanceName=%s\n", i, m_Meshes[i].m_sInstanceName.c_str());
		LOG_PRINT("2:obj[%d].m_dStart=%d\n", i, m_Meshes[i].m_dStart);
		LOG_PRINT("2:obj[%d].m_dStop=%d\n", i, m_Meshes[i].m_dStop);
		LOG_PRINT("2:obj[%d].m_vertices.size()=%d\n", i, m_Meshes[i].m_vertices.size());
		LOG_PRINT("2:obj[%d].m_indices.size()  =%d\n", i, m_Meshes[i].m_indices.size());
    	}
	
	//LOG_PRINT("m_Transforms.size() =%d\n", m_Transforms.size());
	unsigned int i=0;
	for (std::map<std::string, D3DXFROMWINEMATRIX>::iterator it=m_Transforms.begin(); it!=m_Transforms.end(); ++it)
	{
		//LOG_PRINT("3:m_sInstanceName=%s\n", it->first.c_str());
		i++;
	}

	//stx_exit(0);
	
	return 0;
}
int initTextures(std::string afn, std::string afiletosave)
{
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
			stx_Material m(str.c_str()+1, vC);
			vt.push_back(m);
		}
}
	return 0;
	return 0;
}

int init_(std::string afn, std::string afiletosave)
{
//listFiles();

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


{
	D3DXFROMWINEVECTOR3 aiVecs[2] = {D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f),
		D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f) };

	{
		D3DXFROMWINEMATRIX m;
	{
		for( unsigned int a = 0; a < g_verticesC_.size();++a)
		{
			D3DXFROMWINEVECTOR3 pc =g_verticesC_[a].Position;
			D3DXFROMWINEVECTOR3 pc1=pc;
			
			//D3DXFROMWINEVec3TransformCoord((D3DXFROMWINEVECTOR3*)&pc1,(D3DXFROMWINEVECTOR3*)&pc, (D3DXFROMWINEMATRIX*)&aiMe);

			aiVecs[0].x = std::min( aiVecs[0].x, pc1.x);
			aiVecs[0].y = std::min( aiVecs[0].y, pc1.y);
			aiVecs[0].z = std::min( aiVecs[0].z, pc1.z);
			aiVecs[1].x = std::max( aiVecs[1].x, pc1.x);
			aiVecs[1].y = std::max( aiVecs[1].y, pc1.y);
			aiVecs[1].z = std::max( aiVecs[1].z, pc1.z);
		}
	}
	}

	D3DXFROMWINEVECTOR3 vDelta = aiVecs[1]-aiVecs[0];
	D3DXFROMWINEVECTOR3 vHalf =  aiVecs[0] + (vDelta / 2.0f);
	float fScale = 2.0f / D3DXFROMWINEVec3Length(&vDelta);

	D3DXFROMWINEMATRIX m_mWorld =  D3DXFROMWINEMATRIX(
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		-vHalf.x,-vHalf.y,-vHalf.z,1.0f) *
		D3DXFROMWINEMATRIX(
		fScale,0.0f,0.0f,0.0f,
		0.0f,fScale,0.0f,0.0f,
		0.0f,0.0f,fScale,0.0f,
		0.0f,0.0f,0.0f,1.0f);

	{
		for( unsigned int j = 0; j < g_verticesC_.size();++j)
		{
			D3DXFROMWINEVec3TransformCoord(&g_verticesC_[j].Position, &g_verticesC_[j].Position, &m_mWorld);
		}
	}
}

	return 0;
}

void render()
{
	D3DXFROMWINEMATRIX W, R, S;
	#if 0
	float mAngleX=198.0f;
	float mAngleY=289.0f;
	#elif 1
	float mAngleX=198.0f;
	float mAngleY=264.0f;
	#endif
	stx_GetAngles(mAngleX, mAngleY);
	D3DXFROMWINEMatrixRotationYawPitchRoll(&R, mAngleX, mAngleY, 0.0f);
	//D3DXFROMWINEMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
	D3DXFROMWINEMatrixScaling(&S, 1.0f, 1.0f, 1.0f);
	W=R*S;
#if 1//ndef _MSC_VER
	{	
		IRenderer::GetRendererInstance()->setShader(shd);
        	IRenderer::GetRendererInstance()->setVertexFormat(vf);
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
        	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, g_verticesC_.size(), g_indices.size()/3, &g_indices[0], &g_indices[0], CONSTANT_INDEX2, &g_verticesC_[0], &g_verticesC_[0], sizeof(stx_VertexPositionColor));
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

