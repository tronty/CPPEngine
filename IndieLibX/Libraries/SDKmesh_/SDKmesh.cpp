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
//include "DXUT.h"
#include "SDKmesh.h"
//include "SDKMisc.h"

//--------------------------------------------------------------------------------------
int CDXUTSDKMesh::CreateFromFile( std::string szFileName,
                                      bool bCreateAdjacencyIndices )
{
std::ifstream infile(szFileName.c_str());

//get length of file
infile.seekg(0, infile.end);
size_t length = infile.tellg();
infile.seekg(0, infile.beg);

//read file
if (length > 0) {
    buffer.resize(length);    
    infile.read(&buffer[0], length);
}
    __DWORD__ cBytes=buffer.size();
	buffer_=buffer;
	m_pStaticMeshData=&buffer_[0];

    CreateFromMemory( 
                               m_pStaticMeshData,
                               cBytes,
                               bCreateAdjacencyIndices,
                               false);

    return 0;
}

int CDXUTSDKMesh::CreateFromMemory( BYTE* pData,
                                        UINT DataBytes,
                                        bool bCreateAdjacencyIndices,
                                        bool bCopyStatic )
{
    int hr = E_FAIL;
    D3DXFROMWINEVECTOR3 lower; 
    D3DXFROMWINEVECTOR3 upper; 

    // Set outstanding resources to zero
    m_NumOutstandingResources = 0;

    if( bCopyStatic )
    {
        SDKMESH_HEADER* pHeader = ( SDKMESH_HEADER* )pData;

        SIZE_T StaticSize = ( SIZE_T )( pHeader->HeaderSize + pHeader->NonBufferDataSize );
        m_pHeapData = new BYTE[ StaticSize ];
        if( !m_pHeapData )
            return hr;

        m_pStaticMeshData = m_pHeapData;

        //CopyMemory( m_pStaticMeshData, pData, StaticSize );
	stx_memcpy( m_pStaticMeshData, pData, StaticSize );
    }
    else
    {
        m_pHeapData = pData;
        m_pStaticMeshData = pData;
    }

	LOG_PRINT("pData=%x\n", pData );
	LOG_PRINT("m_pHeapData=%x\n", m_pHeapData );
	LOG_PRINT("m_pStaticMeshData=%x\n", m_pStaticMeshData );
	LOG_PRINT("m_pMeshHeader=%x\n", m_pMeshHeader );
	LOG_PRINT("m_pMeshHeader->VertexStreamHeadersOffset=%d\n", m_pMeshHeader->VertexStreamHeadersOffset );

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

    // Setup subsets
    for( UINT i = 0; i < m_pMeshHeader->NumMeshes; i++ )
    {
        m_pMeshArray[i].pSubsets = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].SubsetOffset );
        m_pMeshArray[i].pFrameInfluences = ( UINT* )( m_pStaticMeshData + m_pMeshArray[i].FrameInfluenceOffset );
    }

    // Setup buffer data pointer
    BYTE* pBufferData = pData + m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;

    // Get the start of the buffer data
    UINT64 BufferDataStart = m_pMeshHeader->HeaderSize + m_pMeshHeader->NonBufferDataSize;


    // Create VBs
	LOG_PRINT("NumVertexBuffers=%d\n", m_pMeshHeader->NumVertexBuffers);
    m_ppVertices = new BYTE*[m_pMeshHeader->NumVertexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumVertexBuffers; i++ )
    {
        BYTE* pVertices = NULL;
        pVertices = ( BYTE* )( pBufferData + ( m_pVertexBufferArray[i].DataOffset - BufferDataStart ) );

            //CreateVertexBuffer(  &m_pVertexBufferArray[i], pVertices );

        m_ppVertices[i] = pVertices;
    }

    // Create IBs
	LOG_PRINT("NumIndexBuffers=%d\n", m_pMeshHeader->NumIndexBuffers);
    m_ppIndices = new BYTE*[m_pMeshHeader->NumIndexBuffers];
    for( UINT i = 0; i < m_pMeshHeader->NumIndexBuffers; i++ )
    {
        BYTE* pIndices = NULL;
        pIndices = ( BYTE* )( pBufferData + ( m_pIndexBufferArray[i].DataOffset - BufferDataStart ) );

            //CreateIndexBuffer(  &m_pIndexBufferArray[i], pIndices );

        m_ppIndices[i] = pIndices;
    }

            UINT *ind = ( UINT * )m_ppIndices[0];
            FLOAT *verts =  ( FLOAT* )m_ppVertices[0];
            UINT stride = (UINT)m_pVertexBufferArray[0].StrideBytes;
LOG_PRINT("verts=%x\n", verts);
LOG_PRINT("ind=%x\n", ind);
LOG_PRINT("stride=%d\n", stride);

    // Load Materials
        //LoadMaterials( m_pMaterialArray, m_pMeshHeader->NumMaterials);
	LOG_PRINT("NumFrames=%d\n", m_pMeshHeader->NumFrames);

    // Create a place to store our bind pose frame matrices
    m_pBindPoseFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];

    // Create a place to store our transformed frame matrices
    m_pTransformedFrameMatrices = new D3DXFROMWINEMATRIX[ m_pMeshHeader->NumFrames ];
#if 0
    hr = 0;

    SDKMESH_SUBSET* pSubset = NULL;

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
#endif
    // Update 
    return hr;
}
int ApplicationLogic()
{
	CDXUTSDKMesh mesh;
	mesh.CreateFromFile("../../IndieLib_resources/MotionBlur/Warrior.sdkmesh", false);

	LOG_PRINT("NumVertexBuffers=%d\n", mesh.m_pMeshHeader->NumVertexBuffers);
	LOG_PRINT("NumIndexBuffers=%d\n", mesh.m_pMeshHeader->NumIndexBuffers);
	LOG_PRINT("NumMeshes=%d\n", mesh.m_pMeshHeader->NumMeshes);
	LOG_PRINT("NumTotalSubsets=%d\n", mesh.m_pMeshHeader->NumTotalSubsets);
	LOG_PRINT("NumFrames=%d\n", mesh.m_pMeshHeader->NumFrames);
	LOG_PRINT("NumMaterials=%d\n", mesh.m_pMeshHeader->NumMaterials);

	for(unsigned int i=0;i<mesh.m_pMeshHeader->NumMaterials;i++)
	{
    		LOG_PRINT("MaterialName=%s\n", mesh.m_pMaterialArray[i].Name);
		LOG_PRINT("MaterialInstancePath=%s\n", mesh.m_pMaterialArray[i].MaterialInstancePath);
	}

	return 0;
}

