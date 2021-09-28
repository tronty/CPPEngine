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
#pragma once
#ifndef _SDKMESH_
#define _SDKMESH_
typedef uint_least64_t UINT64; // ???

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
        VertexBuffer9* pVB9;
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
        IndexBuffer9* pIB9;
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

    TextureID pDiffuseTexture9;
    TextureID pNormalTexture9;
    TextureID pSpecularTexture9;
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

#ifndef _CONVERTER_APP_

//--------------------------------------------------------------------------------------
// CDXUTSDKMesh class.  This class reads the sdkmesh file format for use by the samples
//--------------------------------------------------------------------------------------
class CDXUTSDKMesh
{
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
    char*                           m_strPathW[MAX_PATH];
    char                            m_strPath[MAX_PATH];

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
                                                   UINT NumMaterials,  );
    HRESULT                         CreateVertexBuffer( 
                                                        SDKMESH_VERTEX_BUFFER_HEADER* pHeader, void* pVertices,
                                                         );
    HRESULT                         CreateIndexBuffer( 
                                                       SDKMESH_INDEX_BUFFER_HEADER* pHeader, void* pIndices,
                                                        );

    //frame manipulation
    void                            TransformBindPoseFrame( UINT iFrame, D3DXFROMWINEMATRIX* pParentWorld );
    void                            TransformFrame( UINT iFrame, D3DXFROMWINEMATRIX* pParentWorld, double fTime );
    void                            TransformFrameAbsolute( UINT iFrame, double fTime );
    
//--------------------------------------------------------------------------------------


    //Direct3D 9 rendering helpers
    void                            RenderMesh( UINT iMesh,
                                                
                                                LPD3DXEFFECT pEffect,
                                                D3DXHANDLE hTechnique,
                                                D3DXHANDLE htxDiffuse,
                                                D3DXHANDLE htxNormal,
                                                D3DXHANDLE htxSpecular );
    void                            RenderFrame( UINT iFrame,
                                                 
                                                 LPD3DXEFFECT pEffect,
                                                 D3DXHANDLE hTechnique,
                                                 D3DXHANDLE htxDiffuse,
                                                 D3DXHANDLE htxNormal,
                                                 D3DXHANDLE htxSpecular );

public:
                                    CDXUTSDKMesh();
    virtual                         ~CDXUTSDKMesh();

    virtual HRESULT                 Create( LPCTSTR szFileName, bool bCreateAdjacencyIndices=
                                            false,  );
    virtual HRESULT                 Create( BYTE* pData, UINT DataBytes,
                                            bool bCreateAdjacencyIndices=false, bool bCopyStatic=false,
                                             );
    virtual HRESULT                 LoadAnimation( char** szFileName );
    virtual void                    Destroy();

    //Frame manipulation
    void                            TransformBindPose( D3DXFROMWINEMATRIX* pWorld );
    void                            TransformMesh( D3DXFROMWINEMATRIX* pWorld, double fTime );
    
    //Direct3D 9 Rendering
    virtual void                    Render( 
                                            LPD3DXEFFECT pEffect,
                                            D3DXHANDLE hTechnique,
                                            D3DXHANDLE htxDiffuse = 0,
                                            D3DXHANDLE htxNormal = 0,
                                            D3DXHANDLE htxSpecular = 0 );

    //Helpers (D3D9 specific)
    static D3DPRIMITIVETYPE         GetPrimitiveType9( SDKMESH_PRIMITIVE_TYPE PrimType );
    D3DFORMAT                       GetIBFormat9( UINT iMesh );
    VertexBuffer9* GetVB9( UINT iMesh, UINT iVB );
    IndexBuffer9* GetIB9( UINT iMesh );

    //Helpers (general)
    char* GetMeshPathA();
    char** GetMeshPathW();
    UINT                            GetNumMeshes();
    UINT                            GetNumMaterials();
    UINT                            GetNumVBs();
    UINT                            GetNumIBs();
    VertexBuffer9* GetVB9At( UINT iVB );
    IndexBuffer9* GetIB9At( UINT iIB );
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
};

//-----------------------------------------------------------------------------
// Name: class CDXUTXFileMesh
// Desc: Class for loading and rendering file-based meshes
//-----------------------------------------------------------------------------
class CDXUTXFileMesh
{
public:
    char*       m_strName[512];
    LPD3DXMESH m_pMesh;   // Managed mesh

    // Cache of data in m_pMesh for easy access
    VertexBuffer9* m_pVB;
    IndexBuffer9* m_pIB;
    VertexDeclaration9* m_pDecl;
    DWORD m_dwNumVertices;
    DWORD m_dwNumFaces;
    DWORD m_dwBytesPerVertex;

    DWORD m_dwNumMaterials; // Materials for the mesh
    D3DMATERIAL9* m_pMaterials;
    CHAR        (*m_strMaterials )[MAX_PATH];
    BaseTexture9** m_pTextures;
    bool m_bUseMaterials;

public:
    // Rendering
    HRESULT     Render( 
                        bool bDrawOpaqueSubsets = true,
                        bool bDrawAlphaSubsets = true );
    HRESULT     Render( ID3DXEffect* pEffect,
                        D3DXHANDLE hTexture = NULL,
                        D3DXHANDLE hDiffuse = NULL,
                        D3DXHANDLE hAmbient = NULL,
                        D3DXHANDLE hSpecular = NULL,
                        D3DXHANDLE hEmissive = NULL,
                        D3DXHANDLE hPower = NULL,
                        bool bDrawOpaqueSubsets = true,
                        bool bDrawAlphaSubsets = true );

    // Mesh access
    LPD3DXMESH  GetMesh()
    {
        return m_pMesh;
    }

    // Rendering options
    void        UseMeshMaterials( bool bFlag )
    {
        m_bUseMaterials = bFlag;
    }
    HRESULT     SetFVF(  DWORD dwFVF );
    HRESULT     SetVertexDecl(  const D3DVERTEXELEMENT9* pDecl,
                               bool bAutoComputeNormals = true, bool bAutoComputeTangents = true,
                               bool bSplitVertexForOptimalTangents = false );

    // Creation/destruction
    HRESULT     Create(  const char* strFilename );
    HRESULT     Create(  LPD3DXFILEDATA pFileData );
    HRESULT     Create(  ID3DXMesh* pInMesh, D3DXMATERIAL* pd3dxMaterials,
                        DWORD dwMaterials );
    HRESULT     CreateMaterials( const char* strPath, D3DXMATERIAL* d3dxMtrls,
                                 DWORD dwNumMaterials );
    HRESULT     Destroy();

                CDXUTXFileMesh( const char* strName = L"CDXUTXMeshFile_Mesh" );
    virtual     ~CDXUTXFileMesh();
};


#endif

#endif

