#include "explosion.h"
#include <assert.h>

const float RADIUS_SCALE_FACTOR	= 0.65f;
//IND_Animation Explosion::AnimExplosion;
//unsigned int Explosion::m_nAnimExplosion=0;

Explosion::Explosion(const char* szXmlAnimationFilename, D3DXFROMWINEVECTOR3 pos, D3DXFROMWINEVECTOR3 vel)
{
	//if(m_nAnimExplosion==0)
	CIndieLib::Instance()->AnimationManager->AddToSurface (&AnimExplosion, szXmlAnimationFilename, IND_ALPHA, IND_32);
	//m_nAnimExplosion++;
	CIndieLib::Instance()->Entity2dManager->Add(&entity);
	entity.SetAnimation(&AnimExplosion);
	entity.SetHotSpot(0.5f, 0.5f);
	entity.SetPosition(pos.x, pos.y, 0);
	entity.SetSequence(0);
	numFrames = AnimExplosion.GetNumFrames(0);

	vecPos = pos;
	vecVel = vel;
	bIsDead = false;
}


Explosion::~Explosion( )
{
	//m_nAnimExplosion--;
	//if(!m_nAnimExplosion)
	CIndieLib::Instance()->AnimationManager->Delete(&AnimExplosion);
	#ifndef OMITDELETE
	CIndieLib::Instance()->Entity2dManager->Delete(&entity);
#endif
}

void Explosion::Update( float fElapsedTime )
{
	// Move the explosion	
	vecPos += vecVel * fElapsedTime;
	entity.SetPosition(vecPos.x, vecPos.y, 0);

	IND_Animation* pAnim = entity.GetAnimation();
	int pos = pAnim->GetActualFramePos(0);

	if ( pos >= numFrames - 1)
	{
		// todo - fade out
		
		// die
		bIsDead = true;
	}
}
