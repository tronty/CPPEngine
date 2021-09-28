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
#include "SDKmisc.h"

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::LoadMaterials( SDKMESH_MATERIAL* pMaterials, UINT numMaterials,
                                   )
{
    char strPath[MAX_PATH];
    
    {
        for( UINT m = 0; m < numMaterials; m++ )
        {
            pMaterials[m].pDiffuseTexture9 = NULL;
            pMaterials[m].pNormalTexture9 = NULL;
            pMaterials[m].pSpecularTexture9 = NULL;
            pMaterials[m].pDiffuseRV10 = NULL;
            pMaterials[m].pNormalRV10 = NULL;
            pMaterials[m].pSpecularRV10 = NULL;

            // load textures
            if( pMaterials[m].DiffuseTexture[0] != 0 )
            {
                stx_snprintf( strPath, MAX_PATH, "%s%s", m_strPath, pMaterials[m].DiffuseTexture );
                if( FAILED( DXUTGetGlobalResourceCache().CreateTextureFromFile(
                                                                                strPath,
                                                                                &pMaterials[m].pDiffuseTexture9 ) ) )
                    pMaterials[m].pDiffuseTexture9 = ( Texture9* )ERROR_RESOURCE_VALUE;
            }
            if( pMaterials[m].NormalTexture[0] != 0 )
            {
                stx_snprintf( strPath, MAX_PATH, "%s%s", m_strPath, pMaterials[m].NormalTexture );
                if( FAILED( DXUTGetGlobalResourceCache().CreateTextureFromFile(
                                                                                strPath,
                                                                                &pMaterials[m].pNormalTexture9 ) ) )
                    pMaterials[m].pNormalTexture9 = ( Texture9* )ERROR_RESOURCE_VALUE;
            }
            if( pMaterials[m].SpecularTexture[0] != 0 )
            {
                stx_snprintf( strPath, MAX_PATH, "%s%s", m_strPath, pMaterials[m].SpecularTexture );
                if( FAILED( DXUTGetGlobalResourceCache().CreateTextureFromFile(
                                                                                strPath,
                                                                                &pMaterials[m].pSpecularTexture9 ) ) )
                    pMaterials[m].pSpecularTexture9 = ( Texture9* )ERROR_RESOURCE_VALUE;
            }

        }
    }
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTSDKMesh::CreateVertexBuffer( SDKMESH_VERTEX_BUFFER_HEADER* pHeader,
                                          void* pVertices )
{
    HRESULT hr = S_OK;

    pHeader->DataOffset = 0;
    {
        hr = IRenderer::GetRendererInstance()->CreateVertexBuffer( ( UINT )pHeader->SizeBytes,
                                             D3DUSAGE_WRITEONLY,
                                             0,
                                             D3DPOOL_DEFAULT,
                                             &pHeader->pVB9,
                                             NULL );

        //lock
        if( SUCCEEDED( hr ) )
        {
            void* pLockedVerts = NULL;
            V_RETURN( pHeader->pVB9->Lock( 0, 0, &pLockedVerts, 0 ) );
            CopyMemory( pLockedVerts, pVertices, ( size_t )pHeader->SizeBytes );
            pHeader->pVB9->Unlock();
        }
    }

    return hr;
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTSDKMesh::CreateIndexBuffer( SDKMESH_INDEX_BUFFER_HEADER* pHeader,
                                         void* pIndices )
{
    HRESULT hr = S_OK;

    pHeader->DataOffset = 0;

    D3DFORMAT ibFormat = D3DFMT_INDEX16;
    switch( pHeader->IndexType )
    {
        case IT_16BIT:
            ibFormat = D3DFMT_INDEX16;
            break;
        case IT_32BIT:
            ibFormat = D3DFMT_INDEX32;
            break;
    };

    {
        hr = IRenderer::GetRendererInstance()->CreateIndexBuffer( ( UINT )( pHeader->SizeBytes ),
                                            D3DUSAGE_WRITEONLY,
                                            ibFormat,
                                            D3DPOOL_DEFAULT,
                                            &pHeader->pIB9,
                                            NULL );

        if( SUCCEEDED( hr ) )
        {
            void* pLockedIndices = NULL;
            V_RETURN( pHeader->pIB9->Lock( 0, 0, &pLockedIndices, 0 ) );
            CopyMemory( pLockedIndices, pIndices, ( size_t )( pHeader->SizeBytes ) );
            pHeader->pIB9->Unlock();
        }
    }

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
    D3DXMatrixMultiply( &LocalWorld, &m_pFrameArray[iFrame].Matrix, pParentWorld );
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
        D3DXMatrixTranslation( &mTranslate, parentPos.x, parentPos.y, parentPos.z );

        D3DXQUATERNION quat;
        D3DXFROMWINEMATRIX mQuat;
        quat.w = pData->Orientation.w;
        quat.x = pData->Orientation.x;
        quat.y = pData->Orientation.y;
        quat.z = pData->Orientation.z;
        if( quat.w == 0 && quat.x == 0 && quat.y == 0 && quat.z == 0 )
            D3DXQuaternionIdentity( &quat );
        D3DXQuaternionNormalize( &quat, &quat );
        D3DXMatrixRotationQuaternion( &mQuat, &quat );
        LocalTransform = ( mQuat * mTranslate );
    }
    else
    {
        LocalTransform = m_pFrameArray[iFrame].Matrix;
    }

    // Transform ourselves
    D3DXFROMWINEMATRIX LocalWorld;
    D3DXMatrixMultiply( &LocalWorld, &LocalTransform, pParentWorld );
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
    D3DXQUATERNION quat1;
    D3DXQUATERNION quat2;
    D3DXFROMWINEMATRIX mTo;
    D3DXFROMWINEMATRIX mInvTo;
    D3DXFROMWINEMATRIX mFrom;

    UINT iTick = GetAnimationKeyFromTime( fTime );

    if( INVALID_ANIMATION_DATA != m_pFrameArray[iFrame].AnimationDataIndex )
    {
        SDKANIMATION_FRAME_DATA* pFrameData = &m_pAnimationFrameData[ m_pFrameArray[iFrame].AnimationDataIndex ];
        SDKANIMATION_DATA* pData = &pFrameData->pAnimationData[ iTick ];
        SDKANIMATION_DATA* pDataOrig = &pFrameData->pAnimationData[ 0 ];

        D3DXMatrixTranslation( &mTrans1, -pDataOrig->Translation.x,
                               -pDataOrig->Translation.y,
                               -pDataOrig->Translation.z );
        D3DXMatrixTranslation( &mTrans2, pData->Translation.x,
                               pData->Translation.y,
                               pData->Translation.z );

        quat1.x = pDataOrig->Orientation.x;
        quat1.y = pDataOrig->Orientation.y;
        quat1.z = pDataOrig->Orientation.z;
        quat1.w = pDataOrig->Orientation.w;
        D3DXQuaternionInverse( &quat1, &quat1 );
        D3DXMatrixRotationQuaternion( &mRot1, &quat1 );
        mInvTo = mTrans1 * mRot1;

        quat2.x = pData->Orientation.x;
        quat2.y = pData->Orientation.y;
        quat2.z = pData->Orientation.z;
        quat2.w = pData->Orientation.w;
        D3DXMatrixRotationQuaternion( &mRot2, &quat2 );
        mFrom = mRot2 * mTrans2;

        D3DXFROMWINEMATRIX mOutput = mInvTo * mFrom;
        m_pTransformedFrameMatrices[iFrame] = mOutput;
    }
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::RenderMesh( UINT iMesh,
                               
                               LPD3DXEFFECT pEffect,
                               D3DXHANDLE hTechnique,
                               D3DXHANDLE htxDiffuse,
                               D3DXHANDLE htxNormal,
                               D3DXHANDLE htxSpecular )
{
    if( 0 < GetOutstandingBufferResources() )
        return;

    SDKMESH_MESH* pMesh = &m_pMeshArray[iMesh];

    // set vb streams
    for( UINT i = 0; i < ( UINT )pMesh->NumVertexBuffers; i++ )
    {
        IRenderer::GetRendererInstance()->SetStreamSource( i,
                                     m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].pVB9,
                                     0,
                                     ( UINT )m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].StrideBytes );
    }

    // Set our index buffer as well
    IRenderer::GetRendererInstance()->SetIndices( m_pIndexBufferArray[ pMesh->IndexBuffer ].pIB9 );

    // Render the scene with this technique 
    pEffect->SetTechnique( hTechnique );

    SDKMESH_SUBSET* pSubset = NULL;
    SDKMESH_MATERIAL* pMat = NULL;
    D3DPRIMITIVETYPE PrimType;
    UINT cPasses = 0;
    pEffect->Begin( &cPasses, 0 );

    for( UINT p = 0; p < cPasses; ++p )
    {
        pEffect->BeginPass( p );

        for( UINT subset = 0; subset < pMesh->NumSubsets; subset++ )
        {
            pSubset = &m_pSubsetArray[ pMesh->pSubsets[subset] ];

            PrimType = GetPrimitiveType9( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );

            if( INVALID_MATERIAL != pSubset->MaterialID && m_pMeshHeader->NumMaterials > 0 )
            {
                pMat = &m_pMaterialArray[ pSubset->MaterialID ];
                if( htxDiffuse && !IsErrorResource( pMat->pDiffuseTexture9 ) )
                    pEffect->SetTexture( htxDiffuse, pMat->pDiffuseTexture9 );
                if( htxNormal && !IsErrorResource( pMat->pNormalTexture9 ) )
                    pEffect->SetTexture( htxNormal, pMat->pNormalTexture9 );
                if( htxSpecular && !IsErrorResource( pMat->pSpecularTexture9 ) )
                    pEffect->SetTexture( htxSpecular, pMat->pSpecularTexture9 );
            }

            pEffect->CommitChanges();

            UINT PrimCount = ( UINT )pSubset->IndexCount;
            UINT IndexStart = ( UINT )pSubset->IndexStart;
            UINT VertexStart = ( UINT )pSubset->VertexStart;
            UINT VertexCount = ( UINT )pSubset->VertexCount;
            if( D3DPT_TRIANGLELIST == PrimType )
                PrimCount /= 3;
            if( D3DPT_LINELIST == PrimType )
                PrimCount /= 2;
            if( D3DPT_TRIANGLESTRIP == PrimType )
                PrimCount = ( PrimCount - 3 ) + 1;
            if( D3DPT_LINESTRIP == PrimType )
                PrimCount -= 1;

            IRenderer::GetRendererInstance()->DrawIndexedPrimitive( PrimType, VertexStart, 0, VertexCount, IndexStart, PrimCount );
        }

        pEffect->EndPass();
    }

    pEffect->End();
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::RenderFrame( UINT iFrame,
                                
                                LPD3DXEFFECT pEffect,
                                D3DXHANDLE hTechnique,
                                D3DXHANDLE htxDiffuse,
                                D3DXHANDLE htxNormal,
                                D3DXHANDLE htxSpecular )
{
    if( !m_pStaticMeshData || !m_pFrameArray )
        return;

    if( m_pFrameArray[iFrame].Mesh != INVALID_MESH )
    {
        RenderMesh( m_pFrameArray[iFrame].Mesh,
                    
                    pEffect,
                    hTechnique,
                    htxDiffuse,
                    htxNormal,
                    htxSpecular );
    }

    // Render our children
    if( m_pFrameArray[iFrame].ChildFrame != INVALID_FRAME )
        RenderFrame( m_pFrameArray[iFrame].ChildFrame, pEffect, hTechnique, htxDiffuse, htxNormal,
                     htxSpecular );

    // Render our siblings
    if( m_pFrameArray[iFrame].SiblingFrame != INVALID_FRAME )
        RenderFrame( m_pFrameArray[iFrame].SiblingFrame, pEffect, hTechnique, htxDiffuse, htxNormal,
                     htxSpecular );
}


//--------------------------------------------------------------------------------------
CDXUTSDKMesh::CDXUTSDKMesh() : m_NumOutstandingResources( 0 ),
                               m_bLoading( false ),
                               m_hFile( 0 ),
                               m_hFileMappingObject( 0 ),
                               m_pMeshHeader( NULL ),
                               m_pStaticMeshData( NULL ),
                               m_pHeapData( NULL ),
                               m_pAdjacencyIndexBufferArray( NULL ),
                               m_pAnimationData( NULL ),
                               m_ppVertices( NULL ),
                               m_ppIndices( NULL ),
                               m_pBindPoseFrameMatrices( NULL ),
                               m_pTransformedFrameMatrices( NULL ),
                               m_pDev9( NULL ),
                               m_pDev10( NULL )
{
}

//--------------------------------------------------------------------------------------
HRESULT CDXUTSDKMesh::LoadAnimation( char** szFileName )
{
    HRESULT hr = E_FAIL;
    DWORD dwBytesRead = 0;
    LARGE_INTEGER liMove;
    char* strPath[MAX_PATH];

    // Find the path for the file
    V_RETURN( DXUTFindDXSDKMediaFileCch( strPath, MAX_PATH, szFileName ) );

    // Open the file
    HANDLE hFile = CreateFile( strPath, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                               FILE_FLAG_SEQUENTIAL_SCAN, NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return DXUTERR_MEDIANOTFOUND;

    /////////////////////////
    // Header
    SDKANIMATION_FILE_HEADER fileheader;
    if( !ReadFile( hFile, &fileheader, sizeof( SDKANIMATION_FILE_HEADER ), &dwBytesRead, NULL ) )
        goto Error;

    //allocate
    m_pAnimationData = new BYTE[ ( size_t )( sizeof( SDKANIMATION_FILE_HEADER ) + fileheader.AnimationDataSize ) ];
    if( !m_pAnimationData )
    {
        hr = E_OUTOFMEMORY;
        goto Error;
    }

    // read it all in
    liMove.QuadPart = 0;
    if( !SetFilePointerEx( hFile, liMove, NULL, FILE_BEGIN ) )
        goto Error;
    if( !ReadFile( hFile, m_pAnimationData, ( DWORD )( sizeof( SDKANIMATION_FILE_HEADER ) +
                                                       fileheader.AnimationDataSize ), &dwBytesRead, NULL ) )
        goto Error;

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
    CloseHandle( hFile );
    return hr;
}

//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::Destroy()
{
    if( !CheckLoadDone() )
        return;

    if( m_pStaticMeshData )
    {
        if( m_pMaterialArray )
        {
            for( UINT64 m = 0; m < m_pMeshHeader->NumMaterials; m++ )
            {
                if( m_pDev9 )
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

    if( m_pAdjacencyIndexBufferArray )
    {
        for( UINT64 i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
        {
            SAFE_RELEASE( m_pAdjacencyIndexBufferArray[i].pIB10 );
        }
    }
    SAFE_DELETE_ARRAY( m_pAdjacencyIndexBufferArray );

    SAFE_DELETE_ARRAY( m_pHeapData );
    m_pStaticMeshData = NULL;
    SAFE_DELETE_ARRAY( m_pAnimationData );
    SAFE_DELETE_ARRAY( m_pBindPoseFrameMatrices );
    SAFE_DELETE_ARRAY( m_pTransformedFrameMatrices );

    SAFE_DELETE_ARRAY( m_ppVertices );
    SAFE_DELETE_ARRAY( m_ppIndices );

    m_pMeshHeader = NULL;
    m_pVertexBufferArray = NULL;
    m_pIndexBufferArray = NULL;
    m_pMeshArray = NULL;
    m_pSubsetArray = NULL;
    m_pFrameArray = NULL;
    m_pMaterialArray = NULL;

    m_pAnimationHeader = NULL;
    m_pAnimationFrameData = NULL;

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
            D3DXMatrixInverse( &mInvBindPose, NULL, &m_pBindPoseFrameMatrices[i] );
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
                           LPD3DXEFFECT pEffect,
                           D3DXHANDLE hTechnique,
                           D3DXHANDLE htxDiffuse,
                           D3DXHANDLE htxNormal,
                           D3DXHANDLE htxSpecular )
{
    RenderFrame( 0, pEffect, hTechnique, htxDiffuse, htxNormal, htxSpecular );
}

//--------------------------------------------------------------------------------------
DXGI_FORMAT CDXUTSDKMesh::GetIBFormat10( UINT iMesh )
{
    switch( m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].IndexType )
    {
        case IT_16BIT:
            return DXGI_FORMAT_R16_UINT;
        case IT_32BIT:
            return DXGI_FORMAT_R32_UINT;
    };
    return DXGI_FORMAT_R16_UINT;
}

//--------------------------------------------------------------------------------------
D3DPRIMITIVETYPE CDXUTSDKMesh::GetPrimitiveType9( SDKMESH_PRIMITIVE_TYPE PrimType )
{
    D3DPRIMITIVETYPE retType = D3DPT_TRIANGLELIST;

    switch( PrimType )
    {
        case PT_TRIANGLE_LIST:
            retType = D3DPT_TRIANGLELIST;
            break;
        case PT_TRIANGLE_STRIP:
            retType = D3DPT_TRIANGLESTRIP;
            break;
        case PT_LINE_LIST:
            retType = D3DPT_LINELIST;
            break;
        case PT_LINE_STRIP:
            retType = D3DPT_LINESTRIP;
            break;
        case PT_POINT_LIST:
            retType = D3DPT_POINTLIST;
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
VertexBuffer9* CDXUTSDKMesh::GetVB9( UINT iMesh, UINT iVB )
{
    return m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].pVB9;
}

//--------------------------------------------------------------------------------------
IndexBuffer9* CDXUTSDKMesh::GetIB9( UINT iMesh )
{
    return m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].pIB9;
}

//--------------------------------------------------------------------------------------
char* CDXUTSDKMesh::GetMeshPathA()
{
    return m_strPath;
}

//--------------------------------------------------------------------------------------
char** CDXUTSDKMesh::GetMeshPathW()
{
    return m_strPathW;
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
VertexBuffer9* CDXUTSDKMesh::GetVB9At( UINT iVB )
{
    return m_pVertexBufferArray[ iVB ].pVB9;
}

//--------------------------------------------------------------------------------------
IndexBuffer9* CDXUTSDKMesh::GetIB9At( UINT iIB )
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
        if( _stricmp( m_pFrameArray[i].Name, pszName ) == 0 )
        {
            return &m_pFrameArray[i];
        }
    }
    return NULL;
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

    if( m_pDev10 )
    {
        for( UINT i = 0; i < m_pMeshHeader->NumMaterials; i++ )
        {
            if( m_pMaterialArray[i].DiffuseTexture[0] != 0 )
            {
                if( !m_pMaterialArray[i].pDiffuseRV10 && !IsErrorResource( m_pMaterialArray[i].pDiffuseRV10 ) )
                    outstandingResources ++;
            }

            if( m_pMaterialArray[i].NormalTexture[0] != 0 )
            {
                if( !m_pMaterialArray[i].pNormalRV10 && !IsErrorResource( m_pMaterialArray[i].pNormalRV10 ) )
                    outstandingResources ++;
            }

            if( m_pMaterialArray[i].SpecularTexture[0] != 0 )
            {
                if( !m_pMaterialArray[i].pSpecularRV10 && !IsErrorResource( m_pMaterialArray[i].pSpecularRV10 ) )
                    outstandingResources ++;
            }
        }
    }
    else
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


//-------------------------------------------------------------------------------------
// CDXUTXFileMesh implementation.
//-------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CDXUTXFileMesh::CDXUTXFileMesh( const char* strName )
{
    wcscpy_s( m_strName, 512, strName );
    m_pMesh = NULL;
    m_pMaterials = NULL;
    m_pTextures = NULL;
    m_bUseMaterials = TRUE;
    m_pVB = NULL;
    m_pIB = NULL;
    m_pDecl = NULL;
    m_strMaterials = NULL;
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
HRESULT CDXUTXFileMesh::Create( const char* strFilename )
{
    char* strPath[MAX_PATH];
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    LPD3DXBUFFER pMtrlBuffer = NULL;
    HRESULT hr;

    // Cleanup previous mesh if any
    Destroy();

    // Find the path for the file, and convert it to ANSI (for the D3DX API)
    DXUTFindDXSDKMediaFileCch( strPath, sizeof( strPath ) / sizeof( char* ), strFilename );

    // Load the mesh
    if( FAILED( hr = D3DXLoadMeshFromX( strPath, D3DXMESH_MANAGED, 
                                        &pAdjacencyBuffer, &pMtrlBuffer, NULL,
                                        &m_dwNumMaterials, &m_pMesh ) ) )
    {
        return hr;
    }

    // Optimize the mesh for performance
    if( FAILED( hr = m_pMesh->OptimizeInplace(
                D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                ( DWORD* )pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL ) ) )
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

    D3DXMATERIAL* d3dxMtrls = ( D3DXMATERIAL* )pMtrlBuffer->GetBufferPointer();
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
HRESULT CDXUTXFileMesh::Create( 
                                LPD3DXFILEDATA pFileData )
{
    LPD3DXBUFFER pMtrlBuffer = NULL;
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    HRESULT hr;

    // Cleanup previous mesh if any
    Destroy();

    // Load the mesh from the DXFILEDATA object
    if( FAILED( hr = D3DXLoadMeshFromXof( pFileData, D3DXMESH_MANAGED, 
                                          &pAdjacencyBuffer, &pMtrlBuffer, NULL,
                                          &m_dwNumMaterials, &m_pMesh ) ) )
    {
        return hr;
    }

    // Optimize the mesh for performance
    if( FAILED( hr = m_pMesh->OptimizeInplace(
                D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
                ( DWORD* )pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL ) ) )
    {
        SAFE_RELEASE( pAdjacencyBuffer );
        SAFE_RELEASE( pMtrlBuffer );
        return hr;
    }

    D3DXMATERIAL* d3dxMtrls = ( D3DXMATERIAL* )pMtrlBuffer->GetBufferPointer();
    hr = CreateMaterials( L"", d3dxMtrls, m_dwNumMaterials );

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
HRESULT CDXUTXFileMesh::Create( ID3DXMesh* pInMesh,
                                D3DXMATERIAL* pd3dxMaterials, DWORD dwMaterials )
{
    // Cleanup previous mesh if any
    Destroy();

    // Optimize the mesh for performance
    DWORD* rgdwAdjacency = NULL;
    rgdwAdjacency = new DWORD[pInMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    pInMesh->GenerateAdjacency( 1e-6f, rgdwAdjacency );

    D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
    pInMesh->GetDeclaration( decl );

    DWORD dwOptions = pInMesh->GetOptions();
    dwOptions &= ~( D3DXMESH_32BIT | D3DXMESH_SYSTEMMEM | D3DXMESH_WRITEONLY );
    dwOptions |= D3DXMESH_MANAGED;
    dwOptions |= D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE;

    ID3DXMesh* pTempMesh = NULL;
    if( FAILED( pInMesh->Optimize( dwOptions, rgdwAdjacency, NULL, NULL, NULL, &pTempMesh ) ) )
    {
        SAFE_DELETE_ARRAY( rgdwAdjacency );
        return E_FAIL;
    }

    SAFE_DELETE_ARRAY( rgdwAdjacency );
    SAFE_RELEASE( m_pMesh );
    m_pMesh = pTempMesh;

    HRESULT hr;
    hr = CreateMaterials( L"", pd3dxMaterials, dwMaterials );

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
HRESULT CDXUTXFileMesh::CreateMaterials( const char* strPath, D3DXMATERIAL* d3dxMtrls,
                                         DWORD dwNumMaterials )
{
    // Get material info for the mesh
    // Get the array of materials out of the buffer
    m_dwNumMaterials = dwNumMaterials;
    if( d3dxMtrls && m_dwNumMaterials > 0 )
    {
        // Allocate memory for the materials and textures
        m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
        if( m_pMaterials == NULL )
            return E_OUTOFMEMORY;
        m_pTextures = new LPDIRECT3DBASETEXTURE9[m_dwNumMaterials];
        if( m_pTextures == NULL )
            return E_OUTOFMEMORY;
        m_strMaterials = new CHAR[m_dwNumMaterials][MAX_PATH];
        if( m_strMaterials == NULL )
            return E_OUTOFMEMORY;

        // Copy each material and create its texture
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
        {
            // Copy the material
            m_pMaterials[i] = d3dxMtrls[i].MatD3D;
            m_pTextures[i] = NULL;

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
                        Texture9* pTex;
                        if( SUCCEEDED( D3DXCreateTextureFromFile( strTexture, &pTex ) ) )
                        {
                            // Obtain the base texture interface
                            pTex->QueryInterface( IID_BaseTexture9, ( LPVOID* )&m_pTextures[i] );
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
                            pTex->QueryInterface( IID_BaseTexture9, ( LPVOID* )&m_pTextures[i] );
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
                            pTex->QueryInterface( IID_BaseTexture9, ( LPVOID* )&m_pTextures[i] );
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
HRESULT CDXUTXFileMesh::SetFVF( DWORD dwFVF )
{
    LPD3DXMESH pTempMesh = NULL;

    if( m_pMesh )
    {
        if( FAILED( m_pMesh->CloneMeshFVF( m_pMesh->GetOptions(), dwFVF,
                                           &pTempMesh ) ) )
        {
            SAFE_RELEASE( pTempMesh );
            return E_FAIL;
        }

        DWORD dwOldFVF = 0;
        dwOldFVF = m_pMesh->GetFVF();
        SAFE_RELEASE( m_pMesh );
        m_pMesh = pTempMesh;

        // Compute normals if they are being requested and
        // the old mesh does not have them.
        if( !( dwOldFVF & D3DFVF_NORMAL ) && dwFVF & D3DFVF_NORMAL )
        {
            D3DXComputeNormals( m_pMesh, NULL );
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Convert the mesh to the format specified by the given vertex declarations.
//-----------------------------------------------------------------------------
HRESULT CDXUTXFileMesh::SetVertexDecl( const D3DVERTEXELEMENT9* pDecl,
                                       bool bAutoComputeNormals, bool bAutoComputeTangents,
                                       bool bSplitVertexForOptimalTangents )
{
    LPD3DXMESH pTempMesh = NULL;

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
            D3DXComputeNormals( m_pMesh, NULL );
        }

        if( bHaveNormalNow && !bHadTangent && bHaveTangentNow && bAutoComputeTangents )
        {
            ID3DXMesh* pNewMesh;
            HRESULT hr;

            DWORD* rgdwAdjacency = NULL;
            rgdwAdjacency = new DWORD[m_pMesh->GetNumFaces() * 3];
            if( rgdwAdjacency == NULL )
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
                                            &pNewMesh, NULL );

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
HRESULT CDXUTXFileMesh::Destroy()
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
HRESULT CDXUTXFileMesh::Render( bool bDrawOpaqueSubsets,
                                bool bDrawAlphaSubsets )
{
    if( NULL == m_pMesh )
        return E_FAIL;

    // Frist, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
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
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
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
HRESULT CDXUTXFileMesh::Render( ID3DXEffect* pEffect,
                                D3DXHANDLE hTexture,
                                D3DXHANDLE hDiffuse,
                                D3DXHANDLE hAmbient,
                                D3DXHANDLE hSpecular,
                                D3DXHANDLE hEmissive,
                                D3DXHANDLE hPower,
                                bool bDrawOpaqueSubsets,
                                bool bDrawAlphaSubsets )
{
    if( NULL == m_pMesh )
        return E_FAIL;

    UINT cPasses;
    // Frist, draw the subsets without alpha
    if( bDrawOpaqueSubsets )
    {
        pEffect->Begin( &cPasses, 0 );
        for( UINT p = 0; p < cPasses; ++p )
        {
            pEffect->BeginPass( p );
            for( DWORD i = 0; i < m_dwNumMaterials; i++ )
            {
                if( m_bUseMaterials )
                {
                    if( m_pMaterials[i].Diffuse.a < 1.0f )
                        continue;
                    if( hTexture )
                        pEffect->SetTexture( hTexture, m_pTextures[i] );
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
            for( DWORD i = 0; i < m_dwNumMaterials; i++ )
            {
                if( m_bUseMaterials )
                {
                    if( m_pMaterials[i].Diffuse.a == 1.0f )
                        continue;
                    if( hTexture )
                        pEffect->SetTexture( hTexture, m_pTextures[i] );
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
