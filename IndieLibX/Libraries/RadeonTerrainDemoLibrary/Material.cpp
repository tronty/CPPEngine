

//-----------------------------------------------------------------------------
// File: Material.cpp
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include "Helper.h"
#include "Material.h"
#include "DXErrors.h"

CMaterial::CMaterial()
{
    m_dwTexIndx = -1;
	m_pTextureID = -1;
    ZeroMemory(&m_Mtrl, sizeof(D3DFROMWINEMATERIAL9));
}

CMaterial::~CMaterial()
{
    DeleteTexture();
}

int CMaterial::LoadMaterial(FILE *fp)
{
    
	int hr;
    BOOL mipmap, alpha, hasTexture;
    CHAR tname[32];
    CTexture *pTexture;

    
	fread(&m_Mtrl, sizeof(D3DFROMWINEMATERIAL9), 1, fp);
    
	fread(&hasTexture, sizeof(BOOL), 1, fp);
    
	fread(&mipmap, sizeof(BOOL), 1, fp);
    
	fread(&alpha, sizeof(BOOL), 1, fp);
    
	fread(tname, sizeof(CHAR), 32, fp);

    
	DeleteTexture();

    if (hasTexture)
    {
        
	pTexture = new CTexture();

	
	////LOG_PRINT("tname = %s\n", tname);
        
	m_pTextureID=pTexture->Create(tname, mipmap, alpha);
        
	if (m_pTextureID == -1)
        {
            SAFE_DELETE(pTexture);
            return 0;
        }
        
	m_dwTexIndx = GameState::m_TMan->AddTexture(pTexture);
       
	 if (m_dwTexIndx == -1)
        {
            SAFE_DELETE(pTexture);
            return 0;
        }
	
	
    }

    
	return m_pTextureID;
}

TextureID CMaterial::LoadMaterial(D3DFROMWINEMATERIAL9 &mtrl, char *tname, bool mipmap, bool alpha)
{
    ////LOG_PRINT("tname = %s\n", tname);
    int hr;
    CTexture *pTexture;

    m_Mtrl = mtrl;

    DeleteTexture();

    if (tname != 0)
    {
        pTexture = new CTexture();
        m_pTextureID=pTexture->Create(tname, mipmap, alpha);
        
        m_dwTexIndx = GameState::m_TMan->AddTexture(pTexture);
	////LOG_PRINT("m_dwTexIndx = %d\n", m_dwTexIndx);
        if (m_dwTexIndx == -1)
        {
            SAFE_DELETE(pTexture);
		if (m_pTextureID != -1)
            		return m_pTextureID;
		else
			return -1;
        }
    }

    return m_pTextureID;
}

TextureID CMaterial::SetMaterial()
{
#if 1
	//LOG_PRINT("CMaterial::SetMaterial()\n");	
    //LOG_PRINT("m_pTextureID=%x\n", m_pTextureID);
	
	if(m_pTextureID != -1)
	{
		IRenderer::GetRendererInstance()->setTexture("Sampler", m_pTextureID);
		return m_pTextureID;
	}
	return -1;
#else
  int hr=-1;

//    IRenderer::GetRendererInstance()->SetMaterial(&m_Mtrl);
	
	//LOG_PRINT("CMaterial::SetMaterial()\n");	
    //LOG_PRINT("m_dwTexIndx=%x\n", m_dwTexIndx);	
	if (m_dwTexIndx != -1)
    {
        CTexture *pTexture = GameState::m_TMan->GetTexture(m_dwTexIndx);
	//LOG_PRINT("pTexture=%x\n", pTexture);	
        if (pTexture != 0)
	{	
		hr=pTexture->GetSurface();
		//LOG_PRINT("CMaterial::SetMaterial:setTexture:id=%d\n", hr);
		
            IRenderer::GetRendererInstance()->setTexture("Sampler", hr);
        }
	//else
        //    IRenderer::GetRendererInstance()->setTexture(0, 0);
    }
    //else
      //  IRenderer::GetRendererInstance()->SetTexture(0, 0);
	return m_pTextureID;
#endif
}

void CMaterial::DeleteTexture()
{
    if (m_dwTexIndx != -1)
    {
        GameState::m_TMan->DeleteTexture(m_dwTexIndx);
        m_dwTexIndx = -1;
    }
}

