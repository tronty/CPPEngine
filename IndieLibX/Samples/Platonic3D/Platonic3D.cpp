/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <GUI/GUIUtils.h>
#include <FW3.h>

#if 0
D3DXFROMWINEMATRIX CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut, D3DXFROMWINEMATRIX* piMatrix, unsigned int sw)
{
	p_avOut[0] = D3DXFROMWINEVECTOR3( 1e10f, 1e10f, 1e10f);
	p_avOut[1] = D3DXFROMWINEVECTOR3( -1e10f, -1e10f, -1e10f);
	for( unsigned int a = 0; a < vertices.size();++a)
	{
		D3DXFROMWINEVECTOR3 pc1 = vertices[a].Position;
		p_avOut[0].x = std::min( p_avOut[0].x, pc1.x);
		p_avOut[0].y = std::min( p_avOut[0].y, pc1.y);
		p_avOut[0].z = std::min( p_avOut[0].z, pc1.z);
		p_avOut[1].x = std::max( p_avOut[1].x, pc1.x);
		p_avOut[1].y = std::max( p_avOut[1].y, pc1.y);
		p_avOut[1].z = std::max( p_avOut[1].z, pc1.z);
	}

	D3DXFROMWINEVECTOR3 vDelta = p_avOut[1]-p_avOut[0];
	D3DXFROMWINEVECTOR3 vHalf =  p_avOut[0] + (vDelta / 2.0f);
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

	if(sw)
	for( unsigned int j = 0; j < vertices.size();++j)
	{
		D3DXFROMWINEVec3TransformCoord(&vertices[j].Position, &vertices[j].Position, &m_mWorld);
		if(piMatrix)
			D3DXFROMWINEVec3TransformCoord(&vertices[j].Position, &vertices[j].Position, piMatrix);
	}
	if(piMatrix)
		m_mWorld*=(*piMatrix);
	return m_mWorld;
}

// Find a point's index from its letter.
int ToIndex(char ch)
{
            return ch - 'A';
}

        // Add a polygon to the indicated mesh.
        // Do not reuse old points but reuse these points.
        void AddPolygon(std::vector<D3DXFROMWINEVECTOR3>& points)
        {
            // Create the points.
            D3DXFROMWINEVECTOR3 n;
            int index1 = vertices.size();
            for(unsigned int i=0;i<points.size();i++)
            {
            	D3DXFROMWINEVec3Normalize(&n, &points[i]);
            D3DXFROMWINEVECTOR2 uv(points[i].x, points[i].y); // ???
            	stx_VertexPositionNormalTexture(points[i], n, uv);
                vertices.push_back(points[i]);
            }

            // Create the triangles.
            for (int i = 1; i < points.size() - 1; i++)
            {
                indices.push_back(index1);
                indices.push_back(index1 + i);
                indices.push_back(index1 + i + 1);
            }
        }

void AddPolygon(std::string& point_names, std::vector<D3DXFROMWINEVECTOR3>& points)
        {
            std::vector<D3DXFROMWINEVECTOR3> polygon_points(point_names.length());
            for (int i = 0; i < point_names.length(); i++)
            {
                polygon_points[i] = points[ToIndex((char)point_names.at(i))];
            }
            AddPolygon(polygon_points);
        }
#endif
	struct Tetrahedron : public MeshRenderer2
	{
		Tetrahedron(float sideLength=1.0f) : MeshRenderer2()
		{
			float X = sideLength/(2*sqrt(2));
			float Y = -X;
			
			D3DXFROMWINEVECTOR3 a(X,X,X);
			D3DXFROMWINEVECTOR3 b(Y,Y,X);
			D3DXFROMWINEVECTOR3 c(Y,X,Y);
			D3DXFROMWINEVECTOR3 d(X,Y,Y);
            
            		std::vector<D3DXFROMWINEVECTOR3> v;
            		v.push_back(a);
            		v.push_back(b);
            		v.push_back(c);
            		v.push_back(d);
			
			AddPolygon("ABC", v);
			AddPolygon("ABD", v);
			AddPolygon("ACD", v);
			AddPolygon("BCD", v);

        D3DXFROMWINEMATRIX S_, SH;
        D3DXFROMWINEVECTOR3 aSizeCoefficient(1.0f, 1.0f, 1.0f);
        D3DXFROMWINEMatrixScaling(&S_, aSizeCoefficient.x, aSizeCoefficient.y, aSizeCoefficient.z);
        D3DXFROMWINEMatrixScaling(&SH, 0.5f, 0.5f, 0.5f);
	for(unsigned int i=0;i<vertices.size();i++)
	{
		D3DXFROMWINEVec3Normalize(&vertices[i].Normal, &vertices[i].Position);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &S_);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &SH);
		stx_CalculatePositionalSphericalMapping(vertices[i]);
	}
		//D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    //computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
		}
	};
	struct Hexahedron : public MeshRenderer2
	{
		Hexahedron(float sideLength=1.0f) : MeshRenderer2()
		{
			float X = 0.5*sideLength;
			float Y = -0.5*sideLength;
			
			D3DXFROMWINEVECTOR3 a(Y,Y,Y);
			D3DXFROMWINEVECTOR3 b(X,Y,Y);
			D3DXFROMWINEVECTOR3 c(Y,X,Y);
			D3DXFROMWINEVECTOR3 d(Y,Y,X);
			D3DXFROMWINEVECTOR3 e(X,X,Y);
			D3DXFROMWINEVECTOR3 f(X,Y,X);
			D3DXFROMWINEVECTOR3 g(Y,X,X);
			D3DXFROMWINEVECTOR3 h(X,X,X);
            
            		std::vector<D3DXFROMWINEVECTOR3> v;
            		v.push_back(a);
            		v.push_back(b);
            		v.push_back(c);
            		v.push_back(d);
            		v.push_back(e);
            		v.push_back(f);
            		v.push_back(g);
            		v.push_back(h);

			AddPolygon("ABEC", v);
			AddPolygon("ABFD", v);
			AddPolygon("ACGD", v);
			AddPolygon("HFDG", v);
			AddPolygon("HEBF", v);
			AddPolygon("HECG", v);

        D3DXFROMWINEMATRIX S_, SH;
        D3DXFROMWINEVECTOR3 aSizeCoefficient(1.0f, 1.0f, 1.0f);
        D3DXFROMWINEMatrixScaling(&S_, aSizeCoefficient.x, aSizeCoefficient.y, aSizeCoefficient.z);
        D3DXFROMWINEMatrixScaling(&SH, 0.5f, 0.5f, 0.5f);
	for(unsigned int i=0;i<vertices.size();i++)
	{
		D3DXFROMWINEVec3Normalize(&vertices[i].Normal, &vertices[i].Position);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &S_);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &SH);
		stx_CalculatePositionalSphericalMapping(vertices[i]);
	}
		//D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    //computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
		}
	};
	struct Octahedron : public MeshRenderer2
	{
		Octahedron(float sideLength=1.0f) : MeshRenderer2()
		{
			float X = sideLength / sqrt(2);
			float Y = -X;

			D3DXFROMWINEVECTOR3 a(X, 0, 0);
			D3DXFROMWINEVECTOR3 b(0, X, 0);
			D3DXFROMWINEVECTOR3 c(0, 0, X);
			D3DXFROMWINEVECTOR3 d(Y, 0, 0);
			D3DXFROMWINEVECTOR3 e(0, Y, 0);
			D3DXFROMWINEVECTOR3 f(0, 0, Y);
            
            		std::vector<D3DXFROMWINEVECTOR3> v;
            		v.push_back(a);
            		v.push_back(b);
            		v.push_back(c);
            		v.push_back(d);
            		v.push_back(e);
            		v.push_back(f);

			AddPolygon("BAC", v);
			AddPolygon("BAF", v);
			AddPolygon("BCD", v);
			AddPolygon("BDF", v);
			AddPolygon("EFD", v);
			AddPolygon("EFA", v);
			AddPolygon("ECA", v);
			AddPolygon("ECD", v);

        D3DXFROMWINEMATRIX S_, SH;
        D3DXFROMWINEVECTOR3 aSizeCoefficient(1.0f, 1.0f, 1.0f);
        D3DXFROMWINEMatrixScaling(&S_, aSizeCoefficient.x, aSizeCoefficient.y, aSizeCoefficient.z);
        D3DXFROMWINEMatrixScaling(&SH, 0.5f, 0.5f, 0.5f);
	for(unsigned int i=0;i<vertices.size();i++)
	{
		D3DXFROMWINEVec3Normalize(&vertices[i].Normal, &vertices[i].Position);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &S_);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &SH);
		stx_CalculatePositionalSphericalMapping(vertices[i]);
	}
		//D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    //computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
		}
	};
	struct Dodecahedron : public MeshRenderer2
	{
		float root5 = sqrt(5);
		float phi = (1 + root5) / 2;
		float phibar = (1 - root5) / 2;
			
		Dodecahedron(float sideLength=1.0f) : MeshRenderer2()
		{
			float X = sideLength/(root5-1);
			float Y = X*phi;
			float Z = X*phibar;
			float S = -X;
			float T = -Y;
			float W = -Z;
			
			D3DXFROMWINEVECTOR3 a(X,X,X); // 0	a
			D3DXFROMWINEVECTOR3 b(X,X,S); // 1	b
			D3DXFROMWINEVECTOR3 c(X,S,X); // 2	c
			D3DXFROMWINEVECTOR3 d(X,S,S); // 3	d
			D3DXFROMWINEVECTOR3 e(S,X,X); // 4	e
			D3DXFROMWINEVECTOR3 f(S,X,S); // 5	f
			D3DXFROMWINEVECTOR3 g(S,S,X); // 6	g
			D3DXFROMWINEVECTOR3 h(S,S,S); // 7	h
			D3DXFROMWINEVECTOR3 i(W,Y,0); // 8	i
			D3DXFROMWINEVECTOR3 j(Z,Y,0); // 9	j
			D3DXFROMWINEVECTOR3 k(W,T,0); // 10	k
			D3DXFROMWINEVECTOR3 l(Z,T,0); // 11	l
			D3DXFROMWINEVECTOR3 m(Y,0,W); // 12	m
			D3DXFROMWINEVECTOR3 n(Y,0,Z); // 13	n
			D3DXFROMWINEVECTOR3 o(T,0,W); // 14	o
			D3DXFROMWINEVECTOR3 p(T,0,Z); // 15	p
			D3DXFROMWINEVECTOR3 q(0,W,Y); // 16	q
			D3DXFROMWINEVECTOR3 r(0,Z,Y); // 17	r
			D3DXFROMWINEVECTOR3 s(0,W,T); // 18	s
			D3DXFROMWINEVECTOR3 t(0,Z,T); // 19	t
            
            		std::vector<D3DXFROMWINEVECTOR3> v;
            		v.push_back(a);
            		v.push_back(b);
            		v.push_back(c);
            		v.push_back(d);
            		v.push_back(e);
            		v.push_back(f);
            		v.push_back(g);
            		v.push_back(h);
            		v.push_back(i);
            		v.push_back(j);
            		v.push_back(k);
            		v.push_back(l);
            		v.push_back(m);
            		v.push_back(n);
            		v.push_back(o);
            		v.push_back(p);
            		v.push_back(q);
            		v.push_back(r);
            		v.push_back(s);
            		v.push_back(t);
			
			AddPolygon("BIAMN", v);
			AddPolygon("EJFPO", v);
			AddPolygon("CKDNM", v);
			AddPolygon("HLGOP", v);
			AddPolygon("CMAQR", v);
			AddPolygon("BNDTS", v);
			AddPolygon("EOGRQ", v);
			AddPolygon("HPFST", v);
			AddPolygon("EQAIJ", v);
			AddPolygon("CRGLK", v);
			AddPolygon("BSFJI", v);
			AddPolygon("HTDKL", v);

        D3DXFROMWINEMATRIX S_, SH;
        D3DXFROMWINEVECTOR3 aSizeCoefficient(1.0f, 1.0f, 1.0f);
        D3DXFROMWINEMatrixScaling(&S_, aSizeCoefficient.x, aSizeCoefficient.y, aSizeCoefficient.z);
        D3DXFROMWINEMatrixScaling(&SH, 0.5f, 0.5f, 0.5f);
	for(unsigned int i=0;i<vertices.size();i++)
	{
		D3DXFROMWINEVec3Normalize(&vertices[i].Normal, &vertices[i].Position);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &S_);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &SH);
		stx_CalculatePositionalSphericalMapping(vertices[i]);
	}
		//D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    //computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
		}
	};
	struct Icosahedron : public MeshRenderer2
	{
		float root5 = sqrt(5);
		
		Icosahedron(float sideLength=1.0f) : MeshRenderer2()
		{
			float n = sideLength/2;
			float X = n*(1+root5)/2;
			float Y = -X;
			float Z = n;
			float W = -n;
			
			D3DXFROMWINEVECTOR3 a(X,Z,0);
			D3DXFROMWINEVECTOR3 b(Y,Z,0);
			D3DXFROMWINEVECTOR3 c(X,W,0);
			D3DXFROMWINEVECTOR3 d(Y,W,0);
			D3DXFROMWINEVECTOR3 e(Z,0,X);
			D3DXFROMWINEVECTOR3 f(Z,0,Y);
			D3DXFROMWINEVECTOR3 g(W,0,X);
			D3DXFROMWINEVECTOR3 h(W,0,Y);
			D3DXFROMWINEVECTOR3 i(0,X,Z);
			D3DXFROMWINEVECTOR3 j(0,Y,Z);
			D3DXFROMWINEVECTOR3 k(0,X,W);
			D3DXFROMWINEVECTOR3 l(0,Y,W);
            
            		std::vector<D3DXFROMWINEVECTOR3> v;
            		v.push_back(a);
            		v.push_back(b);
            		v.push_back(c);
            		v.push_back(d);
            		v.push_back(e);
            		v.push_back(f);
            		v.push_back(g);
            		v.push_back(h);
            		v.push_back(i);
            		v.push_back(j);
            		v.push_back(k);
            		v.push_back(l);
			
			AddPolygon("AIE", v);
			AddPolygon("AFK", v);
			AddPolygon("CEJ", v);
			AddPolygon("CLF", v);
			AddPolygon("BGI", v);
			AddPolygon("BKH", v);
			AddPolygon("DJG", v);
			AddPolygon("DHL", v);
			AddPolygon("AKI", v);
			AddPolygon("BIK", v);
			AddPolygon("CJL", v);
			AddPolygon("DLJ", v);
			AddPolygon("ECA", v);
			AddPolygon("FAC", v);
			AddPolygon("GBD", v);
			AddPolygon("HDB", v);
			AddPolygon("IGE", v);
			AddPolygon("JEG", v);
			AddPolygon("KFH", v);
			AddPolygon("LHF", v);

        D3DXFROMWINEMATRIX S_, SH;
        D3DXFROMWINEVECTOR3 aSizeCoefficient(1.0f, 1.0f, 1.0f);
        D3DXFROMWINEMatrixScaling(&S_, aSizeCoefficient.x, aSizeCoefficient.y, aSizeCoefficient.z);
        D3DXFROMWINEMatrixScaling(&SH, 0.5f, 0.5f, 0.5f);
	for(unsigned int i=0;i<vertices.size();i++)
	{
		D3DXFROMWINEVec3Normalize(&vertices[i].Normal, &vertices[i].Position);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &S_);
		D3DXFROMWINEVec3TransformCoord(&vertices[i].Position, &vertices[i].Position, &SH);
		stx_CalculatePositionalSphericalMapping(vertices[i]);
	}
		//D3DXFROMWINEVECTOR3 aiVecs[2];D3DXFROMWINEMATRIX m;D3DXFROMWINEMatrixIdentity(&m);CalculateBounds(&aiVecs[0], &m);
	    //computeTangentSpace(&vertices[0], sizeof(vertices[0]), vertices.size());
		}		
	};

#define defaultshader eSimpleShader
//define alternativeshader eAmbientLighting
//define alternativeshader eDiffuseLighting
//define alternativeshader ePhongLighting1
//define alternativeshader ePhongLighting2
#define alternativeshader defaultshader

class _Shape3D: public STXGUI
{
	ShaderID shd;
	VertexFormatID vf;
	TextureID texID;
	MeshRenderer2 shape3D[5];
	unsigned int m_i, m_s;
public:
int init(const char* aTitle)
{     	
	m_s=0;      	
	m_i=0;
        shape3D[0]=Tetrahedron();
        shape3D[1]=Hexahedron();
        shape3D[2]=Octahedron();
        shape3D[3]=Icosahedron();
        shape3D[4]=Dodecahedron();

	STXGUI::init("/Platonic3D/GUILayout.xml");

	SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
	texID=IRenderer::GetRendererInstance()->addImageLibTexture("/test.bmp", false, ss);

	shd = IRenderer::GetRendererInstance()->addShaderFromFile(	"/MeshRenderer2/rest.HLSL.shd", "main", "main");
	FormatDesc fmt[] = { 
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2 };
	vf = IRenderer::GetRendererInstance()->addVertexFormat(fmt, elementsOf(fmt), shd);

	return 0;
}

void render( )
{
	float f=128.0f/256.0f;
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4 (f, f, f, 1.0f));
	IRenderer::GetRendererInstance()->BeginScene();

	float mAngleX=45.0f;
	float mAngleY=45.0f;
	stx_GetAngles(mAngleX, mAngleY);

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

	IRenderer::GetRendererInstance()->setShader(shd);
	IRenderer::GetRendererInstance()->setVertexFormat(vf);
    IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matRot );
		IRenderer::GetRendererInstance()->setTexture("DIFFUSE_SAMPLER", texID);
		IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, shape3D[m_i].vertices.size(), shape3D[m_i].indices.size()/3, &shape3D[m_i].indices[0], &shape3D[m_i].indices[0], CONSTANT_INDEX2, &shape3D[m_i].vertices[0], &shape3D[m_i].vertices[0], sizeof(shape3D[m_i].vertices[0]));
	
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
         	if(callbackString == "Tetrahedron")
      		{
			m_i = 0;
		}
         	if(callbackString == "Hexahedron")
      		{
			m_i = 1;
		}
         	if(callbackString == "Octahedron")
      		{
			m_i = 2;
		}
		if(callbackString == "Icosahedron")
      		{
			m_i = 3;
		}
		if(callbackString == "Dodecahedron")
      		{
			m_i = 4;
		}
	}
	}
}
};
int ApplicationLogic()
{	
	IRenderer* r=IRenderer::GetRendererInstance("Platonic3D");	
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

