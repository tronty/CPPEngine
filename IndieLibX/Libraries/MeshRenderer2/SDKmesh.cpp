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
#if 0
#define LOG_PRINT(...) printf(__VA_ARGS__);
#define LOG_FNLN printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#else
#define LOG_PRINT(...)
#define LOG_FNLN
#endif

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::LoadMaterials( SDKMESH_MATERIAL* pMaterials, UINT numMaterials)
{
	UINT m=0;
	if(numMaterials)
	{
	pMaterials[m].pDiffuseTexture9=-1;
	pMaterials[m].pNormalTexture9=-1;   		pMaterials[m].pSpecularTexture9=-1;
	}
    char strPath[MAX_PATH];
    
    {
        for( m = 0; m < numMaterials; m++ )
        {
		LOG_FNLN;
		int strLastSlash = int(stx_strrchr( m_strPath, stx_getOsSeparator() )-m_strPath);
		m_strPath[strLastSlash]='\0';
                printf("m_strPath=%s\n", m_strPath);
            // load textures
            if( pMaterials[m].DiffuseTexture[0] != 0 )
            {
		stx_snprintf( strPath, MAX_PATH, "%s/%s_Diff.png", m_strPath, pMaterials[m].DiffuseTexture );
		stx_strlcpy(pMaterials[m].DiffuseTexture, strPath, MAX_TEXTURE_NAME);
               
if(stx_fileExists(strPath))  pMaterials[m].pDiffuseTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(strPath, false, IRenderer::GetRendererInstance()->Getlinear());
            }
            if( pMaterials[m].NormalTexture[0] != 0 )
            {
		stx_snprintf( strPath, MAX_PATH, "%s/%s_Diff.png", m_strPath, pMaterials[m].NormalTexture );
		stx_strlcpy(pMaterials[m].NormalTexture, strPath, MAX_TEXTURE_NAME);
                if(stx_fileExists(strPath))  pMaterials[m].pNormalTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(strPath, false, IRenderer::GetRendererInstance()->Getlinear());
            }
            if( pMaterials[m].SpecularTexture[0] != 0 )
            {
		stx_snprintf( strPath, MAX_PATH, "%s/%s_Diff.png", m_strPath, pMaterials[m].SpecularTexture );
	stx_strlcpy(pMaterials[m].SpecularTexture, strPath, MAX_TEXTURE_NAME);        if(stx_fileExists(strPath))  pMaterials[m].pSpecularTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(strPath, false, IRenderer::GetRendererInstance()->Getlinear());
            }

        }
    }
	m=0;
	printf("pMaterials[%d].DiffuseTexture=%x\n", m, pMaterials[m].pDiffuseTexture9);
	printf("pMaterials[%d].NormalTexture=%x\n", m, pMaterials[m].pNormalTexture9);   		
	printf("pMaterials[%d].SpecularTexture=%x\n", m, pMaterials[m].pSpecularTexture9);
	//stx_exit(0);
}

//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateVertexBuffer( SDKMESH_VERTEX_BUFFER_HEADER* pHeader,
                                          void* pVertices )
{
    int hr = S_OK;

    pHeader->DataOffset = 0;
	pHeader->pVB9=IRenderer::GetRendererInstance()->addVertexBuffer(( UINT )pHeader->SizeBytes, STATIC, (const void *) pVertices);

    return hr;
}

//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateIndexBuffer( SDKMESH_INDEX_BUFFER_HEADER* pHeader,
                                         void* pIndices )
{
    int hr = S_OK;

    pHeader->DataOffset = 0;

    STX_INDEXFORMAT ibFormat = CONSTANT_INDEX2;
    switch( pHeader->IndexType )
    {
        case IT_16BIT:
            ibFormat = CONSTANT_INDEX2;
            break;
        case IT_32BIT:
            ibFormat = CONSTANT_INDEX4;
            break;
    };

	pHeader->pIB9=IRenderer::GetRendererInstance()->addIndexBuffer(( UINT )pHeader->SizeBytes/ibFormat, ibFormat, STATIC, (const void *) pIndices);

    return hr;
}

//--------------------------------------------------------------------------------------
// transform bind pose frame using a recursive traversal
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::TransformBindPoseFrame( UINT iFrame, D3DXFROMWINEMATRIX* pParentWorld )
{
    if( !m_pBindPoseFrameMatrices )
        return;

    // Transform ourselves
    D3DXFROMWINEMATRIX LocalWorld;
    D3DXFROMWINEMatrixMultiply( &LocalWorld, &m_pFrameArray[iFrame].Matrix, pParentWorld );
    m_pBindPoseFrameMatrices[iFrame] = LocalWorld;

    // Transform our siblings
    if( m_pFrameArray[iFrame].SiblingFrame != INVALID_FRAME )
        TransformBindPoseFrame( m_pFrameArray[iFrame].SiblingFrame, pParentWorld );

    // Transform our children
    if( m_pFrameArray[iFrame].ChildFrame != INVALID_FRAME )
        TransformBindPoseFrame( m_pFrameArray[iFrame].ChildFrame, &LocalWorld );
}

//--------------------------------------------------------------------------------------
// transform frame using a recursive traversal
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::TransformFrame( UINT iFrame, D3DXFROMWINEMATRIX* pParentWorld, double fTime )
{
    // Get the tick data
    D3DXFROMWINEMATRIX LocalTransform;
    UINT iTick = GetAnimationKeyFromTime( fTime );

    if( INVALID_ANIMATION_DATA != m_pFrameArray[iFrame].AnimationDataIndex )
    {
        SDKANIMATION_FRAME_DATA* pFrameData = &m_pAnimationFrameData[ m_pFrameArray[iFrame].AnimationDataIndex ];
        SDKANIMATION_DATA* pData = &pFrameData->pAnimationData[ iTick ];

        // turn it into a matrix (Ignore scaling for now)
        D3DXFROMWINEVECTOR3 parentPos = pData->Translation;
        D3DXFROMWINEMATRIX mTranslate;
        D3DXFROMWINEMatrixTranslation( &mTranslate, parentPos.x, parentPos.y, parentPos.z );

        D3DXFROMWINEQUATERNION quat;
        D3DXFROMWINEMATRIX mQuat;
        quat.w = pData->Orientation.w;
        quat.x = pData->Orientation.x;
        quat.y = pData->Orientation.y;
        quat.z = pData->Orientation.z;
        if( quat.w == 0 && quat.x == 0 && quat.y == 0 && quat.z == 0 )
            D3DXFROMWINEQuaternionIdentity( &quat );
        D3DXFROMWINEQuaternionNormalize( &quat, &quat );
        D3DXFROMWINEMatrixRotationQuaternion( &mQuat, &quat );
        LocalTransform = ( mQuat * mTranslate );
    }
    else
    {
        LocalTransform = m_pFrameArray[iFrame].Matrix;
    }

    // Transform ourselves
    D3DXFROMWINEMATRIX LocalWorld;
    D3DXFROMWINEMatrixMultiply( &LocalWorld, &LocalTransform, pParentWorld );
    m_pTransformedFrameMatrices[iFrame] = LocalWorld;

    // Transform our siblings
    if( m_pFrameArray[iFrame].SiblingFrame != INVALID_FRAME )
        TransformFrame( m_pFrameArray[iFrame].SiblingFrame, pParentWorld, fTime );

    // Transform our children
    if( m_pFrameArray[iFrame].ChildFrame != INVALID_FRAME )
        TransformFrame( m_pFrameArray[iFrame].ChildFrame, &LocalWorld, fTime );
}

//--------------------------------------------------------------------------------------
// transform frame assuming that it is an absolute transformation
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::TransformFrameAbsolute( UINT iFrame, double fTime )
{
    D3DXFROMWINEMATRIX mTrans1;
    D3DXFROMWINEMATRIX mTrans2;
    D3DXFROMWINEMATRIX mRot1;
    D3DXFROMWINEMATRIX mRot2;
    D3DXFROMWINEQUATERNION quat1;
    D3DXFROMWINEQUATERNION quat2;
    D3DXFROMWINEMATRIX mTo;
    D3DXFROMWINEMATRIX mInvTo;
    D3DXFROMWINEMATRIX mFrom;

    UINT iTick = GetAnimationKeyFromTime( fTime );

    if( INVALID_ANIMATION_DATA != m_pFrameArray[iFrame].AnimationDataIndex )
    {
        SDKANIMATION_FRAME_DATA* pFrameData = &m_pAnimationFrameData[ m_pFrameArray[iFrame].AnimationDataIndex ];
        SDKANIMATION_DATA* pData = &pFrameData->pAnimationData[ iTick ];
        SDKANIMATION_DATA* pDataOrig = &pFrameData->pAnimationData[ 0 ];

        D3DXFROMWINEMatrixTranslation( &mTrans1, -pDataOrig->Translation.x,
                               -pDataOrig->Translation.y,
                               -pDataOrig->Translation.z );
        D3DXFROMWINEMatrixTranslation( &mTrans2, pData->Translation.x,
                               pData->Translation.y,
                               pData->Translation.z );

        quat1.x = pDataOrig->Orientation.x;
        quat1.y = pDataOrig->Orientation.y;
        quat1.z = pDataOrig->Orientation.z;
        quat1.w = pDataOrig->Orientation.w;
        D3DXFROMWINEQuaternionInverse( &quat1, &quat1 );
        D3DXFROMWINEMatrixRotationQuaternion( &mRot1, &quat1 );
        mInvTo = mTrans1 * mRot1;

        quat2.x = pData->Orientation.x;
        quat2.y = pData->Orientation.y;
        quat2.z = pData->Orientation.z;
        quat2.w = pData->Orientation.w;
        D3DXFROMWINEMatrixRotationQuaternion( &mRot2, &quat2 );
        mFrom = mRot2 * mTrans2;

        D3DXFROMWINEMATRIX mOutput = mInvTo * mFrom;
        m_pTransformedFrameMatrices[iFrame] = mOutput;
    }
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::RenderMesh( UINT iMesh,                    
                               ShaderID pEffect,
				VertexFormatID vf,
                               //D3DXFROMWINEHANDLE hTechnique,
                               const char* htxDiffuse,
                               const char* htxNormal,
                               const char* htxSpecular )
{
    if( 0 < GetOutstandingBufferResources() )
        return;

    // Render the scene with this technique 
    //pEffect->SetTechnique( hTechnique );
		if(pEffect>-1)IRenderer::GetRendererInstance()->setShader(pEffect);
		if(vf>-1)IRenderer::GetRendererInstance()->setVertexFormat(vf);

    SDKMESH_MESH* pMesh = &m_pMeshArray[iMesh];

    // set vb streams
    for( UINT i = 0; i < ( UINT )pMesh->NumVertexBuffers; i++ )
    {
        IRenderer::GetRendererInstance()->setVertexBuffer( i,
                                     m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].pVB9);                                    
                                     //( UINT )m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].StrideBytes );
    }

    // Set our index buffer as well
    IRenderer::GetRendererInstance()->setIndexBuffer( m_pIndexBufferArray[ pMesh->IndexBuffer ].pIB9 );

    SDKMESH_SUBSET* pSubset = 0;
    SDKMESH_MATERIAL* pMat = 0;
    Primitives PrimType; /*
    UINT cPasses = 0;
    pEffect->Begin( &cPasses, 0 );

    for( UINT p = 0; p < cPasses; ++p ) */
    {
      //  pEffect->BeginPass( p );
        for( UINT subset = 0; subset < pMesh->NumSubsets; subset++ ) 
        {
		//UINT subset = 0;
            pSubset = &m_pSubsetArray[ pMesh->pSubsets[subset] ];

            PrimType = GetPrimitiveType9( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );

            if( INVALID_MATERIAL != pSubset->MaterialID && m_pMeshHeader->NumMaterials > 0 )
            {
                pMat = &m_pMaterialArray[ pSubset->MaterialID ];
                if( htxDiffuse && ( pMat->pDiffuseTexture9 > -1 ) )
                    IRenderer::GetRendererInstance()->setTexture( htxDiffuse, pMat->pDiffuseTexture9 );
                if( htxNormal && ( pMat->pNormalTexture9 > -1 ) )
                    IRenderer::GetRendererInstance()->setTexture( htxNormal, pMat->pNormalTexture9 );
                if( htxSpecular && ( pMat->pSpecularTexture9 > -1 ) )
                    IRenderer::GetRendererInstance()->setTexture( htxSpecular, pMat->pSpecularTexture9 );
            }

            //pEffect->CommitChanges();

            UINT PrimCount = ( UINT )pSubset->IndexCount;
            UINT IndexStart = ( UINT )pSubset->IndexStart;
            UINT VertexStart = ( UINT )pSubset->VertexStart;
            UINT VertexCount = ( UINT )pSubset->VertexCount;
            if(PRIM_TRIANGLES == PrimType )
                PrimCount /= 3;
            if(PRIM_LINES == PrimType )
                PrimCount /= 2;
            if(PRIM_TRIANGLE_STRIP == PrimType )
                PrimCount = ( PrimCount - 3 ) + 1;
            if(PRIM_LINE_STRIP == PrimType )
                PrimCount -= 1;

            IRenderer::GetRendererInstance()->DrawIndexedPrimitive( PrimType, VertexStart, 0, VertexCount, IndexStart, PrimCount );
        }

        //pEffect->EndPass();
    }

    //pEffect->End();
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::RenderFrame( UINT iFrame,
                                ShaderID pEffect,
				VertexFormatID vf,
                                //D3DXFROMWINEHANDLE hTechnique,
                               const char* htxDiffuse,
                               const char* htxNormal,
                               const char* htxSpecular )
{
    if( !m_pStaticMeshData || !m_pFrameArray )
        return;
	
    if(m_pMeshHeader->NumFrames<=iFrame)
	return;
    LOG_PRINT("iFrame=%d\n", iFrame);

    if( m_pFrameArray[iFrame].Mesh != INVALID_MESH )
    {
        RenderMesh( m_pFrameArray[iFrame].Mesh,
                    
                    pEffect, vf,
                    //hTechnique,
                    htxDiffuse,
                    htxNormal,
                    htxSpecular );
    }

    // Render our children
    if( m_pFrameArray[iFrame].ChildFrame != INVALID_FRAME )
        RenderFrame( m_pFrameArray[iFrame].ChildFrame, pEffect, //hTechnique
			vf, htxDiffuse, htxNormal,
                     htxSpecular );

    // Render our siblings
    if( m_pFrameArray[iFrame].SiblingFrame != INVALID_FRAME )
        RenderFrame( m_pFrameArray[iFrame].SiblingFrame, pEffect, //hTechnique
			vf, htxDiffuse, htxNormal,
                     htxSpecular );
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
    // Get the file size
    UINT cBytes = 0;
	FILE* file = fopen( szFileName, "r" );
	if ( file )
	{
		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		fseek( file, 0, SEEK_END );
		cBytes = ftell( file );
		fseek( file, 0, SEEK_SET );
		// Strange case, but good to handle up front.
		if ( cBytes == 0 )
		{
			fclose( file );
			return -1;
		}

		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.
		// ??? m_pAnimationData = new BYTE[ ( size_t )( sizeof( SDKANIMATION_FILE_HEADER ) + fileheader.AnimationDataSize ) ];
		m_pAnimationData = new BYTE[cBytes];

		const int BUF_SIZE = 2048;
		char buf[BUF_SIZE];
		int j=0;
		while( fgets( buf, BUF_SIZE, file ) )
		{
			for(unsigned int i=0;i<BUF_SIZE;i++)
				m_pAnimationData[j+i] = buf[i];
			j++;
		}
		fclose( file );
	}

    /////////////////////////
    // pointer fixup
    m_pAnimationHeader = ( SDKANIMATION_FILE_HEADER* )m_pAnimationData;
    m_pAnimationFrameData = ( SDKANIMATION_FRAME_DATA* )( m_pAnimationData + m_pAnimationHeader->AnimationDataOffset );

    UINT64 BaseOffset = sizeof( SDKANIMATION_FILE_HEADER );
    for( UINT i = 0; i < m_pAnimationHeader->NumFrames; i++ )
    {
        m_pAnimationFrameData[i].pAnimationData = ( SDKANIMATION_DATA* )( m_pAnimationData +
                                                                          m_pAnimationFrameData[i].DataOffset +
                                                                          BaseOffset );
        SDKMESH_FRAME* pFrame = FindFrame( m_pAnimationFrameData[i].FrameName );
        if( pFrame )
        {
            pFrame->AnimationDataIndex = i;
        }
    }

    hr = S_OK;
Error:
    //CloseHandle( hFile );
    return hr;
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::Destroy()
{
#if 0 // ???
    if( !CheckLoadDone() )
        return;

    if( m_pStaticMeshData )
    {
        if( m_pMaterialArray )
        {
            for( UINT64 m = 0; m < m_pMeshHeader->NumMaterials; m++ )
            {
                {
                    if( !IsErrorResource( m_pMaterialArray[m].pDiffuseTexture9 ) )
                        SAFE_RELEASE( m_pMaterialArray[m].pDiffuseTexture9 );
                    if( !IsErrorResource( m_pMaterialArray[m].pNormalTexture9 ) )
                        SAFE_RELEASE( m_pMaterialArray[m].pNormalTexture9 );
                    if( !IsErrorResource( m_pMaterialArray[m].pSpecularTexture9 ) )
                        SAFE_RELEASE( m_pMaterialArray[m].pSpecularTexture9 );
                }
            }
        }

        for( UINT64 i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
        {
            if( !IsErrorResource( m_pVertexBufferArray[i].pVB9 ) )
                SAFE_RELEASE( m_pVertexBufferArray[i].pVB9 );
        }

        for( UINT64 i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
        {
            if( !IsErrorResource( m_pIndexBufferArray[i].pIB9 ) )
                SAFE_RELEASE( m_pIndexBufferArray[i].pIB9 );
        }
    }

    SAFE_DELETE_ARRAY( m_pAdjacencyIndexBufferArray );

    SAFE_DELETE_ARRAY( m_pHeapData );
    m_pStaticMeshData = 0;
    SAFE_DELETE_ARRAY( m_pAnimationData );
    SAFE_DELETE_ARRAY( m_pBindPoseFrameMatrices );
    SAFE_DELETE_ARRAY( m_pTransformedFrameMatrices );

    SAFE_DELETE_ARRAY( m_ppVertices );
    SAFE_DELETE_ARRAY( m_ppIndices );

    m_pMeshHeader = 0;
    m_pVertexBufferArray = 0;
    m_pIndexBufferArray = 0;
    m_pMeshArray = 0;
    m_pSubsetArray = 0;
    m_pFrameArray = 0;
    m_pMaterialArray = 0;

    m_pAnimationHeader = 0;
    m_pAnimationFrameData = 0;
#endif
}

//--------------------------------------------------------------------------------------
// transform the bind pose
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::TransformBindPose( D3DXFROMWINEMATRIX* pWorld )
{
    TransformBindPoseFrame( 0, pWorld );
}

//--------------------------------------------------------------------------------------
// transform the mesh frames according to the animation for time fTime
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::TransformMesh( D3DXFROMWINEMATRIX* pWorld, double fTime )
{
    if( !m_pAnimationHeader )
        return;

    if( FTT_RELATIVE == m_pAnimationHeader->FrameTransformType )
    {
        TransformFrame( 0, pWorld, fTime );

        // For each frame, move the transform to the bind pose, then
        // move it to the final position
        D3DXFROMWINEMATRIX mInvBindPose;
        D3DXFROMWINEMATRIX mFinal;
        for( UINT i = 0; i < m_pMeshHeader->NumFrames; i++ )
        {
            D3DXFROMWINEMatrixInverse( &mInvBindPose, 0, &m_pBindPoseFrameMatrices[i] );
            mFinal = mInvBindPose * m_pTransformedFrameMatrices[i];
            m_pTransformedFrameMatrices[i] = mFinal;
        }
    }
    else if( FTT_ABSOLUTE == m_pAnimationHeader->FrameTransformType )
    {
        for( UINT i = 0; i < m_pAnimationHeader->NumFrames; i++ )
            TransformFrameAbsolute( i, fTime );
    }
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::Render( 
                           ShaderID pEffect,
			VertexFormatID vf,
                           //D3DXFROMWINEHANDLE hTechnique,
                           const char* htxDiffuse,
                           const char* htxNormal,
                           const char* htxSpecular )
{
    RenderFrame( 0, pEffect, //hTechnique
			vf, htxDiffuse, htxNormal, htxSpecular );
}

//--------------------------------------------------------------------------------------
Primitives CDXUTSDKMesh::GetPrimitiveType9( SDKMESH_PRIMITIVE_TYPE PrimType )
{
    Primitives retType =PRIM_TRIANGLES;

    switch( PrimType )
    {
        case PT_TRIANGLE_LIST:
            retType =PRIM_TRIANGLES;
            break;
        case PT_TRIANGLE_STRIP:
            retType =PRIM_TRIANGLE_STRIP;
            break;
        case PT_LINE_LIST:
            retType =PRIM_LINES;
            break;
        case PT_LINE_STRIP:
            retType =PRIM_LINE_STRIP;
            break;
        case PT_POINT_LIST:
            retType =PRIM_POINTS;
            break;
    };

    return retType;
}

//--------------------------------------------------------------------------------------
D3DFORMAT CDXUTSDKMesh::GetIBFormat9( UINT iMesh )
{
    switch( m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].IndexType )
    {
        case IT_16BIT:
            return D3DFMT_INDEX16;
        case IT_32BIT:
            return D3DFMT_INDEX32;
    };
    return D3DFMT_INDEX16;
}

//--------------------------------------------------------------------------------------
VertexBufferID CDXUTSDKMesh::GetVB9( UINT iMesh, UINT iVB )
{
    return m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].pVB9;
}

//--------------------------------------------------------------------------------------
IndexBufferID CDXUTSDKMesh::GetIB9( UINT iMesh )
{
    return m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].pIB9;
}

//--------------------------------------------------------------------------------------
char* CDXUTSDKMesh::GetMeshPathA()
{
    return m_strPath;
}

//--------------------------------------------------------------------------------------
char* CDXUTSDKMesh::GetMeshPathW()
{
    return m_strPath;
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetNumMeshes()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumMeshes;
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetNumMaterials()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumMaterials;
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetNumVBs()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumVertexBuffers;
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetNumIBs()
{
    if( !m_pMeshHeader )
        return 0;
    return m_pMeshHeader->NumIndexBuffers;
}

//--------------------------------------------------------------------------------------
VertexBufferID CDXUTSDKMesh::GetVB9At( UINT iVB )
{
    return m_pVertexBufferArray[ iVB ].pVB9;
}

//--------------------------------------------------------------------------------------
IndexBufferID CDXUTSDKMesh::GetIB9At( UINT iIB )
{
    return m_pIndexBufferArray[ iIB ].pIB9;
}

//--------------------------------------------------------------------------------------
BYTE* CDXUTSDKMesh::GetRawVerticesAt( UINT iVB )
{
    return m_ppVertices[iVB];
}

//--------------------------------------------------------------------------------------
BYTE* CDXUTSDKMesh::GetRawIndicesAt( UINT iIB )
{
    return m_ppIndices[iIB];
}

//--------------------------------------------------------------------------------------
SDKMESH_MATERIAL* CDXUTSDKMesh::GetMaterial( UINT iMaterial )
{
    return &m_pMaterialArray[ iMaterial ];
}

//--------------------------------------------------------------------------------------
SDKMESH_MESH* CDXUTSDKMesh::GetMesh( UINT iMesh )
{
    return &m_pMeshArray[ iMesh ];
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetNumSubsets( UINT iMesh )
{
    return m_pMeshArray[ iMesh ].NumSubsets;
}

//--------------------------------------------------------------------------------------
SDKMESH_SUBSET* CDXUTSDKMesh::GetSubset( UINT iMesh, UINT iSubset )
{
    return &m_pSubsetArray[ m_pMeshArray[ iMesh ].pSubsets[iSubset] ];
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetVertexStride( UINT iMesh, UINT iVB )
{
    return ( UINT )m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].StrideBytes;
}

//--------------------------------------------------------------------------------------
SDKMESH_FRAME* CDXUTSDKMesh::FindFrame( char* pszName )
{
    for( UINT i = 0; i < m_pMeshHeader->NumFrames; i++ )
    {
        if( stx_strcasecmp( m_pFrameArray[i].Name, pszName ) == 0 )
        {
            return &m_pFrameArray[i];
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------------
UINT64 CDXUTSDKMesh::GetNumVertices( UINT iMesh, UINT iVB )
{
    return m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices;
}

//--------------------------------------------------------------------------------------
UINT64 CDXUTSDKMesh::GetNumIndices( UINT iMesh )
{
    return m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].NumIndices;
}

//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR3 CDXUTSDKMesh::GetMeshBBoxCenter( UINT iMesh )
{
    return m_pMeshArray[iMesh].BoundingBoxCenter;
}

//--------------------------------------------------------------------------------------
D3DXFROMWINEVECTOR3 CDXUTSDKMesh::GetMeshBBoxExtents( UINT iMesh )
{
    return m_pMeshArray[iMesh].BoundingBoxExtents;
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetOutstandingResources()
{
    UINT outstandingResources = 0;
    if( !m_pMeshHeader )
        return 1;

    outstandingResources += GetOutstandingBufferResources();

    {
        for( UINT i = 0; i < m_pMeshHeader->NumMaterials; i++ )
        {
            if( m_pMaterialArray[i].DiffuseTexture[0] != 0 )
            {
                if( !m_pMaterialArray[i].pDiffuseTexture9 && !IsErrorResource( m_pMaterialArray[i].pDiffuseTexture9 ) )
                    outstandingResources ++;
            }

            if( m_pMaterialArray[i].NormalTexture[0] != 0 )
            {
                if( !m_pMaterialArray[i].pNormalTexture9 && !IsErrorResource( m_pMaterialArray[i].pNormalTexture9 ) )
                    outstandingResources ++;
            }

            if( m_pMaterialArray[i].SpecularTexture[0] != 0 )
            {
                if( !m_pMaterialArray[i].pSpecularTexture9 &&
                    !IsErrorResource( m_pMaterialArray[i].pSpecularTexture9 ) )
                    outstandingResources ++;
            }
        }
    }

    return outstandingResources;
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetOutstandingBufferResources()
{
    UINT outstandingResources = 0;
    if( !m_pMeshHeader )
        return 1;

    for( UINT i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        if( !m_pVertexBufferArray[i].pVB9 && !IsErrorResource( m_pVertexBufferArray[i].pVB9 ) )
            outstandingResources ++;
    }

    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        if( !m_pIndexBufferArray[i].pIB9 && !IsErrorResource( m_pIndexBufferArray[i].pIB9 ) )
            outstandingResources ++;
    }

    return outstandingResources;
}

//--------------------------------------------------------------------------------------
bool CDXUTSDKMesh::CheckLoadDone()
{
    if( 0 == GetOutstandingResources() )
    {
        m_bLoading = false;
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------
bool CDXUTSDKMesh::IsLoaded()
{
    if( m_pStaticMeshData && !m_bLoading )
    {
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------
bool CDXUTSDKMesh::IsLoading()
{
    return m_bLoading;
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::SetLoading( bool bLoading )
{
    m_bLoading = bLoading;
}

//--------------------------------------------------------------------------------------
BOOL CDXUTSDKMesh::HadLoadingError()
{
    if( m_pMeshHeader )
    {
        for( UINT i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
        {
            if( IsErrorResource( m_pVertexBufferArray[i].pVB9 ) )
                return TRUE;
        }

        for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
        {
            if( IsErrorResource( m_pIndexBufferArray[i].pIB9 ) )
                return TRUE;
        }
    }

    return FALSE;
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetNumInfluences( UINT iMesh )
{
    return m_pMeshArray[iMesh].NumFrameInfluences;
}

//--------------------------------------------------------------------------------------
const D3DXFROMWINEMATRIX* CDXUTSDKMesh::GetMeshInfluenceMatrix( UINT iMesh, UINT iInfluence )
{
    UINT iFrame = m_pMeshArray[iMesh].pFrameInfluences[ iInfluence ];
    return &m_pTransformedFrameMatrices[iFrame];
}

//--------------------------------------------------------------------------------------
UINT CDXUTSDKMesh::GetAnimationKeyFromTime( double fTime )
{
    UINT iTick = ( UINT )( m_pAnimationHeader->AnimationFPS * fTime );

    iTick = iTick % ( m_pAnimationHeader->NumAnimationKeys - 1 );
    iTick ++;

    return iTick;
}

#if 0
//-------------------------------------------------------------------------------------
// CDXUTXFileMesh implementation.
//-------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CDXUTXFileMesh::CDXUTXFileMesh( const char* strName )
{
    wcscpy_s( m_strName, 512, strName );
    m_pMesh = 0;
    m_pMaterials = 0;
    m_pTextures = 0;
    m_bUseMaterials = TRUE;
    m_pVB = 0;
    m_pIB = 0;
    m_pDecl = 0;
    m_strMaterials = 0;
    m_dwNumMaterials = 0;
    m_dwNumVertices = 0;
    m_dwNumFaces = 0;
    m_dwBytesPerVertex = 0;
}




//-----------------------------------------------------------------------------
CDXUTXFileMesh::~CDXUTXFileMesh()
{
    Destroy();
}




//-----------------------------------------------------------------------------
int CDXUTXFileMesh::Create( const char* strFilename )
{
    char* strPath[MAX_PATH];
    LPD3DXBUFFER pAdjacencyBuffer = 0;
    LPD3DXBUFFER pMtrlBuffer = 0;
    int hr;

    // Cleanup previous mesh if any
    Destroy();

    // Find the path for the file, and convert it to ANSI (for the D3DX API)
    DXUTFindDXSDKMediaFileCch( strPath, sizeof( strPath ) / sizeof( char* ), strFilename );

    // Load the mesh
    if( FAILED( hr = D3DXLoadMeshFromX( strPath, D3DXMESH_MANAGED, 
                                        &pAdjacencyBuffer, &pMtrlBuffer, 0,
                                        &m_dwNumMaterials, &m_pMesh ) ) )
    {
        return hr;
    }

    // Optimize the mesh for performance
    if( FAILED( hr = m_pMesh->OptimizeInplace(
                D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                ( __DWORD__* )pAdjacencyBuffer->GetBufferPointer(), 0, 0, 0 ) ) )
    {
        SAFE_RELEASE( pAdjacencyBuffer );
        SAFE_RELEASE( pMtrlBuffer );
        return hr;
    }

    // Set strPath to the path of the mesh file
    char** pLastBSlash = wcsrchr( strPath, L'\\' );
    if( pLastBSlash )
        *( pLastBSlash + 1 ) = L'\0';
    else
        *strPath = L'\0';

    D3DXFROMWINEMATERIAL* d3dxMtrls = ( D3DXFROMWINEMATERIAL* )pMtrlBuffer->GetBufferPointer();
    hr = CreateMaterials( strPath, d3dxMtrls, m_dwNumMaterials );

    SAFE_RELEASE( pAdjacencyBuffer );
    SAFE_RELEASE( pMtrlBuffer );

    // Extract data from m_pMesh for easy access
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    m_dwNumVertices = m_pMesh->GetNumVertices();
    m_dwNumFaces = m_pMesh->GetNumFaces();
    m_dwBytesPerVertex = m_pMesh->GetNumBytesPerVertex();
    m_pMesh->GetIndexBuffer( &m_pIB );
    m_pMesh->GetVertexBuffer( &m_pVB );
    m_pMesh->GetDeclaration( decl );
    IRenderer::GetRendererInstance()->CreateVertexDeclaration( decl, &m_pDecl );

    return hr;
}


//-----------------------------------------------------------------------------
int CDXUTXFileMesh::Create( 
                                LPD3DXFILEDATA pFileData )
{
    LPD3DXBUFFER pMtrlBuffer = 0;
    LPD3DXBUFFER pAdjacencyBuffer = 0;
    int hr;

    // Cleanup previous mesh if any
    Destroy();

    // Load the mesh from the DXFILEDATA object
    if( FAILED( hr = D3DXLoadMeshFromXof( pFileData, D3DXMESH_MANAGED, 
                                          &pAdjacencyBuffer, &pMtrlBuffer, 0,
                                          &m_dwNumMaterials, &m_pMesh ) ) )
    {
        return hr;
    }

    // Optimize the mesh for performance
    if( FAILED( hr = m_pMesh->OptimizeInplace(
                D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                ( __DWORD__* )pAdjacencyBuffer->GetBufferPointer(), 0, 0, 0 ) ) )
    {
        SAFE_RELEASE( pAdjacencyBuffer );
        SAFE_RELEASE( pMtrlBuffer );
        return hr;
    }

    D3DXFROMWINEMATERIAL* d3dxMtrls = ( D3DXFROMWINEMATERIAL* )pMtrlBuffer->GetBufferPointer();
    hr = CreateMaterials( "", d3dxMtrls, m_dwNumMaterials );

    SAFE_RELEASE( pAdjacencyBuffer );
    SAFE_RELEASE( pMtrlBuffer );

    // Extract data from m_pMesh for easy access
    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    m_dwNumVertices = m_pMesh->GetNumVertices();
    m_dwNumFaces = m_pMesh->GetNumFaces();
    m_dwBytesPerVertex = m_pMesh->GetNumBytesPerVertex();
    m_pMesh->GetIndexBuffer( &m_pIB );
    m_pMesh->GetVertexBuffer( &m_pVB );
    m_pMesh->GetDeclaration( decl );
    IRenderer::GetRendererInstance()->CreateVertexDeclaration( decl, &m_pDecl );

    return hr;
}


//-----------------------------------------------------------------------------
int CDXUTXFileMesh::Create( MeshRenderer2* pInMesh,
                                D3DXFROMWINEMATERIAL* pd3dxMaterials, __DWORD__ dwMaterials )
{
    // Cleanup previous mesh if any
    Destroy();

    // Optimize the mesh for performance
    __DWORD__* rgdwAdjacency = 0;
    rgdwAdjacency = new __DWORD__[pInMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == 0 )
        return E_OUTOFMEMORY;
    pInMesh->GenerateAdjacency( 1e-6f, rgdwAdjacency );

    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    pInMesh->GetDeclaration( decl );

    __DWORD__ dwOptions = pInMesh->GetOptions();
    dwOptions &= ~( D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM | D3DXMESH_WRITEONLY );
    dwOptions |= D3DXMESH_MANAGED;
    dwOptions |= D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE;

    MeshRenderer2* pTempMesh = 0;
    if( FAILED( pInMesh->Optimize( dwOptions, rgdwAdjacency, 0, 0, 0, &pTempMesh ) ) )
    {
        SAFE_DELETE_ARRAY( rgdwAdjacency );
        return E_FAIL;
    }

    SAFE_DELETE_ARRAY( rgdwAdjacency );
    SAFE_RELEASE( m_pMesh );
    m_pMesh = pTempMesh;

    int hr;
    hr = CreateMaterials( "", pd3dxMaterials, dwMaterials );

    // Extract data from m_pMesh for easy access
    m_dwNumVertices = m_pMesh->GetNumVertices();
    m_dwNumFaces = m_pMesh->GetNumFaces();
    m_dwBytesPerVertex = m_pMesh->GetNumBytesPerVertex();
    m_pMesh->GetIndexBuffer( &m_pIB );
    m_pMesh->GetVertexBuffer( &m_pVB );
    m_pMesh->GetDeclaration( decl );
    IRenderer::GetRendererInstance()->CreateVertexDeclaration( decl, &m_pDecl );

    return hr;
}


//-----------------------------------------------------------------------------
int CDXUTXFileMesh::CreateMaterials( const char* strPath, D3DXFROMWINEMATERIAL* d3dxMtrls,
                                         __DWORD__ dwNumMaterials )
{
    // Get material info for the mesh
    // Get the array of materials out of the buffer
    m_dwNumMaterials = dwNumMaterials;
    if( d3dxMtrls && m_dwNumMaterials > 0 )
    {
        // Allocate memory for the materials and textures
        m_pMaterials = new D3DFROMWINEMATERIAL9[m_dwNumMaterials];
        if( m_pMaterials == 0 )
            return E_OUTOFMEMORY;
        m_pTextures = new LPDIRECT3DBASETEXTURE9[m_dwNumMaterials];
        if( m_pTextures == 0 )
            return E_OUTOFMEMORY;
        m_strMaterials = new CHAR[m_dwNumMaterials][MAX_PATH];
        if( m_strMaterials == 0 )
            return E_OUTOFMEMORY;

        // Copy each material and create its texture
        for( __DWORD__ i = 0; i < m_dwNumMaterials; i++ )
        {
            // Copy the material
            m_pMaterials[i] = d3dxMtrls[i].MatD3D;
            m_pTextures[i] = 0;

            // Create a texture
            if( d3dxMtrls[i].pTextureFilename )
            {
                strcpy_s( m_strMaterials[i], MAX_PATH, d3dxMtrls[i].pTextureFilename );

                char* strTexture[MAX_PATH];
                char* strTextureTemp[MAX_PATH];
                D3DXIMAGE_INFO ImgInfo;

                // First attempt to look for texture in the same folder as the input folder.
                MultiByteToWideChar( CP_ACP, 0, d3dxMtrls[i].pTextureFilename, -1, strTextureTemp, MAX_PATH );
                strTextureTemp[MAX_PATH - 1] = 0;

                wcscpy_s( strTexture, MAX_PATH, strPath );
                wcscat_s( strTexture, MAX_PATH, strTextureTemp );

                // Inspect the texture file to determine the texture type.
                if( FAILED( D3DXGetImageInfoFromFile( strTexture, &ImgInfo ) ) )
                {
                    // Search the media folder
                    if( FAILED( DXUTFindDXSDKMediaFileCch( strTexture, MAX_PATH, strTextureTemp ) ) )
                        continue;  // Can't find. Skip.

                    D3DXGetImageInfoFromFile( strTexture, &ImgInfo );
                }

                // Call the appropriate loader according to the texture type.
                switch( ImgInfo.ResourceType )
                {
                    case D3DRTYPE_TEXTURE:
                    {
                        TextureID* pTex;
                        *pTex=addImageLibTexture( strTexture );
                        {
                            // Obtain the base texture interface
                            pTex->QueryInterface( IID_TextureID, ( LPVOID* )&m_pTextures[i] );
                            // Release the specialized instance
                            pTex->Release();
                        }
                        break;
                    }
                    case D3DRTYPE_CUBETEXTURE:
                    {
                        CubeTexture9* pTex;
                        if( SUCCEEDED( D3DXCreateCubeTextureFromFile( strTexture, &pTex ) ) )
                        {
                            // Obtain the base texture interface
                            pTex->QueryInterface( IID_TextureID, ( LPVOID* )&m_pTextures[i] );
                            // Release the specialized instance
                            pTex->Release();
                        }
                        break;
                    }
                    case D3DRTYPE_VOLUMETEXTURE:
                    {
                        VolumeTexture9* pTex;
                        if( SUCCEEDED( D3DXCreateVolumeTextureFromFile( strTexture, &pTex ) ) )
                        {
                            // Obtain the base texture interface
                            pTex->QueryInterface( IID_TextureID, ( LPVOID* )&m_pTextures[i] );
                            // Release the specialized instance
                            pTex->Release();
                        }
                        break;
                    }
                }
            }
        }
    }
    return S_OK;
}


//-----------------------------------------------------------------------------
int CDXUTXFileMesh::SetFVF( __DWORD__ dwFVF )
{
    MeshRenderer2* pTempMesh = 0;

    if( m_pMesh )
    {
        if( FAILED( m_pMesh->CloneMeshFVF( m_pMesh->GetOptions(), dwFVF,
                                           &pTempMesh ) ) )
        {
            SAFE_RELEASE( pTempMesh );
            return E_FAIL;
        }

        __DWORD__ dwOldFVF = 0;
        dwOldFVF = m_pMesh->GetFVF();
        SAFE_RELEASE( m_pMesh );
        m_pMesh = pTempMesh;

        // Compute normals if they are being requested and
        // the old mesh does not have them.
        if( !( dwOldFVF & D3DFVF_NORMAL ) && dwFVF & D3DFVF_NORMAL )
        {
            D3DXComputeNormals( m_pMesh, 0 );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Convert the mesh to the format specified by the given vertex declarations.
//-----------------------------------------------------------------------------
int CDXUTXFileMesh::SetVertexDecl( const D3DVERTEXELEMENT9* pDecl,
                                       bool bAutoComputeNormals, bool bAutoComputeTangents,
                                       bool bSplitVertexForOptimalTangents )
{
    MeshRenderer2* pTempMesh = 0;

    if( m_pMesh )
    {
        if( FAILED( m_pMesh->CloneMesh( m_pMesh->GetOptions(), pDecl,
                                        &pTempMesh ) ) )
        {
            SAFE_RELEASE( pTempMesh );
            return E_FAIL;
        }
    }


    // Check if the old declaration contains a normal.
    bool bHadNormal = false;
    bool bHadTangent = false;
    D3DVERTEXELEMENT9 aOldDecl[MAX_FVF_DECL_SIZE];
    if( m_pMesh && SUCCEEDED( m_pMesh->GetDeclaration( aOldDecl ) ) )
    {
        for( UINT index = 0; index < D3DXGetDeclLength( aOldDecl ); ++index )
        {
            if( aOldDecl[index].Usage == D3DDECLUSAGE_NORMAL )
            {
                bHadNormal = true;
            }
            if( aOldDecl[index].Usage == D3DDECLUSAGE_TANGENT )
            {
                bHadTangent = true;
            }
        }
    }

    // Check if the new declaration contains a normal.
    bool bHaveNormalNow = false;
    bool bHaveTangentNow = false;
    D3DVERTEXELEMENT9 aNewDecl[MAX_FVF_DECL_SIZE];
    if( pTempMesh && SUCCEEDED( pTempMesh->GetDeclaration( aNewDecl ) ) )
    {
        for( UINT index = 0; index < D3DXGetDeclLength( aNewDecl ); ++index )
        {
            if( aNewDecl[index].Usage == D3DDECLUSAGE_NORMAL )
            {
                bHaveNormalNow = true;
            }
            if( aNewDecl[index].Usage == D3DDECLUSAGE_TANGENT )
            {
                bHaveTangentNow = true;
            }
        }
    }

    SAFE_RELEASE( m_pMesh );

    if( pTempMesh )
    {
        m_pMesh = pTempMesh;

        if( !bHadNormal && bHaveNormalNow && bAutoComputeNormals )
        {
            // Compute normals in case the meshes have them
            D3DXComputeNormals( m_pMesh, 0 );
        }

        if( bHaveNormalNow && !bHadTangent && bHaveTangentNow && bAutoComputeTangents )
        {
            MeshRenderer2* pNewMesh;
            int hr;

            __DWORD__* rgdwAdjacency = 0;
            rgdwAdjacency = new __DWORD__[m_pMesh->GetNumFaces() * 3];
            if( rgdwAdjacency == 0 )
                return E_OUTOFMEMORY;
            V( m_pMesh->GenerateAdjacency( 1e-6f, rgdwAdjacency ) );

            float fPartialEdgeThreshold;
            float fSingularPointThreshold;
            float fNormalEdgeThreshold;
            if( bSplitVertexForOptimalTangents )
            {
                fPartialEdgeThreshold = 0.01f;
                fSingularPointThreshold = 0.25f;
                fNormalEdgeThreshold = 0.01f;
            }
            else
            {
                fPartialEdgeThreshold = -1.01f;
                fSingularPointThreshold = 0.01f;
                fNormalEdgeThreshold = -1.01f;
            }

            // Compute tangents, which are required for normal mapping
            hr = D3DXComputeTangentFrameEx( m_pMesh,
                                            D3DDECLUSAGE_TEXCOORD, 0,
                                            D3DDECLUSAGE_TANGENT, 0,
                                            D3DX_DEFAULT, 0,
                                            D3DDECLUSAGE_NORMAL, 0,
                                            0, rgdwAdjacency,
                                            fPartialEdgeThreshold, fSingularPointThreshold, fNormalEdgeThreshold,
                                            &pNewMesh, 0 );

            SAFE_DELETE_ARRAY( rgdwAdjacency );
            if( FAILED( hr ) )
                return hr;

            SAFE_RELEASE( m_pMesh );
            m_pMesh = pNewMesh;
        }
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
int CDXUTXFileMesh::Destroy()
{
    for( UINT i = 0; i < m_dwNumMaterials; i++ )
        SAFE_RELEASE( m_pTextures[i] );
    SAFE_DELETE_ARRAY( m_pTextures );
    SAFE_DELETE_ARRAY( m_pMaterials );
    SAFE_DELETE_ARRAY( m_strMaterials );

    SAFE_RELEASE( m_pMesh );

    m_dwNumMaterials = 0L;

    return S_OK;
}




//-----------------------------------------------------------------------------
int CDXUTXFileMesh::Render( bool bDrawOpaqueSubsets,
                                bool bDrawAlphaSubsets )
{
    if( 0 == m_pMesh )
        return E_FAIL;

    // Frist, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        for( __DWORD__ i = 0; i < m_dwNumMaterials; i++ )
        {
            if( m_bUseMaterials )
            {
                if( m_pMaterials[i].Diffuse.a < 1.0f )
                    continue;
                IRenderer::GetRendererInstance()->SetMaterial( &m_pMaterials[i] );
                IRenderer::GetRendererInstance()->SetTexture( 0, m_pTextures[i] );
            }
            m_pMesh->DrawSubset( i );
        }
    }

    // Then, draw the subsets with alpha
    if( bDrawAlphaSubsets && m_bUseMaterials )
    {
        for( __DWORD__ i = 0; i < m_dwNumMaterials; i++ )
        {
            if( m_pMaterials[i].Diffuse.a == 1.0f )
                continue;

            // Set the material and texture
            IRenderer::GetRendererInstance()->SetMaterial( &m_pMaterials[i] );
            IRenderer::GetRendererInstance()->SetTexture( 0, m_pTextures[i] );
            m_pMesh->DrawSubset( i );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
int CDXUTXFileMesh::Render( ID3DXEffect* pEffect,
                                D3DXFROMWINEHANDLE hTexture,
                                D3DXFROMWINEHANDLE hDiffuse,
                                D3DXFROMWINEHANDLE hAmbient,
                                D3DXFROMWINEHANDLE hSpecular,
                                D3DXFROMWINEHANDLE hEmissive,
                                D3DXFROMWINEHANDLE hPower,
                                bool bDrawOpaqueSubsets,
                                bool bDrawAlphaSubsets )
{
    if( 0 == m_pMesh )
        return E_FAIL;

    UINT cPasses;
    // Frist, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        pEffect->Begin( &cPasses, 0 );
        for( UINT p = 0; p < cPasses; ++p )
        {
            pEffect->BeginPass( p );
            for( __DWORD__ i = 0; i < m_dwNumMaterials; i++ )
            {
                if( m_bUseMaterials )
                {
                    if( m_pMaterials[i].Diffuse.a < 1.0f )
                        continue;
                    if( hTexture )
                        IRenderer::GetRendererInstance()->setTexture( hTexture, m_pTextures[i] );
                    // D3DCOLORVALUE and D3DXFROMWINEVECTOR4 are data-wise identical.
                    // No conversion is needed.
                    if( hDiffuse )
                        pEffect->SetVector( hDiffuse, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Diffuse );
                    if( hAmbient )
                        pEffect->SetVector( hAmbient, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Ambient );
                    if( hSpecular )
                        pEffect->SetVector( hSpecular, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Specular );
                    if( hEmissive )
                        pEffect->SetVector( hEmissive, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Emissive );
                    if( hPower )
                        pEffect->SetFloat( hPower, m_pMaterials[i].Power );
                    pEffect->CommitChanges();
                }
                m_pMesh->DrawSubset( i );
            }
            pEffect->EndPass();
        }
        pEffect->End();
    }

    // Then, draw the subsets with alpha
    if( bDrawAlphaSubsets && m_bUseMaterials )
    {
        pEffect->Begin( &cPasses, 0 );
        for( UINT p = 0; p < cPasses; ++p )
        {
            pEffect->BeginPass( p );
            for( __DWORD__ i = 0; i < m_dwNumMaterials; i++ )
            {
                if( m_bUseMaterials )
                {
                    if( m_pMaterials[i].Diffuse.a == 1.0f )
                        continue;
                    if( hTexture )
                        IRenderer::GetRendererInstance()->setTexture( hTexture, m_pTextures[i] );
                    // D3DCOLORVALUE and D3DXFROMWINEVECTOR4 are data-wise identical.
                    // No conversion is needed.
                    if( hDiffuse )
                        pEffect->SetVector( hDiffuse, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Diffuse );
                    if( hAmbient )
                        pEffect->SetVector( hAmbient, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Ambient );
                    if( hSpecular )
                        pEffect->SetVector( hSpecular, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Specular );
                    if( hEmissive )
                        pEffect->SetVector( hEmissive, ( D3DXFROMWINEVECTOR4* )&m_pMaterials[i].Emissive );
                    if( hPower )
                        pEffect->SetFloat( hPower, m_pMaterials[i].Power );
                    pEffect->CommitChanges();
                }
                m_pMesh->DrawSubset( i );
            }
            pEffect->EndPass();
        }
        pEffect->End();
    }

    return S_OK;
}
#endif

int CDXUTSDKMesh::Create( BYTE* pData, UINT DataBytes)
{
    return CreateFromMemory( pData, DataBytes);
}

int CDXUTSDKMesh::CreateFromFile(const char* szFileName)
{
    int hr = S_OK;
	stx_strlcpy(m_strPath, szFileName, MAX_PATH);
    // Get the file size
    UINT cBytes = 0;
	FILE* file = fopen( szFileName, "r" );
		LOG_PRINT("szFileName=%s\n", szFileName);
	if ( file )
	{
		// Get the file size, so we can pre-allocate the string. HUGE speed impact.
		fseek( file, 0, SEEK_END );
		cBytes = ftell( file );
		fseek( file, 0, SEEK_SET );
		// Strange case, but good to handle up front.
		if ( cBytes == 0 )
		{
			fclose( file );
			return -1;
		}
		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.

    // Allocate memory
    m_pStaticMeshData = new BYTE[ cBytes ];
    if( !m_pStaticMeshData )
    {
        stx_exit(0);
    }
		const int BUF_SIZE = 2048;
		char buf[BUF_SIZE];
		unsigned int j=0;
		while( fgets( buf, BUF_SIZE, file ) )
		{
			for(unsigned int i=0;i<BUF_SIZE;i++)
				m_pStaticMeshData[j*BUF_SIZE+i]=buf[i];
			j++;
		}
		fclose( file );
	}

    //if( SUCCEEDED( hr ) )
    {
        hr = CreateFromMemory( m_pStaticMeshData,
                               cBytes);
        //if( FAILED( hr ) ) delete []m_pStaticMeshData;
    }

    return hr;
}

int CDXUTSDKMesh::CreateFromMemory( BYTE* pData,
                                        UINT DataBytes )
{
    int hr = E_FAIL;
    D3DXFROMWINEVECTOR3 lower; 
    D3DXFROMWINEVECTOR3 upper;

    // Set outstanding resources to zero
    m_NumOutstandingResources = 0;

    //if( bCopyStatic )
	if(0)
    {
        SDKMESH_HEADER* pHeader = ( SDKMESH_HEADER* )pData;

        SIZE_T StaticSize = ( SIZE_T )( pHeader->HeaderSize + pHeader->NonBufferDataSize );
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
    m_pMeshHeader = ( SDKMESH_HEADER* )m_pStaticMeshData;
    m_pVertexBufferArray = ( SDKMESH_VERTEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                              m_pMeshHeader->VertexStreamHeadersOffset );
    m_pIndexBufferArray = ( SDKMESH_INDEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                            m_pMeshHeader->IndexStreamHeadersOffset );
    m_pMeshArray = ( SDKMESH_MESH* )( m_pStaticMeshData + m_pMeshHeader->MeshDataOffset );
    m_pSubsetArray = ( SDKMESH_SUBSET* )( m_pStaticMeshData + m_pMeshHeader->SubsetDataOffset );
    m_pFrameArray = ( SDKMESH_FRAME* )( m_pStaticMeshData + m_pMeshHeader->FrameDataOffset );
    m_pMaterialArray = ( SDKMESH_MATERIAL* )( m_pStaticMeshData + m_pMeshHeader->MaterialDataOffset );

#if 0
	LOG_PRINT("m_pMeshArray->Name=%s\n", m_pMeshArray->Name);
#if 0
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
        //SDKMESH_SUBSET* pSubset = m_pMeshArray[i].pSubsets[0];
        for( UINT subset = 0; subset < m_pMeshArray[i].NumSubsets; subset++ ) 
        {
            pSubset = &m_pSubsetArray[ m_pMeshArray[i].pSubsets[subset] ];
	LOG_PRINT("\tpSubset->Name=%s\n", pSubset->Name);
	}
	}
#endif
	LOG_PRINT("m_pFrameArray->Name=%s\n", m_pFrameArray->Name);
	LOG_PRINT("m_pMaterialArray->Name=%s\n", m_pMaterialArray->Name);
	//LOG_PRINT("m_pArray->FrameName=%s\n", m_pArray->FrameName);
	
//struct SDKANIMATION_FRAME_DATA char FrameName[MAX_FRAME_NAME];
#endif


    LOG_PRINT("m_pStaticMeshData %x\n", m_pStaticMeshData);
    LOG_PRINT("m_pMeshHeader->FrameDataOffset %x\n", m_pMeshHeader->FrameDataOffset);
    LOG_PRINT("m_pMeshHeader->MaterialDataOffset %x\n", m_pMeshHeader->MaterialDataOffset);
    LOG_PRINT("m_pMaterialArray[0].DiffuseTexture=%s\n", m_pMaterialArray[0].DiffuseTexture);
    LOG_PRINT("m_pMaterialArray[0].NormalTexture=%s\n", m_pMaterialArray[0].NormalTexture);
    LOG_PRINT("m_pMaterialArray[0].SpecularTexture=%s\n", m_pMaterialArray[0].SpecularTexture);


    // Load Materials
        LOG_PRINT("NumMaterials: %d\n", m_pMeshHeader->NumMaterials );
        LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials );


    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
        m_pMeshArray[i].pSubsets = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].SubsetOffset );
        m_pMeshArray[i].pFrameInfluences = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].FrameInfluenceOffset );
    }

    // Setup buffer data pointer
    BYTE* pBufferData = 0;

    // Get the start of the buffer data
    UINT64 BufferDataStart = 0;

    // error condition
    if( m_pMeshHeader->Version != SDKMESH_FILE_VERSION )
    {
        LOG_PRINT("E_NOINTERFACE\n");
        goto Error;
    }

    // Setup buffer data pointer
    pBufferData = pData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Get the start of the buffer data
    BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;
    // Create VBs
    m_ppVertices = new BYTE*[m_pMeshHeader->NumVertexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        BYTE* pVertices = NULL;
        pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[i].DataOffset - BufferDataStart ) );

            CreateVertexBuffer( &m_pVertexBufferArray[i], pVertices );

        m_ppVertices[i] = pVertices;
    }

    // Create IBs
    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );

            CreateIndexBuffer( &m_pIndexBufferArray[i], pIndices );

        m_ppIndices[i] = pIndices;
    }
#if 0
    // Load Materials
        LOG_PRINT("NumMaterials: %d\n", m_pMeshHeader->NumMaterials );
        LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials );
#endif
    // Create a place to store our bind pose frame matrices
    m_pBindPoseFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pBindPoseFrameMatrices )
        goto Error;

    // Create a place to store our transformed frame matrices
    m_pTransformedFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pTransformedFrameMatrices )
        goto Error;

    hr = S_OK;
#if 0 // ???
    SDKMESH_SUBSET* pSubset = NULL;
    D3D10_PRIMITIVE_TOPOLOGY PrimType;

    // update bounding volume 
    SDKMESH_MESH* currentMesh = &m_pMeshArray[0];
    int tris = 0;
    for (UINT meshi=0; meshi < m_pMeshHeader->NumMeshes; ++meshi) {
        lower.x = FLT_MAX; lower.y = FLT_MAX; lower.z = FLT_MAX;
        upper.x = -FLT_MAX; upper.y = -FLT_MAX; upper.z = -FLT_MAX;
        currentMesh = GetMesh( meshi );
        INT indsize;
        if (m_pIndexBufferArray[currentMesh->IndexBuffer].IndexType == IT_16BIT ) {
            indsize = 2;
        }else {
            indsize = 4;        
        }

        for( UINT subset = 0; subset < currentMesh->NumSubsets; subset++ )
        {
            pSubset = GetSubset( meshi, subset ); //&m_pSubsetArray[ currentMesh->pSubsets[subset] ];

            PrimType = GetPrimitiveType10( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
            assert( PrimType == D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );// only triangle lists are handled.

            UINT IndexCount = ( UINT )pSubset->IndexCount;
            UINT IndexStart = ( UINT )pSubset->IndexStart;

            /*if( bAdjacent )
            {
                IndexCount *= 2;
                IndexStart *= 2;
            }*/
     
        //BYTE* pIndices = NULL;
            //m_ppIndices[i]
            UINT *ind = ( UINT * )m_ppIndices[currentMesh->IndexBuffer];
            FLOAT *verts =  ( FLOAT* )m_ppVertices[currentMesh->VertexBuffers[0]];
            UINT stride = (UINT)m_pVertexBufferArray[currentMesh->VertexBuffers[0]].StrideBytes;
            assert (stride % 4 == 0);
            stride /=4;
            for (UINT vertind = IndexStart; vertind < IndexStart + IndexCount; ++vertind) {
                UINT current_ind=0;
                if (indsize == 2) {
                    UINT ind_div2 = vertind / 2;
                    current_ind = ind[ind_div2];
                    if (vertind %2 ==0) {
                        current_ind = current_ind << 16;
                        current_ind = current_ind >> 16;
                    }else {
                        current_ind = current_ind >> 16;
                    }
                }else {
                    current_ind = ind[vertind];
                }
                tris++;
                D3DXFROMWINEVECTOR3 *pt = (D3DXFROMWINEVECTOR3*)&(verts[stride * current_ind]);
                if (pt->x < lower.x) {
                    lower.x = pt->x;
                }
                if (pt->y < lower.y) {
                    lower.y = pt->y;
                }
                if (pt->z < lower.z) {
                    lower.z = pt->z;
                }
                if (pt->x > upper.x) {
                    upper.x = pt->x;
                }
                if (pt->y > upper.y) {
                    upper.y = pt->y;
                }
                if (pt->z > upper.z) {
                    upper.z = pt->z;
                }
                //BYTE** m_ppVertices;
                //BYTE** m_ppIndices;
            }
            //pd3dDeviceContext->DrawIndexed( IndexCount, IndexStart, VertexStart );
        }

        D3DXFROMWINEVECTOR3 half = upper - lower;
        half *=0.5f;

        currentMesh->BoundingBoxCenter = lower + half;
        currentMesh->BoundingBoxExtents = half;

    }
    // Update 
#endif   

Error:

    //if( !pLoaderCallbacks10 && !pLoaderCallbacks9 )
    {
        CheckLoadDone();
    }

    return hr;
}

