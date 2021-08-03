//-----------------------------------------------------------------------------
// File: Texture.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "Helper.h"
#include "Texture.h"
#include "DXErrors.h"

CTexture::CTexture()
{
    m_pTextureID = -1;
}

CTexture::~CTexture()
{
}

TextureID CTexture::Create(char *strTexName, bool bMipMap, bool bAlpha)
{
    m_bMipMap = bMipMap;
    m_bHasAlpha = bAlpha;

	std:string fn;
	fn=fn.append(TEXTURE_PATH);
	fn=fn.append(strTexName);
	fn=fn.append(".bmp");

	stx_strlcpy(m_strFileName, fn.c_str(), MAX_PATH);

	m_pTextureID=IRenderer::GetRendererInstance()->addImageLibTexture(
		m_strFileName
		,false,
		//IRenderer::GetRendererInstance()->GetbilinearClamp());
		IRenderer::GetRendererInstance()->Getlinear());

	//LOG_PRINT("CTexture::Create:%s:texid=%d\n",m_strFileName,m_pTextureID);

    return m_pTextureID;
}

int CTexture::Restore()
{
    int hr;
//  __DWORD__ dwFlags;
    /*D3DXFROMWINE_SURFACEFORMAT sf;

    if (m_bMipMap)
        dwFlags = 0;
    else
        dwFlags = D3DXFROMWINE_TEXTURE_NOMIPMAP;

    sf = D3DXFROMWINE_SF_A8R8G8B8;

    hr = D3DXFROMWINECreateTextureFromFile(GameState::m_pd3dDevice,
        &dwFlags,
        0,
        0,
        &sf,
        0,
        &m_pTextureID,
        0,
        m_strFileName,
        D3DXFROMWINE_FT_LINEAR);

	//hr=D3DXFROMWINECreateTextureFromFile( GameState::m_pd3dDevice, m_strFileName, &m_pTextureID );
	m_pTextureID=IRenderer::GetRendererInstance()->addImageLibTexture(
		m_strFileName
		,false,
		//IRenderer::GetRendererInstance()->GetbilinearClamp());
		IRenderer::GetRendererInstance()->Getlinear());

    if (FAILED(hr))
    {
        DD_ERR(hr);
        return 0;
    }
*/
    return 1;
}

void CTexture::Invalidate()
{
    //SAFE_RELEASE(m_pTextureID);
}

TextureID CTexture::GetSurface()
{
    return m_pTextureID;
}
