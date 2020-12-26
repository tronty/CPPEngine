#include "Bullet.h"

const float RADIUS_SCALE_FACTOR	= 0.65f;

Bullet::Bullet(IND_Surface* pSurface, D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 vel, float angle)
{
	

	CIndieLib::Instance()->Entity2dManager->Add(&entity);
	// Must set surface before setting other properties.  Weird indielib bug!
	entity.SetSurface(pSurface);
	entity.SetPosition(pos.x, pos.y, 0);
	entity.SetHotSpot(0.5f, 0.5f);
	entity.SetAngleXYZ(0, 0, angle);
	entity.SetBoundingCircle("", pSurface->GetWidth() / 2, pSurface->GetHeight()/2,pSurface->GetWidth()/2 * RADIUS_SCALE_FACTOR);

	vecVel = vel;
	vecPos = pos;
	bIsDead = false;
}

Bullet::~Bullet( )
{
	#ifndef OMITDELETE
	CIndieLib::Instance()->Entity2dManager->Delete(&entity);
#endif
}

void Bullet::Update( float fElapsedTime )
{
	 // Move the bullet	
	vecPos += vecVel * fElapsedTime;
	entity.SetPosition(vecPos.x, vecPos.y, 0);

}
