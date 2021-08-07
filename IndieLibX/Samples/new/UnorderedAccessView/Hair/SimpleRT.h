/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
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

#pragma once

class SimpleRT 
{
public:
	TextureID  pTexture;
	TextureID  pRTV;
	TextureID  pSRV;

	SimpleRT(unsigned int aWidth, unsigned int aHeight )
	{
		//These have to be set to have a render target
		Image3 img;
		img.create(FORMAT_D32F, aWidth, aHeight, 1, 1);
		pTexture=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	}

	~SimpleRT()
	{
		//////////////////SAFE_RELEASE( pTexture );
	}

	operator TextureID ()
	{
		return pTexture;
	}

	operator TextureID ()
	{
		return pSRV;
	}

	operator TextureID ()
	{
		return pRTV;
	}
};

class SimpleArrayRT : public SimpleRT
{
public:
	TextureID   ppRTVs;
	unsigned int iArraySize;

	SimpleArrayRT(unsigned int aWidth, unsigned int aHeight )
		: SimpleRT( IRenderer::GetRendererInstance(), pTexDesc )
	{
		int hr;

		SURFACE_DESC layerViewDesc;
		pRTV->GetDesc( &layerViewDesc );

		iArraySize = pTexDesc->ArraySize;
		ppRTVs = new TextureID [iArraySize];
		switch (layerViewDesc.ViewDimension) {
			case RTV_DIMENSION_TEXTURE2DARRAY:
				layerViewDesc.Texture2DArray.ArraySize = 1;
				for( unsigned int i = 0; i < iArraySize; ++i )
				{
					layerViewDesc.Texture2DArray.FirstArraySlice = i;
					( IRenderer::GetRendererInstance()->CreateRenderTargetView( pTexture, &layerViewDesc, &ppRTVs[i] ) );
				}
				break;
			case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
				layerViewDesc.Texture2DMSArray.ArraySize = 1;
				for( unsigned int i = 0; i < iArraySize; ++i )
				{
					layerViewDesc.Texture2DMSArray.FirstArraySlice = i;
					( IRenderer::GetRendererInstance()->CreateRenderTargetView( pTexture, &layerViewDesc, &ppRTVs[i] ) );
				}
				break;
		}
	}

	~SimpleArrayRT()
	{
		for (unsigned int i = 0; i < iArraySize; i++) {
			//////////////////SAFE_RELEASE( ppRTVs[i] );
		}
		SAFE_DELETE(ppRTVs);
	}
};

class DepthRT 
{
public:
	TextureID  pTexture;

	DepthRT( unsigned int aWidth, unsigned int aHeight )
	{
		Image3 img;
		img.create(FORMAT_D32F, aWidth, aHeight, 1, 1);
		pTexture=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear()); 
	}

	~DepthRT()
	{
		//////////////////SAFE_RELEASE( pTexture );
		//////////////////SAFE_RELEASE( pDSV );
		//////////////////SAFE_RELEASE( pSRV );
	}

	operator TextureID ()
	{
		return pTexture;
	}

	operator TextureID ()
	{
		return pDSV;
	}

	operator TextureID ()
	{
		return pSRV;
	}
};
