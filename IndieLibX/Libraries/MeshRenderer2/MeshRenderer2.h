#ifndef __MeshRenderer2___
#define __MeshRenderer2___
/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#if 1
// assimp include files. These three are usually needed.
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#endif

#include <time.h>

#include "AssetHelper.h"
#include "AnimEvaluator.h"
#include "SceneAnimator.h"
using namespace AssimpView;

typedef unsigned int SDKMESH_PRIMITIVE_TYPE;
struct SDKMESH_SUBSET
{
	SDKMESH_SUBSET(){}
	virtual ~SDKMESH_SUBSET(){}
	unsigned int IndexCount;
	unsigned int IndexStart;
	unsigned int VertexStart;
	unsigned int MaterialID;
	SDKMESH_PRIMITIVE_TYPE PrimitiveType;
};
struct SDKMESH_MATERIAL
{
	SDKMESH_MATERIAL(){}
	virtual ~SDKMESH_MATERIAL(){}
	TextureID pDiffuseRV10;
	TextureID pNormalRV10;
};
struct MeshRenderer2_
{
public:
	#if 0
	struct SceneAnimator
	{
		SceneAnimator():mRootNode(0), mScene(0){}
		void SetAnimIndex(unsigned int pAnimIndex){}
		unsigned int CurrentAnimIndex(){return 0;}
		int GetNumAnimations(){return 0;}
		void* mRootNode;
		void* mScene;
	};
	struct MeshHelper
	{
		MeshHelper():m_pAnimator(0), apcMeshes(0){}
		~MeshHelper(){}
		SceneAnimator* m_pAnimator;
        AssetHelper* m_pcAsset;
		MeshHelper** apcMeshes;
	};
	#endif
	MeshRenderer2_();
	virtual ~MeshRenderer2_();
	void Render(stx_Effect::stx_Technique* pTechnique, stx_Effect::stx_Variable* g_ptxDiffuse);
#if 1
	void Render( stx_Effect::stx_Technique* pTechnique, const D3DXFROMWINEMATRIX* aw=0);
#else
	void Render( stx_Effect::stx_Technique* pTechnique, const char* awName=0, const D3DXFROMWINEMATRIX* aw=0 );
#endif
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
    bool Import3DFromFile( const std::string& pFile);
    //Assimp::Importer importer;
	//aiScene* pcScene;
	SceneAnimator* m_pAnimator;
    AssetHelper* m_pcAsset;
	ShaderID AmbientShader;
	ShaderID DiffuseShader;
	ShaderID PhongShader1;
	ShaderID PhongShader2;
	ShaderID Shader;
	VertexFormatID AmbientVertexDeclaration;
	VertexFormatID DiffuseVertexDeclaration;
	VertexFormatID PhongVertexDeclaration1;
	VertexFormatID PhongVertexDeclaration2;
        VertexFormatID VertexDeclaration;
        VertexFormatID _VertexDeclaration;
	ShaderID _Shader;
	ShaderID PhysicalBasedShader;
        VertexFormatID PhysicalBasedVertexDeclaration;

	ShaderID SimpleShader;
	VertexFormatID SimpleVertexDeclaration;
	ShaderID OutlineShader;
	VertexFormatID OutlineVertexDeclaration;
	ShaderID ShadingShader;
	VertexFormatID ShadingVertexDeclaration;

	ShaderID CelShader;
	VertexFormatID CelVertexDeclaration;
	ShaderID CelShader1;
	VertexFormatID CelVertexDeclaration1;
	ShaderID CelShader2;
	VertexFormatID CelVertexDeclaration2;
	ShaderID CelShader3;
	VertexFormatID CelVertexDeclaration3;
	ShaderID CelShader4;
	VertexFormatID CelVertexDeclaration4;

	ShaderID DirectionalShader;
	VertexFormatID DirectionalVertexDeclaration;
	ShaderID SpotShader;
	VertexFormatID SpotVertexDeclaration;
	ShaderID MultiShader;
	VertexFormatID MultiVertexDeclaration;

	ShaderID SnowShader;
	VertexFormatID SnowVertexDeclaration;
	TextureID MeshTexture;
	TextureID ExposureDepthMapTexture;
	TextureID ShadingTexture;

	std::string m_sFileName;
	std::string m_szPathTex;
	bool m_bMeshRenderer2Simpler_ProceduralBuilding;
	tShader mShader;
	D3DXFROMWINEVECTOR4 m_v4AmbientColor, m_v4DiffuseColor, m_v4LightDir, m_v4EyeDir;
	D3DXFROMWINEMATRIX m_mWorld;

	float m_fSpinX, m_fSpinY, m_fSpinZ,m_fSX,m_fSy,m_fSZ,m_fTX,m_fTY,m_fTZ;

	// ??? MeshHelper * GetAsset(){return 0;}
public:
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

	int CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut, const D3DXFROMWINEMATRIX& piMatrix, unsigned int a=0);
    	D3DXFROMWINEVECTOR3 GetMeshBBoxExtents( unsigned int );
    	D3DXFROMWINEVECTOR3 GetMeshBBoxCenter( unsigned int );
	D3DXFROMWINEMATRIX ScaleAsset(void);
	void initrindices();
	int InitShader(tShader aShader=eSimpleShader);
	int LoadAsset(const char* filename, bool scaleasset=true);
};

#if 0
#ifdef ASSIMP_DOUBLE_PRECISION
    typedef double ai_real;
    typedef signed long long int ai_int;
    typedef unsigned long long int ai_uint;
#else // ASSIMP_DOUBLE_PRECISION
    typedef float ai_real;
    typedef signed int ai_int;
    typedef unsigned int ai_uint;
#endif // ASSIMP_DOUBLE_PRECISION
#endif

#ifndef SWIG
using namespace Assimp;
#endif
//struct stx_Effect;
//struct stx_Effect::stx_Technique;
struct MeshRenderer2
{
public:
	#if 0
	struct SceneAnimator
	{
		SceneAnimator():mRootNode(0), mScene(0){}
		void SetAnimIndex(unsigned int pAnimIndex){}
		unsigned int CurrentAnimIndex(){return 0;}
		int GetNumAnimations(){return 0;}
		void* mRootNode;
		void* mScene;
	};
	struct MeshHelper
	{
		MeshHelper()		:
					piDiffuseTexture	(-1),
					piSpecularTexture	(-1),
					piAmbientTexture	(-1),
					piNormalTexture		(-1),
					piEmissiveTexture	(-1),
					piOpacityTexture	(-1),
					piShininessTexture	(-1),
					piLightmapTexture	(-1),
					pvOriginalNormals	(0),
					twosided (false),
					m_bHasNormals(false),
					m_bHasDiffuseTexture(false),
					m_pAnimator(0)
		{
		}
		~MeshHelper()
		{
		}
		SceneAnimator* m_pAnimator;
	struct Vertex
        {
            aiVector3D vPosition;
            aiVector3D vNormal;
            aiVector3D dColorDiffuse;
            aiVector3D vTangent;
            aiVector3D vBitangent;
            aiVector2D vTextureUV;
            aiVector2D vTextureUV2;
            unsigned char mBoneIndices[ 4 ];
            unsigned char mBoneWeights[ 4 ]; // last Weight not used, calculated inside the vertex shader
	};
        struct PositionTextureVertex
        {
            aiVector3D vPosition;
            aiVector3D vColorDiffuse;
            aiVector2D vTextureUV;
	};
	struct PositionNormalVertex
        {
            aiVector3D vPosition;
            aiVector3D vNormal;
            aiVector3D vColorDiffuse;
	};

		struct VertexPassThrough
		{
			aiVector3D vPosition;
			aiVector2D vTextureUV;
		};
		struct VertexNormals
		{
			aiVector3D vPosition;
		};
		struct LineVertex
		{
			aiVector3D vPosition;
			__WORD__ dColorDiffuse;
		};

				// vertex buffer
				std::vector<PositionTextureVertex> piPositionTextureVertexVB;
				std::vector<PositionNormalVertex> piPositionNormalVertexVB;

				// index buffer. For partially transparent meshes
				// created with dynamic usage to be able to update
				// the buffer contents quickly
				#if 0
				std::vector<__WORD__> piWIB;
				std::vector<__DWORD__> piDWIB;
				#else
				std::vector<::uint16_t> piWIB;
				std::vector<::uint32_t> piDWIB;
				#endif

				// vertex buffer to be used to draw vertex normals
				// (vertex normals are generated in every case)
				std::vector<VertexNormals> piNormalsVB;

				// material textures
				TextureID piDiffuseTexture;
				TextureID piSpecularTexture;
				TextureID piAmbientTexture;
				TextureID piEmissiveTexture;
				TextureID piNormalTexture;
				TextureID piOpacityTexture;
				TextureID piShininessTexture;
				TextureID piLightmapTexture;
				// material colors
				D3DXFROMWINEVECTOR4 vDiffuseColor;
				D3DXFROMWINEVECTOR4 vSpecularColor;
				D3DXFROMWINEVECTOR4 vAmbientColor;
				D3DXFROMWINEVECTOR4 vEmissiveColor;

				// opacity for the material
				float fOpacity;

				// shininess for the material
				float fShininess;

				// strength of the specular highlight
				float fSpecularStrength;

				// two-sided?
				bool twosided;

				// Stores a pointer to the original normal set of the asset
				aiVector3D* pvOriginalNormals;

				bool m_bHasNormals;
				bool m_bHasDiffuseTexture;
	};

		MeshHelper** apcMeshes;
#endif
	MeshRenderer2();
	virtual ~MeshRenderer2();
	void Render(stx_Effect::stx_Technique* pTechnique, stx_Effect::stx_Variable* g_ptxDiffuse);
#if 1
	void Render( stx_Effect::stx_Technique* pTechnique, const D3DXFROMWINEMATRIX* aw=0);
#else
	void Render( stx_Effect::stx_Technique* pTechnique, const char* awName=0, const D3DXFROMWINEMATRIX* aw=0 );
#endif
	void ChangeShader(const D3DXFROMWINEMATRIX* mat=0, TextureID id=-1, ShaderID shdid=-1, VertexFormatID vfid=-1, 
			D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, -1.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));
	//unsigned int render(D3DXFROMWINEMATRIX* mat=0){Draw(mat);} // ambiguous 
	void render(const D3DXFROMWINEMATRIX* w=0, const TextureID aTexID=-1);
        void BeginDraw(	const D3DXFROMWINEMATRIX* mat=0, TextureID id=-1, ShaderID shdid=-1, VertexFormatID vfid=-1, 
			D3DXFROMWINEVECTOR4 AmbientColor=D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 DiffuseColor=D3DXFROMWINEVECTOR4(0.0f, 0.1f, 0.0f, 1.0f),
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, -1.0f, 1.0f),
			D3DXFROMWINEVECTOR4 EyeDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, 1.0f, 1.0f));

	unsigned int EndDraw(const unsigned int flags=RESET_ALL);

	// ??? void Render(stx_Effect::stx_Technique* aTechnique, TextureID aID=-1){} // ???

	void Render(const D3DXFROMWINEMATRIX* w=0)
	{
		if(w)
			render(w);
		else {D3DXFROMWINEMATRIX I;D3DXFROMWINEMatrixIdentity(&I);render(&I);}
	}
	void DrawSubset(unsigned int a){D3DXFROMWINEMATRIX I;D3DXFROMWINEMatrixIdentity(&I);render(&I);}

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
			D3DXFROMWINEVECTOR4 LightDir=D3DXFROMWINEVECTOR4(0.0f, 0.0f, -1.0f, 1.0f),
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
#if 0
// http://www.songho.ca/opengl/gl_sphere.html
        void buildVerticesSmooth(float radius=1.0f, int sectorCount=20, int stackCount=20, tShader aShader=eShaderNone);
        void buildVerticesFlat(float radius=1.0f, int sectorCount=20, int stackCount=20, tShader aShader=eShaderNone);
        //void buildInterleavedVertices();
        void clearArrays();
        std::vector<float> computeFaceNormal(float x1, float y1, float z1,  // v1
                                             float x2, float y2, float z2,  // v2
                                             float x3, float y3, float z3);  // v3
#endif

        void CreateHalfSphere(float radius=1.0f, tShader aShader=eShaderNone);
        void CreateHalfSphere_(float radius=1.0f, int aslices=20, int stacks=20, tShader aShader=eShaderNone);
#if 1
        void CreateCone(float bottomRadius=1.0f, float height=1.0f, tShader aShader=eShaderNone);
        void CreateCone_(float bottomRadius=1.0f, float height=1.0f, int slices=20, tShader aShader=eShaderNone);
	void CreateHemis(tShader aShader=eShaderNone);
	void CreateHemis_(int stepLng=20, int stepLat=20, tShader aShader=eShaderNone);
	void CreateTorus(float radMajor=1.0f, float radMinor=1.0f, tShader aShader=eShaderNone);
	void CreateTorus_(float radMajor=1.0f, float radMinor=1.0f, int slices=20, int stacks=20, tShader aShader=eShaderNone);
	void CreateTorus2(float radMajor=1.0f, float radMinor=1.0f, tShader aShader=eShaderNone);
	void CreateTorus2_(float radMajor=1.0f, float radMinor=1.0f, int slices=20, int stacks=20, tShader aShader=eShaderNone);
	void CreateTire(float radMajor=1.0f, float radMinor=1.0f, tShader aShader=eShaderNone);
	void CreateTire_(float radMajor=1.0f, float radMinor=1.0f, int slices=20, int stacks=20, tShader aShader=eShaderNone);
    	void CreateIcosahedron(	//int recursionLevel=0, 
    				tShader aShader=eShaderNone);			
	void CreateIcosahedron_( float radius=1.0f,
				 tShader aShader=eShaderNone);
#endif
	void CreateOcta(tShader aShader=eShaderNone);
	void CreateTetra(tShader aShader=eShaderNone);
	void CreateEllipsoid(float fA=1.0f, float fB=1.0f, float fC=1.0f, tShader aShader=eShaderNone);
	void CreateEllipsoid_(float fA=1.0f, float fB=1.0f, float fC=1.0f, int slices=20, int stacks=20, tShader aShader=eShaderNone);
	void CreateEllipsoid__(int slices=64, int stacks=64, float fA=1.0f, float fB=1.0f, float fC=1.0f, float r1=0.0f, float r2=1.0f, tShader aShader=eShaderNone);
        void CreateCylinder(float bottomRadius=1.0f, float topRadius=1.0f, float length=2.0f, tShader aShader=eShaderNone, const int a=1);
        void CreateCylinder_(float bottomRadius=1.0f, float topRadius=1.0f, float length=2.0f, int aslices=20, int stacks=20, tShader aShader=eShaderNone, const int a=1);
	//void CreateCylinderDX(FLOAT radius1=1.0f, FLOAT radius2=1.0f, FLOAT length=2.0f, UINT slices=20, UINT stacks=20, tShader aShader=eShaderNone);	
        void CreatePrism(	tShader aShader_=eShaderNone,
				const D3DXFROMWINEVECTOR3 SizeCoefficient=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f),
				const D3DXFROMWINEVECTOR2 TextureRepeatCoefficient=D3DXFROMWINEVECTOR2(1.0f, 1.0f));
protected:
	int ToIndex(char ch);
	void AddPolygon(std::string point_names, std::vector<D3DXFROMWINEVECTOR3> points);
        void AddPolygon(std::vector<D3DXFROMWINEVECTOR3> points);
public:
        void CreateDodecahedron(	tShader aShader_=eShaderNone,
				const D3DXFROMWINEVECTOR3 SizeCoefficient=D3DXFROMWINEVECTOR3(1.0f, 1.0f, 1.0f),
				const D3DXFROMWINEVECTOR2 TextureRepeatCoefficient=D3DXFROMWINEVECTOR2(1.0f, 1.0f));
						
	int Load(const char* aFilename=0, tShader aShader=eSimpleShader, bool scaleasset=true, bool a_bMeshRenderer2Simpler_ProceduralBuilding=false);
	int LoadFile(const char* aFilename=0, tShader aShader=eSimpleShader, bool scaleasset=true, bool a_bMeshRenderer2Simpler_ProceduralBuilding=false);

	//void CreateMesh(const char* afn, tShader aShader=eSimpleShader, bool scaleasset=true, bool a_bMeshRenderer2Simpler_ProceduralBuilding=false);

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
            #ifndef __APPLE__
			LOG_PRINT("vertices.size()=%d\n", meshes[0].vertices.size());
			LOG_PRINT("indices.size()=%d\n", meshes[0].indices.size());
            #endif
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
	std::vector<__WORD__> lineIndices;
    //Assimp::Importer importer;
	SceneAnimator* m_pAnimator;
    AssetHelper* m_pcAsset;
	ShaderID Shader;
        VertexFormatID VertexDeclaration;
protected:
    bool Import3DFromFile( const std::string& pFile);
	enum tVertex
	{
		ePositionTextureVertex,
		ePositionNormalVertex,
		//ePositionNormalTextureVertex,
		//eCompleteVertex
	};
	tVertex m_Vertex;
	static ShaderID AmbientShader;
	static ShaderID DiffuseShader;
	static ShaderID PhongShader1;
	static ShaderID PhongShader2;
	static VertexFormatID AmbientVertexDeclaration;
	static VertexFormatID DiffuseVertexDeclaration;
	static VertexFormatID PhongVertexDeclaration1;
	static VertexFormatID PhongVertexDeclaration2;
    static VertexFormatID _VertexDeclaration;
	static ShaderID _Shader;

	static ShaderID PhysicalBasedShader;
        static VertexFormatID PhysicalBasedVertexDeclaration;

	static ShaderID SimpleShader;
	static VertexFormatID SimpleVertexDeclaration;

	static ShaderID ColorShader;
	static VertexFormatID ColorVertexDeclaration;

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

	static ShaderID SimpleShader2;
	static VertexFormatID SimpleVertexDeclaration2;

	std::string m_sFileName;
	std::string m_szPathTex;
	bool m_bMeshRenderer2Simpler_ProceduralBuilding;
	tShader mShader;
	D3DXFROMWINEVECTOR4 m_v4AmbientColor, m_v4DiffuseColor, m_v4LightDir, m_v4EyeDir;
	D3DXFROMWINEMATRIX m_mWorld;

	float m_fSpinX, m_fSpinY, m_fSpinZ,m_fSX,m_fSy,m_fSZ,m_fTX,m_fTY,m_fTZ;
public:
	// ??? MeshHelper * GetAsset(){return 0;}

	int CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut, const D3DXFROMWINEMATRIX& piMatrix);
    	D3DXFROMWINEVECTOR3 GetMeshBBoxExtents( unsigned int );
    	D3DXFROMWINEVECTOR3 GetMeshBBoxCenter( unsigned int );
	D3DXFROMWINEMATRIX ScaleAsset();
	void initrindices();
	int InitShader(tShader aShader=eSimpleShader);
	void LoadThreadProc2(std::string& aFileNameMesh);
	int LoadXFile(const char* aFilename);
	int LoadDAEFile(const char* aFilename);
	int Load3DSFile(const char* aFilename);
	int LoadOBJFile(const char* aFilename);
	int LoadSDKMESHFile(const char* aFilename);
	int LoadAsset(const char* filename, bool scaleasset=true);






	protected:
	aiScene* pcScene;
    double g_dCurrent;
    bool g_bPlay;
	int j_tmp, z_tmp;
	D3DXFROMWINEMATRIX w_tmp;
	//BoundsDescriptor aabb;
	TextureID texID, tex_tmp;
	tShape m_Shape;
	D3DXFROMWINEVECTOR3 _max, _min;
public:
    void TransformCoord(D3DXFROMWINEMATRIX& aM);
	void TransformBindPose(D3DXFROMWINEMATRIX* aM){}
	unsigned int GetNumMeshes(){return 0;}
	unsigned int GetNumIndices(){return indices.size();}
	unsigned int GetNumFaces(){return indices.size()/3;}
	unsigned int GetNumVertices( unsigned int m=0, unsigned int a=0 ){return vertices.size();}
	VertexBufferID GetVB10( unsigned int m, unsigned int a ){return -1;}
	unsigned int GetVertexStride( unsigned int m, unsigned int a ){return 0;}
	IndexBufferID GetIB10( unsigned int m ){return -1;}
	unsigned int GetIBFormat10( unsigned int m ){return 0;}
	unsigned int GetNumSubsets( unsigned int m ){return 0;}
	SDKMESH_SUBSET* GetSubset( unsigned int m, unsigned int subset ){return new SDKMESH_SUBSET();}
	SDKMESH_MATERIAL* GetMaterial( unsigned int MaterialID ){return new SDKMESH_MATERIAL();}
     	void TransformMesh(D3DXFROMWINEMATRIX* mIdentity, float fTime ){}
	unsigned int GetNumInfluences( unsigned int aMesh ){return 0;}
	D3DXFROMWINEMATRIX* GetMeshInfluenceMatrix(D3DXFROMWINEMATRIX* iMesh, unsigned int i){return iMesh;}
	Primitives GetPrimitiveType(SDKMESH_PRIMITIVE_TYPE a){return PRIM_TRIANGLES;}

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

	D3DXFROMWINEMATRIX CalculateBounds(D3DXFROMWINEVECTOR3* p_avOut=0, D3DXFROMWINEMATRIX* piMatrix=0, unsigned int sw=0);
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
	
};
#endif

