/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#if 0//ndef __Shape3D__
#define __Shape3D__

//include "BoundsDescriptor.h"

	enum tShape
	{
		eShapeNone,
		eTexturedBox,
		eBox,
		eTexturedQuad,
		eSphere,
		eHalfSphere,
		eCylinder,
		//eGrid,
		eCone,
		eOcta,
		eTetra,
		eEllipsoid,
#if 1
		eHemis,
		eTorus,

#endif
/*
		Hexahedron (cube)
		Tetrahedron
		Octahedron
		Icosahedron
		Dodecahedron
*/
		eMesh
	};

    /// <summary>
    /// Defines a simple 3D shape, such as a box, sphere, or cylinder.
    /// </summary>
    class MeshRenderer2
    {
	public:
	MeshRenderer2();
	virtual ~MeshRenderer2()
	{
		vertices.clear();
		indices.clear();
	}
	MeshRenderer2(const MeshRenderer2& rhs)
	{
		m_Shape=rhs.m_Shape;
		vertices=rhs.vertices;
		indices=rhs.indices;
		_max=rhs._max;
		_min=rhs._min;
		texID=rhs.texID;
		Shader=rhs.Shader;
        	VertexDeclaration=rhs.VertexDeclaration;
	}
        void CreateTexturedBox(float width=2.0f, float height=2.0f, float depth=2.0f, tShader aShader=eShaderNone);
	void CreateTexturedQuad(float width=2.0f, float height=2.0f, tShader aShader=eShaderNone);
        void CreateBox(float width=2.0f, float height=2.0f, float depth=2.0f, tShader aShader=eShaderNone);
        void CreateSphere(float radius=1.0f, tShader aShader=eShaderNone);
        void CreateSphere_(float radius=1.0f, int aslices=20, int stacks=20, tShader aShader=eShaderNone);
        void CreateHalfSphere(float radius=1.0f, tShader aShader=eShaderNone);
        void CreateCone(float bottomRadius=1.0f, float height=1.0f, tShader aShader=eShaderNone);
#if 1
	void CreateHemis(tShader aShader=eShaderNone);
	void CreateTorus(float radMajor=1.0f, float radMinor=1.0f, tShader aShader=eShaderNone);
#endif
	void CreateOcta(tShader aShader=eShaderNone);
	void CreateTetra(tShader aShader=eShaderNone);
	void CreateEllipsoid(float fA=1.0f, float fB=1.0f, float fC=1.0f, tShader aShader=eShaderNone);
        void CreateCylinder(float bottomRadius=1.0f, float topRadius=1.0f, float length=2.0f, tShader aShader=eShaderNone);
	void CreateMesh(const char* afn);
	void GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max);

        unsigned int Draw(	D3DXFROMWINEMATRIX* mat=0, TextureID id=-1, ShaderID shdid=-1, VertexFormatID vfid=-1, 
			D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
        void BeginDraw(	D3DXFROMWINEMATRIX* mat=0, TextureID id=-1, ShaderID shdid=-1, VertexFormatID vfid=-1, 
			D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));

	unsigned int EndDraw(const unsigned int flags=RESET_ALL);

	unsigned int render(D3DXFROMWINEMATRIX* mat=0){Draw(mat);}
	std::vector<stx_VertexPositionNormalTexture> vertices;
	std::vector<__WORD__> indices;
	ShaderID Shader;
        VertexFormatID VertexDeclaration;
	protected:
	//BoundsDescriptor aabb;
	TextureID texID;
	tShape m_Shape;
	D3DXFROMWINEVECTOR3 _max, _min;
        /// <summary>
        /// The number of vertices making up the shape.
        /// </summary>
	unsigned int getVertexCount();
	stx_VertexPositionNormalTexture* getVertices(){return &vertices[0];}

	D3DXFROMWINEVECTOR3 getMin(){return _min;}
	D3DXFROMWINEVECTOR3 getMax(){return _max;}
	D3DXFROMWINEMATRIX ScaleAsset();

        /// <summary>
        /// The number of triangle making up the shape.
        /// </summary>
	unsigned int getTriangleCount();

        /// <summary>
        /// The declaration of the type of vertices making up the shape.
        /// </summary>
	VertexFormatID getVertexDeclaration();
	void setVertexDeclaration(VertexFormatID aVertexDeclaration);

        /// <summary>
        /// The byte size of each vertex making up the shape.
        /// </summary>

	#if 0
	unsigned int getVertexSizeInBytes(){return sizeof(vertices[0]);}
	void setVertexSizeInBytes(unsigned int aVertexSizeInBytes){}
	#endif

	void computeNormals(void* aVertices, unsigned int aVertexSizeInBytes, unsigned int avertexCount, bool aSmoothGroup=false);
	void computeTangentSpace(void* aVertices, unsigned int aVertexSizeInBytes, unsigned int avertexCount, bool aSmoothGroup=false);

        D3DXFROMWINEVECTOR3 GetMeshBBoxExtents();

	D3DXFROMWINEVECTOR3 GetMeshBBoxCenter();

	D3DXFROMWINEMATRIX CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut=0, D3DXFROMWINEMATRIX* piMatrix=0);
	void Grid(std::vector<stx_VertexPositionNormalTexture>& vertices, int gridX, int gridZ);

	void setFace(unsigned int c, unsigned int i1, unsigned int i2, unsigned int i3);
	void makeGrid(int gridX, int gridZ);
        void CreateIndexBuffer(unsigned int vertexCount, unsigned int indexCount, unsigned int slices, bool abBottom=true);
	/* 	a cube inside the mesh ?
		test all cube corners ray-triange collision test
		cast a ray from a cube corner to the opposite cube corners
		inside or surface cube (neigbors list size 6?)
Triangulate the points to make a closed tesselated volume (ask again if you don't know how). Then fire a "ray" from your test point, and count its intersections with the set of triangles. If the number of intersections is odd, the test point is inside the volume.
	*/
	void RemoveHiddenTriangles(const std::vector<D3DXFROMWINEVECTOR3>& minmax);
	void CreateCombinedShape(const std::vector<MeshRenderer2>& av, const std::vector<D3DXFROMWINEMATRIX>& aW);
	void toString()
	{
		LOG_PRINT("vertices.size()=%d\n", vertices.size());
		LOG_PRINT("indices.size()=%d\n", indices.size());
	}

	};
D3DXFROMWINEVECTOR2 CalculatePositionalSphericalMapping(stx_VertexPositionNormalTexture& a);
struct FW3ShadersFactory
{
protected:
    	static std::map<std::string, ShaderID> m_FW3ShadersFactory_Shadercache;
	static ShaderID find_from_FW3ShadersFactory_Shadercache(const char* aShaderName);
	static void insert_to_FW3ShadersFactory_Shadercache(const char* aShaderName, ShaderID id);
    	static std::map<tShape, VertexFormatID> m_FW3ShadersFactory_VertexFormatcache;
	static VertexFormatID find_from_FW3ShadersFactory_VertexFormatcache(tShape aShape);
	static void insert_to_FW3ShadersFactory_VertexFormatcache(tShape aShape, VertexFormatID id);
public:
	static ShaderID GetShader(const char* aShaderName, const char *vsName, const char *psName);
	static VertexFormatID GetVertexFormat(tShape aShape, const ShaderID shd);
	static void GetShaderTxt(std::string aShaderName, std::string& contents);
};

inline void stx_InitShape3D(	const std::string& init_file,
				std::vector<std::string>& ShaderFiles,
				std::vector<ShaderID>& shd,
				std::vector<VertexFormatID>& vf,
				std::vector<std::string>& TextureFiles,
				std::vector<TextureID>& tex)
{
	std::string fn=stx_convertpath(init_file);
    	TiXmlDocument doc(fn.c_str());
    	if (!doc.LoadFile())
	{
		LOG_PRINT("Reading file %s failed!\n", fn.c_str());
      		stx_exit(0);
	}
	else
	{
		LOG_PRINT("Reading file %s succeeded!\n", fn.c_str());
	}
    	TiXmlHandle docHandle(&doc);
    	TiXmlNode* BodyNode = docHandle.FirstChild("Body").Node();
	TiXmlHandle TexturesHandle = BodyNode->FirstChild("Textures");
	TiXmlHandle TextureHandle = TexturesHandle.FirstChild("Texture");
	TiXmlElement* TextureElement = TextureHandle.Element();
	int i=1;
	while ( TextureElement )
	{
		std::string TextureFile=TextureElement->Attribute("File");
		LOG_PRINT("%d TextureFile=%s\n", i, TextureFile.c_str());
		TextureFiles.push_back(TextureFile);
		tex.push_back(IRenderer::GetRendererInstance()->addImageLibTexture(TextureFile.c_str(),false, IRenderer::GetRendererInstance()->Getlinear()));
		TextureHandle = TexturesHandle.Child( "Texture", i++ );
		TextureElement = TextureHandle.Element();
	}
	TiXmlHandle ShadersHandle = BodyNode->FirstChild("Shaders");
	TiXmlHandle ShaderHandle = ShadersHandle.FirstChild("Shader");
	TiXmlElement* ShaderElement = ShaderHandle.Element();
	i=1;
	while ( ShaderElement )
	{
		std::string ShaderFile=ShaderElement->Attribute("File");
		LOG_PRINT("%d ShaderFile=%s\n", i, ShaderFile.c_str());
		ShaderFiles.push_back(ShaderFile);
		shd.push_back(IRenderer::GetRendererInstance()->addShaderFromFile(ShaderFile.c_str(), "main", "main"));
		FormatDesc format[] =
		{
					0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
					0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
					0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
					0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
		};
		vf.push_back(IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd[shd.size()-1]));
		ShaderHandle = ShadersHandle.Child( "Shader", i++ );
		ShaderElement = ShaderHandle.Element();
	}
}
#endif

