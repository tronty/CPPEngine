#include "FloatingText.h"

FloatingText::FloatingText()
{
	m_fAlpha		= 0.0f;
	m_fFadeTimer	= 0.0f;
	m_fFadeDelay	= 0.0f;
	m_fLifetime		= 0.0f;
	m_bIsDead		= false;
	m_pFont			= 0;
}

FloatingText::~FloatingText()
{
	if (m_pFont)
		CIndieLib::Instance()->FontManager->Delete(m_pFont);
	#ifndef OMITDELETE
	CIndieLib::Instance()->Entity2dManager->Delete(&m_Entity2d);
#endif
}

bool FloatingText::Init(string Text, int layer, float FadeDelay, D3DXFROMWINEVECTOR3 vecPosition, D3DXFROMWINEVECTOR3 vecVelocity)
{
	m_fFadeDelay	= FadeDelay;
	m_vecPosition	= vecPosition;
	m_vecVelocity	= vecVelocity;

	// ----- Font creation -----
	m_pFont = new IND_Font;	// we supply our own
	if (!CIndieLib::Instance()->FontManager->Add (m_pFont, "Army_05.png", "Army_05.xml", IND_ALPHA, IND_32))
		return false;

	CIndieLib::Instance()->Entity2dManager->Add (layer, &m_Entity2d);		// Entity adding in layer 
	m_Entity2d.SetFont (m_pFont);								// Set the font into the entity

	m_Entity2d.SetShow(true);

	// TODO - could add a small structure with more parameters such as the font.
	//			could pass in a pointer to a IND_Font as well.
	m_Entity2d.SetLineSpacing	(0);
	m_Entity2d.SetCharSpacing	(0);
	m_Entity2d.SetAlign			(IND_CENTER);
	m_Entity2d.SetTransparency(255);
	m_Entity2d.SetHotSpot(0.5f, 0.5f);

	m_szText = Text;	// copy the string
	m_Entity2d.SetText			((char*)m_szText.c_str());

	return true;
}


bool FloatingText::Init(string Text, IND_Font* pFont, int layer, float FadeDelay, D3DXFROMWINEVECTOR3 vecPosition, D3DXFROMWINEVECTOR3 vecVelocity)
{
	m_fFadeDelay	= FadeDelay;
	m_vecPosition	= vecPosition;
	m_vecVelocity	= vecVelocity;

	CIndieLib::Instance()->Entity2dManager->Add (layer, &m_Entity2d);		// Entity adding in layer 
	m_Entity2d.SetFont (pFont);								// Set the font into the entity
	m_Entity2d.SetTransparency(255);
	m_Entity2d.SetShow(true);

	// TODO - could add a small structure with more parameters such as the font.
	//			could pass in a pointer to a IND_Font as well.
	m_Entity2d.SetLineSpacing	(0);
	m_Entity2d.SetCharSpacing	(0);
	m_Entity2d.SetAlign			(IND_CENTER);

	m_szText = Text;	// copy the string
	m_Entity2d.SetText			((char*)m_szText.c_str());

	return true;

}


void FloatingText::Update(float fElapsedTime)
{
	if (!m_bIsDead )
	{
		m_fFadeTimer += fElapsedTime;

		// nAlpha goes from 255 to 0 in m_fFadeDelay seconds
		m_fAlpha =  (int)(255 * ((m_fFadeDelay - m_fFadeTimer) / m_fFadeDelay));
		m_Entity2d.SetTransparency((unsigned char)m_fAlpha);
		//LOG_PRINT("m_fFadeDelay %f\n", m_fFadeDelay);
		//LOG_PRINT("m_fAlpha %f\n", m_fAlpha);
	}

	m_vecPosition += m_vecVelocity * fElapsedTime;
	m_Entity2d.SetPosition(m_vecPosition.x, m_vecPosition.y, (int)m_vecPosition.z);

	// still alive?
	if (m_fAlpha <= 0.0f)
		m_bIsDead = true;	
}
