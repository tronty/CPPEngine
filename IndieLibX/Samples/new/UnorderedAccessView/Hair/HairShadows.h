/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#include "SimpleRT.h"

#define LIGHT_RES 1024

extern stx_Effect*  g_Effect;

class HairShadows
{
	D3DFROMWINEVIEWPORT9			m_LViewport;
	stx_CModelViewerCamera		m_LCamera;
	SimpleRT*				m_pDepthsRT;
	DepthRT*				m_pDepthsDS;

	//extra variables for DOM
	SimpleRT*          m_DOM;
	SimpleRT*		   m_pDepthsRT_DOM;
	//TEMP SARAH! SimpleArrayRT*          m_DOM; //if you need more than one RT
	float m_Zi[ARRAY_SIZE*4+1];
	static int m_NumLayers;
	bool m_useDOM;

	D3DXFROMWINEMATRIX mWorldI, mWorldIT;
	D3DXFROMWINEMATRIX mWorldView, mWorldViewI, mWorldViewIT;
	D3DXFROMWINEMATRIX mWorldViewProj, mWorldViewProjI;
	D3DXFROMWINEVECTOR3 vLightDirWorld, vLightCenterWorld;

public:
	HairShadows() {
		m_LViewport.Width  = LIGHT_RES;
		m_LViewport.Height = LIGHT_RES;
		m_LViewport.MinDepth = 0;
		m_LViewport.MaxDepth = 1;
		m_LViewport.TopLeftX = 0;
		m_LViewport.TopLeftY = 0;

		m_pDepthsRT = 0;
		m_pDepthsDS = 0;
		m_pDepthsRT_DOM = 0;

		m_useDOM = false;
	}

	void OnCreateDevice() {


		SURFACE_DESC texDesc;
		texDesc.Width = LIGHT_RES;
		texDesc.Height = LIGHT_RES;
		texDesc.ArraySize = 1;
		texDesc.MiscFlags = 0;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Format = FORMAT_R32_FLOAT;
		m_pDepthsRT = new SimpleRT( IRenderer::GetRendererInstance(), &texDesc );

		texDesc.ArraySize          = 1;
		texDesc.BindFlags          = BIND_DEPTH_STENCIL | BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags     = 0;
		texDesc.Format             = FORMAT_R32_TYPELESS;
		texDesc.Width              = LIGHT_RES;
		texDesc.Height             = LIGHT_RES;
		texDesc.MipLevels          = 1;
		texDesc.MiscFlags          = 0;
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage              = USAGE_DEFAULT;
		m_pDepthsDS = new DepthRT( &texDesc );

		//create the resources for Opacity Shadow Maps
		SURFACE_DESC texDesc2;
		texDesc2.Width              = LIGHT_RES;
		texDesc2.Height             = LIGHT_RES;
		if(m_NumLayers==1)
		texDesc2.ArraySize          = 1;
		else
		texDesc2.ArraySize          = ARRAY_SIZE;
		texDesc2.MiscFlags          = 0;
		texDesc2.MipLevels          = 1;
		texDesc2.SampleDesc.Count   = 1;
		texDesc2.SampleDesc.Quality = 0;
		texDesc2.Format = FORMAT_RGBA16F;
		if(m_NumLayers==1)
			m_DOM = new SimpleRT( IRenderer::GetRendererInstance(), &texDesc2 );
		else
			m_DOM = new SimpleArrayRT( IRenderer::GetRendererInstance(), &texDesc2 );


		texDesc.Width = LIGHT_RES;
		texDesc.Height = LIGHT_RES;
		texDesc.ArraySize = 1;
		texDesc.MiscFlags = 0;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Format = FORMAT_R32_FLOAT;
		m_pDepthsRT_DOM = new SimpleRT( IRenderer::GetRendererInstance(), &texDesc );

    }

	void OnDestroyDevice() {
		SAFE_DELETE(m_pDepthsRT);
		SAFE_DELETE(m_pDepthsDS);
		SAFE_DELETE(m_DOM);
		SAFE_DELETE(m_pDepthsRT_DOM);
	}

	stx_CModelViewerCamera &GetCamera() {
		return m_LCamera;
	}

	D3DXFROMWINEMATRIX &GetLightWorldViewProj() {
		return mWorldViewProj;
	}

	D3DXFROMWINEVECTOR3 &GetLightWorldDir() {
		return vLightDirWorld;
	}
	
	D3DXFROMWINEVECTOR3 &GetLightCenterWorld() {
		return vLightCenterWorld;
	}
	
	void UpdateMatrices(D3DXFROMWINEVECTOR3 bbCenter, D3DXFROMWINEVECTOR3 bbExtents) {
		int hr;

		D3DXFROMWINEMATRIX mTrans, mWorld;
		D3DXFROMWINEMatrixTranslation( &mTrans, -bbCenter.x, -bbCenter.y, -bbCenter.z );
		D3DXFROMWINEMatrixMultiply( &mWorld, &mTrans, m_LCamera.GetWorldMatrix());

		D3DXFROMWINEMATRIX mView = *m_LCamera.GetViewMatrix();
		D3DXFROMWINEMatrixInverse(&mWorldI, 0, &mWorld);
		D3DXFROMWINEMatrixTranspose(&mWorldIT, &mWorldI);
		D3DXFROMWINEMatrixMultiply( &mWorldView, &mWorld, &mView );
		D3DXFROMWINEMatrixInverse(&mWorldViewI, 0, &mWorldView);
		D3DXFROMWINEMatrixTranspose(&mWorldViewIT, &mWorldViewI);

		D3DXFROMWINEVECTOR3 vBox[2];
		vBox[0] = bbCenter - bbExtents;
		vBox[1] = bbCenter + bbExtents;

		D3DXFROMWINEVECTOR3 BBox[2];
		BBox[0][0] = BBox[0][1] = BBox[0][2] =  FLT_MAX;
		BBox[1][0] = BBox[1][1] = BBox[1][2] = -FLT_MAX;
		for (int i = 0; i < 8; ++i)
		{
			D3DXFROMWINEVECTOR3 v, v1;
			v[0] = vBox[(i & 1) ? 0 : 1][0];
			v[1] = vBox[(i & 2) ? 0 : 1][1];
			v[2] = vBox[(i & 4) ? 0 : 1][2];
			D3DXFROMWINEVec3TransformCoord(&v1, &v, &mWorldView);
			for (int j = 0; j < 3; ++j)
			{
				BBox[0][j] = min(BBox[0][j], v1[j]);
				BBox[1][j] = max(BBox[1][j], v1[j]);
			}
		}

		float ZNear = BBox[0][2];
		float ZFar =  BBox[1][2];
		(g_Effect->GetVariableByName("ZNear")->AsScalar()->SetFloat(ZNear));
		(g_Effect->GetVariableByName("ZFar")->AsScalar()->SetFloat(ZFar));

		int NumSlices = m_NumLayers * 4 - 1;
		float Zn = 0;

		/*
		//constant spacing
		float DzInc = 6; //this value has to be exposed through a slider to the user
		float dz[33];
		for (int i = 0; i < NumSlices+1; i++) {
			dz[i] = DzInc;
			m_Zi[i] = Zn + dz[i] * i;
		}*/

		// linear spacing
		float dzInc = 3;
		float dz[33];
		for (int i = 0; i < NumSlices+1; i++) {
			dz[i] = dzInc*(i+1);
			if(i==0) m_Zi[0] = Zn;
			else m_Zi[i] = m_Zi[i-1] + dz[i-1];
		}


		//(g_Effect->GetVariableByName("g_DZ")->AsScalar()->SetFloat(DZ));
		(g_Effect->GetVariableByName("g_Dz")->AsVector()->SetFloatVectorArray(&dz[0],0,ARRAY_SIZE));
		(g_Effect->GetVariableByName("g_Zi")->AsVector()->SetFloatVectorArray(&m_Zi[1],0,ARRAY_SIZE));
		//(g_Effect->GetVariableByName("NumLayers")->AsScalar()->SetInt(m_NumLayers));


		float w = (FLOAT)max(abs(BBox[0][0]), (FLOAT)abs(BBox[1][0])) * 2;
		float h = (FLOAT)max(abs(BBox[0][1]), (FLOAT)abs(BBox[1][1])) * 2;
		D3DXFROMWINEMATRIX mProj;
		D3DXFROMWINEMatrixOrthoLH(&mProj, w, h, ZNear, ZFar);
		//D3DXFROMWINEMatrixPerspectiveLH(&mProj, w, h, ZNear, ZFar);

		D3DXFROMWINEMatrixMultiply(&mWorldViewProj, &mWorldView, &mProj);
		D3DXFROMWINEMatrixInverse(&mWorldViewProjI, 0, &mWorldViewProj);
		(g_Effect->GetVariableByName("mLightView")->AsMatrix()->SetMatrix(mWorldView));
		(g_Effect->GetVariableByName("mLightProj")->AsMatrix()->SetMatrix(mProj));
		(g_Effect->GetVariableByName("mLightViewProj")->AsMatrix()->SetMatrix(mWorldViewProj));
		(g_Effect->GetVariableByName("mLightViewProjI")->AsMatrix()->SetMatrix(mWorldViewProjI));

		D3DXFROMWINEMATRIX mClip2Tex = D3DXFROMWINEMATRIX(
			0.5,    0,    0,   0,
			0,	   -0.5,  0,   0,
			0,     0,     1,   0,
			0.5,   0.5,   0,   1 );
		D3DXFROMWINEMATRIX mLightViewProjClip2Tex;
		D3DXFROMWINEMatrixMultiply(&mLightViewProjClip2Tex, &mWorldViewProj, &mClip2Tex);
		(g_Effect->GetVariableByName("mLightViewProjClip2Tex")->AsMatrix()->SetMatrix(mLightViewProjClip2Tex));

		D3DXFROMWINEVECTOR3 vLightClip = D3DXFROMWINEVECTOR3(0, 0, 1);
		D3DXFROMWINEVec3TransformCoord(&vLightDirWorld, &vLightClip, &mWorldViewProjI);

		vLightClip = D3DXFROMWINEVECTOR3(0, 0, 0.5);
		D3DXFROMWINEVec3TransformCoord(&vLightCenterWorld, &vLightClip, &mWorldViewProjI);

        vLightDirWorld -= vLightCenterWorld;
		D3DXFROMWINEVec3Normalize(&vLightDirWorld,&vLightDirWorld);        
		(g_Effect->GetVariableByName("vLightDir")->AsVector()->SetFloatVector(vLightDirWorld));
	}

	void BeginShadowMapRendering(bool DOMPrepass=false)
	{
        float ClearColor[4] = { FLT_MAX };
		if(!DOMPrepass)
		IRenderer::GetRendererInstance()->ClearRenderTargetView( *m_pDepthsRT, ClearColor );
		else
		IRenderer::GetRendererInstance()->ClearRenderTargetView( *m_pDepthsRT_DOM, ClearColor );
			
		IRenderer::GetRendererInstance()->ClearDepthStencilView( *m_pDepthsDS, D3D11_CLEAR_DEPTH, 1.0, 0 );

		g_Effect->GetVariableByName("tShadowMap")->AsShaderResource()->SetResource( 0 );

		TextureID  pRTVs[1];
		if(!DOMPrepass) pRTVs[0] = *m_pDepthsRT;
		else pRTVs[0] = *m_pDepthsRT_DOM;
		IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, pRTVs, *m_pDepthsDS );
		IRenderer::GetRendererInstance()->RSSetViewports(1, &m_LViewport);
	}
	void EndShadowMapRendering(bool DOMPrepass=false)
	{
		IRenderer::GetRendererInstance()->OMSetRenderTargets(0, 0, 0);
	}

	void setupDOMShadowMapRendering()
	{
		IRenderer::GetRendererInstance()->RSSetViewports(1, &m_LViewport);

		float Zero[4] = { 0.0f, 0.0f, 0.0f, FLT_MAX};
		IRenderer::GetRendererInstance()->ClearRenderTargetView( *m_DOM, Zero );

	    g_Effect->GetVariableByName("tShadowMap")->AsShaderResource()->SetResource( 0 );
		g_Effect->GetVariableByName("tShadowMapArray")->AsShaderResource()->SetResource( 0 ); 

		g_Effect->GetVariableByName("tHairDepthMap")->AsShaderResource()->SetResource( *m_pDepthsRT_DOM );
	}

	void setShadowMapRT()
	{
		assert(m_NumLayers == 1);
		IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_DOM->pRTV, 0 );
		IRenderer::GetRendererInstance()->RSSetViewports(1, &m_LViewport);
	}

	void endDOMShadowMapRendering()
	{
	    g_Effect->GetVariableByName("tHairDepthMap")->AsShaderResource()->SetResource( 0 );
	}

	void renderDOMPass(int pass)
	{
		g_Effect->GetVariableByName("g_Zmin")->AsVector()->SetFloatVectorArray(&m_Zi[pass*4*NUM_MRTS],0,NUM_MRTS);
		g_Effect->GetVariableByName("g_Zmax")->AsVector()->SetFloatVectorArray(&m_Zi[pass*4*NUM_MRTS+1],0,NUM_MRTS);

		if(m_NumLayers == 1)
			IRenderer::GetRendererInstance()->OMSetRenderTargets( 1, &m_DOM->pRTV, 0 );
		else
			IRenderer::GetRendererInstance()->OMSetRenderTargets( NUM_MRTS, &(static_cast<SimpleArrayRT*>(m_DOM))->ppRTVs[pass*NUM_MRTS], 0 );

	}
	int getNumDOMPasses()
	{
		return ceil((float)m_NumLayers / NUM_MRTS);
	}

	void SetHairShadowTexture()
	{
        int hr;

		if(m_useDOM)
		{ 
			if(m_NumLayers == 1)
			{	
				(g_Effect->GetVariableByName("tShadowMap")->AsShaderResource()->SetResource( m_DOM->pSRV ));
			}
			else
			{	
				(g_Effect->GetVariableByName("tShadowMapArray")->AsShaderResource()->SetResource( m_DOM->pSRV ));
			} 

			(g_Effect->GetVariableByName("tHairDepthMap")->AsShaderResource()->SetResource( *m_pDepthsRT_DOM ));
		}
		else
		{ (g_Effect->GetVariableByName("tShadowMap")->AsShaderResource()->SetResource( *m_pDepthsRT )); }	
	}

	void setHairDepthMap()
	{
		int hr;
		(g_Effect->GetVariableByName("tHairDepthMap")->AsShaderResource()->SetResource( *m_pDepthsRT_DOM ));	
	}

	void unsetHairDepthMap()
	{
		int hr;
		(g_Effect->GetVariableByName("tHairDepthMap")->AsShaderResource()->SetResource( 0 ));	
	}


	void UnSetHairShadowTexture()
	{
        int hr;
        (g_Effect->GetVariableByName("tShadowMap")->AsShaderResource()->SetResource( 0 ));
		if(m_useDOM)
		{
			(g_Effect->GetVariableByName("tHairDepthMap")->AsShaderResource()->SetResource( 0 ));
		}
		if(m_useDOM && m_NumLayers > 1)
		{
			(g_Effect->GetVariableByName("tShadowMapArray")->AsShaderResource()->SetResource( 0 ));
		}
	}

	void VisualizeShadowMap()
	{
		SetHairShadowTexture();
		IRenderer::GetRendererInstance()->setVertexFormat(0,  0 );
		IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_TRIANGLE_STRIP );
        g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("VisualizeShadowMap")->Apply(0);
        IRenderer::GetRendererInstance()->Draw( 3, 0 );
		UnSetHairShadowTexture();
        g_Effect->GetTechniqueByName("RenderHair")->GetPassByName("VisualizeShadowMap")->Apply(0);
	}
};

int HairShadows::m_NumLayers = 1;
//m_NumLayers = NUM_MRTS;
