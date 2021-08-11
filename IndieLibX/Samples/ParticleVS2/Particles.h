#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include <Framework3/IRenderer.h>

// Particle emitter types
enum {
	EMITTER_CLOUD,
	EMITTER_TREE,
	EMITTER_PEOPLE,
	EMITTER_FIRE,
	EMITTER_EXPLOSION
};

// Particle types and # of particle types
enum {
	PARTICLE_SMOKE,
	PARTICLE_TREE1,
	PARTICLE_TREE2,
	PARTICLE_TREE3,
	PARTICLE_PEOPLE1,
	PARTICLE_PEOPLE2,
	PARTICLE_FIRE,
	PARTICLE_EXPLOSION1,
	PARTICLE_EXPLOSION2,
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
    __DWORD__       m_Type;         // Type of particle

    D3DXFROMWINEVECTOR3 m_vecPos;       // Position of particle
    D3DXFROMWINEVECTOR3 m_vecVelocity;  // Velocity of particle

    __DWORD__       m_Life;         // Life of particle in ms
    float       m_Size;         // Size of particle

    //D3DXFROMWINEVECTOR4       m_Color;        // Diffuse color of particle

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
    sShaderVertex *Ptr;
    sShaderVertex *Ptr0;

    __DWORD__                   m_EmitterType;   // Type of emitter

    std::vector<sShaderVertex> m_VB;            // Particles' vertex buffer
    std::vector<__WORD__>  m_IB;            // Particles' index buffer

    D3DXFROMWINEVECTOR3             m_vecPosition;   // Position of emitter
    __DWORD__                   m_NumParticles;  // Max # particles in buffer
    cParticle              *m_Particles;     // Particle list

    static __DWORD__            m_RefCount;      // Class reference count

    static ShaderID m_pShader;    // Vertex shader
    static VertexFormatID m_pDecl; // Vertex decl
    TextureID m_pTextures[9];      // Textures
    //static int alphaState;

  public:
    cParticleEmitter();
    ~cParticleEmitter();

    bool Create(
                D3DXFROMWINEVECTOR3 *vecPosition,
                __DWORD__ EmitterType,
                __DWORD__ NumParticlesPerBuffer = 32);
    void Free();

    void Add(__DWORD__ Type, D3DXFROMWINEVECTOR3 *vecPos, float Size,
             D3DXFROMWINEVECTOR4 Color, __DWORD__ Life,
             D3DXFROMWINEVECTOR3 *vecVelocity);
    void ClearAll();
    void Process(__DWORD__ Elapsed);

    // Functions to prepare for particle rendering, wrap up rendering,
    // and to render a batch of particles.
    bool Begin(D3DXFROMWINEMATRIX &amatView, D3DXFROMWINEMATRIX &amatProj);
    void Render(D3DXFROMWINEMATRIX &amatView, D3DXFROMWINEMATRIX &amatProj);

    // Specialized functions for various particle types
    void HandleSmoke(D3DXFROMWINEVECTOR3 *vecPos, __DWORD__ Elapsed);
    void HandleFire(D3DXFROMWINEVECTOR3 *vecPos, __DWORD__ Elapsed);
    void HandleExplosion(D3DXFROMWINEVECTOR3 *vecPos, __DWORD__ Elapsed);
    void HandleDucking(D3DXFROMWINEVECTOR3 *vecPos);
};

#endif
