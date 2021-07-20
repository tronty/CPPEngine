#pragma once
#include <CIndieLib.h>

class Bullet
{
public:
			 Bullet(IND_Surface* pSurface, D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 vel, float angle = 0.0f);
	virtual ~Bullet( );

	virtual void	Update( float fElapsedTime );


	IND_Entity2d	entity;
	D3DXFROMWINEVECTOR3		vecPos;
	D3DXFROMWINEVECTOR3		vecVel;
	bool			bIsDead;

	
};
