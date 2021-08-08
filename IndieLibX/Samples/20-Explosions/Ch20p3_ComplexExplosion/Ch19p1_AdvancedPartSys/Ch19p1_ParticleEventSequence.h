/*
	Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
	Licensed under The GNU Lesser General Public License, version 2.1:
		http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#ifndef CH19P1_PARTICLEEVENTSEQUENCE_H_INCLUDED
#define CH19P1_PARTICLEEVENTSEQUENCE_H_INCLUDED

#include "Ch19p1_Particle.h"
#include "RecyclingArrayDyn.h"
#include "Ch19p1_ParticleEvent.h"
#include <vector>

class CParticleEventSequence
{
public:

  int RestoreDeviceObjects( pDev);
  int InvalidateDeviceObjects();

  void Reset(void);
  void Update(float fElapsedTime, float fTimeDelta, D3DXFROMWINEVECTOR3 m_vPartSysPos);
  void Render(VertexBufferID pVB, int iVBSize);

  void CreateNewParticle(D3DXFROMWINEVECTOR3 m_vPartSysPos);
  void CreateFadeLists();

  void SortEvents(void);
  void NailDownRandomTimes(void);

  CParticleEventSequence();
  virtual ~CParticleEventSequence();

  CMinMax<float> GetLifetime(void) const { return(m_Lifetime); }
  void SetLifetime(const CMinMax<float> data) { m_Lifetime = data; }

  int GetSrcBlendMode(void) const { return(m_iSrcBlendMode); }
  void SetSrcBlendMode(const int data) { m_iSrcBlendMode = data; }

  int GetDestBlendMode(void) const { return(m_iDestBlendMode); }
  void SetDestBlendMode(const int data) { m_iDestBlendMode = data; }

  CMinMax<float> GetEmitRate(void) const { return(m_EmitRate); }
  void SetEmitRate(const CMinMax<float> data) { m_EmitRate = data; }

  CMinMax<D3DXFROMWINEVECTOR3> GetEmitRadius(void) const { return(m_vEmitRadius); }
  void SetEmitRadius(const CMinMax<D3DXFROMWINEVECTOR3> &data) { m_vEmitRadius = data; }

  CMinMax<D3DXFROMWINEVECTOR3> GetGravity(void) const { return(m_vGravity); }
  void SetGravity(const CMinMax<D3DXFROMWINEVECTOR3> &data) { m_vGravity = data; }

  std::string GetName(void) const { return(m_strName); }
  void SetName(const std::string &data) { m_strName = data; }
  
  int GetNumActiveParticles(void) { return(m_Particles ? m_Particles->GetNumUsedElements() : 0); }

  int GetNumParticles(void) const { return(m_iNumParticles); }
  void SetNumParticles(const int data) { m_iNumParticles = data; }

	int GetLoops(void) { return(m_Loops); }
	void SetLoops(const int data) { m_Loops = data; }

  CMinMax<D3DXFROMWINEVECTOR3> GetSpawnDir(void) const { return(m_vSpawnDir); }
  void SetSpawnDir(const CMinMax<D3DXFROMWINEVECTOR3> &data) { m_vSpawnDir = data; }

  std::string GetTextureFilename(void) const { return(m_strTexFilename); }
  TextureID GetTexture(void) const { return(m_texParticle); }
  void SetTexture(const char *strTexFilename);

  std::vector<CParticleEvent *> m_Events;

  void RunEvents(CParticle &part);
	void DeleteAllParticles(void) { m_Particles->DeleteAll(); m_iTotalParticleLives = 0; }

private:
  CRecyclingArrayDyn<CParticle> *m_Particles;

  CMinMax<float> m_Lifetime;
  CMinMax<float> m_EmitRate; // in particles/sec
  int m_Loops;
  CMinMax<D3DXFROMWINEVECTOR3> m_vSpawnDir;
  CMinMax<D3DXFROMWINEVECTOR3> m_vEmitRadius;
  CMinMax<D3DXFROMWINEVECTOR3> m_vGravity;

  std::string m_strTexFilename;
  TextureID m_texParticle;  
   m_pd3dDevice;
  int m_iNumParticles;
  int m_iDestBlendMode;
  int m_iSrcBlendMode;
	
	int m_iTotalParticleLives;
  
  std::string m_strName;
  
  float m_fNumNewPartsExcess;
};

#endif