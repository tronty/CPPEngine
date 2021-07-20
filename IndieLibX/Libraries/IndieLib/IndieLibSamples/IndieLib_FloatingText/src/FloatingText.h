#ifndef __FLOATING_TEXT_H__
#define __FLOATING_TEXT_H__

#include <CIndieLib.h>
#include <string>

struct FontDesc
{


};

class FloatingText
{
public:
			 FloatingText( );
	virtual ~FloatingText( );

	bool		Init	(string Text, int layer, float FadeDelay, D3DXFROMWINEVECTOR3 vecPosition, D3DXFROMWINEVECTOR3 vecVelocity);
	bool		Init	(string Text, IND_Font* pFont, int layer, float FadeDelay, D3DXFROMWINEVECTOR3 vecPosition, D3DXFROMWINEVECTOR3 vecVelocity);
	void		Update	(float fElapsedTime);
	bool		IsDead	( )		{ return m_bIsDead; }

private:
	string			m_szText;
	IND_Font*		m_pFont;
	IND_Entity2d	m_Entity2d;

	D3DXFROMWINEVECTOR3		m_vecPosition;
	D3DXFROMWINEVECTOR3		m_vecVelocity;
	float			m_fFadeTimer;
	float			m_fFadeDelay;
	float			m_fLifetime;
	float			m_fAlpha;
	bool			m_bIsDead;
};
#endif //__FLOATING_TEXT_H__
