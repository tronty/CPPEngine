/*
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
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

struct SimpleVertex
{
	D3DXFROMWINEVECTOR3 Pos;
	D3DXFROMWINEVECTOR2 Tex;
};

// Define a default input layout
FormatDesc defaultlayout[] =
{
	0, TYPE_VERTEX,   FORMAT_FLOAT, 4,
	0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
};

template < class T_VERTEX >
class Billboard 
{
	int									vertexCnt;
	T_VERTEX*							pVertices;
	*			pLayout;
	int									numElements;
	VertexFormatID                  pVertexLayout;
	exBufferID                       pVertexBuffer;
	char								pTextureName[MAX_PATH];
	ShaderResourceViewID           pTextureRV;
	stx_Effect*               pRenderTechnique;

public:
	Billboard( );
	virtual ~Billboard(){};

	virtual void	Init		 ( char* texName, T_VERTEX* pInVertices, int numVerts, * layout = (*)defaultlayout, int numLayoutElements = defaultnumElements, 
									__WORD__* indices = 0, int numIndices = 0 );
	virtual void	Destroy		 ( ) { };
	
	virtual int CreateDevice ( stx_Effect*  pTechique );
	
	virtual void	DestroyDevice( );
	
	virtual void	Render		 ( ShaderID  pDiffuseVariable );

};

template < class T_VERTEX  >
int Billboard<T_VERTEX >
::CreateDevice( stx_Effect*  pTechique )
{
	int hr		 = 0;
	pRenderTechnique = pTechique;

	// Create the input layout
	PASS_DESC PassDesc;
	pRenderTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	hr = ->CreateInputLayout( pLayout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &pVertexLayout );
	( hr );

	// Initialize Vertex_ Buffers
	pVertexBuffer=IRenderer::GetRendererInstance()->addVertexBuffer(sizeof( T_VERTEX ) * vertexCnt, STATIC, (const void *)pVertices);

	// Load the Texture
	pTextureRV=IRenderer::GetRendererInstance()->addImageLibTexture(pTextureName, false, IRenderer::GetRendererInstance()->Getlinear());

	return hr;

}

template < class T_VERTEX  >
Billboard<T_VERTEX >::Billboard()
{
	pVertexLayout	 = 0;
	pVertexBuffer	 = 0;
	pTextureRV		 = 0;
	pRenderTechnique = 0;

}

template < class T_VERTEX  >
void Billboard<T_VERTEX >
::Init( char* texName, T_VERTEX* pInVertices, int numVerts, * layout, int numLayoutElements, __WORD__* indices /*= 0*/, int numIndices /*= 0 */ )
{
	_tcscpy(pTextureName, texName);
	pVertices	= pInVertices;
	vertexCnt	= numVerts;
	pLayout		= layout;
	numElements = numLayoutElements;
}



template < class T_VERTEX  >
void Billboard<T_VERTEX >
::DestroyDevice()
{
	//////////////SAFE_RELEASE( pVertexBuffer );
	//////////////SAFE_RELEASE( pVertexLayout );
	//////////////SAFE_RELEASE( pTextureRV );
}
template < class T_VERTEX  >
void Billboard<T_VERTEX >
::Render( ShaderID  pDiffuseVariable )
{
	// Set diffuse texture 
	pDiffuseVariable->SetResource( pTextureRV );

	// Set Vertex_ Buffers & layout
	->setVertexFormat( pVertexLayout );

	unsigned int offset = 0,  stride = sizeof( SimpleVertex );
	->setVertexBuffer( 0, 1, &pVertexBuffer, &stride, &offset );
	->DrawPrimitive( PRIM_TRIANGLES );

	// Render Billboard
	TECHNIQUE_DESC techDesc;
	pRenderTechnique->GetDesc( &techDesc );

	for( unsigned int p = 0; p < techDesc.Passes; ++p )
	{
		pRenderTechnique->GetPassByIndex( p )->Apply( 0 );
		->Draw( vertexCnt, 0 );
	}

}

