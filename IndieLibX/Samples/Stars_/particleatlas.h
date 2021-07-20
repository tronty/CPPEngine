/*
  Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#ifndef _PARTICLEATLAS_H_
#define _PARTICLEATLAS_H_

#include <Framework3/IRenderer.h>

#if 1
#define Vec3TransformCoord D3DXFROMWINEVec3TransformCoord
#define Vec3ProjectArray D3DXFROMWINEVec3ProjectArray
#define Vec3Project D3DXFROMWINEVec3Project
#else
#define Vec3TransformCoord
#define Vec3ProjectArray
#define Vec3Project
#endif

/*
#include "../Terrain.h"
#include "Framework/Direct3D/D3D9Renderer.h"

#include <CommonD3DGL/Image3.h>

#include "Framework/Util/KDTree.h"
//include "VertexCache.h"
*/
#include <libnova/solar.h>
#include <libnova/lunar.h>

#include <libnova/mercury.h>
#include <libnova/venus.h>
#include <libnova/earth.h>
#include <libnova/mars.h>
#include <libnova/jupiter.h>
#include <libnova/saturn.h>
#include <libnova/uranus.h>
#include <libnova/neptune.h>
#include <libnova/pluto.h>

#include <libnova/julian_day.h>
#include <libnova/rise_set.h>
#include <libnova/transform.h>
#include <libnova/utility.h>

const int NSTARS=9111;

//define ASM 1
//define SPHERICAL 1
// Particle emitter types
enum EMITTERS
{
	EMITTER_CLOUD,
	EMITTER_VEGETATION,
	EMITTER_PEOPLE,
	EMITTER_SKY,
	EMITTER_ARBARO,
};
// Particle types and # of particle types
enum PARTICLES
{
	PARTICLE_TREE1,
	PARTICLE_TREE2,
	PARTICLE_TREE3,
	PARTICLE_TREE4,
	PARTICLE_TREE5,
	PARTICLE_TREE6,
	PARTICLE_TREE7,
	PARTICLE_TREE8,
	PARTICLE_TREE9,

	PARTICLE_EARTH,
	PARTICLE_SUN,
	PARTICLE_MOON,
	PARTICLE_STAR,
	PARTICLE_MERCURY,
	PARTICLE_VENUS,
	PARTICLE_MARS,
	PARTICLE_JUPITER,
	PARTICLE_SATURN,
	PARTICLE_URANUS,
	PARTICLE_NEPTUNE,
	PARTICLE_PLUTO,

	PARTICLE_ARBARO1,
	PARTICLE_ARBARO2,
	PARTICLE_ARBARO3,
	PARTICLE_ARBARO4,
	PARTICLE_ARBARO5,
	PARTICLE_ARBARO6,
	PARTICLE_ARBARO7,
	PARTICLE_ARBARO8,
	PARTICLE_ARBARO9,
	PARTICLE_ARBARO10,
	PARTICLE_ARBARO11,
	PARTICLE_ARBARO12,
	PARTICLE_ARBARO13,
	PARTICLE_ARBARO14,
	PARTICLE_ARBARO15,
	PARTICLE_ARBARO16,
	//PARTICLE_ARBARO17,
	//PARTICLE_ARBARO18,

	PARTICLE_SMOKE,

	PARTICLE_PEOPLE1,
	PARTICLE_PEOPLE2,
	NUM_PARTICLE_TYPES
};

// Vertex structure for vertex-shader particles
#if ASM
typedef struct {
  D3DXFROMWINEVECTOR3 vecPos;	  // Coordinates of particle
  D3DXFROMWINEVECTOR2 vecOffset;  // Corner vertex offset coordinates
  D3DXFROMWINEVECTOR4       Diffuse;	  // Diffuse color of particle
  D3DXFROMWINEVECTOR2       uv;	  // Texture coordinates
} sShaderVertex;
#else
typedef struct {
  D3DXFROMWINEVECTOR3 vecPos;	  // Coordinates of particle
  D3DXFROMWINEVECTOR4       Diffuse;	  // Diffuse color of particle
  D3DXFROMWINEVECTOR2       uv;	  // Texture coordinates
  D3DXFROMWINEVECTOR2 vecOffset;  // Corner vertex offset coordinates
  D3DXFROMWINEVECTOR4 allowedRotDir;
} sShaderVertex;
#endif
// Particle container class
class cParticle
{
  public:
    __WORD__	m_Type; 	// Type of particle
    D3DXFROMWINEVECTOR3 m_vecPos;	// Position of particle
	D3DXFROMWINEVECTOR3 m_vecVelocity;	// Velocity of particle

    __WORD__	m_Life; 	// Life of particle in ms
    float	m_Size; 	// Size of particle

    __WORD__	m_Color;	// Diffuse color of particle
    D3DXFROMWINEVECTOR4 m_allowedRotDir;

    char	m_description[256];

    cParticle *m_Prev;		// Prev particle in linked list
    cParticle *m_Next;		// Next particle in linked list

  public:
    // Constructor and destructor to clear/release data
    cParticle()  { m_Prev = 0; m_Next = 0;		}
    ~cParticle() { delete m_Next; m_Next = 0; m_Prev = 0; }
};

// Particle emitter class
class cParticleEmitter
{
  public:
struct CatalogElement
{
	char name[11];
	char varname[11];
//	char remark[128];
	float ra,dec,mag;
};
    sShaderVertex *Ptr;
    sShaderVertex *Ptr0;
	__WORD__ *Ptri;
	  __WORD__ start;
    Image3 atlas0;

    __WORD__		    m_EmitterType;   // Type of emitter

    D3DXFROMWINEVECTOR3	    m_vecPosition;   // Position of emitter
    __WORD__		    m_NumParticles;  // Max # particles in buffer
    cParticle		   *m_Particles;     // Particle list

    static __WORD__	    m_RefCount;      // Class reference count

    std::vector<sShaderVertex> m_VB;            // Particles' vertex buffer
    std::vector<__WORD__>  m_IB;            // Particles' index buffer
     static ShaderID	   m_pEffect;
    static ShaderID m_pShader;	 // Vertex shader
    static VertexFormatID m_pDecl; // Vertex decl
    TextureID m_pAtlasTexture;

    static bool initVegetation;
    static bool initVegetation2;
	static bool initSky;
#ifdef __VERTEXCACHE__
	static CVertexCache* m_vertexCache;
	sShaderVertex *pVertices;
	__WORD__* pIndices;
#endif
  public:
	void RenderStars();
    bool Create(
		D3DXFROMWINEVECTOR3 vecPosition=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f),
		__WORD__ EmitterType=EMITTER_SKY,
		__WORD__ NumParticlesPerBuffer=9107);
    void Free();

    void Add(__WORD__ Type, D3DXFROMWINEVECTOR3& vecPos, float Size,
	     __WORD__ Color=0, __WORD__ Life=0,
	     D3DXFROMWINEVECTOR3 vecVelocity=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f), D3DXFROMWINEVECTOR4 allowedRotDir=D3DXFROMWINEVECTOR4(0.0f,0.0f,0.0f,0.0f),char* decription=0);
    void ClearAll();

    // Functions to prepare for particle rendering, wrap up rendering,
    // and to render a batch of particles.
    bool Begin();//(D3DXFROMWINEMATRIX *matView, D3DXFROMWINEMATRIX *matProj,D3DXFROMWINEVECTOR4& xCamPos=D3DXFROMWINEVECTOR4(0.0f,0.0f,0.0f,1.0f));
    void End();
    void Render();

 D3DXFROMWINEVECTOR3 solarsystempositions[10];
D3DXFROMWINEVECTOR3 screenpositions[10];

//	CTerrain* m_pTerrain;
	D3DXFROMWINEVECTOR3 CENTER;
	D3DXFROMWINEVECTOR2 SIZE;
	int SEED;
	int atlasTextureCount;
	int iatlassize;//=1024;//768
	int isubtexturesize;//=256;
	int AddTextureToAtlas(Image3& atlas,Image3& img,float gamma = 1.0f);
	void MakeAtlas(Image3& atlas);
	void init();
#if 0
	cParticleEmitter(CTerrain* pTerrain,int numParticles=1000,D3DXFROMWINEVECTOR3 center=vO,D3DXFROMWINEVECTOR2 size=D3DXFROMWINEVECTOR2(1.0f,1.0f),int seed=0)
	{
		m_pTerrain=pTerrain;
		m_NumParticles = numParticles;
		CENTER=center;
		SIZE=size;
		SEED=seed;
		stx_srand(seed);


  // Clear object pointers
  m_VB		 = -1;
  m_IB		 = -1;

  // Clear particle data
  m_Particles	 = 0;
  if(numParticles>0)
  {
		AddVegetation(m_pTerrain,numParticles,center,size);
		//AddArbaro(m_pTerrain,numParticles,center,size);
		init("");
  }
	}
#endif
	cParticleEmitter(int seed=0)
	{
		m_NumParticles = 9107;
    		Ptr=&m_VB[0];
    		Ptr0=&m_VB[0];
		Ptri=&m_IB[0];
		CENTER=D3DXFROMWINEVECTOR3(0.0f,0.0f,0.0f);
		SIZE=D3DXFROMWINEVECTOR2(1.0f,1.0f);
		SEED=seed;
		stx_srand(seed);


  // Clear object pointers
  m_VB.clear();
  m_IB.clear();

  // Clear particle data
  m_Particles	 = 0;
  if(m_NumParticles>0)
  {
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	  AddSky();
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
		init();
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
  }
	}
	virtual ~cParticleEmitter()
	{
	  Free();
	  }
	__WORD__ m_CountPolys;
	__WORD__ CountPolys()
	{
	      return m_CountPolys;
	}
	void applyGamma( unsigned char *buffer, size_t size, unsigned char bpp, float gamma = 1.0f );
	void applyGamma( Image3& image, float gamma = 1.0f );
	void AddSky()
	{
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
		D3DXFROMWINEVECTOR3 vO(0.0f,0.0f,0.0f);
		Create(	vO, EMITTER_SKY);
	
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
		ReadCatalog();
	
	//LOG_PRINT("%s:%d\n",__FUNCTION__,__LINE__);
	//LOG_PRINT("m_pEffect=%x\n",m_pEffect);
	}
#if 0
	void AddVegetation(CTerrain* pTerrain,int numVegetation,D3DXFROMWINEVECTOR3 center=vO,D3DXFROMWINEVECTOR2 size=D3DXFROMWINEVECTOR2(1.0f,1.0f),int seed=0)
	{
		Create(	vO, EMITTER_VEGETATION,numVegetation);
	/*
		Distribute foliage over the terrain. To make the distribution fairly even a KD-tree is used
		to check whether an incoming location is too close to any existing location. This way all
		pieces of foliage are guaranteed to be at least maxDist distance apart.
	*/
	KDTree <float, 2> tree;
	float maxDist = 1.5f / sqrtf(float(numVegetation));

	stx_srand(seed);
	__WORD__ Type = PARTICLE_TREE1;
	for (int i = 0; i < numVegetation; i++){
		float tx, tz;

		do {
			tx = float(stx_rand()) / RAND_MAX * 2 - 1;
			tz = float(stx_rand()) / RAND_MAX * 2 - 1;
		} while (tree.hasPointWithin(float2(tx, tz), maxDist));
		tree.add(float2(tx, tz));

		D3DXFROMWINEVECTOR3 pos=center+D3DXFROMWINEVECTOR3(tx*size.x,pTerrain->m_map[0].GetExactHeightAt(center.x+tx*size.x,center.z+tz*size.y),tz*size.y);
		/*
		//LOG_PRINT("Particle i=%d\n",i);
		//LOG_PRINT("pos.x=%f\n",pos.x);
		//LOG_PRINT("pos.y=%f\n",pos.y);
		//LOG_PRINT("pos.z=%f\n",pos.z);
		*/
			    if(Type<PARTICLE_TREE5)
					  Add(Type++,
							&(pos+D3DXFROMWINEVECTOR3(0.0f,25.0f,0.0f)),
							50.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
			   else
					 Add(Type++,
			&(pos+D3DXFROMWINEVECTOR3(0.0f,12.5f,0.0f)),
				    25.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
	      if(Type > PARTICLE_TREE9)
						       Type = PARTICLE_TREE1;
	}
	}
	void AddArbaro(CTerrain* pTerrain,int numVegetation,D3DXFROMWINEVECTOR3 center=vO,D3DXFROMWINEVECTOR2 size=D3DXFROMWINEVECTOR2(1.0f,1.0f),int seed=0)
	{
		Create(	vO, EMITTER_ARBARO,numVegetation);
	/*
		Distribute foliage over the terrain. To make the distribution fairly even a KD-tree is used
		to check whether an incoming location is too close to any existing location. This way all
		pieces of foliage are guaranteed to be at least maxDist distance apart.
	*/
	KDTree <float, 2> tree;
	float maxDist = 1.5f / sqrtf(float(numVegetation));

	stx_srand(seed);
	__WORD__ Type = PARTICLE_TREE1;
	for (int i = 0; i < numVegetation; i++){
		float tx, tz;

		do {
			tx = float(stx_rand()) / RAND_MAX * 2 - 1;
			tz = float(stx_rand()) / RAND_MAX * 2 - 1;
		} while (tree.hasPointWithin(float2(tx, tz), maxDist));
		tree.add(float2(tx, tz));

		D3DXFROMWINEVECTOR3 pos=center+D3DXFROMWINEVECTOR3(tx*size.x,pTerrain->m_map[0].GetExactHeightAt(center.x+tx*size.x,center.z+tz*size.y),tz*size.y);
		/*
		//LOG_PRINT("Particle i=%d\n",i);
		//LOG_PRINT("pos.x=%f\n",pos.x);
		//LOG_PRINT("pos.y=%f\n",pos.y);
		//LOG_PRINT("pos.z=%f\n",pos.z);
		*/
			    if(Type<PARTICLE_ARBARO1)
					  Add(Type++,
							&(pos+D3DXFROMWINEVECTOR3(0.0f,25.0f,0.0f)),
							50.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
			   else
					 Add(Type++,
			&(pos+D3DXFROMWINEVECTOR3(0.0f,12.5f,0.0f)),
				    25.0f, 0xFFFFFFFF, 0, &vO,D3DXFROMWINEVECTOR4(0.0f, 1.0f, 0.0f,0.0f));
	      if(Type > PARTICLE_ARBARO2)
						       Type = PARTICLE_ARBARO1;
	}
	}
#endif
	void render();
	void print_date (char * title, struct ln_zonedate* date);
	void EqutoHoriz(char* name,double JD,struct ln_equ_posn object, struct ln_lnlat_posn observer);
	int sun (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz);
	int moon (double JD,struct ln_lnlat_posn observer,struct ln_equ_posn equ,struct ln_hrz_posn hrz);
	int planet (double JD,struct ln_lnlat_posn observer,PARTICLES body,struct ln_equ_posn equ,struct ln_hrz_posn hrz,double mag);
	//int main (int argc, char * argv[]);
	void ReadCatalog();
	D3DXFROMWINEVECTOR3 CalculatePosition(double aequra, double aequdec,double JD=0);
	int Pick();
	bool LineSphereIntersect(const D3DXFROMWINEVECTOR3 &p, const D3DXFROMWINEVECTOR3 &d,double r, double &i1, double &i2);
	bool LineSphereIntersectNN(const D3DXFROMWINEVECTOR3 &p, const D3DXFROMWINEVECTOR3 &d,double r, double &i1, double &i2);
	int initEffect();
	void ReadConstellationLines(const char* fn);
};
extern 
void cParticleEmitter__DrawStars();
#endif

