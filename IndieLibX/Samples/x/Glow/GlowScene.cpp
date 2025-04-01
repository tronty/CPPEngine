/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
/*--------------------------------------------------------------------- NVMH5 -|----------------------
Path:  Sdk\Demos\Direct3D9\src\Glow\
File:  GlowScene.cpp

Copyright NVIDIA_Corporation 2003
TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS* AND NVIDIA AND
AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA
OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER
INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR INABILITY TO USE THIS
SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


Comments:


-------------------------------------------------------------------------------|--------------------*/

#include "nvafx.h"
#include "GlowScene.h"


#define TEXN_GLOW_01	("textures\\2D\\d3d_set1\\Glow_01.dds")

GlowScene::GlowScene()
{
	SetAllNull();
}

GlowScene::~GlowScene()
{	
	Free();
	SetAllNull();
}

int GlowScene::Free()
{
	int hr = 0;

	const int nNumClear = 5;
	vector< MeshVB ** > * pvToClear[nNumClear];
	vector< MeshVB ** > * pv;
	pvToClear[0]			= &m_vTerrain;
	pvToClear[1]			= &m_vBuildings;
	pvToClear[2]			= &m_vDetailObjects;
	pvToClear[3]			= &m_vParticles;
	pvToClear[4]			= &m_vCylArray;

	int i;
	for( i=0; i < nNumClear; i++ )
	{
		pv = pvToClear[i];
		if( pv != 0 )
		{
			size_t n;
			for( n=0; n < pv->size(); n++ )
			{
				if( pv->at(n) != 0 )
				{
					// delete the mesh, set ptr to 0
					SAFE_DELETE( *(pv->at(n)));
					// delete the ptr
					SAFE_DELETE( pv->at(n) );
				}
			}
			pv->clear();
		}
	}

	SAFE_DELETE( m_pTextureFactory );
	//SAFE_RELEASE( m_pD3DDev );
	return( hr );
}

int GlowScene::Initialize( IDirect3DDevice9 * pDev )
{
	int hr = 0;
	Free();
	FAIL_IF_NULL( pDev );
	m_pD3DDev = pDev;
	m_pD3DDev->AddRef();

	MeshGeoCreator gc;
	Mesh msh;
	float fHalfSize = 0.3f;
	float diag = 0.3f;
	gc.InitCylinder( &msh, D3DXFROMWINEVECTOR3( -diag, -diag, -fHalfSize ), D3DXFROMWINEVECTOR3( diag, diag, fHalfSize ),
						0.20f, 18, 0, 2, 0 );
	
	MeshVB ** ppMeshVB = new MeshVB*;
	FAIL_IF_NULL( ppMeshVB );
	*ppMeshVB = new MeshVB;
	FAIL_IF_NULL( *ppMeshVB );
	m_vDetailObjects.push_back( ppMeshVB );
	(*ppMeshVB)->CreateFromMesh( &msh, m_pD3DDev );

	// Array of cylinders
	float radius = 0.05f;
	float ball_rad = 1.6f;
	int num_long = 12;
	int num_lat = 12;
	D3DXFROMWINEMATRIX matRotY;
	D3DXFROMWINEMatrixRotationY( &matRotY, 3.14159f * 2.0f / ((float)num_long) );
	Mesh mesh_array;
	int i;
	for( i=0; i < num_lat; i++ )
	{
		float sx, sy, ex, ey;
		sx = (float)( ball_rad * sin( 3.14159f * (float)i / ((float)num_lat) ) );
		sy = (float)( ball_rad * cos( 3.14159f * (float)i / ((float)num_lat) ) );
		ex = (float)( ball_rad * sin( 3.14159f * (float)(i+1.0f) / ((float)num_lat) ) );
		ey = (float)( ball_rad * cos( 3.14159f * (float)(i+1.0f) / ((float)num_lat) ) );
		gc.InitCylinder( &msh, D3DXFROMWINEVECTOR3( sx, sy, 0.0f ), D3DXFROMWINEVECTOR3( ex, ey, 0.0f ),
						radius, 5, 0, 0, 0 );

		BYTE r, g, b;
		float fr, fg, fb;
		fr = 255.0f * cos( 3.14159f * (float)i / ((float)num_lat) );
		if( fr < 0.0f ) fr = 0.0f;
		fg = 255.0f * cos( -3.14159f / 2.0f + 3.14159f * (float)i / ((float)num_lat) );
		if( fg < 0.0f ) fg = 0.0f;
		fb = -255.0f * cos( 3.14159f * (float)i / ((float)num_lat) );
		if( fb < 0.0f ) fb = 0.0f;
		r = (BYTE)fr;
		g = (BYTE)fg;
		b = (BYTE)fb;
		msh.SetVertexColor( D3DCOLOR_ARGB( 0x80, r, g, b ) );

		gc.InitArray( &mesh_array, &msh, num_long, matRotY );

		ppMeshVB = new MeshVB*;
		FAIL_IF_NULL( ppMeshVB );
		*ppMeshVB = new MeshVB;
		FAIL_IF_NULL( *ppMeshVB );
		m_vCylArray.push_back( ppMeshVB );
		(*ppMeshVB)->CreateFromMesh( &mesh_array, m_pD3DDev );
	}

	m_pTextureFactory = new TextureFactory;
	FAIL_IF_NULL( m_pTextureFactory );
	hr = m_pTextureFactory->Initialize( GetFilePath::GetFilePath );

	m_ppTex01 = m_pTextureFactory->CreateTextureFromFile( m_pD3DDev, TEXN_GLOW_01 );


	return( hr );
}

int GlowScene::RenderObjects()
{
	int hr = 0;
	const int nNum = 5;
	vector< MeshVB ** > * pvs[nNum];
	vector< MeshVB ** > * pv;
	pvs[0]			= &m_vTerrain;
	pvs[1]			= &m_vBuildings;
	pvs[2]			= &m_vDetailObjects;
	pvs[3]			= &m_vParticles;
	pvs[4]			= &m_vCylArray;

	int i;
	for( i=0; i < nNum; i++ )
	{
		pv = pvs[i];
		if( pv != 0 )
		{
			size_t n;
			for( n=0; n < pv->size(); n++ )
			{
				if( pv->at(n) != 0 )
				{
					if( *(pv->at(n)) != 0 )
					{
						(*(pv->at(n)))->Draw();
					}
				}
			}
		}
	}
	return( hr );
}

int GlowScene::RenderObjectsWithGlowStrengthInAlpha()
{
	int hr = 0;
	FAIL_IF_NULL( m_pD3DDev );
	FAIL_IF_NULL( m_ppTex01 );
	FAIL_IF_NULL( *m_ppTex01 );

	const int nNum = 5;
	vector< MeshVB ** > * pvs[nNum];
	vector< MeshVB ** > * pv;
	pvs[0]			= &m_vTerrain;
	pvs[1]			= &m_vBuildings;
	pvs[2]			= &m_vDetailObjects;
	pvs[3]			= &m_vParticles;
	pvs[4]			= &m_vCylArray;

	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1 );
	m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );

	m_pD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,		D3DTOP_DISABLE );
	m_pD3DDev->SetTextureStageState( 1, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

	m_pD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE,		true );
	m_pD3DDev->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_ONE );
	m_pD3DDev->SetRenderState( D3DRS_DESTBLEND,				D3DBLEND_ZERO );

	m_pD3DDev->SetTexture( 0, *m_ppTex01 );
	m_pD3DDev->SetSamplerState( 0, D3DSAMP_MINFILTER,		D3DTEXF_LINEAR );
	m_pD3DDev->SetSamplerState( 0, D3DSAMP_MAGFILTER,		D3DTEXF_LINEAR );
	m_pD3DDev->SetSamplerState( 0, D3DSAMP_MIPFILTER,		D3DTEXF_POINT );

	m_pD3DDev->SetRenderState( D3DRS_ZENABLE,		true );
	m_pD3DDev->SetRenderState( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );

	int i;
	for( i=0; i < nNum; i++ )
	{
		pv = pvs[i];
		if( pv != 0 )
		{
			if( i==4 )
			{
				m_pD3DDev->SetRenderState( D3DRS_COLORVERTEX,		true );
				m_pD3DDev->SetRenderState( D3DRS_LIGHTING,			false );
				m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );
				m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE );
				m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1 );
				m_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_DIFFUSE );
			}

			size_t n;
			for( n=0; n < pv->size(); n++ )
			{
				if( pv->at(n) != 0 )
				{
					if( *(pv->at(n)) != 0 )
					{
						(*(pv->at(n)))->Draw();
					}
				}
			}
		}
	}
	return( hr );
}

