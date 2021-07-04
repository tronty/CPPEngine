#if 0
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
// File: nv_D3D9FullScreenQuad.h
// Desc: a helper class for rendering full-screen efects
//-----------------------------------------------------------------------------
#pragma once
#include "assert.h"

class nv_D3D9FullScreenQuad
{
	private: /*
		struct SCREEN_VERTEX 
		{
			D3DXFROMWINEVECTOR3 pos;
			D3DXFROMWINEVECTOR3 tex1;
		}; */
		ShaderID shd;
		VertexFormatID  vf;

		//SCREEN_VERTEX m_QuadVertices[4];

		VertexBufferID m_pVB;

		//	creates the fullscreenquad with texture coordinates set up
		//	for mapping texels to pixels on a given render target.
		//
		//	renderTargetSurface			- the target surface that you will be rendering this
		//									quad to
		void SetUpverts(  SurfaceID renderTargetSurface)
		{
			#if 0
			D3DSURFACE_DESC desc;
			renderTargetSurface->GetDesc(&desc);
			
			FLOAT right = (FLOAT)desc.Width;//-1.5f;
			FLOAT top = (FLOAT)desc.Height;// +0.5f;
			FLOAT bottom = 0;//0.5f;
			FLOAT left = 0;//0.5f;
			FLOAT depth = 1.0f;
		   
			// Fill in the vertex values
			m_QuadVertices[0].pos = D3DXFROMWINEVECTOR3(right, bottom, depth);
			m_QuadVertices[0].tex1 = D3DXFROMWINEVECTOR3(1.0f, 0.0f, 1.0);
			
			m_QuadVertices[1].pos = D3DXFROMWINEVECTOR3(right, top, depth);
			m_QuadVertices[1].tex1 = D3DXFROMWINEVECTOR3(1.0f, 1.0, 1.0);
			
			m_QuadVertices[2].pos = D3DXFROMWINEVECTOR3(left, bottom, depth);
			m_QuadVertices[2].tex1 = D3DXFROMWINEVECTOR3(0.0f, 0.0f, 1.0);
			
			m_QuadVertices[3].pos = D3DXFROMWINEVECTOR3(left, top, depth);
			m_QuadVertices[3].tex1 = D3DXFROMWINEVECTOR3(0.0f, 1.0, 1.0);

			for( int i = 0 ; i < 4 ; ++i)
			{
				m_QuadVertices[i].pos.x /= right;
				m_QuadVertices[i].pos.y /= top;
				m_QuadVertices[i].pos.y = 1.0f - m_QuadVertices[i].pos.y;
				m_QuadVertices[i].pos.x -= 0.5f;
				m_QuadVertices[i].pos.y -= 0.5f;
				m_QuadVertices[i].pos.x *= 2.0f;
				m_QuadVertices[i].pos.y *= 2.0f;
			}
			#endif
		}

		
	public:
		nv_D3D9FullScreenQuad()
		{
			shd = IRenderer::GetRendererInstance()->addShaderFromFile("/SimpleTexture/SimpleTexture.shd", "mainVS", "mainPS");
			FormatDesc format[] =
			{
				0, TYPE_VERTEX,   FORMAT_FLOAT, 2,
				0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
			};
			vf = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), shd);
			m_pVB = 0;
		};
		// Rendering
		int Render()
		{
			IRenderer::GetRendererInstance()->setShader(shd);
			IRenderer::GetRendererInstance()->setVertexFormat(vf);
			IRenderer::GetRendererInstance()->setTexture("Base", texture);
			D3DXFROMWINEVECTOR4 color(0.6f, 0.6f, 0.6f, 1.0f);
			IRenderer::GetRendererInstance()->setShaderConstant4f("color", color);
			D3DXFROMWINEMATRIX I;
			D3DXFROMWINEMatrixIdentity(&I);
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", I);
			IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
			//		   x	 y     tx    ty
			float v[16] =  {  1.0f,-1.0f, 1.0f, 1.0f,
			 		  1.0f, 1.0f, 1.0f, 0.0f,
			 		 -1.0f,-1.0f, 0.0f, 1.0f,
			 		 -1.0f, 1.0f, 0.0f, 0.0f };
			#if 1
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLE_STRIP, 2, &v[0], &v[0], 4*sizeof(float));
			#else
			__WORD__ i[6] ={0,1,2,2,1,3};
			IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, 4, 2, &i[0], &i[0], CONSTANT_INDEX2, &v[0], &v[0], 4*sizeof(float));
			#endif
		};

		int SetUpForRenderTargetSurface( SurfaceID renderTargetSurface)
		{
			#if 0
			assert(m_pVB && "you must have called RestoreDeviceObjects before calling this");

			//resetup the verts to handle changes in dimensions
			SetUpverts( renderTargetSurface);


			SCREEN_VERTEX* pVerts;
			int hr = m_pVB->Lock( 0 , 0 , (void**)(&pVerts) , 0 );
			if(FAILED(hr))
				return hr;
			
			for(unsigned int i = 0 ; i < 4 ; ++i)
			{
				pVerts[i] = m_QuadVertices[i];
			}

			hr = m_pVB->Unlock();
			#endif
			return 0;
		};
		
		virtual ~nv_D3D9FullScreenQuad(){};
};
#endif

