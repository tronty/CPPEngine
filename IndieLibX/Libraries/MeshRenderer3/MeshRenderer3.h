#ifndef __MeshRenderer3___
#define __MeshRenderer3___
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>

typedef enum _tShader
{
	eShaderNone,
	eHLSL_Shader,
	eGLSL_Shader,
	eGLES_Shader_android,
	eGLES_Shader_Ubuntu,
		eSimpleShader,
		eSimpleShader2,
		eAmbientLighting,
		eDiffuseLighting,
		ePhongLighting1,
		ePhongLighting2,
		ePositionTexture,
		ePositionNormal,
		ePositionNormalTexture,
		ePositionNormalNormal,
		eCelShader,
		eDirectionalLighting,
		eSpotLighting,
		eMultiLighting,
		eSnowShader,
		eCelShader1,
		eCelShader2,
		eCelShader3,
		eCelShader4
} tShader;
inline const char* GetShaderName(tShader aShader_)
{
	switch(aShader_)
	{
		case ePositionTexture:
			return "PositionTextureShader";
		case ePositionNormal:
			return "PositionNormalShader";
		case eSimpleShader:
			return "SimpleShader";
		case eSimpleShader2:
			return "SimpleShader2";
		case eAmbientLighting:
			return "AmbientLighting";
		case eDiffuseLighting:
			return "DiffuseLighting";
		case ePhongLighting1:
			return "PhongLighting1";
		case ePhongLighting2:
			return "PhongLighting2";
		case eCelShader:
			return "CelShader";
		case eSnowShader:
			return "SnowShader";
		case eDirectionalLighting:
			return "DirectionalLighting";
		case eSpotLighting:
			return "SpotLighting";
		case eMultiLighting:
			return "MultiLighting";
		case eCelShader1:
			return "CelShader1";
		case eCelShader2:
			return "CelShader2";
		case eCelShader3:
			return "CelShader3";
		case eCelShader4:
			return "CelShader4";
		default:
			return "SimpleShader";
	}
	return "SimpleShader";
}

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
struct MeshRenderer3
{
public:
	struct Animator
	{
		Animator():mRootNode(0), mScene(0){}
		void SetAnimIndex(unsigned int pAnimIndex){}
		unsigned int CurrentAnimIndex(){return 0;}
		int GetNumAnimations(){return 0;}
		void* mRootNode;
		void* mScene;
	};
	#if 1
	struct MeshHelper
	{
		MeshHelper():mAnimator(0), apcMeshes(0){}
		~MeshHelper(){}
		Animator* mAnimator;
		MeshHelper** apcMeshes;
	};
	#endif
	MeshRenderer3();
	virtual ~MeshRenderer3();
	int Load(const char* aFilename=0, tShader aShader=eSimpleShader, bool scaleasset=true, bool a_bMeshRenderer3Simpler_ProceduralBuilding=false);
	int LoadFile(const char* aFilename=0, tShader aShader=eSimpleShader, bool scaleasset=true, bool a_bMeshRenderer3Simpler_ProceduralBuilding=false);
	void render(const D3DXFROMWINEMATRIX* w=0);
	void render_(const D3DXFROMWINEMATRIX* w=0);
	void render(
		float a_fTX,float a_fTY,float a_fTZ,
		float a_fSX,float a_fSy,float a_fSZ,
		float a_fSpinX,
		float a_fSpinY,
		float a_fSpinZ);
	void SetAmbientColor(D3DXFROMWINEVECTOR4 a);
	void SetDiffuseColor(D3DXFROMWINEVECTOR4 a);
	void SetLightDir(D3DXFROMWINEVECTOR4 a);
	void SetEyeDir(D3DXFROMWINEVECTOR4 a);
	void GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max);
	void SetPlay(bool a);
	void SetAnimIndex( unsigned int pAnimIndex);
	size_t GetCurrentAnimIndex();
	size_t GetNumAnimations();
	std::vector<stx_Mesh> meshes;
protected:
	//aiScene* pcScene;
	Animator* mAnimator;
	static ShaderID AmbientShader;
	static ShaderID DiffuseShader;
	static ShaderID PhongShader1;
	static ShaderID PhongShader2;
	static ShaderID Shader;
	static VertexFormatID AmbientVertexDeclaration;
	static VertexFormatID DiffuseVertexDeclaration;
	static VertexFormatID PhongVertexDeclaration1;
	static VertexFormatID PhongVertexDeclaration2;
        static VertexFormatID VertexDeclaration;

	static ShaderID SimpleShader;
	static VertexFormatID SimpleVertexDeclaration;
	static ShaderID OutlineShader;
	static VertexFormatID OutlineVertexDeclaration;
	static ShaderID ShadingShader;
	static VertexFormatID ShadingVertexDeclaration;

	static ShaderID CelShader;
	static VertexFormatID CelVertexDeclaration;
	static ShaderID CelShader1;
	static VertexFormatID CelVertexDeclaration1;
	static ShaderID CelShader2;
	static VertexFormatID CelVertexDeclaration2;
	static ShaderID CelShader3;
	static VertexFormatID CelVertexDeclaration3;
	static ShaderID CelShader4;
	static VertexFormatID CelVertexDeclaration4;

	static ShaderID DirectionalShader;
	static VertexFormatID DirectionalVertexDeclaration;
	static ShaderID SpotShader;
	static VertexFormatID SpotVertexDeclaration;
	static ShaderID MultiShader;
	static VertexFormatID MultiVertexDeclaration;
	static ShaderID SnowShader;
	static VertexFormatID SnowVertexDeclaration;
	static TextureID ExposureDepthMapTexture;
	static TextureID ShadingTexture;
	static TextureID CelShade;

#endif

	std::string m_sFileName;
	std::string m_szPathTex;
	bool m_bMeshRenderer3Simpler_ProceduralBuilding;
	tShader mShader;
	D3DXFROMWINEVECTOR4 m_v4AmbientColor, m_v4DiffuseColor, m_v4LightDir, m_v4EyeDir;
	D3DXFROMWINEMATRIX m_mWorld;

	float m_fSpinX, m_fSpinY, m_fSpinZ,m_fSX,m_fSy,m_fSZ,m_fTX,m_fTY,m_fTZ;

	// ??? MeshHelper * GetAsset(){return 0;}

	const char* GetFileName();
	void SetRenderingMode(unsigned int i=0);
	void GetBoundingSphereRadius(float& r);
	void SetTX(float aX);
	void SetTY(float aY);
	void SetTZ(float aZ);
	void SetSX(float aScaleX);
	void SetSY(float aScaleY);
	void SetSZ(float aScaleZ);
	void SetSpinX(float aAngleX);
	void SetSpinY(float aAngleY);
	void SetSpinZ(float aAngleZ);

	int CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut, const D3DXFROMWINEMATRIX& piMatrix);
    	D3DXFROMWINEVECTOR3 GetMeshBBoxExtents( unsigned int );
    	D3DXFROMWINEVECTOR3 GetMeshBBoxCenter( unsigned int );
	D3DXFROMWINEMATRIX ScaleAsset(void);
	void initrindices();
	int InitShader(tShader aShader=eSimpleShader);
	int LoadXFile(const char* aFilename);
	int LoadDAEFile(const char* aFilename);
	int Load3DSFile(const char* aFilename);
	int LoadOBJFile(const char* aFilename);
	int LoadSDKMESHFile(const char* aFilename);
	int LoadAsset(const char* filename, bool scaleasset=true);
};

struct MeshRenderer3
{
public:
	struct Animator
	{
		Animator():mRootNode(0), mScene(0){}
		void SetAnimIndex(unsigned int pAnimIndex){}
		unsigned int CurrentAnimIndex(){return 0;}
		int GetNumAnimations(){return 0;}
		void* mRootNode;
		void* mScene;
	};
	#if 1
	struct MeshHelper
	{
		MeshHelper():mAnimator(0), apcMeshes(0){}
		~MeshHelper(){}
		Animator* mAnimator;
		MeshHelper** apcMeshes;
	};
	#endif
	MeshRenderer3();
	virtual ~MeshRenderer3();
	int Load(const char* aFilename=0, tShader aShader=eSimpleShader, bool scaleasset=true, bool a_bMeshRenderer3Simpler_ProceduralBuilding=false);
	int LoadFile(const char* aFilename=0, tShader aShader=eSimpleShader, bool scaleasset=true, bool a_bMeshRenderer3Simpler_ProceduralBuilding=false);


	void ChangeShader(D3DXFROMWINEMATRIX* mat=0, TextureID id=-1, ShaderID shdid=-1, VertexFormatID vfid=-1, 
			D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
	//unsigned int render(D3DXFROMWINEMATRIX* mat=0){Draw(mat);} // ambiguous 
	void render(const D3DXFROMWINEMATRIX* w=0);
        void BeginDraw(	D3DXFROMWINEMATRIX* mat=0, TextureID id=-1, ShaderID shdid=-1, VertexFormatID vfid=-1, 
			D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));

	unsigned int EndDraw(const unsigned int flags=RESET_ALL);





	void render(
		float a_fTX,float a_fTY,float a_fTZ,
		float a_fSX,float a_fSy,float a_fSZ,
		float a_fSpinX,
		float a_fSpinY,
		float a_fSpinZ);
	void render_(const D3DXFROMWINEMATRIX* w=0);
        unsigned int Draw(	D3DXFROMWINEMATRIX* mat=0, TextureID id=-1, ShaderID shdid=-1, VertexFormatID vfid=-1, 
			D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));


	void SetAmbientColor(D3DXFROMWINEVECTOR4 a);
	void SetDiffuseColor(D3DXFROMWINEVECTOR4 a);
	void SetLightDir(D3DXFROMWINEVECTOR4 a);
	void SetEyeDir(D3DXFROMWINEVECTOR4 a);
	void GetBounds(D3DXFROMWINEVECTOR3& ____min, D3DXFROMWINEVECTOR3& ____max);
	void SetPlay(bool a);
	void SetAnimIndex( unsigned int pAnimIndex);
	size_t GetCurrentAnimIndex();
	size_t GetNumAnimations();



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

	void SetTX(float aX);
	void SetTY(float aY);
	void SetTZ(float aZ);
	void SetSX(float aScaleX);
	void SetSY(float aScaleY);
	void SetSZ(float aScaleZ);
	void SetSpinX(float aAngleX);
	void SetSpinY(float aAngleY);
	void SetSpinZ(float aAngleZ);
	void SetRenderingMode(unsigned int i=0);
	void GetBoundingSphereRadius(float& r);

	const char* GetFileName();

	stx_VertexPositionNormalTexture* getVertices(){return GetVertices();}
	stx_VertexPositionNormalTexture* GetVertices()
	{
		if(meshes.size())
			return &meshes[0].vertices[0];
		return 0;
	}
	__WORD__* GetIndices()
	{
		if(meshes.size())
			return &meshes[0].indices[0];
		return 0;
	}
	void toString()
	{
		if(meshes.size())
		{
			LOG_PRINT("vertices.size()=%d\n", meshes[0].vertices.size());
			LOG_PRINT("indices.size()=%d\n", meshes[0].indices.size());
		}
	}
	void Clear()
	{
		meshes.clear();
		vertices.clear();
		indices.clear();
	}

	std::vector<stx_Mesh> meshes;
	std::vector<stx_VertexPositionNormalTexture> vertices;
	std::vector<__WORD__> indices;
	//aiScene* pcScene;
	Animator* mAnimator;
protected:
	static ShaderID AmbientShader;
	static ShaderID DiffuseShader;
	static ShaderID PhongShader1;
	static ShaderID PhongShader2;
	static ShaderID Shader;
	static VertexFormatID AmbientVertexDeclaration;
	static VertexFormatID DiffuseVertexDeclaration;
	static VertexFormatID PhongVertexDeclaration1;
	static VertexFormatID PhongVertexDeclaration2;
        static VertexFormatID VertexDeclaration;

	static ShaderID SimpleShader;
	static VertexFormatID SimpleVertexDeclaration;
	static ShaderID OutlineShader;
	static VertexFormatID OutlineVertexDeclaration;
	static ShaderID ShadingShader;
	static VertexFormatID ShadingVertexDeclaration;

	static ShaderID DirectionalShader;
	static VertexFormatID DirectionalVertexDeclaration;
	static ShaderID SpotShader;
	static VertexFormatID SpotVertexDeclaration;
	static ShaderID MultiShader;
	static VertexFormatID MultiVertexDeclaration;

	static ShaderID SnowShader;
	static VertexFormatID SnowVertexDeclaration;
	static TextureID MeshTexture;
	static TextureID ExposureDepthMapTexture;
	static TextureID ShadingTexture;
	std::string m_sFileName;
	std::string m_szPathTex;
	bool m_bMeshRenderer3Simpler_ProceduralBuilding;
	tShader mShader;
	D3DXFROMWINEVECTOR4 m_v4AmbientColor, m_v4DiffuseColor, m_v4LightDir, m_v4EyeDir;
	D3DXFROMWINEMATRIX m_mWorld;

	float m_fSpinX, m_fSpinY, m_fSpinZ,m_fSX,m_fSy,m_fSZ,m_fTX,m_fTY,m_fTZ;

	// ??? MeshHelper * GetAsset(){return 0;}

	int CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut, const D3DXFROMWINEMATRIX& piMatrix);
    	D3DXFROMWINEVECTOR3 GetMeshBBoxExtents( unsigned int );
    	D3DXFROMWINEVECTOR3 GetMeshBBoxCenter( unsigned int );
	D3DXFROMWINEMATRIX ScaleAsset();
	void initrindices();
	int InitShader(tShader aShader=eSimpleShader);
	int LoadXFile(const char* aFilename);
	int LoadDAEFile(const char* aFilename);
	int Load3DSFile(const char* aFilename);
	int LoadOBJFile(const char* aFilename);
	int LoadSDKMESHFile(const char* aFilename);
	int LoadAsset(const char* filename, bool scaleasset=true);






	protected:
	int j_tmp, z_tmp;
	D3DXFROMWINEMATRIX w_tmp;
	//BoundsDescriptor aabb;
	TextureID texID, tex_tmp;
	tShape m_Shape;
	D3DXFROMWINEVECTOR3 _max, _min;
        /// <summary>
        /// The number of vertices making up the shape.
        /// </summary>
	unsigned int getVertexCount();

	D3DXFROMWINEVECTOR3 getMin(){return _min;}
	D3DXFROMWINEVECTOR3 getMax(){return _max;}

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
	void CreateCombinedShape(const std::vector<MeshRenderer3>& av, const std::vector<D3DXFROMWINEMATRIX>& aW);
	
};
#endif

