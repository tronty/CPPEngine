#include "PropFighter.h"

const float RADIUS_SCALE_FACTOR	= 0.65f;

PropFighter::PropFighter(IND_Animation* pAnim, D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 vel)
{

	CIndieLib::Instance()->Entity2dManager->Add(&entity);

	entity.SetAnimation(pAnim);

	entity.SetHotSpot(0.5f, 0.5f);

	entity.SetPosition(pos.x, pos.y, 0);

	vecVel = vel;
	vecPos = pos;
	bIsDead = false;
	health = 1.0f;

}

PropFighter::~PropFighter( )
{
	#ifndef OMITDELETE
	CIndieLib::Instance()->Entity2dManager->Delete(&entity);
#endif
}

void PropFighter::Update( float fElapsedTime )
{
	 // Move the PropFighter	
	vecPos += vecVel * fElapsedTime;
	entity.SetPosition(vecPos.x, vecPos.y, 0);

}
