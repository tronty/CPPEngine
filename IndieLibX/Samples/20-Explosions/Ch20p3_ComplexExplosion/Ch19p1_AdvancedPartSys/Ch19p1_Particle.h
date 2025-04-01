/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <d3dx8.h>
#include <vector>

#ifndef CH18P3_PARTICLE_H_DEFINED
#define CH18P3_PARTICLE_H_DEFINED

#include "D3DHelperFuncs.h"

typedef struct 
{
    D3DXFROMWINEVECTOR3 position;
    float       pointsize;
    __D3DCOLOR__    color;
} VERTEX_PARTICLE;

#define D3DFVF_PARTICLE (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_PSIZE)

class CParticleEvent;

class CParticle
{
public:
  CParticle() { 
    m_fWeight = 1.0f; m_fSize = 1.0f; m_fSizeStep = 0.0f;
    m_fLifetime = 1.0f; m_fAge = 0.0f; m_fEventTimer = 0.0f; 
    m_Color = D3DXFROMWINECOLOR(1.0f,1.0f,1.0f,1.0f);
    m_ColorStep = D3DXFROMWINECOLOR(0.0f,0.0f,0.0f,0.0f);
    m_vDir = D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
    m_vDirStep = D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
    m_vPos = D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
  }
  virtual ~CParticle() { }

  std::vector<CParticleEvent *>::iterator m_CurEvent;

  float m_fWeight;
  
  float m_fSize;
  float m_fSizeStep;

  float m_fLifetime;
  float m_fAge;
  
  float m_fEventTimer; // different from age
  
  D3DXFROMWINECOLOR m_Color;
  D3DXFROMWINECOLOR m_ColorStep;
  
  D3DXFROMWINEVECTOR3 m_vPos; // current position of particle
  
  D3DXFROMWINEVECTOR3 m_vDir;
  D3DXFROMWINEVECTOR3 m_vDirStep;

  // this function is inline not because it's small, but because it's only called
  // in one place (CParticleEmitter.Update()).  This way we can also dodge the
  // performance hit associated with calling a function many times (remember, we're
  // in a for loop in CParticleEmitter.Update()).

  // your needs may vary.  For example, if you can afford the speed hit, it might
  // be cool to make this a virtual function.
  inline bool Update(float fTimeDelta)
  {
    // age the particle
		m_fAge += fTimeDelta;
    m_fEventTimer += fTimeDelta;

    // if this particle's age is greater than it's lifetime, it dies.
    if (m_fAge >= m_fLifetime) {
      return(false); // dead!
    }

		// move particle
		m_vPos += m_vDir * fTimeDelta;

    m_Color += m_ColorStep * fTimeDelta;
    m_vDir += m_vDirStep * fTimeDelta;
    m_fSize += m_fSizeStep * fTimeDelta;

    return(true); // particle stays alive
  }
};

#endif