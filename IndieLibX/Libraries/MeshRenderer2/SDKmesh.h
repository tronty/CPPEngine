//--------------------------------------------------------------------------------------
// File: SDKMesh.h
//
// Disclaimer:  
//   The SDK Mesh format (.sdkmesh) is not a recommended file format for shipping titles.  
//   It was designed to meet the specific needs of the SDK samples.  Any real-world 
//   applications should avoid this file format in favor of a destination format that 
//   meets the specific needs of the application.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#ifndef _SDKMESH_
#define _SDKMESH_

#if 0
typedef int_least64_t INT64;
typedef uint_least64_t UINT64;
typedef void* D3DXFROMWINEHANDLE;
#define CGrowableArray std::vector
#endif

//--------------------------------------------------------------------------------------
// Hard Defines for the various structures
//--------------------------------------------------------------------------------------
#define SDKMESH_FILE_VERSION 101
#define MAX_VERTEX_ELEMENTS 32
#define MAX_VERTEX_STREAMS 16
#define MAX_FRAME_NAME 100
#define MAX_MESH_NAME 100
#define MAX_SUBSET_NAME 100
#define MAX_MATERIAL_NAME 100
#define MAX_TEXTURE_NAME MAX_PATH
#define MAX_MATERIAL_PATH MAX_PATH
#define INVALID_FRAME ((UINT)-1)
#define INVALID_MESH ((UINT)-1)
#define INVALID_MATERIAL ((UINT)-1)
#define INVALID_SUBSET ((UINT)-1)
#define INVALID_ANIMATION_DATA ((UINT)-1)
#define ERROR_RESOURCE_VALUE 1
#define INVALID_SAMPLER_SLOT ((UINT)-1)

template<typename TYPE> BOOL IsErrorResource( TYPE data )
{
    if( ( TYPE )ERROR_RESOURCE_VALUE == data )
        return TRUE;
    return FALSE;
}
//--------------------------------------------------------------------------------------
// Enumerated Types.  These will have mirrors in both D3D9 and D3D10
//--------------------------------------------------------------------------------------
enum SDKMESH_PRIMITIVE_TYPE
{
    PT_TRIANGLE_LIST = 0,
    PT_TRIANGLE_STRIP,
    PT_LINE_LIST,
    PT_LINE_STRIP,
    PT_POINT_LIST,
    PT_TRIANGLE_LIST_ADJ,
    PT_TRIANGLE_STRIP_ADJ,
    PT_LINE_LIST_ADJ,
    PT_LINE_STRIP_ADJ,
};

enum SDKMESH_INDEX_TYPE
{
    IT_16BIT = 0,
    IT_32BIT,
};

enum FRAME_TRANSFORM_TYPE
{
    FTT_RELATIVE = 0,
    FTT_ABSOLUTE,		//This is not currently used but is here to support absolute transformations in the future
};

//--------------------------------------------------------------------------------------
// Structures.  Unions with pointers are forced to 64bit.
//--------------------------------------------------------------------------------------
struct SDKMESH_HEADER
{
    //Basic Info and sizes
    UINT Version;
    BYTE IsBigEndian;
    UINT64 HeaderSize;
    UINT64 NonBufferDataSize;
    UINT64 BufferDataSize;

    //Stats
    UINT NumVertexBuffers;
    UINT NumIndexBuffers;
    UINT NumMeshes;
    UINT NumTotalSubsets;
    UINT NumFrames;
    UINT NumMaterials;

    //Offsets to Data
    UINT64 VertexStreamHeadersOffset;
    UINT64 IndexStreamHeadersOffset;
    UINT64 MeshDataOffset;
    UINT64 SubsetDataOffset;
    UINT64 FrameDataOffset;
    UINT64 MaterialDataOffset;
};

struct SDKMESH_VERTEX_BUFFER_HEADER
{
    UINT64 NumVertices;
    UINT64 SizeBytes;
    UINT64 StrideBytes;
    D3DVERTEXELEMENT9 Decl[MAX_VERTEX_ELEMENTS];
    union
    {
        UINT64 DataOffset;				//(This also forces the union to 64bits)
        VertexBufferID pVB9;
    };
};

struct SDKMESH_INDEX_BUFFER_HEADER
{
    UINT64 NumIndices;
    UINT64 SizeBytes;
    UINT IndexType;
    union
    {
        UINT64 DataOffset;				//(This also forces the union to 64bits)
        IndexBufferID pIB9;
    };
};

struct SDKMESH_MESH
{
    char    Name[MAX_MESH_NAME];
    BYTE NumVertexBuffers;
    UINT    VertexBuffers[MAX_VERTEX_STREAMS];
    UINT IndexBuffer;
    UINT NumSubsets;
    UINT NumFrameInfluences; //aka bones

    D3DXFROMWINEVECTOR3 BoundingBoxCenter;
    D3DXFROMWINEVECTOR3 BoundingBoxExtents;

    union
    {
        UINT64 SubsetOffset;	//Offset to list of subsets (This also forces the union to 64bits)
        UINT* pSubsets;	    //Pointer to list of subsets
    };
    union
    {
        UINT64 FrameInfluenceOffset;  //Offset to list of frame influences (This also forces the union to 64bits)
        UINT* pFrameInfluences;      //Pointer to list of frame influences
    };
};

struct SDKMESH_SUBSET
{
    char Name[MAX_SUBSET_NAME];
    UINT MaterialID;
    UINT PrimitiveType;
    UINT64 IndexStart;
    UINT64 IndexCount;
    UINT64 VertexStart;
    UINT64 VertexCount;
};

struct SDKMESH_FRAME
{
    char Name[MAX_FRAME_NAME];
    UINT Mesh;
    UINT ParentFrame;
    UINT ChildFrame;
    UINT SiblingFrame;
    D3DXFROMWINEMATRIX Matrix;
    UINT AnimationDataIndex;		//Used to index which set of keyframes transforms this frame
};

struct SDKMESH_MATERIAL
{
	SDKMESH_MATERIAL()
	{
    		Name[0]=0;
		MaterialInstancePath[0]=0;
		DiffuseTexture[0]=0;
    		NormalTexture[0]=0;
    		SpecularTexture[0]=0;
		pDiffuseTexture9=-1;
   		pNormalTexture9=-1;
    		pSpecularTexture9=-1;
	}

    char    Name[MAX_MATERIAL_NAME];
    // Use MaterialInstancePath
    char    MaterialInstancePath[MAX_MATERIAL_PATH];

    // Or fall back to d3d8-type materials
    char    DiffuseTexture[MAX_TEXTURE_NAME];
    char    NormalTexture[MAX_TEXTURE_NAME];
    char    SpecularTexture[MAX_TEXTURE_NAME];

    D3DXFROMWINEVECTOR4 Diffuse;
    D3DXFROMWINEVECTOR4 Ambient;
    D3DXFROMWINEVECTOR4 Specular;
    D3DXFROMWINEVECTOR4 Emissive;
    FLOAT Power;

    union
    {
        UINT64 Force64_1;
	int pDiffuseTexture9;
    };
    union
    {
        UINT64 Force64_2;
	int pNormalTexture9;
    };
    union
    {
        UINT64 Force64_3;
	int pSpecularTexture9;
    };
    union
    {
        UINT64 Force64_4;
	int pDiffuseRV10;
    };
    union
    {
        UINT64 Force64_5;
	int pNormalRV10;
    };
    union
    {
        UINT64 Force64_6;
	int pSpecularRV10;
    };
};

struct SDKANIMATION_FILE_HEADER
{
    UINT Version;
    BYTE IsBigEndian;
    UINT FrameTransformType;
    UINT NumFrames;
    UINT NumAnimationKeys;
    UINT AnimationFPS;
    UINT64 AnimationDataSize;
    UINT64 AnimationDataOffset;
};

struct SDKANIMATION_DATA
{
    D3DXFROMWINEVECTOR3 Translation;
    D3DXFROMWINEVECTOR4 Orientation;
    D3DXFROMWINEVECTOR3 Scaling;
};

struct SDKANIMATION_FRAME_DATA
{
    char FrameName[MAX_FRAME_NAME];
    union
    {
        UINT64 DataOffset;
        SDKANIMATION_DATA* pAnimationData;
    };
};

//--------------------------------------------------------------------------------------
// CDXUTSDKMesh class.  This class reads the sdkmesh file format for use by the samples
//--------------------------------------------------------------------------------------
class CDXUTSDKMesh
{
public:
	int CreateFromFile(     const char* szFileName);
	int CreateFromMemory( BYTE* pData,
                                        UINT DataBytes, bool bCopyStatic=false );
private:
    UINT m_NumOutstandingResources;
    bool m_bLoading;
    //BYTE*                         m_pBufferData;
    HANDLE m_hFile;
    HANDLE m_hFileMappingObject;
    CGrowableArray <BYTE*> m_MappedPointers;

protected:
    //These are the pointers to the two chunks of data loaded in from the mesh file
    BYTE* m_pStaticMeshData;
    BYTE* m_pHeapData;
    BYTE* m_pAnimationData;
    BYTE** m_ppVertices;
    BYTE** m_ppIndices;

    //Keep track of the path
    //char*                           m_strPathW[MAX_PATH];
    char                            m_strPath[MAX_PATH];
	std::string m_sFileName;

    //General mesh info
    SDKMESH_HEADER* m_pMeshHeader;
    SDKMESH_VERTEX_BUFFER_HEADER* m_pVertexBufferArray;
    SDKMESH_INDEX_BUFFER_HEADER* m_pIndexBufferArray;
    SDKMESH_MESH* m_pMeshArray;
    SDKMESH_SUBSET* m_pSubsetArray;
    SDKMESH_FRAME* m_pFrameArray;
    SDKMESH_MATERIAL* m_pMaterialArray;

    // Adjacency information (not part of the m_pStaticMeshData, so it must be created and destroyed separately )
    SDKMESH_INDEX_BUFFER_HEADER* m_pAdjacencyIndexBufferArray;

    //Animation (TODO: Add ability to load/track multiple animation sets)
    SDKANIMATION_FILE_HEADER* m_pAnimationHeader;
    SDKANIMATION_FRAME_DATA* m_pAnimationFrameData;
    D3DXFROMWINEMATRIX* m_pBindPoseFrameMatrices;
    D3DXFROMWINEMATRIX* m_pTransformedFrameMatrices;

protected:
    void                            LoadMaterials( SDKMESH_MATERIAL* pMaterials,
                                                   UINT NumMaterials );
    int                         CreateVertexBuffer( 
                                                        SDKMESH_VERTEX_BUFFER_HEADER* pHeader, void* pVertices);
    int                         CreateIndexBuffer( 
                                                       SDKMESH_INDEX_BUFFER_HEADER* pHeader, void* pIndices);

    //frame manipulation
    void                            TransformBindPoseFrame( UINT iFrame, D3DXFROMWINEMATRIX* pParentWorld );
    void                            TransformFrame( UINT iFrame, D3DXFROMWINEMATRIX* pParentWorld, double fTime );
    void                            TransformFrameAbsolute( UINT iFrame, double fTime );
    
//--------------------------------------------------------------------------------------

public:
                                    CDXUTSDKMesh();
    virtual                         ~CDXUTSDKMesh();

    //virtual int                 Create( const char* szFileName);
    virtual int                 Create( BYTE* pData, UINT DataBytes);
    virtual int                 LoadAnimation( const char* szFileName );
    virtual void                    Destroy();

    //Frame manipulation
    void                            TransformBindPose( D3DXFROMWINEMATRIX* pWorld );
    void                            TransformMesh( D3DXFROMWINEMATRIX* pWorld, double fTime );

    //Helpers (D3D9 specific)
    static Primitives         GetPrimitiveType9( SDKMESH_PRIMITIVE_TYPE PrimType );
    D3DFORMAT                       GetIBFormat9( UINT iMesh );
    VertexBufferID GetVB9( UINT iMesh, UINT iVB );
    IndexBufferID GetIB9( UINT iMesh );

    //Helpers (general)
    char* GetMeshPathA();
    char* GetMeshPathW();
    UINT                            GetNumMeshes();
    UINT                            GetNumMaterials();
    UINT                            GetNumVBs();
    UINT                            GetNumIBs();
    VertexBufferID GetVB9At( UINT iVB );
    IndexBufferID GetIB9At( UINT iIB );
    BYTE* GetRawVerticesAt( UINT iVB );
    BYTE* GetRawIndicesAt( UINT iIB );
    SDKMESH_MATERIAL* GetMaterial( UINT iMaterial );
    SDKMESH_MESH* GetMesh( UINT iMesh );
    UINT                            GetNumSubsets( UINT iMesh );
    SDKMESH_SUBSET* GetSubset( UINT iMesh, UINT iSubset );
    UINT                            GetVertexStride( UINT iMesh, UINT iVB );
    SDKMESH_FRAME* FindFrame( char* pszName );
    UINT64                          GetNumVertices( UINT iMesh, UINT iVB );
    UINT64                          GetNumIndices( UINT iMesh );
    D3DXFROMWINEVECTOR3                     GetMeshBBoxCenter( UINT iMesh );
    D3DXFROMWINEVECTOR3                     GetMeshBBoxExtents( UINT iMesh );
    UINT                            GetOutstandingResources();
    UINT                            GetOutstandingBufferResources();
    bool                            CheckLoadDone();
    bool                            IsLoaded();
    bool                            IsLoading();
    void                            SetLoading( bool bLoading );
    BOOL                            HadLoadingError();

    //Animation
    UINT                            GetNumInfluences( UINT iMesh );
    const D3DXFROMWINEMATRIX* GetMeshInfluenceMatrix( UINT iMesh, UINT iInfluence );
    UINT                            GetAnimationKeyFromTime( double fTime );
	void Render(  ShaderID pEffect=-1,
				VertexFormatID vf=-1,
                               //D3DXFROMWINEHANDLE hTechnique,
                               const char* htxDiffuse=0,
                               const char* htxNormal=0,
                               const char* htxSpecular=0 );
	void RenderMesh( UINT iMesh=0,                    
                               ShaderID pEffect=-1,
				VertexFormatID vf=-1,
                               //D3DXFROMWINEHANDLE hTechnique,
                               const char* htxDiffuse=0,
                               const char* htxNormal=0,
                               const char* htxSpecular=0 );
	void RenderFrame( UINT iFrame=0,
                                ShaderID pEffect=-1,
				VertexFormatID vf=-1,
                                //D3DXFROMWINEHANDLE hTechnique,
                                const char* htxDiffuse=0,
                                const char* htxNormal=0,
                                const char* htxSpecular=0 );
};
#endif

