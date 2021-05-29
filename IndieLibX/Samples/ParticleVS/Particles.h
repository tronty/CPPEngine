/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <Framework3/IRenderer.h>

// Particle emitter types
enum {
	EMITTER_CLOUD,
	EMITTER_TREE,
	EMITTER_PEOPLE,
};

// Particle types and # of particle types
enum {
	PARTICLE_SMOKE,
	PARTICLE_TREE1,
	PARTICLE_TREE2,
	PARTICLE_TREE3,
	PARTICLE_TREE4,
	PARTICLE_TREE5,
	PARTICLE_TREE6,
	PARTICLE_PEOPLE1,
	PARTICLE_PEOPLE2,
	NUM_PARTICLE_TYPES
};

// Vertex structure for vertex-shader particles
typedef struct {
  D3DXFROMWINEVECTOR3 vecPos;     // Coordinates of particle
  D3DXFROMWINEVECTOR2 vecOffset;  // Corner vertex offset coordinates
  //D3DXFROMWINEVECTOR4       Diffuse;    // Diffuse color of particle
  D3DXFROMWINEVECTOR2 uv;       // Texture coordinates
} sShaderVertex;


// Particle container class
class cParticle
{
  public:
    __WORD__       m_Type;         // Type of particle

    D3DXFROMWINEVECTOR3 m_vecPos;       // Position of particle
    D3DXFROMWINEVECTOR3 m_vecVelocity;  // Velocity of particle

    __WORD__       m_Life;         // Life of particle in ms
    float       m_Size;         // Size of particle

    __WORD__       m_Color;        // Diffuse color of particle

    cParticle *m_Prev;          // Prev particle in linked list
    cParticle *m_Next;          // Next particle in linked list

  public:
    // Constructor and destructor to clear/release data
    cParticle()  { m_Prev = 0; m_Next = 0;                }
    ~cParticle() { delete m_Next; m_Next = 0; m_Prev = 0; }
};

// Particle emitter class
class cParticleEmitter
{
  protected:
    TextureID m_pTextures[NUM_PARTICLE_TYPES];
    std::vector<sShaderVertex> VB;
    std::vector<__WORD__> IB;
    __WORD__                   m_EmitterType;   // Type of emitter

    D3DXFROMWINEVECTOR3             m_vecPosition;   // Position of emitter
    __WORD__                   m_NumParticles;  // Max # particles in buffer
    cParticle              *m_Particles;     // Particle list

    static ShaderID m_pShader;    // Vertex shader
    static VertexFormatID m_pDecl; // Vertex decl

	D3DXFROMWINEMATRIX matVP;
	D3DXFROMWINEVECTOR4 vecRight, vecUp;

  public:
    cParticleEmitter();
    ~cParticleEmitter();

    bool Create(D3DXFROMWINEVECTOR3 *vecPosition,
                __WORD__ EmitterType,                              
                __WORD__ NumParticlesPerBuffer = 32);
    void Free();

    void Add(__WORD__ Type, D3DXFROMWINEVECTOR3 *vecPos, float Size, 
             __WORD__ Color, __WORD__ Life, 
             D3DXFROMWINEVECTOR3 *vecVelocity);
    void ClearAll();
    void Process(__WORD__ Elapsed);

    // Functions to prepare for particle rendering, wrap up rendering,
    // and to render a batch of particles.
    bool Begin(D3DXFROMWINEMATRIX& matView, D3DXFROMWINEMATRIX& matProj);
    void Render(D3DXFROMWINEMATRIX& matView, D3DXFROMWINEMATRIX& matProj);

    // Specialized functions for various particle types
    void HandleSmoke(D3DXFROMWINEVECTOR3 *vecPos, __WORD__ Elapsed);
    void HandleDucking(D3DXFROMWINEVECTOR3 *vecPos);
};

#endif

