#pragma once
#include <CIndieLib.h>

class Explosion
{
public:
			 Explosion(char* szXmlAnimationFilename, int layer, D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 vel);
	virtual ~Explosion( );

	virtual void	Update( float fElapsedTime );

	IND_Entity2d	entity;
	/*static*/ IND_Animation	AnimExplosion;
	/*static*/ //unsigned int	m_nAnimExplosion;
	D3DXFROMWINEVECTOR3		vecPos;
	D3DXFROMWINEVECTOR3		vecVel;
	bool			bIsDead;
	int				numFrames;

	
};
