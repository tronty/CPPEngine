#include "PropFighter.h"
#include "explosion.h"
//include "AudioManager.h"

const float RADIUS_SCALE_FACTOR	= 0.65f;

PropFighter::PropFighter(IND_Animation* pAnim, int layer, D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 vel)
{
	CIndieLib::Instance()->Entity2dManager->Add(layer, &entity);
	entity.SetAnimation(pAnim);
	entity.SetHotSpot(0.5f, 0.5f);
	entity.SetPosition(pos.x, pos.y, 0);

	vecVel = vel;
	vecPos = pos;
	bIsDead = false;
	bIsDying = false;
	health = 1.0f;
	bBoom = false;


	// Create the explosion
	if(0)
	{
	pExplosion = new Explosion("\\IndieLib-TankEnemiesParallax\\images\\explosion.xml", layer, D3DXFROMWINEVECTOR3(0,0,0), D3DXFROMWINEVECTOR3(0,0,0));
	pExplosion->entity.SetShow(false);
	}
	else
		pExplosion=0;
	m_layer=layer;
}

PropFighter::~PropFighter( )
{
	#ifndef OMITDELETE
	CIndieLib::Instance()->Entity2dManager->Delete(&entity);
#endif
	if(pExplosion)
	{
		delete pExplosion;
		pExplosion=0;
	}

}

void PropFighter::Update( float fElapsedTime )
{
	 // Move the PropFighter	
	vecPos += vecVel * fElapsedTime;
	entity.SetPosition(vecPos.x, vecPos.y, 0);

	if (bIsDying && !bIsDead)
	{
		entity.SetShow(false);
		
if(!pExplosion)
	{
	pExplosion = new Explosion("\\IndieLib-TankEnemiesParallax\\images\\explosion.xml", m_layer, D3DXFROMWINEVECTOR3(0,0,0), D3DXFROMWINEVECTOR3(0,0,0));
	//pExplosion->entity.SetShow(false);
	}
		pExplosion->entity.SetShow(true);
		pExplosion->vecPos = vecPos;
		pExplosion->vecVel = vecVel;
		pExplosion->Update( fElapsedTime );

		if (pExplosion->bIsDead)
		{
			bIsDead = true;
		}
	}

	if (bBoom)
	{
		bBoom = false;
		 STX_Service::GetAudioInstance()->Play("bigexplode");	// maybe this should be included in the explosion.  Pass sound name as ctor parm.
	}

}
