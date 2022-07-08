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

UINT g_iMeshVertexBuffers=0;
UINT g_iMeshIndexBuffers=0;

struct uint4
{
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;
};
struct VS_Soldier52
{
    float3 Position;
    float2 Tex;
    uint4 Bones;
    float4 Weights;
};
struct VS_Soldier64
{
    float3 Position;  
    float2 Tex;
    float4 Transform1;
    float4 Transform2;
    float3 m3;
};
struct VS_Soldier
{
    float Tex_[16];

};
struct VS_Warrior
{
    float3 Position;    
    float3 Normal;  
    float2 Tex;
    float3 Tan;
    uint4 Bones;
    float4 Weights;
};
struct VS_Tree
{
    float3 Position;    
    float3 Normal;  
    float2 Tex;
};
struct VS_Dwarf
{
    float3 Position;    
    float3 Normal;  
    float2 Tex;
};
//--------------------------------------------------------------------------------------
void CDXUTSDKMesh::Dump()
{
    LOG_PRINT("NumVertexBuffers=%d\n", m_pMeshHeader->NumVertexBuffers);
    LOG_PRINT("NumIndexBuffers=%d\n", m_pMeshHeader->NumIndexBuffers);
    LOG_PRINT("NumMeshes=%d\n", m_pMeshHeader->NumMeshes);
    LOG_PRINT("NumTotalSubsets=%d\n", m_pMeshHeader->NumTotalSubsets);
    LOG_PRINT("NumFrames=%d\n", m_pMeshHeader->NumFrames);
    LOG_PRINT("NumMaterials=%d\n", m_pMeshHeader->NumMaterials);

    SDKMESH_SUBSET* pSubset = NULL;
    SDKMESH_MESH* currentMesh = 0;
    int tris = 0;
    for (UINT meshi=0; meshi < m_pMeshHeader->NumMeshes; ++meshi)
    {
	currentMesh = &m_pMeshArray[meshi];
	LOG_PRINT("meshi=%d\n", meshi);
    	LOG_PRINT("m_ppVertices[%d]=%x\n", (UINT)currentMesh->VertexBuffers[0], (BYTE*)m_ppVertices[currentMesh->VertexBuffers[0]]);
    	LOG_PRINT("m_ppIndices[%d]=%x\n", (UINT)currentMesh->IndexBuffer, (BYTE*)m_ppIndices[currentMesh->IndexBuffer]);
        //(BYTE*)pVertices=(BYTE*)m_ppVertices[currentMesh->VertexBuffers[0]];
	//(BYTE*)pIndices=(BYTE*)m_ppIndices[currentMesh->IndexBuffer];
        INT indsize;
        if (m_pIndexBufferArray[currentMesh->IndexBuffer].IndexType == IT_16BIT ) {
            indsize = 2;
        }else {
            indsize = 4;        
        }
            UINT *ind = ( UINT * )m_ppIndices[currentMesh->IndexBuffer];
            FLOAT *verts =  ( FLOAT* )m_ppVertices[currentMesh->VertexBuffers[0]];
            UINT stride = (UINT)m_pVertexBufferArray[currentMesh->VertexBuffers[0]].StrideBytes;
	LOG_PRINT("verts=%x\n", verts);
	LOG_PRINT("ind=%x\n", ind);
	LOG_PRINT("stride=%d\n", stride);
	LOG_PRINT("indsize=%d\n", indsize);
    
        for( UINT subset = 0; subset < currentMesh->NumSubsets; subset++ )
        {
	LOG_PRINT("subset=%d:\n", subset);
	LOG_PRINT("m_pMaterialArray[%d]=%s\n", subset, m_pMaterialArray[subset].DiffuseTexture);
            pSubset = &m_pSubsetArray[ currentMesh->pSubsets[subset] ];
#if 0
            UINT IndexCount = 0;if(pSubset) IndexCount = ( UINT )pSubset->IndexCount;
            UINT IndexStart = 0;if(pSubset) IndexStart = ( UINT )pSubset->IndexStart;
	LOG_PRINT("IndexCount=%d\n", IndexCount);
	LOG_PRINT("IndexStart=%d\n", IndexStart);
#endif
	}
    }
	LOG_PRINT("sizeof(VS_Dwarf)=%d\n", sizeof(VS_Dwarf));
	LOG_PRINT("sizeof(VS_Soldier52)=%d\n", sizeof(VS_Soldier52));
	LOG_PRINT("sizeof(VS_Soldier64)=%d\n", sizeof(VS_Soldier64));
	LOG_PRINT("sizeof(VS_Soldier)=%d\n", sizeof(VS_Soldier));
	LOG_PRINT("sizeof(VS_Warrior)=%d\n", sizeof(VS_Warrior));
	LOG_PRINT("sizeof(VS_Tree)=%d\n", sizeof(VS_Tree));
	LOG_PRINT("sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture)=%d\n", sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));
}
void CDXUTSDKMesh::SimpleRender2()
{
	static ShaderID SimpleShader_ = -1;
	static VertexFormatID SimpleVertexDeclaration_ = -1;
	if(SimpleShader_==-1)
	{
#ifndef __ORIGINAL_BUFFERS__
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
#else
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
						0, TYPE_TEXCOORD, FORMAT_FLOAT, 2,
						0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_TEXCOORD,  FORMAT_UBYTE, 4,
						0, TYPE_TEXCOORD,  FORMAT_FLOAT, 4
					};
		SimpleShader_ = IRenderer::GetRendererInstance()->addShaderFromFile("/DXJune2010/rest.hlsl", "main_Warrior", "main");
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
		SimpleShader_ = IRenderer::GetRendererInstance()->addShaderFromFile("/DXJune2010/rest.hlsl", "main_Tree", "main");
		SimpleVertexDeclaration_ = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), SimpleShader_);
	}
#endif
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
#if 0
	IRenderer::GetRendererInstance()->setTexture("txDiffuse", 3);
#if 1
	for(unsigned int i=0;i<g_vDump.size();i++)
	{



m_pSubsetArray
SDKMESH_SUBSET
{
    char Name[MAX_SUBSET_NAME];
    UINT MaterialID;
    UINT PrimitiveType;
    UINT64 IndexStart;
    UINT64 IndexCount;
    UINT64 VertexStart;
    UINT64 VertexCount;




	//UINT NumVertexBuffers=m_pVertexBufferArray[ m_pMeshArray[ i ]].NumVertexBuffers;
	//for( j = 0; j < m_pMeshHeader->NumVertexBuffers; j++ ){}

	UINT NumVertices_=m_pVertexBufferArray[ m_pMeshArray[ i ].VertexBuffers[0] ].NumVertices;
	UINT NumIndices_ =m_pIndexBufferArray[ m_pMeshArray[ i ].IndexBuffer ].NumIndices;
	
        BYTE* pVertices=(BYTE*)*m_ppVertices[m_pMeshArray[ i ].VertexBuffers[0]];
	BYTE* pIndices=(BYTE*)*m_ppIndices[m_pMeshArray[ i ].IndexBuffer];
#if 0
	LOG_PRINT("pVertices=%x\n", pVertices);	
	LOG_PRINT("pIndices=%x\n", pIndices);
#endif
	//LOG_PRINT("NumVertices_=%d\n", NumVertices_);
	if(!NumIndices_){
#if 0
		LOG_PRINT("NumVertices_=%d\n", NumVertices_);
		LOG_PRINT("pVertices=%x\n", i, pVertices);
#endif
		IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES,
			NumVertices_/3,
			pVertices,
			pVertices,
			//StrideBytes_);
			sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));}
	else {
#if 0
		LOG_PRINT("NumVertices_=%d\n", NumVertices_);
		LOG_PRINT("NumIndices_=%d\n", NumIndices_);
		LOG_PRINT("pVertices=%x\n", i, pVertices);
		LOG_PRINT("pIndices=%x\n", i, pIndices);
#endif
		if(pVertices>0)
		if(pIndices>0)
		if(pVertices<pIndices)
			IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
			0,
			NumVertices_,
			NumIndices_/3,
			pIndices,
			pIndices,
			(g_vDump[i].indSize_==2)?CONSTANT_INDEX2:CONSTANT_INDEX4,
			pVertices,
			pVertices,
			//StrideBytes_);
			sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture));
		}
	}
#endif
#endif


    LOG_PRINT("m_pMeshHeader->NumMeshes=%d\n", m_pMeshHeader->NumMeshes);
        for( UINT iMesh = 0; iMesh < m_pMeshHeader->NumMeshes; iMesh++ )
    {
    SDKMESH_MESH* pMesh = &m_pMeshArray[iMesh];
    // set vb streams
#if 0
    for( UINT i = 0; i < ( UINT )pMesh->NumVertexBuffers; i++ )
    {
        IRenderer::GetRendererInstance()->setVertexBuffer( i,
                                     m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].pVB9); /* ,
                                     0,
                                     ( UINT )m_pVertexBufferArray[ pMesh->VertexBuffers[i] ].StrideBytes ); */
    }
#else
	BYTE* pVB = m_pVertexBufferArray[ pMesh->VertexBuffers[0] ].pVB9;
#endif
    // Set our index buffer as well
    BYTE* pIB = m_pIndexBufferArray[ pMesh->IndexBuffer ].pIB9;

    SDKMESH_SUBSET* pSubset = NULL;
    SDKMESH_MATERIAL* pMat = NULL;
    UINT PrimType;

LOG_PRINT("pMesh->NumSubsets=%d\n", pMesh->NumSubsets);
        for( UINT subset = 0; subset < pMesh->NumSubsets; subset++ )
        {
//LOG_FNLN;
LOG_PRINT("iMesh=%d\nsubset=%d\n", iMesh, subset );
	IRenderer::GetRendererInstance()->setShader(SimpleShader_);
	IRenderer::GetRendererInstance()->setVertexFormat(SimpleVertexDeclaration_);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("WorldViewProjection", mvp);
            pSubset = &m_pSubsetArray[ pMesh->pSubsets[subset] ];		
	    //LOG_PRINT("pSubset=%x\n", pSubset );
            if(m_pMaterialArray[subset].pDiffuseTexture9>-1) IRenderer::GetRendererInstance()->setTexture( "txDiffuse", m_pMaterialArray[subset].pDiffuseTexture9 );
            if(m_pMaterialArray[subset].pNormalTexture9>-1) IRenderer::GetRendererInstance()->setTexture( "txNormal", m_pMaterialArray[subset].pNormalTexture9 );
            if(m_pMaterialArray[subset].pSpecularTexture9>-1) IRenderer::GetRendererInstance()->setTexture( "txSpecular", m_pMaterialArray[subset].pSpecularTexture9 );
#if 1
            LOG_PRINT("setShader=%x\n", SimpleShader_);
            LOG_PRINT("setVertexFormat=%x\n", SimpleVertexDeclaration_);
            LOG_PRINT("setShaderConstant4x4f=%x\n", &mvp);
            LOG_PRINT("pDiffuseTexture9=%d\n", m_pMaterialArray[subset].pDiffuseTexture9 );
            LOG_PRINT("pNormalTexture9=%d\n", m_pMaterialArray[subset].pNormalTexture9 );
            LOG_PRINT("pSpecularTexture9=%d\n", m_pMaterialArray[subset].pSpecularTexture9 );
#endif
	    //LOG_PRINT("txDiffuse[%d]=%d\n", subset, m_pMaterialArray[subset].pDiffuseTexture9 );
            PrimType = PT_TRIANGLE_LIST;//( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType;
            UINT PrimCount = ( UINT )pSubset->IndexCount;
            UINT IndexStart = ( UINT )pSubset->IndexStart;
            UINT VertexStart = ( UINT )pSubset->VertexStart;
            UINT VertexCount = ( UINT )pSubset->VertexCount;

		PrimCount = ( UINT )m_pIndexBufferArray[iMesh].NumIndices;
		VertexCount = ( UINT )m_pVertexBufferArray[iMesh].NumVertices;

            PrimCount /= 3;
	    UINT sv=sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture);
#ifdef __ORIGINAL_BUFFERS__
	    sv=( UINT )m_pVertexBufferArray[ pMesh->VertexBuffers[iMesh] ].StrideBytes;
#endif
//LOG_FNLN;
		if(pIB>pVB){
            //IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP( PrimType, VertexStart, 0, VertexCount, IndexStart, PrimCount );
		
	LOG_PRINT("VertexCount=%d\nPrimCount=%d\n", VertexCount, PrimCount);
	LOG_PRINT("pIB=%x\n", pIB);
	LOG_PRINT("pVB=%x\n", pVB);
	LOG_PRINT("sv=%d\n", sv);
	LOG_PRINT("it=%d\n", ((m_pIndexBufferArray[ pMesh->IndexBuffer ].IndexType==0)?2:4));
	    IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES,
			0,
			VertexCount,
			PrimCount,
			pIB,
			pIB,
			((m_pIndexBufferArray[ pMesh->IndexBuffer ].IndexType==0)?CONSTANT_INDEX2:CONSTANT_INDEX4),
			pVB,
			pVB,
			sv);
		}else{
	LOG_PRINT("PrimCount=%d\n", PrimCount);
	LOG_PRINT("pVB=%x\n", pVB);
	LOG_PRINT("sv=%d\n", sv);
	    IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES,
			PrimCount,
			pVB,
			pVB,
			sv);}
	}
	}
#if 0
	LOG_PRINT("sizeof(VS_Soldier52)=%d\n", sizeof(VS_Soldier52));
	LOG_PRINT("sizeof(VS_Soldier64)=%d\n", sizeof(VS_Soldier64));
	LOG_PRINT("sizeof(VS_Soldier)=%d\n", sizeof(VS_Soldier));
#endif
	//stx_exit(0);
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
	LOG_PRINT("pVertices=%x\n", *pVertices);
	LOG_PRINT("pHeader=%x\n", pHeader);
	LOG_PRINT("pHeader->NumVertices=%d\n", pHeader->NumVertices);
	LOG_PRINT("pHeader->SizeBytes=%d\n", pHeader->SizeBytes);
	LOG_PRINT("pHeader->StrideBytes=%d\n", pHeader->StrideBytes);

	pHeader->pVB9=-1;
	pHeader->pVB9=*pVertices;
#ifndef __ORIGINAL_BUFFERS__
	stx_VertexPositionNormalBiNormalTangentColor3Texture* pVertices2=new stx_VertexPositionNormalBiNormalTangentColor3Texture[pHeader->NumVertices]; 
#ifndef _MSC_VER
	if(m_sFileName=="../../IndieLib_resources/DXJune2010/Dwarf/dwarf.sdkmesh")
#else
	if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Dwarf\\dwarf.sdkmesh")
#endif
	{	for(unsigned int i=0;i<pHeader->NumVertices;i++)
	{
		VS_Dwarf* s=((VS_Dwarf*)*pVertices)+0;
		VS_Dwarf* v=((VS_Dwarf*)*pVertices)+i;
		if(((v-s)*sizeof(VS_Dwarf))>pHeader->SizeBytes)
		{
			LOG_PRINT("((v-s)*sizeof(VS_Dwarf))>pHeader->SizeBytes\n");
			break;
		}
		pVertices2[i].Position=v->Position;
		pVertices2[i].Tex=v->Tex;
	}}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/Soldier/soldier.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Soldier\\soldier.sdkmesh")
#endif
	{	for(unsigned int i=0;i<pHeader->NumVertices;i++)
	{
		VS_Soldier52* s=((VS_Soldier52*)*pVertices)+0;
		VS_Soldier52* v=((VS_Soldier52*)*pVertices)+i;
		if(((v-s)*sizeof(VS_Soldier52))>pHeader->SizeBytes)
		{
			LOG_PRINT("((v-s)*sizeof(VS_Soldier52))>pHeader->SizeBytes\n");
			break;
		}
		pVertices2[i].Position=v->Position;
		pVertices2[i].Tex=v->Tex;
	}}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/MotionBlur/Warrior.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\MotionBlur\\Warrior.sdkmesh")
#endif
	{	for(unsigned int i=0;i<pHeader->NumVertices;i++)
	{
		VS_Warrior* s=((VS_Warrior*)*pVertices)+0;
		VS_Warrior* v=((VS_Warrior*)*pVertices)+i;
		if(((v-s)*sizeof(VS_Warrior))>pHeader->SizeBytes)
		{
			LOG_PRINT("((v-s)*sizeof(VS_Warrior))>pHeader->SizeBytes\n");
			break;
		}
		pVertices2[i].Position=v->Position;
		pVertices2[i].Tex=v->Tex;
	}}
#ifndef _MSC_VER
	else if(m_sFileName!="../../IndieLib_resources/DXJune2010/trees/tree.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\trees\\tree.sdkmesh")
#endif
	{	for(unsigned int i=0;i<pHeader->NumVertices;i++)
	{
		VS_Tree* s=((VS_Tree*)*pVertices)+0;
		VS_Tree* v=((VS_Tree*)*pVertices)+i;
		if(((v-s)*sizeof(VS_Tree))>pHeader->SizeBytes)
		{
			LOG_PRINT("((v-s)*sizeof(VS_Tree))>pHeader->SizeBytes\n");
			break;
		}
		pVertices2[i].Position=v->Position;
		pVertices2[i].Tex=v->Tex;
	}}
#ifdef __ORIGINAL_BUFFERS__
#ifndef _MSC_VER
	if(m_sFileName=="../../IndieLib_resources/DXJune2010/Dwarf/dwarf.sdkmesh")
#else
	if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Dwarf\\dwarf.sdkmesh")
#endif
	{	
		pHeader->SizeBytes=pHeader->NumVertices*sizeof(VS_Dwarf);
		pHeader->StrideBytes=sizeof(VS_Dwarf);
	}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/Soldier/soldier.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\Soldier\\soldier.sdkmesh")
#endif
	{	
		pHeader->SizeBytes=pHeader->NumVertices*sizeof(VS_Soldier52);
		pHeader->StrideBytes=sizeof(VS_Soldier52);
	}
#ifndef _MSC_VER
	else if(m_sFileName=="../../IndieLib_resources/DXJune2010/MotionBlur/Warrior.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\MotionBlur\\Warrior.sdkmesh")
#endif
	{	
		pHeader->SizeBytes=pHeader->NumVertices*sizeof(VS_Warrior);
		pHeader->StrideBytes=sizeof(VS_Warrior);
	}
#ifndef _MSC_VER
	else if(m_sFileName!="../../IndieLib_resources/DXJune2010/trees/tree.sdkmesh")
#else
	else if(m_sFileName=="..\\..\\IndieLib_resources\\DXJune2010\\trees\\tree.sdkmesh")
#endif
	{	
		pHeader->SizeBytes=pHeader->NumVertices*sizeof(VS_Tree);
		pHeader->StrideBytes=sizeof(VS_Tree);
	}
	*pVertices=pVertices2;
	pHeader->pVB9=(BYTE*)pVertices2;
#else
	{	
		pHeader->SizeBytes=pHeader->NumVertices*sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture);
		pHeader->StrideBytes=sizeof(stx_VertexPositionNormalBiNormalTangentColor3Texture);
	}
	pHeader->pVB9=(BYTE*)pVertices;
#endif
#elif 0
	pHeader->pVB9=IRenderer::GetRendererInstance()->addVertexBuffer(pHeader->SizeBytes, STATIC, pVertices);
#endif
	m_pVertexBufferArray[g_iMeshVertexBuffers++].NumVertices=pHeader->NumVertices;
	return 0;
}

//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateIndexBuffer( SDKMESH_INDEX_BUFFER_HEADER* pHeader,
                                         void** pIndices )
{
	LOG_FNLN
	LOG_PRINT("pIndices=%x\n", *pIndices);
	LOG_PRINT("pHeader=%x\n", pHeader);
	LOG_PRINT("pHeader->NumIndices=%d\n", pHeader->NumIndices);
	LOG_PRINT("pHeader->SizeBytes=%d\n", pHeader->SizeBytes);
	LOG_PRINT("pHeader->IndexType=%d\n", pHeader->IndexType);

	unsigned int ibs=CONSTANT_INDEX2;
	if(pHeader->SizeBytes==IT_32BIT)
	{
		ibs=CONSTANT_INDEX4;
	}

	ibs=pHeader->SizeBytes/pHeader->NumIndices;
	LOG_PRINT("ibs=%d\n", ibs);

	pHeader->pIB9=-1;
	pHeader->pIB9=*pIndices;
	//pHeader->pIB9=IRenderer::GetRendererInstance()->addIndexBuffer(pHeader->NumIndices, ibs, STATIC, pIndices);
	m_pIndexBufferArray[g_iMeshIndexBuffers++].NumIndices=pHeader->NumIndices;
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
    LOG_PRINT("the entire file content is in memory 1\n");
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
	
	LOG_PRINT("m_pMeshHeader->NumMeshes=%d\n", m_pMeshHeader->NumMeshes);
    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
	LOG_PRINT("m_pMeshArray[%d].NumSubsets=%d\n", i, m_pMeshArray[i].NumSubsets);
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
	//pVertices_=pVertices;

    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
        BYTE* pIndices = NULL;
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        pIndices = NULL;
        pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );
        CreateIndexBuffer( &m_pIndexBufferArray[i], (void**)&pIndices );
        m_ppIndices[i] = pIndices;
    }

    // Load Materials
        LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials );
    hr = S_OK;
	Dump();
	if(pVertices>=pIndices)
	{
		LOG_PRINT("pVertices>=pIndices\n");
		//stx_exit(0);
	}
    return hr;
}

