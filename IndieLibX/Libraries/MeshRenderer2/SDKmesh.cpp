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
#define LOG_FNLN //printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#else
#define LOG_PRINT(...)
#define LOG_FNLN
#endif

BYTE* pVertices_=0;
BYTE* pIndices_=0;
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::LoadMaterials( SDKMESH_MATERIAL* pMaterials, UINT numMaterials)
{
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	UINT m=0;

	for( m = 0; m < numMaterials; m++ )
	{
	pMaterials[m].pDiffuseTexture9=-1;
	pMaterials[m].pNormalTexture9=-1;
	pMaterials[m].pSpecularTexture9=-1;
#if 0
	printf("pMaterials[%d].DiffuseTexture=%s\n", m, pMaterials[m].DiffuseTexture);
	printf("pMaterials[%d].NormalTexture=%s\n", m, pMaterials[m].NormalTexture);
	printf("pMaterials[%d].SpecularTexture=%s\n", m, pMaterials[m].SpecularTexture);
#endif
	}
	//return;
#if 0
	if(m_sFileName!="../../IndieLib_resources/DXJune2010/MotionBlur/Warrior.sdkmesh")
	    return;
#endif
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
		printf("StrPath_=%s\n", StrPath_.c_str());
		stx_snprintf(m_pMaterialArray[m].DiffuseTexture, MAX_TEXTURE_NAME,"%s", StrPath_.c_str());
                if(stx_fileExists(StrPath_.c_str())) pMaterials[m].pDiffuseTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(StrPath_.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
		else {pMaterials[m].pDiffuseTexture9=-1;pMaterials[m].DiffuseTexture[0]=0;}
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
		printf("StrPath_=%s\n", StrPath_.c_str());
		stx_snprintf(m_pMaterialArray[m].NormalTexture, MAX_TEXTURE_NAME,"%s", StrPath_.c_str());
                if(stx_fileExists(StrPath_.c_str()))
                pMaterials[m].pNormalTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(StrPath_.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
		else {pMaterials[m].pNormalTexture9=-1;pMaterials[m].NormalTexture[0]=0;}
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
		printf("StrPath_=%s\n", StrPath_.c_str());
		stx_snprintf(m_pMaterialArray[m].SpecularTexture, MAX_TEXTURE_NAME,"%s", StrPath_.c_str());
                if(stx_fileExists(StrPath_.c_str())) pMaterials[m].pSpecularTexture9=IRenderer::GetRendererInstance()->addImageLibTexture(StrPath_.c_str(), false, IRenderer::GetRendererInstance()->Getlinear());
		else {pMaterials[m].pSpecularTexture9=-1;pMaterials[m].SpecularTexture[0]=0;}
            }
        }
    }
#if 0
	m=0;
	printf("pMaterials[%d].DiffuseTexture=%x\n", m, pMaterials[m].pDiffuseTexture9);
	printf("pMaterials[%d].NormalTexture=%x\n", m, pMaterials[m].pNormalTexture9);   		
	printf("pMaterials[%d].SpecularTexture=%x\n", m, pMaterials[m].pSpecularTexture9);
#endif
	//stx_exit(0);
}
//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateVertexBuffer( SDKMESH_VERTEX_BUFFER_HEADER* pHeader,
                                      void** pVertices )
{
    int hr = S_OK;
    pHeader->DataOffset = 0;
	printf("CreateVertexBuffer: SizeBytes=%d, pVertices=%x\n", ( UINT )pHeader->SizeBytes, (const void *) *pVertices);
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
	else
		stx_exit(0);

	printf("pHeader->pVB9=%x\n", pHeader->pVB9);

    return hr;
}

//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateIndexBuffer( SDKMESH_INDEX_BUFFER_HEADER* pHeader,
                                         void* pIndices )
{
    int hr = S_OK;
#if 1
	if(m_sFileName!="../../IndieLib_resources/DXJune2010/MotionBlur/Warrior.sdkmesh")
	{
	pHeader->pIB9=-1;
	    return hr;
	}
#endif
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
	
	printf("CreateIndexBuffer: SizeBytes=%d, ibFormat=%d, pIndices=%x\n", ( UINT )pHeader->SizeBytes/ibFormat, ibFormat, (const void *) pIndices);
	pHeader->pIB9=IRenderer::GetRendererInstance()->addIndexBuffer(( UINT )pHeader->SizeBytes/ibFormat, ibFormat, STATIC, (const void *) pIndices);
	printf("pHeader->pIB9=%x\n", pHeader->pIB9);

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
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    //if( 0 < GetOutstandingBufferResources() ) return;
//printf("iMesh=%x\n", iMesh);

    SDKMESH_MESH* pMesh = &m_pMeshArray[0];//iMesh]; // ???
#if 0
////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
printf("pEffect=%x\n", pEffect);
printf("vf=%x\n", vf);

printf("pMesh->Name=%s\n", ( char* )pMesh->Name);
printf("pMesh->NumVertexBuffers=%d\n", ( UINT )pMesh->NumVertexBuffers);
//UINT    VertexBuffers[MAX_VERTEX_STREAMS];
printf("pMesh->IndexBuffer=%d\n", ( UINT )pMesh->IndexBuffer);
printf("pMesh->NumSubsets=%d\n", ( UINT )pMesh->NumSubsets);
printf("pMesh->NumFrameInfluences=%d\n", ( UINT )pMesh->NumFrameInfluences);
printf("pMesh->IndexBuffer=%x\n", pMesh->IndexBuffer);
printf("m_pIndexBufferArray[%d].pIB9=%x\n", pMesh->IndexBuffer, m_pIndexBufferArray[ pMesh->IndexBuffer ].pIB9);

if(!pMesh->NumVertexBuffers)
{
	printf("m_pMeshArray[%d].NumVertexBuffers==0 !\n", iMesh, m_pMeshArray[iMesh].NumVertexBuffers);
	printf("Skipping rendering !!!\n");
	//return;
}
//stx_exit(0);
#endif

    // Render the scene with this technique 
    //pEffect->SetTechnique( hTechnique );
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		if(pEffect>-1)IRenderer::GetRendererInstance()->setShader(pEffect);
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		if(vf>-1)IRenderer::GetRendererInstance()->setVertexFormat(vf);

#if 0
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	printf("pMesh=%x\n", pMesh);
	printf("m_pVertexBufferArray=%x\n", m_pVertexBufferArray);
	printf("pMesh->VertexBuffers[%d]=%x\n", 0, pMesh->VertexBuffers[0]);
	printf("m_pVertexBufferArray[ pMesh->VertexBuffers[%d] ].pVB9=%x\n", 0, m_pVertexBufferArray[ pMesh->VertexBuffers[0] ].pVB9);
#endif
	//printf("pMesh->NumVertexBuffers=%d\n", ( UINT )pMesh->NumVertexBuffers);
    // set vb streams
    //for( UINT i = 0; i < ( UINT )pMesh->NumVertexBuffers; i++ )
    for( UINT i = 0; i < 1; i++ )
    {
	//printf("m_pVertexBufferArray[ pMesh->VertexBuffers[%d] ].pVB9=%x\n", i, m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].pVB9);
        IRenderer::GetRendererInstance()->setVertexBuffer( i,
                                     m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].pVB9);                                    
                                     //( UINT )m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].StrideBytes );
    }

	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Set our index buffer as well
	//printf("m_pIndexBufferArray[ pMesh->IndexBuffer ].pIB9=%x\n", m_pIndexBufferArray[ pMesh->IndexBuffer ].pIB9);
    IRenderer::GetRendererInstance()->setIndexBuffer(m_pIndexBufferArray[ pMesh->IndexBuffer ].pIB9 );

	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    SDKMESH_SUBSET* pSubset = 0;
    SDKMESH_MATERIAL* pMat = 0;
    Primitives PrimType; /*
    UINT cPasses = 0;
    pEffect->Begin( &cPasses, 0 );

    for( UINT p = 0; p < cPasses; ++p ) */
    {
      //  pEffect->BeginPass( p );
////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
        for( UINT subset = 0; subset < pMesh->NumSubsets; subset++ ) 
        {
////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
		//UINT subset = 0;
            pSubset = &m_pSubsetArray[ pMesh->pSubsets[subset] ];

            PrimType = GetPrimitiveType9( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
            if( INVALID_MATERIAL != pSubset->MaterialID && m_pMeshHeader->NumMaterials > 0 )
            {
////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
                pMat = &m_pMaterialArray[ pSubset->MaterialID ];
                if( htxDiffuse && ( pMat->pDiffuseTexture9 > -1 ) ){
			printf("%s %x", htxDiffuse, pMat->pDiffuseTexture9 );
                    IRenderer::GetRendererInstance()->setTexture( "txDiffuse", pMat->pDiffuseTexture9 );}
                if( htxNormal && ( pMat->pNormalTexture9 > -1 ) ){
			printf("%s %x", htxNormal, pMat->pNormalTexture9 );
                    IRenderer::GetRendererInstance()->setTexture( "txNormal", pMat->pNormalTexture9 );}
                if( htxSpecular && ( pMat->pSpecularTexture9 > -1 ) ){
			printf("%s %x", htxSpecular, pMat->pSpecularTexture9 );
                    IRenderer::GetRendererInstance()->setTexture( "txSpecular", pMat->pSpecularTexture9 );}
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
#if 1
    BYTE* pBufferData = m_pStaticMeshData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;
    UINT64 BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

        BYTE* pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[0].DataOffset - BufferDataStart ) );
        BYTE* pIndices = 0;
	if(pIndices>pVertices)
		pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[0].DataOffset - BufferDataStart ) );

		UINT NumVertices, NumIndices;
	if(pIndices>pVertices){
	for(int iVB=0;iVB<m_pMeshHeader->NumVertexBuffers;iVB++)
		{
		NumVertices=   m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices;
	UINT StrideBytes=(UINT)m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].StrideBytes;
		NumIndices=m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].NumIndices;
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
		0,
		m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices,
		m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].NumIndices,
		pIndices,
		pIndices,
		m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].IndexType?CONSTANT_INDEX2:CONSTANT_INDEX4,
		pVertices,
		pVertices,
		sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));}}
	else
	for(int iVB=0;iVB<m_pMeshHeader->NumVertexBuffers;iVB++)
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, 		
		m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices/3,
		pVertices, pVertices, sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));
#endif
        }

        //pEffect->EndPass();
    }

    //pEffect->End();
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
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
#if 0
	if(m_sFileName!="../../IndieLib_resources/DXJune2010/MotionBlur/Warrior.sdkmesh")
	    return;
#endif
    if( !m_pStaticMeshData || !m_pFrameArray )
        return;
	
    if(m_pMeshHeader->NumFrames<=iFrame)
	return;
    //printf("iFrame=%d\n", iFrame);
#if 1
	UINT64 iMesh=0; // ??? = m_pFrameArray[iFrame].Mesh;
#if 0
	printf("NumVertices=%x\n", m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[0] ].NumVertices);
	printf("SizeBytes=%x\n", m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[0] ].SizeBytes);
	printf("StrideBytes=%x\n", m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[0] ].StrideBytes);
	printf("SizeBytes=%x\n", m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[0] ].SizeBytes);
	printf("pVB9=%x\n", m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[0] ].pVB9);
	printf("SizeBytes=%x\n", m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].SizeBytes);
	printf("IndexType=%x\n", m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].IndexType);
	printf("pIB9=%x\n", m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].pIB9);
#endif
	if(!m_pMeshHeader->NumVertexBuffers)
		return;
	if(!m_pMeshHeader->NumIndexBuffers)
		return;
	if(!m_pMeshHeader->NumMeshes)
		return;

    BYTE* pBufferData = m_pStaticMeshData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;
    UINT64 BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

        BYTE* pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[0].DataOffset - BufferDataStart ) );
        BYTE* pIndices = 0;
	if(pIndices>pVertices)
		pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[0].DataOffset - BufferDataStart ) );

    SDKMESH_SUBSET* m_pSubsetArray;
#if 0
	printf("pVertices=%x\n", pVertices);
	printf("pIndices=%x\n", pIndices);
#endif
	static ShaderID SimpleShader_ = -1;
	static VertexFormatID SimpleVertexDeclaration_ = -1;
	if(SimpleShader_==-1)
	{
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
	IRenderer::GetRendererInstance()->setShader(SimpleShader_);
	IRenderer::GetRendererInstance()->setVertexFormat(SimpleVertexDeclaration_);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", mvp);
		UINT64 NumVertices, NumIndices;
#if 1
	{
#else
	SDKMESH_MESH* pMesh = &m_pMeshArray[iMesh];
    SDKMESH_SUBSET* pSubset = 0;
    SDKMESH_MATERIAL* pMat = 0;
        for( UINT subset = 0; subset < pMesh->NumSubsets; subset++ ) 
        {
#if 0
            	pSubset = &m_pSubsetArray[ pMesh->pSubsets[subset] ];
		Primitives PrimType = PRIM_TRIANGLES;//GetPrimitiveType9( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );

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
#endif	

		for(int iVB=0;iVB<m_pMeshHeader->NumVertexBuffers;iVB++)
		{
		NumVertices=   m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices;
	UINT StrideBytes=(UINT)m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].StrideBytes;
		NumIndices=m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].NumIndices;
#if 0
		pSubset = &m_pSubsetArray[ pMesh->pSubsets[subset] ];
		printf("pMesh=%x\n", pMesh);
		printf("subset=%x\n", subset);
		printf("pSubset=%x\n", pSubset);
		printf("pSubset->MaterialID=%x\n", pSubset->MaterialID);
        if( m_pMeshHeader->NumMaterials > 0 )
        {
                pMat = &m_pMaterialArray[ pSubset->MaterialID ];
		printf("pMat=%x\n", pMat);
                if( pMat->pDiffuseTexture9 > -1 )
                    IRenderer::GetRendererInstance()->setTexture( "txDiffuse", pMat->pDiffuseTexture9 );
                if( pMat->pNormalTexture9 > -1 )
                    IRenderer::GetRendererInstance()->setTexture( "txNormal", pMat->pNormalTexture9 );
                if( pMat->pSpecularTexture9 > -1 )
                    IRenderer::GetRendererInstance()->setTexture( "txSpecular", pMat->pSpecularTexture9 );
       }
#endif
#endif
#if 1
	static TextureID id=-1;
	if(id==-1)
	{
		id=IRenderer::GetRendererInstance()->addImageLibTexture("/DXJune2010/MotionBlur/warrior_diff.png", false, IRenderer::GetRendererInstance()->Getlinear());
	}
	IRenderer::GetRendererInstance()->setTexture("txDiffuse", id);
#endif
	if(pIndices>pVertices){
	for(int iVB=0;iVB<m_pMeshHeader->NumVertexBuffers;iVB++)
		{
		NumVertices=   m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices;
	UINT StrideBytes=(UINT)m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].StrideBytes;
		NumIndices=m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].NumIndices;
	IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
		0,
		m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices,
		m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].NumIndices,
		pIndices,
		pIndices,
		m_pIndexBufferArray[ m_pMeshArray[ iMesh ].IndexBuffer ].IndexType?CONSTANT_INDEX2:CONSTANT_INDEX4,
		pVertices,
		pVertices,
		sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));}}
	else
	for(int iVB=0;iVB<m_pMeshHeader->NumVertexBuffers;iVB++)
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, 		
		m_pVertexBufferArray[ m_pMeshArray[ iMesh ].VertexBuffers[iVB] ].NumVertices/3,
		pVertices, pVertices, sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));
	}
#endif

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
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    int hr = S_OK;
#if 0
	FILE* file = fopen( szFileName, "rb" );
    // Header
    SDKANIMATION_FILE_HEADER fileheader;
    //ReadFile( hFile, &fileheader, sizeof( SDKANIMATION_FILE_HEADER ), &dwBytesRead, NULL );
	__DWORD__ dwBytesRead=read(file, &fileheader, sizeof( SDKANIMATION_FILE_HEADER ));
#if 0
	fclose(file);
	file = fopen( szFileName, "rb" );
#else
	off_t ret=lseek(file, 0, SEEK_SET);
#endif

    //allocate
    m_pAnimationData = new BYTE[ ( size_t )( sizeof( SDKANIMATION_FILE_HEADER ) + fileheader.AnimationDataSize ) ];

    // read it all in
    //liMove.QuadPart = 0;
    //SetFilePointerEx( hFile, liMove, NULL, FILE_BEGIN );
	//fseek( file, 0, SEEK_START );
	//ReadFile( hFile, m_pAnimationData, ( DWORD )( sizeof( SDKANIMATION_FILE_HEADER ) + fileheader.AnimationDataSize ), &dwBytesRead, NULL ) )
	dwBytesRead=read(file, m_pAnimationData, ( __DWORD__ )( sizeof( SDKANIMATION_FILE_HEADER ) + fileheader.AnimationDataSize ));
	fclose(file);

#else
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
    /////////////////////////
    // pointer fixup
    //m_pMeshHeader = ( SDKMESH_HEADER* )m_pStaticMeshData;
    m_pAnimationHeader = ( SDKANIMATION_FILE_HEADER* )m_pAnimationData;
    m_pAnimationFrameData = ( SDKANIMATION_FRAME_DATA* )( m_pAnimationData + m_pAnimationHeader->AnimationDataOffset );

	LOG_FNLN;
	printf("m_pAnimationHeader=%x\n", m_pAnimationHeader);
	printf("m_pAnimationHeader->AnimationDataOffset=%x\n", m_pAnimationHeader->AnimationDataOffset);
	printf("m_pAnimationHeader->Version=%x\n", m_pAnimationHeader->Version);
	printf("m_pAnimationHeader->IsBigEndian=%x\n", m_pAnimationHeader->IsBigEndian);
    	printf("m_pAnimationHeader->NumFrames=%d\n", m_pAnimationHeader->NumFrames);
	printf("m_pAnimationHeader->NumAnimationKeys=%d\n", m_pAnimationHeader->NumAnimationKeys);

    UINT64 BaseOffset = sizeof( SDKANIMATION_FILE_HEADER );
    for( UINT i = 0; i < m_pAnimationHeader->NumFrames; i++ )
    {
        m_pAnimationFrameData[i].pAnimationData = ( SDKANIMATION_DATA* )( m_pAnimationData +
                                                                          m_pAnimationFrameData[i].DataOffset +
                                                                          BaseOffset );
	printf("m_pAnimationFrameData[%d].FrameName=%s\n", i, m_pAnimationFrameData[i].FrameName);
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
	printf("outstandingResources=%d\n", outstandingResources);
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
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	m_sFileName=szFileName;
    int hr = S_OK;
	stx_strlcpy(m_strPath, szFileName, MAX_PATH);
		int strLastSlash = int(stx_strrchr( m_strPath, '/' )-m_strPath);
		//if(!strLastSlash) return;
		m_strPath[strLastSlash]='\0';
		std::string StrPath;
                printf("m_strPath=%s\n", m_strPath);
		printf("m_sFileName=%s\n", m_sFileName.c_str());
    // Get the file size
    UINT cBytes = 0;
#if 0
	__DWORD__ dwBytesRead=0;

	FILE* file = fopen( szFileName, "rb" );
		fseek( file, 0, SEEK_END );
		cBytes = ftell( file );
		fseek( file, 0, SEEK_SET );
		printf("cBytes=%d\n", cBytes);		
		BYTE* pStaticMeshData = new BYTE[ cBytes ];
		printf("pStaticMeshData=%x\n", pStaticMeshData);

    // Read in the file
    dwBytesRead=read(file, pStaticMeshData, cBytes);
	fclose(file);
#else
  BYTE* pStaticMeshData = 0;
  ifstream file (m_sFileName.c_str(), ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    cBytes = file.tellg();
    pStaticMeshData = new BYTE[ cBytes ];
    file.seekg (0, ios::beg);
    file.read (pStaticMeshData, cBytes);
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
////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
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
	printf("StaticSize=%d\n", StaticSize);
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
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	printf("m_pStaticMeshData=%x\n", m_pStaticMeshData);
    m_pMeshHeader = ( SDKMESH_HEADER* )m_pStaticMeshData;
	printf("m_pMeshHeader=%x\n", m_pMeshHeader);
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    m_pVertexBufferArray = ( SDKMESH_VERTEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                              m_pMeshHeader->VertexStreamHeadersOffset );
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    m_pIndexBufferArray = ( SDKMESH_INDEX_BUFFER_HEADER* )( m_pStaticMeshData +
                                                            m_pMeshHeader->IndexStreamHeadersOffset );
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    m_pMeshArray = ( SDKMESH_MESH* )( m_pStaticMeshData + m_pMeshHeader->MeshDataOffset );
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    m_pSubsetArray = ( SDKMESH_SUBSET* )( m_pStaticMeshData + m_pMeshHeader->SubsetDataOffset );
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    m_pFrameArray = ( SDKMESH_FRAME* )( m_pStaticMeshData + m_pMeshHeader->FrameDataOffset );
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    m_pMaterialArray = ( SDKMESH_MATERIAL* )( m_pStaticMeshData + m_pMeshHeader->MaterialDataOffset );
	////printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#if 0
	printf("m_pVertexBufferArray=%x\n", m_pVertexBufferArray);
	printf("m_pIndexBufferArray=%x\n", m_pIndexBufferArray);
	printf("m_pMeshArray=%x\n", m_pMeshArray);
	printf("m_pSubsetArray=%x\n", m_pSubsetArray);
	printf("m_pFrameArray=%x\n", m_pFrameArray);
	printf("m_pMaterialArray=%x\n", m_pMaterialArray);
#else
	printf("m_pMeshHeader->VertexStreamHeadersOffset=%x\n", m_pMeshHeader->VertexStreamHeadersOffset);
	printf("m_pMeshHeader->IndexStreamHeadersOffset=%x\n", m_pMeshHeader->IndexStreamHeadersOffset);
	printf("m_pMeshHeader->MeshDataOffset=%x\n", m_pMeshHeader->MeshDataOffset);
	printf("m_pMeshHeader->SubsetDataOffset=%x\n", m_pMeshHeader->SubsetDataOffset);
	printf("m_pMeshHeader->FrameDataOffset=%x\n", m_pMeshHeader->FrameDataOffset);
	printf("m_pMeshHeader->MaterialDataOffset=%x\n", m_pMeshHeader->MaterialDataOffset);
#endif
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	printf("m_pMeshHeader->NumMeshes=%d\n", m_pMeshHeader->NumMeshes);
    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
        m_pMeshArray[i].pSubsets = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].SubsetOffset );
        m_pMeshArray[i].pFrameInfluences = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].FrameInfluenceOffset );
    }

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Setup buffer data pointer
    BYTE* pBufferData = 0;

    // Get the start of the buffer data
    UINT64 BufferDataStart = 0;

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	printf("SDKMESH_FILE_VERSION=%x\n", SDKMESH_FILE_VERSION);
	printf("m_pMeshHeader->Version=%x\n", m_pMeshHeader->Version);
	printf("m_pMeshHeader->IsBigEndian=%x\n", m_pMeshHeader->IsBigEndian);
    // error condition
    if( m_pMeshHeader->Version != SDKMESH_FILE_VERSION )
    {
        printf("E_NOINTERFACE\n");
        //goto Error;
    }

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Setup buffer data pointer
    pBufferData = pData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Get the start of the buffer data
    BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Create VBs
    printf("m_pMeshHeader->NumVertexBuffers=%d\n", m_pMeshHeader->NumVertexBuffers);
    m_ppVertices = new BYTE*[m_pMeshHeader->NumVertexBuffers];
	UINT i = 0;
        BYTE* pVertices = NULL;
    for( i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[i].DataOffset - BufferDataStart ) );

            CreateVertexBuffer( &m_pVertexBufferArray[i], &pVertices );

        m_ppVertices[i] = pVertices;
    }
	pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[0].DataOffset - BufferDataStart ) );
	pVertices_=pVertices;

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Create IBs
    printf("m_pMeshHeader->NumIndexBuffers=%d\n", m_pMeshHeader->NumIndexBuffers);
#if 0
    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );

        if( pDev10 )
            CreateIndexBuffer( pDev10, &m_pIndexBufferArray[i], pIndices, pLoaderCallbacks10 );
        else if( pDev9 )
            CreateIndexBuffer( pDev9, &m_pIndexBufferArray[i], pIndices, pLoaderCallbacks9 );

        m_ppIndices[i] = pIndices;
    }
#else
    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices_ = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );
printf("pIndices=%x\n", pIndices);
	pIndices = pIndices_;
	if(pVertices_<pIndices_)
            printf("// ??? CreateIndexBuffer( &m_pIndexBufferArray[i], pIndices );\n");
	else
	{
		printf("pVertices>=pIndices !\n");
		printf("CreateIndexBuffer passed !\n");
	}

        m_ppIndices[i] = pIndices;
    }
#endif
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
#if 1
    // Load Materials
        printf("NumMaterials=%d\n", m_pMeshHeader->NumMaterials );
	printf("m_pMaterialArray=%x\n", m_pMaterialArray);
        LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials );
#else
        LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials );
#endif

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    for( UINT i = 0; i < m_pMeshHeader->NumMaterials; i++ )
    {
    printf("m_pMaterialArray[%d].DiffuseTexture=%s\n", i, m_pMaterialArray[i].DiffuseTexture);
    printf("m_pMaterialArray[%d].NormalTexture=%s\n", i, m_pMaterialArray[i].NormalTexture);
    printf("m_pMaterialArray[%d].SpecularTexture=%s\n", i, m_pMaterialArray[i].SpecularTexture);
	}
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Create a place to store our bind pose frame matrices
    m_pBindPoseFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pBindPoseFrameMatrices )
        goto Error;

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    // Create a place to store our transformed frame matrices
    m_pTransformedFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];
    if( !m_pTransformedFrameMatrices )
        goto Error;

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
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

	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
    //if( !pLoaderCallbacks10 && !pLoaderCallbacks9 )
    {
        CheckLoadDone();
    }
	//printf("%s:%s:%d\n", __FILE__,__FUNCTION__, __LINE__);
	//stx_exit(0);
    return hr;
}

