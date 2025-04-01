/*
	Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#ifndef CH19P1_PARTICLESYSTEM_H_INCLUDED
#define CH19P1_PARTICLESYSTEM_H_INCLUDED

#pragma warning(disable:4786)
#include <map>
#include <string>

#include "CommonFuncs.h"
#include "D3DHelperFuncs.h"
#include "Ch19p1_Particle.h"
#include "Ch19p1_ParticleEventSequence.h"
#include "Ch19p1_ParticleEmitterTokenizer.h"

class CParticleEmitter
{
public:
  std::string m_strLastError;

  CParticleEmitter();
  virtual ~CParticleEmitter();

  virtual void Update(float fElapsedTime, float fTimeDelta);

  virtual int Render();

  virtual int RestoreDeviceObjects( pDev);

  virtual void InvalidateDeviceObjects();

  virtual bool Compile(const char *script);
  virtual void Init();

	virtual void Start() { m_bIsRunning = true; }
	virtual void Pause() { m_bIsRunning = false; }
	virtual void Stop()  { Pause(); DeleteAllParticles(); }
	virtual bool IsRunning() { return(m_bIsRunning); }
	
	void DeleteAllParticles(void) {
		for (std::vector<CParticleEventSequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) {
			(*i)->DeleteAllParticles();
		}
	}
  // attributes
  
  CMinMax<D3DXFROMWINEVECTOR3> GetPosRange(void) const { return(m_vPosRange); }
  void SetPosRange(const CMinMax<D3DXFROMWINEVECTOR3> &data) { m_vPosRange = data; }

  D3DXFROMWINEVECTOR3 GetPos(void) const { return(m_vPos); }
  void SetPos(const D3DXFROMWINEVECTOR3 &data) { m_vPos = data; }
  
  int GetVBSize(void) const { return(m_iVBSize); }
  void SetVBSize(const int data) { m_iVBSize = data; }

  void Reset(void) {
    for (std::vector<CParticleEventSequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) {
      (*i)->Reset();
    }
  }

  int GetNumActiveParticles(void) {
    int count=0;
    for (std::vector<CParticleEventSequence *>::iterator i = m_Sequences.begin(); i != m_Sequences.end(); i++) {
      count += (*i)->GetNumActiveParticles();
    }
    return(count);
  }

  static CParticleEvent *CParticleEmitter::EventFactory(std::string EventName);

private:
  
  bool ProcessParticleSystemBlock(
    std::vector<CParticleEmitterToken>::iterator &TokenIter,
    std::vector<CParticleEmitterToken>::iterator &EndIter);

  bool ProcessEventSequenceBlock(CParticleEventSequence &seq,
    std::vector<CParticleEmitterToken>::iterator &TokenIter,
    std::vector<CParticleEmitterToken>::iterator &EndIter);
  
  // particle system attributes
  CMinMax<D3DXFROMWINEVECTOR3> m_vPosRange; 
  D3DXFROMWINEVECTOR3 m_vPos; // actual position of emitter

  std::string m_strName;

  std::vector<CParticleEventSequence *> m_Sequences;

  int m_iMaxParticles;
	bool m_bIsRunning;

  int m_iVBSize; // size of the vertex buffer (can and should be less than NUMPARTICLES)
   m_pd3dDevice;
  VertexBufferID m_vbParticles;
};

#endif