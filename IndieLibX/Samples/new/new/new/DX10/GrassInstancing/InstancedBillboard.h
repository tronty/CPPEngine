/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <Framework3/IRenderer.h>

// Copyright © 2008 Intel Corporation
// All Rights Reserved
// 
// The sample source code contained or described herein and all documents
// related to the sample source code ("Material") are owned by Intel Corporation
// or its suppliers or licensors. Title to the Material remains with Intel Corporation
// or its suppliers and licensors. The Material may contain trade secrets and
// proprietary and confidential information of Intel Corporation and its suppliers
// and licensors, and is protected by worldwide copyright and trade secret laws and
// treaty provisions. The sample source code is provided AS IS, with no warranties
// of any kind, express or implied. Except as expressly permitted by the Software
// license, neither Intel Corporation nor its suppliers assumes any responsibility
// or liability for any errors or inaccuracies that may appear herein.

#pragma once

template <class T_VERTEX, class T_INSTANCE >
class InstancedBillboard
{

	int									vertexCnt;
	T_VERTEX*							pVertices;
	exBufferID                       pVertexBuffer;

	*			pLayout;
	int									numElements;
	VertexFormatID                  pVertexLayout;

	char								pTextureName[MAX_PATH];
	ShaderResourceViewID           pTextureRV;
	stx_Effect*               pRenderTechnique;

	int									instanceCnt;
	int									maxInstanceCnt;
	T_INSTANCE*							pInstanceData;
	exBufferID						pInstanceBuffer;

	int			LoadInstData ( exBufferID  ppInstanceData, 
								   void* pInData, int dataSize );

public:

	virtual void	Init		 ( char* texName, T_INSTANCE* pInInstanceData, int cnt, T_VERTEX* pInVertices, int numVerts, 
								   * layout, int numLayoutElements, 
								   __WORD__* indices = 0, int numIndices = 0 );

	virtual void	Destroy		 ( ){ };

	virtual int CreateDevice ( stx_Effect*  pTechique );

	virtual void	DestroyDevice( );
//	virtual void    PreRender	 ( double fTime, float fElapsedTime ){};

	virtual void	Render		 ( ShaderID  pDiffuseVariable );
	
	virtual void	SetInstData	 ( T_INSTANCE* pInData, int numInstances );

};

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::SetInstData( T_INSTANCE* pInData, int numInstances )
{
	// ASSERT( numInstances > maxInstanceCnt );
	instanceCnt		= numInstances;
	void* pData		= 0;
	pInstanceBuffer->Map( D3D10_MAP_WRITE_DISCARD, 0, ( void** )&pData );
	stx_memcpy( pData, (void*)pInData, sizeof(T_INSTANCE)*instanceCnt );
	pInstanceBuffer->Unmap();
}


template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::Init( char* texName, T_INSTANCE* pInInstanceData, int cnt , T_VERTEX* pInVertices, int numVerts, 
													* layout, int numLayoutElements, 
													__WORD__* indices /*= 0*/, int numIndices /*= 0 */ )
{
	_tcscpy(pTextureName, texName);

	pVertices		 =  pInVertices;
	vertexCnt		 =  numVerts;
	pLayout			 =  layout;
	numElements		 =  numLayoutElements;
	pInstanceData	 =  pInInstanceData;
	instanceCnt		 =  cnt;
	maxInstanceCnt	 =  cnt;
}

template <class T_VERTEX, class T_INSTANCE>
int InstancedBillboard<T_VERTEX, T_INSTANCE>::CreateDevice( stx_Effect*  pTechique)
{
	int hr = 0;
	pRenderTechnique = pTechique;
	// Create the input layout
	PASS_DESC PassDesc;
	pRenderTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	hr = ->CreateInputLayout( pLayout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &pVertexLayout );

	// Initialize Vertex_ Buffers
	pVertexBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( T_VERTEX ) * vertexCnt, STATIC, (const void *)pVertices);

	// Create Instance Buffer
	// We're creating this buffer as dynamic because in a game, the instance data could be dynamic... aka
	// we could have moving trees.
	pInstanceBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( T_INSTANCE )*instanceCnt, DYNAMIC, (const void *)0);

	// Load the Texture
	pTextureRV=IRenderer::GetRendererInstance()->addImageLibTexture(pTextureName, false, IRenderer::GetRendererInstance()->Getlinear());

	//Initialize instance buffer with data passed in
	SetInstData( pInstanceData, instanceCnt ); 

	return hr;

}

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::DestroyDevice()
{
	//////////////SAFE_RELEASE( pVertexBuffer );
	//////////////SAFE_RELEASE( pVertexLayout );
	//////////////SAFE_RELEASE( pTextureRV );
	//////////////SAFE_RELEASE( pInstanceBuffer );
}

template <class T_VERTEX, class T_INSTANCE>
void InstancedBillboard<T_VERTEX, T_INSTANCE>::Render (ShaderID  pDiffuseVariable )
{
	// Set the input layout
	->setVertexFormat( pVertexLayout );

	exBufferID pVB[2];
	unsigned int strides[2];
	unsigned int offsets[2] = {0,0};
	pVB[0] = pVertexBuffer;
	pVB[1] = pInstanceBuffer;
	strides[0] = sizeof ( T_VERTEX   );
	strides[1] = sizeof ( T_INSTANCE );
	->setVertexBuffer( 0,       //first input slot for binding
									2,       //number of buffers in the array
									pVB,     //array of three vertex buffers
									strides, //array of stride values, one for each buffer
									offsets	 //array of offset values, one for each buffer
									);

	// Set primitive topology
	->DrawPrimitive( PRIM_TRIANGLES );
	pDiffuseVariable->SetResource( pTextureRV );

	// Render Instanced Billboards
	TECHNIQUE_DESC techDesc;
	pRenderTechnique->GetDesc( &techDesc );

	for( unsigned int p = 0; p < techDesc.Passes; ++p )
	{
		pRenderTechnique->GetPassByIndex( p )->Apply( 0 );
		->DrawInstanced( vertexCnt,    	// number of vertices per instance
					  instanceCnt,	 // number of instances
					  0,		 // Index of the first vertex
					  0		 // Index of the first instance
					  );
	}
}

template <class T_VERTEX, class T_INSTANCE>
int InstancedBillboard<T_VERTEX, T_INSTANCE>::LoadInstData( VertexBufferID  ppInstanceData, void* pInData, int dataSize )
{


	int hr = 0;

	// Create a resource with the input matrices
	// We're creating this buffer as dynamic because in a game, the instance data could be dynamic
	ppInstanceData=IRenderer::GetRendererInstance()->addVertexBuffer(dataSize, DYNAMIC, (const void *)0);

	void* pData = 0;
	( *ppInstanceData )->Map( D3D10_MAP_WRITE_DISCARD, 0, ( void** )&pData );

	stx_memcpy( pData, pInData, dataSize );

	( *ppInstanceData )->Unmap();
	return hr;
}

