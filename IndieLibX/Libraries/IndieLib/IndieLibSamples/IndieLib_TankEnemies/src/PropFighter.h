#pragma once
#include <CIndieLib.h>

class PropFighter
{
public:
	PropFighter(IND_Animation* pAnim, D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 vel);
	virtual ~PropFighter( );

	virtual void	Update( float fElapsedTime );

	float			health;

	IND_Entity2d	entity;
	D3DXFROMWINEVECTOR3		vecPos;
	D3DXFROMWINEVECTOR3		vecVel;
	bool			bIsDead;

	
};
