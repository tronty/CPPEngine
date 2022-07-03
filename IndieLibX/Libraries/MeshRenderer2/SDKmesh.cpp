//--------------------------------------------------------------------------------------
// File: SDKMesh.cpp
//
// The SDK Mesh format (.sdkmesh) is not a recommended file format for games.  
// It was designed to meet the specific needs of the SDK samples.  Any real-world 
// applications should avoid this file format in favor of a destination format that 
// meets the specific needs of the application.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <Framework3/IRenderer.h>
//include "DXUT.h"
#include "SDKmesh.h"
#if 1
#define LOG_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#else
#define LOG_PRINT(...)
#define LOG_FNLN
#endif

BYTE* pVertices_=0;
UINT64 StrideBytes_=0;
BYTE* pIndices_=0;
UINT indSize_=2;
UINT NumVertices_=0;
UINT NumIndices_=0;
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::SimpleRender2()
{
	static ShaderID SimpleShader_ = -1;
	static VertexFormatID SimpleVertexDeclaration_ = -1;
	if(SimpleShader_==-1)
	{
#ifndef _MSC_VER
	if(m_sFileName=="../../IndieLib_resources/DXJune2010/Dwarf/dwarf.sdkmesh")
#else
	if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Dwarf\\dwarf.sdkmesh")
#endif
	{
		FormatDesc format[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
					};
		SimpleShader_ = IRenderer::GetRendererInstance()->addShaderFromFile("/DXJune2010/rest.hlsl", "main_Dwarf", "main");
		SimpleVertexDeclaration_ = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SimpleShader_);
	}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/Soldier/soldier.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Soldier\\soldier.sdkmesh")
#endif
	{
		FormatDesc format[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD,  FORMAT_FLOAT, 4,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3
					};
		SimpleShader_ = IRenderer::GetRendererInstance()->addShaderFromFile("/DXJune2010/rest.hlsl", "main_Soldier", "main");
		SimpleVertexDeclaration_ = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SimpleShader_);
	}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/MotionBlur/Warrior.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\MotionBlur\\Warrior.sdkmesh")
#endif
	{
		FormatDesc format[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
					};
		SimpleShader_ = IRenderer::GetRendererInstance()->addShaderFromFile("/DXJune2010/rest.hlsl", "main_Warrior2", "main");
		SimpleVertexDeclaration_ = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SimpleShader_);
	}
#ifndef _MSC_VER
	else if(m_sFileName!="../../IndieLib_resources/DXJune2010/trees/tree.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\trees\\tree.sdkmesh")
#endif
	{
		FormatDesc format[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
					};
		SimpleShader_ = IRenderer::GetRendererInstance()->addShaderFromFile("/DXJune2010/rest.hlsl", "main_Tree2", "main");
		SimpleVertexDeclaration_ = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SimpleShader_);
	}else{
		FormatDesc format[] = 	{
						0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
						0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
						0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
					};
		SimpleShader_ = IRenderer::GetRendererInstance()->addShaderFromFile("/DXJune2010/rest.hlsl", "main", "main");
		SimpleVertexDeclaration_ = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SimpleShader_);
	}
	}

	D3DXFROMWINEMATRIX mvp;
#if 0
	D3DXFROMWINEMatrixIdentity(&I);
#else
	float mAngleX=0.0f;
	float mAngleY=0.0f;
		stx_GetAngles(mAngleX, mAngleY, 0.5f);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &mvp,
		                            D3DXFROMWINEToRadian(mAngleX),
		                            D3DXFROMWINEToRadian(mAngleY),
		                            0.0f );
#endif
	VertexBufferID pVB9=0;
	IndexBufferID pIB9=0;
	IRenderer::GetRendererInstance()->setShader(SimpleShader_);
	IRenderer::GetRendererInstance()->setVertexFormat(SimpleVertexDeclaration_);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", mvp);
	IRenderer::GetRendererInstance()->setTexture("txDiffuse", 3);
#if 1
	NumVertices_=m_pVertexBufferArray[ m_pMeshArray[ 0 ].VertexBuffers[0] ].NumVertices;
	NumIndices_ =m_pIndexBufferArray[ m_pMeshArray[ 0 ].IndexBuffer ].NumIndices;
	
	//LOG_PRINT("NumVertices_=%d\n", NumVertices_);
	//LOG_PRINT("NumIndices_=%d\n", NumIndices_);
	if(!NumIndices_)
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES,
		NumIndices_/3,
		pVertices_,
		pVertices_,
		StrideBytes_);
	else if(pVertices_<pIndices_)
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
		0,
		NumVertices_,
		NumIndices_,
		pIndices_,
		pIndices_,
		(indSize_==2)?CONSTANT_INDEX2:CONSTANT_INDEX4,
		pVertices_,
		pVertices_,
		StrideBytes_);
		//sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));
#endif
}
void CDXUTSDKMesh::LoadMaterials( SDKMESH_MATERIAL* pMaterials, UINT numMaterials)
{
	
	UINT m=0;

	for( m = 0; m < numMaterials; m++ )
	{
	pMaterials[m].pDiffuseTexture9=-1;
	pMaterials[m].pNormalTexture9=-1;
	pMaterials[m].pSpecularTexture9=-1;
}
    char strPath[MAX_PATH];
    
    {
        for( m = 0; m < numMaterials; m++ )
        {
            // load textures: Weapons_bumpmap.png ???
            if( pMaterials[m].DiffuseTexture[0] != 0 )
            {
		std::string StrPath_=m_strPath;
		stx_snprintf( strPath, MAX_PATH, "%s", pMaterials[m].DiffuseTexture );
		std::string StrPath=std::string(strPath);
    		std::transform(StrPath.begin(), StrPath.end(), StrPath.begin(), [](unsigned char c){ return std::tolower(c); });
		stx_ReplaceAll(StrPath, ".jpg", ".png");
		stx_ReplaceAll(StrPath, ".dds", ".png");
		StrPath_.append("/");
		StrPath_.append(StrPath);
		//LOG_PRINT("StrPath_=%s\n", StrPath_.c_str());
		stx_snprintf(m_pMaterialArray[m].DiffuseTexture, MAX_TEXTURE_NAME,"%s", StrPath_.c_str());
                if(stx_fileExists(StrPath_.c_str())) pMaterials[m].pDiffuseTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(StrPath_.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
		else {pMaterials[m].pDiffuseTexture9=-1;pMaterials[m].DiffuseTexture[0]=0;}
		LOG_PRINT("pMaterials[%d].DiffuseTexture=%s\n", m, pMaterials[m].DiffuseTexture);
		LOG_PRINT("pMaterials[%d].pDiffuseTexture9=%x\n", m, pMaterials[m].pDiffuseTexture9);
            }
            if( pMaterials[m].NormalTexture[0] != 0 )
            {
		std::string StrPath_=m_strPath;
		stx_snprintf( strPath, MAX_PATH, "%s", pMaterials[m].NormalTexture );
		std::string StrPath=std::string(strPath);
    		std::transform(StrPath.begin(), StrPath.end(), StrPath.begin(), [](unsigned char c){ return std::tolower(c); });
		stx_ReplaceAll(StrPath, ".jpg", ".png");
		stx_ReplaceAll(StrPath, ".dds", ".png");
		StrPath_.append("/");
		StrPath_.append(StrPath);
		//LOG_PRINT("StrPath_=%s\n", StrPath_.c_str());
		stx_snprintf(m_pMaterialArray[m].NormalTexture, MAX_TEXTURE_NAME,"%s", StrPath_.c_str());
                if(stx_fileExists(StrPath_.c_str()))
                pMaterials[m].pNormalTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(StrPath_.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
		else {pMaterials[m].pNormalTexture9=-1;pMaterials[m].NormalTexture[0]=0;}
		LOG_PRINT("pMaterials[%d].NormalTexture=%s\n", m, pMaterials[m].NormalTexture);
		LOG_PRINT("pMaterials[%d].pNormalTexture9=%x\n", m, pMaterials[m].pNormalTexture9);
            }
            if( pMaterials[m].SpecularTexture[0] != 0 )
            {
		std::string StrPath_=m_strPath;
		stx_snprintf( strPath, MAX_PATH, "%s", pMaterials[m].SpecularTexture );
		std::string StrPath=std::string(strPath);
    		std::transform(StrPath.begin(), StrPath.end(), StrPath.begin(), [](unsigned char c){ return std::tolower(c); });
		stx_ReplaceAll(StrPath, ".jpg", ".png");
		stx_ReplaceAll(StrPath, ".dds", ".png");
		StrPath_.append("/");
		StrPath_.append(StrPath);
		//LOG_PRINT("StrPath_=%s\n", StrPath_.c_str());
		stx_snprintf(m_pMaterialArray[m].SpecularTexture, MAX_TEXTURE_NAME,"%s", StrPath_.c_str());
                if(stx_fileExists(StrPath_.c_str())) pMaterials[m].pSpecularTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(StrPath_.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
		else {pMaterials[m].pSpecularTexture9=-1;pMaterials[m].SpecularTexture[0]=0;}
		LOG_PRINT("pMaterials[%d].SpecularTexture=%s\n", m, pMaterials[m].SpecularTexture);
		LOG_PRINT("pMaterials[%d].pSpecularTexture9=%x\n", m, pMaterials[m].pSpecularTexture9);
            }
        }
    }
}
//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateVertexBuffer( SDKMESH_VERTEX_BUFFER_HEADER* pHeader,
                                      void** pVertices )
{
	LOG_FNLN
	LOG_PRINT("pVertices=%x\n", pVertices);
	LOG_PRINT("pHeader=%x\n", pHeader);
	LOG_PRINT("pHeader->NumVertices=%d\n", pHeader->NumVertices);
	LOG_PRINT("pHeader->SizeBytes=%d\n", pHeader->SizeBytes);
	LOG_PRINT("pHeader->StrideBytes=%d\n", pHeader->StrideBytes);
	pVertices_=(BYTE*)pVertices;
	StrideBytes_=pHeader->StrideBytes;
	NumVertices_=( UINT )pHeader->NumVertices;

	VertexBufferID vbID=-1;
	if(0)
		vbID=IRenderer::GetRendererInstance()->addVertexBuffer(pHeader->SizeBytes, STATIC, *pVertices);
	else
#ifndef _MSC_VER
	if(m_sFileName=="../../IndieLib_resources/DXJune2010/MotionBlur/Warrior.sdkmesh")
#else
	if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\MotionBlur\\Warrior.sdkmesh")
#endif
	{
struct VS_INPUT // Warrior
{
    D3DXFROMWINEVECTOR3 Pos;
    D3DXFROMWINEVECTOR3 Normal;
    D3DXFROMWINEVECTOR2 Tex;
    D3DXFROMWINEVECTOR3 Tan;
    unsigned int Bones[4];
    D3DXFROMWINEVECTOR4 Weights;
};
	VS_INPUT* pVertices_=(VS_INPUT*)*pVertices;
	stx_VertexPositionNormalBiNormalTangentColor3Texture* pVertices2=
		new stx_VertexPositionNormalBiNormalTangentColor3Texture[( UINT )pHeader->NumVertices];
	for(unsigned int i=0;i<( UINT )pHeader->NumVertices;i++)
	{
		pVertices2[i].Position=pVertices_[i].Pos;
		pVertices2[i].Normal=pVertices_[i].Normal;
		pVertices2[i].Tangent=pVertices_[i].Tan;
		pVertices2[i].Tex=pVertices_[i].Tex;
	}
	// ??? delete[] pVertices_;
	*pVertices=pVertices2;
	}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/Dwarf/dwarf.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Dwarf\\dwarf.sdkmesh")
#endif
	{
struct VS_INPUT // Dwarf
{
    D3DXFROMWINEVECTOR4 Pos;
    D3DXFROMWINEVECTOR3 Normal;
    D3DXFROMWINEVECTOR2 Tex;
};
	VS_INPUT* pVertices_=(VS_INPUT*)*pVertices;
	stx_VertexPositionNormalBiNormalTangentColor3Texture* pVertices2=
		new stx_VertexPositionNormalBiNormalTangentColor3Texture[( UINT )pHeader->NumVertices];
	for(unsigned int i=0;i<( UINT )pHeader->NumVertices;i++)
	{
		pVertices2[i].Position.x=pVertices_[i].Pos.x;
		pVertices2[i].Position.y=pVertices_[i].Pos.y;
		pVertices2[i].Position.z=pVertices_[i].Pos.z;
		pVertices2[i].Normal=pVertices_[i].Normal;
		pVertices2[i].Tex=pVertices_[i].Tex;
	}
	// ??? delete[] pVertices_;
	*pVertices=pVertices2;
	}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/Soldier/soldier.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Soldier\\soldier.sdkmesh")
#endif
	{
struct VS_INPUT // Soldier
{
    D3DXFROMWINEVECTOR3 Pos;			//Position
    D3DXFROMWINEVECTOR4 Weights;		//Bone weights
    unsigned int Bones[4];			//Bone indices
    D3DXFROMWINEVECTOR3 Norm;			//Normal
    D3DXFROMWINEVECTOR2 Tex;		    //Texture coordinate
    D3DXFROMWINEVECTOR3 Tan;
};
	VS_INPUT* pVertices_=(VS_INPUT*)*pVertices;
	stx_VertexPositionNormalBiNormalTangentColor3Texture* pVertices2=
		new stx_VertexPositionNormalBiNormalTangentColor3Texture[( UINT )pHeader->NumVertices];
	for(unsigned int i=0;i<( UINT )pHeader->NumVertices;i++)
	{
		pVertices2[i].Position=pVertices_[i].Pos;
		pVertices2[i].Normal=pVertices_[i].Norm;
		pVertices2[i].Tangent=pVertices_[i].Tan;
		pVertices2[i].Tex=pVertices_[i].Tex;
	}
	// ??? delete[] pVertices_;
	*pVertices=pVertices2;
	}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/trees/tree.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\trees\\tree.sdkmesh")
#endif
	{
struct VS_INPUT // Tree
{
    D3DXFROMWINEVECTOR3 Pos;
    D3DXFROMWINEVECTOR3 Normal;
    D3DXFROMWINEVECTOR2 Tex;
    D3DXFROMWINEMATRIX mTransform;
};
	VS_INPUT* pVertices_=(VS_INPUT*)*pVertices;
	stx_VertexPositionNormalBiNormalTangentColor3Texture* pVertices2=
		new stx_VertexPositionNormalBiNormalTangentColor3Texture[( UINT )pHeader->NumVertices];
	for(unsigned int i=0;i<( UINT )pHeader->NumVertices;i++)
	{
		pVertices2[i].Position.z=pVertices_[i].Pos.z;
		pVertices2[i].Normal=pVertices_[i].Normal;
		pVertices2[i].Tex=pVertices_[i].Tex;
	}
	// ??? delete[] pVertices_;
	*pVertices=pVertices2;
		//stx_exit(0);
	}
	else
		stx_exit(0);

	//LOG_PRINT("pHeader->pVB9=%x\n", pHeader->pVB9);
	LOG_PRINT("vbID=%d\n", vbID);
	pHeader->pVB9=vbID;	
	pVertices_=(BYTE*)pVertices;
	return 0;
    int hr = S_OK;
    pHeader->DataOffset = 0;

    return hr;
}

//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateIndexBuffer( SDKMESH_INDEX_BUFFER_HEADER* pHeader,
                                         void* pIndices )
{
	LOG_FNLN
	LOG_PRINT("pIndices=%x\n", pIndices);
	LOG_PRINT("pHeader=%x\n", pHeader);
	LOG_PRINT("pHeader->NumIndices=%d\n", pHeader->NumIndices);
	LOG_PRINT("pHeader->SizeBytes=%d\n", pHeader->SizeBytes);
	LOG_PRINT("pHeader->IndexType=%d\n", pHeader->IndexType);

	unsigned int ibs=CONSTANT_INDEX2;
	indSize_=2;
	if(pHeader->SizeBytes==IT_32BIT)
	{
		ibs=CONSTANT_INDEX4;
		indSize_=4;
	}

	ibs=pHeader->SizeBytes/pHeader->NumIndices;
	LOG_PRINT("ibs=%d\n", ibs);

	IndexBufferID ibID=-1;//IRenderer::GetRendererInstance()->addIndexBuffer(pHeader->NumIndices, ibs, STATIC, pIndices);
	pHeader->pIB9=ibID;
	LOG_PRINT("ibID=%d\n", ibID);
	pIndices_=(BYTE*)pIndices;
	NumIndices_=pHeader->NumIndices;
	return 0;
}

//--------------------------------------------------------------------------------------
CDXUTSDKMesh::~CDXUTSDKMesh(){m_MappedPointers.clear();}
CDXUTSDKMesh::CDXUTSDKMesh() : m_NumOutstandingResources( 0 ),
                               m_bLoading( false ),
                               m_hFile( 0 ),
                               m_hFileMappingObject( 0 ),
                               m_pMeshHeader( 0 ),
                               m_pStaticMeshData( 0 ),
                               m_pHeapData( 0 ),
                               m_pAdjacencyIndexBufferArray( 0 ),
                               m_pAnimationData( 0 ),
                               m_ppVertices( 0 ),
                               m_ppIndices( 0 ),
                               m_pBindPoseFrameMatrices( 0 ),
                               m_pTransformedFrameMatrices( 0 )
{
}

//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::LoadAnimation( const char* szFileName )
{
	
    int hr = S_OK;
#if defined(__SDKANIMATION__)
    SDKANIMATION_FILE_HEADER fileheader;
{
  ifstream file (szFileName, ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    file.seekg (0, ios::beg);
    file.read ((BYTE*)&fileheader, sizeof( SDKANIMATION_FILE_HEADER ));
    file.seekg (0, ios::beg);
    m_pAnimationData = new BYTE[ ( size_t )( sizeof( SDKANIMATION_FILE_HEADER ) + fileheader.AnimationDataSize ) ];
    file.read(m_pAnimationData, ( size_t )( sizeof( SDKANIMATION_FILE_HEADER ) + fileheader.AnimationDataSize ));
    file.seekg (0, ios::beg);
    file.close();
    cout << "the entire file content is in memory 2\n";
  }
  else cout << "Unable to open file 2\n";
}
#endif
#if defined(__SDKANIMATION__)
    ///////
    // pointer fixup
    //m_pMeshHeader = ( SDKMESH_HEADER* )m_pStaticMeshData;
    m_pAnimationHeader = ( SDKANIMATION_FILE_HEADER* )m_pAnimationData;
    m_pAnimationFrameData = ( SDKANIMATION_FRAME_DATA* )( m_pAnimationData + m_pAnimationHeader->AnimationDataOffset );

	LOG_FNLN;
	//LOG_PRINT("m_pAnimationHeader=%x\n", m_pAnimationHeader);
	//LOG_PRINT("m_pAnimationHeader->AnimationDataOffset=%x\n", m_pAnimationHeader->AnimationDataOffset);
	//LOG_PRINT("m_pAnimationHeader->Version=%x\n", m_pAnimationHeader->Version);
	//LOG_PRINT("m_pAnimationHeader->IsBigEndian=%x\n", m_pAnimationHeader->IsBigEndian);
    	//LOG_PRINT("m_pAnimationHeader->NumFrames=%d\n", m_pAnimationHeader->NumFrames);
	//LOG_PRINT("m_pAnimationHeader->NumAnimationKeys=%d\n", m_pAnimationHeader->NumAnimationKeys);

    UINT64 BaseOffset = sizeof( SDKANIMATION_FILE_HEADER );
    for( UINT i = 0; i < m_pAnimationHeader->NumFrames; i++ )
    {
        m_pAnimationFrameData[i].pAnimationData = ( SDKANIMATION_DATA* )( m_pAnimationData +
                                                                          m_pAnimationFrameData[i].DataOffset +
                                                                          BaseOffset );
	//LOG_PRINT("m_pAnimationFrameData[%d].FrameName=%s\n", i, m_pAnimationFrameData[i].FrameName);
        SDKMESH_FRAME* pFrame = FindFrame( m_pAnimationFrameData[i].FrameName );
        if( pFrame )
        {
            pFrame->AnimationDataIndex = i;
        }
    }
#else
    m_pAnimationHeader = 0;
    m_pAnimationFrameData = 0;
#endif

    hr = S_OK;
Error:
    //CloseHandle( hFile );
    return hr;
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::Destroy()
{

}

int CDXUTSDKMesh::Create( BYTE* pData, UINT DataBytes)
{
    return CreateFromMemory( pData, DataBytes);
}

int CDXUTSDKMesh::CreateFromFile(const char* szFileName)
{
	
	m_sFileName=szFileName;
    int hr = S_OK;
	stx_strlcpy(m_strPath, szFileName, MAX_PATH);
		int strLastSlash = int(stx_strrchr( m_strPath, '/' )-m_strPath);
		//if(!strLastSlash) return;
		m_strPath[strLastSlash]='\0';
		std::string StrPath;
                //LOG_PRINT("m_strPath=%s\n", m_strPath);
		//LOG_PRINT("m_sFileName=%s\n", m_sFileName.c_str());
    // Get the file size
    UINT cBytes = 0;
#if 1
  BYTE* pStaticMeshData = 0;
  ifstream file (m_sFileName.c_str(), ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    cBytes = file.tellg();
    pStaticMeshData = new BYTE[ cBytes ];
    file.seekg (0, ios::beg);
    file.read ((char*)pStaticMeshData, cBytes);
    file.close();
    cout << "the entire file content is in memory 1\n";
  }
  else cout << "Unable to open file 1\n";
#endif
        hr = CreateFromMemory( pStaticMeshData,
                               cBytes);
	LoadAnimation( m_sFileName.c_str() );
    return hr;
}

int CDXUTSDKMesh::CreateFromMemory( BYTE* pData,
                                        UINT DataBytes, bool bCopyStatic )
{

    int hr = E_FAIL;
    D3DXFROMWINEVECTOR3 lower; 
    D3DXFROMWINEVECTOR3 upper;

    // Set outstanding resources to zero
    m_NumOutstandingResources = 0;

    if( bCopyStatic )
	//if(1)
    {
        SDKMESH_HEADER* pHeader = ( SDKMESH_HEADER* )pData;

	#if 1
        SIZE_T StaticSize = ( SIZE_T )( pHeader->HeaderSize + pHeader->NonBufferDataSize );
	#else
        SIZE_T StaticSize = DataBytes;
	#endif
	//LOG_PRINT("StaticSize=%d\n", StaticSize);
        m_pHeapData = new BYTE[ StaticSize ];
        if( !m_pHeapData )
            return hr;

        m_pStaticMeshData = m_pHeapData;

        stx_memcpy( m_pStaticMeshData, pData, StaticSize );
    }
    else
    {
        m_pHeapData = pData;
        m_pStaticMeshData = pData;
    }

    // Pointer fixup
	
	//LOG_PRINT("m_pStaticMeshData=%x\n", m_pStaticMeshData);
    m_pMeshHeader = ( SDKMESH_HEADER* )m_pStaticMeshData;
	//LOG_PRINT("m_pMeshHeader=%x\n", m_pMeshHeader);
	
    m_pVertexBufferArray = ( SDKMESH_VERTEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                              m_pMeshHeader->VertexStreamHeadersOffset );
	
    m_pIndexBufferArray = ( SDKMESH_INDEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                            m_pMeshHeader->IndexStreamHeadersOffset );
	
    m_pMeshArray = ( SDKMESH_MESH* )( m_pStaticMeshData + m_pMeshHeader->MeshDataOffset );
	
    m_pSubsetArray = ( SDKMESH_SUBSET* )( m_pStaticMeshData + m_pMeshHeader->SubsetDataOffset );
	
    m_pFrameArray = ( SDKMESH_FRAME* )( m_pStaticMeshData + m_pMeshHeader->FrameDataOffset );
	
    m_pMaterialArray = ( SDKMESH_MATERIAL* )( m_pStaticMeshData + m_pMeshHeader->MaterialDataOffset );
	
#if 0
	//LOG_PRINT("m_pVertexBufferArray=%x\n", m_pVertexBufferArray);
	//LOG_PRINT("m_pIndexBufferArray=%x\n", m_pIndexBufferArray);
	//LOG_PRINT("m_pMeshArray=%x\n", m_pMeshArray);
	//LOG_PRINT("m_pSubsetArray=%x\n", m_pSubsetArray);
	//LOG_PRINT("m_pFrameArray=%x\n", m_pFrameArray);
	//LOG_PRINT("m_pMaterialArray=%x\n", m_pMaterialArray);
#else
	//LOG_PRINT("m_pMeshHeader->VertexStreamHeadersOffset=%x\n", m_pMeshHeader->VertexStreamHeadersOffset);
	//LOG_PRINT("m_pMeshHeader->IndexStreamHeadersOffset=%x\n", m_pMeshHeader->IndexStreamHeadersOffset);
	//LOG_PRINT("m_pMeshHeader->MeshDataOffset=%x\n", m_pMeshHeader->MeshDataOffset);
	//LOG_PRINT("m_pMeshHeader->SubsetDataOffset=%x\n", m_pMeshHeader->SubsetDataOffset);
	//LOG_PRINT("m_pMeshHeader->FrameDataOffset=%x\n", m_pMeshHeader->FrameDataOffset);
	//LOG_PRINT("m_pMeshHeader->MaterialDataOffset=%x\n", m_pMeshHeader->MaterialDataOffset);
	//LOG_PRINT("m_pSubsetArray=%x\n", m_pSubsetArray);
#endif
	
	//LOG_PRINT("m_pMeshHeader->NumMeshes=%d\n", m_pMeshHeader->NumMeshes);
    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
	//LOG_PRINT("m_pMeshArray[%d].SubsetOffset=%x\n", i, m_pMeshArray[i].SubsetOffset);
        m_pMeshArray[i].pSubsets = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].SubsetOffset );
	//LOG_PRINT("m_pMeshArray[%d].pSubsets=%x\n", i, m_pMeshArray[i].pSubsets);
        m_pMeshArray[i].pFrameInfluences = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].FrameInfluenceOffset );
    }

//stx_exit(0);

	
    // Setup buffer data pointer
    BYTE* pBufferData = 0;

    // Get the start of the buffer data
    UINT64 BufferDataStart = 0;

	
	//LOG_PRINT("SDKMESH_FILE_VERSION=%x\n", SDKMESH_FILE_VERSION);
	//LOG_PRINT("m_pMeshHeader->Version=%x\n", m_pMeshHeader->Version);
	//LOG_PRINT("m_pMeshHeader->IsBigEndian=%x\n", m_pMeshHeader->IsBigEndian);
    // error condition
    if( m_pMeshHeader->Version != SDKMESH_FILE_VERSION )
    {
        //LOG_PRINT("E_NOINTERFACE\n");
        //goto Error;
    }

	
    // Setup buffer data pointer
    pBufferData = pData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

	
    // Get the start of the buffer data
    BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Create VBs
    m_ppVertices = new BYTE*[m_pMeshHeader->NumVertexBuffers];
	UINT i = 0;
        BYTE* pVertices = NULL;
    for( i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[i].DataOffset - BufferDataStart ) );

            CreateVertexBuffer( &m_pVertexBufferArray[i], (void**)&pVertices );

        m_ppVertices[i] = pVertices;
    }
	pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[0].DataOffset - BufferDataStart ) );
	pVertices_=pVertices;

    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices_ = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );
	pIndices = pIndices_;
        CreateIndexBuffer( &m_pIndexBufferArray[i], pIndices );
        m_ppIndices[i] = pIndices;
    }

    // Load Materials
        LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials );
    hr = S_OK;

	if(pVertices_>=pIndices_)
	{
		LOG_PRINT("pVertices_>=pIndices_\n");
		LOG_PRINT("NumVertices=%d\n", NumVertices_);
		LOG_PRINT("NumIndices=%d\n", NumIndices_);
	}
    return hr;
}

