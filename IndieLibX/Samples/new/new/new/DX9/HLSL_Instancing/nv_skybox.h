/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//-----------------------------------------------------------------------------
// Copyright NVIDIA Corporation 2004
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED 
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS 
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL 
// NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR 
// CONSEQUENTIAL DAMAGES WHATSOEVER INCLUDING, WITHOUT LIMITATION, DAMAGES FOR 
// LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS 
// INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR 
// INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGES.
// 
// File: nv_skybox.h
// Desc: A little wrapper that makes and renders a skybox mesh.
//-----------------------------------------------------------------------------
#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

class nv_SkyBox
{
	class WorldBoxVertex
	{
	public:
		WorldBoxVertex(const D3DXFROMWINEVECTOR3& vecPosition, const D3DXFROMWINEVECTOR3& vecTexture)
			: mPosition(vecPosition)
			, mTexture(vecTexture)
		{};

		D3DXFROMWINEVECTOR3 mPosition;
		D3DXFROMWINEVECTOR3 mTexture;
	};
	#define WORLDBOX_FVF (D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0))

public:

	nv_SkyBox()
	{
		m_bInit = false;
		m_CubeMap = 0;
		m_pWorldBoxVertices = 0;
		m_pWorldBoxIndices = 0;
	}

	int Init( d3dDevice)
	{
		int hr;

		// Create world-cube buffers
		(d3dDevice->CreateVertexBuffer(24 * sizeof(WorldBoxVertex), D3DUSAGE_WRITEONLY, WORLDBOX_FVF, D3DPOOL_MANAGED, &m_pWorldBoxVertices, 0));
		(d3dDevice->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pWorldBoxIndices, 0));

		WorldBoxVertex  *pVertices = 0;
		WORD            *pIndices  = 0;

		(m_pWorldBoxVertices->Lock(0, 24*sizeof(WorldBoxVertex),(void**)&pVertices, 0));
		(m_pWorldBoxIndices->Lock(0, 36*sizeof(WORD),(void**)&pIndices, 0));

		CreateCube(pVertices, pIndices);

		m_pWorldBoxVertices->Unlock();
		m_pWorldBoxIndices->Unlock();
		return 0;
	}

	void Destroy()
	{
		////////////////SAFE_RELEASE(m_pWorldBoxVertices);
		////////////////SAFE_RELEASE(m_pWorldBoxIndices);
		////////////////SAFE_RELEASE(m_CubeMap);
	}

	void SetCubeMap(TextureID cubemap)
	{
		m_CubeMap = cubemap;
	}

	int Render( d3dDevice, D3DXFROMWINEMATRIX &mWorld)
	{
		int     hr = 0;

		D3DXFROMWINEMATRIX oldWorld,oldView,oldProj;

		hr = d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		hr = d3dDevice->SetPixelShader(0);    
		hr = d3dDevice->SetVertexShader(0);

		// note: setting FVF clobbers our current vertex declaration, so be sure 
		// that the caller re-establishes the vertex declaration on return!
		hr = d3dDevice->SetFVF( WORLDBOX_FVF );

		hr = d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		hr = d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

		hr = d3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0);
		hr = d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		hr = d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

		hr = d3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		hr = d3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		hr = d3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
		hr = d3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE | D3DTTFF_COUNT3 );    
	    
		hr = d3dDevice->SetStreamSource(0, m_pWorldBoxVertices, 0, sizeof(WorldBoxVertex));
		hr = d3dDevice->SetIndices(m_pWorldBoxIndices);

		hr = d3dDevice->SetTexture(0, m_CubeMap);
		hr = d3dDevice->SetTexture(1, 0);

		D3DXFROMWINEMATRIX id, matProj;
		D3DXFROMWINEMatrixIdentity(&id);
		D3DXFROMWINEMatrixPerspectiveFovLH(&matProj, D3DXFROMWINEToRadian(90.0f), 1.0f, 0.1f, 4000.0f);
		d3dDevice->SetTransform(D3DTS_WORLD, &mWorld);
		d3dDevice->SetTransform(D3DTS_VIEW, &id);
		d3dDevice->SetTransform(D3DTS_PROJECTION, &matProj); 

		hr = d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

		hr = d3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);    

		return hr;
	}

	int CreateCube(WorldBoxVertex* pVertices, WORD* pIndices)
	{
		// Set up the vertices for the Floor. Note: to prevent tiling problems,
		// the u/v coords are knocked slightly inwards.
		float const kEpsMult = 1.9999f;
		for (int ix = 0; ix < 2; ++ix)
			for (int iy = 0; iy < 2; ++iy)
				for (int iz = 0; iz < 2; ++iz)
				{
					float const x  = 2000.0f*(static_cast<float>(ix) - 0.5f);
					float const y  = 2000.0f*(static_cast<float>(iy) - 0.5f);
					float const z  = 2000.0f*(static_cast<float>(iz) - 0.5f);

					for (int iv = 0; iv < 3; ++iv)
					{
						const float tx = ((iv == 0) ? 2.0f : kEpsMult) * (static_cast<float>(ix) - 0.5f);
						const float ty = ((iv == 1) ? 2.0f : kEpsMult) * (static_cast<float>(iy) - 0.5f);
						const float tz = ((iv == 2) ? 2.0f : kEpsMult) * (static_cast<float>(iz) - 0.5f);

						*pVertices++ = WorldBoxVertex(D3DXFROMWINEVECTOR3(x, y, z), D3DXFROMWINEVECTOR3(tx, ty, tz));
					}
				}
		// Set up the indices for the cube
		// no offset in x-coords
		*pIndices++ =  0;     *pIndices++ =  6;     *pIndices++ =  3;
		*pIndices++ =  3;     *pIndices++ =  6;     *pIndices++ =  9;
		*pIndices++ = 12;     *pIndices++ = 15;     *pIndices++ = 21;
		*pIndices++ = 12;     *pIndices++ = 21;     *pIndices++ = 18;
	    
		// no offset in y-coords
		*pIndices++ =  9+1;   *pIndices++ =  6+1;   *pIndices++ = 21+1;
		*pIndices++ =  6+1;   *pIndices++ = 18+1;   *pIndices++ = 21+1;
		*pIndices++ =  0+1;   *pIndices++ = 15+1;   *pIndices++ = 12+1;
		*pIndices++ =  0+1;   *pIndices++ =  3+1;   *pIndices++ = 15+1;
	    
		// no offset in z-coords
		*pIndices++ =  3+2;   *pIndices++ =  9+2;   *pIndices++ = 15+2;
		*pIndices++ =  9+2;   *pIndices++ = 21+2;   *pIndices++ = 15+2;
		*pIndices++ =  0+2;   *pIndices++ = 18+2;   *pIndices++ =  6+2;
		*pIndices++ =  0+2;   *pIndices++ = 12+2;   *pIndices++ = 18+2;

		return 0;
	}


	bool m_bInit;
    VertexBufferID m_pWorldBoxVertices;
    IndexBufferID  m_pWorldBoxIndices;
	TextureID m_CubeMap;
};
